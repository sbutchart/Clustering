#include "Clustering.hh"

void Clustering::RunClustering(){

  if (fERecoXMLFile == "") {
    std::cout << "Not using energy reconstruction!!" << std::endl;
  } else {
    fEReco = new ClusterEnergyEstimator(fERecoXMLFile);
  }

  fClustEng = new ClusterEngine();
  fClustEng->SetSorting(fSorting);

  int ConfigBegin = 0;
  int ConfigEnd   = fNConfig;
  if (fConfig != -1) {
    ConfigBegin = fConfig;
    ConfigEnd   = fConfig+1;
  }

  std::cout << "Running over: " << std::endl;
  std::cout << " - " << ConfigEnd - ConfigBegin << " configurations" << std::endl;
  std::cout << " - " << fNEvent << " events" << std::endl;
  size_t nMaxMarleyPerEvent=0;
  
  for (fCurrentEvent=fOffset; fCurrentEvent<fNEvent+fOffset; ++fCurrentEvent) {

    PrintProgress(fCurrentEvent,fNEvent);
    fSNIM->GetEntry(fCurrentEvent);
    fSOM->SetCurrentEvent(fCurrentEvent);
    
    bool goodEvent = true;

    if (fSNIM->True_MarlSample->size() > nMaxMarleyPerEvent)
      nMaxMarleyPerEvent = fSNIM->True_MarlSample->size();

    for(size_t MarleyEvent=0; MarleyEvent<fSNIM->True_MarlSample->size(); ++MarleyEvent) {
      goodEvent = (((*fSNIM->True_VertZ)[MarleyEvent] >  695) &&
                   ((*fSNIM->True_VertZ)[MarleyEvent] < 1160));
    }
    if(!goodEvent) continue;
    fSOM->FillTruthInfo(*fSNIM);

    //MAKE RECOHIT OBJECTS EVENTWISE FROM THE TREE.
    std::vector<WireHit*>    vec_WireHit;
    std::vector<OpticalHit*> vec_OptiHit;
    fSNIM->GetWireHits   (vec_WireHit);
    fSNIM->GetOpticalHits(vec_OptiHit);

    std::random_shuffle(vec_WireHit.begin(), vec_WireHit.end());
    // std::random_shuffle(vec_OptHit .begin(), vec_OptHit .end());
    // std::random_shuffle(vec_WireHit.begin(), vec_WireHit.end());
    // std::random_shuffle(vec_OptHit .begin(), vec_OptHit .end());
    // std::random_shuffle(vec_WireHit.begin(), vec_WireHit.end());
    // std::random_shuffle(vec_OptHit .begin(), vec_OptHit .end());

    for(fCurrentConfig=ConfigBegin; fCurrentConfig<ConfigEnd; ++fCurrentConfig) {
      fSOM->SetCurrentConfig(fCurrentConfig);
      fClustEng->SetTimeWindow   (fcut_TimeWindowSize  [fCurrentConfig]);
      fClustEng->SetChannelWidth (fcut_AdjChanTolerance[fCurrentConfig]);
      fClustEng->SetTimeWindowOpt(fcut_TimeWindowOpt   [fCurrentConfig]);
      fClustEng->SetPositionOpt  (fcut_PositionOpt     [fCurrentConfig]);
      fClustEng->SetBucketSize   (fcut_BucketSize      [fCurrentConfig]);
      
      fSimpleTrigger->SetWireNHitsMin    (fcut_HitsInWindow[fCurrentConfig]);
      fSimpleTrigger->SetWireNChanMin    (fcut_MinChannels [fCurrentConfig]);
      fSimpleTrigger->SetWireChanWidthMin(fcut_MinChanWidth[fCurrentConfig]);
      fSimpleTrigger->SetWireSADCMin     (fcut_TotalADC    [fCurrentConfig]);
      fSimpleTrigger->SetOptiNHitsMin    (fcut_OptHitInCluster[fCurrentConfig]);

      std::vector<WireCluster*>    vec_WireCluster;
      std::vector<OpticalCluster*> vec_OptiCluster;
      
      fClustEng->ClusterOpticalHits(vec_OptiHit, vec_OptiCluster);
      fClustEng->ClusterHits2      (vec_WireHit, vec_WireCluster);
      
      if (fEReco) fEReco->EstimateEnergy(vec_WireCluster);
      
      fTrigger->IsTriggering(vec_OptiCluster);
      fTrigger->IsTriggering(vec_WireCluster);
      
      fSOM->FillClusterTimingInfo(fClustEng,fEReco,vec_WireHit.size(), vec_OptiHit.size());
      
      if (fPrintLevel > -1) {
        std::cout << "----------------------------------------" << std::endl;
        std::cout << "Config        " << fCurrentConfig         << std::endl;
        std::cout << "nWireClusters " << vec_WireCluster.size() << std::endl;
        std::cout << "nOptiClusters " << vec_OptiCluster.size() << std::endl;
      }

      for(size_t i=0; i<vec_WireCluster.size(); ++i) {
        if (vec_WireCluster[i]->GetIsTriggering())
          fSOM->FillClusterInfo(vec_WireCluster[i]);
        delete vec_WireCluster[i];
        vec_WireCluster[i]=NULL;
      }

      for(size_t i=0; i<vec_OptiCluster.size(); ++i) {
        if (vec_OptiCluster[i]->GetIsTriggering())
          fSOM->FillClusterInfo(vec_OptiCluster[i]);
        delete vec_OptiCluster[i];
        vec_OptiCluster[i]=NULL;
      }
    }
    
    for(size_t j = 0; j < vec_WireHit.size(); j++) {
      delete vec_WireHit[j];
      vec_WireHit[j] = NULL;
    }
    for(size_t j = 0; j < vec_OptiHit.size(); j++) {
      delete vec_OptiHit[j];
      vec_OptiHit[j] = NULL;
    }
    vec_WireHit.clear();
    vec_OptiHit.clear();
  }
  
  fSOM->Write();
  
  if (nMaxMarleyPerEvent>1) {
    std::cout << "IMPORTANT WARNING: You have just ran over a file which has several SN interaction / LArSoft event." << std::endl;
    std::cout << "IMPORTANT WARNING: This means the optical cluster information is essentially unusable!!" << std::endl;
  }
}
