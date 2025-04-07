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

using namespace std;

int main(int argc, char** argv) {
  CLI::App app{"SmartTrigger"};

  string InputChainName = "triggerAnaDumpAll/TriggerActivities/tamakerTPC__TriggerAnaTree1x2x6";
  string InputTrueChainName = "triggerAnaDumpAll/mctruths";
  string Feature = "adc_integral";
  string InputSignalFile = "";
  string InputBackgroundFile = "";
  string OutputFileName = "LikelihoodTrigger.root";
  string type = "s";

  app.add_option("-s,--signal",  InputSignalFile, "Input signal file list")->required();
  app.add_option("-b,--background", InputBackgroundFile, "Input background file list")->required();
  app.add_option("-o,--output", OutputFileName, "Output file name");
  //app.add_option("-t,--type", type, "Signal (s) or Background (b)")->required();
  CLI11_PARSE(app, argc, argv);

  unique_ptr<TChain> InputSignalChain         = make_unique<TChain>(InputChainName    .c_str());
  unique_ptr<TChain> InputTrueSignalChain     = make_unique<TChain>(InputTrueChainName.c_str());

  unique_ptr<TChain> InputBackgroundChain     = make_unique<TChain>(InputChainName    .c_str());
  unique_ptr<TChain> InputTrueBackgroundChain = make_unique<TChain>(InputTrueChainName.c_str());
  
  string sline;
  ifstream mysfile (InputSignalFile);
  if (mysfile.is_open()){
    cout << "Parsing list of files...\n";
    while ( getline (mysfile,sline) ){
      InputSignalChain->Add(sline.c_str());
      InputTrueSignalChain->Add(sline.c_str());
    }
  } else {
    cerr << "File " << InputSignalFile << " doesnt exist.\n";
    throw;
  }

  string bline;
  ifstream mybfile (InputBackgroundFile);
  if (mybfile.is_open()){
    cout << "Parsing list of files...\n";
    while ( getline (mybfile,bline) ){
      InputBackgroundChain->Add(bline.c_str());
      InputTrueBackgroundChain->Add(bline.c_str());
    }
  } else {
    cerr << "File " << InputBackgroundFile << " doesnt exist.\n";
    throw;
  }

  std::cout << "Parsed list." << std::endl;
  
  //map<int,shared_ptr<TH1D>> PDF_Background_notype;
  shared_ptr<TH1D> PDF_Background_notype;
  PDF_Background_notype = make_shared<TH1D>("PDF_Background", "PDF;SADC;", 20, 0, 20000);

  shared_ptr<TH1D> PDF_Signal;
  PDF_Signal = make_shared<TH1D>("PDF_Signal", "PDF;SADC;", 20, 0, 20000);

  //map<TFile*,map<int,shared_ptr<TH1D>>> PDF_Background_perfile_notype;

  ULong_t SumADCS=0;
  ULong_t SumADCB=0;
  int Event_true = 0;
  
  //InputChain->SetBranchAddress(Feature.c_str(), &SumADC);
  InputSignalChain->SetBranchAddress("adc_integral", &SumADCS);
  InputBackgroundChain->SetBranchAddress("adc_integral", &SumADCB);
  //std::cout << "Set ADC integral branch" << std::endl;

  //InputTrueChain->SetBranchAddress("en",       &ENu       );
  InputSignalChain->SetBranchAddress("Event",    &Event_true);
  //std::cout << "Set true event branch" << std::endl;

  int nEventsGeneratedS = InputTrueSignalChain->GetMaximum("Event");
  std::cout << "Number of signal events: " << nEventsGeneratedS << std::endl;

  int nEventsGeneratedB = InputTrueBackgroundChain->GetMaximum("Event");
  std::cout << "Number of background events: " << nEventsGeneratedB << std::endl;

  std::cout << "GetEntries: " << std::endl;
  int nEntriesS = InputSignalChain->GetEntries("adc_integral");
  std::cout << "nEntriesS: " << nEntriesS << std::endl;

  int nEntriesB = InputBackgroundChain->GetEntries("adc_integral");
  std::cout << "nEntriesB: " << nEntriesB << std::endl;

  std::cout << "Create and open output file:" << std::endl;
  unique_ptr<TFile> OutputFile = make_unique<TFile>(OutputFileName.c_str(), "RECREATE");
  OutputFile->cd();

  //std::cout << "GetMaximum: " << std::endl;
  //int nEntries = InputChain->GetEntries();
  //int nEntries = 500;
  
  std::cout << "GetEntry loop..." << std::endl;
  for (int iEntry=0; iEntry<nEntriesS; ++iEntry) {
    InputSignalChain->GetEntry(iEntry);
    //PrintProgress(iEntry,nEntries);
    PDF_Signal->Fill(SumADCS);
   }

  for (int iEntry=0; iEntry<nEntriesB; ++iEntry) {
    InputBackgroundChain->GetEntry(iEntry);
    PDF_Background_notype->Fill(SumADCB);
  }

  OutputFile->cd();

  std::cout << "Scaling." << std::endl;

  double scaleBackground = 1. / 2.2e-3 / nEventsGeneratedS / 0.12;
  double scaleSignal = 1.;
 
  PDF_Background_notype->Scale(scaleBackground);
  PDF_Background_notype->Write();

  PDF_Signal->Scale(scaleSignal);
  PDF_Signal->Write();

      OutputFile->Close();
}


