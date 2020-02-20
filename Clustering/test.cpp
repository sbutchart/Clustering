#include <iostream>
#include <thread>


class KS {


public:
  worker (int seed):
    sign_pdf_(nullptr),
    back_pdf_(nullptr),
    nThrow_(100),
    LMCEvent_(10*0.791258),
    BackgroundEvent_(0),
    rand_(seed * 134638286),
    SignalKS(),
    BackgroundKS(){
    TFile* f = new TFile("~/Desktop/SmallTest.pdf.root", "READ");
    back_pdf_ = (TH1D*)f->Get("h_sadc_back_wire");
    sign_pdf_ = (TH1D*)f->Get("h_sadc_sign_wire");
    BackgroundEvent_ = back_pdf_->Integral() * 10.;
    std::cout << "BackgroundEvent " << BackgroundEvent_ << std::endl;
  }
  
public:
  std::vector<double> SignalKS;
  std::vector<double> BackgroundKS;
  void Getworker Stat(){
    TH1D* back_thrown = (TH1D*) sign_pdf_->Clone();
    TH1D* sign_thrown = (TH1D*) back_pdf_->Clone();
    for (int i=0; i<nThrow_; ++i) {
      back_thrown->Reset();
      sign_thrown->Reset();
      int nSignal = rand_.Poisson(LMCEvent_);
      int nBackground = rand_.Poisson(BackgroundEvent_);

      for (int nCluster=0; nCluster<nSignal; ++nCluster) {
        sign_thrown->Fill(sign_pdf_->GetRandom());
      }
    
      for (int nCluster=0; nCluster<nBackground; ++nCluster) {
        back_thrown->Fill(back_pdf_->GetRandom());
      }
      
      SignalKS    .push_back(sign_thrown->KolmogorovTest(back_pdf_, ""));
      BackgroundKS.push_back(back_thrown->KolmogorovTest(back_pdf_, ""));
    }
    std::cout << "in thread " << SignalKS    .size() << std::endl;
    std::cout << "in thread " << BackgroundKS.size() << std::endl;
  }
};

int main() {
  TH1D* KSTest_sign = new TH1D("KS_signal_sign", ";KS Test result (background hypothesis);n Events", 100, 0, 1.1);
  TH1D* KSTest_back = new TH1D("KS_signal_back", "", 100, 0, 1.1);
  KSTest_sign->SetLineColor(kRed);
  KSTest_back->SetLineColor(kBlue);
  KSTest_sign->SetStats(0);
  KSTest_back->SetStats(0);

  gRandom = new TRandom3();

  std::vector<KS> KSs;
  std::vector<std::thread> threads;
  for (int iThread=0; iThread<1; ++iThread) {
    
    KSs.push_back(KS(iThread));
    //KSs.back().GetKSStat();
    threads.push_back(std::thread(&KS::GetKSStat, KSs.back()));
  }

  for (auto& it: threads) {
    it.join();
  }

  for (auto const& k: KSs) {
    std::cout << "once thread is done " << k.SignalKS.size() << std::endl;
    std::cout << "once thread is done " << k.BackgroundKS.size() << std::endl;
    for (auto const& it: k.SignalKS) {
      KSTest_sign->Fill(it);
    }
    for (auto const& it: k.BackgroundKS) {
      KSTest_back->Fill(it);
    }
  }

  TCanvas c;
  gPad->SetLogy();
  KSTest_sign->SetMinimum(1.e-1);
  KSTest_sign->Draw();
  KSTest_back->Draw("SAME");
  c.SaveAs("KS_test.pdf");
  return 0;
}
