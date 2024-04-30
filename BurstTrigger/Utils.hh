#ifndef UTILS_HH
#define UTILS_HH 1
#include <fstream>
#include <string>
#include <vector>

#include "TFile.h"
#include "TMath.h"
#include "TTree.h"

#include "Configuration.hh"


inline double PoissonIntegral(Double_t x, Double_t mean){
  double value = 0;
  for (int i=0; i<x; ++i) {
    value += TMath::Power(mean,i) / TMath::Factorial(i);
  }
  value *=TMath::Exp(-mean);
  return value;
}

std::vector<Configuration> GetConfigurationTextFile(std::string InputFile) {
  std::vector<Configuration> conf;
  if (InputFile == "") return conf;
  int Config;
  double TimeWindow, Eff, EffErr, Bkgd, BkgdErr;
  std::string title;
  std::ifstream inFile;
  inFile.open(InputFile.c_str());

  while(inFile >> Config >> TimeWindow >> Eff >> EffErr >> Bkgd >> BkgdErr >> title){
    Configuration c;
    c.fClusterAlgorithm    = Config;
    c.fBurstTimeWindow     = TimeWindow;
    c.fClusterEfficiency   = Eff;
    c.fClusterEfficiencyError = EffErr;
    c.fBackgroundRate      = Bkgd;
    c.fBackgroundRateError = BkgdErr;
    c.fName = title;
    c.fLegendEntry = title;
    std::cout << "Config: " << Config << std::endl;
    std::cout << "Time Window: " << TimeWindow << std::endl;
    std::cout << "Eff: " << Eff << " +/- " << EffErr << std::endl;
    std::cout << "Bkg Rate: " << Bkgd << " +/- " << BkgdErr << std::endl;
    //if (Eff>0 && Bkgd>0) conf.push_back(c);
    //if (Bkgd>0) conf.push_back(c);
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

#endif
