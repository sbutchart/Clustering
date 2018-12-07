#ifndef SNBURSTTRIGGER_HH
#define SNBURSTTRIGGER_HH 1
#include "Utils.hh"
#include "Configuration.hh"


class SNBurstTrigger {
private:
  int PoissonStatThreshold;
  bool reproduceAlexResult; 
  bool faster;
public:
  
  SNBurstTrigger():
    PoissonStatThreshold(10),
    reproduceAlexResult(false),
    faster(true){};
  void SetPoissonStatThreshold(const int c) { PoissonStatThreshold = c; };
  void FillFakeRateNClusters(Configuration& c) const;
  void FindOnePerMonthFakeRate(Configuration& c, const double TargetRate=1./3600./24./31) const;
  void FillEfficiencyBurst(Configuration& c, const int burstMin=1, const int burstMax=30e4) const;
};

#endif
