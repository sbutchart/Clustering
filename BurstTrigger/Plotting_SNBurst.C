#include <algorithm>
#include <iostream>
#include <vector>
#include <fstream>
#include <unistd.h>

#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "THStack.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TText.h"
#include "TROOT.h"

#include "colors.h"
#include "Configuration.hh"
#include "Utils.hh"

int main(int argc, char** argv) {

  int opt;
  std::string InputRootFile = "";
  std::string OutputFile = "";
  extern char *optarg;
  extern int  optopt;
  while ( (opt = getopt(argc, argv, "t:i:o:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'i':
      InputRootFile = optarg;
      break;
    case 'o':
      OutputFile = optarg;
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
    
  if (OutputFile == "") {
    std::cout << "No output file parsed!! Exiting." << std::endl;
    exit(1);
  }


  std::vector<Configuration> Configs = GetConfigurationRootFile(InputRootFile);
  TCanvas* c = new TCanvas();
  c->Print((OutputFile+"[").c_str());
  std::vector<int> vec_Colors = getColors(2);
  TLegend* leg = new TLegend(0.1,0.1,0.9,0.9);
  
  //fake rate plot
  Configs[0].fTH1DFakeRate_Cut->SetMinimum(1e-8);
  Configs[0].fTH1DFakeRate_Cut->Draw();
  int i=1;
  gPad->SetLogy();
  gPad->SetGridx();
  gPad->SetGridy();
  for (auto const& it: Configs) {
    it.fTH1DFakeRate_Cut       ->SetLineColor(vec_Colors[i]);
    it.fTH1DEfficiency_Burst   ->SetLineColor(vec_Colors[i]);
    it.fTH1DEfficiency_Distance->SetLineColor(vec_Colors[i]);
    it.fTH1DCoverage           ->SetLineColor(vec_Colors[i]);
    it.fTH1DFakeRate_Cut       ->SetLineWidth(2);
    it.fTH1DEfficiency_Burst   ->SetLineWidth(2);
    it.fTH1DEfficiency_Distance->SetLineWidth(2);
    it.fTH1DCoverage           ->SetLineWidth(2);
    leg->AddEntry(it.fTH1DCoverage, it.fLegendEntry.c_str());
    it.fTH1DFakeRate_Cut->Draw("SAME");
    i++;
  }
  TLine *l_perMonth  = new TLine(0, 1./3600./24./31, Configs[0].fTH1DFakeRate_Cut->GetXaxis()->GetXmax(), 1./3600./24./31);
  l_perMonth->Draw();
  gPad->RedrawAxis();
  c->Print(OutputFile.c_str());
  
  //trigger efficiency vs no. of interactions/10kT
  gPad->SetLogy(false);
  gPad->SetLogx(false);
  gPad->SetGridx();
  gPad->SetGridy();
  Configs[0].fTH1DEfficiency_Burst->Draw("");
  for (auto const& it: Configs) {
    it.fTH1DEfficiency_Burst->Draw("SAME");
    it.fTH1DEfficiency_Burst->GetXaxis()->SetRangeUser(1, 100);
    i++;
  }
  gPad->RedrawAxis();
  c->Print(OutputFile.c_str());
  

  //trigger efficiency vs. distance
  gPad->SetGridx();
  gPad->SetGridy();
  Configs[0].fTH1DEfficiency_Distance->Draw();
  for (auto const& it: Configs) {
    it.fTH1DEfficiency_Distance->Draw("SAME");
    it.fTH1DEfficiency_Distance->GetXaxis()->SetRangeUser(1, 70);
    i++;
  }

  //SN1987 line
  TLine *LMC = new TLine(51.4, 0, 51.4, 1.05);
  LMC->SetLineColor(1);
  LMC->SetLineWidth(3);
  LMC->Draw();
  TLatex LMC_label;
  LMC_label.SetTextSize(0.03);
  LMC_label.SetTextAlign(13);
  LMC_label.DrawLatex(52, 0.99, "SN 1987A");

  //SN1054 line
  //TLine *M1 = new TLine(2.0, 0, 2.0, 1.05);
  //M1->SetLineColor(1);
  //M1->SetLineWidth(3);
  //M1->Draw();
  //TLatex M1_label;
  //M1_label.SetTextSize(0.03);
  //M1_label.SetTextAlign(13);
  //M1_label.DrawLatex(2.5, 0.99, "SN 1054 (M1)");


  gPad->SetLogx(false);
  gPad->RedrawAxis();
  c->Print(OutputFile.c_str());


  //SN probability vs. distance  
  gPad->SetLogy(false);
  TH1D  *hSNProbabilityVDistance = (TH1D*)Configs[0].fDistanceProbability->Clone();
  hSNProbabilityVDistance->GetXaxis()->SetRange(0,50);
  hSNProbabilityVDistance->SetLineWidth(4);
  hSNProbabilityVDistance->SetLineColor(46);
  hSNProbabilityVDistance->Draw();
  for (auto const& it: Configs) {
    it.fTH1DCoverage->Draw("SAME");
    i++;
  }
  gPad->RedrawAxis();
  c->Print(OutputFile.c_str());

  gPad->Clear();
  leg->Draw();
  c->Print(OutputFile.c_str());

  c->Print((OutputFile+"]").c_str());

  return 1;
}









  
//   TString s_Filename = "GH_SNMC";
//   ifstream inFile;
//   inFile.open("Analyse_"+s_Filename+".txt");

//   std::vector<int> vec_Config;
  
//   std::map<CONFIG,DATA> map_ConfigToEffAndBkgd;
//   int Config;
//   double TimeWindow, Eff, Bkgd, BkgdErr;

//   while(inFile >> Config >> TimeWindow >> Eff >> Bkgd >> BkgdErr){
//     CONFIG c;
//     c.Config = Config;
//     c.TimeWindow = TimeWindow;
//     DATA d;
//     d.Eff         = Eff;  
//     d.BkgdRate    = Bkgd;
//     d.BkgdRateErr = BkgdErr;   
//     map_ConfigToEffAndBkgd[c] = d;
    
//     if(std::find(vec_Config.begin(), vec_Config.end(), Config) == vec_Config.end())
//       vec_Config.push_back(Config);
//   }
  
//   int nConfig = (int)vec_Config.size();
//   std::cout << "There are " << nConfig << " configs." << std::endl;
  
//   gROOT->ForceStyle();
//   std::vector<int> vec_Colors = getColors(2);
//   TFile *f_SNTheoryDistributions = new TFile("SNTheoryDistributions.root","READ");
//   TH1D  *h_SNProbabilityVDistance = (TH1D*)f_SNTheoryDistributions->Get("h_SNProbabilityVDistance_LMC");
//   h_SNProbabilityVDistance->SetLineWidth(3);
//   h_SNProbabilityVDistance->SetLineColor(46);

//   TFile *f_Input  = new TFile("Analyse_SNBurst_GH_SNMC.root", "READ");
//   std::map<CONFIG,TH1D*>   map_h_FakeRateVNClusters;
//   std::map<CONFIG,TH1D*>   map_h_FakeRateVNClustersLow;
//   std::map<CONFIG,TH1D*>   map_h_EfficiencyVEvents;
//   std::map<CONFIG,TH1D*>   map_h_EfficiencyVDistance;
//   std::map<CONFIG,TH1D*>   map_h_EffGalaxy;
//   std::map<CONFIG,TH1D*>   map_h_Dummy;
//   std::map<CONFIG,TGraph*> map_g_ROC;

//   int globalIt=0;
//   for(auto& it : map_ConfigToEffAndBkgd){
    
//     int color = vec_Colors.at(globalIt % vec_Colors.size());

//     Config     = it.first.Config;
//     TimeWindow = it.first.TimeWindow;
//     std::cout << "Dealing with Config " << Config <<  " time window " << TimeWindow << std::endl;
//     TString s_FakeRateVNClusters     = Form("h_FakeRateVNClusters_Config%i_TW%.3f",    Config, TimeWindow);
//     TString s_FakeRateVNClustersLow  = Form("h_FakeRateVNClustersLow_Config%i_TW%.3f", Config, TimeWindow);
//     TString s_EfficiencyVEvents      = Form("h_EfficiencyVEvents_Config%i_TW%.3f",     Config, TimeWindow);
//     TString s_EfficiencyVDistance    = Form("h_EfficiencyVDistance_Config%i_TW%.3f",   Config, TimeWindow);
//     TString s_EffGalaxy              = Form("h_NeighbourhoodEffiency_Config%i_TW%.3f", Config, TimeWindow);
//     TString s_ROC                    = Form("g_ROC_Config%i_TW%.3f",                   Config, TimeWindow);
    
//     TH1D *h_FakeRateVNClusters    = (TH1D*)f_Input->Get(s_FakeRateVNClusters); 
//     TH1D *h_FakeRateVNClustersLow = (TH1D*)f_Input->Get(s_FakeRateVNClustersLow);
    
//     TH1D *h_EfficiencyVEvents = (TH1D*)f_Input->Get(s_EfficiencyVEvents);
//     if (!h_EfficiencyVEvents) {
//       std::cout << "Erasing config " << Config << " as there is no EfficiencyVEvents plot" << std::endl;
//       map_ConfigToEffAndBkgd.erase(it.first);
//       continue;
//     }

//     int   maxBin_Events = h_EfficiencyVEvents->GetMaximumBin();
//     for(int i = maxBin_Events; i < h_EfficiencyVEvents->GetSize()-1; i++)
//       h_EfficiencyVEvents->SetBinContent(i,1);

//     TH1D *h_EfficiencyVDistance = (TH1D*)f_Input->Get(s_EfficiencyVDistance);
//     int   maxBin_Distance = h_EfficiencyVDistance->GetMaximumBin();
//     for(int i = maxBin_Distance; i > 0; i--)
//       h_EfficiencyVDistance->SetBinContent(i,1);
//     TH1D   *h_EffGalaxy = (TH1D*)f_Input->Get(s_EffGalaxy);
//     if(!h_EffGalaxy){
//       std::cout << "Erasing config " << Config << " as there is no NeighbourhoodEffiency plot" << std::endl;
//       map_ConfigToEffAndBkgd.erase(it.first);
//       continue;
//     }
//     TH1D   *h_Dummy = new TH1D(Form("Config%i",Config), "", 1, 0, 1);
//     map_h_Dummy[it.first] = h_Dummy;
//     map_h_Dummy[it.first]->SetLineWidth(3);
//     map_h_Dummy[it.first]->SetLineColor(color);
    
//     TGraph *g_ROC = (TGraph*)f_Input->Get(s_ROC);
//     h_FakeRateVNClusters   ->SetLineColor  (color);
//     h_FakeRateVNClustersLow->SetLineColor  (color);
//     h_FakeRateVNClusters   ->SetMarkerColor(color);
//     h_FakeRateVNClustersLow->SetMarkerColor(color);
//     h_EfficiencyVEvents    ->SetLineColor  (color);
//     h_EfficiencyVDistance  ->SetLineColor  (color);
//     h_EffGalaxy            ->SetLineColor  (color);
//     g_ROC                  ->SetMarkerColor(color);
//     g_ROC                  ->SetLineColor  (color);

//     g_ROC->SetMarkerStyle(3);

//     map_h_FakeRateVNClusters   [it.first] = h_FakeRateVNClusters;
//     map_h_FakeRateVNClustersLow[it.first] = h_FakeRateVNClustersLow;
//     map_h_EfficiencyVEvents    [it.first] = h_EfficiencyVEvents;
//     map_h_EfficiencyVDistance  [it.first] = h_EfficiencyVDistance;
//     map_h_EffGalaxy            [it.first] = h_EffGalaxy;
//     map_g_ROC                  [it.first] = g_ROC;
//     globalIt++;    
//   }
//   TCanvas *c_Global = new TCanvas();
//   c_Global->Print("Results.pdf[");
//   std::string legHeader = "Individual Marley Eff & 10kT Bkgd Rate";
//   std::string legEntryFormatWire = "Config: %i - Eff: %.2f & Bkgd rate: %.3f Hz";
  
//   THStack *stk_FakeRateVNClusters = new THStack("stk_FakeRateVNClusters",
//                                                 "Number of Clusters in Time Window Required to Trigger vs. Trigger Rate");
//   TLegend *leg_FakeRateVNClusters = new TLegend(0.01, 0.01, 0.99, 0.99);
//   leg_FakeRateVNClusters->SetHeader(legHeader.c_str());

//   for(auto const& it : map_ConfigToEffAndBkgd){
//     stk_FakeRateVNClusters->Add(map_h_FakeRateVNClusters     [it.first]);
//     stk_FakeRateVNClusters->Add(map_h_FakeRateVNClustersLow  [it.first]);
//     leg_FakeRateVNClusters->AddEntry(map_h_Dummy[it.first],
//                                      Form(legEntryFormatWire.c_str(),
//                                           it.first.Config%100,
//                                           it.second.Eff,
//                                           it.second.BkgdRate, "L"));
//   }
//   leg_FakeRateVNClusters->Draw();
//   c_Global->Print("Results.pdf");

//   c_Global->SetLogy();
//   c_Global->SetLogx();
//   stk_FakeRateVNClusters->SetMinimum(1e-9);
//   stk_FakeRateVNClusters->Draw("NOSTACK C");
  
//   stk_FakeRateVNClusters->GetXaxis()->SetTitle("Number of Clusters/Time Window");
//   stk_FakeRateVNClusters->GetYaxis()->SetTitle("Trigger Rate, (Hz)");
//   stk_FakeRateVNClusters->GetXaxis()->SetLimits(0,map_h_FakeRateVNClusters.begin()->second->GetXaxis()->GetXmax());
//   double range = map_h_FakeRateVNClusters.begin()->second->GetXaxis()->GetXmax() - map_h_FakeRateVNClusters.begin()->second->GetXaxis()->GetXmin();
//   TText *t_perMonth  = new TText(map_h_FakeRateVNClusters.begin()->second->GetXaxis()->GetXmax()-0.5*range, 1.1e-7,    "1/Month");
//   TText *t_perYear   = new TText(map_h_FakeRateVNClusters.begin()->second->GetXaxis()->GetXmax()-0.5*range, 1.1e-7/12.,"1/Year");
//   TLine *l_perMonth  = new TLine(0, 4.13e-7, map_h_FakeRateVNClusters.begin()->second->GetXaxis()->GetXmax(), 4.13e-7);
//   TLine *l_perYear   = new TLine(0, 4.13e-7/12., map_h_FakeRateVNClusters.begin()->second->GetXaxis()->GetXmax(), 4.13e-7/12.);
//   l_perMonth->SetLineColor(4);
//   l_perMonth->SetLineWidth(3);
//   l_perYear->SetLineColor(4);
//   l_perYear->SetLineWidth(3);
//   t_perMonth->Draw();
//   l_perMonth->Draw();
//   t_perYear->Draw();
//   l_perYear->Draw();
//   gPad->RedrawAxis(); 
//   c_Global->Print("Results.pdf");

//   c_Global->SetLogx(false);
//   THStack *stk_EfficiencyVEvents = new THStack("stk_EfficiencyVEvents", "Efficiency vs. Number of Events in SN Burst, Fake Trigger Rate: 1/Month");
//   for(auto const& it_Config : map_ConfigToEffAndBkgd){
//     stk_EfficiencyVEvents->Add(map_h_EfficiencyVEvents[it_Config.first]);
//   }
//   c_Global->Clear();
//   c_Global->SetLogy(false);
//   c_Global->Draw();
//   c_Global->SetLogx();
//   stk_EfficiencyVEvents->Draw("NOSTACK");
//   stk_EfficiencyVEvents->GetXaxis()->SetTitle("Number of Events in SN Burst");
//   stk_EfficiencyVEvents->GetYaxis()->SetTitle("Efficiency");
//   gPad->RedrawAxis();
//   c_Global->Print("Results.pdf");

//   THStack *stk_EfficiencyVDistance = new THStack("stk_EfficiencyVDistance", "Efficiency vs. Distance to SN, Fake Trigger Rate: 1/Month");
//   for(auto const& it_Config : map_ConfigToEffAndBkgd){
//     stk_EfficiencyVDistance->Add(map_h_EfficiencyVDistance[it_Config.first]);
//   }
//   c_Global->Clear();
//   c_Global->Draw();
//   c_Global->SetLogx();
//   stk_EfficiencyVDistance->Draw("NOSTACK");
//   stk_EfficiencyVDistance->GetXaxis()->SetTitle("SN Distance, (kpc)");
//   stk_EfficiencyVDistance->GetYaxis()->SetTitle("Efficiency");
//   gPad->RedrawAxis();
//   c_Global->Print("Results.pdf");


//   gStyle->SetOptStat(0);
//   THStack *stk_EffGalaxy = new THStack("stk_EffGalaxy", "Galactic Neighbourhood Coverage with Fake Trigger Rate 1/Month/10kT");

//   stk_EffGalaxy->Add(h_SNProbabilityVDistance);

//   for(auto const& it_Config : map_ConfigToEffAndBkgd){
//     stk_EffGalaxy->Add(map_h_EffGalaxy[it_Config.first]);
//   }
//   c_Global->Clear();
//   c_Global->Draw();
//   c_Global->SetLogx(true);
//   c_Global->SetLogy();
//   stk_EffGalaxy->SetMinimum(1e-5);
//   stk_EffGalaxy->Draw("NOSTACK HIST");
//   stk_EffGalaxy->GetXaxis()->SetTitle("SN Distance, (kpc)");
//   stk_EffGalaxy->GetYaxis()->SetTitle("Efficiency x SN Probability");
//   stk_EffGalaxy->Draw("NOSTACK HIST");
//   TText *text_single = new TText(160,0.00005, "Single event SN (10kT, arbitrary normalisation)");
//   text_single->SetTextSize(0.03);
//   text_single->SetTextAngle(90);
//   TText *text_LMC = new TText(45,0.00005, "Large Magellanic Cloud (13 events/10kT)");
//   text_LMC->SetTextSize(0.03);
//   text_LMC->SetTextAngle(90);
//   TText *text_MW  = new TText(33,0.00005, "Far end of the Milky Way (60 events/10kT)");
//   text_MW->SetTextSize(0.03);
//   text_MW->SetTextAngle(90);

//   text_single->Draw();
//   text_LMC->Draw();
//   text_MW->Draw();
//   gPad->SetGridy();
//   gPad->RedrawAxis();
//   c_Global->Print("Results.pdf");


//   c_Global->Clear();
//   c_Global->Draw();
//   double minX=0.6, maxX=1;
//   double minY=10e-15, maxY=10;

//   map_g_ROC.begin()->second->GetXaxis()->SetLimits(minX, maxX);
//   map_g_ROC.begin()->second->SetMaximum(maxY);
//   map_g_ROC.begin()->second->SetMinimum(minY);
//   map_g_ROC.begin()->second->SetTitle("Fake Trigger Rate vs. Galactic Neighbourhood Coverage");
//   map_g_ROC.begin()->second->GetXaxis()->SetTitle("Galactic Neighbourhood Coverage");
//   map_g_ROC.begin()->second->GetYaxis()->SetTitle("Fake Trigger Rate, (Hz)");
//   map_g_ROC.begin()->second->SetMaximum(10);
//   map_g_ROC.begin()->second->Draw("AP");

//   for(auto const& it_Config : map_ConfigToEffAndBkgd){
//     map_g_ROC[it_Config.first]->Draw("P");
//   }
//   TLine *l_perMonth_2 = new TLine(minX, 4.13e-7, maxX, 4.13e-7);
//   l_perMonth_2->SetLineColor(1);
//   l_perMonth_2->SetLineWidth(3);
//   TText *t_perMonth_2 = new TText(minX+0.015, 8e-7, "1/Month");
//   gPad->RedrawAxis();
//   l_perMonth_2->Draw();
//   t_perMonth_2->Draw();
//   c_Global->Print("Results.pdf");
//   c_Global->Print("Results.pdf]");

//   return 0;
// }
