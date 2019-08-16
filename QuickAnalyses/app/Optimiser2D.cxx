#include "TFile.h"
#include "TCanvas.h"
#include "TH2D.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TGraph.h"
#include "TMath.h"
//#include <numeric>
#include <iostream>
#include <thread>
#include <functional>
#include <atomic>
#include <random>
#include <time.h>

#include "Utils/Helper.h"

std::piecewise_constant_distribution<double> GetDistrib(TH2D* histo_) {
  std::vector<double> intervals;
  std::vector<double> weights;
  for (int i=1; i<histo_->GetNbinsX(); ++i){
    intervals.push_back(histo_->GetBinLowEdge(i));
    weights.push_back(histo_->GetBinContent(i));
  }
  intervals.push_back(histo_->GetBinLowEdge(histo_->GetNbinsX() - 1) + histo_->GetBinWidth(histo_->GetNbinsX() - 1));
  std::piecewise_constant_distribution<double> distribution (intervals.begin(),intervals.end(),weights.begin());
  return distribution;
}


double GetKolmogorovSmirnovTest(std::piecewise_constant_distribution<double>& pdf, std::vector<double>& trial) {
  
  if (trial.size() == 0) {
    std::cout << "trial has no element" << std::endl;
    return 1; 
  }
  // std::cout << "trial.size() : " << trial.size() << std::endl;
  // for (size_t it=0; it<trial.size(); ++it)
  //   std::cout << "it " << it << " : " << trial[it] << std::endl;
  
  std::vector<double> interval = pdf.intervals();
  std::vector<double> density  = pdf.densities();
  double sum = std::accumulate(density.begin(), density.end(), 0.);
  
  std::vector<double> CDF;
  for (size_t it=0; it<density.size(); ++it) {
    CDF.push_back((double)std::accumulate(density.begin(), density.begin()+it, 0.) / sum);
  }
  CDF.push_back(1.);

  std::sort(trial.begin(), trial.end());
  std::vector<double> trialCDF;
  std::vector<double> diff;
  double step = 1. / trial.size();

  for (size_t i=0; i<trial.size(); ++i) {
    trialCDF.push_back((i+1) * step);
    size_t interv = 0;
    for (; interv<interval.size(); ++interv) {
      if (interval[interv] > trial[i]) {
        break;
      }
    }
    // std::cout << interval[interv] << " " << interv << " " << trial[i] << std::endl;
    diff.push_back(std::abs(CDF.at(interv-1) - trialCDF.back()));
  }
  //std::cout << *std::max_element(diff.begin(), diff.end()) << std::endl;
  double KS = *std::max_element(diff.begin(), diff.end()) * sqrt((double)sum * trial.size() / ((double)sum + trial.size()));
  
  double p = TMath::KolmogorovProb(KS);
  // double fj[4] = {-2,-8,-18,-32}, r[4];
  // const double w = 2.50662827;
  // // c1 - -pi**2/8, c2 = 9*c1, c3 = 25*c1
  // const double c1 = -1.2337005501361697;
  // const double c2 = -11.103304951225528;
  // const double c3 = -30.842513753404244;
 
  // double p;
  // if (KS < 0.2) {
  //   p = 1;
  // } else if (KS < 0.755) {
  //   double v = 1./(KS*KS);
  //   p = 1 - w*(exp(c1*v) + exp(c2*v) + exp(c3*v))/KS;
  // } else if (KS < 6.8116) {
  //   r[1] = 0;
  //   r[2] = 0;
  //   r[3] = 0;
  //   double v = KS*KS;
  //   Int_t maxj = std::max(1,int(3./KS));
  //   for (Int_t j=0; j<maxj;j++) {
  //     r[j] = exp(fj[j]*v);
  //   }
  //   p = 2*(r[0] - r[1] +r[2] - r[3]);
  // } else {
  //   p = 0;
  // }
  // //std::cout << KS << "/" << p << std::endl;
  return p;
  
}



class KS {

private:
  std::piecewise_constant_distribution<double> sign_pdf_;
  std::piecewise_constant_distribution<double> back_pdf_;
  int nThrow_;
  double LMCEvent_;
  double BackgroundEvent_;
  std::default_random_engine     generator;
  std::poisson_distribution<int> distribution_signal;
  std::poisson_distribution<int> distribution_background;
  TRandom3 rand;
  TH2D* th2_sign;
  TH2D* th2_back;
public:
  int UniqueID;
  std::vector<double> SignalKS    ;
  std::vector<double> BackgroundKS;
  
  KS():
    sign_pdf_(),
    back_pdf_(),
    nThrow_(0),
    LMCEvent_(0),
    BackgroundEvent_(0),
    generator(clock()),
    rand(),
    th2_sign(nullptr),
    th2_back(nullptr),
    SignalKS(),
    BackgroundKS(){
  }
  void SetBackground      (TH2D* back_pdf)         { back_pdf_ = GetDistrib(back_pdf); th2_back = (TH2D*)back_pdf->Clone(); }
  void SetSignal          (TH2D* sign_pdf)         { sign_pdf_ = GetDistrib(sign_pdf); th2_sign = (TH2D*)sign_pdf->Clone(); }
  void SetNBackgroundEvent(double BackgroundEvent) { BackgroundEvent_ = BackgroundEvent; }
  void SetNSignalEvent    (double SignalEvent)     { LMCEvent_ = SignalEvent;            }
  void SetNToys           (int nToys)              { nThrow_ = nToys;                    }
  void SetSeed            (int Seed)               { generator.seed(Seed);rand.SetSeed(Seed); }
  
  KS(const KS& t) {
    this->sign_pdf_               = t.sign_pdf_;
    this->back_pdf_               = t.back_pdf_;
    this->nThrow_                 = t.nThrow_;
    this->LMCEvent_               = t.LMCEvent_;
    this->BackgroundEvent_        = t.BackgroundEvent_;
    this->SignalKS                = t.SignalKS;
    this->BackgroundKS            = t.BackgroundKS;
    this->generator               = t.generator;
    this->distribution_signal     = t.distribution_signal;
    this->distribution_background = t.distribution_background;
    this->rand                    = t.rand;
    if (t.th2_sign) this->th2_sign = (TH2D*)t.th2_sign->Clone();
    if (t.th2_back) this->th2_back = (TH2D*)t.th2_back->Clone();
  }
  ~KS() {
    if (th2_sign) {
      delete th2_sign;
      th2_sign = nullptr;
    }
    if (th2_back) {
      delete th2_back;
      th2_back = nullptr;
    }
  }
  
public:

  
  void GetKSStat(){
    
    distribution_signal     = std::poisson_distribution<int>(LMCEvent_);
    distribution_background = std::poisson_distribution<int>(BackgroundEvent_);
    
    for (int i=0; i<nThrow_; ++i) {
      if (i%1000==0) std::cout << i << "/" << nThrow_ << std::endl;
      std::vector<double> back_thrown;
      std::vector<double> sign_thrown;
      int nSignal      = distribution_signal    (generator);
      int nBackground  = distribution_background(generator);
      int nBackground2 = distribution_background(generator);

      for (int nCluster=0; nCluster<nSignal; ++nCluster) {
        sign_thrown.push_back(sign_pdf_(generator));
      }
      for (int nCluster=0; nCluster<nBackground; ++nCluster) {
        sign_thrown.push_back(back_pdf_(generator));
      }
    
      for (int nCluster=0; nCluster<nBackground2; ++nCluster) {
        back_thrown.push_back(back_pdf_(generator));
      }

      if (sign_thrown.size() != 0) {
        SignalKS.push_back(GetKolmogorovSmirnovTest(back_pdf_,sign_thrown));
      } else {
        SignalKS.push_back(1.);
      }

      if (back_thrown.size() != 0) {
        BackgroundKS.push_back(GetKolmogorovSmirnovTest(back_pdf_,back_thrown));  
      } else {
        BackgroundKS.push_back(1.);  
      }
    }

    std::cout << "in thread " << SignalKS    .size() << std::endl;
    std::cout << "in thread " << BackgroundKS.size() << std::endl;
    
  } // GetKSStat



  void GetKSStatROOT(){
    TH2D* th2_thrown_back = (TH2D*)th2_back->Clone();
    TH2D* th2_thrown_sign = (TH2D*)th2_sign->Clone();
    for (int i=0; i<nThrow_; ++i) {
      if (i%1000==0) std::cout << i << "/" << nThrow_ << std::endl;
      int nSignal      = rand.Poisson(LMCEvent_);
      int nBackground  = rand.Poisson(BackgroundEvent_);
      int nBackground2 = rand.Poisson(BackgroundEvent_);
      th2_thrown_back->Reset();
      th2_thrown_sign->Reset();
      double x,y;

      for (int nCluster=0; nCluster<nSignal; ++nCluster) {
        th2_sign->GetRandom2(x,y);
        th2_thrown_sign->Fill(x,y);
      }
      for (int nCluster=0; nCluster<nBackground; ++nCluster) {
        th2_back->GetRandom2(x,y);
        th2_thrown_sign->Fill(x,y);
      }
    
      for (int nCluster=0; nCluster<nBackground2; ++nCluster) {
        th2_back->GetRandom2(x,y);
        th2_thrown_back->Fill(x,y);
      }

      if (th2_thrown_sign->GetEntries() != 0) {
        SignalKS.push_back(th2_thrown_sign->KolmogorovTest(th2_back));
      } else {
        SignalKS.push_back(1.);
      }

      if (th2_thrown_back->GetEntries() != 0) {
        BackgroundKS.push_back(th2_thrown_back->KolmogorovTest(th2_back));  
      } else {
        BackgroundKS.push_back(1.);  
      }
    }

    std::cout << "in thread " << SignalKS    .size() << std::endl;
    std::cout << "in thread " << BackgroundKS.size() << std::endl;
    
  } // GetKSStat
};

int main(int argc, char *argv[]) {
  (void)argc;

  int nBin = 100;
  TH1D* KSTest_sign = new TH1D("KS_signal_sign", ";KS Test result (background hypothesis);Rate [Hz]", nBin, 0, 1.01);
  TH1D* KSTest_back = new TH1D("KS_signal_back", "", nBin, 0, 1.1);
  KSTest_sign->SetLineColor(kRed);
  KSTest_back->SetLineColor(kBlue);
  KSTest_sign->SetStats(0);
  KSTest_back->SetStats(0);
  gRandom = new TRandom3();

  std::vector<KS> KSs;
  std::vector<std::thread> threads;
  TFile* f = new TFile("../build/RealTest.pdf.root", "READ");
  TH2D* back_pdf = (TH2D*)f->Get((std::string("h_")+argv[1]+"_back_wire").c_str());
  TH2D* sign_pdf = (TH2D*)f->Get((std::string("h_")+argv[1]+"_sign_wire").c_str());
  int nThread = atoi(argv[2]);
  
  double BackgroundEvent = back_pdf->Integral() * 10.;
  back_pdf->Scale(back_pdf->GetEntries() / back_pdf->Integral());
  sign_pdf->Scale(sign_pdf->GetEntries() / sign_pdf->Integral());
  double SignalEvent = 10*0.791258;
  std::cout << "BackgroundEvent " << BackgroundEvent << std::endl;
  std::cout << "Entries " << back_pdf->GetEntries() << std::endl;

  
  for (int iThread=0; iThread<nThread; ++iThread) {
    KSs.push_back(KS());
    KSs.back().UniqueID = iThread;
    KSs.back().SetSeed((iThread+1)*2968);
    KSs.back().SetBackground(back_pdf);
    KSs.back().SetSignal    (sign_pdf);
    KSs.back().SetNBackgroundEvent(BackgroundEvent);
    KSs.back().SetNSignalEvent    (SignalEvent);
    KSs.back().SetNToys((double)back_pdf->GetEntries() / 10. / (double) nThread);
  }

  for (std::vector<KS>::iterator it = KSs.begin(); it != KSs.end(); ++it) {
    threads.push_back(std::thread(&KS::GetKSStatROOT, it));
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

  gr->SetTitle(";Fake rate [Hz];Efficiency");
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
