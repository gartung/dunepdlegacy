///////////////////////////////////////////////////////////////////////////////////////////////////
// Class:       IcebergChannelMapService
// Module type: service
// File:        IcebergChannelMapService.h
// Author:      Tom Junk, November 2018
//
// Implementation of hardware-offline channel mapping reading from a file.
///////////////////////////////////////////////////////////////////////////////////////////////////

#include "IcebergChannelMapService.h"
#include "messagefacility/MessageLogger/MessageLogger.h"

// Bad channel value
unsigned int bad() {
  unsigned int val = std::numeric_limits<unsigned int>::max();
  return val;
}

dune::IcebergChannelMapService::IcebergChannelMapService(fhicl::ParameterSet const& pset) {

  fBadCrateNumberWarningsIssued = 0;
  fBadSlotNumberWarningsIssued = 0;
  fBadFiberNumberWarningsIssued = 0;
  fSSPBadChannelNumberWarningsIssued = 0;
  fASICWarningsIssued = 0;
  fASICChanWarningsIssued = 0;

  std::string channelMapFile = pset.get<std::string>("FileName");

  std::string fullname;
  cet::search_path sp("FW_SEARCH_PATH");
  sp.find_file(channelMapFile, fullname);

  if (fullname.empty()) {
    std::cout << "Input file " << channelMapFile << " not found" << std::endl;
    throw cet::exception("File not found");
  }
  else
    std::cout << "Iceberg Channel Map: Building RCE TPC wiremap from file " << channelMapFile << std::endl;

  std::ifstream inFile(fullname, std::ios::in);
  std::string line;

  while (std::getline(inFile,line)) {
    unsigned int crateNo, slotNo, fiberNo, FEMBChannel, StreamChannel, slotID, fiberID, chipNo, chipChannel, asicNo, asicChannel, planeType, offlineChannel;
    std::stringstream linestream(line);
    linestream >> crateNo >> slotNo >> fiberNo>> FEMBChannel >> StreamChannel >> slotID >> fiberID >> chipNo >> chipChannel >> asicNo >> asicChannel >> planeType >> offlineChannel;

    // fill lookup tables.  Throw an exception if any number is out of expected bounds.
    // checking for negative values produces compiler warnings as these are unsigned ints

    if (offlineChannel >= fNChans)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood Offline Channel: " << offlineChannel << "\n";
      }
    if (crateNo >= fNCrates)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood Crate Number: " << crateNo << "\n";
      }
    if (slotNo >= fNSlots)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood Slot Number: " << slotNo << "\n";
      }
    if (fiberNo >= fNFibers)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood Fiber Number: " << fiberNo << "\n";
      }
    if (StreamChannel >= fNFEMBChans)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood FEMB (Stream) Channel Number: " << StreamChannel << "\n";
      }

    farrayCsfcToOffline[crateNo][slotNo][fiberNo][StreamChannel] = offlineChannel;
    fvAPAMap[offlineChannel] = crateNo; 
    fvWIBMap[offlineChannel] = slotNo; 
    fvFEMBMap[offlineChannel] = fiberNo; 
    fvFEMBChannelMap[offlineChannel] = FEMBChannel; 
    fvStreamChannelMap[offlineChannel] = StreamChannel;
    fvSlotIdMap[offlineChannel] = slotID; 
    fvFiberIdMap[offlineChannel] = fiberID; 
    fvChipMap[offlineChannel] = chipNo; 
    fvChipChannelMap[offlineChannel] = chipChannel;
    fvASICMap[offlineChannel] = asicNo;
    fvASICChannelMap[offlineChannel] = asicChannel;
    fvPlaneMap[offlineChannel] = planeType;

  }
  inFile.close();



  std::string FELIXchannelMapFile = pset.get<std::string>("FELIXFileName");
  sp.find_file(FELIXchannelMapFile, fullname);

  if (fullname.empty()) {
    std::cout << "Input file " << FELIXchannelMapFile << " not found" << std::endl;
    throw cet::exception("File not found");
  }
  else
    std::cout << "Iceberg Channel Map: Building FELIX TPC wiremap from file " << channelMapFile << std::endl;

  std::ifstream FELIXinFile(fullname, std::ios::in);

  while (std::getline(FELIXinFile,line)) {
    unsigned int crateNo, slotNo, fiberNo, FEMBChannel, StreamChannel, slotID, fiberID, chipNo, chipChannel, asicNo, asicChannel, planeType, offlineChannel;
    std::stringstream linestream(line);
    linestream >> crateNo >> slotNo >> fiberNo>> FEMBChannel >> StreamChannel >> slotID >> fiberID >> chipNo >> chipChannel >> asicNo >> asicChannel >> planeType >> offlineChannel;

    // fill lookup tables.  Throw an exception if any number is out of expected bounds.
    // checking for negative values produces compiler warnings as these are unsigned ints

    if (offlineChannel >= fNChans)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood Offline Channel: " << offlineChannel << "\n";
      }
    if (crateNo >= fNCrates)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood Crate Number: " << crateNo << "\n";
      }
    if (slotNo >= fNSlots)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood Slot Number: " << slotNo << "\n";
      }
    if (fiberNo >= fNFibers)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood Fiber Number: " << fiberNo << "\n";
      }
    if (StreamChannel >= fNFEMBChans)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood FEMB (Stream) Channel Number: " << StreamChannel << "\n";
      }

    fFELIXarrayCsfcToOffline[crateNo][slotNo][fiberNo][StreamChannel] = offlineChannel;
    fFELIXvAPAMap[offlineChannel] = crateNo; 
    fFELIXvWIBMap[offlineChannel] = slotNo; 
    fFELIXvFEMBMap[offlineChannel] = fiberNo; 
    fFELIXvFEMBChannelMap[offlineChannel] = FEMBChannel; 
    fFELIXvStreamChannelMap[offlineChannel] = StreamChannel;
    fFELIXvSlotIdMap[offlineChannel] = slotID; 
    fFELIXvFiberIdMap[offlineChannel] = fiberID; 
    fFELIXvChipMap[offlineChannel] = chipNo; 
    fFELIXvChipChannelMap[offlineChannel] = chipChannel;
    fFELIXvASICMap[offlineChannel] = asicNo;
    fFELIXvASICChannelMap[offlineChannel] = asicChannel;
    fFELIXvPlaneMap[offlineChannel] = planeType;

  }
  inFile.close();


  std::string SSPchannelMapFile = pset.get<std::string>("SSPFileName");

  std::string SSPfullname;
  sp.find_file(SSPchannelMapFile, SSPfullname);

  if (SSPfullname.empty()) {
    std::cout << "Input file for SSP Channel Map " << SSPchannelMapFile << " not found in FW_SEARCH_PATH " << std::endl;
    throw cet::exception("File not found");
  }
  else
    std::cout << "Iceberg Channel Map: Building SSP channel map from file " << SSPchannelMapFile << std::endl;

  std::ifstream SSPinFile(SSPfullname, std::ios::in);

  while (std::getline(SSPinFile,line)) {
    unsigned int onlineChannel, APA, SSP, SSPGlobal, ChanWithinSSP, OpDetNo, offlineChannel;
    std::stringstream linestream(line);
    linestream >> onlineChannel >> APA >> SSP >> SSPGlobal >> ChanWithinSSP >> OpDetNo >> offlineChannel;

    // fill lookup tables.  Throw an exception if any number is out of expected bounds.
    // checking for negative values produces compiler warnings as these are unsigned ints

    if (onlineChannel >= fNSSPChans)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood SSP Online Channel: " << onlineChannel << "\n";
      }
    if (offlineChannel >= fNSSPChans)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood SSP Offline Channel: " << offlineChannel << "\n";
      }
    if (APA > fNAPAs) // APAs count from 1
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood APA Number in SSP map file: " << APA << "\n";
      }
    //if (SSP >= fNSSPsPerAPA) -- These checks don't make sense
    //  {
    //     throw cet::exception("IcebergChannelMapService") << "Ununderstood SSP number within this APA: " << SSP << " " << APA << "\n";
    //  }
    //if (SSPGlobal >= fNSSPs)
    //  {
    //     throw cet::exception("IcebergChannelMapService") << "Ununderstood Global SSP number: " << SSPGlobal << "\n";
    //  }
    if (ChanWithinSSP >= fNChansPerSSP)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood Channel within SSP Number: " << ChanWithinSSP << " " << SSPGlobal << "\n";
      }
    if (OpDetNo >= 60)
      {
	throw cet::exception("IcebergChannelMapService") << "Ununderstood SSP Module Number: " << OpDetNo << "\n";
      }

    farraySSPOnlineToOffline[onlineChannel] = offlineChannel;
    farraySSPOfflineToOnline[offlineChannel] = onlineChannel;
    fvSSPAPAMap[offlineChannel] = APA;
    fvSSPWithinAPAMap[offlineChannel] = SSP;
    fvSSPGlobalMap[offlineChannel] = SSPGlobal;
    fvSSPChanWithinSSPMap[offlineChannel] = ChanWithinSSP;
    fvOpDetNoMap[offlineChannel] = OpDetNo;
  }
  SSPinFile.close();
}

dune::IcebergChannelMapService::IcebergChannelMapService(fhicl::ParameterSet const& pset, art::ActivityRegistry&) : IcebergChannelMapService(pset) {
}

// assumes crate goes from 1-6, in "installed crate ordering"
// assumes slot goes from 0-5.
// assumes fiber goes from 1-4.
// These conventions are observed in Run 2973, a cryo commissioning run.

unsigned int dune::IcebergChannelMapService::GetOfflineNumberFromDetectorElements(unsigned int crate, unsigned int slot, unsigned int fiber, unsigned int streamchannel, FelixOrRCE frswitch) {

  unsigned int offlineChannel=0;
  unsigned int lcrate = crate;
  unsigned int lslot = slot;
  unsigned int lfiber = fiber;

  if (crate > fNCrates || crate == 0)
    {
      if ( count_bits(fBadCrateNumberWarningsIssued) == 1)
	{
	  mf::LogWarning("IcebergChannelMapService: Bad Crate Number, expecting a number of 1.  Falling back to 1.  Ununderstood crate number=") << crate;
	}
      fBadCrateNumberWarningsIssued++;
      lcrate = 1;
    }

  if (slot >= fNSlots)
    {
      if (count_bits(fBadSlotNumberWarningsIssued) == 1)
	{
	  mf::LogWarning("IcebergChannelMapService: Bad slot number, using slot number zero as a fallback.  Ununderstood slot number: ") << slot;
	}
      fBadSlotNumberWarningsIssued++;
      lslot = 0;
    }

  if (fiber > fNFibers || fiber == 0)
    {
      if (count_bits(fBadFiberNumberWarningsIssued)==1)
	{
	  mf::LogWarning("IcebergChannelMapService: Bad fiber number, falling back to 1.  Ununderstood fiber number: ") << fiber;
	}
      fBadFiberNumberWarningsIssued++;
      lfiber = 1;
    }

  if (streamchannel >= fNFEMBChans)
    {
      throw cet::exception("IcebergChannelMapService") << "Ununderstood Stream (FEMB) chan: " 
						    << crate << " " << slot << " " << fiber << " " << streamchannel << "\n";
    }

  if (frswitch == kRCE)
    {
      offlineChannel = farrayCsfcToOffline[lcrate-1][lslot][lfiber-1][streamchannel];
    }
  else
    {
      offlineChannel = fFELIXarrayCsfcToOffline[lcrate-1][lslot][lfiber-1][streamchannel];
    }

  return offlineChannel;
}

// does not depend on FELIX or RCE -- offline channels should always map to the same APA/crate regardless of RCE or FELIX

unsigned int dune::IcebergChannelMapService::APAFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvAPAMap[offlineChannel];
  // return fFELIXvAPAMap[offlineChannel];   // -- FELIX one -- should be the same
}


// does not depend on FELIX or RCE 

unsigned int dune::IcebergChannelMapService::WIBFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvWIBMap[offlineChannel];       
  // return fFELIXvWIBMap[offlineChannel];    // -- FELIX one -- should be the same
}

// does not depend on FELIX or RCE 

unsigned int dune::IcebergChannelMapService::FEMBFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvFEMBMap[offlineChannel]+1; 
  //return fFELIXvFEMBMap[offlineChannel];   
}

// does not depend on FELIX or RCE 

unsigned int dune::IcebergChannelMapService::FEMBChannelFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvFEMBChannelMap[offlineChannel];       
  // return fFELIXvFEMBChannelMap[offlineChannel];    // -- FELIX one -- should be the same
}

// this one does depend on FELIX or RCE

unsigned int dune::IcebergChannelMapService::StreamChannelFromOfflineChannel(unsigned int offlineChannel, FelixOrRCE frswitch) const {
  check_offline_channel(offlineChannel);
  if (frswitch == kRCE)
    {
      return fvStreamChannelMap[offlineChannel];       
    }
  else
    {
      return fFELIXvStreamChannelMap[offlineChannel];       
    }
}

// does not depend on FELIX or RCE 

unsigned int dune::IcebergChannelMapService::SlotIdFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvSlotIdMap[offlineChannel];  
  //  return fFELIXvSlotIdMap[offlineChannel];    // -- FELIX one -- should be the same
}

// may potentially depend on FELIX or RCE, but if fibers are switched between the WIB and the FELIX or RCE,
// we can fix this in the channel map but report with this method the fiber coming out of the WIB, not the
// one going in to the FELIX or RCE

unsigned int dune::IcebergChannelMapService::FiberIdFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvFiberIdMap[offlineChannel];       
}

// does not depend on FELIX or RCE 

unsigned int dune::IcebergChannelMapService::ChipFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvChipMap[offlineChannel];       
}

unsigned int dune::IcebergChannelMapService::AsicFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvChipMap[offlineChannel];       
}

unsigned int dune::IcebergChannelMapService::ChipChannelFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvChipChannelMap[offlineChannel];
}

// from David Adams -- use the chip channel instead of the asic channel

unsigned int dune::IcebergChannelMapService::AsicChannelFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvChipChannelMap[offlineChannel];
}

// really shouldn't be using this as it doesn't mean asic

unsigned int dune::IcebergChannelMapService::ASICFromOfflineChannel(unsigned int offlineChannel) {
  if (count_bits(fASICWarningsIssued) == 1)
    {
	  mf::LogWarning("IcebergChannelMapService: Deprecated call to ASICFromOfflineChannel.  Use AsicLinkFromOfflineChannel");
    }
  fASICWarningsIssued++;
  check_offline_channel(offlineChannel);
  return fvASICMap[offlineChannel];       
}

unsigned int dune::IcebergChannelMapService::AsicLinkFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvASICMap[offlineChannel];       
}

unsigned int dune::IcebergChannelMapService::ASICChannelFromOfflineChannel(unsigned int offlineChannel) {
  if (count_bits(fASICChanWarningsIssued) == 1)
    {
	  mf::LogWarning("IcebergChannelMapService: Deprecated call to ASICChannelFromOfflineChannel.  Not a meaningful number -- channels are grouped by 16's not 8's");
    }
  fASICChanWarningsIssued++;
  check_offline_channel(offlineChannel);
  return fvASICChannelMap[offlineChannel];
}

unsigned int dune::IcebergChannelMapService::PlaneFromOfflineChannel(unsigned int offlineChannel) const {
  check_offline_channel(offlineChannel);
  return fvPlaneMap[offlineChannel];       
}

size_t dune::IcebergChannelMapService::count_bits(size_t i)
{
  size_t result=0;
  size_t s = sizeof(size_t)*8;
  for (size_t j=0; j<s; ++j)
    {
      if (i & 1) ++result;
      i >>= 1;
    }
  return result;
}

unsigned int dune::IcebergChannelMapService::SSPOfflineChannelFromOnlineChannel(unsigned int onlineChannel) 
{
  unsigned int lchannel = onlineChannel;

  if (onlineChannel > fNSSPChans)
    {
      if (count_bits(fSSPBadChannelNumberWarningsIssued)==1)
	{
	  mf::LogWarning("IcebergChannelMapService: Online Channel Number too high, using zero as a fallback: ") << onlineChannel;
	}
      fSSPBadChannelNumberWarningsIssued++;
      lchannel = 0;
    }
  return farraySSPOnlineToOffline[lchannel];
}

unsigned int dune::IcebergChannelMapService::SSPOnlineChannelFromOfflineChannel(unsigned int offlineChannel) const
{
  SSP_check_offline_channel(offlineChannel);
  return farraySSPOfflineToOnline[offlineChannel];
}

unsigned int dune::IcebergChannelMapService::SSPAPAFromOfflineChannel(unsigned int offlineChannel) const
{
  SSP_check_offline_channel(offlineChannel);
  return fvSSPAPAMap[offlineChannel];
}

unsigned int dune::IcebergChannelMapService::SSPWithinAPAFromOfflineChannel(unsigned int offlineChannel) const
{
  SSP_check_offline_channel(offlineChannel);
  return fvSSPWithinAPAMap[offlineChannel];
}

unsigned int dune::IcebergChannelMapService::SSPGlobalFromOfflineChannel(unsigned int offlineChannel) const
{
  SSP_check_offline_channel(offlineChannel);
  return fvSSPGlobalMap[offlineChannel];
}

unsigned int dune::IcebergChannelMapService::SSPChanWithinSSPFromOfflineChannel(unsigned int offlineChannel) const
{
  SSP_check_offline_channel(offlineChannel);
  return fvSSPChanWithinSSPMap[offlineChannel];
}

unsigned int dune::IcebergChannelMapService::OpDetNoFromOfflineChannel(unsigned int offlineChannel) const
{
  SSP_check_offline_channel(offlineChannel);
  return fvOpDetNoMap[offlineChannel];
}

DEFINE_ART_SERVICE(dune::IcebergChannelMapService)
