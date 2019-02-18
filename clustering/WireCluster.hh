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
    fNElectron     (0),
    fTrackIDs       (),
    fPartEnergies   (),
    fPartGenType    (),
    fPartTimes      (),
    fPartChannels   (),
    fPartSADCs      (),
    fPartGenTypes   (){
  };
  
  virtual ~WireCluster() {};
  using Cluster::Print;
  // bool   GetIsSelected        () const { return fIsSelected                  ; };
  double GetHitSumADC         () const { return fSumPeak                     ; };
  double GetMC_UnderlyingE    () const { return fMC_UnderlyingE              ; };
  double GetNElectron         () const { return fNElectron                   ; };
  std::vector<int> GetPartGenType       () const { return fPartGenType       ; };
  std::set<int> GetTrackIDs   () const { return fTrackIDs                    ; };
  std::vector<double> GetPartE() const { return fPartEnergies                ; };

  std::vector<float> GetPartTimes()    const { return fPartTimes            ; };
  std::vector<int>    GetPartChannels() const { return fPartChannels         ; };
  std::vector<double> GetPartSADCs()    const { return fPartSADCs            ; };
  std::vector<int>    GetPartGenTypes() const { return fPartGenTypes         ; };
  
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
  double fMC_UnderlyingE;
  double fNElectron;
  std::set<int> fTrackIDs;
  std::vector<double> fPartEnergies;
  std::vector<int>    fPartGenType;
  std::vector<float> fPartTimes;
  std::vector<int>    fPartChannels;
  std::vector<double> fPartSADCs;
  std::vector<int>    fPartGenTypes;
  
};


#endif
