#include "IO/InputManager.hh"
#include "Algorithm/ClusterEngine.hh"
#include "Algorithm/Trigger.hh"

#include "Utils/Helper.h"

#include "TCanvas.h"
#include "TEfficiency.h"
#include "TFile.h"
#include "TPad.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TTree.h"
#include "TError.h"

#include <iostream>
#include <unistd.h>

// --- Function to print a progress bar along the terminal
void progress(float prog) {

  int barWidth = 60;
  std::cout << "[";
  int pos = barWidth * prog;

  for (int i=0; i<barWidth; ++i) {
    if      (i < pos)           { std::cout << "="; }
    else if (pos+1 == barWidth) { std::cout << "="; }
    else if (i == pos)          { std::cout << ">"; }
    else                        { std::cout << " "; }
  }
  std::cout << "] " << (int)round(100.0 * prog) << " %\r" << std::flush;
}

int main(int argc, char** argv) {

  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  std::string InFileName ="";
  std::string OutFileName="";
  int nEvent=-1;
  int sigma = 0;
  while ( (opt = getopt(argc, argv, "i:o:n:s:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'i':
      InFileName = optarg;
      break;
    case 'o':
      OutFileName = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case 's':
      sigma = atoi(optarg);
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  // --- Root information IO
  std::map<int, SNAnaInputManager*> aaim;  
  if (sigma) {
    aaim[30] = new SNAnaInputManager(); aaim[30]->SetInputTree("snanarunsum3/SNSimTree" );
    aaim[40] = new SNAnaInputManager(); aaim[40]->SetInputTree("snanarunsum4/SNSimTree" );
    aaim[50] = new SNAnaInputManager(); aaim[50]->SetInputTree("snanarunsum5/SNSimTree" );
    aaim[60] = new SNAnaInputManager(); aaim[60]->SetInputTree("snanarunsum6/SNSimTree" );
    aaim[70] = new SNAnaInputManager(); aaim[70]->SetInputTree("snanarunsum7/SNSimTree" );
  } else {
    aaim[40 ] = new SNAnaInputManager(); aaim[40 ]->SetInputTree("snanarunsum800/SNSimTree" );
    aaim[50 ] = new SNAnaInputManager(); aaim[50 ]->SetInputTree("snanarunsum1000/SNSimTree" );
    aaim[60 ] = new SNAnaInputManager(); aaim[60 ]->SetInputTree("snanarunsum1200/SNSimTree" );
    aaim[70 ] = new SNAnaInputManager(); aaim[70 ]->SetInputTree("snanarunsum1400/SNSimTree" );
    aaim[80 ] = new SNAnaInputManager(); aaim[80 ]->SetInputTree("snanarunsum1600/SNSimTree" );
    aaim[90 ] = new SNAnaInputManager(); aaim[90 ]->SetInputTree("snanarunsum1800/SNSimTree" );
    aaim[100] = new SNAnaInputManager(); aaim[100]->SetInputTree("snanarunsum2000/SNSimTree");
    aaim[120] = new SNAnaInputManager(); aaim[120]->SetInputTree("snanarunsum2400/SNSimTree");
    aaim[140] = new SNAnaInputManager(); aaim[140]->SetInputTree("snanarunsum2800/SNSimTree");
    aaim[160] = new SNAnaInputManager(); aaim[160]->SetInputTree("snanarunsum3200/SNSimTree"); 
  }


  // Setup data containers for plots
  TH1D *electronEnergy = new TH1D("Electron Energy", ";Electron KE [MeV];", 200, 0, 10);
  TH1D *matClusterSize = new TH1D("MatClusterSize" , ";nHitsInCluster;"   ,  20, 0, 20);
  std::map<int, TEfficiency*> effEnergyCollection;
  std::map<int, TEfficiency*> effEnergyInduction;
  std::map<int, TEfficiency*> effEnergyMatched;

  std::map<int, TProfile*> nColNoiseHit     ; std::map<int, TProfile*> nIndNoiseHit     ;
  std::map<int, TProfile*> nColSignalHit    ; std::map<int, TProfile*> nIndSignalHit    ;
  std::map<int, TProfile*> nColBackgroundHit; std::map<int, TProfile*> nIndBackgroundHit;
  std::map<int, TProfile*> nColNoiseCluster ; std::map<int, TProfile*> nIndNoiseCluster ;
  std::map<int, TProfile*> nColSignalCluster; std::map<int, TProfile*> nIndSignalCluster;
  std::map<int, TProfile*> nMatNoiseCluster ;
  std::map<int, TProfile*> nMatSignalCluster;

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }

  // --- Set up cluster engine and trigger
  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  clusteng.SetTimeWindow   (20 ); wiretrigger.SetWireNHitsMin    (6);
  clusteng.SetChannelWidth (2  ); wiretrigger.SetWireNChanMin    (2);
  clusteng.SetTimeWindowOpt(0.2); wiretrigger.SetWireChanWidthMin(0);
  clusteng.SetPositionOpt  (300); wiretrigger.SetWireSADCMin     (0);
  clusteng.SetBucketSize   (1  );


  for (auto const& it: aaim) {
    int fNEvent = nEvent;
    int adc_threshold = it.first;
    SNAnaInputManager* im = it.second;
    
    if (nEvent == -1) fNEvent = im->GetEntries();
    else              fNEvent = nEvent;

    effEnergyCollection[adc_threshold] = new TEfficiency(Form("EffCol_Th%i", adc_threshold), 
							 "Collection wire only;Electron KE [MeV];Trigger Efficiency", 50, 0, 2);

    effEnergyInduction [adc_threshold] = new TEfficiency(Form("EffInd_Th%i", adc_threshold), 
							 "Induction wire only;Electron KE [MeV];Trigger Efficiency", 50, 0, 2);

    effEnergyMatched   [adc_threshold] = new TEfficiency(Form("EffMat_Th%i", adc_threshold), 
							 "Hit Matching;Electron KE [MeV];Trigger Efficiency", 50, 0, 2);

    nColNoiseHit       [adc_threshold] = new TProfile(Form("nColNoiseHit_Th%i", adc_threshold), 
						      ";Electron KE [MeV];nColNoiseHit", 
						      50, 0, 2);
    nColSignalHit      [adc_threshold] = new TProfile(Form("nColSignalHit_Th%i", adc_threshold), 
						      ";Electron KE [MeV];nColSignalHit", 
						      50, 0, 2);
    nColBackgroundHit  [adc_threshold] = new TProfile(Form("nColBackgroundHit_Th%i", adc_threshold), 
						      ";Electron KE [MeV];nColBackgroundHit", 
						      50, 0, 2);
    nColNoiseCluster   [adc_threshold] = new TProfile(Form("nColNoiseCluster_Th%i", adc_threshold), 
						      ";Electron KE [MeV];nColNoiseCluster", 
						      50, 0, 2);
    nColSignalCluster  [adc_threshold] = new TProfile(Form("nColSignalCluster_Th%i", adc_threshold), 
						      ";Electron KE [MeV];nColSignalCluster", 
						      50, 0, 2);
    nIndNoiseHit       [adc_threshold] = new TProfile(Form("nIndNoiseHit_Th%i", adc_threshold),
						      ";Electron KE [MeV];nIndNoiseHit", 
						      50, 0, 2);
    nIndSignalHit      [adc_threshold] = new TProfile(Form("nIndSignalHit_Th%i", adc_threshold),
						      ";Electron KE [MeV];nIndSignalHit", 
						      50, 0, 2);
    nIndBackgroundHit  [adc_threshold] = new TProfile(Form("nIndBackgroundHit_Th%i", adc_threshold),
						      ";Electron KE [MeV];nIndBackgroundHit", 
						      50, 0, 2);
    nIndNoiseCluster   [adc_threshold] = new TProfile(Form("nIndNoiseCluster_Th%i", adc_threshold),
						      ";Electron KE [MeV];nIndNoiseCluster", 
						      50, 0, 2);
    nIndSignalCluster  [adc_threshold] = new TProfile(Form("nIndSignalCluster_Th%i", adc_threshold),
						      ";Electron KE [MeV];nIndSignalCluster", 
						      50, 0, 2);
    nMatNoiseCluster   [adc_threshold] = new TProfile(Form("nMatNoiseCluster_Th%i", adc_threshold),
						      ";Electron KE [MeV];nMatNoiseCluster", 
						      50, 0, 2);
    nMatSignalCluster  [adc_threshold] = new TProfile(Form("nMatSignalCluster_Th%i", adc_threshold),
						      ";Electron KE [MeV];nMatSignalCluster", 
						      50, 0, 2);

    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);

      std::cout << "Running on ADC threshold " << adc_threshold << " ";
      float prog = (float)CurrentEvent / (float)fNEvent;
      progress(prog);


      std::vector<int> nColHit = {0, 0, 0};
      std::vector<int> nIndHit = {0, 0, 0};
      std::vector<WireHit*> vec_WireHitU;
      std::vector<WireHit*> vec_WireHitV;
      std::vector<WireHit*> vec_WireHitZ;
      for (size_t j=0; j<im->Hit_View->size(); ++j) {
	WireHit* hit = new WireHit((*im->Hit_View)[j],        (*im->Hit_True_GenType)[j],  (*im->Hit_Chan)[j],
				   (*im->Hit_Time)[j],        (*im->Hit_SADC)[j],          (*im->Hit_RMS)[j],
				   (*im->Hit_True_Energy)[j], (*im->Hit_True_EvEnergy)[j], (*im->Hit_True_MainTrID)[j],
				   0.5, 0.5, 0.5,
				   (*im->Hit_True_X)[j],      (*im->Hit_True_Y)[j],        (*im->Hit_True_Z)[j],
				   0, (*im->Hit_True_nElec)[j]);
	if ((*im->Hit_View)[j] == 0) vec_WireHitU.push_back(hit);
	if ((*im->Hit_View)[j] == 1) vec_WireHitV.push_back(hit);
	if ((*im->Hit_View)[j] == 2) vec_WireHitZ.push_back(hit);
	if ((*im->Hit_View)[j] == 0) {
	  if      ((*im->Hit_True_GenType)[j] == 0) nIndHit[0]++;
	  else if ((*im->Hit_True_GenType)[j] == 1) nIndHit[1]++;
	  else                                      nIndHit[2]++;
	}
	if ((*im->Hit_View)[j] == 1) {
	  if      ((*im->Hit_True_GenType)[j] == 0) nIndHit[0]++;
	  else if ((*im->Hit_True_GenType)[j] == 1) nIndHit[1]++;
	  else                                      nIndHit[2]++;
	}
	if ((*im->Hit_View)[j] == 2) {
	  if      ((*im->Hit_True_GenType)[j] == 0) nColHit[0]++;
	  else if ((*im->Hit_True_GenType)[j] == 1) nColHit[1]++;
	  else                                      nColHit[2]++; 
	}
      }

      std::vector<WireCluster*> vec_WireClusterU; clusteng.ClusterHits2(vec_WireHitU, vec_WireClusterU);
      std::vector<WireCluster*> vec_WireClusterV; clusteng.ClusterHits2(vec_WireHitV, vec_WireClusterV);
      std::vector<WireCluster*> vec_WireClusterZ; clusteng.ClusterHits2(vec_WireHitZ, vec_WireClusterZ);
      wiretrigger.IsTriggering(vec_WireClusterU);
      wiretrigger.IsTriggering(vec_WireClusterV);
      wiretrigger.IsTriggering(vec_WireClusterZ);

      std::vector<WireCluster*> test_Cluster;
      if (vec_WireClusterZ.size()) {
	clusteng.HitMatching(2, vec_WireHitZ, vec_WireClusterU, vec_WireClusterV, vec_WireClusterZ, test_Cluster);
	wiretrigger.IsTriggering(test_Cluster);
      }

      // --- Analyse the collection wire clusters before hit matching
      bool selectedCol = false; 
      bool selectedInd = false;
      bool selectedMat = false;
      int nColsignalCluster = 0; int nIndsignalCluster = 0; int nMatsignalCluster = 0;
      int nColnoiseCluster  = 0; int nIndnoiseCluster  = 0; int nMatnoiseCluster  = 0;
      for (auto const& clust: vec_WireClusterZ) {
	if (clust->GetIsTriggering()) {
	  if (clust->GetType()) { selectedCol = true; nColsignalCluster++; }
	  else                  { nColnoiseCluster++; }
	}
      }

      for (auto const& clust: vec_WireClusterU) {
	if (clust->GetIsTriggering()) {
	  if (clust->GetType()) { selectedInd = true; nIndsignalCluster++; }
	  else                  { nIndnoiseCluster++; }
	}
      }

      for (auto const& clust: vec_WireClusterV) {
	if (clust->GetIsTriggering()) {
	  if (clust->GetType()) { selectedInd = true; nIndsignalCluster++; }
	  else                  { nIndnoiseCluster++; }
	}
      }

      for (auto const& clust: test_Cluster) {
	if (clust->GetIsTriggering()) {
	  if (clust->GetType()) { selectedMat = true; nMatsignalCluster++; }
	  else                  { nMatnoiseCluster++; }
	} 
      } 

      double KE = ((*im->True_Bck_Energy)[0] * 1000.0) - 0.511;
      electronEnergy                    ->Fill(KE                   );
      effEnergyCollection[adc_threshold]->Fill(selectedCol, KE      );
      effEnergyInduction [adc_threshold]->Fill(selectedInd, KE      );
      effEnergyMatched   [adc_threshold]->Fill(selectedMat, KE      );
      nColNoiseHit       [adc_threshold]->Fill(KE, nColHit[0]       ); 
      nColSignalHit	 [adc_threshold]->Fill(KE, nColHit[1]       );
      nColBackgroundHit	 [adc_threshold]->Fill(KE, nColHit[2]       );
      nColSignalCluster	 [adc_threshold]->Fill(KE, nColsignalCluster);
      nColNoiseCluster   [adc_threshold]->Fill(KE, nColnoiseCluster );
      nIndNoiseHit       [adc_threshold]->Fill(KE, nIndHit[0]       );
      nIndSignalHit	 [adc_threshold]->Fill(KE, nIndHit[1]       );
      nIndBackgroundHit	 [adc_threshold]->Fill(KE, nIndHit[2]       );
      nIndSignalCluster	 [adc_threshold]->Fill(KE, nIndsignalCluster);
      nIndNoiseCluster   [adc_threshold]->Fill(KE, nIndnoiseCluster );
      nMatSignalCluster	 [adc_threshold]->Fill(KE, nMatsignalCluster);
      nMatNoiseCluster   [adc_threshold]->Fill(KE, nMatnoiseCluster );

      for (auto const& clust: test_Cluster) { matClusterSize->Fill(clust->GetHit().size()); }

      for (auto& it: vec_WireHitU    ) { delete it; it = NULL; }
      for (auto& it: vec_WireHitV    ) { delete it; it = NULL; }
      for (auto& it: vec_WireHitZ    ) { delete it; it = NULL; }
      for (auto& it: vec_WireClusterU) { delete it; it = NULL; }
      for (auto& it: vec_WireClusterV) { delete it; it = NULL; }
      for (auto& it: vec_WireClusterZ) { delete it; it = NULL; }
      for (auto& it: test_Cluster    ) { delete it; it = NULL; }
      vec_WireHitU    .clear();
      vec_WireHitV    .clear();
      vec_WireHitZ    .clear();
      vec_WireClusterU.clear();
      vec_WireClusterV.clear();
      vec_WireClusterZ.clear();
      test_Cluster    .clear();


    } // Looping through the events

  } // Looping through the AAIMs

  // --- Convert efficiencies to histograms
  std::map<int, TGraphAsymmErrors*> colEffHists;
  std::map<int, TGraphAsymmErrors*> matEffHists;
  std::map<int, TGraph*>            diffGraphs ;

  for (auto const& it: effEnergyCollection) {
    colEffHists[it.first] = effEnergyCollection[it.first]->CreateGraph();
    matEffHists[it.first] = effEnergyMatched   [it.first]->CreateGraph();
    diffGraphs [it.first] = new TGraph(colEffHists[it.first]->GetN());

    int nPoints = colEffHists[it.first]->GetN();
    double ax[100], ay[100];
    double bx[100], by[100];

    for (int i=0; i<nPoints; ++i) {
      colEffHists[it.first]->GetPoint(i, ax[i], ay[i]);
      matEffHists[it.first]->GetPoint(i, bx[i], by[i]);
      diffGraphs [it.first]->SetPoint(diffGraphs [it.first]->GetN(), ax[i], by[i]-ay[i]);
    }

  }

  

  const std::vector<int> color = getColors();
  int i=0;
  TLegend* legend = new TLegend(0.1,0.1,0.9,0.9);
  for (auto const& it: effEnergyCollection) {
    nColNoiseHit       [it.first]->SetStats(0);
    nColSignalHit      [it.first]->SetStats(0);	 
    nColBackgroundHit  [it.first]->SetStats(0);	 
    nColSignalCluster  [it.first]->SetStats(0);	 
    nColNoiseCluster   [it.first]->SetStats(0);
    nIndNoiseHit       [it.first]->SetStats(0);
    nIndSignalHit      [it.first]->SetStats(0);	 
    nIndBackgroundHit  [it.first]->SetStats(0);	 
    nIndSignalCluster  [it.first]->SetStats(0);	 
    nIndNoiseCluster   [it.first]->SetStats(0);
    nMatSignalCluster  [it.first]->SetStats(0);	 
    nMatNoiseCluster   [it.first]->SetStats(0);
    effEnergyCollection[it.first]->SetMarkerColor(color.at(i));
    effEnergyMatched   [it.first]->SetMarkerColor(color.at(i));
    effEnergyInduction [it.first]->SetMarkerColor(color.at(i));
    nColNoiseHit       [it.first]->SetMarkerColor(color.at(i));
    nColSignalHit      [it.first]->SetMarkerColor(color.at(i));	 
    nColBackgroundHit  [it.first]->SetMarkerColor(color.at(i));	 
    nColSignalCluster  [it.first]->SetMarkerColor(color.at(i));	 
    nColNoiseCluster   [it.first]->SetMarkerColor(color.at(i));
    nIndNoiseHit       [it.first]->SetMarkerColor(color.at(i));
    nIndSignalHit      [it.first]->SetMarkerColor(color.at(i));	 
    nIndBackgroundHit  [it.first]->SetMarkerColor(color.at(i));	 
    nIndSignalCluster  [it.first]->SetMarkerColor(color.at(i));	 
    nIndNoiseCluster   [it.first]->SetMarkerColor(color.at(i));
    nMatSignalCluster  [it.first]->SetMarkerColor(color.at(i));	 
    nMatNoiseCluster   [it.first]->SetMarkerColor(color.at(i));
    effEnergyMatched   [it.first]->SetLineColor  (color.at(i));
    effEnergyCollection[it.first]->SetLineColor  (color.at(i));
    effEnergyInduction [it.first]->SetLineColor  (color.at(i));
    nColNoiseHit       [it.first]->SetLineColor  (color.at(i));
    nColSignalHit      [it.first]->SetLineColor  (color.at(i));	 
    nColBackgroundHit  [it.first]->SetLineColor  (color.at(i));	 
    nColSignalCluster  [it.first]->SetLineColor  (color.at(i));	 
    nColNoiseCluster   [it.first]->SetLineColor  (color.at(i));
    nIndNoiseHit       [it.first]->SetLineColor  (color.at(i));
    nIndSignalHit      [it.first]->SetLineColor  (color.at(i));	 
    nIndBackgroundHit  [it.first]->SetLineColor  (color.at(i));	 
    nIndSignalCluster  [it.first]->SetLineColor  (color.at(i));	 
    nIndNoiseCluster   [it.first]->SetLineColor  (color.at(i));
    nMatSignalCluster  [it.first]->SetLineColor  (color.at(i));	 
    nMatNoiseCluster   [it.first]->SetLineColor  (color.at(i));
    legend->AddEntry(effEnergyCollection[it.first], Form("ADC Threshold = %i", it.first/10), "L");
    i++;
  }


  TCanvas *c = new TCanvas("c", "c");
  c->Print((OutFileName + "[").c_str());
  
  for (auto const it: diffGraphs) {
    if (it.first == 40) it.second->Draw("AP*");
    else                it.second->Draw("P*");
  }
  c->Print((OutFileName)      .c_str());

  electronEnergy->Draw();
  c->Print((OutFileName)      .c_str());

  matClusterSize->Draw();
  c->Print((OutFileName)      .c_str());

  PlotAll(effEnergyCollection);
  c->Print((OutFileName)      .c_str());

  PlotAll(effEnergyInduction);
  c->Print((OutFileName)      .c_str());

  PlotAll(effEnergyMatched);
  c->Print((OutFileName)      .c_str());

  PlotAll(nColNoiseHit         );
  c->Print((OutFileName)      .c_str());

  PlotAll(nColSignalHit	       );
  c->Print((OutFileName)      .c_str());

  PlotAll(nColBackgroundHit    );	   
  c->Print((OutFileName)      .c_str());

  PlotAll(nIndNoiseHit         );
  c->Print((OutFileName)      .c_str());

  PlotAll(nIndSignalHit	       );
  c->Print((OutFileName)      .c_str());

  PlotAll(nIndBackgroundHit    );	   
  c->Print((OutFileName)      .c_str());

  PlotAll(nColSignalCluster    );	   
  c->Print((OutFileName)      .c_str());

  PlotAll(nColNoiseCluster);
  c->Print((OutFileName)      .c_str());

  PlotAll(nIndSignalCluster    );	   
  c->Print((OutFileName)      .c_str());

  PlotAll(nIndNoiseCluster);
  c->Print((OutFileName)      .c_str());

  PlotAll(nMatSignalCluster    );	   
  c->Print((OutFileName)      .c_str());

  PlotAll(nMatNoiseCluster);
  c->Print((OutFileName)      .c_str());

  c->Clear();
  legend->Draw();
  c->Print(OutFileName.c_str());
  c->Print((OutFileName + "]").c_str());



}
