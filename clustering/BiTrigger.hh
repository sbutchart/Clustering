#ifndef BITRIGGER_HH
#define BITRIGGER_HH

#include "TCanvas.h"
#include "TGraph.h"
#include "TH2D.h"
#include "TPad.h"

#include "Cluster.hh"
#include "OpticalCluster.hh"
#include "Trigger.hh"
#include "WireCluster.hh"

struct ClusterEnsemble {
  ClusterEnsemble():
    nWireCluster      (0),
    nOpticalCluster   (0),
    nHitWireCluster   (0),
    nHitOpticalCluster(0),
    PeakWireCluster   (0),
    PeakOpticalCluster(0){};
  ClusterEnsemble(const ClusterEnsemble& c):
    nWireCluster      (c.nWireCluster      ),
    nOpticalCluster   (c.nOpticalCluster   ),
    nHitWireCluster   (c.nHitWireCluster   ),
    nHitOpticalCluster(c.nHitOpticalCluster),
    PeakWireCluster   (c.PeakWireCluster   ),
    PeakOpticalCluster(c.PeakOpticalCluster){};
  
  size_t nWireCluster;
  size_t nOpticalCluster;
  size_t nHitWireCluster;
  size_t nHitOpticalCluster;
  double PeakWireCluster;
  double PeakOpticalCluster;
  void AddCluster(const OpticalCluster& c) {
    ++nOpticalCluster;
    nHitOpticalCluster += c.GetNHit();
    PeakOpticalCluster += c.GetSumPeak();
  };
  void AddCluster(const WireCluster& c) {
    ++nWireCluster;
    nHitWireCluster += c.GetNHit();  
    PeakWireCluster += c.GetSumPeak();
  };
};

class BiTrigger: public Trigger{
public:
  BiTrigger():
    Trigger(),
    fTot_WireNCluster_Sign(0),
    fTot_WireNCluster_Back(0),
    fTot_OptiNCluster_Sign(0),
    fTot_OptiNCluster_Back(0){};
  void AddEventToStudy(const std::vector<std::unique_ptr<WireCluster>>& wc,
                       const std::vector<std::unique_ptr<OpticalCluster>>& oc);
  void FinishStudy();
  
  ~BiTrigger() {
    if (fPt_nClu_Sign) delete fPt_nClu_Sign;
    if (fPt_nClu_Back) delete fPt_nClu_Back;
    if (fPt_Peak_Sign) delete fPt_Peak_Sign;
    if (fPt_Peak_Back) delete fPt_Peak_Back;
    if (fPt_nHit_Sign) delete fPt_nHit_Sign;
    if (fPt_nHit_Back) delete fPt_nHit_Back;
    if (fLikelihood_nClu_Sign) delete fLikelihood_nClu_Sign;
    if (fLikelihood_nClu_Back) delete fLikelihood_nClu_Back;
    if (fLikelihood_Peak_Sign) delete fLikelihood_Peak_Sign;
    if (fLikelihood_Peak_Back) delete fLikelihood_Peak_Back;
    if (fLikelihood_nHit_Sign) delete fLikelihood_nHit_Sign;
    if (fLikelihood_nHit_Back) delete fLikelihood_nHit_Back;
    fPt_nClu_Sign = NULL;
    fPt_nClu_Back = NULL;
    fPt_Peak_Sign = NULL;
    fPt_Peak_Back = NULL;
    fPt_nHit_Sign = NULL;
    fPt_nHit_Back = NULL;
    fLikelihood_nClu_Sign = NULL;
    fLikelihood_nClu_Back = NULL;
    fLikelihood_Peak_Sign = NULL;
    fLikelihood_Peak_Back = NULL;
    fLikelihood_nHit_Sign = NULL;
    fLikelihood_nHit_Back = NULL;
  };
  
private:
  TGraph* fPt_nClu_Sign;
  TGraph* fPt_nClu_Back;
  TGraph* fPt_Peak_Sign;
  TGraph* fPt_Peak_Back;
  TGraph* fPt_nHit_Sign;
  TGraph* fPt_nHit_Back;

  TH2D* fLikelihood_nClu_Sign;
  TH2D* fLikelihood_nClu_Back;
  TH2D* fLikelihood_Peak_Sign;
  TH2D* fLikelihood_Peak_Back;
  TH2D* fLikelihood_nHit_Sign;
  TH2D* fLikelihood_nHit_Back;

  std::vector<ClusterEnsemble> fSignCluster;
  std::vector<ClusterEnsemble> fBackCluster;

  size_t fTot_WireNCluster_Sign;
  size_t fTot_WireNCluster_Back;
  size_t fTot_OptiNCluster_Sign;
  size_t fTot_OptiNCluster_Back;

};

#endif
