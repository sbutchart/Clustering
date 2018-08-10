#ifndef CLUSTERENGINE_HH
#define CLUSTERENGINE_HH
#include "TStopwatch.h"


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
    fPositionOpt(PositionOpt),
    TimeOrdering_Time (),
    SpaceOrdering_Time(),
    Clustering_Time   (),
    fStopwatch(NULL) {
    fStopwatch = new TStopwatch();
  };
  
  ClusterEngine():
    fChannelWidth(0),
    fTimeWindow(0),
    fMinHitADC(0),
    fTimeWindowOpt(0),
    fPositionOpt(0),
    TimeOrdering_Time (),
    SpaceOrdering_Time(),
    Clustering_Time   (),
    fStopwatch(NULL) {
    fStopwatch = new TStopwatch();
  };
  
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


  std::vector<double> GetElapsedTime_TimeOrdering () { return TimeOrdering_Time;  } ;
  std::vector<double> GetElapsedTime_SpaceOrdering() { return SpaceOrdering_Time; } ;
  std::vector<double> GetElapsedTime_Clustering   () { return Clustering_Time;    } ;
  
  ~ClusterEngine(){
    if (fStopwatch) delete fStopwatch;
    fStopwatch = NULL;
    TimeOrdering_Time .clear();
    SpaceOrdering_Time.clear();
    Clustering_Time   .clear();
  };
  
private:
  TStopwatch *fStopwatch;

  double fChannelWidth;
  double fTimeWindow;
  double fMinHitADC;
  double fTimeWindowOpt;
  double fPositionOpt;

  std::vector<double> TimeOrdering_Time;
  std::vector<double> SpaceOrdering_Time;
  std::vector<double> Clustering_Time;   
 
};

#endif
