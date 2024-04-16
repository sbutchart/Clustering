//____________________________________________________________________ 
//  
// $Id$ 
// Author: Pierre Lasorak <plasorak@Pierres-MacBook-Pro.local>
// Update: 2019-02-14 15:38:26+0000
// Copyright: 2019 (C) Pierre Lasorak
//
//

#include "TriggerDecision/StatisticalTest.hh"
#include "TriggerDecision/TriggerLatencyCalculator.hh"

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



int main(int argc, char** argv) {

  CLI::App app{"SmartTrigger"};
  std::string InputFileName = "";
  std::string OutputFileName = "";
  std::string Method = "Likelihood";
  
  int Config, nToy, nThread=1, nEvent;
  app.add_option("--ntoy",    nToy,           "Number of toys to throw"  )->required();
  app.add_option("--nthread", nThread,        "Number of threads to use" );
  app.add_option("--nevent",  nEvent,         "Number of event in the SN")->required();
  app.add_option("--config",  Config,         "Which config to run on"   )->required();
  app.add_option("--output",  OutputFileName, "Output file name (root)"  )->required();
  app.add_option("--input",   InputFileName,  "Input file nam (root)"    )->required();
  app.add_option("--method",  Method,         "Method can be \"Likelihood\", \"LikelihoodNorm\" or \"LikelihoodShape\"")->required();

  CLI11_PARSE(app, argc, argv);
  TriggerLatencyCalculator tlc(nEvent, 10, nToy, nThread, Config, InputFileName, OutputFileName, Method);
  tlc.Compute();
  
  return 0;
}

//____________________________________________________________________ 
//  
// EOF
//
//____________________________________________________________________ 
//
