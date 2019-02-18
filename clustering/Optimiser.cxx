#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TGraph.h"
#include "Helper.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <random>

class KS {

private:
  TH1D* sign_pdf_;
  TH1D* back_pdf_;
  int nThrow_;
  double LMCEvent_;
  double BackgroundEvent_;
  std::default_random_engine generator;
  std::poisson_distribution<int> distribution_signal;
  std::poisson_distribution<int> distribution_background;
public:
  KS():
    sign_pdf_(nullptr),
    back_pdf_(nullptr),
    nThrow_(0),
    LMCEvent_(0),
    BackgroundEvent_(0),
    generator(),
    SignalKS(),
    BackgroundKS(){
  }
  void SetBackground(TH1D* back_pdf) {back_pdf_ = (TH1D*) back_pdf->Clone();}
  void SetSignal(TH1D* sign_pdf) {sign_pdf_ = (TH1D*) sign_pdf->Clone();}
  void SetNBackgroundEvent(double BackgroundEvent) {BackgroundEvent_ = BackgroundEvent;}
  void SetNSignalEvent(double SignalEvent) {LMCEvent_ = SignalEvent;}
  void SetNToys(int nToys) { nThrow_ = nToys; }
  
  KS(const KS& t) {
    if (t.sign_pdf_) this->sign_pdf_        = (TH1D*)t.sign_pdf_->Clone();
    if (t.back_pdf_) this->back_pdf_        = (TH1D*)t.back_pdf_->Clone();
    this->nThrow_          = t.nThrow_         ;
    this->LMCEvent_        = t.LMCEvent_       ;
    this->BackgroundEvent_ = t.BackgroundEvent_;
    this->SignalKS         = t.SignalKS        ;
    this->BackgroundKS     = t.BackgroundKS    ;
  }
  

public:
  std::vector<double> SignalKS    ;
  std::vector<double> BackgroundKS;
  void GetKSStat(){
    // TH1D* back_thrown = (TH1D*) sign_pdf_->Clone();
    // TH1D* sign_thrown = (TH1D*) back_pdf_->Clone();
    // back_thrown->SetName("back_thrown");
    // sign_thrown->SetName("sign_thrown");
    SignalKS    .clear();
    BackgroundKS.clear();
    distribution_signal     = std::poisson_distribution<int>(LMCEvent_);
    distribution_background = std::poisson_distribution<int>(BackgroundEvent_);
    for (int i=0; i<nThrow_; ++i) {
      // back_thrown->Reset();
      // sign_thrown->Reset();
      int nSignal     = distribution_signal(generator);
      int nBackground = distribution_background(generator);

      for (int nCluster=0; nCluster<nSignal; ++nCluster) {
        //sign_thrown->Fill(sign_pdf_->GetRandom());
      }
    
      for (int nCluster=0; nCluster<nBackground; ++nCluster) {
        //back_thrown->Fill(back_pdf_->GetRandom());
      }
      
      // SignalKS    .push_back(sign_thrown->KolmogorovTest(back_pdf_, ""));
      // BackgroundKS.push_back(back_thrown->KolmogorovTest(back_pdf_, ""));
       SignalKS    .push_back(0);
       BackgroundKS.push_back(0);
    }
    std::cout << "in thread " << SignalKS    .size() << std::endl;
    std::cout << "in thread " << BackgroundKS.size() << std::endl;
  }
};

int main(int argc, char *argv[]) {
  (void)argc;
  TH1D* KSTest_sign = new TH1D("KS_signal_sign", ";KS Test result (background hypothesis);Rate [Hz]", 100000, 0, 1.01);
  TH1D* KSTest_back = new TH1D("KS_signal_back", "", 100000, 0, 1.1);
  KSTest_sign->SetLineColor(kRed);
  KSTest_back->SetLineColor(kBlue);
  KSTest_sign->SetStats(0);
  KSTest_back->SetStats(0);

  gRandom = new TRandom3();

  std::vector<KS> KSs;
  std::vector<std::thread> threads;
  TFile* f = new TFile("~/Desktop/RealTest.pdf.root", "READ");
  TH1D* back_pdf = (TH1D*)f->Get((std::string("h_")+argv[1]+"_back_wire").c_str());
  TH1D* sign_pdf = (TH1D*)f->Get((std::string("h_")+argv[1]+"_sign_wire").c_str());
  
  double BackgroundEvent = back_pdf->Integral() * 10.;
  double SignalEvent = 10*0.791258;
  std::cout << "BackgroundEvent " << BackgroundEvent << std::endl;

  for (int iThread=0; iThread<2; ++iThread) {
    KSs.push_back(KS());
    KSs.back().SetBackground(back_pdf);
    KSs.back().SetSignal(sign_pdf);
    KSs.back().SetNBackgroundEvent(BackgroundEvent);
    KSs.back().SetNSignalEvent(SignalEvent);
    KSs.back().SetNToys(10000);
    //KSs.back().GetKSStat();
    threads.push_back(std::thread(&KS::GetKSStat, &KSs.back()));
  }
  std::cout << "threads setup" << std::endl;
  for (auto& it: threads) {
    it.join();
  }
  double ntoys=0;
  for (auto const& k: KSs) {
    std::cout << "once thread is done " << k.SignalKS.size() << std::endl;
    std::cout << "once thread is done " << k.BackgroundKS.size() << std::endl;
    for (auto const& it: k.SignalKS) {
      KSTest_sign->Fill(it);
      ntoys++;
    }
    for (auto const& it: k.BackgroundKS) {
      KSTest_back->Fill(it);
    }
  }
  KSTest_sign->Scale(0.1 /ntoys);
  KSTest_back->Scale(0.1 /ntoys);
  std::vector<double> max = {KSTest_sign->GetMaximum(),KSTest_back->GetMaximum()};
  
  TCanvas c;
  gPad->SetLogy();
  
  KSTest_sign->SetMinimum(1.e-2 / ntoys);
  KSTest_sign->SetMaximum(10. * (*std::max_element(max.begin(), max.end())));
  c.Print((std::string("KS")+argv[1]+".pdf[").c_str());
  KSTest_sign->Draw();
  KSTest_back->Draw("SAME");
  c.SaveAs((std::string("KS")+argv[1]+".pdf").c_str());

  TGraph* gr = new TGraph(KSTest_sign->GetNbinsX());
  for (int i=0; i<gr->GetN(); i++) {
    double eff = KSTest_sign->Integral(0, i) * 0.791258 / KSTest_sign->Integral();
    double background = KSTest_back->Integral(0, i);
    gr->SetPoint(i, background,eff);
  }
  gPad->SetLogy(0);
  gPad->SetLogx(1);

  gr->SetTitle(";Background rate [Hz];Efficiency");
  gr->Draw("ALP");
  
  c.SaveAs((std::string("KS")+argv[1]+".pdf").c_str());
  c.SaveAs((std::string("KS")+argv[1]+".pdf]").c_str());


  TFile f2((std::string("KS")+argv[1]+".root").c_str(), "RECREATE");
  gr->Write();
  KSTest_sign->Write();
  KSTest_back->Write();
  f2.Close();
  
  return 0;
  
}
