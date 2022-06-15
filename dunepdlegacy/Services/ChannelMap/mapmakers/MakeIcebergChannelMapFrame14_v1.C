
/*************************************************************************
    > File Name: MakeIcebergChannelMap_v1.C
    > Author: Tom Junk
 ************************************************************************/

using namespace std;

// Implementation of Table 5 in DocDB 4064: FEMB channel (chipNo & chipChannel) to plane type
int plane[8][16] = {
										 {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2},
										 {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2}, 
										 {2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
										 {2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
										 {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2},
										 {0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2},
										 {2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0},
										 {2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0}
	                 };

// Implementation of Table 5 in DocDB 4064: FEMB channel (chipNo & chipChannel) to plane channel number                 
int planeCh[8][16] = {
										 {19, 17, 15, 13, 11, 19, 17, 15, 13, 11, 23, 21, 19, 17, 15, 13},
										 {9,  7,  5,  3,  1,  9,  7,  5,  3,  1,  11, 9,  7,  5,  3,  1}, 
										 {14, 16, 18, 20, 22, 24, 12, 14, 16, 18, 20, 12, 14, 16, 18, 20},
										 {2,  4,  6,  8,  10, 12, 2,  4,  6,  8,  10, 2,  4,  6,  8,  10},
										 {29, 27, 25, 23, 21, 29, 27, 25, 23, 21, 35, 33, 31, 29, 27, 25},
										 {39, 37, 35, 33, 31, 39, 37, 35, 33, 31, 47, 45, 43, 41, 39, 37},
										 {26, 28, 30, 32, 34, 36, 22, 24, 26, 28, 30, 22, 24, 26, 28, 30},
										 {38, 40 ,42, 44, 46, 48, 32, 34, 36, 38, 40, 32, 34, 36, 38, 40}
	                 };


// Implementation of the FELIX channel map: FEMB channel (chipNo & chipChannel) to FELIX fragment channel index.  The second 128 channels in the frame
// have the same behavior but are offset by 128

// FRAME14 version of this -- from Ben Land's unapack.C it looks like the channels are laid out
// in sequence so we don't need a table



  // indexed by offline-oriented slot and fiber, to give online slot and fiber
  // implements Table 4 and Fig. 3a in DocDB 4064. 
  // offline slot and fiber numbers increase counterclockwise around the even APA's

// follow conventions of PD-SP and number fibers from zero.  They will have 1 added to them in the mapping code

int slotList[10] = {0,2,1,0,2,0,1,2,0,1};
int fiberList[10] = {0,1,1,1,2,3,3,3,2,2};

void MakeIcebergChannelMapFrame14_v1() {
 
  ofstream fmapfelix;
  fmapfelix.open ("IcebergChannelMap_FELIX_FRAME14_v1.txt");
  int nChipPerFEMB = 8;
  int nASICPerChip = 2;
  int nChannelPerASIC = 8;
  int FEMBChannel = 0;
  int FELIXStreamChannel = 0;
  int chipChannel = 0;
  int asicChannel = 0;
  int slotID = 0;
  int fiberID = 0;

  int nFEMBs=10;
  int crateNo = 0;
  
  int Asic = 0;
  int offlineChannel  = 0;

  int planeType = 0;
  vector<int> Uwire, Vwire, Zwire;

  for (int iFEMB = 0; iFEMB < nFEMBs; ++iFEMB)
    {
      int slotNo = slotList[iFEMB];
      int fiberNo = fiberList[iFEMB];
      slotID = slotNo;

      FEMBChannel = 0;
      FELIXStreamChannel = 0;
      for(int chipNo = 0;chipNo<nChipPerFEMB;chipNo++) {  
	chipChannel =0; 	 	 	 
	for(int asicNo = 0;asicNo<nASICPerChip;asicNo++) {
	  asicChannel = 0;
	  for(int chNo = 0; chNo<nChannelPerASIC; chNo++) {
	    //plane type
	    planeType =  plane[chipNo][chipChannel];

	    // try another mapping
	    if (planeType == 0)
	      {
		FELIXStreamChannel = planeCh[chipNo][chipChannel]-1;
	      }
	    else if (planeType == 1)
	      {
		FELIXStreamChannel = planeCh[chipNo][chipChannel]-1 + 40;
	      }
	    else if (planeType == 2)
	      {
		FELIXStreamChannel = planeCh[chipNo][chipChannel]-1 + 80;
	      }

	    int slotNoOnline = slotNo;
	    int fiberNoOnline = fiberNo;
     	 	            	 	        	
	    //offline number
	    if(planeType == 0) {
	      Uwire.push_back(planeCh[chipNo][chipChannel]);

	      int itmp = 40*iFEMB + (planeCh[chipNo][chipChannel]-1);  // should go from 0 to 399

	      if (itmp < 200)
		{
		  itmp = 199-itmp;
		}
	      else
		{
		  itmp = 399 + 200 - itmp;
		}

	      offlineChannel = itmp;
	    }
	    if(planeType == 1) {
	      Vwire.push_back(planeCh[chipNo][chipChannel]);

	      int itmp = 40*iFEMB + (planeCh[chipNo][chipChannel]-1);  // goes from 0 to 399
	      offlineChannel = 400 + itmp;

	    }
	    if(planeType == 2) {
	      Zwire.push_back(planeCh[chipNo][chipChannel]);   

	      int itmp = 48*iFEMB  + (planeCh[chipNo][chipChannel]-1);  // goes from 0 to 479
	      // the Z2 channels increase in the opposite order

	      if (itmp<240)
		{
		  // do nothing
		}
	      else
		{
		  itmp = 240 + 479 - itmp;
		}

	      offlineChannel = 800 + itmp;
	    }
     	 	       
	    fmapfelix<<crateNo<<"\t"<<slotNoOnline<<"\t"<<fiberNoOnline<<"\t"<<FEMBChannel
		     <<"\t"<<FELIXStreamChannel<<"\t"<<slotID<<"\t"<<fiberID
		     <<"\t"<<chipNo<<"\t"<<chipChannel<<"\t"<<asicNo<<"\t"
		     <<asicChannel<<"\t"<<planeType<<"\t"<<offlineChannel<<endl;  
	    FEMBChannel++;
	    chipChannel++;
	    asicChannel++;
	    //     	 	       offlineChannel++; 	 	
	  }
	}    	 	   
      } 
      fiberID++; 
    }
  fmapfelix.close();  
}
