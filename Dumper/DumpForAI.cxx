#include <algorithm>    // std::min_element, std::max_element
#include <fstream>
#include <unistd.h>
#include <fstream>
#include <string>
#include <iostream>

#include "Utils/Helper.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TPad.h"
#include "TTree.h"
#include "TLegend.h"
#include "TEfficiency.h"
#include "TStyle.h"

int main(int argc, char** argv){

  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  std::string InputFile;
  std::string OutputFile;
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

  int    Cluster       ;
  int    Event         ;
  int    Config        ;
  double StartChan     ;
  double EndChan       ;
  double ChanWidth     ;
  double NChan         ;
  double Type          ;
  double NHits         ;
  double SumADC        ;
  double FirstTimeHit  ;
  double LastTimeHit   ;
  double TimeWidth     ;
  double E_deposited   ;
  double MC_UnderlyingE;
  double TrClusterPosX ;
  double TrClusterPosY ;
  double TrClusterPosZ ;
  double RecClusterPosX;
  double RecClusterPosY;
  double RecClusterPosZ;
  double pur_Other     ;
  double pur_SNnu      ;
  double pur_Ar39Lar   ;
  double pur_Ar42Lar   ;
  double pur_K42Lar    ;
  double pur_Kr85Lar   ;
  double pur_Rn222Lar  ;
  double pur_K40CPA    ;
  double pur_Ar42CPA   ;
  double pur_U238CPA   ;
  double pur_Co60APA   ;
  double pur_U238APA   ;
  double pur_Rn222PDS  ;
  double pur_Neutron   ;

  //double pur_APA       ;
  //double pur_CPA       ;
  //double pur_Ar39      ;
  //double pur_Neutron   ;
  //double pur_Krypton   ;
  //double pur_Polonium  ;
  //double pur_Radon     ;
  //double pur_Ar42      ;
  
  std::vector<int>    * HitView  = NULL;
  std::vector<int>    * HitGenType = NULL;
  std::vector<int>    * HitChan  = NULL;
  std::vector<double> * HitTime  = NULL;
  std::vector<double> * HitSADC  = NULL;
  std::vector<double> * HitRMS   = NULL;
  std::vector<double> * TrPosX   = NULL; 
  std::vector<double> * TrPosY   = NULL; 
  std::vector<double> * TrPosZ   = NULL;

  int MarleyIndex;
  int EventTrue;
  std::vector<double> * MarlTime = NULL;
  std::vector<double> * ENu      = NULL;
  std::vector<double> * ENu_Lep  = NULL;
  std::vector<double> * PosX     = NULL;
  std::vector<double> * PosY     = NULL;
  std::vector<double> * PosZ     = NULL;
  std::vector<double> * PosT     = NULL;
  std::vector<double> * DirX     = NULL;
  std::vector<double> * DirY     = NULL;
  std::vector<double> * DirZ     = NULL;
  
  TTree* t_Output_triggeredclusteredhits = (TTree*)f_Input->Get("ClusteredWireHit");
  t_Output_triggeredclusteredhits->SetBranchAddress("Cluster",        &Cluster      );
  t_Output_triggeredclusteredhits->SetBranchAddress("MarleyIndex",    &MarleyIndex  );
  t_Output_triggeredclusteredhits->SetBranchAddress("Event",          &Event        );
  t_Output_triggeredclusteredhits->SetBranchAddress("Config",         &Config       );
  t_Output_triggeredclusteredhits->SetBranchAddress("StartChan",      &StartChan    );
  t_Output_triggeredclusteredhits->SetBranchAddress("EndChan",        &EndChan      );
  t_Output_triggeredclusteredhits->SetBranchAddress("ChanWidth",      &ChanWidth    );
  t_Output_triggeredclusteredhits->SetBranchAddress("NChan",          &NChan        );
  t_Output_triggeredclusteredhits->SetBranchAddress("Type",           &Type         );
  t_Output_triggeredclusteredhits->SetBranchAddress("NHits",          &NHits        );
  t_Output_triggeredclusteredhits->SetBranchAddress("SumADC",         &SumADC       );
  t_Output_triggeredclusteredhits->SetBranchAddress("FirstTimeHit",   &FirstTimeHit );
  t_Output_triggeredclusteredhits->SetBranchAddress("LastTimeHit",    &LastTimeHit  );
  t_Output_triggeredclusteredhits->SetBranchAddress("TimeWidth",      &TimeWidth    );
  t_Output_triggeredclusteredhits->SetBranchAddress("E_deposited",    &E_deposited  );
  t_Output_triggeredclusteredhits->SetBranchAddress("MC_UnderlyingE", &MC_UnderlyingE);
  t_Output_triggeredclusteredhits->SetBranchAddress("RecClusterPosX", &RecClusterPosX);
  t_Output_triggeredclusteredhits->SetBranchAddress("RecClusterPosY", &RecClusterPosY);
  t_Output_triggeredclusteredhits->SetBranchAddress("RecClusterPosZ", &RecClusterPosZ);
  t_Output_triggeredclusteredhits->SetBranchAddress("TrClusterPosX",  &TrClusterPosX);
  t_Output_triggeredclusteredhits->SetBranchAddress("TrClusterPosY",  &TrClusterPosY);
  t_Output_triggeredclusteredhits->SetBranchAddress("TrClusterPosZ",  &TrClusterPosZ);
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Other"   ,   &pur_Other   );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_SNnu"    ,   &pur_SNnu    );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar39Lar",    &pur_Ar39Lar   );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar42Lar",    &pur_Ar42Lar   );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_K42Lar",     &pur_K42Lar    );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Kr85Lar",    &pur_Kr85Lar   );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Rn222Lar",   &pur_Rn222Lar  );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_K40CPA",     &pur_K40CPA    );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar42CPA",    &pur_Ar42CPA   );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_U238CPA",    &pur_U238CPA   );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Co60APA",    &pur_Co60APA   );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_U238APA",    &pur_U238APA   );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Rn222PDS",   &pur_Rn222PDS  );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Neutron",    &pur_Neutron   );

  //t_Output_triggeredclusteredhits->SetBranchAddress("pur_APA"     ,   &pur_APA     );
  //t_Output_triggeredclusteredhits->SetBranchAddress("pur_CPA"     ,   &pur_CPA     );
  //t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar39"    ,   &pur_Ar39    );
  //t_Output_triggeredclusteredhits->SetBranchAddress("pur_Neutron" ,   &pur_Neutron );
  //t_Output_triggeredclusteredhits->SetBranchAddress("pur_Krypton" ,   &pur_Krypton );
  //t_Output_triggeredclusteredhits->SetBranchAddress("pur_Polonium",   &pur_Polonium);
  //t_Output_triggeredclusteredhits->SetBranchAddress("pur_Radon"   ,   &pur_Radon   );
  //t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar42"    ,   &pur_Ar42    );
  t_Output_triggeredclusteredhits->SetBranchAddress("HitView",        &HitView);
  t_Output_triggeredclusteredhits->SetBranchAddress("GenType",        &HitGenType);
  t_Output_triggeredclusteredhits->SetBranchAddress("HitChan",        &HitChan);
  t_Output_triggeredclusteredhits->SetBranchAddress("HitTime",        &HitTime);
  t_Output_triggeredclusteredhits->SetBranchAddress("HitSADC",        &HitSADC);
  t_Output_triggeredclusteredhits->SetBranchAddress("HitRMS",         &HitRMS );
  t_Output_triggeredclusteredhits->SetBranchAddress("TrPosX",         &TrPosX );
  t_Output_triggeredclusteredhits->SetBranchAddress("TrPosY",         &TrPosY );
  t_Output_triggeredclusteredhits->SetBranchAddress("TrPosZ",         &TrPosZ );


  TTree* t_TrueInfo = (TTree*)f_Input->Get("TrueInfo");
  t_TrueInfo->SetBranchAddress("Event",    &EventTrue);
  t_TrueInfo->SetBranchAddress("MarlTime", &MarlTime );
  t_TrueInfo->SetBranchAddress("ENu",      &ENu      );
  t_TrueInfo->SetBranchAddress("ENu_Lep",  &ENu_Lep  );
  t_TrueInfo->SetBranchAddress("PosX",     &PosX     );
  t_TrueInfo->SetBranchAddress("PosY",     &PosY     );
  t_TrueInfo->SetBranchAddress("PosZ",     &PosZ     );
  t_TrueInfo->SetBranchAddress("PosT",     &PosT     );
  t_TrueInfo->SetBranchAddress("DirX",     &DirX     );
  t_TrueInfo->SetBranchAddress("DirY",     &DirY     );
  t_TrueInfo->SetBranchAddress("DirZ",     &DirZ     );

  if (nEvent == 0) {
    nEvent = t_TrueInfo->GetEntries();
  } else {
    nEvent = std::min(nEvent,(int)t_TrueInfo->GetEntries());
  }

  std::cout << "Running over " << nEvent << " events." << std::endl;
 
  std::map<int,int> map_Event_TrueEntry;
  int nMarleyEvent=0;
  for (int i=0; i<nEvent; ++i) {
    t_TrueInfo->GetEntry(i);
    map_Event_TrueEntry[EventTrue] = i;
    nMarleyEvent += (int)ENu->size();
  }
  
  std::cout << "nMarleyEvent " << nMarleyEvent << std::endl;

  std::ofstream out(OutputFile);
  out << "Event,ENu,Type,ChanWidth,TimeWidth,NHits,SumADC"<<std::endl;
  std::map<int,std::vector<int> > map_event_entry_wire;
  for(int i=0; i<t_Output_triggeredclusteredhits->GetEntries();++i) {
    PrintProgress(i,t_Output_triggeredclusteredhits->GetEntries());
    t_Output_triggeredclusteredhits->GetEntry(i);
    map_event_entry_wire[Event].push_back(i);
  }

  
  for(int i=0; i<t_Output_triggeredclusteredhits->GetEntries();++i) {
    t_Output_triggeredclusteredhits->GetEntry(i);
    t_TrueInfo->GetEntry(map_Event_TrueEntry[Event]);
    
    if (Type==1 && abs(PosX->at(0)) > 50 && abs(PosX->at(0)) < 100 && ENu_Lep->at(0)*1000. < 10) {
      out << Event <<","
          << ENu_Lep->at(0)*1000. << ","
          << Type      << ","
          << ChanWidth << ","
          << TimeWidth << ","
          << NHits     << ","
          << SumADC    << std::endl;
    }
    
  }
  out.close();

  return 0;

}
