#include "TProfile.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TCanvas.h"

#include <iostream>
#include <unistd.h>
#include <limits>

#include "IO/ArbitraryAnaInputManager.hh"
#include "Algorithm/ClusterEngine.hh"
#include "Algorithm/Trigger.hh"


int main(int argc, char** argv){

  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  std::string InputFile = "/pnfs/dune/persistent/users/rodriges/mcc11_snb_radiologicals_snana_defaultnoise_pdreco/output/11182119_971/prodmarley_nue_spectrum_radiological_timedep_hudepohl_11.2M_3perevent_dune10kt_1x2x6_10887276_816_20180822T202524_g4_detsim-9eac67e2-8ab0-407b-bf19-0acdb31ba8c8_defaultnoise_pdreco_snana.root";
  std::string OutputFile = "Test.pdf";
  int nEvent = 0;

  while ( (opt = getopt(argc, argv, "o:i:n:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'i':
      InputFile = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case 'o':
      OutputFile = optarg;
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
  TFile *f_Input = new TFile(InputFile.c_str(), "READ");
  TTree *hit_tree = (TTree*)f_Input->Get("snanatrigprim2000/SNSimTree");
  
  if (nEvent == 0) {
    nEvent = hit_tree->GetEntries();
  } else {
    nEvent = std::min(nEvent,(int)hit_tree->GetEntries());
  }

  double minX, minY, minZ;
  double maxX, maxY, maxZ;
  minX = minY = minZ = std::numeric_limits<double>::max();
  maxX = maxY = maxZ = std::numeric_limits<double>::min();
  
  TH2D* th2dhitMarleyIndex    = new TH2D("TH2DhitMarleyIndex",    ";Marley Index;nhits",           3,    0,    3, 40, 0, 40);
  TH2D* th2dhitXDistance      = new TH2D("TH2DhitXDistance",      ";X Distance [cm];nhits",       20, -500,  500, 20, 0, 20);
  TH2D* th2dhitYDistance      = new TH2D("TH2DhitYDistance",      ";Y Distance [cm];nhits",       20, -800,  800, 20, 0, 20);
  TH2D* th2dhitZDistance      = new TH2D("TH2DhitZDistance",      ";Z Distance [cm];nhits",       20, -100, 1600, 20, 0, 20);
  TH2D* th2dhitEnergyLepton   = new TH2D("TH2DhitEnergyLepton",   ";Energy Lepton [MeV];nhits",   20,    0,   30, 20, 0, 20);
  TH2D* th2dhitEnergyNeutrino = new TH2D("TH2DhitEnergyNeutrino", ";Energy Neutrino [MeV];nhits", 20,    0,   30, 20, 0, 20);

  TH1D* th1notrigNHit           = new TH1D("TH1DnotrigNHit",           ";NHits;nEvents",                 20,    0,   20);
  TH1D* th1notrigXDistance      = new TH1D("TH1DnotrigXDistance",      ";X Distance [cm];nEvents",       20, -500,  500);
  TH1D* th1notrigYDistance      = new TH1D("TH1DnotrigYDistance",      ";Y Distance [cm];nEvents",       20, -800,  800);
  TH1D* th1notrigZDistance      = new TH1D("TH1DnotrigZDistance",      ";Z Distance [cm];nEvents",       20, -100, 1600);
  TH1D* th1notrigTDistance      = new TH1D("TH1DnotrigTDistance",      ";T [tick];nEvents",              20,    0, 4000);
  TH1D* th1notrigEnergyLepton   = new TH1D("TH1DnotrigEnergyLepton",   ";Energy Lepton [MeV];nEvents",   20,    0,   30);
  TH1D* th1notrigEnergyNeutrino = new TH1D("TH1DnotrigEnergyNeutrino", ";Energy Neutrino [MeV];nEvents", 20,    0,   30);

  TH1D* th1trigNHit           = new TH1D("TH1DtrigNHit",           ";NHits;nEvents",                 20,    0,   20);
  TH1D* th1trigXDistance      = new TH1D("TH1DtrigXDistance",      ";X Distance [cm];nEvents",       20, -500,  500);
  TH1D* th1trigYDistance      = new TH1D("TH1DtrigYDistance",      ";Y Distance [cm];nEvents",       20, -800,  800);
  TH1D* th1trigZDistance      = new TH1D("TH1DtrigZDistance",      ";Z Distance [cm];nEvents",       20, -100, 1600);
  TH1D* th1trigTDistance      = new TH1D("TH1DtrigTDistance",      ";T [tick];nEvents",              20,    0, 4000);
  TH1D* th1trigEnergyLepton   = new TH1D("TH1DtrigEnergyLepton",   ";Energy Lepton [MeV];nEvents",   20,    0,   30);
  TH1D* th1trigEnergyNeutrino = new TH1D("TH1DtrigEnergyNeutrino", ";Energy Neutrino [MeV];nEvents", 20,    0,   30);

  TProfile* hitMarleyIndex    = new TProfile("hitMarleyIndex",    ";MarleyIndex;nhits" ,  3,    0,    3);
  TProfile* hitXDistance      = new TProfile("hitXDistance",      ";XDistance;nhits"   , 20, -500,  500);
  TProfile* hitYDistance      = new TProfile("hitYDistance",      ";YDistance;nhits"   , 20, -800,  800);
  TProfile* hitZDistance      = new TProfile("hitZDistance",      ";ZDistance;nhits"   , 20, -100, 1600);
  TProfile* hitEnergyLepton   = new TProfile("hitEnergyLepton",   ";EnergyLepton;nhits", 20,    0,   30);
  TProfile* hitEnergyNeutrino = new TProfile("hitEnergyNeutrino", ";EnergyLepton;nhits", 20,    0,   30);

  TH1D* histoEnergyLepton   = new TH1D("histoEnergyLepton",   ";EnergyLepton;", 20,    0,   30);
  TH1D* histoEnergyNeutrino = new TH1D("histoEnergyNeutrino", ";EnergyLepton;", 20,    0,   30);

  
  TEfficiency* effMarleyIndex    = new TEfficiency("EfficiencyMarleyIndex",    ";Marley Index;Efficiency",           3,    0,    3);
  TEfficiency* effXDistance      = new TEfficiency("EfficiencyXDistance",      ";X Distance [cm];Efficiency",       20, -500,  500);
  TEfficiency* effYDistance      = new TEfficiency("EfficiencyYDistance",      ";Y Distance [cm];Efficiency",       20, -800,  800);
  TEfficiency* effZDistance      = new TEfficiency("EfficiencyZDistance",      ";Z Distance [cm];Efficiency",       20, -100, 1600);
  TEfficiency* effEnergyLepton   = new TEfficiency("EfficiencyEnergyLepton",   ";Energy Lepton [MeV];Efficiency",   20,    0,   30);
  TEfficiency* effEnergyNeutrino = new TEfficiency("EfficiencyEnergyNeutrino", ";Energy Neutrino [MeV];Efficiency", 20,    0,   30);
  //TEfficiency* effNNoBTHit       = new TEfficiency("effNNoBTHit",              ";nNoBTHits;Average Efficiency",     20,    0, 1000);

  TH2D* th2dNoBThitXDistance      = new TH2D("TH2DNoBThitXDistance",      ";X Distance [cm];nNoBTHits",       20, -500,  500, 20, 0, 100);
  TH2D* th2dNoBThitYDistance      = new TH2D("TH2DNoBThitYDistance",      ";Y Distance [cm];nNoBTHits",       20, -800,  800, 20, 0, 100);
  TH2D* th2dNoBThitZDistance      = new TH2D("TH2DNoBThitZDistance",      ";Z Distance [cm];nNoBTHits",       20, -100, 1600, 20, 0, 100);
  TH2D* th2dNoBThitEnergyLepton   = new TH2D("TH2DNoBThitEnergyLepton",   ";Energy Lepton [MeV];nNoBTHits",   20,    0,   30, 20, 0, 100);
  TH2D* th2dNoBThitEnergyNeutrino = new TH2D("TH2DNoBThitEnergyNeutrino", ";Energy Neutrino [MeV];nNoBTHits", 20,    0,   30, 20, 0, 100);

  TProfile* hitNoBTXDistance      = new TProfile("hitNoBTXDistance",      ";XDistance;nNoBTHits"   , 20, -500,  500);
  TProfile* hitNoBTYDistance      = new TProfile("hitNoBTYDistance",      ";YDistance;nNoBTHits"   , 20, -800,  800);
  TProfile* hitNoBTZDistance      = new TProfile("hitNoBTZDistance",      ";ZDistance;nNoBTHits"   , 20, -100, 1600);
  TProfile* hitNoBTEnergyLepton   = new TProfile("hitNoBTEnergyLepton",   ";EnergyLepton;nNoBTHits", 20,    0,   30);
  TProfile* hitNoBTEnergyNeutrino = new TProfile("hitNoBTEnergyNeutrino", ";EnergyLepton;nNoBTHits", 20,    0,   30);

  std::vector<int>                 * Hit_View             = NULL;
  std::vector<int>                 * Hit_Size             = NULL;
  std::vector<int>                 * Hit_TPC              = NULL;
  std::vector<int>                 * Hit_Chan             = NULL;
  std::vector<float>               * Hit_Time             = NULL;
  std::vector<float>               * Hit_RMS              = NULL;
  std::vector<float>               * Hit_SADC             = NULL;
  std::vector<float>               * Hit_Int              = NULL;
  std::vector<float>               * Hit_Peak             = NULL;
  std::vector<int>                 * Hit_True_GenType     = NULL;
  std::vector<int>                 * Hit_True_MainTrID    = NULL;
  std::vector<float>               * Hit_True_EvEnergy    = NULL;
  std::vector<float>               * Hit_True_X           = NULL;                  
  std::vector<float>               * Hit_True_Y           = NULL;                  
  std::vector<float>               * Hit_True_Z           = NULL;                  
  std::vector<float>               * Hit_True_Energy      = NULL;             
  std::vector<float>               * Hit_True_nElec       = NULL;
  std::vector<int>                 * Hit_True_nIDEs       = NULL; 
  std::vector<int>                 * Hit_True_MarleyIndex = NULL;
  
  std::vector<int>                 * True_VertexChan  = NULL;
  std::vector<int>                 * True_Nu_Type     = NULL;
  std::vector<int>                 * True_Nu_Lep_Type = NULL;
  std::vector<int>                 * True_Mode        = NULL;
  std::vector<int>                 * True_CCNC        = NULL;
  std::vector<int>                 * True_HitNucleon  = NULL;
  std::vector<int>                 * True_Target      = NULL;
  std::vector<int>                 * True_MarlSample  = NULL;
  std::vector<float>               * True_MarlTime    = NULL;
  std::vector<float>               * True_MarlWeight  = NULL;
  std::vector<float>               * True_ENu         = NULL;
  std::vector<float>               * True_ENu_Lep     = NULL;
  std::vector<float>               * True_VertX       = NULL;
  std::vector<float>               * True_VertY       = NULL;
  std::vector<float>               * True_VertZ       = NULL;
  std::vector<float>               * True_VertexT     = NULL;
  std::vector<float>               * True_Px          = NULL;
  std::vector<float>               * True_Py          = NULL;
  std::vector<float>               * True_Pz          = NULL;
  std::vector<float>               * True_Dirx        = NULL;
  std::vector<float>               * True_Diry        = NULL;
  std::vector<float>               * True_Dirz        = NULL;
  std::vector<float>               * True_Time        = NULL;
                                                              
  hit_tree->SetBranchAddress("Hit_View"                 , &Hit_View                 );
  hit_tree->SetBranchAddress("Hit_Size"                 , &Hit_Size                 );
  hit_tree->SetBranchAddress("Hit_TPC"                  , &Hit_TPC                  );
  hit_tree->SetBranchAddress("Hit_Chan"                 , &Hit_Chan                 );
  hit_tree->SetBranchAddress("Hit_Time"                 , &Hit_Time                 );
  hit_tree->SetBranchAddress("Hit_RMS"                  , &Hit_RMS                  );
  hit_tree->SetBranchAddress("Hit_SADC"                 , &Hit_SADC                 );
  hit_tree->SetBranchAddress("Hit_Int"                  , &Hit_Int                  );
  hit_tree->SetBranchAddress("Hit_Peak"                 , &Hit_Peak                 );
  hit_tree->SetBranchAddress("Hit_True_GenType"         , &Hit_True_GenType         );
  hit_tree->SetBranchAddress("Hit_True_MainTrID"        , &Hit_True_MainTrID        );
  hit_tree->SetBranchAddress("Hit_True_EvEnergy"        , &Hit_True_EvEnergy        );
  hit_tree->SetBranchAddress("Hit_True_X"               , &Hit_True_X               );
  hit_tree->SetBranchAddress("Hit_True_Y"               , &Hit_True_Y               );
  hit_tree->SetBranchAddress("Hit_True_Z"               , &Hit_True_Z               );
  hit_tree->SetBranchAddress("Hit_True_Energy"          , &Hit_True_Energy          );
  hit_tree->SetBranchAddress("Hit_True_nElec"           , &Hit_True_nElec           );
  hit_tree->SetBranchAddress("Hit_True_nIDEs"           , &Hit_True_nIDEs           );
  hit_tree->SetBranchAddress("Hit_True_MarleyIndex"     , &Hit_True_MarleyIndex     );
        
  hit_tree->SetBranchAddress("True_VertexChan"          , &True_VertexChan          );
  hit_tree->SetBranchAddress("True_Nu_Type"             , &True_Nu_Type             );
  hit_tree->SetBranchAddress("True_Nu_Lep_Type"         , &True_Nu_Lep_Type         );
  hit_tree->SetBranchAddress("True_Mode"                , &True_Mode                );
  hit_tree->SetBranchAddress("True_CCNC"                , &True_CCNC                );
  hit_tree->SetBranchAddress("True_HitNucleon"          , &True_HitNucleon          );
  hit_tree->SetBranchAddress("True_Target"              , &True_Target              );
  hit_tree->SetBranchAddress("True_MarlSample"          , &True_MarlSample          );
  hit_tree->SetBranchAddress("True_MarlTime"            , &True_MarlTime            );
  hit_tree->SetBranchAddress("True_MarlWeight"          , &True_MarlWeight          );
  hit_tree->SetBranchAddress("True_ENu"                 , &True_ENu                 );
  hit_tree->SetBranchAddress("True_ENu_Lep"             , &True_ENu_Lep             );
  hit_tree->SetBranchAddress("True_VertX"               , &True_VertX               );
  hit_tree->SetBranchAddress("True_VertY"               , &True_VertY               );
  hit_tree->SetBranchAddress("True_VertZ"               , &True_VertZ               );
  hit_tree->SetBranchAddress("True_VertexT"             , &True_VertexT             );
  hit_tree->SetBranchAddress("True_Px"                  , &True_Px                  );
  hit_tree->SetBranchAddress("True_Py"                  , &True_Py                  );
  hit_tree->SetBranchAddress("True_Pz"                  , &True_Pz                  );
  hit_tree->SetBranchAddress("True_Dirx"                , &True_Dirx                );
  hit_tree->SetBranchAddress("True_Diry"                , &True_Diry                );
  hit_tree->SetBranchAddress("True_Dirz"                , &True_Dirz                );
  hit_tree->SetBranchAddress("True_Time"                , &True_Time                );
  
  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  clusteng.SetTimeWindow   (20);
  clusteng.SetChannelWidth (2);
  clusteng.SetTimeWindowOpt(0.2);
  clusteng.SetPositionOpt  (300);
  clusteng.SetBucketSize   (1);
      
  wiretrigger.SetWireNHitsMin    (6);
  wiretrigger.SetWireNChanMin    (2);
  wiretrigger.SetWireChanWidthMin(0);
  wiretrigger.SetWireSADCMin     (0);
  std::map<int, int> eff_more;
  int nInterestingEvent=0;
  int nDetectedEvent = 0;

  for (int iEvent=0; iEvent<nEvent; iEvent++) {
    PrintProgress(iEvent, nEvent);
    hit_tree->GetEntry(iEvent);
    std::map<int, int> nhit;

    nhit.clear();
    for (auto const& it:(*Hit_True_MarleyIndex)) {
      if (it >= 0) nhit[it]++;
    }
    if (nhit[0] == 0) std::cout << "No hit event " << iEvent << std::endl;
    for (auto const& it: (*Hit_True_GenType)) {
      if (it == kOther) nhit[-1]++;
    }

    if (nhit[-1] != 0) std::cout << "Not BT hits: " << nhit[-1] << std::endl;

    std::vector<WireHit*> vec_WireHit;
    for (size_t j=0; j<Hit_View->size(); ++j) {
      //std::cout << (*Hit_True_MarleyIndex)[j] << std::endl;
      WireHit* hit = new WireHit((*Hit_View)[j],        (*Hit_True_GenType)[j],  (*Hit_Chan)[j],
                                 (*Hit_Time)[j],        (*Hit_SADC)[j],          (*Hit_RMS)[j],
                                 (*Hit_True_Energy)[j], (*Hit_True_EvEnergy)[j], (*Hit_True_MainTrID)[j],
                                 0.5, 0.5, 0.5,
                                 0.5, 0.5, 0.5,
                                 (*Hit_True_MarleyIndex)[j], (*Hit_True_nElec)[j]);
      vec_WireHit.push_back(hit);
    }
    std::map<int, int> eff;
    eff[0] = 0;

    std::vector<WireCluster*> vec_WireCluster;
    clusteng.ClusterHits2(vec_WireHit, vec_WireCluster);
    wiretrigger.IsTriggering(vec_WireCluster);
    
    for (size_t c=0; c<vec_WireCluster.size(); ++c) {
      WireCluster* clust = vec_WireCluster[c];
      if (clust->GetIsTriggering()) {
        if (clust->GetType() == 1) {
          eff_more[iEvent]++;
          eff[clust->GetMarleyIndex()]++;
        }
      }
    }

    for (size_t i=0; i<True_VertX->size(); i++) {

      if (True_VertX->at(i) < minX) minX = True_VertX->at(i);
      if (True_VertY->at(i) < minY) minY = True_VertY->at(i);
      if (True_VertZ->at(i) < minZ) minZ = True_VertZ->at(i);
      if (True_VertX->at(i) > maxX) maxX = True_VertX->at(i);
      if (True_VertY->at(i) > maxY) maxY = True_VertY->at(i);
      if (True_VertZ->at(i) > maxZ) maxZ = True_VertZ->at(i);
            
      // if (std::abs(True_VertX->at(i)) < 200 &&
      //     std::abs(True_VertY->at(i)) < 300 &&
      //     True_VertZ->at(i) > 100 &&
      //     True_VertZ->at(i) < 1000) {
      if (true) {
        nInterestingEvent++;
        nDetectedEvent += eff[i]>0;
        if (eff[i]==0){
          std::cout << "Not triggering event : " << iEvent << std::endl;
          std::cout << "VertX : " << True_VertX->at(i)           << std::endl;
          std::cout << "VertY : " << True_VertY->at(i)           << std::endl;
          std::cout << "VertZ : " << True_VertZ->at(i)           << std::endl;
          std::cout << "VertT : " << True_VertexT->at(i)         << std::endl;
          std::cout << "ENu   : " << True_ENu_Lep->at(i) * 1000. << std::endl;
          std::cout << "ELep  : " << True_ENu    ->at(i) * 1000. << std::endl;
          th1notrigNHit          ->Fill(nhit[0]                     );
          th1notrigXDistance     ->Fill(True_VertX->at(i)           );
          th1notrigYDistance     ->Fill(True_VertY->at(i)           );
          th1notrigZDistance     ->Fill(True_VertZ->at(i)           );
          th1notrigTDistance     ->Fill(True_VertexT->at(i)         );
          th1notrigEnergyLepton  ->Fill(True_ENu_Lep->at(i) * 1000. );
          th1notrigEnergyNeutrino->Fill(True_ENu    ->at(i) * 1000. );
        } else {
            th1trigNHit          ->Fill(nhit[0]                     );
            th1trigXDistance     ->Fill(True_VertX->at(i)           );
            th1trigYDistance     ->Fill(True_VertY->at(i)           );
            th1trigZDistance     ->Fill(True_VertZ->at(i)           );
            th1trigTDistance     ->Fill(True_VertexT->at(i)         );
            th1trigEnergyLepton  ->Fill(True_ENu_Lep->at(i) * 1000. );
            th1trigEnergyNeutrino->Fill(True_ENu    ->at(i) * 1000. );
          }
            
        th2dhitMarleyIndex   ->Fill(i                          , nhit[i]);
        th2dhitXDistance     ->Fill(True_VertX->at(i)          , nhit[i]);        
        th2dhitYDistance     ->Fill(True_VertY->at(i)          , nhit[i]);        
        th2dhitZDistance     ->Fill(True_VertZ->at(i)          , nhit[i]);        
        th2dhitEnergyLepton  ->Fill(True_ENu_Lep->at(i) * 1000., nhit[i]);
        th2dhitEnergyNeutrino->Fill(True_ENu    ->at(i) * 1000., nhit[i]);
        hitMarleyIndex       ->Fill(i                          , nhit[i]);
        hitXDistance         ->Fill(True_VertX->at(i)          , nhit[i]);
        hitYDistance         ->Fill(True_VertY->at(i)          , nhit[i]);
        hitZDistance         ->Fill(True_VertZ->at(i)          , nhit[i]);
        hitEnergyLepton      ->Fill(True_ENu_Lep->at(i) * 1000., nhit[i]);
        hitEnergyNeutrino    ->Fill(True_ENu    ->at(i) * 1000., nhit[i]);
        effMarleyIndex       ->Fill(eff[i]>0, i                          );
        effXDistance         ->Fill(eff[i]>0, True_VertX->at(i)          );
        effYDistance         ->Fill(eff[i]>0, True_VertY->at(i)          );
        effZDistance         ->Fill(eff[i]>0, True_VertZ->at(i)          );
        effEnergyLepton      ->Fill(eff[i]>0, True_ENu_Lep->at(i) * 1000.);
        effEnergyNeutrino    ->Fill(eff[i]>0, True_ENu    ->at(i) * 1000.);
        histoEnergyLepton    ->Fill(True_ENu_Lep->at(i) * 1000.);
        histoEnergyNeutrino  ->Fill(True_ENu    ->at(i) * 1000.);
      }
    }

    for (auto& it: vec_WireHit) {
      delete it;
      it = NULL;
    }
    vec_WireHit.clear();
    for (auto& it: vec_WireCluster){
      delete it;
      it = NULL;
    }
    vec_WireCluster.clear();
  }

  
  hitMarleyIndex   ->SetLineWidth(2);
  hitXDistance     ->SetLineWidth(2);
  hitYDistance     ->SetLineWidth(2);
  hitZDistance     ->SetLineWidth(2);
  hitEnergyLepton  ->SetLineWidth(2);
  hitEnergyNeutrino->SetLineWidth(2);

  
  hitNoBTXDistance     ->SetLineWidth(2);
  hitNoBTYDistance     ->SetLineWidth(2);
  hitNoBTZDistance     ->SetLineWidth(2);
  hitNoBTEnergyLepton  ->SetLineWidth(2);
  hitNoBTEnergyNeutrino->SetLineWidth(2);
    
  hitMarleyIndex   ->SetLineColor(kBlack);
  hitXDistance     ->SetLineColor(kBlack);
  hitYDistance     ->SetLineColor(kBlack);
  hitZDistance     ->SetLineColor(kBlack);
  hitEnergyLepton  ->SetLineColor(kBlack);
  hitEnergyNeutrino->SetLineColor(kBlack);

  hitNoBTXDistance     ->SetLineColor(kBlack);
  hitNoBTYDistance     ->SetLineColor(kBlack);
  hitNoBTZDistance     ->SetLineColor(kBlack);
  hitNoBTEnergyLepton  ->SetLineColor(kBlack);
  hitNoBTEnergyNeutrino->SetLineColor(kBlack);

  histoEnergyLepton  ->SetLineWidth(2);
  histoEnergyNeutrino->SetLineWidth(2);

  histoEnergyLepton  ->SetLineColor(kRed);
  histoEnergyNeutrino->SetLineColor(kRed);

  histoEnergyLepton  ->Scale(18. / histoEnergyLepton  ->GetMaximum());
  histoEnergyNeutrino->Scale(18. / histoEnergyNeutrino->GetMaximum());
// for (auto const& it: eff_more){
  //   if(it.second>0)nDetectedEvent ++;
  // }
  std::cout <<"efficiency " << (double)nDetectedEvent/nInterestingEvent << std::endl;

    
  TCanvas* c = new TCanvas();
  c->Print((OutputFile+"[").c_str());
  th2dhitMarleyIndex   ->Draw("COLZ"); hitMarleyIndex   ->Draw("SAME"); c->Print(OutputFile.c_str());
  th2dhitXDistance     ->Draw("COLZ"); hitXDistance     ->Draw("SAME"); c->Print(OutputFile.c_str());
  th2dhitYDistance     ->Draw("COLZ"); hitYDistance     ->Draw("SAME"); c->Print(OutputFile.c_str());
  th2dhitZDistance     ->Draw("COLZ"); hitZDistance     ->Draw("SAME"); c->Print(OutputFile.c_str());  
  th2dhitEnergyLepton  ->Draw("COLZ"); hitEnergyLepton  ->Draw("SAME"); histoEnergyLepton  ->Draw("SAME"); c->Print(OutputFile.c_str());
  th2dhitEnergyNeutrino->Draw("COLZ"); hitEnergyNeutrino->Draw("SAME"); histoEnergyNeutrino->Draw("SAME"); c->Print(OutputFile.c_str());
  effMarleyIndex   ->Draw(); c->Print(OutputFile.c_str());
  effXDistance     ->Draw(); c->Print(OutputFile.c_str());
  effYDistance     ->Draw(); c->Print(OutputFile.c_str());
  effZDistance     ->Draw(); c->Print(OutputFile.c_str());
  effEnergyLepton  ->Draw(); c->Print(OutputFile.c_str());
  effEnergyNeutrino->Draw(); c->Print(OutputFile.c_str());

  th2dNoBThitXDistance     ->Draw("COLZ"); hitNoBTXDistance     ->Draw("SAME"); c->Print(OutputFile.c_str());
  th2dNoBThitYDistance     ->Draw("COLZ"); hitNoBTYDistance     ->Draw("SAME"); c->Print(OutputFile.c_str());
  th2dNoBThitZDistance     ->Draw("COLZ"); hitNoBTZDistance     ->Draw("SAME"); c->Print(OutputFile.c_str());  
  th2dNoBThitEnergyLepton  ->Draw("COLZ"); hitNoBTEnergyLepton  ->Draw("SAME"); c->Print(OutputFile.c_str());
  th2dNoBThitEnergyNeutrino->Draw("COLZ"); hitNoBTEnergyNeutrino->Draw("SAME"); c->Print(OutputFile.c_str());


  th1trigNHit          ->SetLineColor(kRed);
  th1trigXDistance     ->SetLineColor(kRed);
  th1trigYDistance     ->SetLineColor(kRed);
  th1trigZDistance     ->SetLineColor(kRed);
  th1trigTDistance     ->SetLineColor(kRed);
  th1trigEnergyLepton  ->SetLineColor(kRed);
  th1trigEnergyNeutrino->SetLineColor(kRed);

  th1notrigNHit          ->Draw(); th1trigNHit          ->Draw("SAME"); c->Print(OutputFile.c_str());
  th1notrigXDistance     ->Draw(); th1trigXDistance     ->Draw("SAME"); c->Print(OutputFile.c_str());
  th1notrigYDistance     ->Draw(); th1trigYDistance     ->Draw("SAME"); c->Print(OutputFile.c_str());
  th1notrigZDistance     ->Draw(); th1trigZDistance     ->Draw("SAME"); c->Print(OutputFile.c_str());
  th1notrigTDistance     ->Draw(); th1trigTDistance     ->Draw("SAME"); c->Print(OutputFile.c_str());
  th1notrigEnergyLepton  ->Draw(); th1trigEnergyLepton  ->Draw("SAME"); c->Print(OutputFile.c_str());
  th1notrigEnergyNeutrino->Draw(); th1trigEnergyNeutrino->Draw("SAME"); c->Print(OutputFile.c_str());

  
  c->Print((OutputFile+"]").c_str());

  std::cout << "minX " << minX << std::endl;
  std::cout << "minY " << minY << std::endl;
  std::cout << "minZ " << minZ << std::endl;
  std::cout << "maxX " << maxX << std::endl;
  std::cout << "maxY " << maxY << std::endl;
  std::cout << "maxZ " << maxZ << std::endl;
 
}
