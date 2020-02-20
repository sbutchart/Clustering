//____________________________________________________________________ 
//  
// $Id$ 
// Author: Pierre Lasorak <plasorak@pa-246-59.byodstaff.susx.ac.uk>
// Update: 2019-06-29 08:12:08+0100
// Copyright: 2019 (C) Pierre Lasorak
//
//
#ifndef __CINT__
// PUT HEADERS HERE
#include "TGraph.h"
#endif

#include "Helper.h"


double Eval(TGraph* s) {
  std::vector<double> x, y;
  
  for (int i=0; i<s->GetN(); ++i) {
    double xx, yy;
    s->GetPoint(i, xx, yy);
    x.push_back(xx);
    y.push_back(yy);
  }
  std::stable_sort(x.begin(), x.end());
  std::stable_sort(y.begin(), y.end());

  double xmean1 = 0.5*(x[0]+x[1]);
  double ymean1 = 0.5*(y[0]+y[1]);
  double xmean2 = 0.5*(x[1]+x[2]);
  double ymean2 = 0.5*(y[1]+y[2]);

  double slope  = (ymean1 - ymean2) / (xmean1 - xmean2);
  double origin = ymean1 - slope * xmean1;

  return origin + slope * 1;
}

int PlotEfficiencySmartTrigger()
{
  // DEFINE YOUR MAIN FUNCTION HERE
  int UpTo = 30;
  TGraph* EfficiencyLLHFull  = new TGraph(UpTo);
  TGraph* EfficiencyLLHShape = new TGraph(UpTo);
  TGraph* EfficiencyLLHNorm  = new TGraph(UpTo);
  
  for (int nEvent=1; nEvent<=UpTo; ++nEvent) {
    TFile f(Form("./LikelihoodTriggerOutput_n%d.root", nEvent));
    TGraph* full  = (TGraph*)f.Get("loglikelihood");
    TGraph* shape = (TGraph*)f.Get("loglikelihoodshape");
    TGraph* norm  = (TGraph*)f.Get("loglikelihoodnorm");
    
    EfficiencyLLHFull ->SetPoint(nEvent-1, nEvent, full ->Eval(1.,NULL, "S"));
    EfficiencyLLHShape->SetPoint(nEvent-1, nEvent, Eval(shape));
    EfficiencyLLHNorm ->SetPoint(nEvent-1, nEvent, norm ->Eval(1.,NULL, "S"));
    f.Close();
  }
  std::vector<int> col = getColors();
  
  EfficiencyLLHFull ->SetLineColor(col[0]);
  EfficiencyLLHShape->SetLineColor(col[1]);
  EfficiencyLLHNorm ->SetLineColor(col[2]);

  EfficiencyLLHFull ->SetMarkerColor(col[0]);
  EfficiencyLLHShape->SetMarkerColor(col[1]);
  EfficiencyLLHNorm ->SetMarkerColor(col[2]);

  EfficiencyLLHFull ->SetLineWidth(2.);
  EfficiencyLLHShape->SetLineWidth(2.);
  EfficiencyLLHNorm ->SetLineWidth(2.);

  TCanvas c;
  c.Print("Efficiencies.pdf[");
  EfficiencyLLHFull ->GetHistogram()->SetMaximum(1.2);
  EfficiencyLLHFull ->GetHistogram()->SetMinimum(0);
  EfficiencyLLHFull ->SetTitle(";Number of SN event in 10 kT;Triggering efficiency (1 fake / month)");
  EfficiencyLLHFull ->Draw("ALP");
  EfficiencyLLHShape->Draw("LP");
  EfficiencyLLHNorm ->Draw("LP");
  gPad->SetTicks();
  gPad->SetGridx();
  gPad->SetGridy();

  TLegend* leg = new TLegend(0.5,0.1,0.9,0.3);
  leg->AddEntry(EfficiencyLLHFull , "Full likelihood");
  leg->AddEntry(EfficiencyLLHShape, "Likelihood with only shape");
  leg->AddEntry(EfficiencyLLHNorm , "Likelihood with only normalisation");
  leg->Draw();
  c.Print("Efficiencies.pdf");

  EfficiencyLLHFull ->SetLineColor(kBlack);
  EfficiencyLLHFull ->Draw("ALP");
  c.Print("Efficiencies.pdf");
  
  for (int nEvent=1; nEvent<=UpTo; ++nEvent) {
    TFile f(Form("./LikelihoodTriggerOutput_n%d.root", nEvent));
    TGraph* full  = (TGraph*)f.Get("loglikelihood");
    TGraph* shape = (TGraph*)f.Get("loglikelihoodshape");
    TGraph* norm  = (TGraph*)f.Get("loglikelihoodnorm");
    
    full ->SetLineColor(col[0]);
    shape->SetLineColor(col[1]);
    norm ->SetLineColor(col[2]);

    full ->SetMarkerColor(col[0]);
    shape->SetMarkerColor(col[1]);
    norm ->SetMarkerColor(col[2]);

    full ->SetLineWidth(2.);
    shape->SetLineWidth(2.);
    norm ->SetLineWidth(2.);
  
    full ->GetHistogram()->SetMaximum(1.4);
    full ->GetHistogram()->SetMinimum(0);
    gPad->SetLogx();
    full ->SetTitle(Form("SN of %d #nu",nEvent));
    full ->Draw("ALP");
    shape->Draw("LP");
    norm ->Draw("LP");


    TLegend* leg = new TLegend(0.1,0.8,0.5,0.9);
    leg->AddEntry(full , "Full likelihood");
    leg->AddEntry(shape, "Likelihood with only shape");
    leg->AddEntry(norm , "Likelihood with only normalisation");
    leg->Draw();
  
    c.Print("Efficiencies.pdf");

    f.Close();
  }

  
  c.Print("Efficiencies.pdf]");
  return 0;
}

#ifndef __CINT__
int main(int argc, char** argv)
{
  return PlotEfficiencySmartTrigger();
}
#endif

//____________________________________________________________________ 
//  
// EOF
//
