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

#include <iostream>
#include <unistd.h>

int main (int argc, char** argv) {
  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  std::string InFileName ="";
  std::string OutFileName="";
  int nEvent=-1;
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
  
  std::map<std::string,ArbitraryAnaInputManager*> aaim;
  aaim["FastHit 10" ] = new ArbitraryAnaInputManager();
  aaim["FastHit 15" ] = new ArbitraryAnaInputManager();
  aaim["FastHit 20" ] = new ArbitraryAnaInputManager();
  aaim["FastHit 25" ] = new ArbitraryAnaInputManager();
  aaim["FastHit 30" ] = new ArbitraryAnaInputManager();
  aaim["FastHit 35" ] = new ArbitraryAnaInputManager();
  aaim["FastHit 40" ] = new ArbitraryAnaInputManager();
  aaim["GausHit 10" ] = new ArbitraryAnaInputManager();
  aaim["GausHit 15" ] = new ArbitraryAnaInputManager();
  aaim["GausHit 20" ] = new ArbitraryAnaInputManager();
  aaim["GausHit 25" ] = new ArbitraryAnaInputManager();
  aaim["GausHit 30" ] = new ArbitraryAnaInputManager();
  aaim["GausHit 35" ] = new ArbitraryAnaInputManager();
  aaim["GausHit 40" ] = new ArbitraryAnaInputManager();
  aaim["TrigPrim 10"] = new ArbitraryAnaInputManager();
  aaim["TrigPrim 15"] = new ArbitraryAnaInputManager();
  aaim["TrigPrim 20"] = new ArbitraryAnaInputManager();
  aaim["TrigPrim 25"] = new ArbitraryAnaInputManager();
  aaim["TrigPrim 30"] = new ArbitraryAnaInputManager();
  aaim["TrigPrim 35"] = new ArbitraryAnaInputManager();
  aaim["TrigPrim 40"] = new ArbitraryAnaInputManager();

  aaim["FastHit 10" ]->SetInputTree("arbitraryanafasthit10/SNSimTree");
  aaim["FastHit 15" ]->SetInputTree("arbitraryanafasthit15/SNSimTree");
  aaim["FastHit 20" ]->SetInputTree("arbitraryanafasthit20/SNSimTree");
  aaim["FastHit 25" ]->SetInputTree("arbitraryanafasthit25/SNSimTree");
  aaim["FastHit 30" ]->SetInputTree("arbitraryanafasthit30/SNSimTree");
  aaim["FastHit 35" ]->SetInputTree("arbitraryanafasthit35/SNSimTree");
  aaim["FastHit 40" ]->SetInputTree("arbitraryanafasthit40/SNSimTree");
  aaim["GausHit 10" ]->SetInputTree("arbitraryanagaushit10/SNSimTree");
  aaim["GausHit 15" ]->SetInputTree("arbitraryanagaushit15/SNSimTree");
  aaim["GausHit 20" ]->SetInputTree("arbitraryanagaushit20/SNSimTree");
  aaim["GausHit 25" ]->SetInputTree("arbitraryanagaushit25/SNSimTree");
  aaim["GausHit 30" ]->SetInputTree("arbitraryanagaushit30/SNSimTree");
  aaim["GausHit 35" ]->SetInputTree("arbitraryanagaushit35/SNSimTree");
  aaim["GausHit 40" ]->SetInputTree("arbitraryanagaushit40/SNSimTree");
  aaim["TrigPrim 10"]->SetInputTree("arbitraryanatrigprim1000/SNSimTree");
  aaim["TrigPrim 15"]->SetInputTree("arbitraryanatrigprim1500/SNSimTree");
  aaim["TrigPrim 20"]->SetInputTree("arbitraryanatrigprim2000/SNSimTree");
  aaim["TrigPrim 25"]->SetInputTree("arbitraryanatrigprim2500/SNSimTree");
  aaim["TrigPrim 30"]->SetInputTree("arbitraryanatrigprim3000/SNSimTree");
  aaim["TrigPrim 35"]->SetInputTree("arbitraryanatrigprim3500/SNSimTree");
  aaim["TrigPrim 40"]->SetInputTree("arbitraryanatrigprim4000/SNSimTree");

  std::map<std::string, int> threshold;
  threshold["FastHit 10" ] = 10;
  threshold["FastHit 15" ] = 15;
  threshold["FastHit 20" ] = 20;
  threshold["FastHit 25" ] = 25;
  threshold["FastHit 30" ] = 30;
  threshold["FastHit 35" ] = 35;
  threshold["FastHit 40" ] = 40;
  threshold["GausHit 10" ] = 10;
  threshold["GausHit 15" ] = 15;
  threshold["GausHit 20" ] = 20;
  threshold["GausHit 25" ] = 25;
  threshold["GausHit 30" ] = 30;
  threshold["GausHit 35" ] = 35;
  threshold["GausHit 40" ] = 40;
  threshold["TrigPrim 10"] = 10;
  threshold["TrigPrim 15"] = 15;
  threshold["TrigPrim 20"] = 20;
  threshold["TrigPrim 25"] = 25;
  threshold["TrigPrim 30"] = 30;
  threshold["TrigPrim 35"] = 35;
  threshold["TrigPrim 40"] = 40;

  std::map<std::string, int> type;
  type["FastHit 10" ] = 0;
  type["FastHit 15" ] = 0;
  type["FastHit 20" ] = 0;
  type["FastHit 25" ] = 0;
  type["FastHit 30" ] = 0;
  type["FastHit 35" ] = 0;
  type["FastHit 40" ] = 0;
  type["GausHit 10" ] = 1;
  type["GausHit 15" ] = 1;
  type["GausHit 20" ] = 1;
  type["GausHit 25" ] = 1;
  type["GausHit 30" ] = 1;
  type["GausHit 35" ] = 1;
  type["GausHit 40" ] = 1;
  type["TrigPrim 10"] = 2;
  type["TrigPrim 15"] = 2;
  type["TrigPrim 20"] = 2;
  type["TrigPrim 25"] = 2;
  type["TrigPrim 30"] = 2;
  type["TrigPrim 35"] = 2;
  type["TrigPrim 40"] = 2;

  std::map<int, std::string> Algo;
  Algo[0] = "FastHit" ;
  Algo[1] = "GausHit" ;
  Algo[2] = "TrigPrim";

  
  for (auto& it: aaim) {
    std::cout <<  "BLAAA " << it.first << std::endl;
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }
  std::map<int, TEfficiency*> EfficiencyThreshold;
  std::map<int, TProfile*>    nHit;
  std::map<int, TProfile*>    nCluster;
  std::map<int, TH1D*>        nTH1Hit;
  TH1D*        NeutronSpectrum = new TH1D("neutron", ";KE [MeV];Events", 100, 0, 10);
  std::map<int, TH2D*>        nTH2Hit;
  std::map<int, TH2D*>        nTH2Cluster;

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
    std::string hitfinder = it.first;
    int type_hit = type[hitfinder];
    int adc_threshold = threshold[hitfinder];
    ArbitraryAnaInputManager* im = it.second;
    
    if (fNEvent!=-1) {
      fNEvent = std::min(fNEvent,(int)im->GetEntries());
    } else {
      fNEvent = im->GetEntries();
    }
    if (!EfficiencyThreshold[type_hit]) {
      std::cout << type_hit << std::endl;
      std::cout << (Algo[type_hit]+";ADC Threshold;Efficiency") << std::endl;
      EfficiencyThreshold[type_hit] = new TEfficiency(Form("Eff_Type%i", type_hit),
                                                      (Algo[type_hit]+";ADC Threshold;Efficiency").c_str(), 7, 7.5, 42.5);
    }
    if (!nHit[type_hit])
      nHit[type_hit] = new TProfile(Form("nHit_Type%i", type_hit),
                                    (Algo[type_hit]+";ADC Threshold;nHit").c_str(), 7, 7.5, 42.5);
    if (!nTH2Hit[type_hit])
      nTH2Hit[type_hit] = new TH2D(Form("nTH2Hit_Type%i", type_hit),
                                         (Algo[type_hit]+";ADC Threshold;nHit").c_str(), 7, 7.5, 42.5, 20, 0, 20);
    if (!nCluster[type_hit])
      nCluster[type_hit] = new TProfile(Form("nCluster_Type%i", type_hit),
                                        (Algo[type_hit]+";ADC Threshold;nCluster").c_str(), 7, 7.5, 42.5);
    if (!nTH2Cluster[type_hit])
      nTH2Cluster[type_hit] = new TH2D(Form("nTH2Cluster_Type%i", type_hit),
                                       (Algo[type_hit]+";ADC Threshold;nCluster").c_str(), 7, 7.5, 42.5,  20, 0, 20);
    if (!nTH1Hit[adc_threshold])
      nTH1Hit[adc_threshold] = new TH1D(Form("nTH1Hit_THreshold%i", adc_threshold),
                                       ";nHit TrigPrim @20ADC;Event",  20, 0, 20);

    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);
      std::vector<WireHit*> vec_WireHit;
      for (size_t j=0; j<im->Hit_View->size(); ++j) {
        WireHit* hit = new WireHit((*im->Hit_View)[j],        (*im->Hit_True_GenType)[j],  (*im->Hit_Chan)[j],
                                   (*im->Hit_Time)[j],        (*im->Hit_SADC)[j],          (*im->Hit_RMS)[j],
                                   0.5, 0.5, 5,
                                   0.5, 0.5, 0.5,
                                   0.5, 0.5, 0.5,
                                   0, 0);
        vec_WireHit.push_back(hit);
      }
      bool selected = false;
      int ncluster = 0;
      std::vector<WireCluster*> vec_WireCluster;
      clusteng.ClusterHits2(vec_WireHit, vec_WireCluster);
          
      for (size_t c=0; c<vec_WireCluster.size(); ++c) {
        WireCluster* clust = vec_WireCluster[c];
        if (wiretrigger.IsTriggering(clust)) {
          selected = true;
          ++ncluster;
          std::cout << "selected" << std::endl;
        }
      }
      if (type_hit == 2)
        nTH1Hit[adc_threshold]->Fill(im->Hit_View->size());

      if (type_hit == 2 && adc_threshold == 20)
        //std::cout << (*im->True_Prim_Energy)[0]- 0.939565 << std::endl;
        NeutronSpectrum->Fill(((*im->True_Prim_Energy)[0]-0.939565)*1000.);
      
      EfficiencyThreshold[type_hit]->Fill(selected, adc_threshold);
      nHit               [type_hit]->Fill(adc_threshold, im->Hit_View->size());
      nTH2Hit            [type_hit]->Fill(adc_threshold, im->Hit_View->size());
      nCluster           [type_hit]->Fill(adc_threshold, ncluster     );
      nTH2Cluster        [type_hit]->Fill(adc_threshold, ncluster     );
      
      for (auto& it: vec_WireHit) {
        delete it;
        it = NULL;
      }
      vec_WireHit.clear();
    }
  }

  for (auto& it: EfficiencyThreshold) {
    nHit       [it.first]->SetStats(0);
    nCluster   [it.first]->SetStats(0);
    nTH2Hit    [it.first]->SetStats(0);
    nTH2Cluster[it.first]->SetStats(0);
  }

  TCanvas c;
  c.Print((OutFileName+"[").c_str());
  gPad->SetGridx();
  gPad->SetGridy();

  gPad->SetLeftMargin (1.7*gPad->GetLeftMargin ());
  gPad->SetRightMargin(1.7*gPad->GetRightMargin());
  for (auto const& it: EfficiencyThreshold) {
    it.second->Draw();
    gPad->Update(); 
    auto graph = it.second->GetPaintedGraph(); 
    graph->SetMinimum(0);
    // graph->SetMaximum(1); 
    c.Print(OutFileName.c_str());
  }
  for (auto const& it: nHit) {
    it.second->Draw();
    c.Print(OutFileName.c_str());
  }
  for (auto const& it: nCluster) {
    it.second->Draw();
    c.Print(OutFileName.c_str());
  }
  for (auto const& it: nTH2Hit) {
    it.second->Draw("COLZ");
    c.Print(OutFileName.c_str());
  }
  for (auto const& it: nTH2Cluster) {
    it.second->Draw("COLZ");
    c.Print(OutFileName.c_str());
  }
  for (auto const& it: nTH1Hit) {
    it.second->Draw();
    c.Print(OutFileName.c_str());
  }
  
  NeutronSpectrum->Draw();
    c.Print(OutFileName.c_str());
  c.Print((OutFileName+"]").c_str());
  
}


