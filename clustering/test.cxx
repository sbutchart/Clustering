#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <math.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TF1.h>
#include <TMath.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TObjArray.h>
#include <TArrow.h>
#include <TGraph2D.h>

int main(){

  TFile f("/dune/data/users/plasorak/anahist2.root", "READ");
  TTree *t = (TTree*) f.Get("snanagaushit/SNSimTree");

  std::vector<int> * True_Bck_PDG = NULL;
  t->SetBranchAddress("True_Bck_PDG", &True_Bck_PDG);

  for (int i=0; i<t->GetEntries(); ++i) {

    t->GetEntry(i);
    
    for (int j=0; j<True_Bck_PDG->size(); ++j){
      std::cout << (*True_Bck_PDG)[j] << std::endl;
    }



  }
  
  return 0;
}
