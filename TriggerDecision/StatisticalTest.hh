#pragma once
#include "TH1D.h"

#include <memory>

class StatisticalTest {
public:
  StatisticalTest():
    fPDF(nullptr) { };

  ~StatisticalTest() {
    fPDF.reset(nullptr);
  };
  
  double Calculate(const TH1D* test) const {
    assert (fPDF.get() != nullptr);
    assert (test != nullptr);
    AssertGoodHisto(test);
    
    try {
      return Evaluate(test);
    } catch (...) {
      std::cerr << "There was an error with your histograms, returning -999. \n";
      return -999;
    }
  };
  
  virtual void SetPDF(const TH1D* histo) { fPDF.reset((TH1D*)histo->Clone()); };

protected:
  virtual double Evaluate(const TH1D*) const = 0;
  std::unique_ptr<TH1D> fPDF;

private:
  void AssertGoodHisto(const TH1D* test) const {
    if (test->GetNcells() != fPDF->GetNcells()) {
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
  virtual double const Evaluate(TH1D* test) {
    double llh=0;
    for (int iBin=0; iBin<=test->GetNcells(); ++iBin) {
      double pdfi  = fPDF->GetBinContent(iBin);
      double testi = test->GetBinContent(iBin);
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
    normalised->SetName((std::string(histo->GetName()+"_norm")).c_str());
    normalised->Scale(1./ integral);
    fPDF.reset(normalised);
  };

protected:
  virtual double const Evaluate(TH1D* test) {
    TH1D* normalised = (TH1D*)histo->Clone();
    double integral = normalised->Integral();
    if (integral <= 0) return 0;
    normalised->Scale(1./ integral);
    return LikelihoodTest::Evaluate(normalised);
  }
  
};

class LikelihoodNormalisationTest:public LikelihoodTest {
public:
  LikelihoodNormalisationTest():
    LikelihoodTest(){};

  virtual void SetPDF(const TH1D* histo) {
    double integral = histo->Integral();
    fPDF = std::make_unique<TH1D>(new TH1D((std::string(histo->GetName()+"_norm")).c_str(),
                                           histo->GetTitle(), 1, 0, 1));
    fPDF->SetBinContent(integral);
  };

protected:
  virtual double const Evaluate(TH1D* test) {
    double integral = test->Integral();
    if (integral <= 0) return 0;
    std::unique_ptr<TH1D> test_norm = std::make_unique<TH1D>(new TH1D((std::string(histo->GetName()+"_norm")).c_str(),
                                                                      histo->GetTitle(), 1, 0, 1));
    test_norm->Scale(1./ integral);
    return LikelihoodTest::Evaluate(test_norm.get());
  }
  
}
