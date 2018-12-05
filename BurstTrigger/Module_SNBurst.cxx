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
#include "TMath.h"
#include "TString.h"
#include "TTree.h"

#include "Configuration.hh"

int PoissonStatThreshold = 10;
const bool reproduceAlexResult = false; 
const bool faster = true; 


Double_t PoissonIntegral(Double_t x, Double_t mean){
  double value = 0;
  for (int i=0; i<x; ++i) {
    value += TMath::Power(mean,i) / TMath::Factorial(i);
  }
  value *=TMath::Exp(-mean);
  return value;
}


void FillFakeRateNClusters(Configuration& c) {
  
  double BackgroundMean = c.fBurstTimeWindow * c.fFractionInTimeWindow * c.fBackgroundRate;
  double RMS = sqrt(BackgroundMean);
  double Max = 50*RMS;
  
  for(int nClusters=1; nClusters<Max; nClusters++){
    if(BackgroundMean > PoissonStatThreshold){
      double nRMS     = (nClusters-BackgroundMean) / RMS;
      double fraction = (1 - TMath::Erf(nRMS / std::sqrt(2.))) / 2.;
      double fakeRate = fraction * c.fBackgroundRate;
      c.fFakeRate_Cut[nClusters] = fakeRate;
    } else {
      double integral = 1-PoissonIntegral(nClusters,BackgroundMean);
      double fakeRate = integral*c.fBackgroundRate;
      c.fFakeRate_Cut[nClusters] = fakeRate;
    }
  }
}

void FindOnePerMonthFakeRate(Configuration& c, const double TargetRate=1./3600./24./31) {
  c.fTargetFakeRate = TargetRate;
  c.fActualFakeRate = -1;
  c.fNClusterCut = -1;
  for (auto const& it: c.fFakeRate_Cut) {
    if (it.second<TargetRate) {
      c.fActualFakeRate = it.second;
      c.fNClusterCut = it.first;
      break;
    }
  }
  
  if (c.fNClusterCut == -1) {
    std::cout << "Module_SNBurst::FindOnePerMonthFakeRate(): Couldn't find an appropriate cluster cut for this target rate."
              << std::endl;
    std::cout << " - Configuration: " << c.fLegendEntry << std::endl;
    std::cout << " - TargetRate: " << TargetRate << std::endl;
  }
}

void FillEfficiencyBurst(Configuration& c, const int burstMin=1, const int burstMax=30e4){

  bool IsOne=false;

  for(int Burst=burstMin; Burst<burstMax; Burst++){

    if (IsOne) {
      c.fEfficiency_Burst[Burst] = 1;
      continue;
    }
    
    double NSignal       = Burst * c.fFractionInTimeWindow * c.fClusterEfficiency;
    int    TotalClusters = (int)std::ceil(c.fBackgroundRate * c.fBurstTimeWindow + NSignal);
    double RMS           = sqrt(TotalClusters);
    int    NMaxClusters  = (int)std::ceil(TotalClusters + 20*RMS);
    TH1D *Poisson = new TH1D("f_Poisson", "", NMaxClusters, -0.5, NMaxClusters-0.5);
    for (int i=1; i<NMaxClusters+1; ++i) {
      double BinCenter = Poisson->GetBinCenter(i);
      Poisson->SetBinContent(i, TMath::PoissonI(BinCenter,TotalClusters));
    }
    
    double Integral = 1. - Poisson->Integral(0, c.fNClusterCut);
    
    if(Integral>=0 && Integral<=1) {
      c.fEfficiency_Burst[Burst] = Integral;
      if(Integral>0.9999) {
        c.fEfficiency_Burst[Burst] = Integral;
        IsOne = true;
      }
    } else if(Integral<0) {
      c.fEfficiency_Burst[Burst] = 0;
      std::cout << "Module_SNBurst::FillEfficiencyBurst(): Integral of the Poisson is fischy (= "
        << Integral << ")" << std::endl;
    } else {
        std::cout << "Module_SNBurst::FillEfficiencyBurst(): Integral of the Poisson is fischy (= "
                  << Integral << ")" << std::endl;
      }
    
      if(Poisson){
      delete Poisson;
      Poisson = NULL;
    }
  }

}


std::vector<Configuration> GetConfigurationTextFile(std::string InputFile) {
  std::vector<Configuration> conf;
  if (InputFile == "") return conf;
  int Config;
  double TimeWindow, Eff, Bkgd, BkgdErr;
  std::ifstream inFile;
  inFile.open(InputFile.c_str());

  while(inFile >> Config >> TimeWindow >> Eff >> Bkgd >> BkgdErr){
    Configuration c;
    c.fClusterAlgorithm    = Config;
    c.fBurstTimeWindow     = TimeWindow;
    c.fClusterEfficiency   = Eff;
    c.fBackgroundRate      = Bkgd;
    c.fBackgroundRateError = BkgdErr;
    conf.push_back(c);
  }
  return conf;
  
}

std::vector<Configuration> GetConfigurationRootFile(std::string InputFile) {
  std::vector<Configuration> conf;
  if (InputFile == "") return conf;

  TFile* f = new TFile(InputFile.c_str(), "READ");
  TTree* t = (TTree*)f->Get("Configurations");

  if (t == NULL) {
    std::cout << "Module_SNBurst::GetConfigurationRootFile(): ";
    std::cout << "Tree is not of the right name in the code, you will have to change it by hand and recompile." << std::endl;
    exit(1);
  }
    
  Configuration *c = NULL;
  t->SetBranchAddress("Configuration", &c);

  for (int i=0; i<t->GetEntries(); ++i){
    t->GetEntry(i);
    conf.push_back(*c);
  }
  return conf;
}

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
  c.fBackgroundRate = 0.0985341;
  c.fBurstTimeWindow = 10;
  c.fClusterEfficiency = 0.581915;
  Configs.push_back(c);
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
      PoissonStatThreshold = 0;
      it.fFractionInTimeWindow = 1;
    }
    FillFakeRateNClusters(it);
    FindOnePerMonthFakeRate(it);
    FillEfficiencyBurst(it);
    it.FillHistograms();
    it.DumpAndPlot();
  }


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
  return 0;
}


