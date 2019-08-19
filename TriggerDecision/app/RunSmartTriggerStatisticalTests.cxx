//____________________________________________________________________ 
//  
// $Id$ 
// Author: Pierre Lasorak <plasorak@Pierres-MacBook-Pro.local>
// Update: 2019-02-14 15:38:26+0000
// Copyright: 2019 (C) Pierre Lasorak
//
//

#include "TriggerDecision/StatisticalTest.hh"

#include "Utils/Helper.h"
#include "Utils/CLI11.hpp"

#include "TCanvas.h"
#include "TColor.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TLegend.h"
#include <TMath.h>
#include "TPad.h"
#include "TRandom3.h"
#include "TText.h"
#include "TVectorT.h"

#include <chrono>
#include <iostream>
#include <vector>


ToyThrower()
  CurrentProg=0;
  for (int iToy=0; iToy<nToy; ++iToy) {
    PrintProgress(iToy, nToy);
    
    TH1D* background_trial = (TH1D*)background->Clone();
    TH1D* signal_trial     = (TH1D*)signal    ->Clone();
    
    background_trial->Reset();
    signal_trial    ->Reset();
    background_trial->SetName("BACKGROUND");
    signal_trial    ->SetName("BACKGROUND+SIGNAL");
    int nBackground          = rand.Poisson(background_rate * 10); // Integrate over 10 seconds
    int nSignalAndBackground = rand.Poisson(background_rate * 10) + rand.Poisson(efficiency * nEvent);
// number of events at the LMC
    
    for (int iBackground=0; iBackground<nBackground; ++iBackground) {
      background_trial->Fill(background->GetRandom());
    }
    
    for (int iSignalAndBackground=0; iSignalAndBackground<nSignalAndBackground; ++iSignalAndBackground) {
      signal_trial->Fill(signal->GetRandom());
    }
    LikelihoodStat     .first .push_back(GetLikelihoodStat     (background_trial));
    LikelihoodStat     .second.push_back(GetLikelihoodStat     (signal_trial    ));

    LikelihoodStatNorm .first .push_back(GetLikelihoodStatNorm (background_trial));
    LikelihoodStatNorm .second.push_back(GetLikelihoodStatNorm (signal_trial    ));

    LikelihoodStatShape.first .push_back(GetLikelihoodStatShape(background_trial));
    LikelihoodStatShape.second.push_back(GetLikelihoodStatShape(signa_trial     ));

    delete background_trial;
    delete signal_trial;
  }


void BuildTriggerOnConfig(int config, std::string InputFileName, std::string OutputFileName,
                          int nToy, int nThread, int nEvent) {


  TFile* InputFile = new TFile(InputFileName.c_str(), "READ");
  TH1D* signal     = (TH1D*)InputFile->Get(Form("PDF_Background_1_config%i",config));
  TH1D* background = (TH1D*)InputFile->Get(Form("PDF_Background_config%i",config));
  TVectorD* efficiencies = (TVectorD*)InputFile->Get("Efficiencies");
  signal    ->SetDefaultBufferSize(0);
  background->SetDefaultBufferSize(0);
  signal    ->BufferEmpty(0);
  background->BufferEmpty(0);

  background->Scale(1./0.12); //forgot the detector scaling here....
  double efficiency = (*efficiencies)[config];
  double background_rate = background->Integral();
  background->Scale(10); // integrate over 10 seconds
  signal->Scale(nEvent * efficiency / signal->Integral()); // Number of events at the LMC
  signal->Add(background);

  TRandom3 rand;

  pair<vector<double>,vector<double>> LikelihoodStat;
  pair<vector<double>,vector<double>> LikelihoodStatNorm;
  pair<vector<double>,vector<double>> LikelihoodStatShape;


}


int main(int argc, char** argv) {

  CLI::App app{"SmartTrigger"};
  string InputFileName = "";
  string OutputFileName = "";
  
  int nConfig, nToy, nThread, nEvent;
  app.add_option("--ntoy",    nToy,           "Number of toys to throw"  )->required();
  app.add_option("--nthread", nThread,        "Number of threads to use" );
  app.add_option("--nevent",  nEvent,         "Number of event in the SN")->required();
  app.add_option("--config",  nConfig,        "Which config to run on"   )->required();
  app.add_option("--output",  OutputFileName, "Output file name (root)"  )->required();
  app.add_option("--input",   InputFileName,  "Input file nam (root)"    )->required();

  CLI11_PARSE(app, argc, argv);
  
  BuildTriggerOnConfig(nConfig, InputFileName, OutputFileName, nToy, nThread, nEvent);
  
  return 0;
}

//____________________________________________________________________ 
//  
// EOF
//
//____________________________________________________________________ 
//
