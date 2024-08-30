#include <algorithm>    // std::min_element, std::max_element
#include <iostream>
#include <fstream>
#include <string>

#include "Utils/Helper.h"
#include "Utils/CLI11.hpp"

#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TPad.h"
#include "TTree.h"
#include "TMath.h"
#include "TVectorT.h"
#include "TLegend.h"
#include "TEfficiency.h"
#include "TStyle.h"
#include "TVector3.h"



inline void PlotHistos(TH1D* h1_, TH1D* h2_, TCanvas& c_, std::string filename,
                             bool logx=false, bool logy=false, const char* title="", double maxv=10, double minv=0.1){

  std::vector<double> max = {h1_->GetBinContent(h1_->GetMaximumBin()),
                             h2_->GetBinContent(h2_->GetMaximumBin()),
                             maxv};
  std::vector<double> min = {GetMinNonZeroBinContent(h1_),
                             GetMinNonZeroBinContent(h2_),
                             minv};
  //double ScaleMax=1.2;
  double ScaleMin=0.;
  
  if (logy) {
    //ScaleMax = 5;
    ScaleMin = 0.1;
    for (auto it = min.begin(); it!=min.end();)
      if (*it == 0) {
        it = min.erase(it);
      } else {
        it++;
      }
  }

  h1_->SetStats(0);
  h2_->SetStats(0);
  //h1_->SetMaximum((*std::max_element(max.begin(),max.end()))*ScaleMax);
  h1_->SetMaximum(1.0);
  h1_->SetMinimum((*std::min_element(min.begin(),min.end()))*ScaleMin);
  gPad->SetLogx(logx);
  gPad->SetLogy(logy);
  h1_->SetTitle(title);
  h1_->Draw();
  h2_->Draw("SAME");
  c_.Print(filename.c_str());
}


int main(int argc, char** argv){
  CLI::App app{"A program to produce a pdf of signal and background ADC distributions for each config"};

  std::string InputSignalFileName  = "";
  std::string InputBackgroundFileName  = "";
  std::string OutputFileName = "";
  
  app.add_option("--inputsignal",     InputSignalFileName,     "Input signal file name (root)"    )->required();
  app.add_option("--inputbackground", InputBackgroundFileName, "Input background file name (root)")->required();
  app.add_option("-o,--output",       OutputFileName,          "Output file name (with pdf extension)")->required();
  CLI11_PARSE(app, argc, argv);

  if (InputSignalFileName == "") {
    std::cerr << "Need to provide an input signal file" << std::endl;
    exit(1);
  }

  if (InputBackgroundFileName == "") {
    std::cerr << "Need to provide an input background file" << std::endl;
    exit(1);
  }
   
  TFile *InputSignalFile = new TFile(InputSignalFileName.c_str(), "READ");
  TFile *InputBackgroundFile = new TFile(InputBackgroundFileName.c_str(), "READ");
  TVectorD* effs = (TVectorD*)InputSignalFile->Get("Efficiencies");


  TCanvas c;
  c.Print((OutputFileName+"[").c_str());
 
  std::vector<TH1D*> SignalPDFs;
  std::vector<TH1D*> BackgroundPDFs;

  for (int conf=0; conf<48; ++conf){
    TH1D* SignalPDF_     = (TH1D*)InputSignalFile    ->Get(Form("PDF_Background_marley_config%i",conf));
    TH1D* BackgroundPDF_ = (TH1D*)InputBackgroundFile->Get(Form("PDF_Background_config%i",  conf));

    double Efficiency_   = (*effs)[conf];

    BackgroundPDF_->Scale(1./0.12); //detector scaling
    BackgroundPDF_->Scale(10);      //time window scaling
    BackgroundPDF_->Scale(1./BackgroundPDF_->Integral());

    SignalPDF_->Scale(10 * Efficiency_ / SignalPDF_->Integral()); 
    SignalPDF_->Add(BackgroundPDF_);

    SignalPDF_->SetLineColor(kRed);
    BackgroundPDF_->SetLineColor(kBlue);
    SignalPDF_->SetLineStyle(1);
    BackgroundPDF_->SetLineStyle(1);
    SignalPDF_->SetLineWidth(2);
    BackgroundPDF_->SetLineWidth(2);
    
    gPad->SetTicks();
    gPad->SetGridx();
    gPad->SetGridy();

    SignalPDFs.push_back(SignalPDF_);
    BackgroundPDFs.push_back(BackgroundPDF_);
    std::cout << "Got signal & background PDFs for config " << conf << std::endl;
  }

  std::cout << "SignalPDFs: " << SignalPDFs.size() << std::endl;
  std::cout << "BackgroundPDFs: " << BackgroundPDFs.size() << std::endl;

  for (int conf=0; conf<48; ++conf){
    const char* config = std::to_string(conf).c_str();
    PlotHistos(SignalPDFs[conf]      , BackgroundPDFs[conf]      , c, OutputFileName, false, false, config);
    std::cout << "Plotted signal & background PDFs for config " << conf << std::endl;
  }

  c.Print((OutputFileName+"]").c_str());

//  std::cout << "Opening root file.." << std::endl;
//  TFile f((OutputFileName+".root").c_str(), "RECREATE");
//  f.cd();
//  for (int conf=0; conf>1; ++conf){
//    std::cout << "Loop 3" << std::endl; 
//    SignalPDFs[conf]        ->Write();
//    BackgroundPDFs[conf]    ->Write();
//    std::cout << "Written PDFs to root file for config " << conf << std::endl;
//  }
//  std::cout << "Finished loop 3" << std::endl;
//  f.Close();

  return 1;

}
