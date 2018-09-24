#ifndef CLUSTERING_HH
#define CLUSTERING_HH

// ROOT includes
#include "TH1I.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TTree.h"
#include "TFile.h"
#include "TGraph.h"
#include "TStopwatch.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TLegend.h"


#include <iostream>
#include <vector>
#include <algorithm>

#include "InputManager.h"
#include "Helper.h"

#include "OpticalCluster.hh"
#include "WireCluster.hh"
#include "ClusterEnergyEstimator.hh"
#include "ClusterEngine.hh"
#include "Trigger.hh"


class Clustering {
public:

  Clustering():
    fInputFileName ("/dune/app/users/plasorak/workspace/SNAna.root"),
    fInputTreeName (""),
    fOutputFileName(""),
    f_Output       (NULL),
    fWireTrigger   (NULL),
    fOpticalTrigger(NULL),
    fClustEng      (NULL),
    fEReco         (NULL),
    fPrintLevel    (-1),
    fNAPA          (-1),
    fConfig        (-1),
    fSorting       (0),
    t_Output_ClusteredWireHit   (NULL),
    t_Output_ClusteredOpticalHit(NULL),
    t_Output_TrueInfo           (NULL),
    t_Output_TimingInfo         (NULL),
    h_ENu_MC     (NULL),
    h_MarlTime_MC(NULL),
    fmap_APA_Channel(),
    im()    {

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
      
      fmap_APA_Channel[-1] = std::numeric_limits<int>::max();
      fmap_APA_Channel[0]  = 0;
      fmap_APA_Channel[1]  = 3000;
      fmap_APA_Channel[2]  = 6000;
      fmap_APA_Channel[3]  = 8000;
      fmap_APA_Channel[4]  = 11000;
      fmap_APA_Channel[5]  = 14000;
      fmap_APA_Channel[6]  = 16000;
      fmap_APA_Channel[7]  = 19000;
      fmap_APA_Channel[8]  = 21000;
      fmap_APA_Channel[9]  = 24000;
      fmap_APA_Channel[10] = 26000;
      fmap_APA_Channel[11] = 29000;
      fmap_APA_Channel[12] = 31000;
      
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

  int GetPrintLevel() const { return fPrintLevel; };
  std::string GetInputFile   () const { return fInputFileName;  };
  std::string GetInputTree   () const { return fInputTreeName;  };
  std::string GetOutputFile  () const { return fOutputFileName; };
  std::string GetERecoXMLFile() const { return fERecoXMLFile;   };

  void SetPrintLevel  (const int p=-1) { fPrintLevel     = p; };
  void SetInputFile   (const std::string s="") { fInputFileName  = s; };
  void SetInputTree   (const std::string s="") { fInputTreeName  = s; };
  void SetOutputFile  (const std::string s="") { fOutputFileName = s; };
  void SetERecoXMLFile(const std::string s="") { fERecoXMLFile   = s; };
  void SetNAPA        (const int n=-1)         { fNAPA           = n; };
  void SetConfig      (const int n=-1)         { fConfig         = n; };
  void SetSorting     (const int n=0)          { fSorting        = n; };
    
  void SetupConfig_AdjChanTolerance(const std::vector<float> cut = {2,2,2,2,2,2}            ) { fcut_AdjChanTolerance = cut; };
  void SetupConfig_HitsInWindow    (const std::vector<float> cut = {5,5,5,6,6,6}            ) { fcut_HitsInWindow     = cut; };
  void SetupConfig_MinChannels     (const std::vector<float> cut = {2,2,2,2,2,2}            ) { fcut_MinChannels      = cut; };
  void SetupConfig_MinChanWidth    (const std::vector<float> cut = {0,0,0,0,0,0}            ) { fcut_MinChanWidth     = cut; };
  void SetupConfig_TimeWindowSize  (const std::vector<float> cut = {20,20,20,20,20,20}      ) { fcut_TimeWindowSize   = cut; };
  void SetupConfig_TotalADC        (const std::vector<float> cut = {0,0,0,0,0,0}            ) { fcut_TotalADC         = cut; };
  //void SetupConfig_TimeWindowOpt   (const std::vector<float> cut = {1.0,1.5,2.0,1.0,1.5,2.0}) { fcut_TimeWindowOpt    = cut; };
  void SetupConfig_TimeWindowOpt   (const std::vector<float> cut = {0.8,0.8}) { fcut_TimeWindowOpt    = cut; };
  void SetupConfig_PositionOpt     (const std::vector<float> cut = {300,300,300,300,300,300}) { fcut_PositionOpt      = cut; };
  //void SetupConfig_BucketSize      (const std::vector<float> cut = {20,20,20,20,20,20}      ) { fcut_BucketSize       = cut; };
  void SetupConfig_BucketSize      (const std::vector<float> cut = {20,20000000}      ) { fcut_BucketSize       = cut; };
  void SetupConfig_OptHitInCluster (const std::vector<float> cut = {0,0,15,20,20,20}      ) { fcut_OptHitInCluster  = cut; };

  
  int GetNConfig() const { return fNConfig; };
  // The main guts...
  int ClusterAll(int);
  void FillClusterInfo(WireCluster* clust);
  void FillClusterInfo(OpticalCluster* clust);
  void FillUnusedHitInfo(WireHit* hit){ ResetFillVariable(); };

  void FillClusterEngineTimingInfo_Opti(ClusterEngine* clusterEngine);
  void FillClusterEngineTimingInfo_Wire(ClusterEngine* clusterEngine);
  void FillClusterERecoTimingInfo_Wire (ClusterEnergyEstimator* clusterEReco);
  void FillClusterERecoTimingInfo_Opti (ClusterEnergyEstimator* clusterEReco);

  void ResetFillVariable();

  ~Clustering()
    {
      if(t_Output_ClusteredWireHit   ) delete t_Output_ClusteredWireHit   ;
      if(t_Output_ClusteredOpticalHit) delete t_Output_ClusteredOpticalHit;
      if(t_Output_TrueInfo           ) delete t_Output_TrueInfo           ;
      if(t_Output_TimingInfo         ) delete t_Output_TimingInfo         ;
      
      if(fWireTrigger)    delete fWireTrigger;
      if(fOpticalTrigger) delete fOpticalTrigger;
      if(fClustEng) delete fClustEng;  
      fWireTrigger    = NULL;
      fOpticalTrigger = NULL;
      fClustEng = NULL;

      fWireClusterCount   .clear();
      fOpticalClusterCount.clear();
      
      fcut_AdjChanTolerance.clear();
      fcut_HitsInWindow    .clear();
      fcut_MinChannels     .clear();
      fcut_MinChanWidth    .clear();
      fcut_TimeWindowSize  .clear();
      fcut_TotalADC        .clear();
      fcut_TimeWindowOpt   .clear();
      fcut_PositionOpt     .clear();
      fcut_BucketSize      .clear();
      fcut_OptHitInCluster .clear();

      t_Output_ClusteredWireHit    = NULL;
      t_Output_ClusteredOpticalHit = NULL;
      t_Output_TrueInfo            = NULL;
      t_Output_TimingInfo          = NULL;
      
      if(h_ENu_MC)      delete h_ENu_MC;
      if(h_MarlTime_MC) delete h_MarlTime_MC;
      h_ENu_MC = NULL;
      h_MarlTime_MC = NULL;

      if(f_Output) f_Output->Close();
      f_Output = NULL;

      if (fEReco) delete fEReco;
      fEReco = NULL;
    };
  
private:
  std::string fInputFileName ;
  std::string fInputTreeName ;
  std::string fOutputFileName;
  std::string fERecoXMLFile  ;
  TFile *f_Output;
  SimpleWireTrigger* fWireTrigger;
  SimpleOpticalTrigger* fOpticalTrigger;
  ClusterEngine* fClustEng;
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
  int fPrintLevel;
  int fNAPA;
  int fConfig;
  int fSorting;

  unsigned int fNEvent;
   
  TTree* t_Output_ClusteredWireHit;
  TTree* t_Output_ClusteredOpticalHit;
  TTree* t_Output_TrueInfo;
  TTree* t_Output_TimingInfo;

  TH1D* h_ENu_MC     ;
  TH1D* h_MarlTime_MC;
  
//OUTPUT VARIABLES
  int    out_Event         ;
  int    out_MarleyIndex   ;
  int    out_Cluster       ;
  int    out_Config        ;
  double out_StartChan     ;
  double out_EndChan       ;
  double out_APA           ;
  double out_ChanWidth     ;
  double out_NChan         ;
  double out_Type          ;
  double out_NHits         ;
  double out_SumADC        ;
  double out_FirstTimeHit  ;
  double out_LastTimeHit   ;
  double out_TimeWidth     ;
  double out_E_deposited   ;
  double out_NElectron     ;
  double out_EReco         ;
  double out_MC_UnderlyingE;
  double out_TrClusterPosX ;
  double out_TrClusterPosY ;
  double out_TrClusterPosZ ;
  double out_RecClusterPosX;
  double out_RecClusterPosY;
  double out_RecClusterPosZ;
  double out_pur_Other     ;
  double out_pur_SNnu      ;
  double out_pur_APA       ;
  double out_pur_CPA       ;
  double out_pur_Ar39      ;
  double out_pur_Neutron   ;
  double out_pur_Krypton   ;
  double out_pur_Polonium  ;
  double out_pur_Radon     ;
  double out_pur_Ar42      ;
  double out_YWidth        ;
  double out_ZWidth        ;
  double out_SumPE         ;

  std::vector<double> TimeOrdering_WireClustTime        ;
  std::vector<double> SpaceOrdering_WireClustTime       ;
  std::vector<double> Clustering_WireClustTime          ;
  std::vector<double> EnergyReconstruction_WireClustTime;
  std::vector<double> TimeOrdering_OptClustTime         ;
  std::vector<double> SpaceOrdering_OptClustTime        ;
  std::vector<double> Clustering_OptClustTime           ;
  std::vector<double> EnergyReconstruction_OptClustTime ;
  
    
  std::vector<int>    out_HitView;
  std::vector<int>    out_GenType;
  std::vector<int>    out_HitChan;
  std::vector<double> out_HitTime;
  std::vector<double> out_HitSADC;
  std::vector<double> out_HitSPE;
  std::vector<double> out_HitRMS;
  std::vector<double> out_HitWidth;
  std::vector<double> out_TrPosX; 
  std::vector<double> out_TrPosY; 
  std::vector<double> out_TrPosZ; 

  std::vector<int>    out_PDSHit_GenType  ;
  std::vector<double> out_PDSHit_X        ;
  std::vector<double> out_PDSHit_Y        ;
  std::vector<double> out_PDSHit_Z        ;
  std::vector<double> out_PDSHit_PeakTime ;
  std::vector<double> out_PDSHit_Width    ;
  std::vector<double> out_PDSHit_PE       ;
  std::vector<double> out_PDSHit_OpChannel;
 
  std::vector<double> out_MarlTime;
  std::vector<double> out_ENu;
  std::vector<double> out_ENu_Lep;
  std::vector<double> out_PosX;
  std::vector<double> out_PosY;
  std::vector<double> out_PosZ;
  std::vector<double> out_PosT;
  std::vector<double> out_DirX;
  std::vector<double> out_DirY;
  std::vector<double> out_DirZ;

  std::map<int, int> fmap_APA_Channel;
  InputManager im;

};
#endif

 
