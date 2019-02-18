#ifndef SIMPLEEFFICIENCYPLOT_HH
#define SIMPLEEFFICIENCYPLOT_HH 1

// This one runs the single electron efficiency
// from /dune/data/users/plasorak/anahist2.root (i.e. gaushit file)
// As function of the nhit cut that is applied to the trigger.


#include "Helper.h"
#include "EfficiencyPlotMarley.hh"

class SimpleEfficiencyPlot: public EfficiencyPlotMarley {

public:
  using EfficiencyPlotMarley::EfficiencyPlotMarley;

  void Run() {
    fInputFile = new TFile(fFileName.c_str(), "READ");
    fTree[0].push_back((TTree*) fInputFile->Get("snanagaushit/SNSimTree"));

    TTree* t = fTree[0].back();
            
    std::vector<float>  *ParticleE=NULL;
    std::vector<int>    *Hit_View=NULL;
    std::vector<int>    *Hit_Chan=NULL;
    std::vector<double> *Hit_Time=NULL;
    std::vector<double> *Hit_SADC=NULL;
    std::vector<double> *Hit_RMS =NULL;
    std::vector<int>    *Hit_True_GenType=NULL;
      
    t->SetBranchAddress("True_ENu_Lep",     &ParticleE);
    t->SetBranchAddress("Hit_View",         &Hit_View);
    t->SetBranchAddress("Hit_Chan",         &Hit_Chan);
    t->SetBranchAddress("Hit_Time",         &Hit_Time);
    t->SetBranchAddress("Hit_SADC",         &Hit_SADC);
    t->SetBranchAddress("Hit_RMS",          &Hit_RMS );
    t->SetBranchAddress("Hit_True_GenType", &Hit_True_GenType);
    
    c.SetTimeWindow   (20);
    c.SetChannelWidth (2);
    c.SetTimeWindowOpt(0.2);
    c.SetPositionOpt  (300);
    c.SetBucketSize   (1);
    trigger.SetWireNChanMin    (2);
    trigger.SetWireChanWidthMin(0);
    trigger.SetWireSADCMin     (0);
    
    for (int nhit=1; nhit<10; ++nhit) {
      std::cout << "Considering nhit cut = " << nhit << std::endl;
      fPlotClusterEfficiencyENU[0].push_back(new TEfficiency(Form("Eff_nhit%i",nhit),
                                                             Form("Clusters with nHit >= %i;E electron [MeV];SN#nu Electron Efficiency",nhit),
                                                             30,0,30));
      TEfficiency*& ef = fPlotClusterEfficiencyENU[0].back();
      trigger.SetWireNHitsMin(nhit);
      CurrentProg=0;
      for (int i=0; i<t->GetEntries()/10; ++i) {
        t->GetEntry(i);
        PrintProgress(i, t->GetEntries()/10);
        std::vector<WireHit*> hit_collection;

        for (size_t hit=0; hit<Hit_View->size(); ++hit) {
          WireHit* wh = new WireHit(Hit_View->at(hit),
                                    Hit_True_GenType->at(hit),
                                    Hit_Chan->at(hit),
                                    Hit_Time->at(hit),
                                    Hit_SADC->at(hit),
                                    Hit_RMS ->at(hit),
                                    0,//cTrDepositedE
                                    0,//cTrEPart
                                    0,//cTrIDPart
                                    0,//cRecPosX
                                    0,//cRecPosY
                                    0,//cRecPosZ
                                    0,//cTrPosX
                                    0,//cTrPosY
                                    0,//cTrPosZ
                                    0,//cMarleyIndex
                                    0 //cNelec
            );
          hit_collection.push_back(wh);
        }
        std::vector<WireCluster*> cluster_collection;
        c.ClusterHits2(hit_collection,cluster_collection);
        trigger.IsTriggering(cluster_collection);

        bool eff=0;
        for (auto const& it: cluster_collection) {
          if (it->GetIsTriggering()) {
            eff=1;
            break;
          }
        }
        ef->Fill(eff, ParticleE->at(0)*1000.);
        for (auto& it:cluster_collection){
          delete it;
          it = NULL;
        }
        cluster_collection.clear();
        for (auto& it: hit_collection) {
          delete it;
          it = NULL;
        }
      }
    }
    TCanvas c;
    c.Print("GaushitEff.pdf[");
    for (auto const& it: fPlotClusterEfficiencyENU[0]) {
      it->Draw();
      c.Print("GaushitEff.pdf");
    }
    
    c.Print("GaushitEff.pdf]");


  }
};

#endif
