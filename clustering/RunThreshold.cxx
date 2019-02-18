#include "ArbitraryAnaInputManager.hh"
#include "ClusterEngine.hh"
#include "Trigger.hh"

#include "Helper.h"

#include "TCanvas.h"
#include "TEfficiency.h"
#include "TFile.h"
#include "TPad.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TTree.h"
#include "TGraph.h"

#include <iostream>
#include <unistd.h>
#include <cmath>

// DISPLAY DYNAMIC PROGRESS BAR
void progress(float prog) {

  int barWidth = 60;
  std::cout << "[";
  int pos = barWidth * prog;

  for (int i=0; i<barWidth; ++i) {
    if      (i < pos)           { std::cout << "="; }
    else if (pos+1 == barWidth) { std::cout << "="; }
    else if (i == pos)          { std::cout << ">"; }
    else                        { std::cout << " "; }
  }

  std::cout << "] " << (int)round(100.0 * prog) << " %\r" << std::flush;

}

int main (int argc, char** argv) {

  int opt;
  // Shut GetOpt error messages down (return '?'):
  extern char *optarg;
  extern int  optopt;
  
  std::string InFileName ="";
  std::string OutFileName="";
  std::string TreeName   ="";
  int nEvent=-1;
  int nHitsMinValue=-1;
  while ( (opt = getopt(argc, argv, "i:o:t:n:h:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'i':
      InFileName = optarg;
      break;
    case 'o':
      OutFileName = optarg;
      break;
    case 't':
      TreeName = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case 'h':
      nHitsMinValue = atoi(optarg);
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  // THIS IS THE ROOT FILE THAT WILL BE THE OUTPUT OF THE PROGRAM
  TFile hfile(OutFileName.c_str(), "RECREATE");


  // ------------------------------------------------------- DECLARING ALL OF THE PLOTS THAT I WANT TO BE SAVED ------------------------------------------------------- //
  
  TH1D     *whichRegion      = new TH1D    ("", "", 8, 0, 8); 

  // ALL OF THE EFFICIENCY PLOTS FOR THE SLICES OF THE DETECTOR
  TEfficiency *Total   = new TEfficiency("Total"  , "Total;Electron KE [MeV];Efficiency"  , 25, 0, 25 );
  TEfficiency *Region1 = new TEfficiency("Region1", "Region1;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region2 = new TEfficiency("Region2", "Region2;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region3 = new TEfficiency("Region3", "Region3;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region4 = new TEfficiency("Region4", "Region4;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region5 = new TEfficiency("Region5", "Region5;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region6 = new TEfficiency("Region6", "Region6;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region7 = new TEfficiency("Region7", "Region7;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region8 = new TEfficiency("Region8", "Region8;Electron KE [MeV];Efficiency", 25, 0, 25 );

  // ALL OF THE SPREADS THE FOUR TRIGGER VARIABLES WITH RESPECT TO POSITION
  TProfile *tprof_timeSpreadPos = new TProfile("tprof_timeSpreadPos", ";x-distance;time [ns]   ", 8, 0, 8, 0, 100       );
  TProfile *tprof_chanSpreadPos = new TProfile("tprof_chanSpreadPos", ";x-distance;channels [#]", 8, 0, 8, 0, 100       );
  TProfile *tprof_hitsSpreadPos = new TProfile("tprof_hitsSpreadPos", ";x-distance;hits [#]    ", 8, 0, 8, 0, 100       );
  TProfile *tprof_SADCSpreadPos = new TProfile("tprof_SADCSpreadPos", ";x-distance;SADC [#]    ", 8, 0, 8, 0, 3000      );
  TH2D     *th2d_timeSpreadPos  = new TH2D    ("th2d_timeSpreadPos" , ";x-distance;time [ns]   ", 8, 0, 8, 100, 0, 100  );
  TH2D	   *th2d_chanSpreadPos  = new TH2D    ("th2d_chanSpreadPos" , ";x-distance;channels [#]", 8, 0, 8, 100, 0, 100  );
  TH2D	   *th2d_hitsSpreadPos  = new TH2D    ("th2d_hitsSpreadPos" , ";x-distance;hits [#]    ", 8, 0, 8, 100, 0, 100  );
  TH2D     *th2d_SADCSpreadPos  = new TH2D    ("th2d_SADCSpreadPos" , ";x-distance;SADC [#]    ", 8, 0, 8, 3000, 0, 3000);

  // ALL OF THE SPREADS THE FOUR TRIGGER VARIABLES WITH RESPECT TO ENERGY
  TProfile *tprof_timeSpreadEnergy = new TProfile("tprof_timeSpreadEnergy", ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 0, 100       );
  TProfile *tprof_chanSpreadEnergy = new TProfile("tprof_chanSpreadEnergy", ";Electron KE [MeV];channels [#]", 20, 0, 20, 0, 100       );
  TProfile *tprof_hitsSpreadEnergy = new TProfile("tprof_hitsSpreadEnergy", ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 0, 100       );
  TProfile *tprof_SADCSpreadEnergy = new TProfile("tprof_SADCSpreadEnergy", ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 0, 3000      );
  TH2D     *th2d_timeSpreadEnergy  = new TH2D    ("th2d_timeSpreadEnergy" , ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_chanSpreadEnergy  = new TH2D    ("th2d_chanSpreadEnergy" , ";Electron KE [MeV];channels [#]", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_hitsSpreadEnergy  = new TH2D    ("th2d_hitsSpreadEnergy" , ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 100, 0, 100  );
  TH2D     *th2d_SADCSpreadEnergy  = new TH2D    ("th2d_SADCSpreadEnergy" , ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 3000, 0, 3000);

  // ALL OF THE SPREADS AND PROFILES FOR R1
  TProfile *tprof_timeSpreadEnergyR1 = new TProfile("tprof_timeSpreadEnergyR1", ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 0, 100       );
  TProfile *tprof_chanSpreadEnergyR1 = new TProfile("tprof_chanSpreadEnergyR1", ";Electron KE [MeV];channels [#]", 20, 0, 20, 0, 100       );
  TProfile *tprof_hitsSpreadEnergyR1 = new TProfile("tprof_hitsSpreadEnergyR1", ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 0, 100       );
  TProfile *tprof_SADCSpreadEnergyR1 = new TProfile("tprof_SADCSpreadEnergyR1", ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 0, 3000      );
  TH2D     *th2d_timeSpreadEnergyR1  = new TH2D    ("th2d_timeSpreadEnergyR1" , ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_chanSpreadEnergyR1  = new TH2D    ("th2d_chanSpreadEnergyR1" , ";Electron KE [MeV];channels [#]", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_hitsSpreadEnergyR1  = new TH2D    ("th2d_hitsSpreadEnergyR1" , ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 100, 0, 100  );
  TH2D     *th2d_SADCSpreadEnergyR1  = new TH2D    ("th2d_SADCSpreadEnergyR1" , ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 3000, 0, 3000);

  // ALL OF THE SPREADS AND PROFILES FOR R2
  TProfile *tprof_timeSpreadEnergyR2 = new TProfile("tprof_timeSpreadEnergyR2", ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 0, 100       );
  TProfile *tprof_chanSpreadEnergyR2 = new TProfile("tprof_chanSpreadEnergyR2", ";Electron KE [MeV];channels [#]", 20, 0, 20, 0, 100       );
  TProfile *tprof_hitsSpreadEnergyR2 = new TProfile("tprof_hitsSpreadEnergyR2", ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 0, 100       );
  TProfile *tprof_SADCSpreadEnergyR2 = new TProfile("tprof_SADCSpreadEnergyR2", ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 0, 3000      );
  TH2D     *th2d_timeSpreadEnergyR2  = new TH2D    ("th2d_timeSpreadEnergyR2" , ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_chanSpreadEnergyR2  = new TH2D    ("th2d_chanSpreadEnergyR2" , ";Electron KE [MeV];channels [#]", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_hitsSpreadEnergyR2  = new TH2D    ("th2d_hitsSpreadEnergyR2" , ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 100, 0, 100  );
  TH2D     *th2d_SADCSpreadEnergyR2  = new TH2D    ("th2d_SADCSpreadEnergyR2" , ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 3000, 0, 3000);

  // ALL OF THE SPREADS AND PROFILES FOR R3
  TProfile *tprof_timeSpreadEnergyR3 = new TProfile("tprof_timeSpreadEnergyR3", ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 0, 100       );
  TProfile *tprof_chanSpreadEnergyR3 = new TProfile("tprof_chanSpreadEnergyR3", ";Electron KE [MeV];channels [#]", 20, 0, 20, 0, 100       );
  TProfile *tprof_hitsSpreadEnergyR3 = new TProfile("tprof_hitsSpreadEnergyR3", ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 0, 100       );
  TProfile *tprof_SADCSpreadEnergyR3 = new TProfile("tprof_SADCSpreadEnergyR3", ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 0, 3000      );
  TH2D     *th2d_timeSpreadEnergyR3  = new TH2D    ("th2d_timeSpreadEnergyR3" , ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_chanSpreadEnergyR3  = new TH2D    ("th2d_chanSpreadEnergyR3" , ";Electron KE [MeV];channels [#]", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_hitsSpreadEnergyR3  = new TH2D    ("th2d_hitsSpreadEnergyR3" , ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 100, 0, 100  );
  TH2D     *th2d_SADCSpreadEnergyR3  = new TH2D    ("th2d_SADCSpreadEnergyR3" , ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 3000, 0, 3000);
  
  // ALL OF THE SPREADS AND PROFILES FOR R4
  TProfile *tprof_timeSpreadEnergyR4 = new TProfile("tprof_timeSpreadEnergyR4", ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 0, 100       );
  TProfile *tprof_chanSpreadEnergyR4 = new TProfile("tprof_chanSpreadEnergyR4", ";Electron KE [MeV];channels [#]", 20, 0, 20, 0, 100       );
  TProfile *tprof_hitsSpreadEnergyR4 = new TProfile("tprof_hitsSpreadEnergyR4", ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 0, 100       );
  TProfile *tprof_SADCSpreadEnergyR4 = new TProfile("tprof_SADCSpreadEnergyR4", ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 0, 3000      );
  TH2D     *th2d_timeSpreadEnergyR4  = new TH2D    ("th2d_timeSpreadEnergyR4" , ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_chanSpreadEnergyR4  = new TH2D    ("th2d_chanSpreadEnergyR4" , ";Electron KE [MeV];channels [#]", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_hitsSpreadEnergyR4  = new TH2D    ("th2d_hitsSpreadEnergyR4" , ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 100, 0, 100  );
  TH2D     *th2d_SADCSpreadEnergyR4  = new TH2D    ("th2d_SADCSpreadEnergyR4" , ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 3000, 0, 3000);

  // ALL OF THE SPREADS AND PROFILES FOR R5
  TProfile *tprof_timeSpreadEnergyR5 = new TProfile("tprof_timeSpreadEnergyR5", ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 0, 100       );
  TProfile *tprof_chanSpreadEnergyR5 = new TProfile("tprof_chanSpreadEnergyR5", ";Electron KE [MeV];channels [#]", 20, 0, 20, 0, 100       );
  TProfile *tprof_hitsSpreadEnergyR5 = new TProfile("tprof_hitsSpreadEnergyR5", ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 0, 100       );
  TProfile *tprof_SADCSpreadEnergyR5 = new TProfile("tprof_SADCSpreadEnergyR5", ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 0, 3000      );
  TH2D     *th2d_timeSpreadEnergyR5  = new TH2D    ("th2d_timeSpreadEnergyR5" , ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_chanSpreadEnergyR5  = new TH2D    ("th2d_chanSpreadEnergyR5" , ";Electron KE [MeV];channels [#]", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_hitsSpreadEnergyR5  = new TH2D    ("th2d_hitsSpreadEnergyR5" , ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 100, 0, 100  );
  TH2D     *th2d_SADCSpreadEnergyR5  = new TH2D    ("th2d_SADCSpreadEnergyR5" , ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 3000, 0, 3000);

  // ALL OF THE SPREADS AND PROFILES FOR R6
  TProfile *tprof_timeSpreadEnergyR6 = new TProfile("tprof_timeSpreadEnergyR6", ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 0, 100       );
  TProfile *tprof_chanSpreadEnergyR6 = new TProfile("tprof_chanSpreadEnergyR6", ";Electron KE [MeV];channels [#]", 20, 0, 20, 0, 100       );
  TProfile *tprof_hitsSpreadEnergyR6 = new TProfile("tprof_hitsSpreadEnergyR6", ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 0, 100       );
  TProfile *tprof_SADCSpreadEnergyR6 = new TProfile("tprof_SADCSpreadEnergyR6", ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 0, 3000      );
  TH2D     *th2d_timeSpreadEnergyR6  = new TH2D    ("th2d_timeSpreadEnergyR6" , ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_chanSpreadEnergyR6  = new TH2D    ("th2d_chanSpreadEnergyR6" , ";Electron KE [MeV];channels [#]", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_hitsSpreadEnergyR6  = new TH2D    ("th2d_hitsSpreadEnergyR6" , ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 100, 0, 100  );
  TH2D     *th2d_SADCSpreadEnergyR6  = new TH2D    ("th2d_SADCSpreadEnergyR6" , ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 3000, 0, 3000);

  // ALL OF THE SPREADS AND PROFILES FOR R7
  TProfile *tprof_timeSpreadEnergyR7 = new TProfile("tprof_timeSpreadEnergyR7", ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 0, 100       );
  TProfile *tprof_chanSpreadEnergyR7 = new TProfile("tprof_chanSpreadEnergyR7", ";Electron KE [MeV];channels [#]", 20, 0, 20, 0, 100       );
  TProfile *tprof_hitsSpreadEnergyR7 = new TProfile("tprof_hitsSpreadEnergyR7", ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 0, 100       );
  TProfile *tprof_SADCSpreadEnergyR7 = new TProfile("tprof_SADCSpreadEnergyR7", ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 0, 3000      );
  TH2D     *th2d_timeSpreadEnergyR7  = new TH2D    ("th2d_timeSpreadEnergyR7" , ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_chanSpreadEnergyR7  = new TH2D    ("th2d_chanSpreadEnergyR7" , ";Electron KE [MeV];channels [#]", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_hitsSpreadEnergyR7  = new TH2D    ("th2d_hitsSpreadEnergyR7" , ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 100, 0, 100  );
  TH2D     *th2d_SADCSpreadEnergyR7  = new TH2D    ("th2d_SADCSpreadEnergyR7" , ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 3000, 0, 3000);

  // ALL OF THE SPREADS AND PROFILES FOR R8
  TProfile *tprof_timeSpreadEnergyR8 = new TProfile("tprof_timeSpreadEnergyR8", ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 0, 100       );
  TProfile *tprof_chanSpreadEnergyR8 = new TProfile("tprof_chanSpreadEnergyR8", ";Electron KE [MeV];channels [#]", 20, 0, 20, 0, 100       );
  TProfile *tprof_hitsSpreadEnergyR8 = new TProfile("tprof_hitsSpreadEnergyR8", ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 0, 100       );
  TProfile *tprof_SADCSpreadEnergyR8 = new TProfile("tprof_SADCSpreadEnergyR8", ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 0, 3000      );
  TH2D     *th2d_timeSpreadEnergyR8  = new TH2D    ("th2d_timeSpreadEnergyR8" , ";Electron KE [MeV];time [ns]   ", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_chanSpreadEnergyR8  = new TH2D    ("th2d_chanSpreadEnergyR8" , ";Electron KE [MeV];channels [#]", 20, 0, 20, 100, 0, 100  );
  TH2D	   *th2d_hitsSpreadEnergyR8  = new TH2D    ("th2d_hitsSpreadEnergyR8" , ";Electron KE [MeV];hits [#]    ", 20, 0, 20, 100, 0, 100  );
  TH2D     *th2d_SADCSpreadEnergyR8  = new TH2D    ("th2d_SADCSpreadEnergyR8" , ";Electron KE [MeV];SADC [#]    ", 20, 0, 20, 3000, 0, 3000);

  // ------------------------------------------------------------------------------------------------------------------------------------------------------------------ //
                                                            
  
  std::map<int, SNAnaInputManager*> aaim;
  aaim[1] = new SNAnaInputManager();
  aaim[1] -> SetInputTree(TreeName.c_str());

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }

  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  clusteng.SetTimeWindow   (20);
  clusteng.SetChannelWidth (2);
  clusteng.SetTimeWindowOpt(0.2);
  clusteng.SetPositionOpt  (300);
  clusteng.SetBucketSize   (1);
      
  wiretrigger.SetWireNHitsMin    (3);
  wiretrigger.SetWireNChanMin    (2);
  wiretrigger.SetWireChanWidthMin(0);
  wiretrigger.SetWireSADCMin     (nHitsMinValue);

  int fNEvent = nEvent;
  
  for (auto const& it: aaim) {

    SNAnaInputManager* im = it.second;
    
    if (fNEvent!=-1) {fNEvent = std::min(fNEvent,(int)im->GetEntries());}
    else             {fNEvent = im->GetEntries();}
    
    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);

      float prog = static_cast<float>(CurrentEvent)/static_cast<float>(fNEvent);
      progress(prog);
      
      if (abs((*im->True_VertX)[0]) < 15 || abs((*im->True_VertX)[0]) > 345)
	continue;
      double KE     = im->True_ENu_Lep->at(0) * 1000. - 0.511;
      if (KE > 10.0)
	continue;
      
      std::vector<WireHit*> vec_WireHit;
      for (size_t j=0; j<im->Hit_View->size(); ++j) {
	WireHit* hit = new WireHit((*im->Hit_View)         [j],
				   (*im->Hit_True_GenType) [j],
				   (*im->Hit_Chan)         [j],
				   (*im->Hit_Time)         [j],
				   (*im->Hit_SADC)         [j],
				   (*im->Hit_RMS)          [j],
				   (*im->Hit_True_Energy)  [j],
				   (*im->Hit_True_EvEnergy)[j],
				   (*im->Hit_True_MainTrID)[j],
				   0.5, 0.5, 0.5,
				   (*im->Hit_True_X)       [j],
				   (*im->Hit_True_Y)       [j],
				   (*im->Hit_True_Z)       [j],
				   0,
				   (*im->Hit_True_nElec)   [j]);
	vec_WireHit.push_back(hit);
      }

      bool selected = false;
      int ncluster = 0;
      int nnoisecluster = 0;
      std::vector<WireCluster*> vec_WireCluster;
      clusteng.ClusterHits2(vec_WireHit, vec_WireCluster);
      wiretrigger.IsTriggering(vec_WireCluster);
      
      int    region = static_cast<int>(floor( (((*im->True_VertX)[0])/90) + 4 ));

      for (size_t c=0; c<vec_WireCluster.size(); ++c) {
	WireCluster* clust = vec_WireCluster[c];
	if (clust->GetIsTriggering()) {
	  if (clust->GetType()) {
	    selected = true;

	    std::vector<float> TimeExtremes = {0, 0};
	    std::vector<int>    ChanExtremes = {0, 0};
	
	    std::vector<float> HitTimes       = clust->GetPartTimes();
	    std::vector<int>    HitChans       = clust->GetPartChannels();
	    int                 nHitsInCluster = HitTimes.size();

	    TimeExtremes[0] = HitTimes[0]; TimeExtremes[1] = HitTimes[0];	
	    ChanExtremes[0] = HitChans[0]; ChanExtremes[1] = HitChans[0];

	    for (size_t i=0; i<HitTimes.size(); ++i) {
	      if (HitTimes[i] < TimeExtremes[0]) { TimeExtremes[0] = HitTimes[i]; }
	      if (HitTimes[i] > TimeExtremes[1]) { TimeExtremes[1] = HitTimes[i]; }
	      if (HitChans[i] < ChanExtremes[0]) { ChanExtremes[0] = HitChans[i]; }
	      if (HitChans[i] > ChanExtremes[1]) { ChanExtremes[1] = HitChans[i]; }
	    }
	    
	    // FILLING ALL OF THE POSITION TH2Ds
	    th2d_timeSpreadPos->Fill(region, TimeExtremes[1] - TimeExtremes[0]);
	    th2d_chanSpreadPos->Fill(region, ChanExtremes[1] - ChanExtremes[0]);
	    th2d_SADCSpreadPos->Fill(region, clust->GetHitSumADC()            );
	    th2d_hitsSpreadPos->Fill(region, nHitsInCluster                   );

	    // FILLING ALL OF THE ENERGY TH2Ds
	    th2d_timeSpreadEnergy ->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	    th2d_chanSpreadEnergy ->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	    th2d_hitsSpreadEnergy ->Fill(KE, clust->GetHitSumADC()            );
	    th2d_SADCSpreadEnergy ->Fill(KE, nHitsInCluster                   );
	    
	    // FILLING ALL OF THE POSITION PROFILES
	    tprof_timeSpreadPos->Fill(region, TimeExtremes[1] - TimeExtremes[0]);
	    tprof_chanSpreadPos->Fill(region, ChanExtremes[1] - ChanExtremes[0]);
	    tprof_SADCSpreadPos->Fill(region, clust->GetHitSumADC()            );
	    tprof_hitsSpreadPos->Fill(region, nHitsInCluster                   );

	    // FILLING ALL OF THE ENERGY PROFILES
	    tprof_timeSpreadEnergy->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	    tprof_chanSpreadEnergy->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	    tprof_hitsSpreadEnergy->Fill(KE, clust->GetHitSumADC()            );
	    tprof_SADCSpreadEnergy->Fill(KE, nHitsInCluster                   );

	    // FILLLING ALL OF THE TH2Ds AND PROFILES FOR EACH INDIVIDUAL SLICE
	    if (region == 0) {
	      tprof_timeSpreadEnergyR1->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      tprof_chanSpreadEnergyR1->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      tprof_hitsSpreadEnergyR1->Fill(KE, clust->GetHitSumADC()            );
	      tprof_SADCSpreadEnergyR1->Fill(KE, nHitsInCluster                   );	      
	      th2d_timeSpreadEnergyR1 ->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      th2d_chanSpreadEnergyR1 ->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      th2d_hitsSpreadEnergyR1 ->Fill(KE, clust->GetHitSumADC()            );
	      th2d_SADCSpreadEnergyR1 ->Fill(KE, nHitsInCluster                   );
	    }

	    if (region == 1) {
	      tprof_timeSpreadEnergyR2->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      tprof_chanSpreadEnergyR2->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      tprof_hitsSpreadEnergyR2->Fill(KE, clust->GetHitSumADC()            );
	      tprof_SADCSpreadEnergyR2->Fill(KE, nHitsInCluster                   );	      
	      th2d_timeSpreadEnergyR2 ->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      th2d_chanSpreadEnergyR2 ->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      th2d_hitsSpreadEnergyR2 ->Fill(KE, clust->GetHitSumADC()            );
	      th2d_SADCSpreadEnergyR2 ->Fill(KE, nHitsInCluster                   );
	    }

	    if (region == 2) {
	      tprof_timeSpreadEnergyR3->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      tprof_chanSpreadEnergyR3->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      tprof_hitsSpreadEnergyR3->Fill(KE, clust->GetHitSumADC()            );
	      tprof_SADCSpreadEnergyR3->Fill(KE, nHitsInCluster                   );	      
	      th2d_timeSpreadEnergyR3 ->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      th2d_chanSpreadEnergyR3 ->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      th2d_hitsSpreadEnergyR3 ->Fill(KE, clust->GetHitSumADC()            );
	      th2d_SADCSpreadEnergyR3 ->Fill(KE, nHitsInCluster                   );
	    }

	    if (region == 3) {
	      tprof_timeSpreadEnergyR4->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      tprof_chanSpreadEnergyR4->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      tprof_hitsSpreadEnergyR4->Fill(KE, clust->GetHitSumADC()            );
	      tprof_SADCSpreadEnergyR4->Fill(KE, nHitsInCluster                   );	      
	      th2d_timeSpreadEnergyR4 ->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      th2d_chanSpreadEnergyR4 ->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      th2d_hitsSpreadEnergyR4 ->Fill(KE, clust->GetHitSumADC()            );
	      th2d_SADCSpreadEnergyR4 ->Fill(KE, nHitsInCluster                   );
	    }

	    if (region == 4) {
	      tprof_timeSpreadEnergyR5->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      tprof_chanSpreadEnergyR5->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      tprof_hitsSpreadEnergyR5->Fill(KE, clust->GetHitSumADC()            );
	      tprof_SADCSpreadEnergyR5->Fill(KE, nHitsInCluster                   );	      
	      th2d_timeSpreadEnergyR5 ->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      th2d_chanSpreadEnergyR5 ->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      th2d_hitsSpreadEnergyR5 ->Fill(KE, clust->GetHitSumADC()            );
	      th2d_SADCSpreadEnergyR5 ->Fill(KE, nHitsInCluster                   );
	    }

	    if (region == 5) {
	      tprof_timeSpreadEnergyR6->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      tprof_chanSpreadEnergyR6->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      tprof_hitsSpreadEnergyR6->Fill(KE, clust->GetHitSumADC()            );
	      tprof_SADCSpreadEnergyR6->Fill(KE, nHitsInCluster                   );	      
	      th2d_timeSpreadEnergyR6 ->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      th2d_chanSpreadEnergyR6 ->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      th2d_hitsSpreadEnergyR6 ->Fill(KE, clust->GetHitSumADC()            );
	      th2d_SADCSpreadEnergyR6 ->Fill(KE, nHitsInCluster                   );
	    }

	    if (region == 6) {
	      tprof_timeSpreadEnergyR7->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      tprof_chanSpreadEnergyR7->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      tprof_hitsSpreadEnergyR7->Fill(KE, clust->GetHitSumADC()            );
	      tprof_SADCSpreadEnergyR7->Fill(KE, nHitsInCluster                   );	      
	      th2d_timeSpreadEnergyR7 ->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      th2d_chanSpreadEnergyR7 ->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      th2d_hitsSpreadEnergyR7 ->Fill(KE, clust->GetHitSumADC()            );
	      th2d_SADCSpreadEnergyR7 ->Fill(KE, nHitsInCluster                   );
	    }

	    if (region == 7) {
	      tprof_timeSpreadEnergyR8->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      tprof_chanSpreadEnergyR8->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      tprof_hitsSpreadEnergyR8->Fill(KE, clust->GetHitSumADC()            );
	      tprof_SADCSpreadEnergyR8->Fill(KE, nHitsInCluster                   );	      
	      th2d_timeSpreadEnergyR8 ->Fill(KE, TimeExtremes[1] - TimeExtremes[0]);
	      th2d_chanSpreadEnergyR8 ->Fill(KE, ChanExtremes[1] - ChanExtremes[0]);
	      th2d_hitsSpreadEnergyR8 ->Fill(KE, clust->GetHitSumADC()            );
	      th2d_SADCSpreadEnergyR8 ->Fill(KE, nHitsInCluster                   );
	    }

	    HitTimes.clear();
	    HitChans.clear();

	    ++ncluster;
	  } else {
	    ++nnoisecluster;
	  }
	}
      }

      whichRegion->Fill(region);

      Total->Fill(selected, KE);
      
      if (region == 0 ) { Region1->Fill(selected, KE); }
      if (region == 1 ) { Region2->Fill(selected, KE); }
      if (region == 2 ) { Region3->Fill(selected, KE); }
      if (region == 3 ) { Region4->Fill(selected, KE); }
      if (region == 4 ) { Region5->Fill(selected, KE); }
      if (region == 5 ) { Region6->Fill(selected, KE); }
      if (region == 6 ) { Region7->Fill(selected, KE); }
      if (region == 7 ) { Region8->Fill(selected, KE); }
      
      for (auto& it: vec_WireHit)     { delete it; it = NULL; }
      for (auto& it: vec_WireCluster) {	delete it; it = NULL; }
      vec_WireHit    .clear();
      vec_WireCluster.clear();
      
    }
  }

  hfile.Write();

}
      
