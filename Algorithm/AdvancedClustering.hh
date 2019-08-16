#include "Algorithm/Clustering.hh"



class AdvancedClustering: public Clustering {
  AdvancedClustering(){
    Clustering::Clustering();
  };

  virtual void SetupConfig_AdjChanTolerance(const std::vector<float> cut = {2}  ) { fcut_AdjChanTolerance = cut; };
  virtual void SetupConfig_HitsInWindow    (const std::vector<float> cut = {6}  ) { fcut_HitsInWindow     = cut; };
  virtual void SetupConfig_MinChannels     (const std::vector<float> cut = {2}  ) { fcut_MinChannels      = cut; };
  virtual void SetupConfig_MinChanWidth    (const std::vector<float> cut = {0}  ) { fcut_MinChanWidth     = cut; };
  virtual void SetupConfig_TimeWindowSize  (const std::vector<float> cut = {20} ) { fcut_TimeWindowSize   = cut; };
  virtual void SetupConfig_TotalADC        (const std::vector<float> cut = {0}  ) { fcut_TotalADC         = cut; };
  virtual void SetupConfig_TimeWindowOpt   (const std::vector<float> cut = {1}  ) { fcut_TimeWindowOpt    = cut; };
  virtual void SetupConfig_PositionOpt     (const std::vector<float> cut = {300}) { fcut_PositionOpt      = cut; };
  virtual void SetupConfig_BucketSize      (const std::vector<float> cut = {20} ) { fcut_BucketSize       = cut; };
  virtual void SetupConfig_OptHitInCluster (const std::vector<float> cut = {20} ) { fcut_OptHitInCluster  = cut; };

  virtual void RunClustering();
  
private:
  BiTrigger* fBiTrigger;
}


