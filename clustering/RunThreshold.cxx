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

  TFile hfile(OutFileName.c_str(), "RECREATE");

  TEfficiency *Total   = new TEfficiency("Total"  , "Total;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region1 = new TEfficiency("Region1", "Region1;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region2 = new TEfficiency("Region2", "Region2;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region3 = new TEfficiency("Region3", "Region3;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region4 = new TEfficiency("Region4", "Region4;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region5 = new TEfficiency("Region5", "Region5;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region6 = new TEfficiency("Region6", "Region6;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region7 = new TEfficiency("Region7", "Region7;Electron KE [MeV];Efficiency", 25, 0, 25 );
  TEfficiency *Region8 = new TEfficiency("Region8", "Region8;Electron KE [MeV];Efficiency", 25, 0, 25 );

  
  std::map<int, SNAnaInputManager*> aaim;
  aaim[1] = new SNAnaInputManager();
  aaim[1] -> SetInputTree(TreeName.c_str());

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }


  TH1D*        whichRegion;
  std::map<int, TEfficiency*> effEnergy;

  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  clusteng.SetTimeWindow   (20);
  clusteng.SetChannelWidth (2);
  clusteng.SetTimeWindowOpt(0.2);
  clusteng.SetPositionOpt  (300);
  clusteng.SetBucketSize   (1);
      
  wiretrigger.SetWireNHitsMin    (nHitsMinValue);
  wiretrigger.SetWireNChanMin    (2);
  wiretrigger.SetWireChanWidthMin(0);
  wiretrigger.SetWireSADCMin     (0);

  int fNEvent = nEvent;
  
  for (auto const& it: aaim) {
    // int adc_threshold = it.first;
    SNAnaInputManager* im = it.second;
    
    if (fNEvent!=-1) {
      fNEvent = std::min(fNEvent,(int)im->GetEntries());
    } else {
      fNEvent = im->GetEntries();
    }
    
    whichRegion = new TH1D("", "", 8, 0, 8);    

    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);
      std::vector<WireHit*> vec_WireHit;
      for (size_t j=0; j<im->Hit_View->size(); ++j) {
	WireHit* hit = new WireHit((*im->Hit_View)[j],        (*im->Hit_True_GenType)[j],  (*im->Hit_Chan)[j],
				   (*im->Hit_Time)[j],        (*im->Hit_SADC)[j],          (*im->Hit_RMS)[j],
				   (*im->Hit_True_Energy)[j], (*im->Hit_True_EvEnergy)[j], (*im->Hit_True_MainTrID)[j],
				   0.5, 0.5, 0.5,
				   (*im->Hit_True_X)[j],      (*im->Hit_True_Y)[j],        (*im->Hit_True_Z)[j],
				   0, (*im->Hit_True_nElec)[j]);

	vec_WireHit.push_back(hit);
      }

      bool selected = false;
      int ncluster = 0;
      int nnoisecluster = 0;
      std::vector<WireCluster*> vec_WireCluster;
      clusteng.ClusterHits2(vec_WireHit, vec_WireCluster);
      wiretrigger.IsTriggering(vec_WireCluster);
      
      for (size_t c=0; c<vec_WireCluster.size(); ++c) {
	WireCluster* clust = vec_WireCluster[c];
	if (clust->GetIsTriggering()) {
	  if (clust->GetType()) {
	    selected = true;
	    ++ncluster;
	  } else {
	    ++nnoisecluster;
	  }
	}
      }

      double KE     = im->True_ENu_Lep->at(0) * 1000. - 0.511;
      int    region = static_cast<int>(floor( (((*im->True_VertX)[0])/90) + 4 ));
      whichRegion      ->Fill(region);

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
      
