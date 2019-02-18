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

  for (double f=0; f<=2; f=f+0.05) {
    Configuration c;
    c.fBackgroundRate = 0.0261105 + 0.052221 * f;
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
    snb.FillEfficiencyBurst(it);
    // it.FillHistograms();
    // it.DumpAndPlot();
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
