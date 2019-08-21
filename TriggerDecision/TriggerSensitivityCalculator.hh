#pragma once

#include <thread>
#include <future>

#include "TriggerDecision/StatisticalTest.hh"
#include "Utils/Helper.h"

#include "TRandom3.h"
#include "TH1D.h"
#include "TTree.h"
#include "TMath.h"
#include "TFile.h"
#include "TVectorT.h"


class TriggerSensitivityCalculator{
public:
  TriggerSensitivityCalculator(const int         nSNEvent      ,
                               const float       TimeWindow    ,
                               const int         nToy          ,
                               const int         nThread       ,
                               const int         Config        ,
                               const std::string InputFileName ,
                               const std::string OutputFileName,
                               const std::string Method        ) :
    nSNEvent_                 (nSNEvent      ),
    TimeWindow_               (TimeWindow    ),
    nToy_                     (nToy          ),
    nThread_                  (nThread       ),
    Config_                   (Config        ),
    InputFileName_            (InputFileName ),
    OutputFileName_           (OutputFileName),
    InputFile_                (new TFile(InputFileName_ .c_str(), "READ")),
    OutputFile_               (new TFile(OutputFileName_.c_str(), "RECREATE")),
    SignalPDF_                (nullptr),
    BackgroundPDF_            (nullptr),
    Efficiency_               (0),
    BackgroundRate_           (0),
    SignalStatisticVector_    (),
    BackgroundStatisticVector_(),
    SignalStatistic_          (0),
    BackgroundStatistic_      (0),
    OutputTree_               (nullptr),
    nAutoSave_                (1000),
    Seed_                     (82861282),
    Method_                   (Method),
    StatisticalTest_          (nullptr){

    SignalPDF_     = (TH1D*)InputFile_->Get(Form("PDF_Background_1_config%i",Config_));
    BackgroundPDF_ = (TH1D*)InputFile_->Get(Form("PDF_Background_config%i",  Config_));
    SignalPDF_    ->SetDirectory(NULL);
    BackgroundPDF_->SetDirectory(NULL);
    TVectorD* effs = (TVectorD*)InputFile_->Get("Efficiencies");
    Efficiency_ = (*effs)[Config_];

    assert (BackgroundPDF_->Integral() != 0 &&
            SignalPDF_    ->Integral() != 0);
    
    BackgroundPDF_->Scale(1./0.12); // Detector scaling got lost somewhere
    BackgroundPDF_->Scale(TimeWindow);

    BackgroundRate_ = BackgroundPDF_->Integral();
    
    SignalPDF_->Scale(nSNEvent_ * Efficiency_ / SignalPDF_->Integral());
    SignalPDF_->Add(BackgroundPDF_);

    OutputFile_->cd();
    OutputTree_ = new TTree("Throws", "Throws");
    OutputTree_->Branch("BackgroundTestStatistics3", &BackgroundStatistic_);
    OutputTree_->Branch("SignalTestStatistics",     &SignalStatistic_    );


    if (Method_ == "Likelihood") {
      StatisticalTest_ = std::make_unique<LikelihoodTest>();
    } else if (Method_ == "LikelihoodShape") {
      StatisticalTest_ = std::make_unique<LikelihoodShapeTest>();
    } else if (Method_ == "LikelihoodNorm") {
      StatisticalTest_ = std::make_unique<LikelihoodShapeTest>();
    } else {
      std::cerr << "Method \"" << Method_ << "\" is unknown\n";
      throw;
    }
    StatisticalTest_->SetPDF(BackgroundPDF_);
    
  }


  
  ~TriggerSensitivityCalculator() {
    OutputFile_->Close();
    InputFile_->Close();
  }

  
  void Compute() {
    std::vector<std::future<std::pair<std::vector<double>,std::vector<double>>>> threads;
    for (int iThread=0; iThread<nThread_; ++iThread) {
      threads.push_back(std::async(&TriggerSensitivityCalculator::ThreadedCompute, this));
      Seed_++;
    }
    for (auto& it: threads) {
      std::pair<std::vector<double>, std::vector<double>> signal_back = it.get();
      for (size_t i=0; signal_back.first.size(); ++i) {
        SignalStatistic_     = signal_back.first [i];
        BackgroundStatistic_ = signal_back.second[i];
        OutputTree_->Fill();
      }
    }
    OutputTree_->Write();
  }



  
private:
  int                 nSNEvent_                 ;
  float               TimeWindow_               ;
  int                 nToy_                     ;
  int                 nThread_                  ;
  int                 Config_                   ;
  std::string         InputFileName_            ;
  std::string         OutputFileName_           ;
  TFile*              InputFile_                ;
  TFile*              OutputFile_               ;
  TH1D*               SignalPDF_                ;
  TH1D*               BackgroundPDF_            ;
  double              Efficiency_               ;
  double              BackgroundRate_           ;
  std::vector<double> SignalStatisticVector_    ;
  std::vector<double> BackgroundStatisticVector_;
  double              SignalStatistic_          ;
  double              BackgroundStatistic_      ;
  TTree*              OutputTree_               ;
  int                 nAutoSave_                ;
  int                 Seed_                     ;
  std::string         Method_                   ;
  
  std::unique_ptr<StatisticalTest> StatisticalTest_;


  double GetRandom(TRandom3& rand, TH1D*histo) const {
    Int_t nbinsx = histo->GetNbinsX();
    Double_t integral_total = 0;
    Double_t * integral = histo->GetIntegral();

    if (integral) {
      if (integral[nbinsx+1] != histo->GetEntries()) {
        integral_total = histo->ComputeIntegral(true);
        integral = histo->GetIntegral();
      } else {
       integral_total = integral[nbinsx];
      }
    } else {
       integral_total = histo->ComputeIntegral(true);
       integral = histo->GetIntegral();
    }

    if (integral_total == 0) return 0;

    // return a NaN in case some bins have negative content
    if (integral_total == TMath::QuietNaN() ) return TMath::QuietNaN();
 
    Double_t r1 = rand.Rndm();
    Int_t ibin = TMath::BinarySearch(nbinsx,integral,r1);
    Double_t x = histo->GetBinLowEdge(ibin+1);
    if (r1 > integral[ibin]) x += histo->GetBinWidth(ibin+1) * (r1-integral[ibin])/(integral[ibin+1] - integral[ibin]);
    return x;
 }
 


  
  void ThrowHistos(TRandom3& rand, TH1D* Background, TH1D* Signal) const {
    Background->Reset();
    Signal    ->Reset();
    int nBackground          = rand.Poisson(BackgroundPDF_->Integral());
    int nSignalAndBackground = rand.Poisson(SignalPDF_    ->Integral());

    for (int iBackground=0; iBackground<nBackground; ++iBackground) {
      Background->Fill(GetRandom(rand, BackgroundPDF_));
    }
    
    for (int iSignalAndBackground=0; iSignalAndBackground<nSignalAndBackground; ++iSignalAndBackground) {
      Signal->Fill(GetRandom(rand, SignalPDF_));
    }
  }
  
  std::pair<std::vector<double>,std::vector<double>> ThreadedCompute() {
    TRandom3 rand(Seed_);

    TH1D* SignalTest     = (TH1D*)SignalPDF_    ->Clone();
    TH1D* BackgroundTest = (TH1D*)BackgroundPDF_->Clone();
    SignalTest    ->SetDirectory(NULL);
    BackgroundTest->SetDirectory(NULL);
    
    std::vector<double> SignalStatisticVector;
    std::vector<double> BackgroundStatisticVector;

    for (int iToy=0; iToy<nToy_; ++iToy) {
      PrintProgress(iToy,nToy_);
      ThrowHistos(rand, BackgroundTest, SignalTest);
      double SignalStatistic     = StatisticalTest_->Calculate(*SignalTest);
      double BackgroundStatistic = StatisticalTest_->Calculate(*BackgroundTest);
      
      if (nThread_ == 1) {
        SignalStatistic_     = SignalStatistic    ;
        BackgroundStatistic_ = BackgroundStatistic;
        OutputTree_->Fill();
        if (iToy % nAutoSave_ == 0)
          OutputTree_->AutoSave("SaveSelf");
      } else {
        SignalStatisticVector    .push_back(SignalStatistic    );
        BackgroundStatisticVector.push_back(BackgroundStatistic);
      }
    }
    delete SignalTest    ;
    delete BackgroundTest;
    return std::make_pair(SignalStatisticVector,BackgroundStatisticVector);
  }

};
