#pragma once
#include "TH1D.h"
#include "TMath.h"

#include <memory>
#include <assert.h>
#include <iostream>


class StatisticalTest {
public:
  StatisticalTest():
    PDF_(nullptr) { };

  ~StatisticalTest() {
  };
  double operator()(const TH1D& test) const {
    return Calculate(test);
  }
  double operator()(const std::unique_ptr<TH1D> test) const {
    return Calculate(*test);
  }
  
  double Calculate(const TH1D& test) const {
    assert (PDF_.get() != nullptr);
    AssertGoodHisto(test);
    
    try {
      return Evaluate(test);
    } catch (...) {
      std::cerr << "There was an error with your histograms, returning -999. \n";
      return -999;
    }
  };
  
  virtual void SetPDF(const TH1D* histo) {
    PDF_ = std::unique_ptr<TH1D>((TH1D*)histo->Clone());
    PDF_->SetName((std::string(histo->GetName())+"Clone").c_str());
    PDF_->SetDirectory(NULL);

  };

protected:
  virtual double Evaluate(const TH1D&) const = 0;
  std::unique_ptr<TH1D> PDF_;

private:
  void AssertGoodHisto(const TH1D& test) const {
    if (test.GetNcells() != PDF_->GetNcells()) {
      std::cerr << "Bins are different for test and pdf\n";
      throw;
    }
  }

};


class LikelihoodTest:public StatisticalTest {
public:
  LikelihoodTest():
    StatisticalTest(){};

protected:
  virtual double Evaluate(const TH1D& test) const {
    double llh=0;
    for (int iBin=0; iBin<=test.GetNcells(); ++iBin) {
      double pdfi  = PDF_->GetBinContent(iBin);
      double testi = test.GetBinContent(iBin);
      if (pdfi == 0 && testi == 0) {
        continue;
      } else if (pdfi != 0 && testi == 0) {
        llh += -2 * pdfi;
      } else if (pdfi == 0 && testi != 0) {
        llh += -100;
      } else {
        double contrib = -2 * (testi * TMath::Log(testi / pdfi) + (pdfi - testi));
        if (contrib != contrib || isinf(contrib)) contrib = 0;
        llh += contrib;
      }
    }
  
    return llh;
  }
  
};

class LikelihoodShapeTest:public LikelihoodTest {
public:
  LikelihoodShapeTest():
    LikelihoodTest(){};

  virtual void SetPDF(const TH1D* histo) {
    TH1D* normalised = (TH1D*)histo->Clone();
    double integral = normalised->Integral();
    assert (integral != 0);
    normalised->SetName(((std::string(histo->GetName())+"_norm")).c_str());
    normalised->Scale(1./ integral);
    PDF_.reset(normalised);
    PDF_->SetDirectory(NULL);

  };

protected:
  virtual double Evaluate(const TH1D& test) const {
    TH1D* normalised = (TH1D*)test.Clone();
    double integral = normalised->Integral();
    if (integral <= 0) return 0;
    normalised->Scale(1./ integral);
    return LikelihoodTest::Evaluate(*normalised);
  }
  
};

class LikelihoodNormalisationTest:public LikelihoodTest {
public:
  LikelihoodNormalisationTest():
    LikelihoodTest(){};

  virtual void SetPDF(const TH1D* histo) {
    double integral = histo->Integral();
    PDF_ = std::make_unique<TH1D>((std::string(histo->GetName())+"_norm").c_str(),
                                  histo->GetTitle(), 1, 0, 1);
    PDF_->SetBinContent(1, integral);
    PDF_->SetDirectory(NULL);

  };

protected:
  virtual double Evaluate(const TH1D& test) const {
    double integral = test.Integral();
    if (integral <= 0) return 0;
    std::unique_ptr<TH1D> test_norm = std::unique_ptr<TH1D>(new TH1D((std::string(test.GetName())+"_norm").c_str(),
                                                                test.GetTitle(), 1, 0, 1));
    test_norm->Scale(1./ integral);
    return LikelihoodTest::Evaluate(*test_norm);
  }
  
};
