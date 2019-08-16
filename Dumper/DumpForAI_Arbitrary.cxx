#include "TProfile.h"
#include "TFile.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTree.h"
#include "TCanvas.h"

#include <iostream>
#include <unistd.h>
#include <limits>
#include <fstream>

#include "IO/ArbitraryAnaInputManager.hh"
#include "Algorithm/ClusterEngine.hh"
#include "Algorithm/Trigger.hh"


int main(int argc, char** argv){

  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  std::string InputFile = "/dune/data2/users/plasorak/singles_at_cathode_340cm/Generate_Electron_x340_gen_g4_detsim_gaushit_fasthit_trigprim_ana.root";
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
  TTree *hit_tree = (TTree*)f_Input->Get("arbitraryanatrigprim2000/SNSimTree");
  
  if (nEvent == 0) {
    nEvent = hit_tree->GetEntries();
  } else {
    nEvent = std::min(nEvent,(int)hit_tree->GetEntries());
  }

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
  std::vector<float>               * Hit_True_Energy      = NULL;             
  std::vector<float>               * Hit_True_nElec       = NULL;
  std::vector<int>                 * Hit_True_nIDEs       = NULL; 
  
  std::vector<float>               * True_E           = NULL;
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
  hit_tree->SetBranchAddress("Hit_True_Energy"          , &Hit_True_Energy          );
  hit_tree->SetBranchAddress("Hit_True_nElec"           , &Hit_True_nElec           );
  hit_tree->SetBranchAddress("Hit_True_nIDEs"           , &Hit_True_nIDEs           );
        
  hit_tree->SetBranchAddress("True_E"                   , &True_E                   );
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
  TH1D E_Lep("E_Lep",";ELep [MeV];Event",20,0,40);
  TEfficiency eff("Eff",";ELep [MeV];Efficiency",20,0,40);
  std::ofstream out("DumpForAI.txt");
  out << "Event,ENu,Type,ChanWidth,TimeWidth,NHits,SumADC"<<std::endl;
  
  for (int iEvent=0; iEvent<nEvent; iEvent++) {
    PrintProgress(iEvent, nEvent);
    hit_tree->GetEntry(iEvent);
    
    std::vector<WireHit*> vec_WireHit;
    for (size_t j=0; j<Hit_View->size(); ++j) {
      WireHit* hit = new WireHit((*Hit_View)[j],        (*Hit_True_GenType)[j],  (*Hit_Chan)[j],
                                 (*Hit_Time)[j],        (*Hit_SADC)[j],          (*Hit_RMS)[j],
                                 0,0,0,
                                 0.5, 0.5, 0.5,
                                 0.5, 0.5, 0.5,
                                 0,0);
      vec_WireHit.push_back(hit);
    }
  
    std::vector<WireCluster*> vec_WireCluster;
    clusteng.ClusterHits2(vec_WireHit, vec_WireCluster);
    wiretrigger.IsTriggering(vec_WireCluster);

    bool trigger = 0;
    for (size_t c=0; c<vec_WireCluster.size(); ++c) {
      WireCluster* clust = vec_WireCluster[c];
      if (clust->GetIsTriggering()) {
        if (clust->GetType() == 1) {
          trigger = 1;
          if (True_E->at(0) * 1000. < 10) {
            out << iEvent << ","
                << True_E->at(0)*1000.      << ","
                << 1                        << ","
                << clust->GetChannelWidth() << ","
                << clust->GetTimeWidth()    << ","
                << clust->GetNHit()         << ","
                << clust->GetHitSumADC()    << std::endl;
          }
        }
      }
    }
    std::cout << True_E->at(0) * 1000. << std::endl;
    E_Lep.Fill(True_E->at(0) * 1000. - 105.);
    eff.Fill(trigger, True_E->at(0) * 1000. - 105.);
    
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
  out.close();

  TCanvas c;
  c.Print("DumpForAI_Arb.pdf[");
  E_Lep.Draw();
  c.Print("DumpForAI_Arb.pdf");
  eff.Draw();
  c.Print("DumpForAI_Arb.pdf");
  c.Print("DumpForAI_Arb.pdf]");


}
