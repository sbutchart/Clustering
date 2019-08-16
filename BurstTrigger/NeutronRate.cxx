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

static const int nneutrino=10;
static const std::string axisstr="SN triggering efficiency LMC";

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
      std::cerr << "Unknown option: '" << char(optopt) << "!" << std::endl;
      break;
    }
  }
    
  std::map<double,Configuration> Configs;
  std::vector<double> multiplicator = {0.001,0.004,0.01,0.04,0.1,0.4,1.,4,10.};
  double precision=0.1;
  (void)precision;
  for (auto const& mult: multiplicator) {
    Configuration c;
    c.fBackgroundRate = 0.011156559 + 0.044626238 * mult;
    c.fBurstTimeWindow = 10;
    c.fClusterEfficiency = 0.328914;
    Configs[mult*100.] = c; 
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
  std::map<double,double> Efficiency;

  //LOOP AROUND THE CLUSTERING CONFIGURATIONS.    
  for(auto& it : Configs){
    Configuration c = it.second;
    c.SetDistanceProbability    (hSNProbabilityVDistance);
    c.SetDistanceParametrisation(fInverse);
    c.fFractionInTimeWindow = hTimeProfile->Integral(0,hTimeProfile->FindBin(c.fBurstTimeWindow*1000));
    std::cout << "In a time window of " << c.fBurstTimeWindow
              << "sec, you get " << std::setprecision(9) << 100.*c.fFractionInTimeWindow << "% of the events." << std::endl;

    if(c.fFractionInTimeWindow<0 || c.fFractionInTimeWindow>1.01){
      std::cout << "The fraction in TimeWindow is bonkers!! ("
                << c.fFractionInTimeWindow << ")" << std::endl;
      exit(1);
    }
    if (reproduceAlexResult) {
      snb.SetPoissonStatThreshold(0);
      c.fFractionInTimeWindow = 1;
    }
    snb.FillFakeRateNClusters(c);
    snb.FindOnePerMonthFakeRate(c);
    std::cout << "c.fNClusterCut " << c.fNClusterCut <<std::endl;
    snb.FillEfficiencyBurst(c, nneutrino, nneutrino+1);
    std::cout << c.fEfficiency_Burst.at(nneutrino) << "\n";
    Efficiency[it.first] = c.fEfficiency_Burst.at(nneutrino);
  }


  TCanvas can;
  can.Print("NeutronBurstRate.pdf[");

  TGraph* effLMC = new TGraph(multiplicator.size());
  effLMC->SetLineWidth(2);

  TGraph* CutLMC = new TGraph(multiplicator.size());
  CutLMC->SetLineWidth(2);
  int globalIt=0;
  
  for (auto& it : Efficiency) {
    double rate = it.first;
    effLMC->SetPoint(globalIt, rate, it.second);
    //CutLMC->SetPoint(globalIt, rate, it.second.fNClusterCut);
    globalIt++;
  }
  effLMC->SetMaximum(1);
  effLMC->SetMinimum(0.);
  gPad->SetGridx();
  gPad->SetGridy();
  // gPad->SetLogy();
  gPad->SetLogx();
  effLMC->SetTitle((";Neutron capture rate [Hz];"+axisstr).c_str());
  
  effLMC->Draw();
  effLMC->GetXaxis()->SetTitleOffset(1.5*effLMC->GetXaxis()->GetTitleOffset());
  can.Print("NeutronBurstRate.pdf");
  can.Clear();
  CutLMC->SetTitle(";Neutron capture rate [Hz];SN triggering threshold");
  CutLMC->Draw();
  can.Print("NeutronBurstRate.pdf");
  can.Print("NeutronBurstRate.pdf]");


  std::cout << "Now saving all the configuration in the output tree." << std::endl;
  Configuration *Con = NULL;
  TFile* fOutput = new TFile(OutputRootFile.c_str(), "RECREATE");
  TTree* tree = new TTree("Configurations","Configurations");
  tree->Branch("Configuration", &Con);
  for (auto& it: Configs) {
    Con = &it.second;
    tree->Fill();
  }
  effLMC->Write();
  tree->Write();
  fOutput->Close();
  return 0;
}
