#ifndef SMARTTRIGGER_HH
#define SMARTTRIGGER_HH
#include <algorithm>
#include <iostream>

#include "TAxis.h"
#include "TFile.h"
#include "TMath.h"
#include "TH1D.h"
#include "TTree.h"

#include "WireCluster.hh"
#include "Trigger.hh"

class SmartTrigger: public Trigger {
public:
  SmartTrigger(const std::string);
  ~SmartTrigger(){
    if (fLikelihood_Sign    ) delete fLikelihood_Sign    ;
    if (fLikelihood_Back    ) delete fLikelihood_Back    ;
    if (fLookupDiscriminator) delete fLookupDiscriminator;
    fLikelihood_Sign = NULL;
    fLikelihood_Back = NULL;
    fLookupDiscriminator = NULL;
  };

  void SetIsSelected(Cluster* c) const {};
  void SetIsSelected(const std::vector<Cluster*>&) const;

  void ConstructLikelihood(const std::string, const std::string);
  void SetBinning(const std::vector<double>);
  void SetResultFromCache(const std::string);

  void SetConfig   (const int    c=-1) { fConfig    = c; };
  void SetThreshold(const double d= 0) { fThreshold = d; };
  int    GetConfig   () const { return fConfig;    };
  double GetThreshold() const { return fThreshold; };
  
  TH1D* GetLikelihood_Signal    () const { return fLikelihood_Sign;     };
  TH1D* GetLikelihood_Background() const { return fLikelihood_Back;     };
  TH1D* GetLookupDiscriminator  () const { return fLookupDiscriminator; };
  
  virtual double GetEstimator(const Cluster&) const = 0;

  double GetDiscriminator(const double feat) const {
    if (!fLookupDiscriminator) {
      std::cout << "Likelihoods were not instantiated!" << std::endl;
      exit(1);
    }
    int bin = fLookupDiscriminator->FindBin(feat);
    bin = std::min(bin,fLookupDiscriminator->GetXaxis()->GetNbins());
    return fLookupDiscriminator->GetBinContent(bin);
  };
  
  double GetDiscriminator(const Cluster& cluster) const {
    return GetDiscriminator(GetEstimator(cluster));
  };
  
private:
  TH1D* fLikelihood_Sign;
  TH1D* fLikelihood_Back;
  TH1D* fLookupDiscriminator;
  std::string fFeature;
  int fConfig;
  double fThreshold;
  
};


class SmartERecoWireClusterTrigger: public SmartTrigger {
  using SmartTrigger::SmartTrigger;

  double GetEstimator(const Cluster& c) const{
    WireCluster& wc = (WireCluster&)c;
    return wc.GetRecoEnergy();
  };
  
};


#endif
