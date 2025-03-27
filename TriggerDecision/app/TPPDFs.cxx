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
  //                           bool logx=false, bool logy=false, const char* title="", double maxv=10, double minv=0.1){
                             bool logx=false, bool logy=false, const char* title=""){

  std::cout << "PlotHistos!" << std::endl;
  //std::vector<double> max = {h1_->GetBinContent(h1_->GetMaximumBin()),
  //                           h2_->GetBinContent(h2_->GetMaximumBin()),
  //                           maxv};
  //std::vector<double> min = {GetMinNonZeroBinContent(h1_),
  //                           GetMinNonZeroBinContent(h2_),
  //                           minv};
  //double ScaleMax=1.2;
  //double ScaleMin=0.;
  
  if (logy) {
    //ScaleMax = 5;
    //ScaleMin = 0.1;
  //  for (auto it = min.begin(); it!=min.end();)
  //    if (*it == 0) {
  //      it = min.erase(it);
  //    } else {
  //      it++;
  //    }
  }

  h1_->SetStats(0);
  h2_->SetStats(0);
  std::cout << "Set stats" << std::endl;
  //h1_->SetMaximum((*std::max_element(max.begin(),max.end()))*ScaleMax);
  //h1_->SetMaximum(1.0);
  //h1_->SetMinimum((*std::min_element(min.begin(),min.end()))*ScaleMin);
  //std::cout << "Set min/max" << std::endl;
  gPad->SetLogx(logx);
  gPad->SetLogy(logy);
  std::cout << "Set log" << std::endl;
  h1_->SetTitle(title);
  std::cout << "Set title" << std::endl;
  h1_->Draw();
  h2_->Draw("SAME");
  std::cout << "Drawn" << std::endl;
  c_.Print(filename.c_str());
  std::cout << "Printed" << std::endl;
}


int main(int argc, char** argv){
  CLI::App app{"A program to produce a pdf of signal and background ADC distributions for each config"};

  std::string InputSignalFileName  = "";
  std::string InputSignal2FileName  = "";
  std::string OutputFileName = "";
  
  app.add_option("--inputsignal",     InputSignalFileName,     "Input signal file name (root)"    )->required();
  app.add_option("--inputsignal2",    InputSignal2FileName,    "Input background file name (root)")->required();
  app.add_option("-o,--output",       OutputFileName,          "Output file name (with pdf extension)")->required();
  CLI11_PARSE(app, argc, argv);

  if (InputSignalFileName == "") {
    std::cerr << "Need to provide an input signal file" << std::endl;
    exit(1);
  }

  //if (InputBackgroundFileName == "") {
  //  std::cerr << "Need to provide an input background file" << std::endl;
  //  exit(1);
  //}
   
  TFile *InputSignalFile = new TFile(InputSignalFileName.c_str(), "READ");
  TFile *InputSignal2File = new TFile(InputSignal2FileName.c_str(), "READ");
  //TVectorD* effs = (TVectorD*)InputSignalFile->Get("Efficiencies");


  TCanvas c;
  c.Print((OutputFileName+"[").c_str());
 
  //std::vector<TH1D*> SignalPDFs;
  //std::vector<TH1D*> Signal2PDFs;

  //for (int conf=0; conf<48; ++conf){
  std::cout << "Getting PDFs" << std::endl;
  TH1D* SignalPDF_     = (TH1D*)InputSignalFile    ->Get(Form("adc_integral"));
  TH1D* Signal2PDF_    = (TH1D*)InputSignal2File   ->Get(Form("adc_integral"));
  std::cout << "Got PDFs" << std::endl;

  //double Efficiency_   = (*effs)[conf];

  //BackgroundPDF_->Scale(1./0.12); //detector scaling
  //BackgroundPDF_->Scale(10);      //time window scaling
  //BackgroundPDF_->Scale(1./BackgroundPDF_->Integral());

  //SignalPDF_->Scale(10 * Efficiency_ / SignalPDF_->Integral()); 
  //SignalPDF_->Add(BackgroundPDF_);

  //SignalPDF_->SetLineColor(kRed);
  //Signal2PDF_->SetLineColor(kBlue);
  //SignalPDF_->SetLineStyle(1);
  //Signal2PDF_->SetLineStyle(1);
  //SignalPDF_->SetLineWidth(2);
  //Signal2PDF_->SetLineWidth(2);
  
  //gPad->SetTicks();
  //gPad->SetGridx();
  //gPad->SetGridy();
  
  std::cout << "Set plot formatting" << std::endl;

  //SignalPDFs.push_back(SignalPDF_);
  //Signal2PDFs.push_back(Signal2PDF_);
  //std::cout << "Got signal & background PDFs for config " << conf << std::endl;
  //}

  //std::cout << "SignalPDFs: " << SignalPDFs.size() << std::endl;
  //std::cout << "BackgroundPDFs: " << BackgroundPDFs.size() << std::endl;

  //for (int i=1; i<2; i++){
    //const char* config = std::to_string(conf).c_str();
    //PlotHistos(SignalPDF_[i]      , Signal2PDF_[i]      , c, OutputFileName, false, false);
    //std::cout << "Plotted PDFs" << std::endl;
  //}

  SignalPDF_->SetStats(0);
  Signal2PDF_->SetStats(0);

  gPad->SetLogx(false);
  gPad->SetLogy(false);
  std::cout << "Set log" << std::endl;
  //SignalPDF_->SetTitle(title);
  std::cout << "Set title" << std::endl;
  SignalPDF_->Draw();
  Signal2PDF_->Draw("SAME");
  std::cout << "Drawn" << std::endl;
  c.Print(OutputFileName.c_str());
  std::cout << "Printed" << std::endl;


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
