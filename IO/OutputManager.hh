#ifndef OUTPUTMANAGER_HH
#define OUTPUTMANAGER_HH

#include <vector>
#include <map>
#include <iostream>

#include "TFile.h"
#include "TTree.h"

#include "DataContainer/OpticalCluster.hh"
#include "DataContainer/WireCluster.hh"
#include "IO/InputManager.hh"
#include "Algorithm/ClusterEngine.hh"
#include "Algorithm/ClusterEnergyEstimator.hh"

class OutputManager {
  
protected:
  TFile* fOutput;
  std::string filename;
  std::map<std::string,TTree*> fTrees;

public:
  OutputManager(){};
  virtual ~OutputManager(){};
  
  void Fill() {
    for (auto& it: fTrees)
      it.second->Fill();
  };

  void Fill(std::string name) {
    bool found = false;
    for (auto& it: fTrees) {
      if (it.first == name) {
        found = true;
        it.second->Fill();
      }
    }
    if (found == false) {
      std::cout << "Cannot fill the tree " << name
                << " because it doesnt exist." << std::endl;
    }
  };

  void Write() {
    
    fOutput->cd();
    for (auto const& it: fTrees)
      it.second->Write();
  }
  
  std::string GetOutputFile() const {
    return filename;
  };

  std::vector<std::string> GetOutputTrees() const {
    std::vector<std::string> outputtrees;
    for (auto const& it: fTrees)
      outputtrees.push_back(it.first);
    return outputtrees;
  };

  void SetOutputFile(const std::string s="") { filename=s; };

  void AddOutputTree(const std::string s="") {
    if (fTrees.find(s) != fTrees.end()) {
      fTrees[s] = new TTree(s.c_str(),s.c_str());
    } else {
      std::cout << "This tree (" << s << ") already exists!" << std::endl;
    }
  };
  virtual void Initialise() {};
  
};


class StandardOutputManager: public OutputManager {
public:
  StandardOutputManager():
    fCurrentConfig      (0),
    fCurrentEvent       (0),
    fOutputOffset       (0),
    fWireClusterCount   (),
    fOpticalClusterCount()
    { };

  size_t GetCurrentConfig () const { return fCurrentConfig; };
  size_t GetCurrentEvent  () const { return fCurrentEvent ; };  
  void   SetCurrentConfig (size_t s=0) { fCurrentConfig = s; };
  void   SetCurrentEvent  (size_t s=0) { fCurrentEvent  = s; };  
  void   SetOutputOffset  (size_t s=0) { fOutputOffset  = s; };  

private:
  size_t fCurrentConfig;
  size_t fCurrentEvent ;
  size_t fOutputOffset ;
  std::map<size_t, size_t> fWireClusterCount;
  std::map<size_t, size_t> fOpticalClusterCount;

public:
  int nHitOpti;
  int nHitWire;
  int    Event         ;
  int    MarleyIndex   ;
  int    Cluster       ;
  int    Config        ;
  double StartChan     ;
  double EndChan       ;
  double APA           ;
  double ChanWidth     ;
  double NChan         ;
  double Type          ;
  double NHits         ;
  double SumADC        ;
  double FirstTimeHit  ;
  double LastTimeHit   ;
  double TimeWidth     ;
  double E_deposited   ;
  double NElectron     ;
  double EReco         ;
  double MC_UnderlyingE;
  double TrClusterPosX ;
  double TrClusterPosY ;
  double TrClusterPosZ ;
  double RecClusterPosX;
  double RecClusterPosY;
  double RecClusterPosZ;
  double YWidth        ;
  double ZWidth        ;
  double SumPE         ;

//  double pur_Other     ;
//  double pur_SNnu      ;
//  double pur_APA       ;
//  double pur_CPA       ;
//  double pur_Ar39      ;
//  double pur_Neutron   ;
//  double pur_Krypton   ;
//  double pur_Polonium  ;
//  double pur_Radon     ;
//  double pur_Ar42      ;

  std::map<std::string, double> dyn_purGenType;

  std::vector<double> TimeOrdering_WireClustTime        ;
  std::vector<double> SpaceOrdering_WireClustTime       ;
  std::vector<double> Clustering_WireClustTime          ;
  std::vector<double> EnergyReconstruction_WireClustTime;
  std::vector<double> TimeOrdering_OptClustTime         ;
  std::vector<double> SpaceOrdering_OptClustTime        ;
  std::vector<double> Clustering_OptClustTime           ;
  std::vector<double> EnergyReconstruction_OptClustTime ;
    
  std::vector<int>    HitView;
  std::vector<int>    GenType;
  std::vector<int>    HitChan;
  std::vector<double> HitTime;
  std::vector<double> HitSADC;
  std::vector<double> HitSPE ;
  std::vector<double> HitRMS ;
  std::vector<double> HitWidth;
  std::vector<double> TrPosX; 
  std::vector<double> TrPosY; 
  std::vector<double> TrPosZ; 

  std::vector<int>    PDSHit_GenType  ;
  std::vector<double> PDSHit_X        ;
  std::vector<double> PDSHit_Y        ;
  std::vector<double> PDSHit_Z        ;
  std::vector<double> PDSHit_PeakTime ;
  std::vector<double> PDSHit_Width    ;
  std::vector<double> PDSHit_PE       ;
  std::vector<double> PDSHit_OpChannel;
 
  std::vector<double> MarlTime;
  std::vector<double> ENu     ;
  std::vector<double> ENu_Lep ;
  std::vector<double> PosX;
  std::vector<double> PosY;
  std::vector<double> PosZ;
  std::vector<double> PosT;
  std::vector<double> DirX;
  std::vector<double> DirY;
  std::vector<double> DirZ;

  void Initialise() {
    fOutput = new TFile(filename.c_str(), "RECREATE");
    fTrees["WireHitClusters"] = new TTree("ClusteredWireHit", "ClusteredWireHit");
    fTrees["WireHitClusters"]->Branch("Cluster",        &Cluster,        "Cluster/I"       );
    fTrees["WireHitClusters"]->Branch("Event",          &Event,          "Event/I"         );
    fTrees["WireHitClusters"]->Branch("Config",         &Config,         "Config/I"        );
    fTrees["WireHitClusters"]->Branch("MarleyIndex",    &MarleyIndex,    "MarleyIndex/I"   );
    fTrees["WireHitClusters"]->Branch("APA",            &APA,            "APA/D"           );
    fTrees["WireHitClusters"]->Branch("StartChan",      &StartChan,      "StartChan/D"     );
    fTrees["WireHitClusters"]->Branch("EndChan",        &EndChan,        "EndChan/D"       );
    fTrees["WireHitClusters"]->Branch("ChanWidth",      &ChanWidth,      "ChanWidth/D"     );
    fTrees["WireHitClusters"]->Branch("NChan",          &NChan,          "NChan/D"         );
    fTrees["WireHitClusters"]->Branch("Type",           &Type,           "Type/D"          );
    fTrees["WireHitClusters"]->Branch("NHits",          &NHits,          "NHits/D"         );
    fTrees["WireHitClusters"]->Branch("SumADC",         &SumADC,         "SumADC/D"        );
    fTrees["WireHitClusters"]->Branch("FirstTimeHit",   &FirstTimeHit,   "FirstTimeHit/D"  );
    fTrees["WireHitClusters"]->Branch("LastTimeHit",    &LastTimeHit,    "LastTimeHit/D"   );
    fTrees["WireHitClusters"]->Branch("TimeWidth",      &TimeWidth,      "TimeWidth/D"     );
    fTrees["WireHitClusters"]->Branch("EReco",          &EReco,          "EReco/D"         );
    fTrees["WireHitClusters"]->Branch("E_deposited",    &E_deposited,    "E_deposited/D"   );
    fTrees["WireHitClusters"]->Branch("NElectron",      &NElectron,      "NElectron/D"     );
    fTrees["WireHitClusters"]->Branch("MC_UnderlyingE", &MC_UnderlyingE, "MC_UnderlyingE/D");
    fTrees["WireHitClusters"]->Branch("RecClusterPosX", &RecClusterPosX, "RecClusterPosX/D");
    fTrees["WireHitClusters"]->Branch("RecClusterPosY", &RecClusterPosY, "RecClusterPosY/D");
    fTrees["WireHitClusters"]->Branch("RecClusterPosZ", &RecClusterPosZ, "RecClusterPosZ/D");
    fTrees["WireHitClusters"]->Branch("TrClusterPosX",  &TrClusterPosX,  "TrClusterPosX/D" );
    fTrees["WireHitClusters"]->Branch("TrClusterPosY",  &TrClusterPosY,  "TrClusterPosY/D" );
    fTrees["WireHitClusters"]->Branch("TrClusterPosZ",  &TrClusterPosZ,  "TrClusterPosZ/D" );

    for (auto &x : dyn_purGenType)
      {
        std::stringstream branch;
        branch << "pur_" << x.first;
        std::cout << branch.str() << std::endl;
        std::stringstream leaf;
        leaf << branch.rdbuf() << "/D";
        std::cout << leaf.str() << std::endl;
        
        fTrees["WireHitClusters"]->Branch(branch.str().c_str(), &x.second, leaf.str().c_str() );
      }

//    fTrees["WireHitClusters"]->Branch("pur_Other"   ,   &pur_Other   ,   "pur_Other/D"     );
//    fTrees["WireHitClusters"]->Branch("pur_SNnu"    ,   &pur_SNnu    ,   "pur_SNnu/D"      );
//    fTrees["WireHitClusters"]->Branch("pur_APA"     ,   &pur_APA     ,   "pur_APA/D"       );
//    fTrees["WireHitClusters"]->Branch("pur_CPA"     ,   &pur_CPA     ,   "pur_CPA/D"       );
//    fTrees["WireHitClusters"]->Branch("pur_Ar39"    ,   &pur_Ar39    ,   "pur_Ar39/D"      );
//    fTrees["WireHitClusters"]->Branch("pur_Neutron" ,   &pur_Neutron ,   "pur_Neutron/D"   );
//    fTrees["WireHitClusters"]->Branch("pur_Krypton" ,   &pur_Krypton ,   "pur_Krypton/D"   );
//    fTrees["WireHitClusters"]->Branch("pur_Polonium",   &pur_Polonium,   "pur_Polonium/D"  );
//    fTrees["WireHitClusters"]->Branch("pur_Radon"   ,   &pur_Radon   ,   "pur_Radon/D"     );
//    fTrees["WireHitClusters"]->Branch("pur_Ar42"    ,   &pur_Ar42    ,   "pur_Ar42/D"      );
    fTrees["WireHitClusters"]->Branch("HitView",        &HitView);
    fTrees["WireHitClusters"]->Branch("GenType",        &GenType);
    fTrees["WireHitClusters"]->Branch("HitChan",        &HitChan);
    fTrees["WireHitClusters"]->Branch("HitTime",        &HitTime);
    fTrees["WireHitClusters"]->Branch("HitSADC",        &HitSADC);
    fTrees["WireHitClusters"]->Branch("HitRMS",         &HitRMS );
    fTrees["WireHitClusters"]->Branch("TrPosX",         &TrPosX );
    fTrees["WireHitClusters"]->Branch("TrPosY",         &TrPosY );
    fTrees["WireHitClusters"]->Branch("TrPosZ",         &TrPosZ );

    fTrees["OpticalHitClusters"] = new TTree("ClusteredOpticalHit", "ClusteredOpticalHit");
    fTrees["OpticalHitClusters"]->Branch("Config",         &Config,         "Config/I"        );
    fTrees["OpticalHitClusters"]->Branch("Cluster",        &Cluster,        "Cluster/I"       );
    fTrees["OpticalHitClusters"]->Branch("MarleyIndex",    &MarleyIndex,    "MarleyIndex/I"   );
    fTrees["OpticalHitClusters"]->Branch("Event",          &Event,          "Event/I"         );
    fTrees["OpticalHitClusters"]->Branch("APA",            &APA,            "APA/D"           );
    fTrees["OpticalHitClusters"]->Branch("YWidth",         &YWidth,         "YWidth/D"        );
    fTrees["OpticalHitClusters"]->Branch("ZWidth",         &ZWidth,         "ZWidth/D"        );
    fTrees["OpticalHitClusters"]->Branch("NChan",          &NChan,          "NChan/D"         );
    fTrees["OpticalHitClusters"]->Branch("Type",           &Type,           "Type/D"          );
    fTrees["OpticalHitClusters"]->Branch("NHits",          &NHits,          "NHits/D"         );
    fTrees["OpticalHitClusters"]->Branch("SumPE",          &SumPE,          "SumPE/D"         );
    fTrees["OpticalHitClusters"]->Branch("FirstTimeHit",   &FirstTimeHit,   "FirstTimeHit/D"  );
    fTrees["OpticalHitClusters"]->Branch("LastTimeHit",    &LastTimeHit,    "LastTimeHit/D"   );
    fTrees["OpticalHitClusters"]->Branch("TimeWidth",      &TimeWidth,      "TimeWidth/D"     );
    fTrees["OpticalHitClusters"]->Branch("RecClusterPosX", &RecClusterPosX, "RecClusterPosX/D");
    fTrees["OpticalHitClusters"]->Branch("RecClusterPosY", &RecClusterPosY, "RecClusterPosY/D");
    fTrees["OpticalHitClusters"]->Branch("RecClusterPosZ", &RecClusterPosZ, "RecClusterPosZ/D");
    fTrees["OpticalHitClusters"]->Branch("Hit_GenType"     , &PDSHit_GenType  );
    fTrees["OpticalHitClusters"]->Branch("Hit_X"           , &PDSHit_X        );
    fTrees["OpticalHitClusters"]->Branch("Hit_Y"           , &PDSHit_Y        );
    fTrees["OpticalHitClusters"]->Branch("Hit_Z"           , &PDSHit_Z        );
    fTrees["OpticalHitClusters"]->Branch("Hit_PeakTime"    , &PDSHit_PeakTime );
    fTrees["OpticalHitClusters"]->Branch("Hit_Width"       , &PDSHit_Width    );
    fTrees["OpticalHitClusters"]->Branch("Hit_PE"          , &PDSHit_PE       );
    fTrees["OpticalHitClusters"]->Branch("Hit_OpChannel"   , &PDSHit_OpChannel);

    fTrees["TrueInfo"] = new TTree("TrueInfo", "TrueInfo");
    fTrees["TrueInfo"]->Branch("Event",    &Event, "Event/I");
    fTrees["TrueInfo"]->Branch("MarlTime", &MarlTime);
    fTrees["TrueInfo"]->Branch("ENu",      &ENu     );
    fTrees["TrueInfo"]->Branch("ENu_Lep",  &ENu_Lep );
    fTrees["TrueInfo"]->Branch("PosX",     &PosX    );
    fTrees["TrueInfo"]->Branch("PosY",     &PosY    );
    fTrees["TrueInfo"]->Branch("PosZ",     &PosZ    );
    fTrees["TrueInfo"]->Branch("PosT",     &PosT    );
    fTrees["TrueInfo"]->Branch("DirX",     &DirX    );
    fTrees["TrueInfo"]->Branch("DirY",     &DirY    );
    fTrees["TrueInfo"]->Branch("DirZ",     &DirZ    );

    fTrees["TimingInfo"] = new TTree("TimingInfo", "TimingInfo");
    fTrees["TimingInfo"]->Branch("Event",  &Event,  "Event/I" );
    fTrees["TimingInfo"]->Branch("Config", &Config, "Config/I");
    fTrees["TimingInfo"]->Branch("nHitWire",   &nHitWire,  "nHitWire/I");
    fTrees["TimingInfo"]->Branch("nHitOpti",   &nHitOpti,  "nHitOpti/I");
    fTrees["TimingInfo"]->Branch("TimeOrdering_WireClustTime",         &TimeOrdering_WireClustTime        );
    fTrees["TimingInfo"]->Branch("SpaceOrdering_WireClustTime",        &SpaceOrdering_WireClustTime       );
    fTrees["TimingInfo"]->Branch("Clustering_WireClustTime",           &Clustering_WireClustTime          );
    fTrees["TimingInfo"]->Branch("EnergyReconstruction_WireClustTime", &EnergyReconstruction_WireClustTime);
    fTrees["TimingInfo"]->Branch("TimeOrdering_OptClustTime",          &TimeOrdering_OptClustTime         );
    fTrees["TimingInfo"]->Branch("SpaceOrdering_OptClustTime",         &SpaceOrdering_OptClustTime        );
    fTrees["TimingInfo"]->Branch("Clustering_OptClustTime",            &Clustering_OptClustTime           );
    fTrees["TimingInfo"]->Branch("EnergyReconstruction_OptClustTime",  &EnergyReconstruction_OptClustTime );
  };

  
  void FillClusterInfo(OpticalCluster* clust) {
    Config         = fCurrentConfig;
    Cluster        = fOpticalClusterCount[fCurrentConfig];
    Event          = fCurrentEvent+fOutputOffset;
    MarleyIndex    = clust->GetMarleyIndex();
    APA            = clust->GetAPA();
    YWidth         = clust->GetSize(kY);
    ZWidth         = clust->GetSize(kZ);
    NChan          = clust->GetNChannel();
    Type           = clust->GetType();
    NHits          = clust->GetNHit();
    SumPE          = clust->GetHitSPE();
    FirstTimeHit   = clust->GetFirstHitTime();
    LastTimeHit    = clust->GetLastHitTime();
    TimeWidth      = clust->GetTimeWidth();
    RecClusterPosX = clust->GetPosition(kX);
    RecClusterPosY = clust->GetPosition(kY);
    RecClusterPosZ = clust->GetPosition(kZ);

    PDSHit_GenType  .clear();
    PDSHit_X        .clear();
    PDSHit_Y        .clear();
    PDSHit_Z        .clear();
    PDSHit_PeakTime .clear();
    PDSHit_Width    .clear();
    PDSHit_PE       .clear();
    PDSHit_OpChannel.clear();
    
    for(auto const& it : clust->GetHit()) {
      OpticalHit* hit = (OpticalHit*)it;
      PDSHit_GenType  .push_back(hit->GetGenTypeInt());
      PDSHit_X        .push_back(hit->GetPosition(kX));
      PDSHit_Y        .push_back(hit->GetPosition(kY));
      PDSHit_Z        .push_back(hit->GetPosition(kZ));
      PDSHit_PeakTime .push_back(hit->GetTime   ());
      PDSHit_Width    .push_back(hit->GetWidth  ());
      PDSHit_PE       .push_back(hit->GetSPE    ());
      PDSHit_OpChannel.push_back(hit->GetChannel());
    }

    fTrees["OpticalHitClusters"]->Fill();
  };
  
  void FillClusterInfo(WireCluster* clust) {
    Event          = fCurrentEvent+fOutputOffset;
    Config         = fCurrentConfig;
    Cluster        = fWireClusterCount[fCurrentConfig];
    MarleyIndex    = clust->GetMarleyIndex();
    APA            = clust->GetAPA();
    StartChan      = clust->GetStartChannel();
    EndChan        = clust->GetEndChannel();
    ChanWidth      = clust->GetChannelWidth();
    NChan          = clust->GetNChannel();
    Type           = clust->GetType();
    NHits          = clust->GetNHit();
    SumADC         = clust->GetHitSumADC();
    FirstTimeHit   = clust->GetFirstHitTime();
    LastTimeHit    = clust->GetLastHitTime();
    TimeWidth      = clust->GetTimeWidth();
    EReco          = clust->GetRecoEnergy();
    MC_UnderlyingE = clust->GetMC_UnderlyingE();
    NElectron      = clust->GetNElectron();
    TrClusterPosX  = clust->GetTruePosition(kX);
    TrClusterPosY  = clust->GetTruePosition(kY);
    TrClusterPosZ  = clust->GetTruePosition(kZ);
    RecClusterPosX = clust->GetRecoPosition(kX);
    RecClusterPosY = clust->GetRecoPosition(kY);
    RecClusterPosZ = clust->GetRecoPosition(kZ);

    for (auto const& x : dyn_AllGenType)
      {
        dyn_purGenType[x] = clust->GetPurity(x);
      }
 
//    pur_Other      = clust->GetPurity(kOther   );
//    pur_SNnu       = clust->GetPurity(kSNnu    );
//    pur_APA        = clust->GetPurity(kAPA     );
//    pur_CPA        = clust->GetPurity(kCPA     );
//    pur_Ar39       = clust->GetPurity(kAr39    );
//    pur_Neutron    = clust->GetPurity(kNeutron );
//    pur_Krypton    = clust->GetPurity(kKrypton );
//    pur_Polonium   = clust->GetPurity(kPolonium);
//    pur_Radon      = clust->GetPurity(kRadon   );
//    pur_Ar42       = clust->GetPurity(kAr42    );

    HitView.clear();
    GenType.clear();
    HitChan.clear();
    HitTime.clear();
    HitSADC.clear();
    HitRMS .clear();
    TrPosX .clear();
    TrPosY .clear();
    TrPosZ .clear();
    
    std::vector<Hit*> hits = clust->GetHit();
    E_deposited=0.;

    for(auto const& hit : hits) {
      WireHit* wh = (WireHit*)hit;
      HitView.push_back(wh->GetHitView());
      GenType.push_back(wh->GetGenTypeInt());
      HitChan.push_back(wh->GetHitChan());
      HitTime.push_back(wh->GetHitTime());
      HitSADC.push_back(wh->GetHitSADC());
      HitRMS .push_back(wh->GetHitRMS ());
      TrPosX .push_back(wh->GetTruePosition(kX));
      TrPosY .push_back(wh->GetTruePosition(kY));
      TrPosZ .push_back(wh->GetTruePosition(kZ));
      E_deposited += wh->GetTrDepositedE();
    }

    fTrees["WireHitClusters"]->Fill();
  };

  void FillClusterTimingInfo(ClusterEngine* clusterEngine,
                             ClusterEnergyEstimator* clusterEReco = NULL,
                             int nWire=0, int nOpti=0){
    TimeOrdering_OptClustTime  .clear();
    TimeOrdering_WireClustTime .clear();
    SpaceOrdering_OptClustTime .clear();
    SpaceOrdering_WireClustTime.clear();
    Clustering_OptClustTime    .clear();
    Clustering_WireClustTime   .clear();
    EnergyReconstruction_WireClustTime.clear();
    EnergyReconstruction_OptClustTime = {0};

    nHitWire=nWire;
    nHitOpti=nOpti;

    Config = fCurrentConfig;
    Event  = fCurrentEvent+fOutputOffset;
    
    TimeOrdering_OptClustTime   = clusterEngine->GetElapsedTime_TimeOrdering ();
    TimeOrdering_WireClustTime  = clusterEngine->GetElapsedTime_TimeOrdering ();
    SpaceOrdering_WireClustTime = clusterEngine->GetElapsedTime_SpaceOrdering();
    SpaceOrdering_OptClustTime  = clusterEngine->GetElapsedTime_SpaceOrdering();
    Clustering_WireClustTime    = clusterEngine->GetElapsedTime_Clustering   ();
    Clustering_OptClustTime     = clusterEngine->GetElapsedTime_Clustering   ();

    if (clusterEReco)
      EnergyReconstruction_WireClustTime = clusterEReco->GetElapsedTime();

    fTrees["TimingInfo"]->Fill();
  };


  void FillTruthInfo(SNAnaInputManager& im) {
    MarlTime.clear();
    ENu     .clear();
    ENu_Lep .clear();
    PosX    .clear();
    PosY    .clear();
    PosZ    .clear();
    PosT    .clear();
    DirX    .clear();
    DirY    .clear();
    DirZ    .clear();
    
    for(size_t MarleyEvent=0; MarleyEvent<im.True_VertZ->size(); ++MarleyEvent) {
      Event = fCurrentEvent+fOutputOffset;
      ENu     .push_back((*im.True_ENu)     [MarleyEvent]);
      ENu_Lep .push_back((*im.True_ENu_Lep) [MarleyEvent]);
      PosX    .push_back((*im.True_VertX)   [MarleyEvent]);
      PosY    .push_back((*im.True_VertY)   [MarleyEvent]);
      PosZ    .push_back((*im.True_VertZ)   [MarleyEvent]);
      PosT    .push_back((*im.True_VertexT) [MarleyEvent]);
      DirX    .push_back((*im.True_Dirx)    [MarleyEvent]);
      DirY    .push_back((*im.True_Diry)    [MarleyEvent]);
      DirZ    .push_back((*im.True_Dirz)    [MarleyEvent]);
    }
    for(size_t MarleyEvent=0; MarleyEvent<im.True_MarlTime->size(); ++MarleyEvent) {
      MarlTime.push_back((*im.True_MarlTime)[MarleyEvent]);
    }    
    fTrees["TrueInfo"]->Fill();
  }
};


#endif
