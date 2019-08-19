#pragma once

#include "TriggerDecision/StatisitcalTest.hh"

class TriggerSensitivityCalculator{
public:
  TriggerSensitivityCalculator(const int         nSNEvent      ,
                               const float       TimeWindow    ,
                               const int         nToy          ,
                               const int         nThread       ,
                               const int         Config        ,
                               const std::string InputFileName ,
                               const std::string OutputFileName) :
    nSNEvent_                 (nSNEvent      ),
    TimeWindow_               (TimeWindow    ),
    nToy_                     (nToy          ),
    nThread_                  (nThread       ),
    Config_                   (Config        ),
    InputFileName_            (InputFileName ),
    OutputFileName_           (OutputFileName),
    InputFile_                (new TFile(InputFileName_ .c_str(), "READ")),
    OutputFile_               (new TFile(OutputFileName_.c_str(), "RECREATE")),
    SignalPDF_                ((TH1D*)InputFile_->Get(Form("PDF_Background_1_config%i",Config_))),
    BackgroundPDF_            ((TH1D*)InputFile_->Get(Form("PDF_Background_config%i",  Config_))),
    Efficiency_               ((TVectorD*)InputFile->Get("Efficiencies")->At(Config_)),
    BackgroundRate_           (0),
    SignalStatisticVector_    (),
    BackgroundStatisticVector_(),
    SignalStatistic_          (0),
    BackgroundStatistic_      (0),
    OutputTree_               (nullptr),
    nAutoSave                 (1000),
    StatisticalTest_          (nullptr),
    Seed_                     (82861282) {

    assert (BackgroundPDF_->Integral() != 0 &&
            SignalPDF_    ->Integral() != 0);
    
    BackgroundPDF_->Scale(1./0.12); // Detector scaling got lost somewhere
    BackgroundPDF_->Scale(TimeWindow);

    BackgroundRate_ = BackgroundPDF_->Integral();
    
    SignalPDF_->Scale(nSNEvent_ * Efficiency_ / SignalPDF_->Integral());
    SignalPDF_->Add(BackgroundPDF_);

    OutputFile_->cd();
    OutputTree_ = new TTree("Throws");
    OutputTree_->Branch("BackgroundTestStatistics", &BackgroundStatistic_);
    OutputTree_->Branch("SignalTestStatistics",     &SignalStatistic_    );
  }


  
  ~TriggerSensitivityCalculator() {
  }

  
  void Compute() {
    for (int iThread=0; iThread<nThread_; ++iThread) {
      ThreadedCompute(iThread+Seed_);
    }
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
  
  std::unique_ptr<StatisticalTest> StatisticalTest_;

  void ThreadedCompute(int seed) {
    TRandom3 rand(seed);

    TH1D SignalTest    ();
    TH1D BackgroundTest();
    
    for (int iToy=0; iToy<nToy_; ++iToy) {

      ThrowHisto()
      
      SignalStatistic_ = StatisticalTest_();
      SignalStatistic_ = StatisticalTest_();


      
      if (nThread_ == 0) {
        OutputTree_->Fill();
        if (iToy % nAutoSave_ == 0)
          OutputTree_->AutoSave("SaveSelf");
      } else {
        SignalStatisticVector_    .push_back(SignalStatistic_    );
        BackgroundStatisticVector_.push_back(BackgroundStatistic_);
      }
    }
  }

}
