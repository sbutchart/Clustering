//____________________________________________________________________ 
//  
// $Id$ 
// Author: Pierre Lasorak <plasorak@pa-241-90.byodstaff.susx.ac.uk>
// Update: 2019-09-05 17:01:26+0100
// Copyright: 2019 (C) Pierre Lasorak
//
//
#ifndef __CINT__
// PUT HEADERS HERE
#endif

int MakeEventDisplay() {
  // DEFINE YOUR MAIN FUNCTION HERE
  TFile f ("~/Desktop/InterestingEvent.root");
  TTree* t = (TTree*)f.Get("interesting_event");


  int Config=0;
  int Event=0;
  int MarleyIndex=0;
  double StartChan=0, EndChan=0;
  double FirstTimeHit=0, LastTimeHit=0;
  double elep_tree=0, enu_tree=0, sum_adc_tree=0, time_tree=0;
  int config_tree=0;
  double PosX_tree=0,PosY_tree=0,PosZ_tree=0,DirX_tree=0,DirY_tree=0,DirZ_tree=0;
  std::vector<double> *HitTime= nullptr;
  std::vector<int   > *HitChan= nullptr;
  std::vector<int   > *GenType= nullptr;
  std::vector<double> *HitSADC= nullptr;

  t->SetBranchAddress("Time",          &time_tree   );
  t->SetBranchAddress("ENu",           &enu_tree    );
  t->SetBranchAddress("ELep",          &elep_tree   );
  t->SetBranchAddress("SumADC",        &sum_adc_tree);
  t->SetBranchAddress("PosX",          &PosX_tree   );
  t->SetBranchAddress("PosY",          &PosY_tree   );
  t->SetBranchAddress("PosZ",          &PosZ_tree   );
  t->SetBranchAddress("DirX",          &DirX_tree   );
  t->SetBranchAddress("DirY",          &DirY_tree   );
  t->SetBranchAddress("DirZ",          &DirZ_tree   );
  t->SetBranchAddress("Config",        &config_tree );
  t->SetBranchAddress("HitTime",       &HitTime     );
  t->SetBranchAddress("HitChan",       &HitChan     );
  t->SetBranchAddress("GenType",       &GenType     );
  t->SetBranchAddress("HitSADC",       &HitSADC     );
  t->SetBranchAddress("StartChan",     &StartChan   );
  t->SetBranchAddress("EndChan",       &EndChan     );
  t->SetBranchAddress("FirstTimeHit",  &FirstTimeHit);
  t->SetBranchAddress("LastTimeHit",   &LastTimeHit );

  TH1D histo_posx("posx", ";XPosition [cm];Counts", 100, -400, 400);
  
  TCanvas c;
  c.Print("ev.pdf[");
  for (int i=0; i<t->GetEntries(); i++){
    t->GetEntry(i);

    int max=0;
    int min=1000000000;
    histo_posx.Fill(PosX_tree);
    for(size_t t = 0; t<HitChan->size(); t++) {
      try {
        if(HitChan->at(t)< min) min = HitChan->at(t);
        if(HitChan->at(t)> max) max = HitChan->at(t);
      } catch (...) {
        std::cout << "index " << t << " has a problem! 2" << "\n";
      }
    }

    TH2D histo_gen_type("", Form("E_{#nu} = %.1f MeV, SumADC = %.0f;Channel;Tick;Type",enu_tree*1000, sum_adc_tree), max-min+20, min-10, max+10, LastTimeHit-FirstTimeHit+20, FirstTimeHit-10, LastTimeHit+10);
    TH2D histo_sadc    ("", ";Channel;Tick;SADC", max-min+20, min-10, max+10, LastTimeHit-FirstTimeHit+20, FirstTimeHit-10, LastTimeHit+10);
    for(size_t t = 0; t<HitTime->size(); t++) {
      try {
        histo_gen_type.Fill(HitChan->at(t), HitTime->at(t), GenType->at(t));
        histo_sadc    .Fill(HitChan->at(t), HitTime->at(t), HitSADC->at(t)/100);
      } catch (...) {
        std::cout << "index " << t << " has a problem!" << "\n";
      }
    }
    c.Divide(1,2);
    c.cd(1);
    histo_gen_type.SetStats(0);
    histo_gen_type.Draw("COLZ");
    gPad->SetRightMargin(gPad->GetRightMargin()*1.5);
    gPad->SetBottomMargin(0);
    gPad->SetTicks(1);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    c.cd(2);
    histo_sadc.SetStats(0);
    gPad->SetRightMargin(gPad->GetRightMargin()*1.5);
    gPad->SetTopMargin(0);
    gPad->SetTicks(1);
    gPad->SetGridx(1);
    gPad->SetGridy(1);
    histo_sadc.Draw("COLZ");
    c.Print("ev.pdf");
    c.Clear();
  }
  histo_posx.Draw();
  c.Print("ev.pdf");
  c.Print("ev.pdf]");
  return 0;
}

#ifndef __CINT__
int main(int argc, char** argv)
{
  return MakeEventDisplay();
}
#endif

//____________________________________________________________________ 
//  
// EOF
//
