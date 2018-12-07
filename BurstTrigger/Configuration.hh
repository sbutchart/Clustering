#ifndef CONFIGURATION_HH
#define CONFIGURATION_HH

#include <algorithm>
#include <map>
#include <iostream>
#include <string>

#include "TArrayD.h"
#include "TAxis.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TH1D.h"

class Configuration: public TObject {
public:
  std::string fName;
  std::string fLegendEntry;
  int         fNClusterCut;
  int         fClusterAlgorithm;
  double      fBackgroundRate;
  double      fBackgroundRateError;
  double      fBurstTimeWindow;
  double      fFractionInTimeWindow;
  double      fClusterEfficiency;
  double      fClusterEfficiencyError;
  double      fTargetFakeRate;
  double      fActualFakeRate;
  std::map<double,double> fFakeRate_Cut;
  std::map<double,double> fEfficiency_Burst;
  std::map<double,double> fEfficiency_Distance;
  std::map<double,double> fCoverage;
  std::map<double,double> fLatency_Burst;
  std::map<double,double> fLatency_Distance;
  std::map<double,double> fLatency_Burst95CL;
  std::map<double,double> fLatency_Distance95CL;

  TH1D* fTH1DFakeRate_Cut;
  TH1D* fTH1DEfficiency_Burst;
  TH1D* fTH1DEfficiency_Distance;
  TH1D* fTH1DCoverage;
  TH1D* fTH1DLatency_Burst;
  TH1D* fTH1DLatency_Distance;
  TH1D* fTH1DLatency_Burst95CL;
  TH1D* fTH1DLatency_Distance95CL;

  std::vector<TH1D*> fTH1DList;

  TF1 * fDistanceParametrisation;
  TH1D* fDistanceProbability;
  
public:
  void SetDistanceProbability    (TH1D* h) { fDistanceProbability     = (TH1D*)h->Clone(); }
  void SetDistanceProbability    (TH1D& h) { fDistanceProbability     = (TH1D*)h.Clone(); }
  void SetDistanceParametrisation(TF1*  f) { fDistanceParametrisation = (TF1*)f->Clone(); }
  void SetDistanceParametrisation(TF1&  f) { fDistanceParametrisation = (TF1*)f.Clone(); }

  TF1 * GetDistanceParametrisation(){ return fDistanceParametrisation; }
  TH1D* GetDistanceProbability    (){ return fDistanceProbability; }
  
  
  Configuration(const Configuration& c) {
    fNClusterCut            = c.fNClusterCut;
    fClusterAlgorithm       = c.fClusterAlgorithm;
    fBackgroundRate         = c.fBackgroundRate;
    fBackgroundRateError    = c.fBackgroundRateError;
    fBurstTimeWindow        = c.fBurstTimeWindow;
    fFractionInTimeWindow   = c.fFractionInTimeWindow;
    fClusterEfficiency      = c.fClusterEfficiency;
    fClusterEfficiencyError = c.fClusterEfficiencyError;
    fLegendEntry            = c.fLegendEntry;
    fTargetFakeRate         = c.fTargetFakeRate;
    fActualFakeRate         = c.fActualFakeRate;

    fFakeRate_Cut        = c.fFakeRate_Cut;
    fEfficiency_Burst    = c.fEfficiency_Burst;
    fEfficiency_Distance = c.fEfficiency_Distance;
    fCoverage            = c.fCoverage;
    fLatency_Burst       = c.fLatency_Burst;
    fLatency_Distance    = c.fLatency_Distance;
    fLatency_Burst95CL       = c.fLatency_Burst95CL;
    fLatency_Distance95CL    = c.fLatency_Distance95CL;

    fTH1DList = c.fTH1DList;
    if (c.fTH1DFakeRate_Cut) { fTH1DFakeRate_Cut = (TH1D*)c.fTH1DFakeRate_Cut->Clone(); }
    else                     { fTH1DFakeRate_Cut = NULL; }

    if (c.fTH1DEfficiency_Burst) { fTH1DEfficiency_Burst = (TH1D*)c.fTH1DEfficiency_Burst->Clone(); }
    else                         { fTH1DEfficiency_Burst = NULL; }
    
    if (c.fTH1DEfficiency_Distance) { fTH1DEfficiency_Distance = (TH1D*)c.fTH1DEfficiency_Distance->Clone(); }
    else                            { fTH1DEfficiency_Distance = NULL; }

    if (c.fTH1DCoverage) { fTH1DCoverage = (TH1D*)c.fTH1DCoverage->Clone(); }
    else                 { fTH1DCoverage = NULL; }
    
    if (c.fTH1DLatency_Burst) { fTH1DLatency_Burst = (TH1D*)c.fTH1DLatency_Burst->Clone(); }
    else                      { fTH1DLatency_Burst = NULL; }
    
    if (c.fTH1DLatency_Distance) { fTH1DLatency_Distance = (TH1D*)c.fTH1DLatency_Distance->Clone(); }
    else                         { fTH1DLatency_Distance = NULL; }

    if (c.fTH1DLatency_Burst95CL) { fTH1DLatency_Burst95CL = (TH1D*)c.fTH1DLatency_Burst95CL->Clone(); }
    else                          { fTH1DLatency_Burst95CL = NULL; }
    
    if (c.fTH1DLatency_Distance95CL) { fTH1DLatency_Distance95CL = (TH1D*)c.fTH1DLatency_Distance95CL->Clone(); }
    else                             { fTH1DLatency_Distance95CL = NULL; }

    if (c.fDistanceParametrisation) { fDistanceParametrisation = (TF1*)c.fDistanceParametrisation->Clone(); }
    else                      { fDistanceParametrisation = NULL; }
    
    if (c.fDistanceProbability) { fDistanceProbability = (TH1D*)c.fDistanceProbability->Clone(); }
    else                         { fDistanceProbability = NULL; }
  }

  Configuration():
    fName                    (""),
    fLegendEntry             (""),
    fNClusterCut             (0),
    fClusterAlgorithm        (0),
    fBackgroundRate          (0),
    fBackgroundRateError     (0),    
    fBurstTimeWindow         (0),
    fClusterEfficiency       (0),
    fClusterEfficiencyError  (0),
    fTargetFakeRate          (0),
    fActualFakeRate          (0),
    fFakeRate_Cut            (),
    fEfficiency_Burst        (),
    fEfficiency_Distance     (),
    fCoverage                (),
    fLatency_Burst           (),
    fLatency_Distance        (),
    fLatency_Burst95CL       (),
    fLatency_Distance95CL    (),
    fTH1DFakeRate_Cut        (NULL),
    fTH1DEfficiency_Burst    (NULL),
    fTH1DEfficiency_Distance (NULL),
    fTH1DCoverage            (NULL),
    fTH1DLatency_Burst       (NULL),
    fTH1DLatency_Distance    (NULL),
    fDistanceParametrisation (NULL),
    fDistanceProbability     (NULL),
    fTH1DLatency_Burst95CL   (NULL),
    fTH1DLatency_Distance95CL(NULL),
    fTH1DList               (){
  };
  
  void FillHistograms(const std::map<double,double> input,
                      TH1D*& histo,
                      const std::string Title,
                      const std::string XTitle,
                      const std::string YTitle){

    if (input.size() == 0) return;
    if (histo != NULL) return;

    std::cout << "Configuration::FillHistograms(): Filling histogram " << Title << std::endl;
    std::vector<double> bins;
    bins.push_back(0);
    for (std::map<double,double>::const_iterator it=input.cbegin();
         it != input.end(); ++it) {
      std::map<double,double>::const_iterator next=std::next(it,1);
      if (next==input.end()) break;
      bins.push_back(0.5*(it->first + next->first));
    }
    bins.push_back(input.crbegin()->first);

    histo = new TH1D(Title.c_str(),
                     (";"+XTitle+";"+YTitle).c_str(),
                     input.size(), bins.data());
    histo->SetStats(0);
    for (auto const& it:input) {
      if (it.second>=0)
        histo->SetBinContent(histo->FindBin(it.first), it.second);
    }
    fTH1DList.push_back(histo);
  };

  
  void ConvertToDistance(const TH1D* Input,
                         TH1D*& Output) {

    if (Input == NULL) return;
    if (fDistanceParametrisation == NULL) return;
    if (Output) {
      delete Output;
      Output=NULL;
    }

    std::cout << "Configuration::ConvertToDistance(): Converting histogram " << Input->GetName() << std::endl;

    std::vector<double> old_binning;
    const TArrayD* array = Input->GetXaxis()->GetXbins();
    for (size_t i=0; i<array->GetSize(); ++i) {
      if (array->At(i)>=1) old_binning.push_back(array->At(i));
    }

    std::vector<double> new_binning;
    for (auto const& it: old_binning) {
      new_binning.push_back(fDistanceParametrisation->Eval(it));
    }

    std::sort(new_binning.begin(), new_binning.end());

    std::string name = (std::string)Input->GetName()+"Distance";

    Output = new TH1D(name.c_str(), ((std::string)";Distance [kpc];"+Input->GetYaxis()->GetTitle()).c_str(),
                      new_binning.size()-1, new_binning.data());
    Output->SetStats(0);
    for (int i=0; i<Input->GetXaxis()->GetNbins(); ++i) {
      double distance = fDistanceParametrisation->Eval(Input->GetBinCenter(i));
      Output->SetBinContent(Output->FindBin(distance), Input->GetBinContent(i));
    }
    fTH1DList.push_back(Output);

  };

  
  void ConvertToCoverage(const TH1D* Input,
                         TH1D*& Output) {

    if (Input == NULL) return;
    if (fDistanceProbability == NULL) return;

    std::cout << "Configuration::ConvertToCoverage(): Converting histogram " << Input->GetName() << std::endl;
    if (Output) {
      delete Output;
      Output=NULL;
    }
    Output = (TH1D*)Input->Clone();
    Output->SetTitle(";Distance [kpc];Triggering efficiency #times SN probability");
    
    for (int i=0; i<Output->GetXaxis()->GetNbins(); ++i) {
      Output->SetBinContent(i, Output->GetBinContent(i) * fDistanceProbability->GetBinContent(fDistanceProbability->FindBin(Output->GetBinCenter(i))));
    }
    fTH1DList.push_back(Output);

  };
    
  void FillHistograms() {
    FillHistograms(fFakeRate_Cut, fTH1DFakeRate_Cut,
                   "FakeRate_Cut", "N clusters cut", "Fake rate [Hz]");
    FillHistograms(fEfficiency_Burst, fTH1DEfficiency_Burst,
                   "Efficiency_Burst", "N SN interactions / 10 kT", "Triggering efficiency");
    FillHistograms(fLatency_Burst, fTH1DLatency_Burst,
                   "Latency_Burst", "N SN interactions / 10 kT", "Latency [ms]");
    FillHistograms(fLatency_Burst95CL, fTH1DLatency_Burst95CL,
                   "Latency_Burst95CL", "N SN interactions / 10 kT", "Upper limit on the Latency [ms] @ 95\%CL");
    ConvertToDistance(fTH1DEfficiency_Burst   , fTH1DEfficiency_Distance );
    ConvertToDistance(fTH1DLatency_Burst      , fTH1DLatency_Distance    );
    ConvertToDistance(fTH1DLatency_Burst95CL  , fTH1DLatency_Distance95CL);
    ConvertToCoverage(fTH1DEfficiency_Distance, fTH1DCoverage            );
    std::cout << "Configuration::FillHistograms(): Finished filling histograms" << std::endl;
  };

  void DumpAndPlot(std::string Name="Config.pdf") {
    std::cout << " - - - - - - - - - - - - - -" << std::endl;
    std::cout << " - Name                   : " << fName                   << std::endl;
    std::cout << " - LegendEntry            : " << fLegendEntry            << std::endl;
    std::cout << " - NClusterCut            : " << fNClusterCut            << std::endl;
    std::cout << " - ClusterAlgorithm       : " << fClusterAlgorithm       << std::endl;
    std::cout << " - BackgroundRate         : " << fBackgroundRate         << std::endl;
    std::cout << " - BackgroundRateError    : " << fBackgroundRateError    << std::endl;
    std::cout << " - BurstTimeWindow        : " << fBurstTimeWindow        << std::endl;
    std::cout << " - FractionInTimeWindow   : " << fFractionInTimeWindow   << std::endl;
    std::cout << " - ClusterEfficiency      : " << fClusterEfficiency      << std::endl;
    std::cout << " - ClusterEfficiencyError : " << fClusterEfficiencyError << std::endl;
    std::cout << " - TargetFakeRate         : " << fTargetFakeRate         << std::endl;
    std::cout << " - ActualFakeRate         : " << fActualFakeRate         << std::endl;
    
    std::cout << "Now Plotting all available histos." << std::endl;
    TCanvas c;
    c.Print((Name+"[").c_str());
    for (auto const& it: fTH1DList) {
      if (it == fTH1DFakeRate_Cut || it == fTH1DEfficiency_Burst || it == fTH1DEfficiency_Distance) {
        gPad->SetLogy(true);
      }

      if (it == fTH1DEfficiency_Burst || it == fTH1DEfficiency_Distance) {
        gPad->SetLogx(true);
      }
      
      if (it) {
        it->Draw();
        c.Print(Name.c_str());
      }
      gPad->SetLogy(false);
      gPad->SetLogx(false);
    }
    
    if (fDistanceParametrisation) {
      fDistanceParametrisation->Draw();
      c.Print(Name.c_str());
    }
    if (fDistanceProbability) {    
      fDistanceProbability->Draw();
      c.Print(Name.c_str());
    }
    c.Print((Name+"]").c_str());
  }
  
  ~Configuration() {
    fFakeRate_Cut        .clear();
    fEfficiency_Distance .clear();
    fCoverage            .clear();
    fLatency_Burst       .clear();
    fLatency_Distance    .clear();
    fLatency_Burst95CL   .clear();
    fLatency_Distance95CL.clear();
    for (auto& it: fTH1DList) {
      if (it) delete it;
      it = NULL;
    }
  };
  ClassDef(Configuration, 2);

};


#endif
