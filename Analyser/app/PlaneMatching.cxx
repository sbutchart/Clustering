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
#include "TGraphAsymmErrors.h"
#include "TArrow.h"
#include "TMath.h"

#ifndef ROOT_Math_DistFuncMathCore
#include "Math/DistFuncMathCore.h"
#endif

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

double BinomialExpP(double signalExp, double backgroundExp, double relativeBkgUncert) {
  double mainInf      = signalExp+backgroundExp;
  double tau          = 1./backgroundExp/(relativeBkgUncert*relativeBkgUncert);
  double auxiliaryInf = backgroundExp*tau;

  double P_Bi = TMath::BetaIncomplete(1./(1.+tau),mainInf,auxiliaryInf+1);
  return P_Bi;
}

double PValueToSignificance(double pvalue) {
  return ::ROOT::Math::normal_quantile_c(pvalue, 1);
}

double BinomialExpZ(double signalExp, double backgroundExp, double relativeBkgUncert) {
  return PValueToSignificance(BinomialExpP(signalExp, backgroundExp, relativeBkgUncert));
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
    // aaim[30] = new SNAnaInputManager(); aaim[30]->SetInputTree("snanarunsum3/SNSimTree" );
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

  std::map<int, TEfficiency*> effEnergyCollection;
  std::map<int, TEfficiency*> effEnergyMatched;

  std::map<int, TH1D*> clusterSizeCollection;
  std::map<int, TH1D*> clusterSizeMatched;
  std::map<int, TH1D*> clusterSADCCollection;
  std::map<int, TH1D*> clusterSADCMatched;
  
  std::map<int, TH1D*> clusterSignalSADCCollection;
  std::map<int, TH1D*> clusterSignalSADCMatched   ;
  std::map<int, TH1D*> clusterNoiseSADCCollection ;
  std::map<int, TH1D*> clusterNoiseSADCMatched    ;

  std::map<int, TGraph*> clusterZNCollection;
  std::map<int, TGraph*> clusterZNMatched;

  for (auto const& it: aaim) {
    int fNEvent = nEvent;
    int adc_threshold = it.first;
    SNAnaInputManager* im = it.second;
    
    if (nEvent == -1) fNEvent = im->GetEntries();
    else              fNEvent = nEvent;

    effEnergyCollection   [adc_threshold] = new TEfficiency(Form("effCol_Th%i", adc_threshold), 
							    "Collection Plane Only;Electron KE [MeV];", 100, 0, 10);
    effEnergyMatched      [adc_threshold] = new TEfficiency(Form("effMat_Th%i", adc_threshold), 
							    "Cluster matching on three planes;Electron KE [MeV];", 100, 0, 10);
    clusterSizeCollection [adc_threshold] = new TH1D(Form("clustSizeCol_Th%i", adc_threshold), 
						    "Cluster size - Collection;nHits in cluster;", 20, 0, 20);
    clusterSizeMatched    [adc_threshold] = new TH1D(Form("clustSizeMat_Th%i", adc_threshold),  
						    "Cluster size - Matched;nHits in cluster;", 20, 0, 20);
    clusterSADCCollection [adc_threshold] = new TH1D(Form("clustSADCCol_Th%i", adc_threshold),
						     "Cluster SADC - Collection;SADC;", 500, 0, 5000);
    clusterSADCMatched    [adc_threshold] = new TH1D(Form("clustSADCMat_Th%i", adc_threshold),
						     "Cluster SADC - Matched;SADC;", 50, 0, 5000);
    clusterSignalSADCCollection[adc_threshold]  = new TH1D(Form("clustSignalSADCCol_Th%i", adc_threshold), 
							   Form("clustSignalSADCCol_Th%i;SADC;", adc_threshold), 50, 0, 5000);
    clusterSignalSADCMatched   [adc_threshold]  = new TH1D(Form("clustSignalSADCMat_Th%i", adc_threshold), 
							   Form("clustSignalSADCMat_Th%i;SADC;", adc_threshold), 50, 0, 5000);
    clusterNoiseSADCCollection [adc_threshold]  = new TH1D(Form("clustNoiseSADCCol_Th%i" , adc_threshold), 
							   Form("clustNoiseSADCCol_Th%i;SADC;", adc_threshold) , 50, 0, 5000);
    clusterNoiseSADCMatched    [adc_threshold]  = new TH1D(Form("clustNoiseSADCMat_Th%i" , adc_threshold), 
							   Form("clustNoiseSADCMat_Th%i;SADC;", adc_threshold) , 50, 0, 5000);
    


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

      std::vector<WireCluster*> vec_WireClusterU; 
      std::vector<WireCluster*> vec_WireClusterV; 
      std::vector<WireCluster*> vec_WireClusterZ; 
      clusteng.ClusterHits2(vec_WireHitU, vec_WireClusterU); wiretrigger.IsTriggering(vec_WireClusterU);
      clusteng.ClusterHits2(vec_WireHitV, vec_WireClusterV); wiretrigger.IsTriggering(vec_WireClusterV);
      clusteng.ClusterHits2(vec_WireHitZ, vec_WireClusterZ); wiretrigger.IsTriggering(vec_WireClusterZ);
      
      std::vector<WireCluster*> test_Cluster;
      if (vec_WireClusterZ.size()) {
      	clusteng.HitMatching(2, vec_WireHitZ, vec_WireClusterU, vec_WireClusterV, vec_WireClusterZ, test_Cluster);
      	wiretrigger.IsTriggering(test_Cluster);
      }

      std::vector<WireCluster*> vec_MatchedCluster;
      clusteng.HitMatching2(3, 
      			    vec_WireHitU,
      			    vec_WireHitV,
      			    vec_WireHitZ,
      			    vec_WireClusterZ,
      			    vec_MatchedCluster);
      wiretrigger.IsTriggering(vec_MatchedCluster);

      bool zSelected = false;
      bool mSelected = false;

      for (auto const& clust: vec_WireClusterZ) {
	if (clust->GetIsTriggering()) {
	  if (clust->GetType()) zSelected = true;
	}
      }
      for (auto const& clust: vec_MatchedCluster) {
	if (clust->GetIsTriggering()) {
	  if (clust->GetType()) mSelected = true;
	}
      }

      double KE = (im->True_Bck_Energy->at(0) * 1000) - 0.511;
      effEnergyCollection[adc_threshold]->Fill(zSelected, KE);
      effEnergyMatched   [adc_threshold]->Fill(mSelected, KE);

      for (auto const& clust: vec_WireClusterZ  ) { 
	clusterSizeCollection[adc_threshold]->Fill(clust->GetHit().size()); 
	double TotalCharge = 0;
	for (auto const& hit: clust->GetHit()) 
	  TotalCharge +=  hit->GetPeak();
	clusterSADCCollection[adc_threshold]->Fill(TotalCharge);
	if (clust->GetType()) clusterSignalSADCCollection[adc_threshold]->Fill(TotalCharge);
	else                  clusterNoiseSADCCollection [adc_threshold]->Fill(TotalCharge);
      }

      for (auto const& clust: vec_MatchedCluster) { 
	clusterSizeMatched   [adc_threshold]->Fill(clust->GetHit().size()); 
	double TotalCharge = 0;
	for (auto const& hit: clust->GetHit())
	  TotalCharge += hit->GetPeak();
	clusterSADCMatched[adc_threshold]->Fill(TotalCharge);
	if (clust->GetType()) clusterSignalSADCMatched[adc_threshold]->Fill(TotalCharge);
	else                  clusterNoiseSADCMatched [adc_threshold]->Fill(TotalCharge);

      }
      
      for (auto& it: vec_WireHitU      ) { delete it; it = NULL; }
      for (auto& it: vec_WireHitV      ) { delete it; it = NULL; }
      for (auto& it: vec_WireHitZ      ) { delete it; it = NULL; }
      for (auto& it: vec_WireClusterU  ) { delete it; it = NULL; }
      for (auto& it: vec_WireClusterV  ) { delete it; it = NULL; }
      for (auto& it: vec_WireClusterZ  ) { delete it; it = NULL; }
      for (auto& it: test_Cluster      ) { delete it; it = NULL; }      
      for (auto& it: vec_MatchedCluster) { delete it; it = NULL; }
      vec_WireHitU      .clear();
      vec_WireHitV      .clear();
      vec_WireHitZ      .clear();
      vec_WireClusterU  .clear();
      vec_WireClusterV  .clear();
      vec_WireClusterZ  .clear();
      test_Cluster      .clear();
      vec_MatchedCluster.clear();


    } // Looping through the events

  } // Looping through the AAIMs

  // --- Fill all of the ZN plots
  for (auto const& it: clusterSignalSADCCollection) {
    int nBins = clusterSignalSADCCollection[it.first]->GetNbinsX();
    clusterZNCollection[it.first] = new TGraph(nBins);
    for (int i=0; i<nBins; ++i) {
      double znValue = BinomialExpZ(clusterSignalSADCCollection[it.first]->GetBinContent(i),
				    clusterNoiseSADCCollection [it.first]->GetBinContent(i),
				    0.3);
      if (!isinf(znValue))
	clusterZNCollection[it.first]->SetPoint(clusterZNCollection[it.first]->GetN(),
						clusterSignalSADCCollection[it.first]->GetBinCenter(i),
						znValue);
    }
  }
  for (auto const& it: clusterSignalSADCMatched) {
    int nBins = clusterSignalSADCMatched[it.first]->GetNbinsX();
    clusterZNMatched[it.first] = new TGraph(nBins);
    for (int i=0; i<nBins; ++i) {
      double znValue = BinomialExpZ(clusterSignalSADCMatched[it.first]->GetBinContent(i),
				    clusterNoiseSADCMatched [it.first]->GetBinContent(i),
				    0.3);
      if (!isinf(znValue))
	clusterZNMatched[it.first]->SetPoint(clusterZNMatched[it.first]->GetN(),
					     clusterSignalSADCMatched[it.first]->GetBinCenter(i),
					     znValue);
    }
  }





  std::map<int, TGraph*> Improvement;

  for (auto const& it: effEnergyCollection) {
    auto *gCollection = effEnergyCollection[it.first]->CreateGraph();
    auto *gMatched    = effEnergyMatched   [it.first]->CreateGraph();
    Improvement[it.first] = new TGraph(100);
    double x[100]; double y[100]; double mx[100]; double my[100];
    for (int i=0; i<100; ++i) {
      gMatched   ->GetPoint(i, mx[i], my[i]);
      gCollection->GetPoint(i,  x[i],  y[i]);
      Improvement[it.first]->SetPoint(Improvement[it.first]->GetN(), x[i], my[i]-y[i]);
    }

  }

  const std::vector<int> color = getColors();
  int i=0;
  TLegend* legend = new TLegend(0.1,0.1,0.9,0.9);
  for (auto const& it: effEnergyCollection) {
    clusterSizeCollection[it.first]->SetStats(0);
    clusterSizeMatched   [it.first]->SetStats(0);
    clusterSADCCollection[it.first]->SetStats(0);
    clusterSADCMatched   [it.first]->SetStats(0);
    effEnergyCollection  [it.first]->SetMarkerColor(color.at(i));
    effEnergyMatched     [it.first]->SetMarkerColor(color.at(i));
    clusterSizeCollection[it.first]->SetMarkerColor(color.at(i));
    clusterSizeMatched   [it.first]->SetMarkerColor(color.at(i));
    clusterSADCCollection[it.first]->SetMarkerColor(color.at(i));
    clusterSADCMatched   [it.first]->SetMarkerColor(color.at(i));
    Improvement          [it.first]->SetMarkerColor(color.at(i));
    effEnergyCollection  [it.first]->SetLineColor  (color.at(i));
    effEnergyMatched     [it.first]->SetLineColor  (color.at(i));
    clusterSizeCollection[it.first]->SetLineColor  (color.at(i));
    clusterSizeMatched   [it.first]->SetLineColor  (color.at(i));
    clusterSADCCollection[it.first]->SetLineColor  (color.at(i));
    clusterSADCMatched   [it.first]->SetLineColor  (color.at(i));
    Improvement          [it.first]->SetLineColor  (color.at(i));
    legend->AddEntry(effEnergyCollection[it.first], Form("ADC Threshold = %i", it.first/10), "L");
    i++;
  }

  TCanvas *c = new TCanvas("c", "c");
  c->Print((OutFileName + "[").c_str());

  PlotAll(effEnergyCollection);
  c->Print((OutFileName).c_str());

  PlotAll(effEnergyMatched);
  c->Print((OutFileName).c_str());

  std::string option = "P*";
  for (auto const& it: Improvement) { 
    it.second->SetTitle("");
    it.second->GetXaxis()->SetTitle("Electron KE [MeV]");
    it.second->GetYaxis()->SetTitle("Matched Efficiency - Collection Efficiency");
  }
  PlotAll(Improvement, option);
  TLine *line = new TLine(0, 0, 10, 0);
  line->Draw("SAME");
  c->Print((OutFileName).c_str());

  PlotAll(clusterSizeCollection);
  int height = 20000;
  TLine  *cLine  = new TLine (6, 0, 6, height); 
  TArrow *cArrow = new TArrow(6, height, 8, height); 
  cLine ->SetLineWidth(2); cLine ->SetLineColor(kBlack);
  cArrow->SetLineWidth(2); cArrow->SetArrowSize(0.02); cArrow->SetLineColor(kBlack);
  cLine ->Draw();
  cArrow->Draw();
  c->Print((OutFileName).c_str());
 
  PlotAll(clusterSizeMatched);
  height = 2500;
  TLine  *mLine  = new TLine (6, 0, 6, height); 
  TArrow *mArrow = new TArrow(6, height, 8, height); 
  mLine ->SetLineWidth(2); mLine ->SetLineColor(kBlack);
  mArrow->SetLineWidth(2); mArrow->SetArrowSize(0.02); mArrow->SetLineColor(kBlack);
  mLine ->Draw();
  mArrow->Draw();
  c->Print((OutFileName).c_str());

  c->SetLogy(1);
  if (sigma == 0) {
    for (auto const& it: clusterSADCCollection) {
      it.second->Draw();
      break;
    }
  } else {
    PlotAll(clusterSADCCollection);
  }
  c->Print((OutFileName).c_str());

  c->SetLogy(0);
  PlotAll(clusterSADCMatched);
  c->Print((OutFileName).c_str());

  Double_t x;
  Double_t y;
  for (int i=0; i<clusterZNCollection[40]->GetN(); ++i) {
    clusterZNCollection[40]->GetPoint(i, x, y);
    std::cout << x << " " << y << std::endl;
  }


  TPad *canvas_1 = new TPad("c_1", "c_1", 0.0, 0.3, 1.0, 1.0);
  TPad *canvas_2 = new TPad("c_2", "c_2", 0.0, 0.0, 1.0, 0.3);
  canvas_1->SetBottomMargin(0.01);
  canvas_1->Draw();
  c->cd();
  canvas_2->SetTopMargin(0);
  canvas_2->SetBottomMargin(0.4);
  canvas_2->Draw();
  canvas_2->cd();
  canvas_1->cd();
  canvas_1->SetLogy(1);

  for (auto const& it: clusterSignalSADCCollection) {
    clusterSignalSADCCollection[it.first]->Draw();
    clusterNoiseSADCCollection [it.first]->Draw("SAME");
    clusterSignalSADCCollection[it.first]->SetStats(0);
    clusterNoiseSADCCollection [it.first]->SetStats(0);
    clusterSignalSADCCollection[it.first]->SetLineColor(kBlue);
    clusterNoiseSADCCollection [it.first]->SetLineColor(kRed);

    c->cd(); canvas_2->cd();
    clusterZNCollection[it.first]->Draw("AL");
    clusterZNCollection[it.first]->SetTitle("");
    clusterZNCollection[it.first]->GetYaxis()->SetTitle("Zn 30%");
    c->cd(); canvas_1->cd();
    c->Print((OutFileName).c_str());
  }

  for (auto const& it: clusterSignalSADCMatched) {
    clusterSignalSADCMatched[it.first]->Draw();
    clusterNoiseSADCMatched [it.first]->Draw("SAME");
    clusterSignalSADCMatched[it.first]->SetStats(0);
    clusterNoiseSADCMatched [it.first]->SetStats(0);
    clusterSignalSADCMatched[it.first]->SetLineColor(kBlue);
    clusterNoiseSADCMatched [it.first]->SetLineColor(kRed);

    c->cd(); canvas_2->cd();
    clusterZNMatched[it.first]->Draw("AL");
    clusterZNMatched[it.first]->SetTitle("");
    clusterZNMatched[it.first]->GetYaxis()->SetTitle("Zn 30%");
    c->cd(); canvas_1->cd();
    c->Print((OutFileName).c_str());
  }


  c->Clear();
  legend->Draw();
  c->Print((OutFileName).c_str());
  c->Print((OutFileName + "]").c_str());


  return 0;
}
