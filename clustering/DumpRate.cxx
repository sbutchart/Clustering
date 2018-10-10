#include <iostream>
#include <fstream>
#include <unistd.h>

#include "TCanvas.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TLegend.h"
#include "TPad.h"
#include "TTree.h"

using namespace std;

int main(int argc, char** argv){
  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;
  std::string InputFile;
  std::string InputTree;
  std::string OutputFile;

  while ( (opt = getopt(argc, argv, "i:o:t:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'i':
      InputFile = optarg;
      break;
    case 't':
      InputTree = optarg;
      break;
    case 'o':
      OutputFile = optarg;
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
  
  TFile* file_weights = new TFile(InputFile.c_str(), "READ");
  TTree* tree = (TTree*)file_weights->Get(InputTree.c_str());
  std::cout << "The original tree had " <<tree->GetEntries() << " entries. " << std::endl;
  TH1D* rate_tot_sn_th1 = new TH1D("rate_tot_sn_th1", ";E_{#nu} [MeV];SN#nu PDF", 100, 3, 20);
  tree->Project("rate_tot_sn_th1", "True_ENu*1000.");
  rate_tot_sn_th1->SetStats(0);
  rate_tot_sn_th1->Scale(1. / rate_tot_sn_th1->Integral(0,99));
  rate_tot_sn_th1->Draw();

  TFile* output_weight_file = new TFile(OutputFile.c_str(), "RECREATE");
  rate_tot_sn_th1->Write("SN_ENU_PDF");
  output_weight_file->Close();

  return 0;
}
