#include <iomanip>
#include <unistd.h>
#include <vector>

#include "TGraph.h"

#include "Configuration.hh"
#include "SNBurstTrigger.hh"
#include "Utils.hh"

int main(int argc, char** argv) {

  int opt;
  const int nPts = 100;
  std::string TheoryFile = "";
  std::string InputTextFile = "";
  std::string InputRootFile = "";
  extern char *optarg;
  extern int  optopt;
  while ( (opt = getopt(argc, argv, "t:i:r:")) != -1 ) {  // for each option...
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
  for (auto& it : Configs) {
    it.SetDistanceProbability    (hSNProbabilityVDistance);
    it.SetDistanceParametrisation(fInverse);
    it.fFractionInTimeWindow = hTimeProfile->Integral(0,hTimeProfile->FindBin(it.fBurstTimeWindow*1000));
    std::cout << "In a time window of " << it.fBurstTimeWindow
              << "sec, you get " << std::setprecision(9) << 100.*it.fFractionInTimeWindow << "% of the events." << std::endl;
    if(it.fFractionInTimeWindow<0 || it.fFractionInTimeWindow>1.01){
      std::cout << "The fraction in TimeWindow is bonkers!! ("
                << it.fFractionInTimeWindow << ")" << std::endl;
      exit(1);
    }

    TGraph* EfficiencyBurst = new TGraph(nPts+1);
    EfficiencyBurst->SetTitle(";Triggering efficiency;Clustering efficiency");
    for (int iter=0; iter<=nPts; ++iter) {
      it.fClusterEfficiency = (double)iter/nPts;
      snb.FillFakeRateNClusters(it);
      snb.FindOnePerMonthFakeRate(it);
      snb.FillEfficiencyBurst(it);
      if (it.fTH1DFakeRate_Cut        ) { delete it.fTH1DFakeRate_Cut        ; it.fTH1DFakeRate_Cut         = NULL; }
      if (it.fTH1DEfficiency_Burst    ) { delete it.fTH1DEfficiency_Burst    ; it.fTH1DEfficiency_Burst     = NULL; }
      if (it.fTH1DEfficiency_Distance ) { delete it.fTH1DEfficiency_Distance ; it.fTH1DEfficiency_Distance  = NULL; }
      if (it.fTH1DCoverage            ) { delete it.fTH1DCoverage            ; it.fTH1DCoverage             = NULL; }
      if (it.fTH1DLatency_Burst       ) { delete it.fTH1DLatency_Burst       ; it.fTH1DLatency_Burst        = NULL; }
      if (it.fTH1DLatency_Distance    ) { delete it.fTH1DLatency_Distance    ; it.fTH1DLatency_Distance     = NULL; }
      if (it.fTH1DLatency_Burst95CL   ) { delete it.fTH1DLatency_Burst95CL   ; it.fTH1DLatency_Burst95CL    = NULL; }
      if (it.fTH1DLatency_Distance95CL) { delete it.fTH1DLatency_Distance95CL; it.fTH1DLatency_Distance95CL = NULL; }
      it.fTH1DList.clear();
      it.FillHistograms();
      double eff = it.fTH1DEfficiency_Distance->GetBinContent(it.fTH1DEfficiency_Distance->FindBin(30));
      EfficiencyBurst->SetPoint(iter, it.fClusterEfficiency, eff);
    }
  
    TCanvas c;
    EfficiencyBurst->Draw();
    c.Print("EfficiencyCluster_EfficiencyBurst.pdf");
    
  }
  return 0;
  
}
