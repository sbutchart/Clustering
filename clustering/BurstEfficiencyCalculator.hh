#ifndef BURSTEFFICIENCYCALCULATOR
#define BURSTEFFICIENCYCALCULATOR 1
#include "TH1D.h"
#include <cmath>
#include <TMath.h>

int factorial(const int n) {
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

double PoissonIntegral(const double x, const double mean, const bool verb=0){
  if (verb) {
    std::cout << "x " << x << std::endl;
    std::cout << "mean " << mean << std::endl;
  }
  if (mean < 20) {
    double value = 0;
    for (int i=0; i<=x; ++i) {
      value += std::pow(mean,i) / TMath::Factorial(i);
      if (verb){
        std::cout << "std::pow(" << mean << "," << i << ") " << std::pow(mean,i) <<std::endl;
        std::cout << "factorial(" << i <<") "  << TMath::Factorial(i) << std::endl;
        std::cout << "loop value " << value << std::endl;
      }
    }
    value *= exp(-mean);
    if (verb) {
      std::cout << "exp " << exp(-mean) << std::endl;
      std::cout << "value " << value << std::endl;
    }
    return 1-value;
  } else {
    double RMS  = sqrt(mean);
    if (verb) std::cout << "RMS " << RMS << std::endl;
    double nRMS = (x-mean) / RMS;
    if (verb) std::cout << "nRMS " << nRMS << std::endl;
    return (1. - erf(nRMS/ sqrt(2.))) / 2.;
  }
}

class BurstEfficiencyCalculator {
public:
  BurstEfficiencyCalculator():
    fMaxFake         (0),
    fEfficiencyMarley(0),
    fBackgroundRate  (0),
    fTimeWindow      (0),
    fProportion      (1),
    fTimeProfile     (NULL){}

  double GetEfficiencyAtNBurst(const int nBurst=10) const{
    double Eff=0;
    double AverageNumberOfCluster = fBackgroundRate*fTimeWindow;
    int MaxNumber = AverageNumberOfCluster + 10. * sqrt(AverageNumberOfCluster);
    int Threshold=AverageNumberOfCluster;
    for (; Threshold<MaxNumber; Threshold++) {
      double Frac = PoissonIntegral(Threshold,AverageNumberOfCluster);
      if (Frac<0) PoissonIntegral(Threshold,AverageNumberOfCluster, true);
      double Rate = fBackgroundRate * fTimeWindow * Frac;
      if (Rate<fMaxFake)
        break;
    }
    std::cout << "Threshold " << Threshold << std::endl;
    
    double AverageNumberOfClusterInBurst = (fBackgroundRate*fTimeWindow +
                                            nBurst * fEfficiencyMarley * fProportion);

    Eff = PoissonIntegral(Threshold, AverageNumberOfClusterInBurst);
    if (Eff<0) PoissonIntegral(Threshold,AverageNumberOfCluster, true);
    
    return Eff;
  }


  double GetMaxFake         () const { return fMaxFake         ; };
  double GetEfficiencyMarley() const { return fEfficiencyMarley; };
  double GetBackgroundRate  () const { return fBackgroundRate  ; };
  double GetTimeWindow      () const { return fTimeWindow      ; };
  double GetProportion      () const { return fProportion      ; };
  TH1D*  GetTimeProfile     () const { return fTimeProfile     ; };

  void SetMaxFake         (const double f) { fMaxFake          = f; };
  void SetEfficiencyMarley(const double f) { fEfficiencyMarley = f; };
  void SetBackgroundRate  (const double f) { fBackgroundRate   = f; };
  void SetTimeWindow      (const double f) { fTimeWindow       = f; };
  void SetTimeProfile     (TH1D*  f) { fTimeProfile      = f; };

  
private:
  double fMaxFake         ;
  double fEfficiencyMarley;
  double fBackgroundRate  ;
  double fTimeWindow      ;
  double fProportion      ;
  TH1D*  fTimeProfile     ;
};

#endif
