#include "IO/ArbitraryAnaInputManager.hh"
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
  int nHitsMinValue=3;
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


  
  std::map<int, SNAnaInputManager*> aaim;
  aaim[1] = new SNAnaInputManager();
  aaim[1] -> SetInputTree(TreeName.c_str());

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }

  std::map<double, TEfficiency*> effEnergy;

  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  clusteng.SetTimeWindow   (20);
  clusteng.SetChannelWidth (2);
  clusteng.SetTimeWindowOpt(0.2);
  clusteng.SetPositionOpt  (50);
  clusteng.SetBucketSize   (1);
      
  SimpleGeom sg;
  std::vector<std::pair<int,double>> Threshold = {std::make_pair(1,   0.),
                                                  std::make_pair(2,   0.),
                                                  std::make_pair(3,   0.),
                                                  std::make_pair(4,   0.)};

  std::vector<TH2D*    > ADCDistance_th2d;
  std::vector<TProfile*> ADCDistance_prof;


  for (auto const& threshold: Threshold) {
    ADCDistance_th2d.push_back(new TH2D    (Form("ADCDistance_th2d_%i",threshold.first), ";Abs(X Position) [cm];ADC Cluster", 25, 0, 400, 30, 0, 6000));
    ADCDistance_prof.push_back(new TProfile(Form("ADCDistance_prof_%i",threshold.first), ";Abs(X Position) [cm];ADC Cluster", 25, 0, 400));
    
    ADCDistance_th2d.back().SetStats(0);
    ADCDistance_prof.back().SetStats(0);
    
    ADCDistance_prof.back().SetLineWidth(2.);
    ADCDistance_prof.back().SetLineColor(kBlack);
  } 

  int fNEvent = nEvent;
  
  for (auto const& it: aaim) {
    SNAnaInputManager* im = it.second;
    
    if (fNEvent!=-1) {
      fNEvent = std::min(fNEvent,(int)im->GetEntries());
    } else {
      fNEvent = im->GetEntries();
    }
    
    for (size_t iThreshold = 0; iThreshold<Threshold.size(); iThreshold++) {
      
      double ADCThreshold  = Threshold[iThreshold].first;
      double NHitThreshold = Threshold[iThreshold].second;
      
      wiretrigger.SetWireNHitsMin    (NHitThreshold);
       wiretrigger.SetWireSADCMin     (ADCThreshold);
      wiretrigger.SetWireNChanMin    (2);
      wiretrigger.SetWireChanWidthMin(0);
      
      double n_background_cluster = 0;
      double event_counter = 0;

      CurrentProg=0;
      for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
        PrintProgress(CurrentEvent, fNEvent);
        im->GetEntry(CurrentEvent);
      
        if((*im->True_ENu_Lep)[0]*1000.>10.)
          continue;

        // if(abs((*im->True_VertX)[0]) < 15 || abs((*im->True_VertX)[0]) > 50)
        //   continue;
      
        std::vector<WireHit*> vec_WireHit;
        im->GetWireHits(vec_WireHit);
        // std::vector<OpticalHit*> vec_OptiHit;
        // im->GetOpticalHits(vec_OptiHit);
        
        bool selected = false;
        std::vector<WireCluster*> vec_WireCluster;
        clusteng.ClusterHits2      (vec_WireHit, vec_WireCluster);
        wiretrigger.IsTriggering(vec_WireCluster);

        for (size_t c=0; c<vec_WireCluster.size(); ++c) {
          WireCluster* wire_clust = vec_WireCluster[c];
          if (wire_clust->GetIsTriggering() && wire_clust->GetHitSumADC() > itadc) {
            if (wire_clust->GetType()) {
              ADCDistance_th2d->Fill(abs((*im->True_VertX)[0]), wire_clust->GetSumADC());
              ADCDistance_prof->Fill(abs((*im->True_VertX)[0]), wire_clust->GetSumADC());
            }
          }
        }

        double KE = im->True_ENu_Lep->at(0) * 1000. - 0.511;
        effEnergy[itadc]->Fill(selected, KE);
        event_counter++;
        for (auto& it: vec_WireHit)     { delete it; it = NULL; }
        for (auto& it: vec_WireCluster) { delete it; it = NULL; }
        vec_WireHit    .clear();
        vec_WireCluster.clear();
      }
    }
  }

  hfile.cd();
  for (auto const& it: ADCDistance_th2d) {
    it.second->Write();
  }
  for (auto const& it: ADCDistance_prof) {
    it.second->Write();
  }

  TCanvas c;
  c.Print((OutFileName+".pdf[").c_str());
  for (size_t it=0; it<ADCDistance_th2d.size(); it++) {
    ADCDistance_th2d->Draw("COLZ");
    ADCDistance_prof->Draw("SAME");
    c.Print((OutFileName+".pdf").c_str());  
  }

c.Print((OutFileName+".pdf]").c_str());
  

}

