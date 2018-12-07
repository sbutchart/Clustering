#include "SNBurstTrigger.hh"

void SNBurstTrigger::FillFakeRateNClusters(Configuration& c) const {
  
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


void SNBurstTrigger::FindOnePerMonthFakeRate(Configuration& c, const double TargetRate) const {
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


void SNBurstTrigger::FillEfficiencyBurst(Configuration& c, const int burstMin, const int burstMax) const {

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
