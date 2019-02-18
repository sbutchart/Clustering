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

#include "colors.h"

const bool reproduceAlexResult = false;

int main(int argc, char** argv) {

  int opt;
  std::string TheoryFile = "";
  std::string InputTextFile = "";
  std::string InputRootFile = "";
  std::string OutputRootFile = "";
  extern char *optarg;
  extern int  optopt;
  while ( (opt = getopt(argc, argv, "t:i:r:o:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 't':
      TheoryFile = optarg;
      break;
    case 'i':
      InputTextFile = optarg;
      break;
    case 'r':
      InputRootFile = optarg;
      break;
    case 'o':
      OutputRootFile = optarg;
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
    
  std::vector<Configuration> Configs  = GetConfigurationTextFile(InputTextFile);
  std::vector<Configuration> Configs2 = GetConfigurationRootFile(InputRootFile);
  Configs.insert(Configs.end(), Configs2.begin(), Configs2.end());

  Configuration c;

  c.fBackgroundRate = 0.0623056;
  c.fBurstTimeWindow = 10;
  c.fClusterEfficiency = 0.35;
  
  double c_background=0.475;
//   c.fBackgroundRate = c_background;
//   c.fBurstTimeWindow = 10;
//   c.fClusterEfficiency = 0.3;
//   c.fLegendEntry = "Default";

  Configs.push_back(c);

  c.fBackgroundRate = c_background;
  c.fBurstTimeWindow = 10;
  c.fClusterEfficiency = 0.25;
  c.fLegendEntry = "15cm^{2}";
  Configs.push_back(c);

  c.fBackgroundRate = c_background;
  c.fBurstTimeWindow = 10;
  c.fClusterEfficiency = 0.4;
  c.fLegendEntry = "Pessimistic reflections";
  Configs.push_back(c);

  c.fBackgroundRate = c_background;
  c.fBurstTimeWindow = 10;
  c.fClusterEfficiency = 0.55;
  c.fLegendEntry = "Optimistic reflections";
  Configs.push_back(c);

  // c.fBackgroundRate = c_background;
  // c.fBurstTimeWindow = 10;
  // c.fClusterEfficiency = 0.4237;
  // c.fLegendEntry = "30 cm^{2}";
  // Configs.push_back(c);

  // c.fBackgroundRate = c_background;
  // c.fBurstTimeWindow = 10;
  // c.fClusterEfficiency = 0.4492;
  // c.fLegendEntry = "45 cm^{2}";
  // Configs.push_back(c);

  // c.fBackgroundRate = c_background;
  // c.fBurstTimeWindow = 10;
  // c.fClusterEfficiency = 0.4324;
  // c.fLegendEntry = "60 cm^{2}";
  // Configs.push_back(c);

  // double c_background=0.475;
  // c.fBackgroundRate = c_background;
  // c.fBurstTimeWindow = 10;
  // c.fClusterEfficiency = 0.4428;
  // c.fLegendEntry = "S/N = 7";
  // Configs.push_back(c);

  // c.fBackgroundRate = c_background;
  // c.fBurstTimeWindow = 10;
  // c.fClusterEfficiency = 0.4492;
  // c.fLegendEntry = "S/N = 5";
  // Configs.push_back(c);

  // c.fBackgroundRate = c_background;
  // c.fBurstTimeWindow = 10;
  // c.fClusterEfficiency = 0.4520;
  // c.fLegendEntry = "S/N = 4";
  // Configs.push_back(c);

  // c.fBackgroundRate = c_background;
  // c.fBurstTimeWindow = 10;
  // c.fClusterEfficiency = 0.4380;
  // c.fLegendEntry = "S/N = 3";
  // Configs.push_back(c);


  if (Configs.size() == 0) {
    std::cout << "No Config parsed." << std::endl;
  } else {
    std::cout << Configs.size() << " config parsed." << std::endl;
  }
    
  if (OutputRootFile == "") {
    std::cout << "No output file parsed!! Exiting." << std::endl;
    exit(1);
  }
  
  if (TheoryFile == "") {
    std::cout << "No theory file parsed!! Exiting." << std::endl;
    exit(1);
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
    std::cout << "In a time window of " << it.fBurstTimeWindow
              << "sec, you get " << std::setprecision(9) << 100.*it.fFractionInTimeWindow << "\% of the events." << std::endl;
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
      it.DumpAndPlot();
    }
  }
  
  std::cout << "----------------------------------------------------" << std::endl;
  std::cout << "Configuration\tEfficiency for 10 events" << std::endl;
  for (auto const& it: Configs) {
    if (it.fNClusterCut>=0)
      std::cout << it.fName << "\t" << efficiency_10_events.at(it.fName) << std::endl;
  }
  std::cout << "----------------------------------------------------" << std::endl;
  std::cout << "Now saving all the configuration in the output tree in" << OutputRootFile << std::endl;
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
  can.Print("BurstTrigger.pdf[");
  int globalIt=0;
  std::vector<int> vec_Colors = getColors(2);
  gPad->SetGridx();
  gPad->SetGridy();
  for (auto& ThisConfig : Configs) {
    int color = vec_Colors.at(globalIt % vec_Colors.size());
    int color2 = vec_Colors.at(globalIt% vec_Colors.size()+1);
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
    can.Print("BurstTrigger.pdf");
    gPad->SetLogy(false);
    gPad->SetLogx(false);
    ThisConfig.fTH1DEfficiency_Burst->GetXaxis()->SetRangeUser(0,50);
    ThisConfig.fTH1DEfficiency_Burst->Draw();can.Print("BurstTrigger.pdf");
    ThisConfig.fTH1DEfficiency_Burst->GetXaxis()->SetRangeUser(10,1000);
    ThisConfig.fTH1DEfficiency_Distance->Draw();can.Print("BurstTrigger.pdf");
    ThisConfig.fDistanceProbability->SetLineColor(kBlue);
    ThisConfig.fDistanceProbability->SetLineWidth(3);
    gPad->SetLogy(false);
    gPad->SetLogx(false);
    ThisConfig.fDistanceProbability->GetXaxis()->SetRange(0,50);

    ThisConfig.fDistanceProbability->SetTitle(";Distance [kpc];SN probability #times SN triggering efficiency");
    ThisConfig.fDistanceProbability    ->Draw();
    ThisConfig.fTH1DCoverage           ->Draw("SAME");can.Print("BurstTrigger.pdf");
  }
  
  can.Print("BurstTrigger.pdf]");
  return 0;
}
