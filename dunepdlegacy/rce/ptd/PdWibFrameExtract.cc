// -*-Mode: C++;-*-

/* ---------------------------------------------------------------------- *//*!
 *
 *  @file     PdWibFrameExtract.cc
 *  @brief    Extracts the WIB frames from a data file
 *  @verbatim
 *                               Copyright 2018
 *                                    by
 *
 *                       The Board of Trustees of the
 *                    Leland Stanford Junior University.
 *                           All rights reserved.
 *
 *  @endverbatim
 *
\* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *\
   
   HISTORY
   -------
  
   DATE       WHO WHAT
   ---------- --- ---------------------------------------------------------
   2018.08.30 jjr Added check for TpcEmpty data fragments.
   2017.08.14 jjr Created
  
\* ---------------------------------------------------------------------- */


#include "Reader.hh"
#include "dunepdlegacy/rce/dam/HeaderFragmentUnpack.hh"
#include "dunepdlegacy/rce/dam/DataFragmentUnpack.hh"
#include "dunepdlegacy/rce/dam/TpcFragmentUnpack.hh"
#include "dunepdlegacy/rce/dam/TpcStreamUnpack.hh"
#include "dunepdlegacy/rce/dam/access/WibFrame.hh"


#include <cstdlib>
#include <cstring>
#include <cinttypes>
#include <cstdio>


static void print_summary   (TpcStreamUnpack  const *tpcStream,
                             int                      nwritten,
                             int                         total)
            __attribute__ ((unused));

/* ---------------------------------------------------------------------- *//*!

  \class  Prms
  \brief  The configuration parameters
                                                                          */
/* ---------------------------------------------------------------------- */
class Prms
{
public:
   Prms (int argc, char *const argv[]);

public:
   int            m_npackets; /*!< Number of 1024 packets to extract      */
   int            m_ifilecnt; /*!< Input  file name count                 */ 
   char *const *m_ifilenames; /*!< Input  file name                       */
   char const   *m_ofilename; /*!< Output file names                      */
   enum Reader::FileType 
               m_ifiletype;   /*!< The input file type                    */
};
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Constructor to extract the readers command line parameters

  \param[in] argc The count  of the command line parameters
  \param[in] argv The vector of the command line parameters
                                                                          */
/* ---------------------------------------------------------------------- */
Prms::Prms (int argc, char *const argv[]) :
   m_npackets   ((unsigned)(0x40000000)/1024),
   m_ifilecnt   (0),
   m_ifilenames (NULL),
   m_ofilename  ("/dev/null"),
   m_ifiletype  (Reader::FileType::Binary)
{
   char c;
   while ( (c = getopt (argc, argv, "n:o:bg")) != -1)
   {
      switch (c)
      {
      case 'b': { m_ifiletype = Reader::FileType::Binary;    break; }
      case 'g': { m_ifiletype = Reader::FileType::TextGdb64; break; }
      case 'n': { m_npackets  = strtoul (optarg, NULL, 0);   break; }
      case 'o': { m_ofilename = optarg;                      break; }
      }
   }  

   if (optind < argc)
   {
      m_ifilenames = &argv[optind];
      m_ifilecnt   = argc - optind;
   }
   else
   {
      fprintf (stderr, "Error: No input file provided\n");
      exit (-1);
   }

   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \class WibFrameExtracter
  \brief Extracts the wib frames from a TPC stream and writes them to an
         output file
                                                                          */
/* ---------------------------------------------------------------------- */
class WibFrameExtracter 
{
public:
   static size_t const MaxBuf = 10 * 1024 * 1024;

public:
   WibFrameExtracter (char const      *ofilename, 
                      int              npackets);

public:
   ~WibFrameExtracter ();

public:
   int  open  (char const *ifilename, Reader::FileType ifiletype);
   int  read  ();
   int  close ();

   bool write ();
   bool writeFragment  ();
   bool writeTpcStream (TpcStreamUnpack const *tpcStream);

public: 
   Reader *m_reader;
   int        m_ofd;  
   int    m_nframes;
   int      m_ntogo;
   uint64_t  *m_buf;
};
/* ---------------------------------------------------------------------- */


WibFrameExtracter::WibFrameExtracter (char const      *ofilename, 
                                      int               npackets) 
{
   m_ofd = ::creat (ofilename, S_IRUSR | S_IWUSR
                             | S_IRGRP | S_IWGRP
                             | S_IROTH);

   if (m_ofd < 0)
   {
      fprintf (stderr, "Error: can't open output file %s\n"
                       "       %s\n",
               ofilename,
               strerror (errno));
      exit (-1);
   }

   m_nframes = 1024 * npackets;
   m_ntogo   = m_nframes;
   m_buf     = reinterpret_cast<decltype (m_buf)>(malloc (MaxBuf));

   return;
}
/* ---------------------------------------------------------------------- */   




/* ---------------------------------------------------------------------- *//*!

  \brief WibFrameExtracter destructor
                                                                          */
/* ---------------------------------------------------------------------- */
WibFrameExtracter::~WibFrameExtracter ()
{
   delete m_reader;
   ::close (m_ofd);
   return;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */
int WibFrameExtracter::open (char const *ifilename, Reader::FileType ifiletype)
{
   m_reader = &ReaderCreate (ifilename, ifiletype);
   if (m_reader == 0) return -1;

   int err = m_reader->open ();
   return err;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief Close the input file reader
                                                                          */
/* ---------------------------------------------------------------------- */
int WibFrameExtracter::close ()
{
   delete m_reader;
   m_reader = 0;
   return 0;
}
/* ---------------------------------------------------------------------- */


/* ---------------------------------------------------------------------- *//*!

  \brief  Read the next event
  \retval == 0  Success
  \retval == 1  EOF
                                                                          */
/* ---------------------------------------------------------------------- */
int WibFrameExtracter::read ()
{

   HeaderFragmentUnpack *header = HeaderFragmentUnpack::assign (m_buf);
   ssize_t               nbytes = m_reader->read (header);

   if (nbytes <= 0)
   {
      if (nbytes == 0) 
      {
         // If hit eof, return 'done'
         m_reader->close ();
         return 1;
      }

      else
      {
         // Anything else is an error
         exit (-1);
      }
   }

   // ----------------------------------------------------
   // Get the number of 64-bit words in this data fragment
   // and read the body of the data fragment.
   // ----------------------------------------------------
   uint64_t  n64 = header->getN64 ();
   ssize_t nread = m_reader->read (m_buf, n64, nbytes);
   if (nread <= 0)
   {
      if (nread)
      {
         fprintf (stderr, "Error: Incomplete or corrupted record\n");
         exit (-1);
      }
   }

   return 0;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- */   
bool WibFrameExtracter::write ()
{
   bool   done = writeFragment ();
   return done;
}
/* ---------------------------------------------------------------------- */   



static int extract (WibFrameExtracter &extracter);



/* ---------------------------------------------------------------------- */
int main (int argc, char *const argv[])
{
   // -----------------------------------
   // Extract the command line parameters
   // -----------------------------------
   Prms     prms (argc, argv);
   WibFrameExtracter extracter (prms.m_ofilename,
                                prms.m_npackets);

   for (int idx = 0; idx < prms.m_ifilecnt; idx++)
   {
      printf ("Processing: %s\n", prms.m_ifilenames[idx]);
      int status = extracter.open (prms.m_ifilenames[idx],
                                   prms.m_ifiletype);
      if (status) break;

      bool done = extract (extracter);
      if (done) break;
   }

   
   extracter.~WibFrameExtracter ();

   return 0;
}
/* ---------------------------------------------------------------------- */


static int extract (WibFrameExtracter &extracter)
{
   while (1)
   {
      {
         int done = extracter.read  ();
         if (done) break;
      }

      {
         bool done = extracter.write ();
         if (done) break;
      }
   }

   extracter.close ();

   return 0;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *//*!

  \brief Process an RCE data fragment

  \param[in] buf The data fragment to process
                                                                          */
/* ---------------------------------------------------------------------- */
bool WibFrameExtracter::writeFragment ()
{
   uint64_t const *buf = m_buf;

   // -----------------------------------------------
   // Interpret this as a generic RCE Fragment Header
   // -----------------------------------------------
   HeaderFragmentUnpack const header (buf);


   // ----------------------------
   // Is this an RCE Data Fragment
   // ----------------------------
   if (header.isData ())
   {
      // --------------------------------------------
      // Turn the buffer into a generic data fragment
      // --------------------------------------------
      DataFragmentUnpack df (buf);
      //df.print ();


      // ----------------------------------------------
      // Is this record an error-free TPC data fragment
      // ---------------------------------------------
      if (df.isTpcNormal () || df.isTpcDamaged ())
      {
         /*
         char const *tpcType = df.isTpcNormal  () ? "TpcNormal"
                             : df.isTpcDamaged () ? "TpcDamaged"
                             : "TpcUnknown";
         
         printf ("Have TpcStream data type: %s\n", tpcType);
         */

         // ------------------------------------------------
         // Having verified that this is a TPC data fragment
         // can now convert it an analyze it as such
         // ------------------------------------------------
         TpcFragmentUnpack tpcFragment (df);

         ///df.printHeader  ();
         ///df.printTrailer ();

         // --------------------------------------------
         // Get the number and loop over the TPC streams
         // --------------------------------------------
         int nstreams = tpcFragment.getNStreams ();

         for (int istream = 0; istream < nstreams; ++istream)
         {
            TpcStreamUnpack const *tpcStream = tpcFragment.getStream (istream);
            bool                        done = writeTpcStream (tpcStream);
            if (done) { return true; }
         }
      }
      else
      {
         char const *tpcType = df.isTpcEmpty  () ? "TpcEmpty"
                             : "TpcUnknown";
         
         printf ("Have TpcStream data type: %s\n", tpcType);
         
      }
      //df.printTrailer ();
   }

   return false;
}
/* ---------------------------------------------------------------------- */





/* ---------------------------------------------------------------------- *\
 |                                                                        |
 |  The stuff below is not part of the high level user interface.  While  |
 |  it is public, these methods below are meant for lower level access.   |
 |  As such, they take a bit more expertise.                              |
 |                                                                        |
\* ---------------------------------------------------------------------- */
#include "dunepdlegacy/rce/dam/access/Headers.hh"
#include "dunepdlegacy/rce/dam/access/TpcStream.hh"
#include "dunepdlegacy/rce/dam/access/TpcRanges.hh"
#include "dunepdlegacy/rce/dam/access/TpcToc.hh"
#include "dunepdlegacy/rce/dam/access/TpcPacket.hh"


/* ---------------------------------------------------------------------- *//*!

  \brief Exercises the low level routines that are the backbone of the
         TpcStreamUnpack interface.

  \param[in]  tpcStream  The target TPC stream.
                                                                          */
/* ---------------------------------------------------------------------- */
bool WibFrameExtracter::writeTpcStream (TpcStreamUnpack const *tpcStream)
{
   using namespace pdd;
   using namespace pdd::access;

   int fd = m_ofd;

   ///print_summary (tpcStream);

   TpcStream const &stream = tpcStream->getStream     ();

   // -----------------------
   // Construct the accessors
   // -----------------------
   TpcToc           toc    (stream.getToc    ());
   TpcPacket        pktRec (stream.getPacket ());
   TpcPacketBody    pktBdy (pktRec.getRecord ());

   uint64_t const  *pkts  = pktBdy.getData    ();
   int              npkts = toc.getNPacketDscs ();


   for (int pktNum = 0; pktNum < npkts; ++pktNum)
   {

      TpcTocPacketDsc pktDsc (toc.getPacketDsc (pktNum));
      unsigned int    pktOff = pktDsc.getOffset64 ();
      uint64_t const    *ptr = pkts + pktOff;

      if (pktDsc.isWibFrame ())
      {
         unsigned nWibFrames = pktDsc.getNWibFrames ();

         m_ntogo -= nWibFrames;
         if (m_ntogo < 0) { putchar ('\n'); return false; }
         

         print_summary (tpcStream, m_nframes - m_ntogo,m_nframes);




         /*
           unsigned int   pktType = pktDsc.getType ();
           unsigned int    pktLen = pktDsc.getLen64 ();

         printf ("Packet[%2u:%1u(WibFrames ).%4d] = "
                 " %16.16" PRIx64 " %16.16" PRIx64 " %16.16" PRIx64 "\n",
                 pktNum, pktType, nWibFrames,
                 ptr[0], ptr[1], ptr[2]);
            */

         ::write (fd, ptr, nWibFrames * sizeof (WibFrame));

      }
      else if (pktDsc.isCompressed ())
      {
         fprintf (stderr, "Error: Can't handle compressed frames yet\n");
      }
   }

   putchar ('\n');
   return false;
}
/* ---------------------------------------------------------------------- */



/* ---------------------------------------------------------------------- *//*!

  \brief Prints a summary of the specified \a tpcStream

  \param[in] tpcStream The TPC stream
  \param[in]  nwritten Number of frames written so far
  \param[in]     total The total to write
                                                                          */
/* ---------------------------------------------------------------------- */
static void print_summary (TpcStreamUnpack const *tpcStream,
                           int                     nwritten,
                           int                        total)
{
   TpcStreamUnpack::Identifier id = tpcStream->getIdentifier ();
   
   printf ("Writing: WibId:%2d.%1d.%1d  %6d/%6d\r",
           id.getCrate (),
           id.getSlot  (),
           id.getFiber (),
           nwritten,
           total);

   return;
}
/* ---------------------------------------------------------------------- */
