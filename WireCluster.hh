#ifndef WIRECLUSTER_HH
#define WIRECLUSTER_HH
#include "Cluster.hh"
#include "WireHit.hh"

class WireCluster: public Cluster {

public:
  WireCluster(std::vector<Hit*>&);
  WireCluster():
    Cluster(),
    fMC_UnderlyingE(0),
    fNElectron     (0){
  };
    
  ~WireCluster() {};
  using Cluster::Print;
  bool   GetIsSelected    () const { return fIsSelected                  ; };
  double GetHitSumADC     () const { return fSumPeak                     ; };
  double GetMC_UnderlyingE() const { return fMC_UnderlyingE              ; };
  double GetNElectron     () const { return fNElectron                   ; };
  
protected:
  virtual void SetTypeFromSumHit(const std::map<GenType,double>& nHit) {
    fTrueGenType = kOther;
    for (auto const& it: nHit) {
      if (it.second > 0)
        if (it.first == kSNnu) {
          fTrueGenType = kSNnu;
          break;
        }
    }
    if (fTrueGenType != kSNnu) {
      fTrueGenType = GetMax(nHit).first;
    }
  };
  
private:
  double fMC_UnderlyingE;
  double fNElectron;
};


#endif
