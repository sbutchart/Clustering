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
  std::vector<int> vec_Burst;

  for (int Burst=0; Burst<1000; Burst++){
    vec_Burst.push_back(Burst);    
  }
  for (int Burst=1100; Burst<30e4; Burst=Burst+100) {
    vec_Burst.push_back(Burst);
  }

  if (vec_Burst.front() < burstMin) {
    size_t index=0;
    for (size_t it=0; it<vec_Burst.size(); ++it) {
      if (vec_Burst[it]>= burstMin) {
        index = it;
        break;
      }
    }
    vec_Burst.erase(vec_Burst.begin(), vec_Burst.begin()+index);
  }

  if (vec_Burst.back() > burstMax) {
    size_t index=0;
    for (size_t it=0; it<vec_Burst.size(); ++it) {
      if (vec_Burst[it]>= burstMax) {
        index = it;
        break;
      }
    }
    vec_Burst.erase(vec_Burst.begin()+index, vec_Burst.begin()+vec_Burst.size());
  }

  
  for(int Burst=burstMin; Burst<burstMax; Burst++){
    if (Burst%10000==0) std::cout << "Burst " << Burst << " / " << burstMax << std::endl;
    if (IsOne) {
      c.fEfficiency_Burst[Burst] = 1;
      continue;
    }
    
    double NSignal       = Burst * c.fFractionInTimeWindow * c.fClusterEfficiency;
    double TotalClusters = c.fBackgroundRate * c.fBurstTimeWindow + NSignal;
    if (c.fNClusterCut == 7)
      std::cout << "TotalClusters " << TotalClusters << std::endl;
    double RMS           = sqrt(TotalClusters);
    int    NMaxClusters  = (int)std::ceil(TotalClusters + 20*RMS);
    TH1D *Poisson = new TH1D("f_Poisson", "", NMaxClusters, -0.5, NMaxClusters-0.5);
    if (c.fNClusterCut>10.*TotalClusters) {
      c.fEfficiency_Burst[Burst] = 0;
      continue;
    }

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
      std::cout << "TotalClusters " << TotalClusters << std::endl;
      std::cout << "ClusterCut    " << c.fNClusterCut << std::endl;      
    } else {
      std::cout << "Module_SNBurst::FillEfficiencyBurst(): Integral of the Poisson is fischy (= "
                << Integral << ")" << std::endl;
    }
    
    if(Poisson){
      delete Poisson;
      Poisson = NULL;
    }
  }
  // for (auto const& it:c.fEfficiency_Burst){
  //   std::cout << it.first << std::endl;
  //   if (it.first > 100) break;
  // }
}
