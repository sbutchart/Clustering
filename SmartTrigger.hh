#ifndef SMARTTRIGGER_HH
#define SMARTTRIGGER_HH
#include <algorithm>
#include <iostream>

#include "THn.h"
#include "TAxis.h"
#include "TFile.h"
#include "TTree.h"


class SmartTrigger {
public:
  SmartTrigger(const std::vector<std::string>);
  ~SmartTrigger(){
    delete fLikelihood_Sign;
    delete fLikelihood_Back;
    fLikelihood_Sign = NULL;
    fLikelihood_Back = NULL;
  };

  void ConstructLikelihood(const std::string, const std::string);
  void SetBinning(const std::string,const std::vector<double>);

  void SetConfig(const int c=-1) { fConfig = c; };
  int  GetConfig() const { return fConfig; };
  
  THnD* GetLikelihood_Signal    () const { return fLikelihood_Sign; };
  THnD* GetLikelihood_Background() const { return fLikelihood_Back; };


private:
  THnD* fLikelihood_Sign;
  THnD* fLikelihood_Back;
  std::vector<std::string> fFeature;
  int fConfig;
  
  
};

#endif
