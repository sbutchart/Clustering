#ifndef TRIGGER_HH
#define TRIGGER_HH

#include "Cluster.hh"
#include "WireCluster.hh"
#include "OpticalCluster.hh"

class Trigger {
public:
  virtual ~Trigger(){};

protected:
  Trigger (){};

public:
  virtual bool IsTriggering(WireCluster&    c) const { c. SetIsTriggering(true); return true; };
  virtual bool IsTriggering(OpticalCluster& c) const { c. SetIsTriggering(true); return true; };
  virtual bool IsTriggering(WireCluster*    c) const { c->SetIsTriggering(true); return true; };
  virtual bool IsTriggering(OpticalCluster* c) const { c->SetIsTriggering(true); return true; };
  virtual bool IsTriggering(const std::vector<WireCluster*   >& c) const {
    bool trigger=false;
    for (auto const& it: c)
      if (IsTriggering(it)) trigger = true;
    return trigger;
  };
  virtual bool IsTriggering(const std::vector<OpticalCluster*>& c) const {
    bool trigger=false;
    for (auto const& it: c)
      if (IsTriggering(it)) trigger = true;
    return trigger;
  };
  // by default does an OR
  virtual bool IsTriggering(const std::vector<WireCluster*   >& wc,
                            const std::vector<OpticalCluster*>& oc) const {
    for (auto const& it: wc)
      if (IsTriggering(it)) return true;
    for (auto const& it: oc)
      if (IsTriggering(it)) return true;
    return false;
  };
};



// A simple trigger
class SimpleTrigger: public Trigger {
public:
  SimpleTrigger(const size_t whit      =0,
                const size_t wchan     =0,
                const double wchanwidth=0,
                const double wsadc     =0,
                const size_t ohit      =0):
    fWireNHitsMin    (whit      ),
    fWireNChanMin    (wchan     ),
    fWireChanWidthMin(wchanwidth),
    fWireSADCMin     (wsadc     ),
    fOptiNHitsMin    (ohit      ){};
  
  ~SimpleTrigger(){};
  
  void SetWireNHitsMin    (const int    i) { fWireNHitsMin     = i; };
  void SetWireNChanMin    (const int    i) { fWireNChanMin     = i; };
  void SetWireChanWidthMin(const double i) { fWireChanWidthMin = i; };
  void SetWireSADCMin     (const double i) { fWireSADCMin      = i; };
  void SetOptiNHitsMin    (const int    i) { fOptiNHitsMin     = i; };
  
  int    GetWireNHitsMin    () const { return fWireNHitsMin    ; };
  int    GetWireNChanMin    () const { return fWireNChanMin    ; };
  double GetWireChanWidthMin() const { return fWireChanWidthMin; };
  double GetWireSADCMin     () const { return fWireSADCMin     ; };
  int    GetOptiNHitsMin    () const { return fOptiNHitsMin    ; };

  using Trigger::IsTriggering;
  
  bool IsTriggering(WireCluster* c) const {
    c->SetIsTriggering(true);
    if (c->GetNHit()         < fWireNHitsMin    ) { c->SetIsTriggering(false); return false; }
    if (c->GetNChannel()     < fWireNChanMin    ) { c->SetIsTriggering(false); return false; }
    if (c->GetChannelWidth() < fWireChanWidthMin) { c->SetIsTriggering(false); return false; }
    if (c->GetHitSumADC()    < fWireSADCMin     ) { c->SetIsTriggering(false); return false; }
    return true;
  };

  bool IsTriggering(OpticalCluster* c) const {
    c->SetIsTriggering(true);
    if (c->GetNHit()         < fOptiNHitsMin    ) { c->SetIsTriggering(false); return false; }
    return true;
  };



private:
  size_t fWireNHitsMin    ;
  size_t fWireNChanMin    ;
  double fWireChanWidthMin;
  double fWireSADCMin     ;
  size_t fOptiNHitsMin    ;
};


#endif
