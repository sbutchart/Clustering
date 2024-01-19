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
  
  virtual ~OpticalCluster() {};
  double GetHitSPE()const { return fSumPeak; };
  using Cluster::Print;

protected:
  virtual void SetTypeFromSumHit(const std::map<std::string,double>& nHit) {
    (void)nHit;
    std::map<std::string,int> h_type;
    for (auto const& it: fHit) {
      h_type[it->GetGenType()]++;
      if (it->GetGenType() == "marley") {
        fTrueGenType = "marley";
        return;
      }
    }
//    fTrueGenType = kOther;
    
  };

};

#endif
