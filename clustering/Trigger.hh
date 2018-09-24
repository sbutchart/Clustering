#ifndef TRIGGER_HH
#define TRIGGER_HH

#include "Cluster.hh"
#include "WireCluster.hh"
#include "OpticalCluster.hh"

class Trigger {
protected:
  Trigger (){};
  ~Trigger(){};

  virtual void SetIsSelected(Cluster*) const = 0;
  virtual void SetIsSelected(const std::vector<Cluster*>& clust) const {
    for(auto const& it: clust)
      SetIsSelected(it);
  };
  virtual void SetIsSelected(const std::vector<Cluster*>* clust) const {
    SetIsSelected(*clust);
  };


  virtual size_t CountAreSelected(const std::vector<Cluster*>& clust) const {
    size_t count = 0;
    for (auto const& it:clust)
      count += it->GetIsSelected();
    return count;
  };
  virtual size_t CountAreSelected(const std::vector<Cluster*>* clust) const {
    return CountAreSelected(*clust);
  };

  virtual size_t CountAreNotSelected(const std::vector<Cluster*>& clust) const {
    return clust.size() - CountAreSelected(clust);
  };
  
  virtual size_t CountAreNotSelected(const std::vector<Cluster*>* clust) const {
    return CountAreNotSelected(*clust);
  };

};


class SimpleWireTrigger: public Trigger {
public:
  SimpleWireTrigger(const int hit=0,
                    const int chan=0,
                    const double chanwidth=0,
                    const double sadc=0):
    Trigger(),
    fNHitsMin(hit),
    fNChanMin(chan),
    fChanWidthMin(chanwidth),
    fSADCMin(sadc) {};
  
  ~SimpleWireTrigger(){};
  
  void SetNHitsMin    (const int    i) { fNHitsMin     = i; };
  void SetNChanMin    (const int    i) { fNChanMin     = i; };
  void SetChanWidthMin(const double i) { fChanWidthMin = i; };
  void SetSADCMin     (const double i) { fSADCMin      = i; };

  int    GetNHitsMin    () const { return fNHitsMin    ; };
  int    GetNChanMin    () const { return fNChanMin    ; };
  double GetChanWidthMin() const { return fChanWidthMin; };
  double GetSADCMin     () const { return fSADCMin     ; };

  
  void SetIsSelected(Cluster* c) const {
    WireCluster* wc = (WireCluster*)c;
    c->SetIsSelected(true);
    if (wc->GetNHit()         < fNHitsMin    ) { c->SetIsSelected(false); return; }
    if (wc->GetNChannel()     < fNChanMin    ) { c->SetIsSelected(false); return; }
    if (wc->GetChannelWidth() < fChanWidthMin) { c->SetIsSelected(false); return; }
    if (wc->GetHitSumADC()    < fSADCMin     ) { c->SetIsSelected(false); return; }
  };
  
  void SetIsSelected(const std::vector<WireCluster*>& clust) const {
    for(auto const& it: clust) {
      SetIsSelected(it);
    }
  };
  
  void SetIsSelected(const std::vector<WireCluster*>* clust) const {
    SetIsSelected(*clust);
  };

  virtual size_t CountAreSelected(const std::vector<WireCluster*>& clust) const {
    size_t count = 0;
    for (auto const& it:clust)
      count += it->GetIsSelected();
    return count;
  };
  virtual size_t CountAreSelected(const std::vector<WireCluster*>* clust) const {
    return CountAreSelected(*clust);
  };

  virtual size_t CountAreNotSelected(const std::vector<WireCluster*>& clust) const {
    return clust.size() - CountAreSelected(clust);
  };
  
  virtual size_t CountAreNotSelected(const std::vector<WireCluster*>* clust) const {
    return CountAreNotSelected(*clust);
  };

private:
  int    fNHitsMin;
  int    fNChanMin;
  double fChanWidthMin;
  double fSADCMin;

};


class SimpleOpticalTrigger: public Trigger {
public:
  SimpleOpticalTrigger(const int hit=0):
    Trigger(),
    fNHitsMin(hit){};
  
  ~SimpleOpticalTrigger(){};
  
  void SetNHitsMin(const int i) { fNHitsMin = i; };
  int  GetNHitsMin() const { return fNHitsMin; };

  
  void SetIsSelected(Cluster* c) const {
    OpticalCluster* wc = (OpticalCluster*)c;
    c->SetIsSelected(true);
    if (wc->GetNHit() < fNHitsMin) { c->SetIsSelected(false); return; }
  };
  
  void SetIsSelected(const std::vector<OpticalCluster*>& clust) const {
    for(auto const& it: clust) {
      SetIsSelected(it);
    }
  };
  
  void SetIsSelected(const std::vector<OpticalCluster*>* clust) const {
    SetIsSelected(*clust);
  };

  virtual size_t CountAreSelected(const std::vector<OpticalCluster*>& clust) const {
    size_t count = 0;
    for (auto const& it:clust)
      count += it->GetIsSelected();
    return count;
  };
  virtual size_t CountAreSelected(const std::vector<OpticalCluster*>* clust) const {
    return CountAreSelected(*clust);
  };

  virtual size_t CountAreNotSelected(const std::vector<OpticalCluster*>& clust) const {
    return clust.size() - CountAreSelected(clust);
  };
  
  virtual size_t CountAreNotSelected(const std::vector<OpticalCluster*>* clust) const {
    return CountAreNotSelected(*clust);
  };

private:
  int fNHitsMin;
};


#endif
