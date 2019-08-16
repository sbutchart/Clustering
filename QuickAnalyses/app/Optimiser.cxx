#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TGraph.h"
#include "TVector3.h"
#include "TMath.h"
//#include <numeric>
#include <iostream>
#include <thread>
#include <functional>
#include <atomic>
#include <random>
#include <time.h>

#include "Utils/CLI11.hpp"
#include "Utils/Helper.h"

std::piecewise_constant_distribution<double> GetDistrib(TH1D* histo_) {
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
  TH1D* th1_sign;
  TH1D* th1_back;
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
    th1_sign(nullptr),
    th1_back(nullptr),
    SignalKS(),
    BackgroundKS(){
  }
  void SetBackground      (TH1D* back_pdf)         { back_pdf_ = GetDistrib(back_pdf); th1_back = (TH1D*)back_pdf->Clone(); }
  void SetSignal          (TH1D* sign_pdf)         { sign_pdf_ = GetDistrib(sign_pdf); th1_sign = (TH1D*)sign_pdf->Clone(); }
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
    if (t.th1_sign) this->th1_sign = (TH1D*)t.th1_sign->Clone();
    if (t.th1_back) this->th1_back = (TH1D*)t.th1_back->Clone();
  }
  ~KS() {
    if (th1_sign) {
      delete th1_sign;
      th1_sign = nullptr;
    }
    if (th1_back) {
      delete th1_back;
      th1_back = nullptr;
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
    TH1D* th1_thrown_back = (TH1D*)th1_back->Clone();
    TH1D* th1_thrown_sign = (TH1D*)th1_sign->Clone();
    for (int i=0; i<nThrow_; ++i) {
      if (i%1000==0) std::cout << i << "/" << nThrow_ << std::endl;
      int nSignal      = rand.Poisson(LMCEvent_);
      int nBackground  = rand.Poisson(BackgroundEvent_);
      int nBackground2 = rand.Poisson(BackgroundEvent_);
      th1_thrown_back->Reset();
      th1_thrown_sign->Reset();

      for (int nCluster=0; nCluster<nSignal; ++nCluster) {
        th1_thrown_sign->Fill(th1_sign->GetRandom());
      }
      for (int nCluster=0; nCluster<nBackground; ++nCluster) {
        th1_thrown_sign->Fill(th1_back->GetRandom());
      }
    
      for (int nCluster=0; nCluster<nBackground2; ++nCluster) {
        th1_thrown_back->Fill(th1_back->GetRandom());
      }

      if (th1_thrown_sign->GetEntries() != 0) {
        SignalKS.push_back(th1_thrown_sign->Chi2Test(th1_back));
        if (SignalKS.back() >= 1.) {
          std::cout << "Weird sign " << SignalKS.back() << std::endl;
        }
      } else {
        SignalKS.push_back(1.);
      }

      if (th1_thrown_back->GetEntries() != 0) {
        BackgroundKS.push_back(th1_thrown_back->Chi2Test(th1_back));  
        if (SignalKS.back() >= 1.) {
          std::cout << "Weird back" << SignalKS.back()<< std::endl;
        }
      } else {
        BackgroundKS.push_back(1.);  
      }
    }

    std::cout << "in thread " << SignalKS    .size() << std::endl;
    std::cout << "in thread " << BackgroundKS.size() << std::endl;
    
  } // GetKSStat
};

int main(int argc, char *argv[]) {

  CLI::App app{"KS Optimiser"};

  std::string filename = "";
  int nThread = 4;
  std::string param = "sadc";
  std::string outputfilename = "KS_optimiser.pdf";
  app.add_option("-f,--file", filename, "Input filename");
  app.add_option("-t,--thread", nThread, "Number of thread this will run on");
  app.add_option("-p,--param", param, "Parameter name");
  app.add_option("-o,--output", outputfilename, "Output file name");

  CLI11_PARSE(app, argc, argv);

  int nBin = 10;
  TH1D* KSTest_sign = new TH1D("KS_signal_sign", ";KS Test result (background hypothesis);Rate [Hz]", nBin, 0, 1);
  TH1D* KSTest_back = new TH1D("KS_signal_back", "", nBin, 0, 1);
  KSTest_sign->SetLineColor(kRed);
  KSTest_back->SetLineColor(kBlue);
  KSTest_sign->SetStats(0);
  KSTest_back->SetStats(0);
  gRandom = new TRandom3();

  std::vector<KS> KSs;
  std::vector<std::thread> threads;
  TFile* f = new TFile(filename.c_str(), "READ");
  TH1D* back_pdf = (TH1D*)f->Get((std::string("h_")+param+"_back_wire").c_str());
  TH1D* sign_pdf = (TH1D*)f->Get((std::string("h_")+param+"_sign_wire").c_str());
  TVector3* Efficiency = (TVector3*)f->Get("Efficiency");
  TVector3* BackgroundRate = (TVector3*)f->Get("BackgroundRate");

  AddOverflow(back_pdf);
  AddOverflow(sign_pdf);
 
  double BackgroundEvent = BackgroundRate->X() * 10.;
  back_pdf->Scale(back_pdf->GetEntries() / back_pdf->Integral());
  sign_pdf->Scale(sign_pdf->GetEntries() / sign_pdf->Integral());
  double SignalEvent = 10 * Efficiency->X();
  std::cout << "BackgroundEvent " << BackgroundEvent << std::endl;
  std::cout << "Entries " << back_pdf->GetEntries() << std::endl;

  int nToys = (double)back_pdf->GetEntries() / 10. / (double) nThread;
  if (nToys == 0) {
    nToys = (double)back_pdf->GetEntries() / 10.;
    nThread = 1;
  }
  if (nToys < 10) {
    std::cout << "not enough stats to make toys!" << std::endl;
    exit(0);
  }
  
  for (int iThread=0; iThread<nThread; ++iThread) {
    KSs.push_back(KS());
    KSs.back().UniqueID = iThread;
    KSs.back().SetSeed((iThread+1)*2968);
    KSs.back().SetBackground(back_pdf);
    KSs.back().SetSignal    (sign_pdf);
    KSs.back().SetNBackgroundEvent(BackgroundEvent);
    KSs.back().SetNSignalEvent    (SignalEvent);
    KSs.back().SetNToys(nToys);
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
  
  KSTest_sign->SetBinContent(100, KSTest_sign->GetBinContent(100) +  KSTest_sign->GetBinContent(101));
  KSTest_back->SetBinContent(100, KSTest_back->GetBinContent(100) +  KSTest_back->GetBinContent(101));

  KSTest_sign->SetBinContent(101, 0);
  KSTest_back->SetBinContent(101, 0);
  std::vector<double> max = {KSTest_sign->GetMaximum(),KSTest_back->GetMaximum()};
  std::vector<double> min = {KSTest_sign->GetMinimum(),KSTest_back->GetMinimum()};
  
  TCanvas c;
  gPad->SetLogy();
  
  //KSTest_sign->SetMinimum(1.e-1 *);
  KSTest_sign->SetMaximum(10. * (*std::max_element(max.begin(), max.end())));
  c.SaveAs((outputfilename+"[").c_str());
  KSTest_sign->Draw();
  KSTest_back->Draw("SAME");
  c.SaveAs(outputfilename.c_str());

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
  
  c.SaveAs(outputfilename.c_str());
  c.SaveAs((outputfilename+"]").c_str());


  TFile f2((outputfilename+".root").c_str(), "RECREATE");
  gr->Write();
  KSTest_sign->Write();
  KSTest_back->Write();
  f2.Close();
  
  return 0;
  
}
