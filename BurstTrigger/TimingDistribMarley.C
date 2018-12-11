#include <iostream>
#include <vector>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TPaveStats.h"
#include "TProfile.h"
#include "TTree.h"


int main() {
  TFile *file = new TFile("/Users/plasorak/Documents/Sussex - Postdoc/Physics/SNAna.root", "READ");
  TTree *tree = (TTree*)file->Get("snanagaushit/SNSimTree");
  TH1D*     timing             = new TH1D("timing",
                                          "Timing distribution (with neutronisation burst);Time [s];nEvent", 100, 0, 10);
  TH1D*     timing_noburst     = new TH1D("timing_noburst",
                                          "Timing distribution (without neutronisation burst);Time [s];nEvent", 100, 0, 10);
  TH1D*     energy_nu          = new TH1D("E_{#nu} with burst",
                                          ";Neutrino energy [MeV];nEvent", 100, 0, 50);
  TH1D*     energy_lep         = new TH1D("E_{e} with burst",
                                          ";Electron energy [MeV];nEvent", 100, 0, 50);
  TH1D*     energy_nu_noburst  = new TH1D("E_{#nu} without burst",
                                          ";Neutrino energy [MeV];nEvent", 100, 0, 50);
  TH1D*     energy_lep_noburst = new TH1D("E_{e} without burst",
                                          ";Electron energy [MeV];nEvent", 100, 0, 50);
  TProfile* timing_energy_nu   = new TProfile("energy_prof_nu",  ";Time [s];Neutrino energy [MeV]", 100, 0, 10);
  TProfile* timing_energy_lep  = new TProfile("energy_prof_lep", ";Time [s];Electron energy [MeV]", 100, 0, 10);

  std::vector<float>* ENu  = NULL;
  std::vector<float>* ELep = NULL;
  std::vector<float>* Time = NULL;

  tree->SetBranchAddress("True_ENu",      &ENu );
  tree->SetBranchAddress("True_ENu_Lep",  &ELep);
  tree->SetBranchAddress("True_MarlTime", &Time);
  
  for (int i=0; i<tree->GetEntries(); ++i) {
    tree->GetEntry(i);
    if (i%1000==0) std::cout << i << " / " << tree->GetEntries() << std::endl;
    double t = -1000;
    if (Time->size() > 0)
      t = Time->at(0);
      
    timing           ->Fill(t);
    energy_nu        ->Fill(ENu->at(0)*1000. );
    energy_lep       ->Fill(ELep->at(0)*1000.);
    timing_energy_nu ->Fill(t,ENu->at(0)*1000.);
    timing_energy_lep->Fill(t,ELep->at(0)*1000.);
    if (t>0.2) {
      timing_noburst->Fill(t);
      energy_nu_noburst ->Fill(ENu->at(0)*1000. );
      energy_lep_noburst->Fill(ELep->at(0)*1000.);
    }
    
  }

  TCanvas c;
  c.Print("TimingEnergyDistrib.pdf[");

  timing            ->SetLineWidth(2);
  timing_noburst    ->SetLineWidth(2);
  energy_nu         ->SetLineWidth(2);
  energy_lep        ->SetLineWidth(2);
  energy_nu_noburst ->SetLineWidth(2);
  energy_lep_noburst->SetLineWidth(2);
  timing_energy_nu  ->SetLineWidth(2);
  timing_energy_lep ->SetLineWidth(2);

  energy_nu_noburst ->SetLineColor(kRed);
  energy_lep_noburst->SetLineColor(kRed);

  timing            ->SetMinimum(0);
  timing_noburst    ->SetMinimum(0);
  energy_nu         ->SetMinimum(0);
  energy_lep        ->SetMinimum(0);
  energy_nu_noburst ->SetMinimum(0);
  energy_lep_noburst->SetMinimum(0);
  timing_energy_nu  ->SetMinimum(0);
  timing_energy_lep ->SetMinimum(0);

  // timing           ->SetStats(0); 
  // energy_nu        ->SetStats(0);
  // energy_lep       ->SetStats(0);
  // timing_energy_nu ->SetStats(0);
  // timing_energy_lep->SetStats(0);
  
  timing            ->Draw("E"); c.Print("TimingEnergyDistrib.pdf");
  timing_noburst    ->Draw("E"); c.Print("TimingEnergyDistrib.pdf");

  energy_nu         ->Draw("E");
  c.Modified(); c.Update();
  TPaveStats *stats =(TPaveStats*) c.GetPrimitive("stats");
  stats->SetName("h1stats");
  stats->SetX1NDC(0.7);
  stats->SetX2NDC(0.9);
  stats->SetY1NDC(0.7);
  stats->SetY2NDC(0.9);
  stats->SetTextColor(kBlue);
  
  energy_nu_noburst ->Draw("E SAMES");
  c.Modified(); c.Update();
  stats =(TPaveStats*) c.GetPrimitive("stats");
  stats->SetName("h2stats");
  stats->SetX1NDC(0.7);
  stats->SetX2NDC(0.9);
  stats->SetY1NDC(0.5);
  stats->SetY2NDC(0.7);
  stats->SetTextColor(kRed);
  c.Print("TimingEnergyDistrib.pdf");

  
  
  energy_lep        ->Draw("E");
  c.Modified(); c.Update();
  stats =(TPaveStats*) c.GetPrimitive("stats");
  stats->SetName("h3stats");
  stats->SetX1NDC(0.7);
  stats->SetX2NDC(0.9);
  stats->SetY1NDC(0.7);
  stats->SetY2NDC(0.9);
  stats->SetTextColor(kBlue);
  
  energy_lep_noburst->Draw("E SAMES");
  c.Modified(); c.Update();
  stats =(TPaveStats*) c.GetPrimitive("stats");
  stats->SetName("h4stats");
  stats->SetX1NDC(0.7);
  stats->SetX2NDC(0.9);
  stats->SetY1NDC(0.5);
  stats->SetY2NDC(0.7);
  stats->SetTextColor(kRed);
  c.Print("TimingEnergyDistrib.pdf");

  
  timing_energy_nu  ->Draw("E"); c.Print("TimingEnergyDistrib.pdf");
  timing_energy_lep ->Draw("E"); c.Print("TimingEnergyDistrib.pdf");
  c.Print("TimingEnergyDistrib.pdf]");

  return 0;
}
 
