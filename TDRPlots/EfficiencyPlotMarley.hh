#ifndef EFFICIENCYPLOTMARLEY_HH
#define EFFICIENCYPLOTMARLEY_HH 1

#include "EfficiencyPlot.hh"



class EfficiencyPlotMarley: public EfficiencyPlot {

public:
  std::map<int, std::vector<TEfficiency*>> fPlotClusterEfficiencyENU;
  std::map<int, std::vector<TProfile*>>    fPlotNHitProfileENU;
  std::map<int, std::vector<TH2D*>>        fPlotNHitTH2DENU;
  std::map<int, TH1D*>                     f10MeVEfficiencyENU;

  virtual void SetUpTreeName() {
    std::cout << "Setting up the correct tree" << std::endl;
    fTreeName[0].push_back("snanafasthit10"   );
    fTreeName[0].push_back("snanafasthit15"   );
    fTreeName[0].push_back("snanafasthit20"   );
    fTreeName[0].push_back("snanafasthit25"   );
    fTreeName[0].push_back("snanafasthit30"   );
    fTreeName[0].push_back("snanafasthit35"   );
    fTreeName[0].push_back("snanafasthit40"   );
    
    fTreeName[1].push_back("snanagaushit10"   );
    fTreeName[1].push_back("snanagaushit15"   );
    fTreeName[1].push_back("snanagaushit20"   );
    fTreeName[1].push_back("snanagaushit25"   );
    fTreeName[1].push_back("snanagaushit30"   );
    fTreeName[1].push_back("snanagaushit35"   );
    fTreeName[1].push_back("snanagaushit40"   );
    
    fTreeName[2].push_back("snanatrigprim1500");
    fTreeName[2].push_back("snanatrigprim2000");
    fTreeName[2].push_back("snanatrigprim2500");
    fTreeName[2].push_back("snanatrigprim3000");
    fTreeName[2].push_back("snanatrigprim3500");
    fTreeName[2].push_back("snanatrigprim4000");
  }

  using EfficiencyPlot::EfficiencyPlot;

  virtual void Run() {
    fInputFile = new TFile(fFileName.c_str(), "READ");
    SetUpTreeName();
    
    std::vector<float>  *LepE=NULL;
    std::vector<float>  *ENu=NULL;
    std::vector<int>    *Hit_View=NULL;
    std::vector<int>    *Hit_Chan=NULL;
    std::vector<double> *Hit_Time=NULL;
    std::vector<double> *Hit_SADC=NULL;
    std::vector<double> *Hit_RMS =NULL;
    std::vector<bool>   *Hit_True_GenType=NULL;
    
    int NColHit=0;
    fLegendGroup = new TLegend(0.6,0.1,0.9,0.3);
    std::vector<int> color = getColors(0);

    for (auto const&treegroup: fTreeName) {
      if (treegroup.first != 2) continue;
      int group=treegroup.first;
      int iter=0;
      fLegend[group] = new TLegend(0.1,0.6,0.4,0.9);
      f10MeVEfficiency[group] = new TH1D(Form("f10MeVEfficiency_%i",group), ";ADC Threshold;Efficiency @E_{e}=10MeV", 7, 7.5, 42.5);
      f10MeVEfficiency[group]->SetLineColor(color[group]);
      f10MeVEfficiency[group]->SetLineWidth(2);
      f10MeVEfficiency[group]->SetStats(0);
      f10MeVEfficiencyENU[group] = new TH1D(Form("f10MeVEfficiencyENU_%i",group), ";ADC Threshold;Efficiency @E_{#nu}=10MeV", 7, 7.5, 42.5);
      f10MeVEfficiencyENU[group]->SetLineColor(color[group]);
      f10MeVEfficiencyENU[group]->SetLineWidth(2);
      f10MeVEfficiencyENU[group]->SetStats(0);
      std::string algorithm = treegroup.second[0];
      algorithm = std::regex_replace(algorithm, std::regex(R"([0-9])"), "");
      algorithm = std::regex_replace(algorithm, std::regex("snana"), "");
      fLegendGroup->AddEntry(f10MeVEfficiency[group], algorithm.c_str());
      
      for (auto const&tree: treegroup.second) {
        if (tree != "snanatrigprim2000") continue;
        fTree[group].push_back((TTree*) fInputFile->Get((tree+"/SNSimTree").c_str()));
        if (fTree[group].back() == NULL) {
          throw WrongFileException();
        }
        fPlotClusterEfficiency[group].push_back(new TEfficiency((tree+"Eff").c_str(),
                                                                ";Electron energy [MeV];Clustering efficiency", 30, 0, 30));
        fPlotNHitProfile[group]      .push_back(new TProfile((tree+"Prof").c_str(),
                                                             ";Electron energy [MeV];NHit", 30, 0, 30));
        fPlotNHitTH2D[group]         .push_back(new TH2D((tree+"TH2D").c_str(),
                                                         ";Electron energy [MeV];NHit", 30, 0, 30, 20, 0, 20));

        fPlotClusterEfficiencyENU[group].push_back(new TEfficiency((tree+"EffENU").c_str(),
                                                                   ";Neutrino energy [MeV];Clustering efficiency", 30, 0, 30));
        fPlotNHitProfileENU[group]      .push_back(new TProfile((tree+"ProfENU").c_str(),
                                                                ";Neutrino energy [MeV];NHit", 30, 0, 30));
        fPlotNHitTH2DENU[group]         .push_back(new TH2D((tree+"TH2DENU").c_str(),
                                                            ";Neutrino energy [MeV];NHit", 30, 0, 30, 20, 0, 20));
      
        TTree* t = fTree[group].back();
        TEfficiency*& ef = fPlotClusterEfficiency[group].back();
        std::cout << "Filling " << ef->GetName() << std::endl;
        TProfile*& pr = fPlotNHitProfile[group].back();
        TH2D*& th = fPlotNHitTH2D[group].back();
        TEfficiency*& efENU = fPlotClusterEfficiencyENU[group].back();
        TProfile*& prENU = fPlotNHitProfileENU[group].back();
        TH2D*& thENU = fPlotNHitTH2D[group].back();
        t->SetBranchAddress("True_ENu_Lep", &LepE);
        t->SetBranchAddress("True_ENu",     &ENu);
        t->SetBranchAddress("NColHit",  &NColHit);
        t->SetBranchAddress("Hit_View", &Hit_View);
        t->SetBranchAddress("Hit_Chan", &Hit_Chan);
        t->SetBranchAddress("Hit_Time", &Hit_Time);
        t->SetBranchAddress("Hit_SADC", &Hit_SADC);
        t->SetBranchAddress("Hit_RMS",  &Hit_RMS );
        t->SetBranchAddress("Hit_True_GenType", &Hit_True_GenType);
        pr->SetStats(0);
        prENU->SetStats(0);
        pr->SetLineColor(color[iter]);
        ef->SetLineColor(color[iter]);
        prENU->SetLineColor(color[iter]);
        efENU->SetLineColor(color[iter]);
        ++iter;

        std::string title = tree;
        std::string entry = tree;
        title = std::regex_replace(title, std::regex(R"([0-9])"), "");
        title = std::regex_replace(title, std::regex("snana"), "");
        entry = std::regex_replace(entry, std::regex("snanagaushit"), "");
        entry = std::regex_replace(entry, std::regex("snanafasthit"), "");
        entry = std::regex_replace(entry, std::regex("snanatrigprim"), "");
        std::size_t found = entry.find("000");
        if (found!=std::string::npos) entry.erase(found+1, found+2);
        found = entry.find("00");
        if (found!=std::string::npos) entry.erase(found, found+2);
        int threshold = atoi(entry.c_str());

        entry.insert(0, "ADC threshold: ");
        
        fLegend[group]->SetHeader(title.c_str(), "C");
        fLegend[group]->AddEntry(pr, entry.c_str(), "L");
      
        for (int i=0; i<t->GetEntries(); ++i) {
          t->GetEntry(i);
          pr->Fill(LepE->at(0)*1000., NColHit);
          th->Fill(LepE->at(0)*1000., NColHit);
          prENU->Fill(ENu->at(0)*1000., NColHit);
          thENU->Fill(ENu->at(0)*1000., NColHit);

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

          if (correctly_backtracked!=NColHit) std::cout << title << " " << entry << ": " << correctly_backtracked << " / " << NColHit << std::endl;
          std::vector<WireCluster*> cluster_collection;
          cluster_collection.clear();
          c.ClusterHits2(hit_collection,cluster_collection);
          trigger.IsTriggering(cluster_collection);

          bool eff=0;
          for (auto const& it: cluster_collection) {
            if (it->GetIsTriggering()) {
              eff=1;
              break;
            }
          }
          ef   ->Fill(eff, LepE->at(0)*1000.);
          efENU->Fill(eff, ENu ->at(0)*1000.);
          
        }
        f10MeVEfficiency[group]->SetBinContent(f10MeVEfficiency[group]->FindBin(threshold), ef->GetEfficiency(ef->FindFixBin(10)));
        f10MeVEfficiency[group]->SetBinError  (f10MeVEfficiency[group]->FindBin(threshold), 0.5*(ef->GetEfficiencyErrorLow(ef->FindFixBin(10))+ef->GetEfficiencyErrorLow(ef->FindFixBin(10))));

        f10MeVEfficiencyENU[group]->SetBinContent(f10MeVEfficiencyENU[group]->FindBin(threshold), efENU->GetEfficiency(efENU->FindFixBin(10)));
        f10MeVEfficiencyENU[group]->SetBinError  (f10MeVEfficiencyENU[group]->FindBin(threshold), 0.5*(efENU->GetEfficiencyErrorLow(efENU->FindFixBin(10))+efENU->GetEfficiencyErrorLow(efENU->FindFixBin(10))));

      }
    }
    Plot();
  }
  virtual void Plot() const{
    TCanvas* c = new TCanvas();
    c->Print("TDRPlots.pdf[");


    for (auto const& it: fPlotNHitProfile) {
      if (it.second.size() > 0) {
        it.second[0]->SetMinimum(0);
        it.second[0]->Draw();
        for (auto const& it2: it.second) it2->Draw("SAME");
        fLegend.at(it.first)->Draw();
        c->Print("TDRPlots.pdf");
      }
    }
    for (auto const& it: fPlotNHitTH2D) {
      for (size_t it2=0; it2<it.second.size(); ++it2) {
        it.second[it2]->Draw("COLZ");
        fPlotNHitProfile.at(it.first).at(it2)->Draw("SAME");
        c->Print("TDRPlots.pdf");
      }
    }
    for (auto const& it: fPlotClusterEfficiency) {
      if (it.second.size() > 0) {
        it.second[0]->Draw();
        gPad->Update(); 
        auto graph = it.second[0]->GetPaintedGraph(); 
        graph->SetMinimum(0);
        graph->SetMaximum(1.2);
        for (auto const& it2: it.second) it2->Draw("SAME");
        fLegend.at(it.first)->SetX1NDC(0.6);
        fLegend.at(it.first)->SetX2NDC(0.9);
        fLegend.at(it.first)->SetY1NDC(0.1);
        fLegend.at(it.first)->SetY2NDC(0.4);
        fLegend.at(it.first)->Draw();
        c->Print("TDRPlots.pdf");
      }
    }
    // std::cout << f10MeVEfficiency.size() << std::endl;
    // if (f10MeVEfficiency.size() > 0) {
    //   f10MeVEfficiency.at(0)->SetMaximum(1.2);
    //   f10MeVEfficiency.at(0)->SetMinimum(0.0);
    //   gPad->SetGridx();
    //   f10MeVEfficiency.at(0)->Draw();
    //   for (auto const& it: f10MeVEfficiency)
    //     it.second->Draw("SAME");
    //   gPad->RedrawAxis();
    //   fLegendGroup->Draw();
    //   c->Print("TDRPlots.pdf");
    // }
    gPad->SetGridx(false);

    for (auto const& it: fPlotNHitProfileENU) {
      if (it.second.size() > 0) {
        it.second[0]->SetMinimum(0);
        it.second[0]->Draw();
        for (auto const& it2: it.second) it2->Draw("SAME");
        fLegend.at(it.first)->Draw();
        c->Print("TDRPlots.pdf");
      }
    }
    for (auto const& it: fPlotNHitTH2DENU) {
      for (auto const& it2: it.second) {
        it2->Draw("COLZ");
        c->Print("TDRPlots.pdf");
      }
    }
    
    for (auto const& it: fPlotClusterEfficiencyENU) {
      if (it.second.size() > 0) {
        it.second[0]->Draw();
        gPad->Update(); 
        auto graph = it.second[0]->GetPaintedGraph(); 
        graph->SetMinimum(0);
        graph->SetMaximum(1.2);
        for (auto const& it2: it.second) it2->Draw("SAME");
        fLegend.at(it.first)->SetX1NDC(0.6);
        fLegend.at(it.first)->SetX2NDC(0.9);
        fLegend.at(it.first)->SetY1NDC(0.1);
        fLegend.at(it.first)->SetY2NDC(0.4);
        fLegend.at(it.first)->Draw();
        c->Print("TDRPlots.pdf");
      }
    }
    
    // if (f10MeVEfficiencyENU.size() > 0) {
    //   f10MeVEfficiencyENU.at(0)->SetMaximum(1.2);
    //   f10MeVEfficiencyENU.at(0)->SetMinimum(0.0);
    //   gPad->SetGridx();
    //   f10MeVEfficiencyENU.at(0)->Draw();
    //   for (auto const& it: f10MeVfficiencyENU)
    //     it.second->Draw("SAME");
    //   gPad->RedrawAxis();
    //   fLegendGroup->Draw();
    //   c->Print("TDRPlots.pdf");
    // }
    c->Print("TDRPlots.pdf]");
    delete c;
    c = NULL;
  };

};

#endif
