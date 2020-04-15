#ifndef WIRECLUSTER_HH
#define WIRECLUSTER_HH
#include "Cluster.hh"
#include "WireHit.hh"

class WireCluster: public Cluster {

public:
  WireCluster(std::vector<Hit*>&);
  WireCluster():
    Cluster(),
    fWirePlane     (0),
    fMC_UnderlyingE(0),
    fNElectron     (0){
  };
  
  virtual ~WireCluster() {};
  using Cluster::Print;
  int    GetWirePlane     () const { return fWirePlane                   ; };
  bool   GetIsSelected    () const { return fIsSelected                  ; };
  double GetHitSumADC     () const { return fSumPeak                     ; };
  double GetMC_UnderlyingE() const { return fMC_UnderlyingE              ; };
  double GetNElectron     () const { return fNElectron                   ; };
  
protected:
  virtual void SetTypeFromSumHit(const std::map<GenType,double>& nHit) {
    (void)nHit;
    fTrueGenType = kOther;
    int sign=0;
    for (auto const& it: fHit) {
      sign+=(it->GetGenType() == kSNnu);
    }
    if (sign>1) fTrueGenType = kSNnu;
  };
  
private:
  int    fWirePlane;
  double fMC_UnderlyingE;
  double fNElectron;
};


#endif
