#include <algorithm>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TPad.h"
#include "TTree.h"


using namespace std;

int main(int argc, char** argv){

  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  int nHitCut=-1, RequestedConfig=0;
  std::string InputFile;
  std::string OutputFile;
  int nEvent = 0;

  while ( (opt = getopt(argc, argv, "h:o:i:n:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'h':
      nHitCut = atoi(optarg);
      break;
    case 'i':
      InputFile = optarg;
      break;
    case 'c':
      RequestedConfig = atoi(optarg);
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case 'o':
      OutputFile = optarg;
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
  
  TGraph *flux_HEP = new TGraph("/Users/plasorak/Downloads/SNOFluxSpectrum_HEP.txt");
  TGraph *flux_B8  = new TGraph("/Users/plasorak/Downloads/SNOFluxSpectrum_B8.txt");
  ifstream xsec_file;
  xsec_file.open("/Users/plasorak/Downloads/marley_ve_cc_total_xs.txt");

  double E, xsec1_d, xsec2_d;
  std::vector<double> E_vec, xsec1_vec, xsec2_vec;
  while (xsec_file >> E >> xsec1_d >> xsec2_d) {
    E_vec    .push_back(E);
    xsec1_vec.push_back(xsec1_d);
    xsec2_vec.push_back(xsec2_d);
  }
  
  TGraph* xsec1 = new TGraph(xsec1_vec.size());
  TGraph* xsec2 = new TGraph(xsec2_vec.size());
  xsec1->SetTitle("");
  xsec2->SetTitle("");
  xsec1->SetLineColor(kRed);
  xsec2->SetLineColor(kBlue);
  
  for (size_t i=0; i<xsec1_vec.size(); ++i) {
    xsec1->SetPoint(i, E_vec[i], xsec1_vec[i]);
    xsec2->SetPoint(i, E_vec[i], xsec2_vec[i]);
  }

  // Translate these from area normalised to actual fluxes
  for (size_t i=0; i<flux_B8->GetN(); ++i) {
    double E, flux;
    flux_B8->GetPoint(i, E, flux);
    flux_B8->SetPoint(i, E, flux*5.69e+06);
  }
  for (size_t i=0; i<flux_HEP->GetN(); ++i) {
    double E, flux;
    flux_HEP->GetPoint(i, E, flux);
    flux_HEP->SetPoint(i, E, flux*7.93e+03);
  }


  flux_HEP->SetTitle("");
  flux_B8 ->SetTitle("");
  flux_HEP->SetLineColor(kRed);
  flux_B8 ->SetLineColor(kBlue);
  
  TCanvas* c = new TCanvas();
  c->Print("SolarWeights.pdf[");
  gPad->SetLogy();
  gPad->SetLogx();
  flux_HEP->GetXaxis()->SetLimits(0.1,50.);
  flux_HEP->GetXaxis()->SetTitle("E_{#nu} [MeV]");
  flux_HEP->GetYaxis()->SetTitle("#Phi [s^{-1} cm^{-2}]");
  flux_HEP->GetHistogram()->SetMaximum(1e7);
  flux_HEP->GetHistogram()->SetMinimum(1e-1);
  flux_HEP->Draw("AL"); 
  flux_B8 ->Draw("L");
  
  TLegend* leg_flux = new TLegend(0.1,0.8,0.3,0.9);
  leg_flux->AddEntry(flux_HEP,"HEP Solar flux", "L");
  leg_flux->AddEntry(flux_B8 ,"B8 Solar flux" , "L");
  leg_flux->Draw();
  c->Print("SolarWeights.pdf");
  c->Clear();

  gPad->SetLogy();
  gPad->SetLogx();
  xsec1->GetXaxis()->SetLimits(0.1,50.);
  xsec1->GetHistogram()->SetMaximum(1e-38);
  xsec1->GetHistogram()->SetMinimum(1e-45);
  xsec1->GetXaxis()->SetTitle("E_{#nu} [MeV]");
  xsec1->GetYaxis()->SetTitle("#sigma [cm^{2}]");
  xsec1->Draw("AL"); 
  xsec2->Draw("L");
  TLegend* leg_xsec = new TLegend(0.1,0.8,0.3,0.9);
  leg_xsec->AddEntry(xsec1,"XSec option 1", "L");
  leg_xsec->AddEntry(xsec2,"XSec option 2", "L");
  leg_xsec->Draw();
  c->Print("SolarWeights.pdf");
  c->Clear();
  
  TGraph *rate_HEP = new TGraph(flux_HEP->GetN());
  TGraph *rate_B8  = new TGraph(flux_B8 ->GetN());
  //                   10  k     T->kg in g     
  double rate_factor = 10.*1000.*1000.*1000.*6.022140857e23/39.948*60.*60.*24./3.; // 10kT (in g) * avogadro number / molar mass of argon * (seconds/day) * oscillation probability (1./3.)
  
  for (size_t i=0; i<flux_HEP->GetN(); ++i) {
    double E, HEP;
    flux_HEP->GetPoint(i, E, HEP);
    double cross_section = xsec2->Eval(E);
    rate_HEP->SetPoint(i, E, HEP * rate_factor * cross_section);
  }
  
  for (size_t i=0; i<flux_B8->GetN(); ++i) {
    double E, B8;
    flux_B8->GetPoint(i, E, B8);
    double cross_section = xsec2->Eval(E);
    rate_B8->SetPoint(i, E, B8 * rate_factor * cross_section);
  }

  double E_max;
  double dum;
  flux_HEP->GetPoint(flux_HEP->GetN()-1,E_max, dum);
  std::vector<double> E_all_vec;
  for (size_t i=0; i<flux_HEP->GetN(); ++i) {
    double E, HEP;
    flux_HEP->GetPoint(i, E, HEP);
    E_all_vec.push_back(E);
  }
  
  for (size_t i=0; i<flux_B8->GetN(); ++i) {
    double E, B8;
    flux_B8->GetPoint(i, E, B8);
    E_all_vec.push_back(E);
  } 

  std::sort(E_all_vec.begin(), E_all_vec.end());
  TGraph *rate_Tot = new TGraph(E_all_vec.size());
  
  double E_max_B8 ;
  double E_min_B8 ;
  double E_max_HEP;
  double E_min_HEP;
  rate_B8 ->GetPoint(0,                  E_min_B8 , dum);
  rate_B8 ->GetPoint(rate_B8 ->GetN()-1, E_max_B8 , dum);
  rate_HEP->GetPoint(0,                  E_min_HEP, dum);
  rate_HEP->GetPoint(rate_HEP->GetN()-1, E_max_HEP, dum);

  for (size_t i=0; i<E_all_vec.size(); ++i) {
    double it=E_all_vec[i];
    if        (E_min_B8  < it && it < E_max_B8  &&
               E_min_HEP < it && it < E_max_HEP) {
      double r_B8  = rate_B8 ->Eval(it);
      double r_HEP = rate_HEP->Eval(it);
      rate_Tot->SetPoint(i, it, r_B8+r_HEP);
    } else if (E_min_B8 < it  && it < E_max_B8)  {
      double r_B8  = rate_B8 ->Eval(it);
      rate_Tot->SetPoint(i, it, r_B8);
    } else if (E_min_HEP < it && it < E_max_HEP) {
      double r_HEP = rate_HEP->Eval(it);
      rate_Tot->SetPoint(i, it, r_HEP);
    } else {
      rate_Tot->SetPoint(i, it, 0.);
    }
  }
  
  gPad->SetLogy();
  gPad->SetLogx();
  rate_B8 ->SetLineColor(kRed); 
  rate_HEP->SetLineColor(kBlue);
  rate_Tot->SetLineColor(kBlack);
  rate_B8 ->GetXaxis()->SetLimits(1.,50.);
  rate_B8 ->SetTitle(""); 
  rate_HEP->SetTitle("");
  rate_Tot->SetTitle("");
  rate_B8->GetXaxis()->SetTitle("E_{#nu} [MeV]");
  rate_B8->GetYaxis()->SetTitle("Events / 10 kT / day");
  rate_B8 ->Draw("AL"); 
  rate_HEP->Draw("L");
  rate_Tot->Draw("L");
  TLegend* leg_rate = new TLegend(0.1,0.7,0.3,0.9);
  leg_rate->AddEntry(rate_B8,  "B8 rate" ,   "L");
  leg_rate->AddEntry(rate_HEP, "HEP rate",   "L");
  leg_rate->AddEntry(rate_Tot, "total rate", "L");
  leg_rate->Draw();
  c->Print("SolarWeights.pdf");
  TH1D* rate_tot_th1 = new TH1D("rate_tot_th1","",100,3,20);
  for (size_t i=0; i<=rate_tot_th1->GetXaxis()->GetNbins(); ++i) {
    rate_tot_th1->SetBinContent(i, std::max(0.,rate_Tot->Eval(rate_tot_th1->GetBinCenter(i))));
    rate_tot_th1->SetBinError(i, 0);
  }

  rate_tot_th1->Scale(rate_Tot->Integral() / rate_tot_th1->Integral());
  TH1D* integrated_rate_tot_th1 = new TH1D("integrated_rate_tot_th1",
                                           ";E_{threshold} [MeV];Events / 10 kT / day",
                                           100,3,20);
  for (size_t i=0; i<=rate_tot_th1->GetXaxis()->GetNbins(); ++i) {
    integrated_rate_tot_th1->SetBinContent(i, rate_tot_th1->Integral(i, rate_tot_th1->GetXaxis()->GetNbins()));
  }
  
  integrated_rate_tot_th1->SetStats(0);
  integrated_rate_tot_th1->SetLineColor(kBlue);
  integrated_rate_tot_th1->Draw();
  c->Print("SolarWeights.pdf");
  
  TFile* file_weights = new TFile("../clustering/data/RateMarley.root", "READ");
  TTree* tree = (TTree*)file_weights->Get("snanagaushit/SNSimTree");
  std::cout << "The original tree had " <<tree->GetEntries() << " entries. " << std::endl;
  TH1D* rate_tot_sn_th1 = new TH1D("rate_tot_sn_th1", ";E_{#nu} [MeV];SN#nu PDF", 100, 3, 20);
  tree->Project("rate_tot_sn_th1", "True_ENu*1000.");
  rate_tot_sn_th1->SetStats(0);
  rate_tot_sn_th1->Scale(1. / rate_tot_sn_th1->Integral(0,99));
  rate_tot_sn_th1->Draw();
  c->Print("SolarWeights.pdf");

  TH1D* weight_tot_th1 = (TH1D*)rate_tot_th1->Clone();
  weight_tot_th1->Divide(rate_tot_sn_th1);
  weight_tot_th1->SetStats(0);
  weight_tot_th1->SetTitle(";E_{#nu} [MeV];Weight [Solar events / 10 kT / day] / [SN event]");
  weight_tot_th1->Draw();
  c->Print("SolarWeights.pdf");
  c->Print("SolarWeights.pdf]");

  TFile* output_weight_file = new TFile("WeightFile.root", "RECREATE");
  weight_tot_th1->Write("SolarNu_weight");
  output_weight_file->Close();

  return 0;
}
