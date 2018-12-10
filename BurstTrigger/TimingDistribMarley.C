




void TimingDistribMarley() {
  TFile *file = new TFile("../../SNAna.root", "READ");
  TTree *tree = (TTree*)file->Get("snanagaushit/SNSimTree");
  TH1D*     timing            = new TH1D("timing", ";Time [s];nEvent", 100, 0, 50);
  TH1D*     energy_nu         = new TH1D("energy", ";Neutrino energy [MeV];nEvent", 100, 0, 50);
  TH1D*     energy_lep        = new TH1D("energy", ";Electron energy [MeV];nEvent", 100, 0, 50);
  TProfile* timing_energy_nu  = new TProfile("energy_prof_nu",  ";Time [s];Neutrino energy [MeV]", 100, 0, 50);
  TProfile* timing_energy_lep = new TProfile("energy_prof_lep", ";Time [s];Electron energy [MeV]", 100, 0, 50);

  std::vector<float>* ENu  = NULL;
  std::vector<float>* ELep = NULL;
  std::vector<float>* Time = NULL;

  tree->SetBranchAddress("True_ENu",     &ENu );
  tree->SetBranchAddress("True_ENu_Lep", &ELep);
  tree->SetBranchAddress("True_Time",    &Time);
  
  for (int i=0; i<tree->GetEntries(); ++i) {
    tree->GetEntry(i);
    timing           ->Fill(Time->at(0));
    energy_nu        ->Fill(ENu->at(0));
    energy_lep       ->Fill(ELep->at(0));
    timing_energy_nu ->Fill(Time->at(0),ENu->at(0));
    timing_energy_lep->Fill(Time->at(0),ELep->at(0));    
  }

  TCanvas c;
  c.Print("TimingEnergyDistrib.pdf[");
  timing           ->Draw();c.Print("TimingEnergyDistrib.pdf");
  energy_nu        ->Draw();c.Print("TimingEnergyDistrib.pdf");
  energy_lep       ->Draw();c.Print("TimingEnergyDistrib.pdf");
  timing_energy_nu ->Draw();c.Print("TimingEnergyDistrib.pdf");
  timing_energy_lep->Draw();c.Print("TimingEnergyDistrib.pdf");
  c.Print("TimingEnergyDistrib.pdf]");
}
 
