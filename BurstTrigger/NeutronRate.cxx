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
    
  std::vector<Configuration> Configs;
  double precision=0.1;
  for (double f=0; f<=100; f=f+precision) {
    Configuration c;
    c.fBackgroundRate = 0.0178016 + 0.044504 * f;
    c.fBurstTimeWindow = 10;
    c.fClusterEfficiency = 0.35;
    Configs.push_back(c); 
  }
  
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
  snb.SetPoissonStatThreshold(100);
  
  //LOOP AROUND THE CLUSTERING CONFIGURATIONS.    
  for(auto& it : Configs){
    it.SetDistanceProbability    (hSNProbabilityVDistance);
    it.SetDistanceParametrisation(fInverse);
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
    std::cout << "c.fNClusterCut " << it.fNClusterCut <<std::endl;
    snb.FillEfficiencyBurst(it, 10, 11);
    snb.FillEfficiencyBurst(it);
    // it.FillHistograms();
    // it.DumpAndPlot();
  }


  TCanvas can;
  can.Print("NeutronBurstRate.pdf[");

  TGraph* effLMC = new TGraph((int)2/precision);
  effLMC->SetLineWidth(2);

  TGraph* CutLMC = new TGraph((int)2/precision);
  CutLMC->SetLineWidth(2);

  int globalIt=0;
  for (auto& ThisConfig : Configs) {
    double f=globalIt*precision;
    effLMC->SetPoint(globalIt, f, ThisConfig.fEfficiency_Burst.at(10));
    CutLMC->SetPoint(globalIt, f, ThisConfig.fNClusterCut);
    globalIt++;
  }
  effLMC->SetMaximum(1);
  effLMC->SetMinimum(1e-5);
  gPad->SetGridx();
  gPad->SetGridy();
  gPad->SetLogy();
  gPad->SetLogx();
  effLMC->SetTitle(";Neutron rate [#times default];SN triggering efficiency at 10 evts");
  effLMC->Draw();
  can.Print("NeutronBurstRate.pdf");
  can.Clear();
  CutLMC->SetTitle(";Neutron rate [#times default];SN triggering threshold");
  CutLMC->Draw();
  can.Print("NeutronBurstRate.pdf");
  can.Print("NeutronBurstRate.pdf]");


  std::cout << "Now saving all the configuration in the output tree." << std::endl;
  Configuration *Con = NULL;
  TFile* fOutput = new TFile(OutputRootFile.c_str(), "RECREATE");
  TTree* tree = new TTree("Configurations","Configurations");
  tree->Branch("Configuration", &Con);
  for (auto& it: Configs) {
    Con = &it;
    tree->Fill();
  }
  tree->Write();
  fOutput->Close();
  // fTheory->Close();


  TCanvas can;
  can.Print("BurstTrigger.pdf[");
  int globalIt=0;
  std::vector<int> vec_Colors = getColors(2);

  TGraph* effLMC = new TGraph(40);
  effLMC->SetLineWidth(2);

  int globalIt=0;
  for (auto& ThisConfig : Configs) {
    double f=globalIt*0.02;
    effLMC->SetPoint(globalIt, f, ThisConfig->fEfficiency_Burst.at(10));
  }
  effLMC->Draw();
  can.Print("BurstTrigger.pdf]");

  return 0;
}
