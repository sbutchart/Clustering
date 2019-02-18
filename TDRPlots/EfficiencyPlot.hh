#ifndef EFFICIENCYPLOT_HH
#define EFFICIENCYPLOT_HH 1

#include <exception>
#include <iostream>
#include <regex>
#include <string>

#include "TCanvas.h"
#include "TEfficiency.h"
#include "TFile.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TTree.h"

#include "Helper.h"
#include "ClusterEngine.hh"
#include "Trigger.hh"

struct WrongFileException : public std::exception{
  const char * what () const throw () {
      return "The file isn't of the right format!";
    }
};
  
  
class EfficiencyPlot {
public:
  std::string fFileName;
  TFile * fInputFile;
  std::map<int, TLegend*>                  fLegend;
  TLegend*                                 fLegendGroup;
  std::map<int, std::vector<TEfficiency*>> fPlotClusterEfficiency;
  std::map<int, std::vector<TProfile*>>    fPlotNHitProfile;
  std::map<int, std::vector<TH2D*>>        fPlotNHitTH2D;
  std::map<int, std::vector<TH2D*>>        fPlotADCTH2D;
  std::map<int, std::vector<TProfile*>>    fPlotADCProfile;
  std::map<int, std::vector<TTree*>>       fTree;
  std::map<int, TH1D*>                     f10MeVEfficiency;
  std::map<int, std::vector<std::string>>  fTreeName;
  ClusterEngine c;
  TCanvas* fCanvas;
  SimpleTrigger trigger;

  EfficiencyPlot(std::string filename):
    fFileName(filename),
    fInputFile(NULL),
    fLegend(),
    fLegendGroup(NULL),
    fPlotClusterEfficiency(),
    fPlotNHitProfile(),
    fPlotNHitTH2D(),
    fTree(),
    f10MeVEfficiency(),
    fTreeName(),
    c(){
    c.SetTimeWindow   (20);
    c.SetChannelWidth (3);
    c.SetTimeWindowOpt(0.2);
    c.SetPositionOpt  (300);
    c.SetBucketSize   (1);
    trigger.SetWireNChanMin    (2);
    trigger.SetWireChanWidthMin(0);
    trigger.SetWireSADCMin     (0);
  }

  virtual void SetUpTreeName() {
    // fTreeName[0].push_back("arbitraryanafasthit10"   );
    // fTreeName[0].push_back("arbitraryanafasthit15"   );
    // fTreeName[0].push_back("arbitraryanafasthit20"   );
    // fTreeName[0].push_back("arbitraryanafasthit25"   );
    // fTreeName[0].push_back("arbitraryanafasthit30"   );
    // fTreeName[0].push_back("arbitraryanafasthit35"   );
    // fTreeName[0].push_back("arbitraryanafasthit40"   );
    
    // fTreeName[1].push_back("arbitraryanagaushit10"   );
    // fTreeName[1].push_back("arbitraryanagaushit15"   );
    // fTreeName[1].push_back("arbitraryanagaushit20"   );
    // fTreeName[1].push_back("arbitraryanagaushit25"   );
    // fTreeName[1].push_back("arbitraryanagaushit30"   );
    // fTreeName[1].push_back("arbitraryanagaushit35"   );
    // fTreeName[1].push_back("arbitraryanagaushit40"   );
    
    // fTreeName[2].push_back("arbitraryanatrigprim1000");
    fTreeName[2].push_back("arbitraryanatrigprim1500");
    fTreeName[2].push_back("arbitraryanatrigprim2000");
    fTreeName[2].push_back("arbitraryanatrigprim2500");
    fTreeName[2].push_back("arbitraryanatrigprim3000");
    fTreeName[2].push_back("arbitraryanatrigprim3500");
    fTreeName[2].push_back("arbitraryanatrigprim4000");
  }

  virtual void Run() {
  
    fInputFile = new TFile(fFileName.c_str(), "READ");
    SetUpTreeName();
  
    std::vector<double> *ParticleE=NULL;
    std::vector<int>    *ParticlePDG=NULL;
    std::vector<int>    *Hit_View=NULL;
    std::vector<int>    *Hit_Chan=NULL;
    std::vector<double> *Hit_Time=NULL;
    std::vector<double> *Hit_SADC=NULL;
    std::vector<double> *Hit_RMS =NULL;
    std::vector<bool>   *Hit_True_GenType=NULL;
    
    int NColHit=0;


    fCanvas = new TCanvas();
    fCanvas->Print("TDRPlots.pdf[");
    for (int nhitcut=1; nhitcut<=10; nhitcut++) {
      fLegendGroup = new TLegend(0.6,0.1,0.9,0.3);
      std::cout << "nhitcut " << nhitcut << std::endl;
      trigger.SetWireNHitsMin    (nhitcut);
      
      std::vector<int> color = getColors(0);
      for (auto const&treegroup: fTreeName) {
        int group=treegroup.first;
        fLegend[group] = new TLegend(0.6,0.1,0.9,0.4);

        int iter=0;
        f10MeVEfficiency[group] = new TH1D(Form("f10MeVEfficiency_nhitcut%i_%i",nhitcut,group), Form("n hit cut = %i;ADC Threshold;Efficiency @10MeV",nhitcut), 7, 7.5, 42.5);
        f10MeVEfficiency[group]->SetLineColor(color[group]);
        f10MeVEfficiency[group]->SetLineWidth(2);
        f10MeVEfficiency[group]->SetStats(0);
        std::string algorithm = treegroup.second[0];
        algorithm = std::regex_replace(algorithm, std::regex(R"([0-9])"), "");
        algorithm = std::regex_replace(algorithm, std::regex("arbitraryana"), "");
        fLegendGroup->AddEntry(f10MeVEfficiency[group], algorithm.c_str());

        fPlotClusterEfficiency[group].clear();
        fPlotNHitProfile[group]      .clear();
        fPlotNHitTH2D[group]         .clear();
        fPlotADCTH2D[group]          .clear();
        fPlotADCProfile[group]       .clear();
      
        for (auto const&tree: treegroup.second) {
          std::cout << "Trying the tree " << tree << std::endl;
          std::string title = tree;
          std::string entry = tree;
          title = std::regex_replace(title, std::regex(R"([0-9])"), "");
          title = std::regex_replace(title, std::regex("arbitraryana"), "");
          entry = std::regex_replace(entry, std::regex("arbitraryanagaushit"), "");
          entry = std::regex_replace(entry, std::regex("arbitraryanafasthit"), "");
          entry = std::regex_replace(entry, std::regex("arbitraryanatrigprim"), "");
          std::size_t found = entry.find("000");
          if (found!=std::string::npos) entry.erase(found+1, found+2);
          found = entry.find("00");
          if (found!=std::string::npos) entry.erase(found, found+2);
          int threshold = atoi(entry.c_str());

          entry.insert(0, "ADC threshold: ");
          fTree[group].push_back((TTree*) fInputFile->Get((tree+"/SNSimTree").c_str()));
          if (fTree[group].back() == NULL) {
            std::cout << "Tree doesn't exist " << tree << "/SNSimTree" << std::endl;
            throw WrongFileException();
          }
          fPlotClusterEfficiency[group].push_back(new TEfficiency((tree+Form("Eff%i",nhitcut)).c_str(),
                                                                  Form("n hit cut = %i;Electron energy [MeV];Clustering efficiency",nhitcut), 30, 0, 30));
          fPlotNHitProfile[group]      .push_back(new TProfile((tree+Form("Prof%i",nhitcut)).c_str(),
                                                               ";Electron energy [MeV];NHit", 30, 0, 30));
          fPlotNHitTH2D[group]         .push_back(new TH2D((tree+Form("TH2D%i",nhitcut)).c_str(),
                                                           (algorithm+entry+";Electron energy [MeV];NHit").c_str(), 30, 0, 30, 20, 0, 20));
          fPlotADCTH2D[group]          .push_back(new TH2D((tree+Form("ADC_TH2D%i",nhitcut)).c_str(),
                                                           (algorithm+entry+";Electron energy [MeV];Max hit ADC").c_str(), 30, 0, 30, 20, 0, 4000));
          fPlotADCProfile[group]       .push_back(new TProfile((tree+Form("ADC_Prof%i",nhitcut)).c_str(),
                                                               (algorithm+entry+";Electron energy [MeV];Max hit ADC").c_str(), 30, 0, 30));
          TTree* t = fTree[group].back();
          TEfficiency*& ef = fPlotClusterEfficiency[group].back();
          TProfile*& pr = fPlotNHitProfile[group].back();
          TH2D*& th = fPlotNHitTH2D[group].back();
          TProfile*& pradc = fPlotADCProfile[group].back();
          TH2D*& thadc = fPlotADCTH2D[group].back();
        
          t->SetBranchAddress("True_E",   &ParticleE);
          t->SetBranchAddress("True_Geant4_PDG", &ParticlePDG);
          t->SetBranchAddress("NColHit",         &NColHit);
          t->SetBranchAddress("Hit_View", &Hit_View);
          t->SetBranchAddress("Hit_Chan", &Hit_Chan);
          t->SetBranchAddress("Hit_Time", &Hit_Time);
          t->SetBranchAddress("Hit_SADC", &Hit_SADC);
          t->SetBranchAddress("Hit_RMS",  &Hit_RMS );
          t->SetBranchAddress("Hit_True_GenType", &Hit_True_GenType);
          pr->SetStats(0);
          th->SetStats(0);
          thadc->SetStats(0);
          pr->SetLineColor(color[iter]);
          ef->SetLineColor(color[iter]);

          TH1D* dummy = new TH1D();
          dummy->SetLineColor(color[iter]);
          ++iter;
          fLegend[group]->SetHeader(title.c_str(), "C");
          fLegend[group]->AddEntry(dummy, entry.c_str(), "L");
      
          for (int i=0; i<t->GetEntries(); ++i) {
            t->GetEntry(i);
            pr->Fill(ParticleE->at(0)*1000., NColHit);
            th->Fill(ParticleE->at(0)*1000., NColHit);

            std::vector<WireHit*> hit_collection;
            hit_collection.clear();
            int correctly_backtracked=0;
            double MaxADC=0;
            for (int hit=0; hit<NColHit; ++hit) {
              if (Hit_SADC->at(hit)>MaxADC)
                MaxADC=Hit_SADC->at(hit);
            
              WireHit* wh = new WireHit(Hit_View->at(hit),
                                        1,
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
              correctly_backtracked+=Hit_True_GenType->at(hit);
            }
            thadc->Fill(ParticleE->at(0)*1000.,MaxADC);
            pradc->Fill(ParticleE->at(0)*1000.,MaxADC);
            if (correctly_backtracked!=NColHit) std::cout << title << " " << entry << ": " << correctly_backtracked << " / " << NColHit << std::endl;
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
          
          }
          f10MeVEfficiency[group]->SetBinContent(f10MeVEfficiency[group]->FindBin(threshold), ef->GetEfficiency(ef->FindFixBin(10)));
          f10MeVEfficiency[group]->SetBinError  (f10MeVEfficiency[group]->FindBin(threshold), 0.5*(ef->GetEfficiencyErrorLow(ef->FindFixBin(10))+ef->GetEfficiencyErrorLow(ef->FindFixBin(10))));

        }
      }
      Plot(nhitcut==1);
    
    }
    fCanvas->Print("TDRPlots.pdf]");
  };
  

  
  ~EfficiencyPlot() {
    if(fInputFile)  fInputFile->Close();
    // for (auto& it: fPlotClusterEfficiency) {
    //   if (it) {
    //     delete it;
    //     it = NULL;
    //   }
    // }
    // fPlotClusterEfficiency.clear();
    
    // for (auto& it: fPlotNHitProfile) {
    //   if (it) {
    //     delete it;
    //     it = NULL;
    //   }
    // }
    // fPlotNHitProfile.clear();
    
    // for (auto& it: fPlotNHitTH2D) {
    //   if (it) {
    //     delete it;
    //     it = NULL;
    //   }
    // }
    // fPlotNHitTH2D.clear();
  };
  
  virtual void Plot(bool first=false) {
    if (first) {
      for (auto const& it: fPlotNHitProfile) {
        if (it.second.size() > 0) {
          it.second[0]->SetMinimum(0);
          it.second[0]->Draw();
          for (auto const& it2: it.second) it2->Draw("SAME");
          fLegend.at(it.first)->Draw();
          fCanvas->Print("TDRPlots.pdf");
        }
      }
      for (auto const& it: fPlotNHitTH2D) {
        for (size_t it2=0; it2<it.second.size(); ++it2) {
          it.second[it2]->Draw("COLZ");
          fPlotNHitProfile.at(it.first).at(it2)->SetLineColor(kRed);
          fPlotNHitProfile.at(it.first).at(it2)->SetLineWidth(2);
          fPlotNHitProfile.at(it.first).at(it2)->Draw("SAME");
          fCanvas->Print("TDRPlots.pdf");
        }
      }

      for (auto const& it: fPlotADCTH2D) {
        for (size_t it2=0; it2<it.second.size(); ++it2) {
          it.second[it2]->Draw("COLZ");
          fPlotADCProfile.at(it.first).at(it2)->SetLineColor(kRed);
          fPlotADCProfile.at(it.first).at(it2)->SetLineWidth(2);
          fPlotADCProfile.at(it.first).at(it2)->Draw("SAME");
          fCanvas->Print("TDRPlots.pdf");
        }
      }
    }
    for (auto const& it: fPlotClusterEfficiency) {
      if (it.second.size() > 0) {
        // it.second[0]->SetMinimum(0);
        // it.second[0]->SetMaximum(1.2);
        it.second[0]->Draw();
        for (auto const& it2: it.second) it2->Draw("SAME");
        fLegend.at(it.first)->SetX1NDC(0.6);
        fLegend.at(it.first)->SetX2NDC(0.9);
        fLegend.at(it.first)->SetY1NDC(0.1);
        fLegend.at(it.first)->SetY2NDC(0.4);
        gPad->SetGridx();
        gPad->SetGridy();

        fLegend.at(it.first)->Draw();
        fCanvas->Print("TDRPlots.pdf");
      }
    }
    
    try {
      if (f10MeVEfficiency.size() > 0) {
        gPad->SetGridx();
        gPad->SetGridy();
        f10MeVEfficiency.begin()->second->SetMaximum(1.2);
        f10MeVEfficiency.begin()->second->SetMinimum(0.0);
        f10MeVEfficiency.begin()->second->Draw();
        for (auto const& it: f10MeVEfficiency)
          it.second->Draw("SAME");
        gPad->RedrawAxis();
        fLegendGroup->Draw();
      }
      fCanvas->Print("TDRPlots.pdf");
    } catch(...) {
      std::cout << "Couldnt plot 5 MeV efficiency" << std::endl;
    }

  };

  
};

#endif
