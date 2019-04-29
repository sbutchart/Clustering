#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TGraph.h"
#include "Helper.h"
//#include <numeric>
#include <iostream>
#include <thread>
#include <functional>
#include <atomic>
#include <random>


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
  std::cout << "Here ";
  if (trial.size() == 0) return 1;
  
  std::vector<double> interval = pdf.intervals();
  std::vector<double> density  = pdf.densities();
  double sum = std::accumulate(density.begin(), density.end(), 0);
  std::vector<double> CDF;
  for (std::vector<double>::iterator it=density.begin(); it!=density.end(); ++it) {
    CDF.push_back(std::accumulate(density.begin(), it, 0) / sum);
  }

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
    diff.push_back(std::abs(CDF.at(interv) - trialCDF[i]));
  }
  double KS = *std::max_element(diff.begin(), diff.end());
  
  double fj[4] = {-2,-8,-18,-32}, r[4];
  const double w = 2.50662827;
  // c1 - -pi**2/8, c2 = 9*c1, c3 = 25*c1
  const double c1 = -1.2337005501361697;
  const double c2 = -11.103304951225528;
  const double c3 = -30.842513753404244;
 
  double p;
  if (KS < 0.2) {
    p = 1;
  } else if (KS < 0.755) {
    double v = 1./(KS*KS);
    p = 1 - w*(exp(c1*v) + exp(c2*v) + exp(c3*v))/KS;
  } else if (KS < 6.8116) {
    r[1] = 0;
    r[2] = 0;
    r[3] = 0;
    double v = KS*KS;
    Int_t maxj = std::max(1,int(3./KS));
    for (Int_t j=0; j<maxj;j++) {
      r[j] = exp(fj[j]*v);
    }
    p = 2*(r[0] - r[1] +r[2] - r[3]);
  } else {
    p = 0;
  }
  std::cout << "Done" << std::endl;
  return p;
  
}



class KS {

private:
  std::piecewise_constant_distribution<double> sign_pdf_;
  std::piecewise_constant_distribution<double> back_pdf_;
  int nThrow_;
  double LMCEvent_;
  double BackgroundEvent_;
  std::default_random_engine generator;
  std::poisson_distribution<int> distribution_signal;
  std::poisson_distribution<int> distribution_background;
public:
  KS():
    sign_pdf_(),
    back_pdf_(),
    nThrow_(0),
    LMCEvent_(0),
    BackgroundEvent_(0),
    generator(),
    SignalKS(),
    BackgroundKS(){
  }
  void SetBackground      (TH1D* back_pdf)         { back_pdf_ = GetDistrib(back_pdf);   }
  void SetSignal          (TH1D* sign_pdf)         { sign_pdf_ = GetDistrib(sign_pdf);   }
  void SetNBackgroundEvent(double BackgroundEvent) { BackgroundEvent_ = BackgroundEvent; }
  void SetNSignalEvent    (double SignalEvent)     { LMCEvent_ = SignalEvent;            }
  void SetNToys           (int nToys)              { nThrow_ = nToys;                    }
  void SetSeed            (int Seed)               { generator.seed(Seed);               }
  
  KS(const KS& t) {
    this->sign_pdf_        = t.sign_pdf_;
    this->back_pdf_        = t.back_pdf_;
    this->nThrow_          = t.nThrow_         ;
    this->LMCEvent_        = t.LMCEvent_       ;
    this->BackgroundEvent_ = t.BackgroundEvent_;
    this->SignalKS         = t.SignalKS        ;
    this->BackgroundKS     = t.BackgroundKS    ;
  }
  int UniqueID;


public:
  std::vector<double> SignalKS    ;
  std::vector<double> BackgroundKS;
  void GetKSStat(){
    std::cout << "UniqueID "<< UniqueID << std::endl;
    distribution_signal     = std::poisson_distribution<int>(LMCEvent_);
    distribution_background = std::poisson_distribution<int>(BackgroundEvent_);
    
    for (int i=0; i<nThrow_; ++i) {
      std::cout << "begin of throw" << std::endl;

      if (i%1000==0) std::cout << i << "/" << nThrow_ << std::endl;
      std::vector<double> back_thrown;
      std::vector<double> sign_thrown;
      int nSignal     = distribution_signal    (generator);
      int nBackground = distribution_background(generator);

      for (int nCluster=0; nCluster<nSignal; ++nCluster) {
        sign_thrown.push_back(sign_pdf_(generator));
      }
    
      for (int nCluster=0; nCluster<nBackground; ++nCluster) {
        back_thrown.push_back(back_pdf_(generator));
      }

      if (sign_thrown.size() != 0) {
        std::cout << "signal " << sign_thrown.size() << std::endl;
        for (auto const& ut: sign_thrown) {
          std::cout << ut << std::endl;
        }
        SignalKS.push_back(GetKolmogorovSmirnovTest(back_pdf_,sign_thrown));
      } else {
        std::cout << "no signal!" << std::endl; SignalKS.push_back(1.);
      }
      
      std::cout << "end of throw" << std::endl;
      BackgroundKS.push_back(GetKolmogorovSmirnovTest(back_pdf_,back_thrown));
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
  TFile* f = new TFile("RealTest.pdf.root", "READ");
  TH1D* back_pdf = (TH1D*)f->Get((std::string("h_")+argv[1]+"_back_wire").c_str());
  TH1D* sign_pdf = (TH1D*)f->Get((std::string("h_")+argv[1]+"_sign_wire").c_str());
  
  double BackgroundEvent = back_pdf->Integral() * 10.;
  double SignalEvent = 10*0.791258;
  std::cout << "BackgroundEvent " << BackgroundEvent << std::endl;
  std::piecewise_constant_distribution<double> distrib =  GetDistrib(back_pdf);
  TH1D* thrown = (TH1D*)back_pdf->Clone();
  thrown->SetLineColor(kRed);
  thrown->SetLineColorAlpha(kRed,0.5);
  thrown->Reset();
  std::default_random_engine generator;
  for (int i=0; i<10*back_pdf->GetEntries(); ++i) {
    thrown->Fill(distrib(generator));
  }
  thrown->Scale(back_pdf->Integral() / thrown->Integral());

  TCanvas c1;
  back_pdf->Draw();
  gPad->SetLogy();
  thrown->Draw("SAME");

  c1.SaveAs("test.pdf");
  for (int iThread=0; iThread<1; ++iThread) {
    KSs.push_back(KS());
    KSs.back().UniqueID = iThread;
    KSs.back().SetSeed((iThread+1)*2968);
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

  TGraph* gr = new TGraph(KSTest_sign->GetNbinsX() / 10);
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
