#include "IO/InputManager.hh"
#include "Algorithm/ClusterEngine.hh"
#include "Algorithm/Trigger.hh"

#include "Utils/Helper.h"

#include "TCanvas.h"
#include "TEfficiency.h"
#include "TFile.h"
#include "TPad.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TTree.h"

#include <iostream>
#include <unistd.h>

int main(int argc, char** argv) {

  int opt;

  extern char *optarg;
  extern int   optopt;

  int nEvent              = -1;
  std::string InFileName  = "";
  std::string OutFileName = "";

  while ( (opt = getopt(argc, argv, "i:o:n:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'i':
      InFileName = optarg;
      break;
    case 'o':
      OutFileName = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  std::map<int, SNAnaInputManager*> aaim;
  aaim[800]  = new SNAnaInputManager(); aaim[800] ->SetInputTree("snanarunsum800/SNSimTree");
  aaim[1000] = new SNAnaInputManager();	aaim[1000]->SetInputTree("snanarunsum1000/SNSimTree");
  aaim[1200] = new SNAnaInputManager();	aaim[1200]->SetInputTree("snanarunsum1200/SNSimTree");
  aaim[1400] = new SNAnaInputManager();	aaim[1400]->SetInputTree("snanarunsum1400/SNSimTree");
  aaim[1600] = new SNAnaInputManager();	aaim[1600]->SetInputTree("snanarunsum1600/SNSimTree");
  aaim[1800] = new SNAnaInputManager();	aaim[1800]->SetInputTree("snanarunsum1800/SNSimTree");
  aaim[2000] = new SNAnaInputManager();	aaim[2000]->SetInputTree("snanarunsum2000/SNSimTree");
  aaim[2400] = new SNAnaInputManager();	aaim[2400]->SetInputTree("snanarunsum2400/SNSimTree");
  aaim[2800] = new SNAnaInputManager();	aaim[2800]->SetInputTree("snanarunsum2800/SNSimTree");
  aaim[3200] = new SNAnaInputManager();	aaim[3200]->SetInputTree("snanarunsum3200/SNSimTree");

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }

  std::map<int, TEfficiency*> effEnergyU;
  std::map<int, TEfficiency*> effEnergyV;
  std::map<int, TEfficiency*> effEnergyZ;

  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  clusteng.SetTimeWindow   (20);
  clusteng.SetChannelWidth (2);
  clusteng.SetTimeWindowOpt(0.2);
  clusteng.SetPositionOpt  (300);
  clusteng.SetBucketSize   (1);
      
  wiretrigger.SetWireNHitsMin    (6);
  wiretrigger.SetWireNChanMin    (2);
  wiretrigger.SetWireChanWidthMin(0);
  wiretrigger.SetWireSADCMin     (0);

  for (auto const& it: aaim) {
    int fNEvent = nEvent;
    int adc_threshold = it.first;
    SNAnaInputManager* im = it.second;
    
    if (fNEvent!=-1) { fNEvent = std::min(fNEvent,(int)im->GetEntries()); } 
    else             { fNEvent = im->GetEntries(); }

    effEnergyU[adc_threshold] = new TEfficiency(Form("Eff_Th%i", adc_threshold),
						"U Plane;Electron KE [MeV];Efficiency", 25, 0, 25);
    effEnergyV[adc_threshold] = new TEfficiency(Form("Eff_Th%i", adc_threshold),
						"V Plane;Electron KE [MeV];Efficiency", 25, 0, 25);
    effEnergyZ[adc_threshold] = new TEfficiency(Form("Eff_Th%i", adc_threshold),
						"Z Plane;Electron KE [MeV];Efficiency", 25, 0, 25);

    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);
      std::vector<WireHit*> vec_UWireHit;
      std::vector<WireHit*> vec_VWireHit;
      std::vector<WireHit*> vec_ZWireHit;

      // --- Make the wire hits and fill the vectors
      for (size_t j=0; j<im->Hit_View->size(); ++j) {
        WireHit* hit = new WireHit((*im->Hit_View)[j]       ,(*im->Hit_True_GenType)[j] ,(*im->Hit_Chan)[j],
                                   (*im->Hit_Time)[j]       ,(*im->Hit_SADC)[j]         ,(*im->Hit_RMS)[j],
                                   (*im->Hit_True_Energy)[j],(*im->Hit_True_EvEnergy)[j],(*im->Hit_True_MainTrID)[j],
                                   0.5, 0.5, 0.5,
                                   (*im->Hit_True_X)[j]     ,(*im->Hit_True_Y)[j]       ,(*im->Hit_True_Z)[j],
                                   0, (*im->Hit_True_nElec)[j]);

	if      (hit->GetHitView() == 0) { vec_UWireHit.push_back(hit); }
	else if (hit->GetHitView() == 1) { vec_VWireHit.push_back(hit); }
	else if (hit->GetHitView() == 2) { vec_ZWireHit.push_back(hit); }
	  
      } // Looping through hits
      
      // --- Run clustering algorithm over the wire hits
      std::vector<WireCluster*> vec_UWireCluster; clusteng.ClusterHits2(vec_UWireHit, vec_UWireCluster);
      std::vector<WireCluster*> vec_VWireCluster; clusteng.ClusterHits2(vec_VWireHit, vec_VWireCluster);
      std::vector<WireCluster*> vec_ZWireCluster; clusteng.ClusterHits2(vec_ZWireHit, vec_ZWireCluster);
      wiretrigger.IsTriggering(vec_UWireCluster);
      wiretrigger.IsTriggering(vec_VWireCluster);
      wiretrigger.IsTriggering(vec_ZWireCluster);


      bool selectedU = false; int nclusterU = 0; int nnoiseclusterU = 0;
      bool selectedV = false; int nclusterV = 0; int nnoiseclusterV = 0;
      bool selectedZ = false; int nclusterZ = 0; int nnoiseclusterZ = 0;

      for (size_t c=0; c<vec_UWireCluster.size(); ++c) {
        WireCluster* clust = vec_UWireCluster[c];
        if (clust->GetIsSelected()) {
          if (clust->GetType()) { selectedU = true; ++nclusterU; } 
      	  else                  { ++nnoiseclusterU;              }
        }
      }

      for (size_t c=0; c<vec_VWireCluster.size(); ++c) {
        WireCluster* clust = vec_VWireCluster[c];
        if (clust->GetIsSelected()) {
          if (clust->GetType()) { selectedV = true; ++nclusterV; } 
      	  else                  { ++nnoiseclusterV;              }
        }
      }

      for (size_t c=0; c<vec_ZWireCluster.size(); ++c) {
        WireCluster* clust = vec_ZWireCluster[c];
        if (clust->GetIsSelected()) {
          if (clust->GetType()) { selectedZ = true; ++nclusterZ; } 
      	  else                  { ++nnoiseclusterZ;              }
        }
      }

      // vec_ZWireCluster[0]->Print();

      // --- Fill some interesting plots
      double KE = (im->True_Bck_Energy->at(0) * 1000.0) - 0.511;
      effEnergyU[adc_threshold]->Fill(selectedU, KE);
      effEnergyV[adc_threshold]->Fill(selectedV, KE);
      effEnergyZ[adc_threshold]->Fill(selectedZ, KE);

      // --- Cleanup for memory management
      for (auto& it: vec_UWireHit)     { delete it; it = NULL; }
      for (auto& it: vec_VWireHit)     { delete it; it = NULL; }
      for (auto& it: vec_ZWireHit)     { delete it; it = NULL; }
      for (auto& it: vec_UWireCluster) { delete it; it = NULL; }
      for (auto& it: vec_VWireCluster) { delete it; it = NULL; }
      for (auto& it: vec_ZWireCluster) { delete it; it = NULL; }      

    } // end of Current Event Loop
  }

  TCanvas *c = new TCanvas("c", "c");
  c->Print((OutFileName + "[").c_str());
  PlotAll(effEnergyU);
  c->Print((OutFileName).c_str());

  PlotAll(effEnergyV);
  c->Print((OutFileName).c_str());

  PlotAll(effEnergyZ);
  c->Print((OutFileName).c_str());
  c->Print((OutFileName + "]").c_str());

} // End of Main
