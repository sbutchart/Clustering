#include <iostream>
#include <map>
#include <memory>

#include "TH1D.h"
#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TVectorT.h"
#include "TLeaf.h"

#include "Utils/CLI11.hpp"
#include "Utils/Helper.h"

using namespace std;

int main(int argc, char** argv) {
  CLI::App app{"SmartTrigger"};

  string InputChainName = "triggerAnaDumpAll/TriggerActivities/tamakerTPC__TriggerAnaTree1x2x6";
  string InputTrueChainName = "triggerAnaDumpAll/mctruths";
  string Feature = "adc_integral";
  string InputFile = "";
  string OutputFileName = "LikelihoodTrigger.root";
  string type = "s";

  app.add_option("-l,--list",  InputFile, "Input file list")->required();
  app.add_option("-o,--output", OutputFileName, "Output file name");
  app.add_option("-t,--type", type, "Signal (s) or Background (b)")->required();
  CLI11_PARSE(app, argc, argv);

  unique_ptr<TChain> InputChain     = make_unique<TChain>(InputChainName    .c_str());
  unique_ptr<TChain> InputTrueChain = make_unique<TChain>(InputTrueChainName.c_str());
  
  string line;
  ifstream myfile (InputFile);
  if (myfile.is_open()){
    cout << "Parsing list of files...\n";
    while ( getline (myfile,line) ){
      InputChain->Add(line.c_str());
      InputTrueChain->Add(line.c_str());
    }
  } else {
    cerr << "File " << InputFile << " doesnt exist.\n";
    throw;
  }

  std::cout << "Parsed list." << std::endl;
  
  //map<int,shared_ptr<TH1D>> PDF_Background_notype;
  shared_ptr<TH1D> PDF_Background_notype;
  PDF_Background_notype = make_shared<TH1D>("PDF_Background", "PDF;SADC;", 20, 0, 200);

  //map<TFile*,map<int,shared_ptr<TH1D>>> PDF_Background_perfile_notype;

  double SumADC=0;
  int Event_true = 0;
  
  InputChain->SetBranchAddress(Feature.c_str(), &SumADC);
  //std::cout << "Set ADC integral branch" << std::endl;

  //InputTrueChain->SetBranchAddress("en",       &ENu       );
  InputTrueChain->SetBranchAddress("Event",    &Event_true);
  //std::cout << "Set true event branch" << std::endl;

  int nEventsGenerated=InputTrueChain->GetMaximum("Event");
  std::cout << "nEventsGenerated: " << nEventsGenerated << std::endl;


  std::cout << "Create and open output file:" << std::endl;
  unique_ptr<TFile> OutputFile = make_unique<TFile>(OutputFileName.c_str(), "RECREATE");
  OutputFile->cd();

  std::cout << "GetMaximum: " << std::endl;
  int nEntries = InputChain->GetMaximum("adc_integral");
  //int nEntries = 500;
  
  std::cout << "GetEntry loop..." << std::endl;
  for (int iEntry=0; iEntry<nEntries; ++iEntry) {
    InputChain->GetEntry(iEntry);
    PrintProgress(iEntry,nEntries);

    if (type == "s") {
    } else {
      PDF_Background_notype->Fill(SumADC/100.);
       }
    }

  OutputFile->cd();

  std::cout << "Scaling." << std::endl;

  double scaleBackground = 1. / 2.2e-3 / nEventsGenerated / 0.12;

  if (type == "b") {
    PDF_Background_notype->Scale(scaleBackground);
    PDF_Background_notype->Write();

  }

      OutputFile->Close();
}


