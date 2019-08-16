#ifndef CLUSTERENGINE_HH
#define CLUSTERENGINE_HH
#include <time.h>

#include "DataContainer/WireCluster.hh"
#include "DataContainer/OpticalCluster.hh"

class ClusterEngine{

public:
  ClusterEngine(double ChannelWidth=2,    double TimeWindow=20,   double MinHitADC=0,
                double TimeWindowOpt=0.2, double PositionOpt=300, double BucketSize=1):
    fChannelWidth(ChannelWidth),
    fTimeWindow(TimeWindow),
    fMinHitADC(MinHitADC),
    fTimeWindowOpt(TimeWindowOpt),
    fPositionOpt(PositionOpt),
    fBucketSize(BucketSize),
    fPreCutPEOpt(0),
    fStopwatch0(0),
    fStopwatch1(0),
    fSorting(0),
    TimeOrdering_Time (),
    SpaceOrdering_Time(),
    Clustering_Time   (){
  };
  
  void ClusterHits (const std::vector<WireHit*>&, std::vector<WireCluster*>&);
  void ClusterHits2(std::vector<WireHit*>&     vec_wirehit,
                    std::vector<WireCluster*>& vec_cluster) {
    if (fSorting==1) { ClusterHits2_stable_sort(vec_wirehit, vec_cluster); }
    else             { ClusterHits2_sort       (vec_wirehit, vec_cluster); }
  }
  void ClusterHits2_stable_sort(std::vector<WireHit*>&, std::vector<WireCluster*>&);
  void ClusterHits2_sort(std::vector<WireHit*>&,  std::vector<WireCluster*>&);
  void ClusterHits3(const std::vector<WireHit*>&, std::vector<WireCluster*>&);

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

  double GetBucketSize() const { return fBucketSize; };
  void   SetBucketSize(double B)  { fBucketSize = B; };

  double GetPreCutPEOpt() const { return fPreCutPEOpt; };
  void   SetPreCutPEOpt(double B)  { fPreCutPEOpt = B; };
  
  void SetSorting(const int i=0) { fSorting = i; };
  
  std::vector<double> GetElapsedTime_TimeOrdering () const { return TimeOrdering_Time;  } ;
  std::vector<double> GetElapsedTime_SpaceOrdering() const { return SpaceOrdering_Time; } ;
  std::vector<double> GetElapsedTime_Clustering   () const { return Clustering_Time;    } ;
  
  ~ClusterEngine(){
    TimeOrdering_Time .clear();
    SpaceOrdering_Time.clear();
    Clustering_Time   .clear();
  };
private:
  double fChannelWidth ;
  double fTimeWindow   ;
  double fMinHitADC    ;
  double fTimeWindowOpt;
  double fPositionOpt  ;
  double fBucketSize   ;
  double fPreCutPEOpt  ;
  clock_t fStopwatch0;
  clock_t fStopwatch1;

  int fSorting;
  
  std::vector<double> TimeOrdering_Time;
  std::vector<double> SpaceOrdering_Time;
  std::vector<double> Clustering_Time;   

  void ResetTime() {
    TimeOrdering_Time .clear();
    SpaceOrdering_Time.clear();
    Clustering_Time   .clear();   
  };

  
};

#endif
