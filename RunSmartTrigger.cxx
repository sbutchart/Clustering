#include <unistd.h>
#include <iostream>

#include "THn.h"
#include "TCanvas.h"

#include "SmartTrigger.hh"



int main(int argc, char** argv) {

  int opt;
  int Config, Energy=-1;
  double nNeutrino=10;
  double TimeWindow=10;
  bool useADC  = true;
  bool useTime = false;
  bool useChan = false;
  bool useEner = false;
  std::string InputFile = "";
  std::string OutputFile = "";
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;
  // Retrieve the options:
  while ( (opt = getopt(argc, argv, "c:e:t:f:n:i:o:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'c':
      std::cout <<optarg<<std::endl;
      Config = atoi(optarg);
      break;
    case 'e':
      Energy = atoi(optarg);
      break;
    case 't':
      TimeWindow = atof(optarg);
      break;
    case 'i':
      InputFile = optarg;
      break;
    case 'o':
      OutputFile = optarg;
      break;
    case 'n':
      nNeutrino = atof(optarg);
      break;
    case 'f':
      useADC  = (std::string(optarg).find("a") != std::string::npos);
      useTime = (std::string(optarg).find("t") != std::string::npos);
      useChan = (std::string(optarg).find("c") != std::string::npos);
      useEner = (std::string(optarg).find("e") != std::string::npos);
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
  if (InputFile == "") {
    std::cout << "No input file!!" << std::endl;
    exit(1);
  }
  std::cout << "Using file " << InputFile << std::endl;
  
  if (Config<0 || Config>5) {
    std::cout << "Invalid config " << Config << std::endl;
    exit(1);
  }
  if (Energy<-1 || Energy>3) {
    std::cout << "Invalid Energy " << Energy << std::endl;
    exit(1);
  }
  std::vector<std::string> feature = {"SumADC"};
  SmartTrigger strig(feature);
  strig.SetConfig(Config);
  std::vector<double> bin = {0,400,600,800,1000,1200,1400,1600,1800,2000,2500,30000};
  strig.SetBinning("SumADC",bin);
  strig.ConstructLikelihood(InputFile, OutputFile);
  THnD* Likelihood_Sign = strig.GetLikelihood_Signal    ();
  THnD* Likelihood_Back = strig.GetLikelihood_Background();
  
  
}



