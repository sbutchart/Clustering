#ifndef CLUSTERENGINE_HH
#define CLUSTERENGINE_HH
#include "WireCluster.hh"
#include "OpticalCluster.hh"


class ClusterEngine{

public:
  ClusterEngine(double ChannelWidth,  double TimeWindow, double MinHitADC,
                double TimeWindowOpt, double PositionOpt):
    fChannelWidth(ChannelWidth),
    fTimeWindow(TimeWindow),
    fMinHitADC(MinHitADC),
    fTimeWindowOpt(TimeWindowOpt),
    fPositionOpt(PositionOpt){};

  ClusterEngine():
    fChannelWidth(0),
    fTimeWindow(0),
    fMinHitADC(0),
    fTimeWindowOpt(0),
    fPositionOpt(0){};
  
  void ClusterHits (const std::vector<WireHit*>&, std::vector<WireCluster*>*, std::vector<WireHit*>*);
  void ClusterHits2(const std::vector<WireHit*>&, std::vector<WireCluster*>*, std::vector<WireHit*>*);
  void ClusterHits3(const std::vector<WireHit*>&, std::vector<WireCluster*>*, std::vector<WireHit*>*);

  void ClusterOpticalHits (std::vector<OpticalHit*>&, std::vector<OpticalCluster*>&);
  void ClusterOpticalHits2(std::vector<OpticalHit*>&, std::vector<OpticalCluster*>&);
  
  double GetTimeWindow() const { return fTimeWindow; };
  void   SetTimeWindow(double inTimeWindow=1) { fTimeWindow = inTimeWindow; };

  double GetTimeWindowOpt() const { return fTimeWindowOpt; };
  void   SetTimeWindowOpt(double inTimeWindow=0.1) { fTimeWindowOpt = inTimeWindow; };

  double GetChannelWidth() const { return fChannelWidth; };
  void   SetChannelWidth(double inChannelWidth=1) { fChannelWidth = inChannelWidth; };

  double GetPositionOpt() const { return fPositionOpt; };
  void   SetPositionOpt(double inPositionOpt=300) { fPositionOpt=inPositionOpt; };
    
  ~ClusterEngine(){};
private:
  double fChannelWidth  = 0;
  double fTimeWindow    = 0;
  double fMinHitADC     = 0;
  double fTimeWindowOpt = 0;
  double fPositionOpt   = 0;

public:
//  ClassDef(ClusterEngine,1)  //Simple event class
};


#endif
