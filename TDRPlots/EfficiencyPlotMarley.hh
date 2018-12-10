#ifndef EFFICIENCYPLOTMARLEY_HH
#define EFFICIENCYPLOTMARLEY_HH 1

#include "EfficiencyPlot.hh"



class EfficiencyPlotMarley: public EfficiencyPlot {

  
  void SetUpTreeName() {
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

  void Run() {
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
      int group=treegroup.first;
      int iter=0;
      fLegend[group] = new TLegend(0.1,0.6,0.4,0.9);
      f5MeVEfficiency[group] = new TH1D(Form("f5MeVEfficiency_%i",group), ";ADC Threshold;Efficiency @5MeV", 7, 7.5, 42.5);
      f5MeVEfficiency[group]->SetLineColor(color[group]);
      f5MeVEfficiency[group]->SetLineWidth(2);
      f5MeVEfficiency[group]->SetStats(0);
      std::string algorithm = treegroup.second[0];
      algorithm = std::regex_replace(algorithm, std::regex(R"([0-9])"), "");
      algorithm = std::regex_replace(algorithm, std::regex("arbitraryana"), "");
      fLegendGroup->AddEntry(f5MeVEfficiency[group], algorithm.c_str());
      for (auto const&tree: treegroup.second) {
        fTree[group].push_back((TTree*) fInputFile->Get((tree+"/SNSimTree").c_str()));
        if (fTree[group].back() == NULL) {
          throw WrongFileException();
        }
        fPlotClusterEfficiency[group].push_back(new TEfficiency((tree+"Eff").c_str(),
                                                                ";Electron energy [MeV];Clustering efficiency", 30, 0, 30));
        fPlotNHitProfile[group]      .push_back(new TProfile((tree+"Prof").c_str(),
                                                             ";Electron energy [MeV];NHit", 30, 0, 30));
        fPlotNHitTH2D[group]         .push_back(new TH2D((tree+"TH2D").c_str(),
                                                         ";Electron energy [MeV];NHit", 30, 0, 30, 10, 0, 10));
      
        TTree* t = fTree[group].back();
        TEfficiency*& ef = fPlotClusterEfficiency[group].back();
        TProfile*& pr = fPlotNHitProfile[group].back();
        TH2D*& th = fPlotNHitTH2D[group].back();
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
        th->SetStats(0);
        pr->SetLineColor(color[iter]);
        ef->SetLineColor(color[iter]);
        ++iter;

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
        
        fLegend[group]->SetHeader(title.c_str(), "C");
        fLegend[group]->AddEntry(pr, entry.c_str(), "L");
      
        for (int i=0; i<t->GetEntries(); ++i) {
          t->GetEntry(i);
          pr->Fill(ENu->at(0)*1000., NColHit);
          th->Fill(ENu->at(0)*1000., NColHit);

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
          c.ClusterHits2(hit_collection,cluster_collection);
          ef->Fill(cluster_collection.size()>0, ENu->at(0)*1000.);
          
        }
        f5MeVEfficiency[group]->SetBinContent(f5MeVEfficiency[group]->FindBin(threshold), ef->GetEfficiency(ef->FindFixBin(5)));
        f5MeVEfficiency[group]->SetBinError  (f5MeVEfficiency[group]->FindBin(threshold), 0.5*(ef->GetEfficiencyErrorLow(ef->FindFixBin(5))+ef->GetEfficiencyErrorLow(ef->FindFixBin(5))));

      }
    }
    Plot();
  }

};

#endif
