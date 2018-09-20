#ifndef OPTICALCLUSTER_HH
#define OPTICALCLUSTER_HH
#include <iostream>
#include <map>
#include <vector>

#include "Cluster.hh"
#include "OpticalHit.hh"

class OpticalCluster: public Cluster {
public:
  
  OpticalCluster(std::vector<Hit*>&);
  OpticalCluster(): Cluster() {
  };
  
  ~OpticalCluster() {};
  double GetHitSPE()const { return fSumPeak; };
  using Cluster::Print;

protected:
  virtual void SetTypeFromSumHit(const std::map<GenType,double>& nHit) {
    (void)nHit;
    double sign = 0;
    double back = 0;
    std::map<GenType,int> h_type;
    for (auto const& it: fHit) {
      h_type[it->GetGenType()]++;
      if (it->GetGenType() == kSNnu) {
        fTrueGenType = kSNnu;
        return;
      }
    }
    fTrueGenType = kOther;
    
  };

};

#endif
