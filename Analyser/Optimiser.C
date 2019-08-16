//____________________________________________________________________ 
//  
// $Id$ 
// Author: Pierre Lasorak <plasorak@Pierres-MacBook-Pro.local>
// Update: 2019-02-14 15:38:26+0000
// Copyright: 2019 (C) Pierre Lasorak
//
//
#ifndef __CINT__
#include "TCanvas.h"
#include "TH1D.h"
#include "TFile.h"
#endif

TCanvas c;

using namespace std;

void AnalyseConfig(int config, TFile* file) {

  TH1D* signal = (TH1D*)file->Get(Form("PDF_Background_1_config%i",config));
  TH1D* background = (TH1D*)file->Get(Form("PDF_Background_5_config%i",config));
  TVectorD* efficiency = (TVectorD*)file->Get("Efficiencies");

  double background_rate = background->Integral();
  signal->Scale(1./signal->Integral());
  background->Scale(1./background_rate);

  signal->SetLineColor(kRed);
  background->SetLineColor(kBlue);

  TLegend* leg = new TLegend(0.7, 0.8, 0.9, 0.9);
  leg->AddEntry(signal, "Signal");
  leg->AddEntry(background, "Background");
  vector<double> maxes = {background->GetMaximum(), signal->GetMaximum()};
  double max = (*max_element(maxes.begin(), maxes.end()))*1.2;
  
  TText text_eff (80, 0.8*max, Form("Individual SN event efficiency = %.f%%", (*efficiency)[config]*100));
  TText text_back(80, 0.7*max, Form("Background rate = %.2f Hz", background_rate));
  text_eff.SetTextSize(0.04);
  text_back.SetTextSize(0.04);
  signal->SetStats(0);
  signal->SetMaximum(max);
  signal->Draw();
  background->Draw("SAME");
  text_eff.Draw();
  text_back.Draw();
  leg->Draw();
  c.Print("Likelihood.pdf");

}




int Optimiser() {
  TFile* f = new TFile("~/Documents/Sussex-Postdoc/Physics/LikelihoodTrigger.root", "READ");
  c.Print("Likelihood.pdf[");
  for (int i=0; i<48; ++i) {
    AnalyseConfig(i, f);
  }

  c.Print("Likelihood.pdf]");

  return 0;
}

#ifndef __CINT__
int main(int argc, char** argv)
{
  return Optimiser();
}
#endif

//____________________________________________________________________ 
//  
// EOF
//
//____________________________________________________________________ 
//  
