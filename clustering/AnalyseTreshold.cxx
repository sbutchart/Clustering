#include <iostream>
#include <unistd.h>

#include "TFile.h"
#include "TCanvas.h"
#include "TEfficiency.h"
#include "TMath.h"
#include "TF1.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TLegend.h"

TF1* f1, *f2, *f3, *f4;
TF1* lin1;

double finter(double *x, double*par) {
  return TMath::Abs(f1->EvalPar(x,par) - f2->EvalPar(x,par));
}

double finter30(double *x, double*par) {
  return TMath::Abs(f1->EvalPar(x,par) - f3->EvalPar(x,par));
}

double finter70(double *x, double*par) {
  return TMath::Abs(f1->EvalPar(x,par) - f4->EvalPar(x,par));
}

int main(int argc, char** argv) {
  
  int opt;

  extern char *optarg;
  
  std::string InFileName  = "";

  while ( (opt = getopt(argc, argv, "i:")) != -1 ) {
    switch (opt) {
    case 'i':
      InFileName = optarg;
      break;
    }
  }

  TFile *f = new TFile(InFileName.c_str(), "READ");

  TEfficiency *TOTAL = (TEfficiency*) f->Get("Total"              ); 
  TEfficiency *R1    = (TEfficiency*) f->Get("Region1"            ); 
  TEfficiency *R2    = (TEfficiency*) f->Get("Region2"            );
  TEfficiency *R3    = (TEfficiency*) f->Get("Region3"            );
  TEfficiency *R4    = (TEfficiency*) f->Get("Region4"            );
  TEfficiency *R5    = (TEfficiency*) f->Get("Region5"            );
  TEfficiency *R6    = (TEfficiency*) f->Get("Region6"            );
  TEfficiency *R7    = (TEfficiency*) f->Get("Region7"            );
  TEfficiency *R8    = (TEfficiency*) f->Get("Region8"            );
  
  TH2D        *timeH = (TH2D*)        f->Get("th2d_timeSpreadPos" );
  TH2D        *chanH = (TH2D*)        f->Get("th2d_chanSpreadPos" );
  TH2D        *hitsH = (TH2D*)        f->Get("th2d_hitsSpreadPos" );
  TH2D        *SADCH = (TH2D*)        f->Get("th2d_SADCSpreadPos" );
  TProfile    *timeP = (TProfile*)    f->Get("tprof_timeSpreadPos");
  TProfile    *chanP = (TProfile*)    f->Get("tprof_chanSpreadPos");
  TProfile    *hitsP = (TProfile*)    f->Get("tprof_hitsSpreadPos");
  TProfile    *SADCP = (TProfile*)    f->Get("tprof_SADCSpreadPos");

  TH2D        *timeHEnergyR1 = (TH2D*)        f->Get("th2d_timeSpreadEnergyR1" );
  TH2D        *chanHEnergyR1 = (TH2D*)        f->Get("th2d_chanSpreadEnergyR1" );
  TH2D        *hitsHEnergyR1 = (TH2D*)        f->Get("th2d_hitsSpreadEnergyR1" );
  TH2D        *SADCHEnergyR1 = (TH2D*)        f->Get("th2d_SADCSpreadEnergyR1" );
  TProfile    *timePEnergyR1 = (TProfile*)    f->Get("tprof_timeSpreadEnergyR1");
  TProfile    *chanPEnergyR1 = (TProfile*)    f->Get("tprof_chanSpreadEnergyR1");
  TProfile    *hitsPEnergyR1 = (TProfile*)    f->Get("tprof_hitsSpreadEnergyR1");
  TProfile    *SADCPEnergyR1 = (TProfile*)    f->Get("tprof_SADCSpreadEnergyR1");

  TH2D        *timeHEnergyR2 = (TH2D*)        f->Get("th2d_timeSpreadEnergyR2" );
  TH2D        *chanHEnergyR2 = (TH2D*)        f->Get("th2d_chanSpreadEnergyR2" );
  TH2D        *hitsHEnergyR2 = (TH2D*)        f->Get("th2d_hitsSpreadEnergyR2" );
  TH2D        *SADCHEnergyR2 = (TH2D*)        f->Get("th2d_SADCSpreadEnergyR2" );
  TProfile    *timePEnergyR2 = (TProfile*)    f->Get("tprof_timeSpreadEnergyR2");
  TProfile    *chanPEnergyR2 = (TProfile*)    f->Get("tprof_chanSpreadEnergyR2");
  TProfile    *hitsPEnergyR2 = (TProfile*)    f->Get("tprof_hitsSpreadEnergyR2");
  TProfile    *SADCPEnergyR2 = (TProfile*)    f->Get("tprof_SADCSpreadEnergyR2");

  TH2D        *timeHEnergyR3 = (TH2D*)        f->Get("th2d_timeSpreadEnergyR3" );
  TH2D        *chanHEnergyR3 = (TH2D*)        f->Get("th2d_chanSpreadEnergyR3" );
  TH2D        *hitsHEnergyR3 = (TH2D*)        f->Get("th2d_hitsSpreadEnergyR3" );
  TH2D        *SADCHEnergyR3 = (TH2D*)        f->Get("th2d_SADCSpreadEnergyR3" );
  TProfile    *timePEnergyR3 = (TProfile*)    f->Get("tprof_timeSpreadEnergyR3");
  TProfile    *chanPEnergyR3 = (TProfile*)    f->Get("tprof_chanSpreadEnergyR3");
  TProfile    *hitsPEnergyR3 = (TProfile*)    f->Get("tprof_hitsSpreadEnergyR3");
  TProfile    *SADCPEnergyR3 = (TProfile*)    f->Get("tprof_SADCSpreadEnergyR3");

  TH2D        *timeHEnergyR4 = (TH2D*)        f->Get("th2d_timeSpreadEnergyR4" );
  TH2D        *chanHEnergyR4 = (TH2D*)        f->Get("th2d_chanSpreadEnergyR4" );
  TH2D        *hitsHEnergyR4 = (TH2D*)        f->Get("th2d_hitsSpreadEnergyR4" );
  TH2D        *SADCHEnergyR4 = (TH2D*)        f->Get("th2d_SADCSpreadEnergyR4" );
  TProfile    *timePEnergyR4 = (TProfile*)    f->Get("tprof_timeSpreadEnergyR4");
  TProfile    *chanPEnergyR4 = (TProfile*)    f->Get("tprof_chanSpreadEnergyR4");
  TProfile    *hitsPEnergyR4 = (TProfile*)    f->Get("tprof_hitsSpreadEnergyR4");
  TProfile    *SADCPEnergyR4 = (TProfile*)    f->Get("tprof_SADCSpreadEnergyR4");

  TH2D        *timeHEnergyR5 = (TH2D*)        f->Get("th2d_timeSpreadEnergyR5" );
  TH2D        *chanHEnergyR5 = (TH2D*)        f->Get("th2d_chanSpreadEnergyR5" );
  TH2D        *hitsHEnergyR5 = (TH2D*)        f->Get("th2d_hitsSpreadEnergyR5" );
  TH2D        *SADCHEnergyR5 = (TH2D*)        f->Get("th2d_SADCSpreadEnergyR5" );
  TProfile    *timePEnergyR5 = (TProfile*)    f->Get("tprof_timeSpreadEnergyR5");
  TProfile    *chanPEnergyR5 = (TProfile*)    f->Get("tprof_chanSpreadEnergyR5");
  TProfile    *hitsPEnergyR5 = (TProfile*)    f->Get("tprof_hitsSpreadEnergyR5");
  TProfile    *SADCPEnergyR5 = (TProfile*)    f->Get("tprof_SADCSpreadEnergyR5");

  TH2D        *timeHEnergyR6 = (TH2D*)        f->Get("th2d_timeSpreadEnergyR6" );
  TH2D        *chanHEnergyR6 = (TH2D*)        f->Get("th2d_chanSpreadEnergyR6" );
  TH2D        *hitsHEnergyR6 = (TH2D*)        f->Get("th2d_hitsSpreadEnergyR6" );
  TH2D        *SADCHEnergyR6 = (TH2D*)        f->Get("th2d_SADCSpreadEnergyR6" );
  TProfile    *timePEnergyR6 = (TProfile*)    f->Get("tprof_timeSpreadEnergyR6");
  TProfile    *chanPEnergyR6 = (TProfile*)    f->Get("tprof_chanSpreadEnergyR6");
  TProfile    *hitsPEnergyR6 = (TProfile*)    f->Get("tprof_hitsSpreadEnergyR6");
  TProfile    *SADCPEnergyR6 = (TProfile*)    f->Get("tprof_SADCSpreadEnergyR6");

  TH2D        *timeHEnergyR7 = (TH2D*)        f->Get("th2d_timeSpreadEnergyR7" );
  TH2D        *chanHEnergyR7 = (TH2D*)        f->Get("th2d_chanSpreadEnergyR7" );
  TH2D        *hitsHEnergyR7 = (TH2D*)        f->Get("th2d_hitsSpreadEnergyR7" );
  TH2D        *SADCHEnergyR7 = (TH2D*)        f->Get("th2d_SADCSpreadEnergyR7" );
  TProfile    *timePEnergyR7 = (TProfile*)    f->Get("tprof_timeSpreadEnergyR7");
  TProfile    *chanPEnergyR7 = (TProfile*)    f->Get("tprof_chanSpreadEnergyR7");
  TProfile    *hitsPEnergyR7 = (TProfile*)    f->Get("tprof_hitsSpreadEnergyR7");
  TProfile    *SADCPEnergyR7 = (TProfile*)    f->Get("tprof_SADCSpreadEnergyR7");

  TH2D        *timeHEnergyR8 = (TH2D*)        f->Get("th2d_timeSpreadEnergyR8" );
  TH2D        *chanHEnergyR8 = (TH2D*)        f->Get("th2d_chanSpreadEnergyR8" );
  TH2D        *hitsHEnergyR8 = (TH2D*)        f->Get("th2d_hitsSpreadEnergyR8" );
  TH2D        *SADCHEnergyR8 = (TH2D*)        f->Get("th2d_SADCSpreadEnergyR8" );
  TProfile    *timePEnergyR8 = (TProfile*)    f->Get("tprof_timeSpreadEnergyR8");
  TProfile    *chanPEnergyR8 = (TProfile*)    f->Get("tprof_chanSpreadEnergyR8");
  TProfile    *hitsPEnergyR8 = (TProfile*)    f->Get("tprof_hitsSpreadEnergyR8");
  TProfile    *SADCPEnergyR8 = (TProfile*)    f->Get("tprof_SADCSpreadEnergyR8");
  
  std::vector<TEfficiency*> regions = {TOTAL, R1, R2, R3, R4, R5, R6, R7, R8};
  
  f1 = new TF1("f1","[0] + [1]*(1 + TMath::Erf((x-[2])/([3])))",-0.5,25);
  lin1 = new TF1("lin1","[0]*TMath::Exp(-[2]*abs(x - [1]))", 0, 8);


  double thresholds[9];
  double efficiencies[9];
  double range[9];
  double resolution[9];
  double slopes[9];
  
  TCanvas *c = new TCanvas();
  c->Print("EfficiencyFits.pdf[");

  for (int i=0; i<9; ++i) {

    efficiencies[i] = regions[i]->GetEfficiency(regions[i]->FindFixBin(5.0));
    f1->SetParameters(1.0, 0.3, 3.0, 0.5);
    regions[i]->Draw();
    gPad->Update();
    auto *graph = regions[i]->GetPaintedGraph();
    graph->SetMinimum(0);
    graph->SetMaximum(1);
    graph->Fit(f1);
    
    f2 = new TF1("f2", "0.75");  // THE 75% EFFICIENCY LINE
    f3 = new TF1("f3", "0.30");  // THE 30% EFFICIENCY LINE
    f4 = new TF1("f4", "0.70");  // THE 70% EFFICIENCY LINE
    
    TF1* fint1 = new TF1("fint1", finter, 0, 30, 0);
    double threshold = fint1->GetMinimumX();

    TF1* fint2 = new TF1("fint2", finter30, 0, 30, 0);
    TF1* fint3 = new TF1("fint3", finter70, 0, 30, 0);
    double RoughSlope = (0.7 - 0.3)/(fint3->GetMinimumX() - fint2->GetMinimumX());
    
    slopes[i] = RoughSlope;
    range[i] = static_cast<double>(i);
    thresholds[i] = threshold;
    resolution[i] = f1->GetParameter(1)/f1->GetParameter(2);
    
    c->Print("EfficiencyFits.pdf");
  }
  c->Print("EfficiencyFits.pdf]");  

  TLegend *legend = new TLegend(0.6,0.6,0.9,0.9);
  legend->AddEntry((TObject*)0, "A = -345 < x < -270 [cm]", "");
  legend->AddEntry((TObject*)0, "B = -270 < x < -180 [cm]", "");
  legend->AddEntry((TObject*)0, "C = -180 < x < -90  [cm]", ""); 
  legend->AddEntry((TObject*)0, "D =  -90 < x < -15  [cm]", ""); 
  legend->AddEntry((TObject*)0, "E =   15 < x < 90   [cm]", ""); 
  legend->AddEntry((TObject*)0, "F =   90 < x < 180  [cm]", ""); 
  legend->AddEntry((TObject*)0, "G =  180 < x < 270  [cm]", ""); 
  legend->AddEntry((TObject*)0, "H =  270 < x < 345  [cm]", ""); 

  TCanvas c1;
  TGraph *Thresh  = new TGraph(9, range, thresholds);
  c1.Print("Threshold.pdf[");
  Thresh->GetXaxis()->SetBinLabel(1 , "Total");
  Thresh->GetXaxis()->SetBinLabel(11, "A");
  Thresh->GetXaxis()->SetBinLabel(22, "B");
  Thresh->GetXaxis()->SetBinLabel(33, "C");
  Thresh->GetXaxis()->SetBinLabel(44, "D");
  Thresh->GetXaxis()->SetBinLabel(55, "E");
  Thresh->GetXaxis()->SetBinLabel(66, "F");
  Thresh->GetXaxis()->SetBinLabel(77, "G");
  Thresh->GetXaxis()->SetBinLabel(88, "H");
  Thresh->Draw("AP+");
  Thresh->SetMarkerStyle(2);
  Thresh->SetMarkerSize(3);
  // legend->Draw("SAME");
  Thresh->SetTitle("Energy threshold for 75% efficiency");
  Thresh->GetXaxis()->SetTitle("Region of the detector");
  Thresh->GetYaxis()->SetTitle("Threshold Energy [MeV]");
  c1.Print("Threshold.pdf");

  TGraph *eff = new TGraph(9, range, efficiencies);
  eff->GetXaxis()->SetBinLabel(1 , "Total");
  eff->GetXaxis()->SetBinLabel(11, "A");
  eff->GetXaxis()->SetBinLabel(22, "B");
  eff->GetXaxis()->SetBinLabel(33, "C");
  eff->GetXaxis()->SetBinLabel(44, "D");
  eff->GetXaxis()->SetBinLabel(55, "E");
  eff->GetXaxis()->SetBinLabel(66, "F");
  eff->GetXaxis()->SetBinLabel(77, "G");
  eff->GetXaxis()->SetBinLabel(88, "H");
  eff->SetTitle("Efficiency at 5 MeV");
  eff->Draw("AP+");
  eff->SetMarkerStyle(2);
  eff->SetMarkerSize(3);
  eff->GetXaxis()->SetTitle("Region of the detector");
  eff->GetYaxis()->SetTitle("Trigger Efficiency");
  c1.Print("Threshold.pdf");

  TGraph *res = new TGraph(9, range, resolution);
  res->SetTitle("Resolution, #mu / #sigma"); 
  res->GetXaxis()->SetBinLabel(1 , "Total");
  res->GetXaxis()->SetBinLabel(11, "A");
  res->GetXaxis()->SetBinLabel(22, "B");
  res->GetXaxis()->SetBinLabel(33, "C");
  res->GetXaxis()->SetBinLabel(44, "D");
  res->GetXaxis()->SetBinLabel(55, "E");
  res->GetXaxis()->SetBinLabel(66, "F");
  res->GetXaxis()->SetBinLabel(77, "G");
  res->GetXaxis()->SetBinLabel(88, "H");
  res->Draw("ALP");
  c1.Print("Threshold.pdf");
  c1.Print("Threshold.pdf]");

  TCanvas *Spread = new TCanvas("Spread", "Spread");
  Spread->Print("Spreads.pdf[");
  timeH->SetTitle("Time Spread");
  timeH->GetYaxis()->SetRangeUser(0, 50);
  timeH->GetYaxis()->SetTitle("#Deltat [#mus]");
  timeH->Draw("COLZ");  
  timeP->Draw("SAME");
  timeP->SetLineWidth(2);
  Spread->Print("Spreads.pdf");

  chanH->SetTitle("Channel Spread");
  chanH->GetYaxis()->SetTitle("#DeltaChannel [#]");
  chanH->GetYaxis()->SetRangeUser(0, 6);
  chanH->Draw("COLZ");  
  chanP->Draw("SAME");
  chanP->SetLineWidth(2);
  Spread->Print("Spreads.pdf");

  SADCH->SetTitle("SADC");
  SADCH->GetYaxis()->SetTitle("SADC");
  SADCH->GetYaxis()->SetRangeUser(0, 1500);
  SADCH->Draw("COLZ");  
  lin1->SetParameters(1200, 4.5, 10);
  SADCP->Fit("lin1");
  SADCP->Draw("SAME");
  SADCP->SetLineWidth(2);
  Spread->Print("Spreads.pdf");

  hitsH->SetTitle("Hit Spread");
  hitsH->GetYaxis()->SetTitle("hits");
  hitsH->GetYaxis()->SetRangeUser(0, 10);
  hitsH->Draw("COLZ");  
  hitsP->Draw("SAME");
  hitsP->SetLineWidth(2);
  Spread->Print("Spreads.pdf");
  Spread->Print("Spreads.pdf]");

  for (auto const& i: slopes) {std::cout << i << " ";}

  // ----------------------------------------------------------------------- //
  
  TCanvas *Slice1Spreads = new TCanvas("Slice1Spreads", "Slice1Spreads");
  Slice1Spreads->Print("Region1Spreads.pdf[");

  timeHEnergyR1->Draw("COLZ");
  timePEnergyR1->Draw("SAME");
  timeHEnergyR1->GetXaxis()->SetRangeUser(0, 10);
  timeHEnergyR1->GetYaxis()->SetRangeUser(0, 60);
  timeHEnergyR1->SetStats(0);
  timePEnergyR1->SetStats(0);
  Slice1Spreads->Print("Region1Spreads.pdf");
    
  chanHEnergyR1->Draw("COLZ");
  chanPEnergyR1->Draw("SAME");
  chanHEnergyR1->GetXaxis()->SetRangeUser(0, 10);
  chanHEnergyR1->GetYaxis()->SetRangeUser(0, 15);
  chanHEnergyR1->SetStats(0);
  chanPEnergyR1->SetStats(0);
  Slice1Spreads->Print("Region1Spreads.pdf");
  
  hitsHEnergyR1->Draw("COLZ");
  hitsPEnergyR1->Draw("SAME");
  hitsHEnergyR1->GetXaxis()->SetRangeUser(0, 10);
  hitsHEnergyR1->GetYaxis()->SetRangeUser(0, 10);
  hitsHEnergyR1->SetStats(0);
  hitsPEnergyR1->SetStats(0);
  Slice1Spreads->Print("Region1Spreads.pdf");
  
  SADCHEnergyR1->Draw("COLZ");
  SADCPEnergyR1->Draw("SAME");
  SADCHEnergyR1->GetXaxis()->SetRangeUser(0, 10);
  SADCHEnergyR1->GetYaxis()->SetRangeUser(0, 20);
  SADCHEnergyR1->SetStats(0);
  SADCPEnergyR1->SetStats(0);
  Slice1Spreads->Print("Region1Spreads.pdf");
  Slice1Spreads->Print("Region1Spreads.pdf]");

  // ----------------------------------------------------------------------- //
  
  TCanvas *Slice2Spreads = new TCanvas("Slice2Spreads", "Slice2Spreads");
  Slice2Spreads->Print("Region2Spreads.pdf[");

  timeHEnergyR2->Draw("COLZ");
  timePEnergyR2->Draw("SAME");
  timeHEnergyR2->GetXaxis()->SetRangeUser(0, 10);
  timeHEnergyR2->GetYaxis()->SetRangeUser(0, 60);
  timeHEnergyR2->SetStats(0);
  timePEnergyR2->SetStats(0);
  Slice2Spreads->Print("Region2Spreads.pdf");
    
  chanHEnergyR2->Draw("COLZ");
  chanPEnergyR2->Draw("SAME");
  chanHEnergyR2->GetXaxis()->SetRangeUser(0, 10);
  chanHEnergyR2->GetYaxis()->SetRangeUser(0, 15);
  chanHEnergyR2->SetStats(0);
  chanPEnergyR2->SetStats(0);
  Slice2Spreads->Print("Region2Spreads.pdf");
  
  hitsHEnergyR2->Draw("COLZ");
  hitsPEnergyR2->Draw("SAME");
  hitsHEnergyR2->GetXaxis()->SetRangeUser(0, 10);
  hitsHEnergyR2->GetYaxis()->SetRangeUser(0, 10);
  hitsHEnergyR2->SetStats(0);
  hitsPEnergyR2->SetStats(0);
  Slice2Spreads->Print("Region2Spreads.pdf");
  
  SADCHEnergyR2->Draw("COLZ");
  SADCPEnergyR2->Draw("SAME");
  SADCHEnergyR2->GetXaxis()->SetRangeUser(0, 10);
  SADCHEnergyR2->GetYaxis()->SetRangeUser(0, 20);
  SADCHEnergyR2->SetStats(0);
  SADCPEnergyR2->SetStats(0);
  Slice2Spreads->Print("Region2Spreads.pdf");
  Slice2Spreads->Print("Region2Spreads.pdf]");

  // ----------------------------------------------------------------------- //
  
  TCanvas *Slice3Spreads = new TCanvas("Slice3Spreads", "Slice3Spreads");
  Slice3Spreads->Print("Region3Spreads.pdf[");

  timeHEnergyR3->Draw("COLZ");
  timePEnergyR3->Draw("SAME");
  timeHEnergyR3->GetXaxis()->SetRangeUser(0, 10);
  timeHEnergyR3->GetYaxis()->SetRangeUser(0, 60);
  timeHEnergyR3->SetStats(0);
  timePEnergyR3->SetStats(0);
  Slice3Spreads->Print("Region3Spreads.pdf");
    
  chanHEnergyR3->Draw("COLZ");
  chanPEnergyR3->Draw("SAME");
  chanHEnergyR3->GetXaxis()->SetRangeUser(0, 10);
  chanHEnergyR3->GetYaxis()->SetRangeUser(0, 15);
  chanHEnergyR3->SetStats(0);
  chanPEnergyR3->SetStats(0);
  Slice3Spreads->Print("Region3Spreads.pdf");
  
  hitsHEnergyR3->Draw("COLZ");
  hitsPEnergyR3->Draw("SAME");
  hitsHEnergyR3->GetXaxis()->SetRangeUser(0, 10);
  hitsHEnergyR3->GetYaxis()->SetRangeUser(0, 10);
  hitsHEnergyR3->SetStats(0);
  hitsPEnergyR3->SetStats(0);
  Slice3Spreads->Print("Region3Spreads.pdf");
  
  SADCHEnergyR3->Draw("COLZ");
  SADCPEnergyR3->Draw("SAME");
  SADCHEnergyR3->GetXaxis()->SetRangeUser(0, 10);
  SADCHEnergyR3->GetYaxis()->SetRangeUser(0, 20);
  SADCHEnergyR3->SetStats(0);
  SADCPEnergyR3->SetStats(0);
  Slice3Spreads->Print("Region3Spreads.pdf");
  Slice3Spreads->Print("Region3Spreads.pdf]");

  // ----------------------------------------------------------------------- //

  TCanvas *Slice4Spreads = new TCanvas("Slice4Spreads", "Slice4Spreads");
  Slice4Spreads->Print("Region4Spreads.pdf[");
  
  timeHEnergyR4->Draw("COLZ");
  timePEnergyR4->Draw("SAME");
  timeHEnergyR4->GetXaxis()->SetRangeUser(0, 10);
  timeHEnergyR4->GetYaxis()->SetRangeUser(0, 60);
  timeHEnergyR4->SetStats(0);
  timePEnergyR4->SetStats(0);
  Slice4Spreads->Print("Region4Spreads.pdf");
    
  chanHEnergyR4->Draw("COLZ");
  chanPEnergyR4->Draw("SAME");
  chanHEnergyR4->GetXaxis()->SetRangeUser(0, 10);
  chanHEnergyR4->GetYaxis()->SetRangeUser(0, 15);
  chanHEnergyR4->SetStats(0);
  chanPEnergyR4->SetStats(0);
  Slice4Spreads->Print("Region4Spreads.pdf");
  
  hitsHEnergyR4->Draw("COLZ");
  hitsPEnergyR4->Draw("SAME");
  hitsHEnergyR4->GetXaxis()->SetRangeUser(0, 10);
  hitsHEnergyR4->GetYaxis()->SetRangeUser(0, 10);
  hitsHEnergyR4->SetStats(0);
  hitsPEnergyR4->SetStats(0);
  Slice4Spreads->Print("Region4Spreads.pdf");
  
  SADCHEnergyR4->Draw("COLZ");
  SADCPEnergyR4->Draw("SAME");
  SADCHEnergyR4->GetXaxis()->SetRangeUser(0, 10);
  SADCHEnergyR4->GetYaxis()->SetRangeUser(0, 20);
  SADCHEnergyR4->SetStats(0);
  SADCPEnergyR4->SetStats(0);
  Slice4Spreads->Print("Region4Spreads.pdf");
  Slice4Spreads->Print("Region4Spreads.pdf]");

  // ----------------------------------------------------------------------- //

  TCanvas *Slice5Spreads = new TCanvas("Slice5Spreads", "Slice5Spreads");
  Slice5Spreads->Print("Region5Spreads.pdf[");

  timeHEnergyR5->Draw("COLZ");
  timePEnergyR5->Draw("SAME");
  timeHEnergyR5->GetXaxis()->SetRangeUser(0, 10);
  timeHEnergyR5->GetYaxis()->SetRangeUser(0, 60);
  timeHEnergyR5->SetStats(0);
  timePEnergyR5->SetStats(0);
  Slice5Spreads->Print("Region5Spreads.pdf");
    
  chanHEnergyR5->Draw("COLZ");
  chanPEnergyR5->Draw("SAME");
  chanHEnergyR5->GetXaxis()->SetRangeUser(0, 10);
  chanHEnergyR5->GetYaxis()->SetRangeUser(0, 15);
  chanHEnergyR5->SetStats(0);
  chanPEnergyR5->SetStats(0);
  Slice5Spreads->Print("Region5Spreads.pdf");
  
  hitsHEnergyR5->Draw("COLZ");
  hitsPEnergyR5->Draw("SAME");
  hitsHEnergyR5->GetXaxis()->SetRangeUser(0, 10);
  hitsHEnergyR5->GetYaxis()->SetRangeUser(0, 10);
  hitsHEnergyR5->SetStats(0);
  hitsPEnergyR5->SetStats(0);
  Slice5Spreads->Print("Region5Spreads.pdf");
  
  SADCHEnergyR5->Draw("COLZ");
  SADCPEnergyR5->Draw("SAME");
  SADCHEnergyR5->GetXaxis()->SetRangeUser(0, 10);
  SADCHEnergyR5->GetYaxis()->SetRangeUser(0, 20);
  SADCHEnergyR5->SetStats(0);
  SADCPEnergyR5->SetStats(0);
  Slice5Spreads->Print("Region5Spreads.pdf");
  Slice5Spreads->Print("Region5Spreads.pdf]");

  // ----------------------------------------------------------------------- //
  
  TCanvas *Slice6Spreads = new TCanvas("Slice6Spreads", "Slice6Spreads");
  Slice6Spreads->Print("Region6Spreads.pdf[");

  timeHEnergyR6->Draw("COLZ");
  timePEnergyR6->Draw("SAME");
  timeHEnergyR6->GetXaxis()->SetRangeUser(0, 10);
  timeHEnergyR6->GetYaxis()->SetRangeUser(0, 60);
  timeHEnergyR6->SetStats(0);
  timePEnergyR6->SetStats(0);
  Slice6Spreads->Print("Region6Spreads.pdf");
    
  chanHEnergyR6->Draw("COLZ");
  chanPEnergyR6->Draw("SAME");
  chanHEnergyR6->GetXaxis()->SetRangeUser(0, 10);
  chanHEnergyR6->GetYaxis()->SetRangeUser(0, 15);
  chanHEnergyR6->SetStats(0);
  chanPEnergyR6->SetStats(0);
  Slice6Spreads->Print("Region6Spreads.pdf");
  
  hitsHEnergyR6->Draw("COLZ");
  hitsPEnergyR6->Draw("SAME");
  hitsHEnergyR6->GetXaxis()->SetRangeUser(0, 10);
  hitsHEnergyR6->GetYaxis()->SetRangeUser(0, 10);
  hitsHEnergyR6->SetStats(0);
  hitsPEnergyR6->SetStats(0);
  Slice6Spreads->Print("Region6Spreads.pdf");
  
  SADCHEnergyR6->Draw("COLZ");
  SADCPEnergyR6->Draw("SAME");
  SADCHEnergyR6->GetXaxis()->SetRangeUser(0, 10);
  SADCHEnergyR6->GetYaxis()->SetRangeUser(0, 20);
  SADCHEnergyR6->SetStats(0);
  SADCPEnergyR6->SetStats(0);
  Slice6Spreads->Print("Region6Spreads.pdf");
  Slice6Spreads->Print("Region6Spreads.pdf]");

  // ----------------------------------------------------------------------- //

  TCanvas *Slice7Spreads = new TCanvas("Slice7Spreads", "Slice7Spreads");
  Slice7Spreads->Print("Region7Spreads.pdf[");

  timeHEnergyR7->Draw("COLZ");
  timePEnergyR7->Draw("SAME");
  timeHEnergyR7->GetXaxis()->SetRangeUser(0, 10);
  timeHEnergyR7->GetYaxis()->SetRangeUser(0, 60);
  timeHEnergyR7->SetStats(0);
  timePEnergyR7->SetStats(0);
  Slice7Spreads->Print("Region7Spreads.pdf");
    
  chanHEnergyR7->Draw("COLZ");
  chanPEnergyR7->Draw("SAME");
  chanHEnergyR7->GetXaxis()->SetRangeUser(0, 10);
  chanHEnergyR7->GetYaxis()->SetRangeUser(0, 15);
  chanHEnergyR7->SetStats(0);
  chanPEnergyR7->SetStats(0);
  Slice7Spreads->Print("Region7Spreads.pdf");
  
  hitsHEnergyR7->Draw("COLZ");
  hitsPEnergyR7->Draw("SAME");
  hitsHEnergyR7->GetXaxis()->SetRangeUser(0, 10);
  hitsHEnergyR7->GetYaxis()->SetRangeUser(0, 10);
  hitsHEnergyR7->SetStats(0);
  hitsPEnergyR7->SetStats(0);
  Slice7Spreads->Print("Region7Spreads.pdf");
  
  SADCHEnergyR7->Draw("COLZ");
  SADCPEnergyR7->Draw("SAME");
  SADCHEnergyR7->GetXaxis()->SetRangeUser(0, 10);
  SADCHEnergyR7->GetYaxis()->SetRangeUser(0, 20);
  SADCHEnergyR7->SetStats(0);
  SADCPEnergyR7->SetStats(0);
  Slice7Spreads->Print("Region7Spreads.pdf");
  Slice7Spreads->Print("Region7Spreads.pdf]");

  // ----------------------------------------------------------------------- //
  
  TCanvas *Slice8Spreads = new TCanvas("Slice8Spreads", "Slice8Spreads");
  Slice8Spreads->Print("Region8Spreads.pdf[");

  timeHEnergyR8->Draw("COLZ");
  timePEnergyR8->Draw("SAME");
  timeHEnergyR8->GetXaxis()->SetRangeUser(0, 10);
  timeHEnergyR8->GetYaxis()->SetRangeUser(0, 60);
  timeHEnergyR8->SetStats(0);
  timePEnergyR8->SetStats(0);
  Slice8Spreads->Print("Region8Spreads.pdf");
    
  chanHEnergyR8->Draw("COLZ");
  chanPEnergyR8->Draw("SAME");
  chanHEnergyR8->GetXaxis()->SetRangeUser(0, 10);
  chanHEnergyR8->GetYaxis()->SetRangeUser(0, 15);
  chanHEnergyR8->SetStats(0);
  chanPEnergyR8->SetStats(0);
  Slice8Spreads->Print("Region8Spreads.pdf");
  
  hitsHEnergyR8->Draw("COLZ");
  hitsPEnergyR8->Draw("SAME");
  hitsHEnergyR8->GetXaxis()->SetRangeUser(0, 10);
  hitsHEnergyR8->GetYaxis()->SetRangeUser(0, 10);
  hitsHEnergyR8->SetStats(0);
  hitsPEnergyR8->SetStats(0);
  Slice8Spreads->Print("Region8Spreads.pdf");
  
  SADCHEnergyR8->Draw("COLZ");
  SADCPEnergyR8->Draw("SAME");
  SADCHEnergyR8->GetXaxis()->SetRangeUser(0, 10);
  SADCHEnergyR8->GetYaxis()->SetRangeUser(0, 20);
  SADCHEnergyR8->SetStats(0);
  SADCPEnergyR8->SetStats(0);
  Slice8Spreads->Print("Region8Spreads.pdf");
  Slice8Spreads->Print("Region8Spreads.pdf]");

  // ----------------------------------------------------------------------- //
  
}
