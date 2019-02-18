#ifndef HITCOMPARISON_HH
#define HITCOMPARISON_HH 1

#include <exception>
#include <iostream>
#include <limits>
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

class HitComparison{
public:
  std::string fFileName;
  TFile * fInputFile;
  std::map<std::string, std::vector<TEfficiency*>> fPlotClusterEfficiencyX;
  std::map<std::string, std::vector<TEfficiency*>> fPlotClusterEfficiencyY;
  std::map<std::string, std::vector<TEfficiency*>> fPlotClusterEfficiencyZ;
  std::map<std::string, std::vector<TProfile*>>    fPlotNHitProfileX;
  std::map<std::string, std::vector<TProfile*>>    fPlotNHitProfileY;
  std::map<std::string, std::vector<TProfile*>>    fPlotNHitProfileZ;
  std::map<std::string, std::vector<TProfile*>>    fPlotSADCProfileX;
  std::map<std::string, std::vector<TProfile*>>    fPlotSADCProfileY;
  std::map<std::string, std::vector<TProfile*>>    fPlotSADCProfileZ;
  std::map<std::string, std::vector<TProfile*>>    fPlotTimeProfileX;
  std::map<std::string, std::vector<TProfile*>>    fPlotTimeProfileY;
  std::map<std::string, std::vector<TProfile*>>    fPlotTimeProfileZ;
  std::map<std::string, std::vector<TH2D*>>        fPlotNHitTH2DX;
  std::map<std::string, std::vector<TH2D*>>        fPlotNHitTH2DY;
  std::map<std::string, std::vector<TH2D*>>        fPlotNHitTH2DZ;
  std::map<std::string, std::vector<TH2D*>>        fPlotSADCTH2DX;
  std::map<std::string, std::vector<TH2D*>>        fPlotSADCTH2DY;
  std::map<std::string, std::vector<TH2D*>>        fPlotSADCTH2DZ;
  std::map<std::string, std::vector<TH2D*>>        fPlotTimeTH2DX;
  std::map<std::string, std::vector<TH2D*>>        fPlotTimeTH2DY;
  std::map<std::string, std::vector<TH2D*>>        fPlotTimeTH2DZ;          
  std::map<std::string, std::vector<TTree*>>       fTree;
  std::map<std::string, std::vector<std::string>>  fTreeName;
  TLegend* fLegend;
  ClusterEngine c;
  SimpleTrigger trigger;

  HitComparison(std::string filename):
    fFileName(filename),
    fInputFile(),
    fPlotClusterEfficiencyX(),
    fPlotClusterEfficiencyY(),
    fPlotClusterEfficiencyZ(),
    fPlotNHitProfileX(),
    fPlotNHitProfileY(),
    fPlotNHitProfileZ(),
    fPlotSADCProfileX(),
    fPlotSADCProfileY(),
    fPlotSADCProfileZ(),
    fPlotTimeProfileX(),
    fPlotTimeProfileY(),
    fPlotTimeProfileZ(),
    fPlotNHitTH2DX(),
    fPlotNHitTH2DY(),
    fPlotNHitTH2DZ(),
    fPlotSADCTH2DX(),
    fPlotSADCTH2DY(),
    fPlotSADCTH2DZ(),
    fPlotTimeTH2DX(),
    fPlotTimeTH2DY(),
    fPlotTimeTH2DZ(),       
    fTree(),
    fTreeName(),
    fLegend(),
    c(){
    c.SetTimeWindow   (20);
    c.SetChannelWidth (3);
    c.SetTimeWindowOpt(0.2);
    c.SetPositionOpt  (300);
    c.SetBucketSize   (1);
    trigger.SetWireNChanMin    (2);
    trigger.SetWireChanWidthMin(0);
    trigger.SetWireSADCMin     (0);
    trigger.SetWireNHitsMin    (6);
  }

  virtual void SetUpTreeName() {
    fTreeName["gaushit"] .push_back("arbitraryanagaushit");
    //fTreeName["trigprim"].push_back("arbitraryanatrigprim1000");
    fTreeName["trigprim"].push_back("arbitraryanatrigprim1500");
    fTreeName["trigprim"].push_back("arbitraryanatrigprim1800");
    fTreeName["trigprim"].push_back("arbitraryanatrigprim2000");
    // fTreeName["trigprim"].push_back("arbitraryanatrigprim2500");
    // fTreeName["trigprim"].push_back("arbitraryanatrigprim3000");
    // fTreeName["trigprim"].push_back("arbitraryanatrigprim3500");
    // fTreeName["trigprim"].push_back("arbitraryanatrigprim4000");
  }
  template <class T>
  void PlotShape(std::map<T,std::vector<TProfile*>>& map_, TCanvas* c, std::string name) const{
    double Max=std::numeric_limits<double>::min();
    double Min=std::numeric_limits<double>::max();
    for (auto const& it:map_) {
      for (auto const& it2: it.second) {
        it2->Scale(1./it2->Integral());
        if (Max < it2->GetBinContent(it2->GetMaximumBin())) Max = it2->GetBinContent(it2->GetMaximumBin());
        if (Min > it2->GetBinContent(it2->GetMinimumBin())) Min = it2->GetBinContent(it2->GetMinimumBin());
      }
    }
    map_.begin()->second.at(0)->SetMaximum(Max*1.2);
    map_.begin()->second.at(0)->SetMinimum(Min*0.8);
    map_.begin()->second.at(0)->Draw();
    for (auto const& it:map_) {
      for (auto const& it2: it.second) {
        if (it2 == map_.begin()->second.at(0)) continue;
        it2->Draw("SAME");
      }
    }
    c->Print(name.c_str());
  }
  template <class T>
  void Plot(std::map<T,std::vector<TProfile*>>& map_, TCanvas* c, std::string name) const{
    double Max=std::numeric_limits<double>::min();
    double Min=std::numeric_limits<double>::max();
    for (auto const& it:map_) {
      for (auto const& it2: it.second) {
        if (Max < it2->GetMaximum()) Max = it2->GetMaximum();
        if (Min > it2->GetMinimum()) Min = it2->GetMinimum();
        //it2->Draw();
        //c->Print(name.c_str());
      }
    }
    map_.begin()->second.at(0)->SetMaximum(Max*1.2);
    map_.begin()->second.at(0)->SetMinimum(Min*0.8);
    map_.begin()->second.at(0)->Draw();
    for (auto const& it:map_) {
      for (auto const& it2: it.second) {
        if (it2 == map_.begin()->second.at(0)) continue;
        it2->Draw("SAME");
      }
    }
    c->Print(name.c_str());
  }
  
  template <class T>
  void Plot(std::map<T,std::vector<TH2D*>> map_, TCanvas* c, std::string name) const {
    for (auto const& it:map_) {
      for (auto const& it2: it.second) {
        it2->Draw("COLZ");
        c->Print(name.c_str());
      }
    }
  }

  virtual void Run() {
  
    fInputFile = new TFile(fFileName.c_str(), "READ");
    SetUpTreeName();
  
    std::vector<double> *ParticleE=NULL;
    std::vector<double> *ParticleX=NULL;
    std::vector<double> *ParticleY=NULL;
    std::vector<double> *ParticleZ=NULL;
    std::vector<int>    *ParticlePDG=NULL;
    std::vector<int>    *Hit_View=NULL;
    std::vector<int>    *Hit_Chan=NULL;
    std::vector<double> *Hit_Time=NULL;
    std::vector<double> *Hit_SADC=NULL;
    std::vector<double> *Hit_RMS =NULL;
    std::vector<bool>   *Hit_True_GenType=NULL;
    
    int NColHit=0;
    int iter=0;
    fLegend = new TLegend(0.1,0.1,0.9,0.9);

    std::vector<int> color = getColors(0);
    for (auto const&treegroup: fTreeName) {
      std::string group=treegroup.first;
     
      for (auto const&tree: treegroup.second) {
        ++iter;

        fTree[group].push_back((TTree*) fInputFile->Get((tree+"/SNSimTree").c_str()));
        if (fTree[group].back() == NULL) {
          std::cerr << tree+"/SNSimTree" << " doesnt exist in " << fFileName << std::endl;
          throw WrongFileException();
        }
        std::cout << tree << std::endl;
        fPlotClusterEfficiencyX[group].push_back(new TEfficiency((tree+"XEff").c_str(), ";Proj X;Clustering efficiency", 10, -1., 1.));
        fPlotClusterEfficiencyY[group].push_back(new TEfficiency((tree+"YEff").c_str(), ";Proj Y;Clustering efficiency", 10, -1., 1.));
        fPlotClusterEfficiencyZ[group].push_back(new TEfficiency((tree+"ZEff").c_str(), ";Proj Z;Clustering efficiency", 10, -1., 1.));
        fPlotNHitProfileX      [group].push_back(new TProfile((tree+"_nhit_XProf").c_str(), ";Proj X;NHit", 10, -1., 1.));
        fPlotNHitProfileY      [group].push_back(new TProfile((tree+"_nhit_YProf").c_str(), ";Proj Y;NHit", 10, -1., 1.));
        fPlotNHitProfileZ      [group].push_back(new TProfile((tree+"_nhit_ZProf").c_str(), ";Proj Z;NHit", 10, -1., 1.));
        fPlotSADCProfileX      [group].push_back(new TProfile((tree+"_sadc_XProf").c_str(), ";Proj X;SADC", 10, -1., 1.));
        fPlotSADCProfileY      [group].push_back(new TProfile((tree+"_sadc_YProf").c_str(), ";Proj Y;SADC", 10, -1., 1.));
        fPlotSADCProfileZ      [group].push_back(new TProfile((tree+"_sadc_ZProf").c_str(), ";Proj Z;SADC", 10, -1., 1.));
        fPlotTimeProfileX      [group].push_back(new TProfile((tree+"_time_XProf").c_str(), ";Proj X;Time", 10, -1., 1.));
        fPlotTimeProfileY      [group].push_back(new TProfile((tree+"_time_YProf").c_str(), ";Proj Y;Time", 10, -1., 1.));
        fPlotTimeProfileZ      [group].push_back(new TProfile((tree+"_time_ZProf").c_str(), ";Proj Z;Time", 10, -1., 1.));
        fPlotNHitTH2DX         [group].push_back(new TH2D((tree+"_nhit_XTH2D").c_str(), ";ProjX;NHit", 10, -1., 1., 10, 0, 10));
        fPlotNHitTH2DY         [group].push_back(new TH2D((tree+"_nhit_YTH2D").c_str(), ";ProjY;NHit", 10, -1., 1., 10, 0, 10));
        fPlotNHitTH2DZ         [group].push_back(new TH2D((tree+"_nhit_ZTH2D").c_str(), ";ProjZ;NHit", 10, -1., 1., 10, 0, 10));
        fPlotSADCTH2DX         [group].push_back(new TH2D((tree+"_sadc_XTH2D").c_str(), ";ProjX;SADC", 10, -1., 1., 10, 0, 1000));
        fPlotSADCTH2DY         [group].push_back(new TH2D((tree+"_sadc_YTH2D").c_str(), ";ProjY;SADC", 10, -1., 1., 10, 0, 1000));
        fPlotSADCTH2DZ         [group].push_back(new TH2D((tree+"_sadc_ZTH2D").c_str(), ";ProjZ;SADC", 10, -1., 1., 10, 0, 1000));
        fPlotTimeTH2DX         [group].push_back(new TH2D((tree+"_time_XTH2D").c_str(), ";ProjX;Time", 10, -1., 1., 10, 0, 1000));
        fPlotTimeTH2DY         [group].push_back(new TH2D((tree+"_time_YTH2D").c_str(), ";ProjY;Time", 10, -1., 1., 10, 0, 1000));
        fPlotTimeTH2DZ         [group].push_back(new TH2D((tree+"_time_ZTH2D").c_str(), ";ProjZ;Time", 10, -1., 1., 10, 0, 1000));

        TTree* t = fTree[group].back();
        t->SetBranchAddress("True_Geant4_E",    &ParticleE);
        t->SetBranchAddress("True_Geant4_PDG",  &ParticlePDG);
        t->SetBranchAddress("True_Dirx",        &ParticleX);
        t->SetBranchAddress("True_Diry",        &ParticleY);
        t->SetBranchAddress("True_Dirz",        &ParticleZ);
        t->SetBranchAddress("NColHit",          &NColHit );
        t->SetBranchAddress("Hit_View",         &Hit_View);
        t->SetBranchAddress("Hit_Chan",         &Hit_Chan);
        t->SetBranchAddress("Hit_Time",         &Hit_Time);
        t->SetBranchAddress("Hit_SADC",         &Hit_SADC);
        t->SetBranchAddress("Hit_RMS",          &Hit_RMS );
        t->SetBranchAddress("Hit_True_GenType", &Hit_True_GenType);
        fPlotClusterEfficiencyX[group].back()->SetLineColor(color[iter]);
        fPlotClusterEfficiencyY[group].back()->SetLineColor(color[iter]);
        fPlotClusterEfficiencyZ[group].back()->SetLineColor(color[iter]);
        fPlotNHitProfileX      [group].back()->SetLineColor(color[iter]);
        fPlotNHitProfileY      [group].back()->SetLineColor(color[iter]);
        fPlotNHitProfileZ      [group].back()->SetLineColor(color[iter]);
        fPlotSADCProfileX      [group].back()->SetLineColor(color[iter]);
        fPlotSADCProfileY      [group].back()->SetLineColor(color[iter]);
        fPlotSADCProfileZ      [group].back()->SetLineColor(color[iter]);
        fPlotTimeProfileX      [group].back()->SetLineColor(color[iter]);
        fPlotTimeProfileY      [group].back()->SetLineColor(color[iter]);
        fPlotTimeProfileZ      [group].back()->SetLineColor(color[iter]);

        fPlotClusterEfficiencyX[group].back()->SetLineWidth(2);
        fPlotClusterEfficiencyY[group].back()->SetLineWidth(2);
        fPlotClusterEfficiencyZ[group].back()->SetLineWidth(2);
        fPlotNHitProfileX      [group].back()->SetLineWidth(2);
        fPlotNHitProfileY      [group].back()->SetLineWidth(2);
        fPlotNHitProfileZ      [group].back()->SetLineWidth(2);
        fPlotSADCProfileX      [group].back()->SetLineWidth(2);
        fPlotSADCProfileY      [group].back()->SetLineWidth(2);
        fPlotSADCProfileZ      [group].back()->SetLineWidth(2);
        fPlotTimeProfileX      [group].back()->SetLineWidth(2);
        fPlotTimeProfileY      [group].back()->SetLineWidth(2);
        fPlotTimeProfileZ      [group].back()->SetLineWidth(2);

        fPlotNHitProfileX      [group].back()->SetStats(0);
        fPlotNHitProfileY      [group].back()->SetStats(0);
        fPlotNHitProfileZ      [group].back()->SetStats(0);
        fPlotSADCProfileX      [group].back()->SetStats(0);
        fPlotSADCProfileY      [group].back()->SetStats(0);
        fPlotSADCProfileZ      [group].back()->SetStats(0);
        fPlotTimeProfileX      [group].back()->SetStats(0);
        fPlotTimeProfileY      [group].back()->SetStats(0);
        fPlotTimeProfileZ      [group].back()->SetStats(0);
        
        std::string entry = std::regex_replace(tree, std::regex("arbitraryana"), "");
        entry = std::regex_replace(entry, std::regex("00"), "");
        fLegend->AddEntry(fPlotNHitProfileZ[group].back(), entry.c_str());
        for (int i=0; i<t->GetEntries(); ++i) {
          t->GetEntry(i);
          if(ParticleX->at(0)<-1 || ParticleX->at(0)>1.) {std::cout << "VertX " << ParticleX->at(0) << std::endl;}
          if(ParticleY->at(0)<-1 || ParticleY->at(0)>1.) {std::cout << "VertY " << ParticleY->at(0) << std::endl;}
          if(ParticleZ->at(0)<-1 || ParticleZ->at(0)>1.) {std::cout << "VertZ " << ParticleZ->at(0) << std::endl;}
          fPlotNHitProfileX[group].back()->Fill(ParticleX->at(0), Hit_View->size());
          fPlotNHitProfileY[group].back()->Fill(ParticleY->at(0), Hit_View->size());
          fPlotNHitProfileZ[group].back()->Fill(ParticleZ->at(0), Hit_View->size());
          fPlotNHitTH2DX   [group].back()->Fill(ParticleX->at(0), Hit_View->size());
          fPlotNHitTH2DY   [group].back()->Fill(ParticleY->at(0), Hit_View->size());
          fPlotNHitTH2DZ   [group].back()->Fill(ParticleZ->at(0), Hit_View->size());


          double SADC=0;
          for (size_t i=0; i<Hit_View->size(); ++i) {
            SADC += Hit_SADC->at(i);
            fPlotTimeProfileX[group].back()->Fill(ParticleX->at(0), Hit_Time->at(i));
            fPlotTimeProfileY[group].back()->Fill(ParticleY->at(0), Hit_Time->at(i));
            fPlotTimeProfileZ[group].back()->Fill(ParticleZ->at(0), Hit_Time->at(i));
            fPlotTimeTH2DX   [group].back()->Fill(ParticleX->at(0), Hit_Time->at(i));
            fPlotTimeTH2DY   [group].back()->Fill(ParticleY->at(0), Hit_Time->at(i));
            fPlotTimeTH2DZ   [group].back()->Fill(ParticleZ->at(0), Hit_Time->at(i));
          }

          fPlotSADCTH2DX   [group].back()->Fill(ParticleX->at(0), SADC);
          fPlotSADCTH2DY   [group].back()->Fill(ParticleY->at(0), SADC);
          fPlotSADCTH2DZ   [group].back()->Fill(ParticleZ->at(0), SADC);
          fPlotSADCProfileX[group].back()->Fill(ParticleX->at(0), SADC);
          fPlotSADCProfileY[group].back()->Fill(ParticleY->at(0), SADC);
          fPlotSADCProfileZ[group].back()->Fill(ParticleZ->at(0), SADC);

          std::vector<WireHit*> hit_collection;
          hit_collection.clear();
          int correctly_backtracked=0;
          for (int hit=0; hit<NColHit; ++hit) {
            WireHit* wh = new WireHit(Hit_View->at(hit),
                                      1,
                                      Hit_Chan->at(hit),
                                      Hit_Time->at(hit),
                                      Hit_SADC->at(hit),
                                      Hit_RMS ->at(hit),
                                      0,//cTrDepositedE
                                      0,//cTrEPart
                                      0,//cTrIDPart4
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

          if (correctly_backtracked!=NColHit) std::cout << group << " " << i << ": " << correctly_backtracked << " / " << NColHit << std::endl;
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
          fPlotClusterEfficiencyX[group].back()->Fill(eff, ParticleX->at(0));
          fPlotClusterEfficiencyY[group].back()->Fill(eff, ParticleY->at(0));
          fPlotClusterEfficiencyZ[group].back()->Fill(eff, ParticleZ->at(0));
        }
      }
    }
    
    Plot();
  };
  
  ~HitComparison() {
    if(fInputFile)  fInputFile->Close();
  };
  
  void Plot() {
    TCanvas* can = new TCanvas();
    can->Print("HitPlots.pdf[");


    fPlotClusterEfficiencyX.at("gaushit").at(0)->Draw();
    gPad->Update(); 
    fPlotClusterEfficiencyX.at("gaushit").at(0)->GetPaintedGraph()->SetMinimum(0.);
    fPlotClusterEfficiencyX.at("gaushit").at(0)->GetPaintedGraph()->SetMaximum(1.2);
    for (auto const& it: fPlotClusterEfficiencyX.at("trigprim")) {
      it->Draw("SAME");
    }
    can->Print("HitPlots.pdf");
    
    fPlotClusterEfficiencyY.at("gaushit").at(0)->Draw();
    gPad->Update(); 
    fPlotClusterEfficiencyY.at("gaushit").at(0)->GetPaintedGraph()->SetMinimum(0.);
    fPlotClusterEfficiencyY.at("gaushit").at(0)->GetPaintedGraph()->SetMaximum(1.2);
    for (auto const& it: fPlotClusterEfficiencyY.at("trigprim")) {
      it->Draw("SAME");
    }
    can->Print("HitPlots.pdf");

    fPlotClusterEfficiencyZ.at("gaushit").at(0)->Draw();
    gPad->Update(); 
    fPlotClusterEfficiencyZ.at("gaushit").at(0)->GetPaintedGraph()->SetMinimum(0.);
    fPlotClusterEfficiencyZ.at("gaushit").at(0)->GetPaintedGraph()->SetMaximum(1.2);
    for (auto const& it: fPlotClusterEfficiencyZ.at("trigprim")) {
      it->Draw("SAME");
    }
    can->Print("HitPlots.pdf");
    Plot(fPlotNHitProfileX, can, "HitPlots.pdf");
    Plot(fPlotNHitProfileY, can, "HitPlots.pdf");
    Plot(fPlotNHitProfileZ, can, "HitPlots.pdf");
    Plot(fPlotSADCProfileX, can, "HitPlots.pdf");
    Plot(fPlotSADCProfileY, can, "HitPlots.pdf");
    Plot(fPlotSADCProfileZ, can, "HitPlots.pdf");
    Plot(fPlotTimeProfileX, can, "HitPlots.pdf");
    Plot(fPlotTimeProfileY, can, "HitPlots.pdf");
    Plot(fPlotTimeProfileZ, can, "HitPlots.pdf");
    Plot(fPlotNHitTH2DX,    can, "HitPlots.pdf");
    Plot(fPlotNHitTH2DY,    can, "HitPlots.pdf");
    Plot(fPlotNHitTH2DZ,    can, "HitPlots.pdf");
    Plot(fPlotSADCTH2DX,    can, "HitPlots.pdf");
    Plot(fPlotSADCTH2DY,    can, "HitPlots.pdf");
    Plot(fPlotSADCTH2DZ,    can, "HitPlots.pdf");
    Plot(fPlotTimeTH2DX,    can, "HitPlots.pdf");
    Plot(fPlotTimeTH2DY,    can, "HitPlots.pdf");
    Plot(fPlotTimeTH2DZ,    can, "HitPlots.pdf");  

    PlotShape(fPlotNHitProfileX,    can, "HitPlots.pdf");
    PlotShape(fPlotNHitProfileY,    can, "HitPlots.pdf");
    PlotShape(fPlotNHitProfileZ,    can, "HitPlots.pdf");
    PlotShape(fPlotSADCProfileX,    can, "HitPlots.pdf");
    PlotShape(fPlotSADCProfileY,    can, "HitPlots.pdf");
    PlotShape(fPlotSADCProfileZ,    can, "HitPlots.pdf");
    PlotShape(fPlotTimeProfileX,    can, "HitPlots.pdf");
    PlotShape(fPlotTimeProfileY,    can, "HitPlots.pdf");
    PlotShape(fPlotTimeProfileZ,    can, "HitPlots.pdf");  
    can->Clear();
    fLegend->Draw();
    can->Print("HitPlots.pdf");
    can->Print("HitPlots.pdf]");
    delete can;
    can = NULL;
  };

  
};

#endif
