#include "ArbitraryAnaInputManager.hh"
#include "ClusterEngine.hh"
#include "Trigger.hh"

#include "Helper.h"

#include "TCanvas.h"
#include "TEfficiency.h"
#include "TFile.h"
#include "TPad.h"
#include "TH2D.h"
#include "TF1.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TMath.h"

#include <iostream>
#include <unistd.h>

TF1* f1, *f2, *f3;

double finter(double *x, double*par) {
  return TMath::Abs(f1->EvalPar(x,par) - f2->EvalPar(x,par));
}

int main(int argc, char** argv) {

  int opt;

  extern char *optarg;
  
  std::string InFileName  = "";
  std::string OutFileName = "";

  while ( (opt = getopt(argc, argv, "i:o:")) != -1 ) {
    switch (opt) {
    case 'i':
      InFileName = optarg;
      break;
    case 'o':
      OutFileName = optarg;
      break;
    }
  }
  
  TFile f(InFileName.c_str(), "READ");
  
  // TEfficiency *R1 = (TEfficiency*) f.Get("Region1");   
  // TEfficiency *R2 = (TEfficiency*) f.Get("Region2");
  // TEfficiency *R3 = (TEfficiency*) f.Get("Region3");
  // TEfficiency *R4 = (TEfficiency*) f.Get("Region4");
  TEfficiency *R5 = (TEfficiency*) f.Get("Region5");
  // TEfficiency *R6 = (TEfficiency*) f.Get("Region6");
  // TEfficiency *R7 = (TEfficiency*) f.Get("Region7");
  // TEfficiency *R8 = (TEfficiency*) f.Get("Region8");


  f1 = new TF1("f1","(1+[0]*TMath::Erf((x-[1])/[2]))/2.0",-0.5,25);
  // f1 = new TF1("f1","(1+TMath::Erf((x-[0])/[1]))/2.", 0, 10);
  f1->SetParameters(1.,0.0, 0.05);
  R5->Fit(f1);
  f2 = new TF1("f2", "0.9");
  TF1* fint = new TF1("fint",finter,0,30,0);
  double threshold = fint->GetMinimumX();
  std::cout << threshold << std::endl;

  
  TCanvas c;
  c.Print("Balls.pdf[");
  R5->Draw();
  f1->Draw("SAME");
  c.Print("Balls.pdf");
  c.Print("Balls.pdf]");


  
}
