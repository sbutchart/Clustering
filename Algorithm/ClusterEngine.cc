#include "ClusterEngine.hh"


inline bool HitOrderedInPosX(const Hit* lhs, const Hit* rhs){ return (lhs->fPosition.at(kX) < rhs->fPosition.at(kX)); }
inline bool HitOrderedInPosY(const Hit* lhs, const Hit* rhs){ return (lhs->fPosition.at(kY) < rhs->fPosition.at(kY)); }
inline bool HitOrderedInPosZ(const Hit* lhs, const Hit* rhs){ return (lhs->fPosition.at(kZ) < rhs->fPosition.at(kZ)); }
inline bool HitOrderedInChan(const Hit* lhs, const Hit* rhs){ return (lhs->fChannel         < rhs->fChannel        ); }
inline bool HitOrderedInTime(const Hit* lhs, const Hit* rhs){ return (lhs->fPosition.at(kT) < rhs->fPosition.at(kT)); }

double GetCenterTime(WireCluster* clust) { return double(clust->GetFirstHitTime() + clust->GetLastHitTime()) / 2.0; }

void ClusterEngine::ClusterHits2_stable_sort(std::vector<WireHit*>& vec_Hits,
                                             std::vector<WireCluster*>& vec_Clusters) {

  ResetTime();
  fStopwatch0 = clock();
  std::stable_sort(vec_Hits.begin(), vec_Hits.end(), HitOrderedInChan);
  SpaceOrdering_Time.push_back(((float)(clock()-fStopwatch0))/((float)CLOCKS_PER_SEC));
   
  vec_Clusters.clear();

  if(vec_Hits.size() < 2) return;
  for(size_t i = 0; i < vec_Hits.size()-1; i++){

    std::vector<WireHit*> vec_TempHits;
    if(std::abs(vec_Hits[i]->GetHitChan()-vec_Hits[i+1]->GetHitChan())<=fChannelWidth)
    {
      int channelCount = 1;

      vec_TempHits.push_back(vec_Hits[i  ]);
      vec_TempHits.push_back(vec_Hits[i+1]);

      while((i+channelCount+1)<vec_Hits.size() &&
            std::abs(vec_Hits[i+channelCount]->GetHitChan()-vec_Hits[i+channelCount+1]->GetHitChan()) <= fChannelWidth)
      {
        vec_TempHits.push_back(vec_Hits[i + channelCount + 1]);
        channelCount++;
      }

      i = i + channelCount;
      fStopwatch1 = clock();
      std::stable_sort(vec_TempHits.begin(), vec_TempHits.end(), HitOrderedInTime);
      TimeOrdering_Time.push_back((float(clock() - fStopwatch1))/((float)CLOCKS_PER_SEC));

      for(size_t j = 0; j < vec_TempHits.size()-1; j++)
      {
        std::vector<Hit*> vec_TempHits2;
        if(std::abs(vec_TempHits[j]->GetHitTime()-vec_TempHits[j+1]->GetHitTime())<=fTimeWindow)
        {
          int timeCount = 1;

          vec_TempHits2.push_back(vec_TempHits[j  ]);
          vec_TempHits2.push_back(vec_TempHits[j+1]);

          while((j+timeCount+1)<vec_TempHits.size() &&
                std::abs(vec_TempHits[j+timeCount]->GetHitTime()-vec_TempHits[j+timeCount+1]->GetHitTime())
                <=fTimeWindow)
          {
            if(vec_TempHits[j + timeCount + 1]->GetHitSADC()>fMinHitADC)
              vec_TempHits2.push_back(vec_TempHits[j + timeCount + 1]);
            timeCount++;
          }

          j = j + timeCount;
          vec_Clusters.push_back(new WireCluster(vec_TempHits2));
          vec_TempHits2.clear();
        }
      }
      vec_TempHits.clear();
    }
  }
  Clustering_Time.push_back(((float)(clock() - fStopwatch0))/((float)CLOCKS_PER_SEC));
  
}


void ClusterEngine::ClusterHits2_sort(std::vector<WireHit*>& vec_Hits,
                                      std::vector<WireCluster*>& vec_Clusters) {

  ResetTime();
  fStopwatch0 = clock();
  std::sort(vec_Hits.begin(), vec_Hits.end(), HitOrderedInChan);
  SpaceOrdering_Time.push_back(((float)(clock()-fStopwatch0))/((float)CLOCKS_PER_SEC));
   
  vec_Clusters.clear();
  
  if(vec_Hits.size() < 2) return;
  for(size_t i = 0; i < vec_Hits.size()-1; i++){
    std::vector<WireHit*> vec_TempHits;
    if(std::abs(vec_Hits[i]->GetHitChan()-vec_Hits[i+1]->GetHitChan())<=fChannelWidth)
    {
      int channelCount = 1;
      
      vec_TempHits.push_back(vec_Hits[i  ]);
      vec_TempHits.push_back(vec_Hits[i+1]);

      while((i+channelCount+1)<vec_Hits.size() &&
            std::abs(vec_Hits[i+channelCount]->GetHitChan()-vec_Hits[i+channelCount+1]->GetHitChan()) <= fChannelWidth)
      {
        vec_TempHits.push_back(vec_Hits[i + channelCount + 1]);
        channelCount++;
      }

      i = i + channelCount;
      fStopwatch1 = clock();
      std::sort(vec_TempHits.begin(), vec_TempHits.end(), HitOrderedInTime);
      TimeOrdering_Time.push_back((float(clock() - fStopwatch1))/((float)CLOCKS_PER_SEC));

      for(size_t j = 0; j < vec_TempHits.size()-1; j++)
      {
        std::vector<Hit*> vec_TempHits2;
        if(std::abs(vec_TempHits[j]->GetHitTime()-vec_TempHits[j+1]->GetHitTime())<=fTimeWindow)
        {
          int timeCount = 1;
                                     
          vec_TempHits2.push_back(vec_TempHits[j  ]);
          vec_TempHits2.push_back(vec_TempHits[j+1]);

          while((j+timeCount+1)<vec_TempHits.size() && 
                std::abs(vec_TempHits[j+timeCount]->GetHitTime()-vec_TempHits[j+timeCount+1]->GetHitTime())
                <=fTimeWindow)
          {
            if(vec_TempHits[j + timeCount + 1]->GetHitSADC()>fMinHitADC)
              vec_TempHits2.push_back(vec_TempHits[j + timeCount + 1]);
            timeCount++;
          }

          j = j + timeCount;
          vec_Clusters.push_back(new WireCluster(vec_TempHits2));
          vec_TempHits2.clear();
          //fVecClusters.push_back(temp);
        }
      }
      vec_TempHits.clear();
    }
  }
  Clustering_Time.push_back(((float)(clock() - fStopwatch0))/((float)CLOCKS_PER_SEC));
  
}


void ClusterEngine::ClusterOpticalHits(std::vector<OpticalHit*>& vec_OptHitIn,
                                       std::vector<OpticalCluster*>& vec_OptCluster) {

  std::vector<OpticalHit*> vec_OptHit;
  for (auto const& it: vec_OptHitIn) {
    if (it->GetSPE() > fPreCutPEOpt) {
      vec_OptHit.push_back(it);
    }
  }
  ResetTime();
  fStopwatch0 = clock();
  std::sort(vec_OptHit.begin(), vec_OptHit.end(), HitOrderedInTime);
  TimeOrdering_Time.push_back(((float)(clock() - fStopwatch0))/((float)CLOCKS_PER_SEC));
  vec_OptCluster.clear();

  if(vec_OptHit.size() < 2) return;
  for(size_t i = 0; i < vec_OptHit.size()-1; i++){
    
    std::vector<OpticalHit*> vec_TempHit;
    if(std::abs(vec_OptHit[i]->GetTime()-vec_OptHit[i+1]->GetTime())<=fTimeWindowOpt)
    {
      int channelCount = 1;

      vec_TempHit.push_back(vec_OptHit[i  ]);
      vec_TempHit.push_back(vec_OptHit[i+1]);

      while((i+channelCount+1)<vec_OptHit.size() &&
            std::abs(vec_OptHit[i+channelCount]->GetTime()-vec_OptHit[i+channelCount+1]->GetTime())<=fTimeWindowOpt)
      {
        vec_TempHit.push_back(vec_OptHit[i + channelCount + 1]);
        channelCount++;
      }

      i = i + channelCount;
      std::map<OpticalHit*, std::vector<OpticalHit*>> bucket;
      for (size_t ii = 0; ii < vec_TempHit.size(); ii++){
          // bucket[vec_TempHit[ii]].push_back(vec_TempHit[0]);
          for(size_t j = ii; j < vec_TempHit.size(); j++){
              double time_diff = vec_TempHit[j]->GetTime() -  vec_TempHit[ii]->GetTime();
              if (time_diff < fBucketSize)
                  bucket[vec_TempHit[ii]].push_back(vec_TempHit[j]);
          }
      }
      size_t max_threshold = 0;
      OpticalHit* optical_hit_idx;
      for (size_t iii=0; iii<vec_TempHit.size(); iii++){
          size_t bucket_size = bucket[vec_TempHit[iii]].size();
          if (bucket_size > max_threshold){
              max_threshold = bucket_size;
              optical_hit_idx = vec_TempHit[iii];
          }
      }

      //std::sort(vec_TempHit.begin(), vec_TempHit.end(), OpticalHitOrderedInSpacePtr);
      //if(bucket[optical_hit_idx].size() > 4 )
      // std::cout << "vec_TempHit " << vec_TempHit.size() << "\t" ;
      // std::cout << "bucket size " << bucket[optical_hit_idx].size() << std::endl;
      fStopwatch1 = clock();

      std::sort(bucket[optical_hit_idx].begin(), bucket[optical_hit_idx].end(), HitOrderedInPosZ);
      SpaceOrdering_Time.push_back(((float)(clock() - fStopwatch1))/((float)CLOCKS_PER_SEC));
      for(size_t j = 0; j < bucket[optical_hit_idx].size()-1; j++){
        std::vector<Hit*> vec_TempHit2;
        if(std::abs(bucket[optical_hit_idx][j]->GetRecoPosition(2)-bucket[optical_hit_idx][j+1]->GetRecoPosition(2))<=fPositionOpt)
        {
          int PosCount = 1;

          vec_TempHit2.push_back(bucket[optical_hit_idx][j  ]);
          vec_TempHit2.push_back(bucket[optical_hit_idx][j+1]);

          while((j+PosCount+1)<bucket[optical_hit_idx].size() &&
                std::abs(bucket[optical_hit_idx][j+PosCount]->GetRecoPosition(2)-bucket[optical_hit_idx][j+PosCount+1]->GetRecoPosition(2))<=fPositionOpt)
          {
            vec_TempHit2.push_back(bucket[optical_hit_idx][j + PosCount + 1]);
            PosCount++;
          }

          j = j + PosCount;
          vec_OptCluster.push_back(new OpticalCluster(vec_TempHit2));
        }
      }
    }
  }
  Clustering_Time.push_back(((float)(clock() - fStopwatch0))/((float)CLOCKS_PER_SEC));

}


void ClusterEngine::ClusterOpticalHits2(std::vector<OpticalHit*>& vec_OptHit,
                                       std::vector<OpticalCluster*>& vec_OptCluster) {

  std::sort(vec_OptHit.begin(), vec_OptHit.end(), HitOrderedInTime);
  vec_OptCluster.clear();

  if(vec_OptHit.size() < 2) return;
  for(size_t i = 0; i < vec_OptHit.size()-1; i++){

    std::vector<OpticalHit*> vec_TempHit;
    if(std::abs(vec_OptHit[i]->GetTime()-vec_OptHit[i+1]->GetTime())<=fTimeWindowOpt)
    {
      int channelCount = 1;

      vec_TempHit.push_back(vec_OptHit[i  ]);
      vec_TempHit.push_back(vec_OptHit[i+1]);

      while((i+channelCount+1)<vec_OptHit.size() &&
            std::abs(vec_OptHit[i+channelCount]->GetTime()-vec_OptHit[i+channelCount+1]->GetTime())<=fTimeWindowOpt)
      {
        vec_TempHit.push_back(vec_OptHit[i + channelCount + 1]);
        channelCount++;
      }

      i = i + channelCount;
      std::sort(vec_TempHit.begin(), vec_TempHit.end(), HitOrderedInPosZ);
      for(size_t j = 0; j < vec_TempHit.size()-1; j++){

        std::vector<Hit*> vec_TempHit2;
        if(std::abs(vec_TempHit[j]->GetRecoPosition(2)-vec_TempHit[j+1]->GetRecoPosition(2))<=fPositionOpt)
        {
          int PosCount = 1;

          vec_TempHit2.push_back(vec_TempHit[j  ]);
          vec_TempHit2.push_back(vec_TempHit[j+1]);

          while((j+PosCount+1)<vec_TempHit.size() &&
                std::abs(vec_TempHit[j+PosCount]->GetRecoPosition(2)-vec_TempHit[j+PosCount+1]->GetRecoPosition(2))<=fPositionOpt)
          {
            vec_TempHit2.push_back(vec_TempHit[j + PosCount + 1]);
            PosCount++;
          }

          j = j + PosCount;
          vec_OptCluster.push_back(new OpticalCluster(vec_TempHit2));
        }
      }
    }
  }
}


void ClusterEngine::ClusterHits3(const std::vector<WireHit*>& vec_Hits,
                                 std::vector<WireCluster*>& vec_Clusters) {
  vec_Clusters.clear();
  if(vec_Hits.size() < 2) return;
  for(size_t i = 0; i < vec_Hits.size()-1; i++){

    std::vector<Hit*> vec_TempHits;
    if(std::abs(vec_Hits[i]->GetHitChan()-vec_Hits[i+1]->GetHitChan())<=fChannelWidth &&
       std::abs(vec_Hits[i]->GetHitTime()-vec_Hits[i+1]->GetHitTime())<=fTimeWindow)
    {
      int channelCount = 1;

      vec_TempHits.push_back(vec_Hits[i  ]);
      vec_TempHits.push_back(vec_Hits[i+1]);

      while((i+channelCount+1)<vec_Hits.size() &&
            std::abs(vec_Hits[i+channelCount]->GetHitChan()-vec_Hits[i+channelCount+1]->GetHitChan()) <=fChannelWidth &&
            std::abs(vec_Hits[i+channelCount]->GetHitTime()-vec_Hits[i+channelCount+1]->GetHitTime()) <= fTimeWindow)
      {
        vec_TempHits.push_back(vec_Hits[i + channelCount + 1]);
        channelCount++;
      }

      i = i + channelCount;
      vec_Clusters.push_back(new WireCluster(vec_TempHits));
      vec_TempHits.clear();
    }
  }

}


void ClusterEngine::ClusterHits(const std::vector<WireHit*>& vec_Hits,
                                std::vector<WireCluster*>& vec_Clusters) {
  vec_Clusters.clear();

  std::vector<WireHit*> LocalHits = vec_Hits;
  std::sort(LocalHits.begin(), LocalHits.end(), HitOrderedInPosZ);

  for(std::vector<WireHit*>::iterator it_hit1 = LocalHits.begin();
      it_hit1 != LocalHits.end()-1;++it_hit1)
  {
    std::vector<Hit*> vec_ClusteredHits;
    vec_ClusteredHits.clear();

    WireHit* hit1 = *it_hit1;
    bool hit1InACluster = false;
    int i = 0;
    for(std::vector<WireHit*>::iterator it_hit2 = it_hit1+1;
        it_hit2 != LocalHits.end();++it_hit2)
    {
      //
      i++;
      WireHit* hit2 = *it_hit2;
      double chanDiff = std::abs(hit1->GetHitChan()-
                                 hit2->GetHitChan());
      double timeDiff = std::abs(hit1->GetHitTime()-
                                 hit2->GetHitTime());

      if(chanDiff<fChannelWidth &&
         timeDiff<fTimeWindow   &&
         std::find(vec_ClusteredHits.begin(), vec_ClusteredHits.end(), hit2) == vec_ClusteredHits.end())
      {
        hit1InACluster = true;

        vec_ClusteredHits.push_back(hit2);
      }
    }

    if(hit1InACluster &&
       std::find(vec_ClusteredHits.begin(), vec_ClusteredHits.end(), hit1) == vec_ClusteredHits.end()){
      vec_ClusteredHits.push_back(hit1);
      vec_Clusters.push_back(new WireCluster(vec_ClusteredHits));
    }

  }

}

void ClusterEngine::ClusterHits_onlyChan(std::vector<WireHit*>&     vec_Hits, 
					 std::vector<WireCluster*>& vec_Clusters) {

  std::sort(vec_Hits.begin(), vec_Hits.end(), HitOrderedInChan);   
  vec_Clusters.clear();
  
  if (vec_Hits.size() < 2) return;

  for (size_t i = 0; i < vec_Hits.size()-1; i++) {

    std::vector<Hit*> vec_TempHits;

    if(std::abs(vec_Hits[i]->GetHitChan()-vec_Hits[i+1]->GetHitChan())<=fChannelWidth) {
      int channelCount = 1;      
      vec_TempHits.push_back(vec_Hits[i  ]);
      vec_TempHits.push_back(vec_Hits[i+1]);

      while ((i+channelCount+1)<vec_Hits.size() &&
	     std::abs(vec_Hits[i+channelCount]->GetHitChan()-
		      vec_Hits[i+channelCount+1]->GetHitChan()) <= fChannelWidth) {
        vec_TempHits.push_back(vec_Hits[i + channelCount + 1]); channelCount++;
      }

      vec_Clusters.push_back(new WireCluster(vec_TempHits));
      vec_TempHits.clear();
    }
  }
}


void ClusterEngine::ClusterClusters(std::vector<WireCluster*>& vec_uCluster,
				    std::vector<WireCluster*>& vec_vCluster,
				    std::vector<WireCluster*>& vec_zCluster,
				    std::vector<std::vector<WireCluster*> >& vec_clustCluster) {

  vec_clustCluster.clear();
  std::vector<WireCluster*> TripleCluster;
  std::vector<WireCluster*> potentialUClusters;
  std::vector<WireCluster*> potentialVClusters;

  for (size_t c=0; c<vec_zCluster.size(); ++c) {
    WireCluster *clust = vec_zCluster[c];
    TripleCluster.clear(); TripleCluster.push_back(clust);
    size_t ThisClustAPA       = clust->GetAPA         (); bool APAMatch     = false;
    double ThisClustStartTime = clust->GetFirstHitTime(); bool StartOverlap = false;
    double ThisClustEndTime   = clust->GetLastHitTime (); bool EndOverlap   = false;

    // --- Search through U-Plane clusters
    for (auto const& it: vec_uCluster) {
      size_t UClustAPA       = it->GetAPA         ();
      double UClustStartTime = it->GetFirstHitTime();
      double UClustEndTime   = it->GetLastHitTime ();

      APAMatch     = UClustAPA == ThisClustAPA;
      StartOverlap = (UClustStartTime + 10) > ThisClustStartTime && (UClustStartTime - 10) < ThisClustStartTime;
      EndOverlap   = (UClustEndTime   + 10) > ThisClustEndTime   && (UClustEndTime   - 10) < ThisClustEndTime;
      if (APAMatch && StartOverlap && EndOverlap) potentialUClusters.push_back(it);    
    }

    // --- Search through V-Plane clusters
    for (auto const& it: vec_vCluster) {
      size_t VClustAPA       = it->GetAPA         ();
      double VClustStartTime = it->GetFirstHitTime();
      double VClustEndTime   = it->GetLastHitTime ();

      APAMatch     = VClustAPA == ThisClustAPA;
      StartOverlap = (VClustStartTime + 10) > ThisClustStartTime && (VClustStartTime - 20) < ThisClustStartTime;
      EndOverlap   = (VClustEndTime   + 10) > ThisClustEndTime   && (VClustEndTime   - 20) < ThisClustEndTime;
      if (APAMatch && StartOverlap && EndOverlap) potentialVClusters.push_back(it);
    }
    
    // --- Fill the potential triple
    if (potentialUClusters.size() > 0) TripleCluster.push_back(potentialUClusters[0]);
    if (potentialVClusters.size() > 0) TripleCluster.push_back(potentialVClusters[0]);
    vec_clustCluster.push_back(TripleCluster);
    potentialUClusters.clear();
    potentialVClusters.clear();
    TripleCluster.clear();
  }
  return;
}

void ClusterEngine::HitMatching(int TimeTolerance,
				std::vector<WireHit*>&     vec_WireHitZ,
				std::vector<WireCluster*>& vec_WireClusterU,
				std::vector<WireCluster*>& vec_WireClusterV,
				std::vector<WireCluster*>& vec_WireClusterZ,
				std::vector<WireCluster*>& vec_clustCluster) {

  for (auto const& clust: vec_WireClusterZ) {
    std::set   <Hit*>         vec_TempHit;
    std::vector<Hit*>         matched_WireHitU;
    std::vector<Hit*>         matched_WireHitV;
    std::vector<WireCluster*> matched_WireClusterU;
    std::vector<WireCluster*> matched_WireClusterV;

    bool   DoUPlane     = vec_WireClusterU.size() > 0;
    bool   DoVPlane     = vec_WireClusterV.size() > 0;

    size_t ClusterAPA   = clust->GetAPA         ();
    double FirstHitTime = clust->GetFirstHitTime();
    double LastHitTime  = clust->GetLastHitTime ();

    // --- Fill vector with hits from collection wire cluster
    for (auto const& hit: clust->GetHit()) vec_TempHit.insert(hit);
      
    // --- Test the U plane hits
    if (DoUPlane) {
      for (auto const& uClust: vec_WireClusterU) {
	size_t uClustAPA     = uClust->GetAPA         ();
	double FirstUHitTime = uClust->GetFirstHitTime();
	double LastUHitTime  = uClust->GetLastHitTime ();

	if (uClustAPA == ClusterAPA && 
	    FirstUHitTime > (FirstHitTime - TimeTolerance) &&
	    LastUHitTime  < (LastHitTime  + TimeTolerance)) {
	  matched_WireClusterU.push_back(uClust);
	  for (auto const& hit: uClust->GetHit()) matched_WireHitU.push_back(hit);
	}
      }
    }
    if (DoVPlane) {
      for (auto const& vClust: vec_WireClusterV) {
	size_t vClustAPA     = vClust->GetAPA         ();
	double FirstVHitTime = vClust->GetFirstHitTime();
	double LastVHitTime  = vClust->GetLastHitTime ();

	if (vClustAPA == ClusterAPA && 
	    FirstVHitTime > (FirstHitTime - TimeTolerance) &&
	    LastVHitTime  < (LastHitTime  + TimeTolerance)) {
	  matched_WireClusterV.push_back(vClust);
	  for (auto const& hit: vClust->GetHit()) matched_WireHitV.push_back(hit);
	}
      }
    }

    // --- Match u hits to known collection hits
    for (auto const& uhit: matched_WireHitU) {
      for (auto const& hit: vec_WireHitZ) {
	if (hit->GetAPA    () == uhit->GetAPA() &&
	    hit->GetTime   () > (uhit->GetTime() - TimeTolerance) &&
	    hit->GetTime   () < (uhit->GetTime() + TimeTolerance) &&
	    hit->GetGenType() == uhit->GetGenType()) {
	  vec_TempHit.insert(hit);
	}
      }
    }
    // --- Match v hits to known collection hits
    for (auto const& vhit: matched_WireHitV) {
      for (auto const& hit: vec_WireHitZ) {
	if (hit->GetAPA    () == vhit->GetAPA() &&
	    hit->GetTime   () > (vhit->GetTime() - TimeTolerance) &&
	    hit->GetTime   () < (vhit->GetTime() + TimeTolerance) && 
	    hit->GetGenType() == vhit->GetGenType()) {
	  vec_TempHit.insert(hit);
	}
      }
    }
    std::vector<Hit*> vec_MatchedHits(vec_TempHit.begin(), vec_TempHit.end());
    vec_clustCluster.push_back(new WireCluster(vec_MatchedHits));
    // std::cout << vec_TempHit.size() << std::endl;
    // std::cout << std::endl;
  } // Looping through the collection wire clusters



/*
void ClusterEngine::HitMatching(int TimeTolerance,
				std::vector<WireHit*>&     vec_WireHitZ,
				std::vector<WireCluster*>& vec_WireClusterU,
				std::vector<WireCluster*>& vec_WireClusterV,
				std::vector<WireCluster*>& vec_WireClusterZ,
				std::vector<WireCluster*>& vec_clustCluster) {

  // --- Set up the data containers
  std::set   <Hit*> newClustHits;
  std::vector<Hit*> potentialHits;
  std::vector<Hit*> matchedHits;

  for (auto const& clust: vec_WireClusterU) {
    std::vector<Hit*> tempHits = clust->GetHit();
    for (auto const& hit: tempHits)
  	potentialHits.push_back(hit);
  }
  for (auto const& clust: vec_WireClusterV) {
    std::vector<Hit*> tempHits = clust->GetHit();
    for (auto const& hit: tempHits)
  	potentialHits.push_back(hit);
  }

  for (auto const& hit: vec_WireHitZ) {
    int lower = hit->GetTime()-TimeTolerance;
    int upper = hit->GetTime()+TimeTolerance;
    for (auto const& phit: potentialHits) {
  	if (phit->GetTime() < upper && phit->GetTime() > lower)
  	  matchedHits.push_back(hit);
    }
  }

  for (auto const& clust: vec_WireClusterZ) {
    std::vector<Hit*> tempHits = clust->GetHit();
    for (auto const& hit: tempHits)
  	newClustHits.insert(hit);
  }

  for (auto const& hit: matchedHits)
    newClustHits.insert(hit);

  // std::vector<Hit*> new_Hits(newClustHits.begin(), newClustHits.end());
  // vec_clustCluster.push_back(new WireCluster(new_Hits));
  */
  return;
}

void ClusterEngine::HitMatching2(int TimeTolerance,
				 std::vector<WireHit*>& vec_WireHitU,
				 std::vector<WireHit*>& vec_WireHitV,
				 std::vector<WireHit*>& vec_WireHitZ,
				 std::vector<WireCluster*>& vec_WireClusterZ,
				 std::vector<WireCluster*>& vec_clustCluster) {

  vec_clustCluster.clear();
  // --- Do hit clustering on the U and V planes
  std::vector<WireCluster*> vec_WireClusterU; ClusterHits2(vec_WireHitU, vec_WireClusterU);
  std::vector<WireCluster*> vec_WireClusterV; ClusterHits2(vec_WireHitV, vec_WireClusterV);

  // --- Get Z cluster center times
  std::vector<double> ZClusterTimes;
  std::vector<size_t> ZClusterAPA;
  for (auto const& it: vec_WireClusterZ) {
    double centerTime = GetCenterTime(it);
    ZClusterTimes.push_back(centerTime);
    ZClusterAPA  .push_back(it->GetAPA());
  }
  
  // --- Match the U clusters
  std::map<WireCluster*, WireCluster*> MatchedU;
  std::map<WireCluster*, WireCluster*> MatchedV;
  
  double diff = DBL_MAX;

  // --- Cluster matching on the U plane
  for (auto const& clust: vec_WireClusterU) {
    diff = DBL_MAX; double centerTime = GetCenterTime(clust);

    for (size_t i=0; i<ZClusterTimes.size(); ++i) {
      if (abs(centerTime - ZClusterTimes[i]) < diff && 
	  clust->GetAPA() == ZClusterAPA[i] && 
	  abs(centerTime - ZClusterTimes[i]) < 15) {
	diff = abs(centerTime - ZClusterTimes[i]);
	MatchedU[clust] = vec_WireClusterZ[i];
      }
    }
  }

  // --- Cluster matching on the V plane
  for (auto const& clust: vec_WireClusterV) {
    diff = DBL_MAX; double centerTime = GetCenterTime(clust);

    for (size_t i=0; i<ZClusterTimes.size(); ++i) {
      if (abs(centerTime - ZClusterTimes[i]) < diff && 
	  clust->GetAPA() == ZClusterAPA[i] && 
	  abs(centerTime - ZClusterTimes[i]) < 15) {
	diff = abs(centerTime - ZClusterTimes[i]);
	MatchedV[clust] = vec_WireClusterZ[i];
      }
    }
  }

  // --- Enhance Z clusters with U clusters
  std::map<WireCluster*, std::set<Hit*> > ZClusterToUset;
  for (auto const& it: MatchedU) {
    WireCluster* uclust = it.first;
    WireCluster* zclust = it.second;

    std::set<Hit*> set_tempHitsZ; 
    for (auto const& hit : zclust->GetHit()) set_tempHitsZ.insert(hit);
    for (auto const& uhit: uclust->GetHit()) {
      size_t HitTime = uhit->GetTime();
      size_t HitAPA  = uhit->GetAPA ();
      for (auto const& zhit: vec_WireHitZ) {
	if (zhit->GetAPA() == HitAPA &&
	    HitTime < zhit->GetTime() + TimeTolerance &&
	    HitTime > zhit->GetTime() - TimeTolerance) {
	  set_tempHitsZ.insert(zhit);
	}
      }
    }
    ZClusterToUset[zclust] = set_tempHitsZ;
  }

  // --- Enhance Z clusters with V clusters
  std::vector<std::set<Hit*> > vec_SetWireHitsVZ;
  std::map<WireCluster*, std::set<Hit*> > ZClusterToVset;
  for (auto const& it: MatchedV) {
    WireCluster* vclust = it.first;
    WireCluster* zclust = it.second;

    std::set<Hit*> set_tempHitsZ; 
    for (auto const& hit : zclust->GetHit()) set_tempHitsZ.insert(hit);
    for (auto const& vhit: vclust->GetHit()) {
      size_t HitTime = vhit->GetTime();
      size_t HitAPA  = vhit->GetAPA ();
      for (auto const& zhit: vec_WireHitZ) {
	if (zhit->GetAPA() == HitAPA &&
	    HitTime < zhit->GetTime() + TimeTolerance &&
	    HitTime > zhit->GetTime() - TimeTolerance) {
	  set_tempHitsZ.insert(zhit);
	}
      }
    }
    ZClusterToVset[zclust] = set_tempHitsZ;
  }

  std::set<std::vector<Hit*> > set_VecAllHits;
  for (auto const& uz: ZClusterToUset) {
    std::set<Hit*> set_tempU = uz.second;
    for (auto const& vz: ZClusterToVset) {
      if (uz.first == vz.first) {
	for (auto const& hit: vz.second) set_tempU.insert(hit);
      }
    }
    std::vector<Hit*> vec_tempU(set_tempU.begin(), set_tempU.end());
    set_VecAllHits.insert(vec_tempU);
  }

  for (auto const& vz: ZClusterToVset) {
    for (auto const& uz: ZClusterToUset) {
      if (vz.first != uz.first) {
  	std::vector<Hit*> vec_tempV(vz.second.begin(), vz.second.end());
  	set_VecAllHits.insert(vec_tempV);
      }
    }
  }
  
  for (auto& it: set_VecAllHits) {
    std::vector<Hit*> JustWork = it;
    vec_clustCluster.push_back(new WireCluster(JustWork));
  }

  return;
}
