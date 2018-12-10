#ifndef RADIOLOGICALCHEATGENERATOR_HH
#define RADIOLOGICALCHEATGENERATOR_HH 1

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>

#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TRandom3.h"

struct Part{
  double x,y,z,t;
  double px,py,pz,p,E;
  int pdg;
  double M;
  Part():
    x(-999),
    y(-999),
    z(-999),
    t(-999),
    px(-999),
    py(-999),
    pz(-999),
    p(-999),
    E(-999),
    pdg(-999),
    M(-999){};
};

enum RadType{
  kAr39=0,
  kK40  ,
  kNi59 ,
  kCo60 ,
  kKr85 ,
  kAr42 ,
  kRn222,
  kTh232,
  kU238 ,
  kNeutron
};

enum DecayType{
  kAlpha    = 0,
  kBeta     = 1,
  kGamma    = 2,
  kNeutronD = 3
};

struct Decay{
  DecayType Type;
  TH1D* Spectrum;
  double BranchingRatio;
  std::vector<double> GammaEnergies;
  Decay():
    Type(kBeta),
    Spectrum(NULL),
    BranchingRatio(1),
    GammaEnergies(){
    GammaEnergies.clear();
  };
  ~Decay(){
    GammaEnergies.clear();
  }
};

class RadiologicalCheatGenerator {
private:
  size_t fNEvent;
  std::string fOutputFileName;
  std::string fOutputFCLFileName;
  RadType fRequestedBackgroundType;
  std::ofstream fFile;
  std::ofstream fFCLFile;
  size_t fIter;
  std::map<std::string, RadType> fConvertor;
  TRandom3 rand;
  std::map<RadType, std::vector<Decay> > fDecays;
  std::vector<Part> fParticles;
  
public:
  void SetNEvent(const size_t e=1000) { fNEvent = e; };
  void SetOutputFile(const std::string f="out.txt") { fOutputFileName=f; };
  void SetOutputFCLFile(const std::string f="out.fcl") { fOutputFCLFileName=f; };
  void SetBackgroundType(const std::string f="Ar39") {
    if (fConvertor.find(f) == fConvertor.end()) {
      std::cout << "Background type requested not in the list (" << f << ")" << std::endl;
      std::cout << "Possible backgrounds are:" << std::endl;
      for (auto const& it: fConvertor)
        std::cout << it.first << std::endl;
      exit(1);
    }
    fRequestedBackgroundType = fConvertor.at(f);
  };
  
  RadiologicalCheatGenerator();
  std::string CreateFCLFile(std::string);
  int Run();
  void GenerateAr39   ();
  void GenerateK40    ();
  void GenerateAr42   ();
  void GenerateNi59   ();
  void GenerateCo60   ();
  void GenerateKr85   ();
  void GenerateRn222  ();
  void GenerateTh232  ();
  void GeneratePo210  ();
  void GenerateU238   ();
  void GenerateNeutron();
  
  void PrintParticle(const Part& p);
  void FillParticlePositionInArgon        (Part& p);
  void FillParticlePositionOnPDS          (Part& p);
  void FillParticlePositionOnAPA          (Part& p);
  void FillParticlePositionOnCPA          (Part& p);
  void FillParticlePoistionFromSurrounding(Part& p);

  void FillParticleIsotropicDirection(Part& p);
  std::vector<TH1D*> GetSpectrum(std::string filename){
    std::string dir="";
    try{
      dir = std::string(std::getenv("DECAYS"));
    } catch (...) {
      std::cout << "you must source SourceMe.sh from the source before starting." << std::endl;
      exit(1);
    }
    if (dir == "") {
      std::cout << "you must source SourceMe.sh from the source before starting." << std::endl;
      exit(1);
    }
    std::cout <<"Opening file "<< dir+"/"+filename << std::endl;
    TFile* file = new TFile((dir+"/"+filename).c_str(), "READ");

    
    TGraph* beta = (TGraph*)file->Get("Betas");
    std::vector<TH1D*> Spectrum;
    if (beta) {
      double x, y;
      beta->GetPoint(beta->GetN()-1, x, y);
      TH1D* s = new TH1D((filename+"_beta").c_str(), "", 3*beta->GetN(), 0, x);
      for (int i=0; i<s->GetXaxis()->GetNbins(); ++i) {
        s->SetBinContent(i, beta->Eval(s->GetBinCenter(i)));
      }
      TCanvas c;
      s->Draw();
      Spectrum.push_back(s);
      c.SaveAs((filename+"_beta_Spectrum.png").c_str());
    }

    TGraph* gamma = (TGraph*)file->Get("Gammas");
    if (gamma) {
      double x, y;
      gamma->GetPoint(gamma->GetN()-1, x, y);
      TH1D* s = new TH1D((filename+"_gamma").c_str(), "", 3*gamma->GetN(), 0, x);
      for (int i=0; i<s->GetXaxis()->GetNbins(); ++i) {
        s->SetBinContent(i, gamma->Eval(s->GetBinCenter(i)));
      }
      TCanvas c;
      s->Draw();
      Spectrum.push_back(s);
      c.SaveAs((filename+"_gamma_Spectrum.png").c_str());
    }

    TGraph* alpha = (TGraph*)file->Get("Alphas");
    if (alpha) {
      double x, y;
      alpha->GetPoint(alpha->GetN()-1, x, y);
      TH1D* s = new TH1D((filename+"_alphas").c_str(), "", 3*alpha->GetN(), 0, x);
      for (int i=0; i<s->GetXaxis()->GetNbins(); ++i) {
        s->SetBinContent(i, alpha->Eval(s->GetBinCenter(i)));
      }
      TCanvas c;
      s->Draw();
      Spectrum.push_back(s);
      c.SaveAs((filename+"_alpha_Spectrum.png").c_str());
    }

    TGraph* neutron = (TGraph*)file->Get("Neutrons");
    if (neutron) {
      double x, y;
      neutron->GetPoint(neutron->GetN()-1, x, y);
      TH1D* s = new TH1D((filename+"_neutrons").c_str(), "", 3*neutron->GetN(), 0, x);
      for (int i=0; i<s->GetXaxis()->GetNbins(); ++i) {
        s->SetBinContent(i, neutron->Eval(s->GetBinCenter(i)));
      }
      TCanvas c;
      s->Draw();
      Spectrum.push_back(s);
      c.SaveAs((filename+"_neutron_Spectrum.png").c_str());
    }
    
    return Spectrum;
  }
  
};

#endif
