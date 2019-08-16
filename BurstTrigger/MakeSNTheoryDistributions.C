
#include <iostream>
#include <vector>
#include <utility>
#include <TRandom.h>
#include <TH1.h>
#include <TPad.h>
#include <TF1.h>
#include <TCanvas.h>
#include <algorithm>
#include <TGraph.h>
#include <TString.h>
#include <TLine.h>
#include <TText.h>
#include <TMath.h>
#include <TFile.h>
#include <TLegend.h>
#include <TStyle.h>

TF1 *f_EventsVSNDistance_10kt;

void MakeZeroError(TH1D* h) {
  for (int i=0; i<=h->GetXaxis()->GetNbins(); ++i)
    h->SetBinError(i,0);

}

double finter(double *x, double*par) {
  return TMath::Abs(f_EventsVSNDistance_10kt->EvalPar(x,par) - 1);
}

std::pair<TF1*,TF1*> makeEventsVSNDistance() {
  double position_GalacticCentre(8), position_LMC(50), position_Andromeda(778), position_FarGalaxyEdge(23), position_NearGalaxyEdge(7);

  double dy(TMath::Log10(2.5e4)-TMath::Log10(0.03)), dx(TMath::Log10(1)-TMath::Log10(1000));
  f_EventsVSNDistance_10kt = new TF1("f_EventsVSNDistance_10kt","TMath::Power(10,[1])*TMath::Power(x,[0])",1,1000);
  f_EventsVSNDistance_10kt->SetParameter(0, dy/dx);
  f_EventsVSNDistance_10kt->SetParameter(1, TMath::Log10(3e4));
  f_EventsVSNDistance_10kt->SetLineWidth(2);

  TF1 *fint = new TF1("fint",finter,0,100000000,0);
  double xint = fint->GetMinimumX();
  double nevent = f_EventsVSNDistance_10kt->Eval(xint);
  std::cout << "1 event: " << xint << " (nevent " << nevent << ")"<< std::endl; 
  nevent = f_EventsVSNDistance_10kt->Eval(50);
  std::cout << "LMC nevents " << nevent << std::endl; 
  
  TF1 *f_EventsVSNDistance_10kt_Lower = new TF1("f_EventsVSNDistance_10kt_Lower","TMath::Power(10,[1])*TMath::Power(x,[0])",1,1000);
  f_EventsVSNDistance_10kt_Lower->SetParameter(0, dy/dx);
  f_EventsVSNDistance_10kt_Lower->SetParameter(1, TMath::Log10(2e4));
  f_EventsVSNDistance_10kt_Lower->SetLineStyle(8);
  TF1 *f_EventsVSNDistance_10kt_Upper = new TF1("f_EventsVSNDistance_10kt_Upper","TMath::Power(10,[1])*TMath::Power(x,[0])",1,1000);
  f_EventsVSNDistance_10kt_Upper->SetParameter(0, dy/dx);
  f_EventsVSNDistance_10kt_Upper->SetParameter(1, TMath::Log10(7e4));
  f_EventsVSNDistance_10kt_Upper->SetLineStyle(8);

  TF1 *f_EventsVSNDistance_40kt = new TF1("f_EventsVSNDistance_40kt","TMath::Power(10,[1])*TMath::Power(x,[0])",1,1000);
  f_EventsVSNDistance_40kt->SetParameter(0, dy/dx);
  f_EventsVSNDistance_40kt->SetParameter(1, TMath::Log10(1e5));
  f_EventsVSNDistance_40kt->SetLineColor(4);
  f_EventsVSNDistance_40kt->SetLineWidth(2);
  TF1 *f_EventsVSNDistance_40kt_Upper = new TF1("f_EventsVSNDistance_40kt_Upper","TMath::Power(10,[1])*TMath::Power(x,[0])",1,1000);
  f_EventsVSNDistance_40kt_Upper->SetParameter(0, dy/dx);
  f_EventsVSNDistance_40kt_Upper->SetParameter(1, TMath::Log10(4.5e5));
  f_EventsVSNDistance_40kt_Upper->SetLineColor(4);
  f_EventsVSNDistance_40kt_Upper->SetLineStyle(8);

  TText *text_GalacticCentre = new TText(position_GalacticCentre, 3e5, "Galactic Center");
  text_GalacticCentre->SetTextSize(0.02);
  TLine *line_GalacticCenter = new TLine(position_GalacticCentre, 0, position_GalacticCentre, 3e5);
  TText *text_LMC = new TText(position_LMC, 6e5, "LMC");
  text_LMC->SetTextSize(0.02);
  TLine *line_LMC = new TLine(position_LMC, 0, position_LMC, 6e5);
  TText *text_Andromeda = new TText(position_Andromeda, 6e5, "Andromeda");
  text_Andromeda->SetTextSize(0.02);
  TLine *line_Andromeda = new TLine(position_Andromeda, 0, position_Andromeda, 6e5);
  TText *text_FarGalaxyEdge = new TText(position_FarGalaxyEdge, 6e5, "Galaxy Far Side");
  text_FarGalaxyEdge->SetTextSize(0.02);
  TLine *line_FarGalaxyEdge = new TLine(position_FarGalaxyEdge, 0, position_FarGalaxyEdge, 6e5);
  TText *text_NearGalaxyEdge = new TText(position_NearGalaxyEdge, 6e5, "Galaxy Near Side");
  text_NearGalaxyEdge->SetTextSize(0.02);
  TLine *line_NearGalaxyEdge = new TLine(position_NearGalaxyEdge, 0, position_NearGalaxyEdge, 6e5);
  TText   *text_Reference = new TText(25,2000, "K. Scholberg, DUNE collaboration meeting. Sep '16");
  text_Reference->SetTextSize(0.03);

  TLegend *leg_EventsVSNDistance = new TLegend(0.7,0.7,0.88,0.88);
  TCanvas *c_EventsVSNDistance   = new TCanvas("c_EventsVSNDistance","c_EventsVSNDistance", 800, 500);
  c_EventsVSNDistance->Draw();
  c_EventsVSNDistance->SetLogx();
  c_EventsVSNDistance->SetLogy();
  f_EventsVSNDistance_10kt->SetTitle("Expected Number of Events for a Given Supernova Distance");
  f_EventsVSNDistance_10kt->GetXaxis()->SetTitle("Distance to Supernova, (kpc)");
  f_EventsVSNDistance_10kt->GetYaxis()->SetTitle("Expected Number of Events");
  f_EventsVSNDistance_10kt->SetMinimum(1e-2);
  f_EventsVSNDistance_10kt->SetMaximum(5e5);
  f_EventsVSNDistance_10kt->Draw();
  f_EventsVSNDistance_10kt_Lower->Draw("SAME");
  f_EventsVSNDistance_10kt_Upper->Draw("SAME");
  f_EventsVSNDistance_40kt->Draw("SAME");
  f_EventsVSNDistance_40kt_Upper->Draw("SAME");
  line_GalacticCenter->Draw();
  text_GalacticCentre->Draw();
  line_LMC->Draw();
  text_LMC->Draw();
  line_Andromeda->Draw();
  text_Andromeda->Draw();
  line_FarGalaxyEdge->Draw();
  text_FarGalaxyEdge->Draw();
  line_NearGalaxyEdge->Draw();
  text_NearGalaxyEdge->Draw();
  text_Reference->Draw();
  leg_EventsVSNDistance->AddEntry(f_EventsVSNDistance_10kt,"10kT","L");
  leg_EventsVSNDistance->AddEntry(f_EventsVSNDistance_40kt,"40kT","L");
  leg_EventsVSNDistance->Draw();

  c_EventsVSNDistance->SaveAs("EventsVSNDistance.pdf");

  TF1 *f_EventsVSNDistance_10kt_100kpc = new TF1("f_EventsVSNDistance_10kt_100kpc","TMath::Power(10,[1])*TMath::Power(x,[0])",1,100);
  f_EventsVSNDistance_10kt_100kpc->SetParameter(0, dy/dx);
  f_EventsVSNDistance_10kt_100kpc->SetParameter(1, TMath::Log10(3e4));
  TF1 *f_EventsVSNDistance_40kt_100kpc = new TF1("f_EventsVSNDistance_40kt_100kpc","TMath::Power(10,[1])*TMath::Power(x,[0])",1,100);
  f_EventsVSNDistance_40kt->SetParameter(0, dy/dx);
  f_EventsVSNDistance_40kt->SetParameter(1, TMath::Log10(1e5));
  std::pair<TF1*,TF1*> map_EventsVSNDistance = {f_EventsVSNDistance_10kt_100kpc, f_EventsVSNDistance_40kt_100kpc};

  return map_EventsVSNDistance;
}


TH1D* makeSNProbabilityVDistance()
{
  TH1D *h_SNProbabilityVDistance = new TH1D("h_SNProbabilityVDistance", "h_SNProbabilityVDistance", 31,-0.5,30.5);
  //WITH THESE EYEBALLED VALUES, THE INTEGRAL IS 0.9943. SO NO NOTICEABLE DIFFERENCE WHEN WE SCALE THE HISTOGRAM SO THAT IT INTEGRATES TO 1.
  std::vector<double> SNProb = {0, 0.0135, 0.026, 0.038, 0.048, 0.0535, 0.056, 0.057, 0.057, 0.0575, 0.0625, 0.0705, 0.0775, 0.0778,
                                    0.074,0.0605,  0.05, 0.038, 0.0255, 0.018, 0.012, 0.008, 0.0055, 0.0025, 0.002,  0.0015,  0.001, 
                                    0.0005, 0.0005,   0,    0};

  for(int i = 1; i < h_SNProbabilityVDistance->GetSize()-1; i++)
  {
    h_SNProbabilityVDistance->SetBinContent(i,SNProb[i-1]);
  }
  h_SNProbabilityVDistance->Scale(1/h_SNProbabilityVDistance->Integral());

  TCanvas *c_SNProbabilityVDistance = new TCanvas("c_SNProbabilityVDistance","c_SNProbabilityVDistance",800,500);
  gStyle->SetOptStat(0);
  TText   *text_Reference = new TText(17.5,0.06, "Mirizzi, Raffelt & Serpico, astro-ph/0604300");
  text_Reference->SetTextSize(0.03);
  h_SNProbabilityVDistance->SetTitle("Probability of a Supernova Happening at a Given Distance");
  h_SNProbabilityVDistance->GetXaxis()->SetTitle("Distance, (kpc)");
  h_SNProbabilityVDistance->GetYaxis()->SetTitle("SN Probability Distribution");
  MakeZeroError(h_SNProbabilityVDistance);
  h_SNProbabilityVDistance->Draw();
  text_Reference->Draw();
  c_SNProbabilityVDistance->SaveAs("SNProbabilityVDistance.pdf");

  return h_SNProbabilityVDistance;
}


TH1D* makeSNProbabilityVDistance_LMC_SingleEvent() {

  double milkyWayFraction = 0.8;
  TH1D *h_SNProbabilityVDistance = new TH1D("h_SNProbabilityVDistance_LMC", "h_SNProbabilityVDistance_LMC", 195, 4.5,199.5);
  //WITH THESE EYEBALLED VALUES, THE INTEGRAL IS 0.9943. SO NO NOTICEABLE DIFFERENCE WHEN WE SCALE THE HISTOGRAM SO THAT IT INTEGRATES TO 1.
  std::vector<double> SNProb = {0, 0.0135, 0.026, 0.038, 0.048, 0.0535, 0.056, 0.057, 0.057, 0.0575, 0.0625, 0.0705, 0.0775, 0.0778,
                                    0.074,0.0605,  0.05, 0.038, 0.0255, 0.018, 0.012, 0.008, 0.0055, 0.0025, 0.002,  0.0015,  0.001, 
                                    0.0005, 0.0005,   0,    0};

  for(unsigned int i = 0; i < SNProb.size(); i++)
    h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(i),SNProb[i]);

  h_SNProbabilityVDistance->Scale(milkyWayFraction/h_SNProbabilityVDistance->Integral());

  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(49), 0.05);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(50), 0.05);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(51), 0.05);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(52), 0.05);

  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(175), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(176), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(177), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(178), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(179), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(180), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(181), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(182), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(183), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(184), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(185), 0.1);
  h_SNProbabilityVDistance->SetBinContent(h_SNProbabilityVDistance->FindBin(186), 0.1);
  std::cout << "THE PROBABILITY OF A SUPERNOVA HAPPENING IN THE GALACTIC NEIGHBOURHOOD IS: " << h_SNProbabilityVDistance->Integral() 
            << std::endl;

  TCanvas *c_SNProbabilityVDistance = new TCanvas("c_SNProbabilityVDistance_LMC","c_SNProbabilityVDistance_LMC",800,500);
  gStyle->SetOptStat(0);
  TText *text_Reference = new TText(5.5,0.1, "Mirizzi, Raffelt & Serpico, astro-ph/0604300");
  TText *text_single    = new TText(160,0.0005, "Single event SN (arbitrary normalisation)");
  text_Reference->SetTextSize(0.03);
  text_single   ->SetTextSize(0.03);
  text_single->SetTextAngle(90);
  h_SNProbabilityVDistance->SetTitle("Probability of a Supernova Happening at a Given Distance");
  h_SNProbabilityVDistance->GetXaxis()->SetTitle("Distance, (kpc)");
  h_SNProbabilityVDistance->GetYaxis()->SetTitle("SN Probability Distribution");
  h_SNProbabilityVDistance->Draw();
  MakeZeroError(h_SNProbabilityVDistance);
  text_Reference->Draw();
  text_single->Draw();
  gPad->SetLogx();
  gPad->SetLogy();
  c_SNProbabilityVDistance->SaveAs("SNProbabilityVDistance_LMC.pdf");

  return h_SNProbabilityVDistance;
}



TH1D* MakeTimeDistributionMarley() {
  TH1D *MarleyTimingDistrib = new TH1D("h_MarlTime", "h_MarlTime", 100, 0, 10000);
  std::vector<double> TimeDistrib = {0.001253364,0.2235393,0.09093123,0.07145459,0.06371181,0.05943476,0.04810839,0.03627973,
                                     0.0254753,0.0217516,0.02007399,0.01885441,0.01764688,0.01669913,0.01547588,0.01459596,
                                     0.01392764,0.01301943,0.01226548,0.01147224,0.01056875,0.01002796,0.009188631,0.008774073,
                                     0.007997595,0.007613677,0.007073154,0.006473445,0.00603139,0.005543505,0.00518656,
                                     0.004737434,0.004409558,0.00424274,0.004143749,0.003941576,0.00369567,0.003340559,
                                     0.003194691,0.003324584,0.003205166,0.003016088,0.002978639,0.002936999,0.002853721,
                                     0.002789298,0.002686117,0.002521918,0.002443615,0.002360599,0.002385215,0.002283867,
                                     0.002268416,0.002190376,0.002094789,0.002012296,0.001887641,0.001838145,0.00182348,
                                     0.001704062,0.001722917,0.001656138,0.001514722,0.001594857,0.001512103,0.001444014,
                                     0.001401851,0.001331405,0.001343713,0.001145207,0.001317263,0.001089688,0.001097021,
                                     0.001003529,0.001118495,0.0009765556,0.0008948486,0.000960319,0.0009254887,0.0009058476,
                                     0.00084771,0.0007356247,0.0007657411,0.000706294,0.000779097,0.0006515608,0.0006274677,
                                     0.0005884474,0.0005439276,0.0005376424,0.0004912894,0.0004664107,0.0004365562,0.000439175,
                                     0.0004567211,0.0003600869,0.0003469928,0.0003187096,0.0003003779,0.0002697378,0.0002807368,
                                     0.0003328512};
  for(unsigned int i = 0; i < TimeDistrib.size(); i++)
    MarleyTimingDistrib->SetBinContent(i, TimeDistrib[i]);

  return MarleyTimingDistrib;
}



int main() {
  TFile *f_Output = new TFile("SNTheoryDistributions.root","RECREATE");
  std::pair<TF1*,TF1*> map_EventsVSNDistance = makeEventsVSNDistance();
  TH1D* h_SNProbabilityVDistance     = makeSNProbabilityVDistance();
  TH1D* h_SNProbabilityVDistance_LMC = makeSNProbabilityVDistance_LMC_SingleEvent();
  TH1D* h_MarlTime                   = MakeTimeDistributionMarley();
  MakeZeroError(h_SNProbabilityVDistance    );
  MakeZeroError(h_SNProbabilityVDistance_LMC);
  MakeZeroError(h_MarlTime);
  map_EventsVSNDistance.first->Write();
  map_EventsVSNDistance.second->Write();
  h_SNProbabilityVDistance->Write();
  h_SNProbabilityVDistance_LMC->Write();
  h_MarlTime->Write();
  f_Output->Close();
  return 0;
}
