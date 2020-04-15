#include <algorithm>    // std::min_element, std::max_element
#include <iostream>
#include <fstream>

#include "Utils/Helper.h"
#include "Utils/CLI11.hpp"
#include "IO/InputManager.hh"
#include "Algorithm/ClusterEngine.hh"
#include "Algorithm/Trigger.hh"

#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TPad.h"
#include "TTree.h"
#include "TLegend.h"
#include "TEfficiency.h"
#include "TStyle.h"
#include "TVector3.h"
#include "TAxis.h"

std::vector<std::vector<WireCluster*> > CombineClusters(std::vector<WireCluster*>               vec_SignalClusterU, 
							std::vector<WireCluster*>               vec_SignalClusterV, 
							std::vector<WireCluster*>               vec_SignalClusterZ) {

  std::vector<std::vector<WireCluster*> > vec_TripCluster;
  std::vector<WireCluster*> TripleCluster;
  std::vector<WireCluster*> potentialUClusters;
  std::vector<WireCluster*> potentialVClusters;
  for (size_t c=0; c<vec_SignalClusterZ.size(); ++c) {
    WireCluster *clust = vec_SignalClusterZ[c];
    TripleCluster.clear();
    TripleCluster.push_back(clust);
    size_t ThisClustAPA       = clust->GetAPA         ();
    double ThisClustStartTime = clust->GetFirstHitTime();
    double ThisClustEndTime   = clust->GetLastHitTime ();

    bool APAMatch     = false;
    bool StartOverlap = false;
    bool EndOverlap   = false;

    for (auto const& it: vec_SignalClusterU) {
      size_t UClustAPA       = it->GetAPA         ();
      double UClustStartTime = it->GetFirstHitTime();
      double UClustEndTime   = it->GetLastHitTime ();

      APAMatch     = UClustAPA == ThisClustAPA;
      StartOverlap = (UClustStartTime + 10) > ThisClustStartTime && (UClustStartTime - 10) < ThisClustStartTime;
      EndOverlap   = (UClustEndTime   + 10) > ThisClustEndTime   && (UClustEndTime   - 10) < ThisClustEndTime;
      if (APAMatch && StartOverlap && EndOverlap) potentialUClusters.push_back(it);
    }
    for (auto const& it: vec_SignalClusterV) {
      size_t VClustAPA       = it->GetAPA         ();
      double VClustStartTime = it->GetFirstHitTime();
      double VClustEndTime   = it->GetLastHitTime ();

      APAMatch     = VClustAPA == ThisClustAPA;
      StartOverlap = (VClustStartTime + 10) > ThisClustStartTime && (VClustStartTime - 20) < ThisClustStartTime;
      EndOverlap   = (VClustEndTime   + 10) > ThisClustEndTime   && (VClustEndTime   - 20) < ThisClustEndTime;
      if (APAMatch && StartOverlap && EndOverlap) potentialVClusters.push_back(it);
    }
    

    if (potentialUClusters.size() > 0) TripleCluster.push_back(potentialUClusters[0]);
    if (potentialVClusters.size() > 0) TripleCluster.push_back(potentialVClusters[0]);
    vec_TripCluster.push_back(TripleCluster);
  }
  return vec_TripCluster;
}

int main(int argc, char** argv) {

  CLI::App app{"A program to compare the clusters on each plane"};
  std::string InputFile ;
  std::string InputTree ;
  std::string OutputFile;
  app.add_option("-i,--input" , InputFile , "Input filename (root file, output from SNAna_module)"       )->required();
  app.add_option("-t,--tree"  , InputTree , "Input treename (basename of the TTrees, e.g. snanatrigprim)")->required();
  app.add_option("-o,--output", OutputFile, "Output filename (no extension required)"                    )->required();
  CLI11_PARSE(app, argc, argv);

  std::map<int, SNAnaInputManager*> aaim;
  aaim[40 ] = new SNAnaInputManager(); aaim[40 ]->SetInputTree((InputTree + "800/SNSimTree" ).c_str());
  aaim[50 ] = new SNAnaInputManager(); aaim[50 ]->SetInputTree((InputTree + "1000/SNSimTree").c_str());
  aaim[60 ] = new SNAnaInputManager(); aaim[60 ]->SetInputTree((InputTree + "1200/SNSimTree").c_str());
  aaim[70 ] = new SNAnaInputManager(); aaim[70 ]->SetInputTree((InputTree + "1400/SNSimTree").c_str());
  aaim[80 ] = new SNAnaInputManager(); aaim[80 ]->SetInputTree((InputTree + "1600/SNSimTree").c_str());
  aaim[90 ] = new SNAnaInputManager(); aaim[90 ]->SetInputTree((InputTree + "1800/SNSimTree").c_str());
  aaim[100] = new SNAnaInputManager(); aaim[100]->SetInputTree((InputTree + "2000/SNSimTree").c_str());
  aaim[120] = new SNAnaInputManager(); aaim[120]->SetInputTree((InputTree + "2400/SNSimTree").c_str());
  aaim[140] = new SNAnaInputManager(); aaim[140]->SetInputTree((InputTree + "2800/SNSimTree").c_str());
  aaim[160] = new SNAnaInputManager(); aaim[160]->SetInputTree((InputTree + "3200/SNSimTree").c_str()); 

  for (auto& it: aaim) {
    it.second->SetInputFile(InputFile);
    it.second->LoadTree();
  }

  // --- Set up cluster engine and trigger
  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  // --- Define the clustering and trigger parameters
  clusteng.SetTimeWindow   (20 ); wiretrigger.SetWireNHitsMin    (6);
  clusteng.SetChannelWidth (2  ); wiretrigger.SetWireNChanMin    (2);
  clusteng.SetTimeWindowOpt(0.2); wiretrigger.SetWireChanWidthMin(0);
  clusteng.SetPositionOpt  (300); wiretrigger.SetWireSADCMin     (0);
  clusteng.SetBucketSize   (1  );

  // --- Define the plot data containers
  std::map<int, TH1D*> HitView;
  std::map<int, TH1D*> HitTypeU;
  std::map<int, TH1D*> HitTypeV;
  std::map<int, TH1D*> HitTypeZ;
  std::map<int, TH1D*> ClusterTypeU;
  std::map<int, TH1D*> ClusterTypeV;
  std::map<int, TH1D*> ClusterTypeZ;
  std::map<int, TH1D*> TripCluster;

  for (auto const& it: aaim) {
    int adc_threshold     = it.first;
    SNAnaInputManager* im = it.second;

    HitView       [adc_threshold] = new TH1D(Form("HitView_Th%i" , adc_threshold), 
					     Form("ADC THRESHOLD = %i;Hit View;",adc_threshold)  , 3, 0, 3);
    HitTypeU      [adc_threshold] = new TH1D(Form("HitTypeU_Th%i"    , adc_threshold), ";Hit Type U;"    , 3, 0, 3);
    HitTypeV      [adc_threshold] = new TH1D(Form("HitTypeV_Th%i"    , adc_threshold), ";Hit Type V;"    , 3, 0, 3);
    HitTypeZ      [adc_threshold] = new TH1D(Form("HitTypeZ_Th%i"    , adc_threshold), ";Hit Type Z;"    , 3, 0, 3);
    ClusterTypeU  [adc_threshold] = new TH1D(Form("ClusterTypeU_Th%i", adc_threshold), ";Cluster Type U;", 2, 0, 2);
    ClusterTypeV  [adc_threshold] = new TH1D(Form("ClusterTypeV_Th%i", adc_threshold), ";Cluster Type V;", 2, 0, 2);
    ClusterTypeZ  [adc_threshold] = new TH1D(Form("ClusterTypeZ_Th%i", adc_threshold), ";Cluster Type Z;", 2, 0, 2);
    TripCluster   [adc_threshold] = new TH1D(Form("TripCluster_Th%i" , adc_threshold), ""                , 4, 0, 4);

    PrintProgress(adc_threshold, 160);

    for (int CurrentEvent=0; CurrentEvent<im->GetEntries(); ++CurrentEvent) {
      im->GetEntry(CurrentEvent);

      // --- Fill hit information data containers
      std::vector<WireHit*> vec_WireHitU; std::vector<int> nhitU = {0, 0, 0};
      std::vector<WireHit*> vec_WireHitV; std::vector<int> nhitV = {0, 0, 0};
      std::vector<WireHit*> vec_WireHitZ; std::vector<int> nhitZ = {0, 0, 0};

      for (size_t j=0; j<im->Hit_View->size(); ++j) {
        WireHit* hit = new WireHit((*im->Hit_View)[j],        (*im->Hit_True_GenType)[j],  (*im->Hit_Chan)[j],
                                   (*im->Hit_Time)[j],        (*im->Hit_SADC)[j],          (*im->Hit_RMS)[j],
                                   (*im->Hit_True_Energy)[j], (*im->Hit_True_EvEnergy)[j], (*im->Hit_True_MainTrID)[j],
                                   0.5, 0.5, 0.5,
                                   (*im->Hit_True_X)[j],      (*im->Hit_True_Y)[j],        (*im->Hit_True_Z)[j],
                                   0, (*im->Hit_True_nElec)[j]);
	if ((*im->Hit_View)[j] == 0) {
	  if      ((*im->Hit_True_GenType)[j] == 0) nhitU[0]++;
	  else if ((*im->Hit_True_GenType)[j] == 1) nhitU[1]++;
	  else                                      nhitU[2]++;
	  vec_WireHitU.push_back(hit);
	}
	if ((*im->Hit_View)[j] == 1) {
	  if      ((*im->Hit_True_GenType)[j] == 0) nhitV[0]++;
	  else if ((*im->Hit_True_GenType)[j] == 1) nhitV[1]++;
	  else                                      nhitV[2]++;
	  vec_WireHitV.push_back(hit);
	}

	if ((*im->Hit_View)[j] == 2) {
	  if      ((*im->Hit_True_GenType)[j] == 0) nhitZ[0]++;
	  else if ((*im->Hit_True_GenType)[j] == 1) nhitZ[1]++;
	  else                                      nhitZ[2]++;
	  vec_WireHitZ.push_back(hit);
	}
      } // Looping through hits

      for (size_t i=0; i<vec_WireHitU.size(); ++i) HitView[adc_threshold]->Fill(0);
      for (size_t i=0; i<vec_WireHitV.size(); ++i) HitView[adc_threshold]->Fill(1);
      for (size_t i=0; i<vec_WireHitZ.size(); ++i) HitView[adc_threshold]->Fill(2);

      for (int i=0; i<nhitU[0]; ++i) HitTypeU[adc_threshold]->Fill(0); 
      for (int i=0; i<nhitU[1]; ++i) HitTypeU[adc_threshold]->Fill(1); 
      for (int i=0; i<nhitU[2]; ++i) HitTypeU[adc_threshold]->Fill(2); 
      for (int i=0; i<nhitV[0]; ++i) HitTypeV[adc_threshold]->Fill(0);
      for (int i=0; i<nhitV[1]; ++i) HitTypeV[adc_threshold]->Fill(1);
      for (int i=0; i<nhitV[2]; ++i) HitTypeV[adc_threshold]->Fill(2);
      for (int i=0; i<nhitZ[0]; ++i) HitTypeZ[adc_threshold]->Fill(0);
      for (int i=0; i<nhitZ[1]; ++i) HitTypeZ[adc_threshold]->Fill(1);
      for (int i=0; i<nhitZ[2]; ++i) HitTypeZ[adc_threshold]->Fill(2);

      // --- Fill the hit cluster information data containers
      // int nUcluster      = 0; int nVcluster      = 0; int nZcluster      = 0;
      // int nUnoisecluster = 0; int nVnoisecluster = 0; int nZnoisecluster = 0;
      std::vector<WireCluster*> vec_WireClusterU; std::vector<WireCluster*> vec_SignalClusterU; 
      std::vector<WireCluster*> vec_WireClusterV; std::vector<WireCluster*> vec_SignalClusterV; 
      std::vector<WireCluster*> vec_WireClusterZ; std::vector<WireCluster*> vec_SignalClusterZ; 
      clusteng.ClusterHits2(vec_WireHitU, vec_WireClusterU); wiretrigger.IsTriggering(vec_WireClusterU);
      clusteng.ClusterHits2(vec_WireHitV, vec_WireClusterV); wiretrigger.IsTriggering(vec_WireClusterV);
      clusteng.ClusterHits2(vec_WireHitZ, vec_WireClusterZ); wiretrigger.IsTriggering(vec_WireClusterZ);

      // --- Analyse each wire cluster vector
      for (auto const& clust: vec_WireClusterU) 
	if (clust->GetIsTriggering())  
	  if (clust->GetType()) vec_SignalClusterU.push_back(clust);
      for (auto const& clust: vec_WireClusterV) 
	if (clust->GetIsSelected())  
	  if (clust->GetType()) vec_SignalClusterV.push_back(clust);
      for (auto const& clust: vec_WireClusterZ) 
	if (clust->GetIsSelected())  
	  if (clust->GetType()) vec_SignalClusterZ.push_back(clust);

      std::vector<std::vector<WireCluster*> > vec_TripCluster;
      // vec_TripCluster = CombineClusters(vec_SignalClusterU, vec_SignalClusterV, vec_SignalClusterZ);
      // for (auto const& it: vec_TripCluster) TripCluster[adc_threshold]->Fill(it.size());
      // std::cout << "Clustering clusters" << std::endl;
      clusteng.ClusterClusters(vec_SignalClusterU, vec_SignalClusterV, vec_SignalClusterZ, vec_TripCluster);
      for (auto const& it: vec_TripCluster) TripCluster[adc_threshold]->Fill(it.size()); 

      // --- Memory management - deleteing all of the new objects from memory
      for (auto& it: vec_WireHitU    ) { delete it; it = NULL; } ; vec_WireHitU    .clear();
      for (auto& it: vec_WireHitV    ) { delete it; it = NULL; } ; vec_WireHitV    .clear();
      for (auto& it: vec_WireHitZ    ) { delete it; it = NULL; } ; vec_WireHitZ    .clear();
      for (auto& it: vec_WireClusterU) { delete it; it = NULL; } ; vec_WireClusterU.clear();
      for (auto& it: vec_WireClusterV) { delete it; it = NULL; } ; vec_WireClusterV.clear();
      for (auto& it: vec_WireClusterZ) { delete it; it = NULL; } ; vec_WireClusterZ.clear();
    }

  }

  TCanvas *c = new TCanvas("c", "c");
  c->Print((OutputFile + ".pdf[").c_str());

  for (auto const& it: TripCluster) {it.second->Draw(); c->Print((OutputFile + ".pdf" ).c_str());}

  std::vector<std::string> HitViewLabels = {"U-Plane", "V-Plane", "Z-Plane"};
  for (auto const& it: HitView) {
    it.second->Draw(); 
    it.second->SetStats(0);
    it.second->SetBit(TAxis::kLabelsHori);
    for (size_t itt=0; itt<HitViewLabels.size(); ++itt) { it.second->GetXaxis()->SetBinLabel(itt+1, HitViewLabels[itt].c_str()); }
    c->Print((OutputFile + ".pdf" ).c_str());
  }

  c->Divide(3, 1);
  std::vector<std::string> HitTypeLabels = {"Noise", "SN", "Background"};
  for (auto const& it: HitTypeU) {
    c->cd(1); HitTypeU[it.first]->Draw(); HitTypeU[it.first]->SetStats(0); HitTypeU[it.first]->SetBit(TAxis::kLabelsHori);
    for (size_t itt=0; itt<HitTypeLabels.size(); ++itt)
      HitTypeU[it.first]->GetXaxis()->SetBinLabel(itt+1, HitTypeLabels[itt].c_str());

    c->cd(2); HitTypeV[it.first]->Draw(); HitTypeV[it.first]->SetStats(0); HitTypeV[it.first]->SetBit(TAxis::kLabelsHori);
    for (size_t itt=0; itt<HitTypeLabels.size(); ++itt) 
      HitTypeV[it.first]->GetXaxis()->SetBinLabel(itt+1, HitTypeLabels[itt].c_str());

    c->cd(3); HitTypeZ[it.first]->Draw(); HitTypeZ[it.first]->SetStats(0); HitTypeZ[it.first]->SetBit(TAxis::kLabelsHori);
    for (size_t itt=0; itt<HitTypeLabels.size(); ++itt)
      HitTypeZ[it.first]->GetXaxis()->SetBinLabel(itt+1, HitTypeLabels[itt].c_str());
    c->Print((OutputFile + ".pdf" ).c_str());
  }

  c->Print((OutputFile + ".pdf]").c_str());
  






}
