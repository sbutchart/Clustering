#include "Configuration.hh"

#include <fstream>
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
#include "TLegend.h"
#include "TString.h"


#include "colors.h"

Double_t PoissonIntegral(Double_t x, Double_t mean){
  double value = 0;
  for (int i=0; i<x; ++i) {
    value += TMath::Power(mean,i) / TMath::Factorial(i);
  }
  value *=TMath::Exp(-mean);
  return value;
}



int main(int argc, char** argv) {

  int opt;
  std::string TheoryFile = "";
  std::string InputTextFile = "";
  std::string InputRootFile = "";
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
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
    
  if (TheoryFile == "") {
    std::cout << "No theory file parsed!! Exiting." << std::endl;
    exit(1);
  }
  
  int burstMin(1), burstMax(300);
  TFile *fTheory = new TFile(TheoryFile.c_str(),"READ");
  TH1D  *hSNProbabilityVDistance = (TH1D*)fTheory->Get("h_SNProbabilityVDistance_LMC");
  TH1D  *hTimeProfile            = (TH1D*)fTheory->Get("h_MarlTime");
  TF1   *fEventsVSNDistance_10kt = (TF1*) fTheory->Get("f_EventsVSNDistance_10kt_100kpc");

  double gradient  = fEventsVSNDistance_10kt->GetParameter(0);
  double intercept = fEventsVSNDistance_10kt->GetParameter(1); 
  TF1   *fInverse  = new TF1("f_Inverse", "TMath::Power(x/(TMath::Power(10,[0])),1/[1])", 1,40e4);
  fInverse->SetParameter(0, intercept);
  fInverse->SetParameter(1, gradient);

  TH1D * hTimeProfileIntegrated = (TH1D*)hTimeProfile->Clone();
  TH1D * hTimeProfileIntegratedNoBurst = (TH1D*)hTimeProfile->Clone();
  double Proportion=1;
  for (int i=0; i<hTimeProfile->GetXaxis()->GetNbins(); ++i) {
    hTimeProfileIntegrated->SetBinContent(i, hTimeProfile->Integral(0, i));
    Proportion = hTimeProfile->Integral(0, i);
  }
  
  std::cout << "Proportion " << Proportion << std::endl;

  double ProportionOutsideBurst=1;
  int burstbin = hTimeProfile->FindBin(200);
  std::cout << burstbin << std::endl;
  for (int i=burstbin; i<hTimeProfile->GetXaxis()->GetNbins(); ++i) {
    ProportionOutsideBurst = hTimeProfile->Integral(burstbin, i);
    hTimeProfileIntegratedNoBurst->SetBinContent(i, hTimeProfile->Integral(burstbin, i));
  }
  
  std::cout << "ProportionOutsideBurst " << ProportionOutsideBurst << std::endl;
  std::vector<Configuration> Configs;
  Configuration config1;

  // config1.fNClusterCut       = 6;
  // config1.fClusterEfficiency = 0.32;
  // config1.SetDistanceParametrisation(fInverse);
  // config1.SetDistanceProbability(hSNProbabilityVDistance);
  // Configs.push_back(config1);
  
  config1.fNClusterCut       = 6;
  config1.fClusterEfficiency = 0.34;
  config1.SetDistanceParametrisation(fInverse);
  config1.SetDistanceProbability(hSNProbabilityVDistance);
  Configs.push_back(config1);

  // config1.fNClusterCut       = 7;
  // config1.fClusterEfficiency = 0.35;
  // config1.SetDistanceParametrisation(fInverse);
  // config1.SetDistanceProbability(hSNProbabilityVDistance);
  // Configs.push_back(config1);

  // config1.fNClusterCut       = 9;
  // config1.fClusterEfficiency = 0.35;
  // config1.SetDistanceParametrisation(fInverse);
  // config1.SetDistanceProbability(hSNProbabilityVDistance);
  // Configs.push_back(config1);

  // config1.fNClusterCut       = 20;
  // config1.fClusterEfficiency = 0.38;
  // config1.SetDistanceParametrisation(fInverse);
  // config1.SetDistanceProbability(hSNProbabilityVDistance);
  // Configs.push_back(config1);

  // config1.fNClusterCut       = 30;
  // config1.fClusterEfficiency = 0.40;
  // config1.SetDistanceParametrisation(fInverse);
  // config1.SetDistanceProbability(hSNProbabilityVDistance);
  // Configs.push_back(config1);

  if (Configs.size() == 0) {
    std::cout << "No Config parsed." << std::endl;
  }
  
  std::vector<int> vec_Colors = getColors(2);

  for (auto& ThisConfig : Configs) {
    for (int burst=burstMin; burst<burstMax; ++burst) {
      double LatencyBin = -1;
      if (burst*ThisConfig.fClusterEfficiency > ThisConfig.fNClusterCut) {
        for (int i=0; i<hTimeProfileIntegrated->GetXaxis()->GetNbins(); ++i) {
          double NClusterInWindow = burst*ThisConfig.fClusterEfficiency*hTimeProfileIntegrated->GetBinContent(i);
          if(ThisConfig.fNClusterCut < NClusterInWindow) {
            LatencyBin=i;
            break;
          }
        }
      }
      ThisConfig.fLatency_Burst[burst] = hTimeProfileIntegrated->GetBinCenter(LatencyBin);

      
      int actualNEvent=0;
      for (int NEvent=0; NEvent<burst*ThisConfig.fClusterEfficiency; NEvent++) {
        if (PoissonIntegral(NEvent+1, burst*ThisConfig.fClusterEfficiency)>0.05) {
          actualNEvent=NEvent;
          break;
        }
      }
      LatencyBin = -1;
      if (actualNEvent > ThisConfig.fNClusterCut) {
        for (int i=0; i<hTimeProfileIntegrated->GetXaxis()->GetNbins(); ++i) {
          double NClusterInWindow = actualNEvent*hTimeProfileIntegrated->GetBinContent(i);
          if(ThisConfig.fNClusterCut < NClusterInWindow) {
            LatencyBin=i;
            break;
          }
        }
      }
      ThisConfig.fLatency_Burst95CL[burst] = hTimeProfileIntegrated->GetBinCenter(LatencyBin);


      if (burst*ThisConfig.fClusterEfficiency*ProportionOutsideBurst > ThisConfig.fNClusterCut) {
        for (int i=0; i<hTimeProfileIntegratedNoBurst->GetXaxis()->GetNbins(); ++i) {
          double NClusterInWindow = burst*ThisConfig.fClusterEfficiency*hTimeProfileIntegratedNoBurst->GetBinContent(i);
          if(ThisConfig.fNClusterCut < NClusterInWindow) {
            LatencyBin=i;
            break;
          }
        }
      }
      ThisConfig.fLatency_NoBurstBurst[burst] = hTimeProfileIntegratedNoBurst->GetBinCenter(LatencyBin);

      actualNEvent=0;
      for (int NEvent=0; NEvent<burst*ThisConfig.fClusterEfficiency*ProportionOutsideBurst; NEvent++) {
        if (PoissonIntegral(NEvent+1, burst*ThisConfig.fClusterEfficiency*ProportionOutsideBurst)>0.05) {
          actualNEvent=NEvent;
          break;
        }
      }
      LatencyBin = -1;
      if (actualNEvent > ThisConfig.fNClusterCut) {
        for (int i=0; i<hTimeProfileIntegratedNoBurst->GetXaxis()->GetNbins(); ++i) {
          double NClusterInWindow = actualNEvent*hTimeProfileIntegratedNoBurst->GetBinContent(i);
          if(ThisConfig.fNClusterCut < NClusterInWindow) {
            LatencyBin=i;
            break;
          }
        }
      }
      ThisConfig.fLatency_NoBurstBurst95CL[burst] = hTimeProfileIntegratedNoBurst->GetBinCenter(LatencyBin);
      
    }
    ThisConfig.FillHistograms();
    ThisConfig.DumpAndPlot();
  }
  hTimeProfileIntegrated->Draw();

  TCanvas c;
  c.Print("Latency.pdf[");
  hTimeProfile->Draw();
  c.Print("Latency.pdf");
 
  gPad->SetLeftMargin(gPad->GetLeftMargin()*1.5);
  gPad->SetLogx(true);
  Configs[0].fTH1DLatency_Burst->SetMaximum(11000);
  Configs[0].fTH1DLatency_Burst->GetXaxis()->SetRangeUser(10, 1000);

  Configs[0].fTH1DLatency_Burst->Draw();
  int globalIt=0;
  TLegend* leg = new TLegend(0.4,0.65,0.9,0.9);
  leg->SetHeader("Latency", "C");
  for (auto& ThisConfig : Configs) {
    //std::cout << ThisConfig.
    int color = vec_Colors.at(globalIt % vec_Colors.size());
    int color2 = vec_Colors.at(globalIt+1 % vec_Colors.size());
    ThisConfig.fTH1DLatency_Burst       ->SetLineColor(color);
    ThisConfig.fTH1DLatency_Distance    ->SetLineColor(color);
    ThisConfig.fTH1DLatency_Burst95CL   ->SetLineColor(color);
    ThisConfig.fTH1DLatency_Distance95CL->SetLineColor(color);
    ThisConfig.fTH1DLatency_Burst       ->SetLineWidth(2);
    ThisConfig.fTH1DLatency_Distance    ->SetLineWidth(2);
    ThisConfig.fTH1DLatency_Burst95CL   ->SetLineWidth(2);
    ThisConfig.fTH1DLatency_Distance95CL->SetLineWidth(2);
    ThisConfig.fTH1DLatency_Burst95CL   ->SetLineStyle(2);
    ThisConfig.fTH1DLatency_Distance95CL->SetLineStyle(2);

    ThisConfig.fTH1DLatency_NoBurstBurst       ->SetLineColor(color2);
    ThisConfig.fTH1DLatency_NoBurstDistance    ->SetLineColor(color2);
    ThisConfig.fTH1DLatency_NoBurstBurst95CL   ->SetLineColor(color2);
    ThisConfig.fTH1DLatency_NoBurstDistance95CL->SetLineColor(color2);
    ThisConfig.fTH1DLatency_NoBurstBurst       ->SetLineWidth(2);
    ThisConfig.fTH1DLatency_NoBurstDistance    ->SetLineWidth(2);
    ThisConfig.fTH1DLatency_NoBurstBurst95CL   ->SetLineWidth(2);
    ThisConfig.fTH1DLatency_NoBurstDistance95CL->SetLineWidth(2);
    ThisConfig.fTH1DLatency_NoBurstBurst95CL   ->SetLineStyle(2);
    ThisConfig.fTH1DLatency_NoBurstDistance95CL->SetLineStyle(2);

    ThisConfig.fTH1DLatency_Burst    ->Draw("SAME");
    ThisConfig.fTH1DLatency_Burst95CL->Draw("SAME");
    ThisConfig.fTH1DLatency_NoBurstBurst    ->Draw("SAME");
    ThisConfig.fTH1DLatency_NoBurstBurst95CL->Draw("SAME");
    globalIt++;
    leg->AddEntry(ThisConfig.fTH1DLatency_Burst    , "Central value (with neutronisation burst)");
    leg->AddEntry(ThisConfig.fTH1DLatency_Burst95CL, "at 95%CL (with neutronisation burst)");
    leg->AddEntry(ThisConfig.fTH1DLatency_NoBurstBurst    , "Central value (without neutronisation burst)");
    leg->AddEntry(ThisConfig.fTH1DLatency_NoBurstBurst95CL, "at 95%CL (without neutronisation burst)");
    std::cout <<globalIt << std::endl;
  }
  gPad->RedrawAxis();
  leg->Draw();
  c.Print("Latency.pdf");
  
  Configs[0].fTH1DLatency_Distance->SetMaximum(14000);
  Configs[0].fTH1DLatency_Distance->Draw();
  Configs[0].fTH1DLatency_Distance->GetXaxis()->SetRangeUser(0.1, 120);
  TH1D* dist = (TH1D*)Configs[0].GetDistanceProbability()->Clone();
  dist->SetLineColor(kMagenta);
  dist->SetFillColorAlpha(kMagenta,0.5);
  dist->Scale(9000/dist->GetMaximum());
  dist->Draw("SAME");
  leg->AddEntry(dist, "SN probability");
  for (auto& ThisConfig : Configs) {
    ThisConfig.fTH1DLatency_Distance    ->Draw("SAME");
    ThisConfig.fTH1DLatency_Distance95CL->Draw("SAME");
    ThisConfig.fTH1DLatency_NoBurstDistance    ->Draw("SAME");
    ThisConfig.fTH1DLatency_NoBurstDistance95CL->Draw("SAME");
  }
  gPad->RedrawAxis();
  leg->Draw();
  c.Print("Latency.pdf");
  c.Print("Latency.pdf]");


  return 0;
}
