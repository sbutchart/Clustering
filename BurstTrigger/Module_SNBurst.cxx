#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <unistd.h>
#include <vector>

#include "TCanvas.h"
#include "TF1.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TLine.h"
#include "TMath.h"
#include "TString.h"
#include "TTree.h"

#include "Configuration.hh"
#include "Utils.hh"
#include "SNBurstTrigger.hh"

#include "Utils/CLI11.hpp"

#include "colors.h"

const bool reproduceAlexResult = false;

int main(int argc, char** argv) {

  CLI::App app{"A program to generate supernova triggering sensitivities"};
  std::string TheoryFile = "";
  std::string InputTextFile = "";
  std::string InputRootFile = "";
  std::string OutputRootFile = "";
  std::string OutputPdfFile = "";
  app.add_option("-t,--theory-file", TheoryFile,      "Input theory file (root file, in Clustering/Burst/data/SNTheoryDistributions.root)")->required();
  app.add_option("--input-text",     InputTextFile,   "Input file name (text file, the output file of GetEffBackRate)");
  app.add_option("--input-root",     InputRootFile,   "Input file name (root file, the output file of ?)");
  app.add_option("--output-root",    OutputRootFile,  "Output file name (root file containing the histos)")->required();
  app.add_option("--output-pdf",     OutputPdfFile,   "Output file name (pdf file containing the histos)")->required();
  CLI11_PARSE(app, argc, argv);

  std::vector<Configuration> Configs  = GetConfigurationTextFile(InputTextFile);
  std::vector<Configuration> Configs2 = GetConfigurationRootFile(InputRootFile);
  Configs.insert(Configs.end(), Configs2.begin(), Configs2.end());

  if (Configs.size() == 0) {
    std::cout << "No Config parsed.\n";
    exit(1);
  } else {
    std::cout << Configs.size() << " config parsed.\n";
  }
    
  //GRAB THEORY PLOTS.
  TFile *fTheory = new TFile(TheoryFile.c_str(),"READ");
  TH1D  *hSNProbabilityVDistance = (TH1D*)fTheory->Get("h_SNProbabilityVDistance_LMC");
  TH1D  *hTimeProfile            = (TH1D*)fTheory->Get("h_MarlTime");
  TF1   *fEventsVSNDistance_10kt = (TF1*) fTheory->Get("f_EventsVSNDistance_10kt_100kpc");

  double gradient  = fEventsVSNDistance_10kt->GetParameter(0);
  double intercept = fEventsVSNDistance_10kt->GetParameter(1); 
  TF1   *fInverse  = new TF1("f_Inverse", "TMath::Power(x/(TMath::Power(10,[0])),1/[1])", 1,40e4);
  fInverse->SetParameter(0, intercept);
  fInverse->SetParameter(1, gradient);
  
  SNBurstTrigger snb;
  std::map<std::string,double> efficiency_10_events;

  //LOOP AROUND THE CLUSTERING CONFIGURATIONS.    
  for(auto& it : Configs){
    it.SetDistanceProbability    ((TH1D*)hSNProbabilityVDistance->Clone());
    it.SetDistanceParametrisation((TF1*)fInverse->Clone());
    it.fFractionInTimeWindow = hTimeProfile->Integral(0,hTimeProfile->FindBin(it.fBurstTimeWindow*1000));
    // std::cout << "In a time window of " << it.fBurstTimeWindow
    //           << "sec, you get " << std::setprecision(9) << 100.*it.fFractionInTimeWindow << "% of the events.\n";

    if(it.fFractionInTimeWindow<0 || it.fFractionInTimeWindow>1.01){
      std::cout << "The fraction in TimeWindow is bonkers!! ("
                << it.fFractionInTimeWindow << ")" << std::endl;
      exit(1);
    }
    
    if (reproduceAlexResult) {
      snb.SetPoissonStatThreshold(0);
      it.fFractionInTimeWindow = 1;
    }
    snb.FillFakeRateNClusters(it);
    snb.FindOnePerMonthFakeRate(it);
    if (it.fNClusterCut>=0) {
      snb.FillEfficiencyBurst(it);
      it.FillHistograms();
      efficiency_10_events[it.fName] = it.fEfficiency_Burst.at(10);
      //it.DumpAndPlot();
    }
  }
  
  std::cout << "----------------------------------------------------\n";
  std::cout << "Configuration\tEfficiency for 10 events\n";
  for (auto const& it: Configs) {
    if (it.fNClusterCut>=0)
      std::cout << it.fName << "\t" << efficiency_10_events.at(it.fName) << std::endl;
  }
  std::cout << "----------------------------------------------------\n";
  std::cout << "Now saving all the configuration in the output tree in" << OutputRootFile << "\n";
  Configuration *Con = NULL;
  TFile* fOutput = new TFile(OutputRootFile.c_str(), "RECREATE");
  TTree* tree = new TTree("Configurations","Configurations");
  tree->Branch("Configuration", &Con);
  for (auto& it: Configs) {
    Con = &it;
    if (it.fNClusterCut>=0)
      tree->Fill();
  }
  tree->Write();
  fOutput->Close();
  // fTheory->Close();


  TCanvas can;
  can.Print((OutputPdfFile+"[").c_str());
  int globalIt=0;
  std::vector<int> vec_Colors = getColors(2);
  gPad->SetGridx();
  gPad->SetGridy();
  for (auto& ThisConfig : Configs) {
    int color = vec_Colors.at(globalIt % vec_Colors.size());
    //int color2 = vec_Colors.at(globalIt% vec_Colors.size()+1);
    ThisConfig.fTH1DFakeRate_Cut       ->SetLineColor(color);
    ThisConfig.fTH1DEfficiency_Burst   ->SetLineColor(color);
    ThisConfig.fTH1DEfficiency_Distance->SetLineColor(color);
    ThisConfig.fTH1DCoverage           ->SetLineColor(color);
    ThisConfig.fTH1DFakeRate_Cut       ->SetLineColor(2);
    ThisConfig.fTH1DEfficiency_Burst   ->SetLineColor(2);
    ThisConfig.fTH1DEfficiency_Distance->SetLineColor(2);
    ThisConfig.fTH1DCoverage           ->SetLineColor(2);
    ThisConfig.fTH1DFakeRate_Cut->SetMinimum(1e-10);
    gPad->SetLogx(false);
    gPad->SetLogy(true);
    ThisConfig.fTH1DFakeRate_Cut       ->Draw();
    TLine *l_perMonth_2 = new TLine(0, ThisConfig.fTargetFakeRate,
                                    ThisConfig.fTH1DFakeRate_Cut->GetXaxis()->GetXmax(), ThisConfig.fTargetFakeRate);
    l_perMonth_2->SetLineColor(1);
    l_perMonth_2->SetLineWidth(3);
    l_perMonth_2->Draw();
    can.Print(OutputPdfFile.c_str());

    gPad->SetLogy(false);
    gPad->SetLogx(false);
    ThisConfig.fTH1DEfficiency_Burst->GetXaxis()->SetRangeUser(0,50);
    ThisConfig.fTH1DEfficiency_Burst->Draw();
    can.Print(OutputPdfFile.c_str());

    ThisConfig.fTH1DEfficiency_Burst->GetXaxis()->SetRangeUser(10,1000);
    ThisConfig.fTH1DEfficiency_Distance->Draw();
    can.Print(OutputPdfFile.c_str());

    ThisConfig.fDistanceProbability->SetLineColor(kBlue);
    ThisConfig.fDistanceProbability->SetLineWidth(3);
    gPad->SetLogy(false);
    gPad->SetLogx(false);
    ThisConfig.fDistanceProbability->GetXaxis()->SetRange(0,50);

    ThisConfig.fDistanceProbability->SetTitle(";Distance [kpc];SN probability #times SN triggering efficiency");
    ThisConfig.fDistanceProbability    ->Draw();
    ThisConfig.fTH1DCoverage           ->Draw("SAME");
    can.Print(OutputPdfFile.c_str());
  }
  
  can.Print((OutputPdfFile+"]").c_str());
  return 0;
}
