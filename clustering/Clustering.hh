#ifndef CLUSTERING_HH
#define CLUSTERING_HH 1

#include <iostream>
#include <vector>

#include "ClusteringBase.hh"
#include "InputManager.hh"
#include "Helper.h"

#include "OpticalCluster.hh"
#include "WireCluster.hh"
#include "ClusterEnergyEstimator.hh"
#include "ClusterEngine.hh"
#include "Trigger.hh"


class Clustering: public ClusteringBase {
public:

  Clustering():
    fEReco  (NULL),
    fNAPA   (  -1),
    fSorting(   0){
    InitialiseBase();

    fSNIM = new SNAnaInputManager    ();
    fSOM  = new StandardOutputManager();
    fSimpleTrigger = new SimpleTrigger();
    fIM = fSNIM;
    fOM = fSOM ;
    fTrigger = fSimpleTrigger;

  };

  void Initialise() {
    fSOM ->SetOutputFile(fOutputFileName.c_str());
    fSOM ->Initialise   ();
    fSOM ->SetOutputOffset(fOutputOffset);
    fSNIM->SetInputFile (fInputFileName.c_str());
    fSNIM->SetInputTree (fInputTreeName.c_str());
    fSNIM->LoadTree     ();
    fSNIM->SetAPAChannelMap(fmap_APA_Channel);
    fSNIM->SetNAPA(fNAPA);
  };


  void SetupCuts() {
    SetupConfig_AdjChanTolerance();
    SetupConfig_HitsInWindow    ();
    SetupConfig_MinChannels     ();
    SetupConfig_MinChanWidth    ();
    SetupConfig_TimeWindowSize  ();
    SetupConfig_TotalADC        ();
    SetupConfig_TimeWindowOpt   ();
    SetupConfig_PositionOpt     ();
    SetupConfig_BucketSize      ();
    SetupConfig_OptHitInCluster ();

    std::vector<size_t> sizes ={fcut_AdjChanTolerance.size(),
                                fcut_HitsInWindow    .size(),
                                fcut_MinChannels     .size(),
                                fcut_MinChanWidth    .size(),
                                fcut_TimeWindowSize  .size(),
                                fcut_TotalADC        .size(),
                                fcut_TimeWindowOpt   .size(),
                                fcut_PositionOpt     .size(),
                                fcut_BucketSize      .size(),
                                fcut_OptHitInCluster .size()};

    fNConfig = (int)(*std::min_element(sizes.begin(), sizes.end()));
    fWireClusterCount    = std::vector<float>(fNConfig,0);
    fOpticalClusterCount = std::vector<float>(fNConfig,0);
  };

  std::string GetERecoXMLFile() const { return fERecoXMLFile;   };
  int GetNConfig() const { return fNConfig; };

  void SetERecoXMLFile(const std::string s="") { fERecoXMLFile   = s; };
  void SetConfig      (const int n=-1)         { fConfig         = n; };

  virtual void SetupConfig_AdjChanTolerance(const std::vector<float> cut = {2,2,2,2,2,2}            ) { fcut_AdjChanTolerance = cut; };
  virtual void SetupConfig_HitsInWindow    (const std::vector<float> cut = {6,6,6,6,6,6}            ) { fcut_HitsInWindow     = cut; };
  virtual void SetupConfig_MinChannels     (const std::vector<float> cut = {2,2,2,2,2,2}            ) { fcut_MinChannels      = cut; };
  virtual void SetupConfig_MinChanWidth    (const std::vector<float> cut = {0,0,0,0,0,0}            ) { fcut_MinChanWidth     = cut; };
  virtual void SetupConfig_TimeWindowSize  (const std::vector<float> cut = {20,20,20,20,20,20}      ) { fcut_TimeWindowSize   = cut; };
  virtual void SetupConfig_TotalADC        (const std::vector<float> cut = {0,0,0,0,0,0}            ) { fcut_TotalADC         = cut; };
  virtual void SetupConfig_TimeWindowOpt   (const std::vector<float> cut = {0.2,1}                  ) { fcut_TimeWindowOpt    = cut; };
  virtual void SetupConfig_PositionOpt     (const std::vector<float> cut = {300,300,300,300,300,300}) { fcut_PositionOpt      = cut; };
  virtual void SetupConfig_BucketSize      (const std::vector<float> cut = {1,20}                   ) { fcut_BucketSize       = cut; };
  virtual void SetupConfig_OptHitInCluster (const std::vector<float> cut = {20,20,20}               ) { fcut_OptHitInCluster  = cut; };


  void RunClustering();

  void ResetFillVariable();

  ~Clustering()
    {
      if (fEReco) delete fEReco;
      fEReco = NULL;
    };

private:
  SimpleTrigger* fSimpleTrigger;

  std::string fERecoXMLFile;
  ClusterEnergyEstimator* fEReco;

  std::vector<float> fWireClusterCount   ;
  std::vector<float> fOpticalClusterCount;

  std::vector<float> fcut_AdjChanTolerance;
  std::vector<float> fcut_HitsInWindow    ;
  std::vector<float> fcut_MinChannels     ;
  std::vector<float> fcut_MinChanWidth    ;
  std::vector<float> fcut_TimeWindowSize  ;
  std::vector<float> fcut_TotalADC        ;
  std::vector<float> fcut_TimeWindowOpt   ;
  std::vector<float> fcut_PositionOpt     ;
  std::vector<float> fcut_BucketSize      ;
  std::vector<float> fcut_OptHitInCluster ;

  int fNConfig;
  int fCurrentConfig;
  int fNAPA   ;
  int fConfig ;
  int fSorting;

  SNAnaInputManager    * fSNIM;
  StandardOutputManager* fSOM ;

};

#endif
