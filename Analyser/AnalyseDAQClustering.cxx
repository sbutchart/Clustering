#include <algorithm>    // std::min_element, std::max_element
#include <iostream>
#include <fstream>

#include "Utils/Helper.h"
#include "Utils/CLI11.hpp"
//#include "IO/OutputManager.hh"

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
#include "TVector3.h"

TProfile* SetUpTProfileGenType(std::string name, std::string title) {
  Helper h;
  TProfile* p_ = new TProfile(name.c_str(), title.c_str(), h.dyn_GenName.size(), 0, (double)h.dyn_GenName.size());
  for(auto const& it : h.dyn_ShortGenName)
    p_->GetXaxis()->SetBinLabel(it.first+1, it.second.c_str());
  return p_;
  
}

std::map<int, int> GetMapOfHit(std::vector<int> const* dyn_GenType){
  std::map<int,int> map_;
  for(std::vector<int>::const_iterator it=dyn_GenType->begin(); it!=dyn_GenType->end(); ++it)
    map_[*it]++;
  return map_;
}

int main(int argc, char** argv){
  CLI::App app{"A program to produce a pdf output containing various interesting things that have to be checked out when running the clustering"};

  int nHitCut=-1, RequestedConfig=0;
  std::string InputFile;
  std::string OutputFile;
  int nEvent = 0;
  app.add_option("-i,--input",  InputFile,       "Input filename (root file, the output file of RunDAQClustering)")->required();
  app.add_option("-c,--config", RequestedConfig, "What configuration of the clustering to analyse")->required();
  app.add_option("-o,--output", OutputFile,      "Output file name (with pdf extension)")->required();
  app.add_option("-e,--event",  nEvent,          "The number of events to run on (not sure this is working properly, use with caution)");
  app.add_option("--hitcut",    nHitCut,         "An extra hit cut that can be applied here. Minimum number of hits a cluster should have to be considered");
  CLI11_PARSE(app, argc, argv);

  if (InputFile == "") {
    std::cerr << "Need to provide an input file" << std::endl;
    exit(1);
  }

  TFile *f_Input = new TFile(InputFile.c_str(), "READ");
     
  if (!f_Input->IsOpen()) {
    std::cerr << "The file " << InputFile.c_str() << " does not exist." << std::endl;
    exit(1);
  }


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
//  double pur_Other     ;
//  double pur_SNnu      ;
//  double pur_APA       ;
//  double pur_CPA       ;
//  double pur_Ar39      ;
//  double pur_Neutron   ;
//  double pur_Krypton   ;
//  double pur_Polonium  ;
//  double pur_Radon     ;
//  double pur_Ar42      ;

  // DYNAMIC BACKGROUNDS
  std::map<std::string, int> ID_map;
  std::vector<std::string> AllGenTypeDynamic;
  std::vector<int> pur_dynamic;
  std::cout << "[Analyser] Dynamic: Loading IDs" << std::endl;
  std::string delim = "/";
//  std::string tree_name_token = InputFile.substr(0, InputFile.find(delim));
  std::string ID_tree = "fIDs";
    
  if (f_Input->Get( ID_tree.c_str() )) {
    TTree *t_IDs = (TTree*)f_Input->Get( ID_tree.c_str() );
    TObjArray *branchList; 
    branchList  = t_IDs->GetListOfBranches();
    int nBranch = t_IDs->GetNbranches();
    TString IDtreenames[nBranch];
  
    std::cout << "ID trees: " << nBranch << std::endl;
    for(int i=0;i<nBranch;i++){
      IDtreenames[i] = branchList->At(i)->GetName();
 
      int temp_id;
      t_IDs->SetBranchAddress(IDtreenames[i], &temp_id);
      t_IDs->GetEntry(0);
      std::string delimiter = "_";
      std::string temp_string = IDtreenames[i].Data();
      std::string token = temp_string.substr(0, temp_string.find(delimiter));
      std::pair<std::string, int> temp_pair {token, temp_id};
      ID_map.insert( temp_pair );
      AllGenTypeDynamic.push_back(token);
    }
    std::pair<std::string, int> temp_pair_allbckg {"AllBackground", 99};
    ID_map.insert( temp_pair_allbckg );
    AllGenTypeDynamic.push_back("AllBackground");
    std::pair<std::string, int> temp_pair_all {"All", 100};
    ID_map.insert( temp_pair_all );
    AllGenTypeDynamic.push_back("All");

   std::cout << "[Analyser] Loaded Dynamic IDs" << std::endl;
   for (auto const& x : ID_map){
     std::cout << x.first << " : " << x.second << std::endl;
   }
   // setup some vars in helper that will be used throughout
   SetDynamicVars(ID_map); 
 } else {
   std::cerr << "The requested tree 'fIDs' does not exist in file " << InputFile << std::endl;
 }
  
 
  std::vector<int>    * HitView    = NULL;
  std::vector<int>    * HitGenType = NULL;
  std::vector<int>    * HitChan    = NULL;
  std::vector<double> * HitTime    = NULL;
  std::vector<double> * HitSADC    = NULL;
  std::vector<double> * HitRMS     = NULL;
  std::vector<double> * TrPosX     = NULL; 
  std::vector<double> * TrPosY     = NULL; 
  std::vector<double> * TrPosZ     = NULL;

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
  t_Output_triggeredclusteredhits->SetBranchAddress("Cluster",        &Cluster       );
  t_Output_triggeredclusteredhits->SetBranchAddress("MarleyIndex",    &MarleyIndex   );
  t_Output_triggeredclusteredhits->SetBranchAddress("Event",          &Event         );
  t_Output_triggeredclusteredhits->SetBranchAddress("Config",         &Config        );
  t_Output_triggeredclusteredhits->SetBranchAddress("StartChan",      &StartChan     );
  t_Output_triggeredclusteredhits->SetBranchAddress("EndChan",        &EndChan       );
  t_Output_triggeredclusteredhits->SetBranchAddress("ChanWidth",      &ChanWidth     );
  t_Output_triggeredclusteredhits->SetBranchAddress("NChan",          &NChan         );
  t_Output_triggeredclusteredhits->SetBranchAddress("Type",           &Type          );
  t_Output_triggeredclusteredhits->SetBranchAddress("NHits",          &NHits         );
  t_Output_triggeredclusteredhits->SetBranchAddress("SumADC",         &SumADC        );
  t_Output_triggeredclusteredhits->SetBranchAddress("FirstTimeHit",   &FirstTimeHit  );
  t_Output_triggeredclusteredhits->SetBranchAddress("LastTimeHit",    &LastTimeHit   );
  t_Output_triggeredclusteredhits->SetBranchAddress("TimeWidth",      &TimeWidth     );
  t_Output_triggeredclusteredhits->SetBranchAddress("E_deposited",    &E_deposited   );
  t_Output_triggeredclusteredhits->SetBranchAddress("MC_UnderlyingE", &MC_UnderlyingE);
  t_Output_triggeredclusteredhits->SetBranchAddress("RecClusterPosX", &RecClusterPosX);
  t_Output_triggeredclusteredhits->SetBranchAddress("RecClusterPosY", &RecClusterPosY);
  t_Output_triggeredclusteredhits->SetBranchAddress("RecClusterPosZ", &RecClusterPosZ);
  t_Output_triggeredclusteredhits->SetBranchAddress("TrClusterPosX",  &TrClusterPosX );
  t_Output_triggeredclusteredhits->SetBranchAddress("TrClusterPosY",  &TrClusterPosY );
  t_Output_triggeredclusteredhits->SetBranchAddress("TrClusterPosZ",  &TrClusterPosZ );


  pur_dynamic.resize(ID_map.size());
  for (auto & x : ID_map) {
    std::stringstream branch;
    branch << "pur_" << x.first;
    //std::cout << branch.str() << std::endl;
    double branch_double = static_cast <double> (pur_dynamic[x.second]);
    t_Output_triggeredclusteredhits->SetBranchAddress(branch.str().c_str(), &branch_double );
      }

//  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Other",      &pur_Other     );
//  t_Output_triggeredclusteredhits->SetBranchAddress("pur_SNnu",       &pur_SNnu      );
//  t_Output_triggeredclusteredhits->SetBranchAddress("pur_APA",        &pur_APA       );
//  t_Output_triggeredclusteredhits->SetBranchAddress("pur_CPA",        &pur_CPA       );
//  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar39",       &pur_Ar39      );
//  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Neutron",    &pur_Neutron   );
//  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Krypton",    &pur_Krypton   );
//  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Polonium",   &pur_Polonium  );
//  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Radon",      &pur_Radon     );
//  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar42",       &pur_Ar42      );
  t_Output_triggeredclusteredhits->SetBranchAddress("HitView",        &HitView       );
  t_Output_triggeredclusteredhits->SetBranchAddress("GenType",        &HitGenType    );
  t_Output_triggeredclusteredhits->SetBranchAddress("HitChan",        &HitChan       );
  t_Output_triggeredclusteredhits->SetBranchAddress("HitTime",        &HitTime       );
  t_Output_triggeredclusteredhits->SetBranchAddress("HitSADC",        &HitSADC       );
  t_Output_triggeredclusteredhits->SetBranchAddress("HitRMS",         &HitRMS        );
  t_Output_triggeredclusteredhits->SetBranchAddress("TrPosX",         &TrPosX        );
  t_Output_triggeredclusteredhits->SetBranchAddress("TrPosY",         &TrPosY        );
  t_Output_triggeredclusteredhits->SetBranchAddress("TrPosZ",         &TrPosZ        );


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
  int nMarleyPerEvent=-1;
  for (int i=0; i<nEvent; ++i) {
    t_TrueInfo->GetEntry(i);
    map_Event_TrueEntry[EventTrue] = i;
    nMarleyEvent += (int)ENu->size();
    if (i==0) {
      nMarleyPerEvent = (int)ENu->size();
    } else {
      if (nMarleyPerEvent != (int)ENu->size()) {
        std::cerr << "You are running with a mix of file containing different number of marley event, which this cannot process." << std::endl;
        std::cerr << "Exiting" << std::endl;
        exit(1);
      }
    }
  }
  std::cout << "nMarleyEvent " << nMarleyEvent << std::endl;
  TCanvas c;
  c.Print((OutputFile+"[").c_str());
  TProfile* p_gentype_sign_wire      = SetUpTProfileGenType("p_gentype_sign_wire", ";;average number of hits in cluster");
  TProfile* p_gentype_back_wire      = SetUpTProfileGenType("p_gentype_back_wire", ";;average number of hits in cluster");
  TProfile* p_gentype_sign_neut_wire = SetUpTProfileGenType("p_gentype_sign_neut_wire", ";;average number of hits in cluster");
  TProfile* p_gentype_back_neut_wire = SetUpTProfileGenType("p_gentype_back_neut_wire", ";;average number of hits in cluster");
  p_gentype_sign_wire->SetLineColor(kRed);
  p_gentype_back_wire->SetLineColor(kBlue);
  p_gentype_sign_neut_wire->SetLineColor(kRed);
  p_gentype_back_neut_wire->SetLineColor(kBlue);
  p_gentype_sign_wire->SetLineStyle(1);
  p_gentype_back_wire->SetLineStyle(1);
  p_gentype_sign_neut_wire->SetLineStyle(1);
  p_gentype_back_neut_wire->SetLineStyle(1);
  p_gentype_sign_wire->SetLineWidth(2);
  p_gentype_back_wire->SetLineWidth(2);
  p_gentype_sign_neut_wire->SetLineWidth(2);
  p_gentype_back_neut_wire->SetLineWidth(2);

  Helper h;

  TH1D* h_rate_back = new TH1D("rates", ";;Rate [Hz]", h.dyn_GenName.size(),-0.5, (double)h.dyn_GenName.size()-0.5);
  h_rate_back->SetLineWidth(2);
  for(auto const& it : h.dyn_ShortGenName)
    h_rate_back->GetXaxis()->SetBinLabel(it.first+1, it.second.c_str());
  
  TH1D* h_nhit_sign_wire = new TH1D("h_nhit_sign_wire", ";n Hits;Rate [Hz]", 50, 0, 50);
  TH1D* h_nhit_back_wire = new TH1D("h_nhit_back_wire", ";n Hits;Rate [Hz]", 50, 0, 50);
  h_nhit_sign_wire->SetLineColor(kRed);
  h_nhit_back_wire->SetLineColor(kBlue);
  h_nhit_sign_wire->SetLineStyle(1);
  h_nhit_back_wire->SetLineStyle(1);
  h_nhit_sign_wire->SetLineWidth(2);
  h_nhit_back_wire->SetLineWidth(2);

  TH1D* h_sadc_sign_wire = new TH1D("h_sadc_sign_wire", ";SADC;Rate [Hz]", 100, 0, 10000);
  TH1D* h_sadc_back_wire = new TH1D("h_sadc_back_wire", ";SADC;Rate [Hz]", 100, 0, 10000);
  h_sadc_sign_wire->SetLineColor(kRed);
  h_sadc_back_wire->SetLineColor(kBlue);
  h_sadc_sign_wire->SetLineStyle(1);
  h_sadc_back_wire->SetLineStyle(1);
  h_sadc_sign_wire->SetLineWidth(2);
  h_sadc_back_wire->SetLineWidth(2);

  TH1D* h_time_sign_wire = new TH1D("h_time_sign_wire", ";Time;Rate [Hz]", 100, 0, 100);
  TH1D* h_time_back_wire = new TH1D("h_time_back_wire", ";Time;Rate [Hz]", 100, 0, 100);
  h_time_sign_wire->SetLineColor(kRed);
  h_time_back_wire->SetLineColor(kBlue);
  h_time_sign_wire->SetLineStyle(1);
  h_time_back_wire->SetLineStyle(1);
  h_time_sign_wire->SetLineWidth(2);
  h_time_back_wire->SetLineWidth(2);

  TH1D* h_sadc_hits_sign_wire = new TH1D("h_sadc_hits_sign_wire", ";SADC;n Hits / cluster", 100, 0, 5000);
  TH1D* h_sadc_hits_back_wire = new TH1D("h_sadc_hits_back_wire", ";SADC;n Hits / cluster", 100, 0, 5000);
  h_sadc_hits_sign_wire->SetLineColor(kRed);
  h_sadc_hits_back_wire->SetLineColor(kBlue);
  h_sadc_hits_sign_wire->SetLineStyle(1);
  h_sadc_hits_back_wire->SetLineStyle(1);
  h_sadc_hits_sign_wire->SetLineWidth(2);
  h_sadc_hits_back_wire->SetLineWidth(2);

  TH1D* h_nchan_sign_wire = new TH1D("h_nchan_sign_wire", ";Time;Rate [Hz]", 20, 0, 20);
  TH1D* h_nchan_back_wire = new TH1D("h_nchan_back_wire", ";Time;Rate [Hz]", 20, 0, 20);
  h_nchan_sign_wire->SetLineColor(kRed);
  h_nchan_back_wire->SetLineColor(kBlue);
  h_nchan_sign_wire->SetLineStyle(1);
  h_nchan_back_wire->SetLineStyle(1);
  h_nchan_sign_wire->SetLineWidth(2);
  h_nchan_back_wire->SetLineWidth(2);

  TH2D* h_nhit_sadc_sign_wire  = new TH2D("h_nhit_sadc_sign_wire",  ";n Hits;SADC"      ,  50, 0,    50, 100, 0, 10000);
  TH2D* h_nhit_sadc_back_wire  = new TH2D("h_nhit_sadc_back_wire",  ";n Hits;SADC"      ,  50, 0,    50, 100, 0, 10000);
  TH2D* h_sadc_time_sign_wire  = new TH2D("h_sadc_time_sign_wire",  ";SADC;Time"        , 100, 0, 10000, 100, 0,   100);
  TH2D* h_sadc_time_back_wire  = new TH2D("h_sadc_time_back_wire",  ";SADC;Time"        , 100, 0, 10000, 100, 0,   100);
  TH2D* h_time_nhit_sign_wire  = new TH2D("h_time_nhit_sign_wire",  ";Time;n Hits"      , 100, 0,   100,  50, 0,    50);
  TH2D* h_time_nhit_back_wire  = new TH2D("h_time_nhit_back_wire",  ";Time;n Hits"      , 100, 0,   100,  50, 0,    50);
  TH2D* h_nchan_sadc_sign_wire = new TH2D("h_nchan_sadc_sign_wire", ";n Channels;SADC"  ,  10, 0,    10, 100, 0, 10000);
  TH2D* h_nchan_sadc_back_wire = new TH2D("h_nchan_sadc_back_wire", ";n Channels;SADC"  ,  10, 0,    10, 100, 0, 10000);
  TH2D* h_nchan_time_sign_wire = new TH2D("h_nchan_time_sign_wire", ";n Channels;Time"  ,  10, 0,    10, 100, 0,   100);
  TH2D* h_nchan_time_back_wire = new TH2D("h_nchan_time_back_wire", ";n Channels;Time"  ,  10, 0,    10, 100, 0,   100);
  TH2D* h_nchan_nhit_sign_wire = new TH2D("h_nchan_nhit_sign_wire", ";n Channels;n Hits",  10, 0,    10,  50, 0,    50);
  TH2D* h_nchan_nhit_back_wire = new TH2D("h_nchan_nhit_back_wire", ";n Channels;n Hits",  10, 0,    10,  50, 0,    50);
  
  t_Output_triggeredclusteredhits->Project("h_nhit_sign_wire"     , "NHits",     Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nhit_sign_wire"      << std::endl;
  t_Output_triggeredclusteredhits->Project("h_nhit_back_wire"     , "NHits",     Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nhit_back_wire"      << std::endl;
  t_Output_triggeredclusteredhits->Project("h_sadc_sign_wire"     , "SumADC",    Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_sadc_sign_wire"      << std::endl;
  t_Output_triggeredclusteredhits->Project("h_sadc_back_wire"     , "SumADC",    Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_sadc_back_wire"      << std::endl;
  t_Output_triggeredclusteredhits->Project("h_time_sign_wire"     , "TimeWidth", Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_time_sign_wire"      << std::endl;
  t_Output_triggeredclusteredhits->Project("h_time_back_wire"     , "TimeWidth", Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_time_back_wire"      << std::endl;
  t_Output_triggeredclusteredhits->Project("h_nchan_sign_wire"    , "NChan",     Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nchan_sign_wire"     << std::endl;
  t_Output_triggeredclusteredhits->Project("h_nchan_back_wire"    , "NChan",     Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nchan_back_wire"     << std::endl;
  t_Output_triggeredclusteredhits->Project("h_sadc_hits_sign_wire", "HitSADC",   Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_sadc_hits_back_wire" << std::endl;
  t_Output_triggeredclusteredhits->Project("h_sadc_hits_back_wire", "HitSADC",   Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_sadc_hits_back_wire" << std::endl;
  
  
  t_Output_triggeredclusteredhits->Project("h_nhit_sadc_sign_wire" , "SumADC:NHits",     Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nhit_sadc_sign_wire"  << std::endl;
  t_Output_triggeredclusteredhits->Project("h_nhit_sadc_back_wire" , "SumADC:NHits",     Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nhit_sadc_back_wire"  << std::endl;
  t_Output_triggeredclusteredhits->Project("h_sadc_time_sign_wire" , "TimeWidth:SumADC", Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_sadc_time_sign_wire"  << std::endl;
  t_Output_triggeredclusteredhits->Project("h_sadc_time_back_wire" , "TimeWidth:SumADC", Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_sadc_time_back_wire"  << std::endl;
  t_Output_triggeredclusteredhits->Project("h_time_nhit_sign_wire" , "NHits:TimeWidth",  Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_time_nhit_sign_wire"  << std::endl;
  t_Output_triggeredclusteredhits->Project("h_time_nhit_back_wire" , "NHits:TimeWidth",  Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_time_nhit_back_wire"  << std::endl;
  t_Output_triggeredclusteredhits->Project("h_nchan_sadc_sign_wire", "SumADC:NChan",     Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nchan_sadc_sign_wire" << std::endl;
  t_Output_triggeredclusteredhits->Project("h_nchan_sadc_back_wire", "SumADC:NChan",     Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nchan_sadc_back_wire" << std::endl;
  t_Output_triggeredclusteredhits->Project("h_nchan_time_sign_wire", "TimeWidth:NChan",  Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nchan_time_sign_wire" << std::endl;
  t_Output_triggeredclusteredhits->Project("h_nchan_time_back_wire", "TimeWidth:NChan",  Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nchan_time_back_wire" << std::endl;
  t_Output_triggeredclusteredhits->Project("h_nchan_nhit_sign_wire", "NHits:NChan",      Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nchan_nhit_sign_wire" << std::endl;
  t_Output_triggeredclusteredhits->Project("h_nchan_nhit_back_wire", "NHits:NChan",      Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut)); std::cout << "Projected h_nchan_nhit_back_wire" << std::endl;
  if (h_nhit_sign_wire->GetEntries())
    h_sadc_hits_sign_wire ->Scale(1. / (double) h_nhit_sign_wire->GetEntries());
  h_nhit_sign_wire      ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_sadc_sign_wire      ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_time_sign_wire      ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_nchan_sign_wire     ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_nhit_sadc_sign_wire ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_sadc_time_sign_wire ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_time_nhit_sign_wire ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_nchan_sadc_sign_wire->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_nchan_time_sign_wire->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_nchan_nhit_sign_wire->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  
  if (h_nhit_back_wire->GetEntries())
    h_sadc_hits_back_wire ->Scale(1. / (double) h_nhit_back_wire->GetEntries());
  h_nhit_back_wire      ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_sadc_back_wire      ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_time_back_wire      ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_nchan_back_wire     ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_nhit_sadc_back_wire ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_sadc_time_back_wire ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_time_nhit_back_wire ->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_nchan_sadc_back_wire->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_nchan_time_back_wire->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);
  h_nchan_nhit_back_wire->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);

  gPad->SetTicks();
  gPad->SetGridx();
  gPad->SetGridy();
  PlotThese2Histos(h_sadc_hits_back_wire , h_sadc_hits_sign_wire , c, OutputFile, false, true);
  PlotThese2Histos(h_nhit_sign_wire      , h_nhit_back_wire      , c, OutputFile, false, true);
  PlotThese2Histos(h_sadc_sign_wire      , h_sadc_back_wire      , c, OutputFile, false, true);
  PlotThese2Histos(h_time_sign_wire      , h_time_back_wire      , c, OutputFile, false, true);
  PlotThese2Histos(h_nchan_sign_wire     , h_nchan_back_wire     , c, OutputFile, false, true);
  PlotThese2Histos(h_nhit_sadc_sign_wire , h_nhit_sadc_back_wire , c, OutputFile, false, false);
  PlotThese2Histos(h_sadc_time_sign_wire , h_sadc_time_back_wire , c, OutputFile, false, false);
  PlotThese2Histos(h_time_nhit_sign_wire , h_time_nhit_back_wire , c, OutputFile, false, false);
  PlotThese2Histos(h_nchan_sadc_sign_wire, h_nchan_sadc_back_wire, c, OutputFile, false, false);
  PlotThese2Histos(h_nchan_time_sign_wire, h_nchan_time_back_wire, c, OutputFile, false, false);
  PlotThese2Histos(h_nchan_nhit_sign_wire, h_nchan_nhit_back_wire, c, OutputFile, false, false);

  TH1D* h_maxADChit_sign_wire = new TH1D("h_maxADChit_sign_wire", ";Max hit ADC;Rate [Hz]", 150, 0, 15000);
  TH1D* h_maxADChit_back_wire = new TH1D("h_maxADChit_back_wire", ";Max hit ADC;Rate [Hz]", 150, 0, 15000);
  h_maxADChit_sign_wire->SetLineColor(kRed);
  h_maxADChit_back_wire->SetLineColor(kBlue);
  h_maxADChit_sign_wire->SetLineStyle(1);
  h_maxADChit_back_wire->SetLineStyle(1);
  h_maxADChit_sign_wire->SetLineWidth(2);
  h_maxADChit_back_wire->SetLineWidth(2);

  int nBackgroundEventWire=0;
//  int ncluster_sign=0;
  int ncluster_back=0;

  std::map<std::string,TH1D*> h_singled_maxADChit_wire  = GetHistos  ("h_maxADChit_wire" , ";Max hit ADC;Rate [Hz]",  150, 0, 15000);
  std::map<std::string,TH1D*> h_singled_sadc_nhit_wire  = GetHistos  ("h_sadc_hits_wire" , ";ADC;N hit"            ,  100, 0,  5000);
  std::map<std::string,TH1D*> h_singled_nhit_wire       = GetHistos  ("h_nhit_wire"      , ";n Hits;Rate [Hz]"     ,   50, 0,    50);
  std::map<std::string,TH1D*> h_singled_sadc_wire       = GetHistos  ("h_sadc_wire"      , ";SADC;Rate [Hz]"       ,  100, 0, 10000);
  std::map<std::string,TH1D*> h_singled_time_wire       = GetHistos  ("h_time_wire"      , ";Time;Rate [Hz]"       ,  100, 0,   100);
  std::map<std::string,TH1D*> h_singled_nchan_wire      = GetHistos  ("h_nchan_wire"     , ";n Channels;Rate [Hz]" ,   20, 0,    20);
  std::map<std::string,TH2D*> h_singled_nhit_sadc_wire  = Get2DHistos("h_nhit_sadc_wire" , ";n Hits;SADC"          ,   50, 0,    50, 100, 0, 10000);
  std::map<std::string,TH2D*> h_singled_sadc_time_wire  = Get2DHistos("h_sadc_time_wire" , ";SADC;Time"            ,  100, 0, 10000, 100, 0,   100);
  std::map<std::string,TH2D*> h_singled_time_nhit_wire  = Get2DHistos("h_time_nhit_wire" , ";Time;n Hits"          ,  100, 0,   100,  50, 0,    50);
  std::map<std::string,TH2D*> h_singled_nchan_sadc_wire = Get2DHistos("h_nchan_sadc_wire", ";n Channels;SADC"      ,   10, 0,    10, 100, 0, 10000);
  std::map<std::string,TH2D*> h_singled_nchan_time_wire = Get2DHistos("h_nchan_time_wire", ";n Channels;Time"      ,   10, 0,    10, 100, 0,   100);
  std::map<std::string,TH2D*> h_singled_nchan_nhit_wire = Get2DHistos("h_nchan_nhit_wire", ";n Channels;n Hits"    ,   10, 0,    10,  50, 0,    50);

  TEfficiency* eff_enu  = new TEfficiency("eff_enu",  ";E_{#nu} [MeV]"  ,  50, 0, 50);
  TEfficiency* eff_elep = new TEfficiency("eff_elep", ";E_{e} [MeV]"    ,  50, 0, 50);
  TEfficiency* eff_dirx = new TEfficiency("eff_dirx", ";Direction x"    ,  50, -1, 1);
  TEfficiency* eff_diry = new TEfficiency("eff_diry", ";Direction y"    ,  50, -1, 1);
  TEfficiency* eff_dirz = new TEfficiency("eff_dirz", ";Direction z"    ,  50, -1, 1);
  TEfficiency* eff_posx = new TEfficiency("eff_posx", ";Position x [cm]",  50, -380, 380);
  TEfficiency* eff_posy = new TEfficiency("eff_posy", ";Position y [cm]",  50, -650, 650);
  TEfficiency* eff_posz = new TEfficiency("eff_posz", ";Position z [cm]",  50, -100, 1300);

  
  std::map<int, std::map<int,bool> > nDetectedEvent;
  for(int i=0; i<t_Output_triggeredclusteredhits->GetEntries();++i) {
    t_Output_triggeredclusteredhits->GetEntry(i);
    PrintProgress(i, t_Output_triggeredclusteredhits->GetEntries());
    std::map<int, int> map_gentype_nhit_sign;
    std::map<int, int> map_gentype_nhit_back;
    if (Config != RequestedConfig) continue;
    //t_Output_triggeredclusteredhits->GetEntry(it2);
    nDetectedEvent[Event][-1] = false;
    
    if (Type==0) {
      // std::cout << "----------" << std::endl;
      // std::cout << "Entry " << i << std::endl;
      // std::cout << "Event " << Event << std::endl;
      // std::cout << "StartChan " << StartChan << std::endl;
      //map_gentype_nhit_back = GetMapOfHit(HitGenType);

      //filling map of hits by gentype (?)
      std::map<int, int> m_gentype;
      for (auto const& it:(*HitGenType))
        m_gentype[it]++;
      //find gentype contributing most to cluster
      int tpe = GetMax(m_gentype).first;
      std::cout << "TPE: " << tpe << std::endl;
      ++ncluster_back;
      ++nBackgroundEventWire;
      h_rate_back->Fill(tpe);
      double MaxADC=0;
      for (auto const& adc:(*HitSADC))
        if (MaxADC<adc)MaxADC=adc;
      std::string gen = ConvertIDIntToString(tpe);
      //int gen = tpe;
      //std::cout << "Gen: " << gen << std::endl;
      for (auto const& it: (*HitSADC)) {
        h_singled_sadc_nhit_wire [gen]->Fill(it);
      }
      h_maxADChit_back_wire         ->Fill(MaxADC   );
      h_singled_maxADChit_wire [gen]->Fill(MaxADC   );
      h_singled_nhit_wire      [gen]->Fill(NHits    );
      h_singled_sadc_wire      [gen]->Fill(SumADC   );
      h_singled_time_wire      [gen]->Fill(TimeWidth);
      h_singled_nchan_wire     [gen]->Fill(NChan    );
      h_singled_nhit_sadc_wire [gen]->Fill(NHits    , SumADC   );
      h_singled_sadc_time_wire [gen]->Fill(SumADC   , TimeWidth);
      h_singled_time_nhit_wire [gen]->Fill(TimeWidth, NHits    );
      h_singled_nchan_sadc_wire[gen]->Fill(NChan    , SumADC   );
      h_singled_nchan_time_wire[gen]->Fill(NChan    , TimeWidth);
      h_singled_nchan_nhit_wire[gen]->Fill(NChan    , NHits    );

    } else {
      nDetectedEvent[Event][MarleyIndex] = true;
//      map_gentype_nhit_sign = GetMapOfHit(HitGenType);

// commenting out marley case for now - NEEDS FIXING
//      double MaxADC=0;
//      for (auto const& adc:(*HitSADC))
//        if (MaxADC<adc)MaxADC=adc;
//      for (auto const& it: (*HitSADC)) {
//        h_singled_sadc_nhit_wire ["marley"]->Fill(it);
//      }
//      h_maxADChit_sign_wire           ->Fill(MaxADC   );
//      h_singled_maxADChit_wire ["marley"]->Fill(MaxADC   );
//      h_singled_nhit_wire      ["marley"]->Fill(NHits    );
//      h_singled_sadc_wire      ["marley"]->Fill(SumADC   );
//      h_singled_time_wire      ["marley"]->Fill(TimeWidth);
//      h_singled_nchan_wire     ["marley"]->Fill(NChan    );
//      h_singled_nhit_sadc_wire ["marley"]->Fill(NHits    , SumADC   );
//      h_singled_sadc_time_wire ["marley"]->Fill(SumADC   , TimeWidth);
//      h_singled_time_nhit_wire ["marley"]->Fill(TimeWidth, NHits    );
//      h_singled_nchan_sadc_wire["marley"]->Fill(NChan    , SumADC   );
//      h_singled_nchan_time_wire["marley"]->Fill(NChan    , TimeWidth);
//      h_singled_nchan_nhit_wire["marley"]->Fill(NChan    , NHits    );
//      ++ncluster_sign;
    }

     for (auto const& genhit: map_gentype_nhit_sign) p_gentype_sign_wire->Fill(genhit.first, genhit.second);
     for (auto const& genhit: map_gentype_nhit_back) p_gentype_back_wire->Fill(genhit.first, genhit.second);

//    bool fillneutron=map_gentype_nhit_sign[kNeutron]>0;
//    if (fillneutron)
//      for (auto const& genhit: map_gentype_nhit_back)
//        p_gentype_back_neut_wire->Fill(genhit.first, genhit.second);
//
//    fillneutron=map_gentype_nhit_sign[kNeutron]>0;
//    if (fillneutron)
//      for (auto const& genhit: map_gentype_nhit_sign)
//        p_gentype_sign_neut_wire->Fill(genhit.first, genhit.second);
  }
  
  for (auto& it: h_singled_sadc_nhit_wire) {
    if(h_singled_sadc_wire.at(it.first)->GetEntries()>0)
      it.second->Scale(1./ h_singled_sadc_wire.at(it.first)->GetEntries());
  }

  std::cout << "now calculating efficiencies " << std::endl;
  int ntotaldetected=0;
  for (auto const& it: nDetectedEvent) {
    int ThisEvent = it.first;
    std::map<int, bool> map_index_detected = it.second;
    for (int i=0; i<nMarleyPerEvent; ++i) {
      t_TrueInfo->GetEntry(map_Event_TrueEntry[ThisEvent]);
      eff_enu ->Fill(map_index_detected[i], ENu->at(i) * 1000.);
      eff_elep->Fill(map_index_detected[i], ENu_Lep->at(i) * 1000.);
      if (DirX->at(i) == DirX->at(i)) eff_dirx->Fill(map_index_detected[i], DirX->at(i));
      if (DirY->at(i) == DirY->at(i)) eff_diry->Fill(map_index_detected[i], DirY->at(i));
      if (DirZ->at(i) == DirZ->at(i)) eff_dirz->Fill(map_index_detected[i], DirZ->at(i));
      eff_posx->Fill(map_index_detected[i], PosX->at(i));
      eff_posy->Fill(map_index_detected[i], PosY->at(i));
      eff_posz->Fill(map_index_detected[i], PosZ->at(i));
      ntotaldetected += (map_index_detected[i]);
    }
  }
  std::cout <<"totaldetected " << ntotaldetected << std::endl;
  std::cout <<"Efficiency " << (double)ntotaldetected / nMarleyEvent << std::endl;
  TVector3 Efficiency((double)ntotaldetected / nMarleyEvent, -1, -1);
  
  gPad->SetLogy(true);
  
  std::vector<double> max = {p_gentype_sign_wire->GetMaximum(),
                             p_gentype_back_wire->GetMaximum()};
  
  p_gentype_sign_wire->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  p_gentype_sign_wire->SetMinimum(0.1);
  gPad->SetGridx();
  gPad->SetGridy();
  p_gentype_sign_wire->SetStats(0);
  p_gentype_sign_wire->Draw("E");
  p_gentype_back_wire->Draw("E SAME");
  c.Print(OutputFile.c_str());
  gPad->SetGridx(false);
  gPad->SetGridy(false);

  //p_gentype_sign_neut_wire->Draw("E");
  //p_gentype_back_neut_wire->Draw("E SAME");
  c.Print(OutputFile.c_str());

  gPad->SetTicks();
  gPad->SetGridx();
  gPad->SetGridy();
  //                  ksiourmen
  gStyle->SetOptStat(       10);
  // gStyle->SetStatX(0.88);
  // gStyle->SetStatY(0.8);
  // gStyle->SetStatH(0.1);
  // gStyle->SetStatW(0.2);
  h_rate_back->Scale(1. / (double) nEvent / 2.246e-3 / 0.12);

  int nentries = h_rate_back->GetEntries();
  double errorint;
  double integral = h_rate_back->IntegralAndError(1,10,errorint);
  h_rate_back->SetBinContent(11, integral);
  h_rate_back->SetBinError(11, errorint);
  h_rate_back->SetEntries(nentries);
  h_rate_back->Draw();
  std::cout << "TOTAL Background rate " << integral << std::endl;
  TVector3 BackgroundRate(integral, -1, -1);

  for (int i=3; i<=10; i++){
    std::cout <<h_rate_back->GetXaxis()->GetBinLabel(i) << " : " << h_rate_back->GetBinContent(i) << std::endl;
  }
  h_rate_back->GetXaxis()->SetRangeUser(2,10);
  c.Print(OutputFile.c_str());

  h_maxADChit_sign_wire->Draw();
  h_maxADChit_back_wire->Draw("SAME");
  c.Print(OutputFile.c_str());
  gPad->Clear();
  TLegend* leg = new TLegend(0.1, 0.1, 0.9, 0.9);
  leg->AddEntry(p_gentype_sign_wire, "Signal TPC clusters (arbitrary normalisation)");
  leg->AddEntry(p_gentype_back_wire, "Background TPC clusters (10kT rate)");
  leg->Draw();
  c.Print(OutputFile.c_str());
  
  ScaleTheseHistos(h_singled_maxADChit_wire , 1. / (double) nEvent / 2.246e-3 / 0.12);
  ScaleTheseHistos(h_singled_nhit_wire      , 1. / (double) nEvent / 2.246e-3 / 0.12);
  ScaleTheseHistos(h_singled_sadc_wire      , 1. / (double) nEvent / 2.246e-3 / 0.12);
  ScaleTheseHistos(h_singled_time_wire      , 1. / (double) nEvent / 2.246e-3 / 0.12);
  ScaleTheseHistos(h_singled_nchan_wire     , 1. / (double) nEvent / 2.246e-3 / 0.12);
  ScaleTheseHistos(h_singled_nhit_sadc_wire , 1. / (double) nEvent / 2.246e-3 / 0.12);
  ScaleTheseHistos(h_singled_sadc_time_wire , 1. / (double) nEvent / 2.246e-3 / 0.12);
  ScaleTheseHistos(h_singled_time_nhit_wire , 1. / (double) nEvent / 2.246e-3 / 0.12);
  ScaleTheseHistos(h_singled_nchan_sadc_wire, 1. / (double) nEvent / 2.246e-3 / 0.12);
  ScaleTheseHistos(h_singled_nchan_time_wire, 1. / (double) nEvent / 2.246e-3 / 0.12);
  ScaleTheseHistos(h_singled_nchan_nhit_wire, 1. / (double) nEvent / 2.246e-3 / 0.12);

  FormatTheseHistos(h_singled_sadc_nhit_wire , 10.);
  FormatTheseHistos(h_singled_maxADChit_wire , 10.);
  FormatTheseHistos(h_singled_nhit_wire      , 10.);
  FormatTheseHistos(h_singled_sadc_wire      , 10.);
  FormatTheseHistos(h_singled_time_wire      , 10.);
  FormatTheseHistos(h_singled_nchan_wire     , 10.);
  FormatTheseHistos(h_singled_nhit_sadc_wire );
  FormatTheseHistos(h_singled_sadc_time_wire );
  FormatTheseHistos(h_singled_time_nhit_wire );
  FormatTheseHistos(h_singled_nchan_sadc_wire);
  FormatTheseHistos(h_singled_nchan_time_wire);
  FormatTheseHistos(h_singled_nchan_nhit_wire);

  PlotAll(h_singled_sadc_nhit_wire , "", c, OutputFile, 0, 1);
  PlotAll(h_singled_maxADChit_wire , "", c, OutputFile, 0, 1);
  PlotAll(h_singled_nhit_wire      , "", c, OutputFile, 0, 1);
  PlotAll(h_singled_sadc_wire      , "", c, OutputFile, 0, 1);
  PlotAll(h_singled_time_wire      , "", c, OutputFile, 0, 1);
  PlotAll(h_singled_nchan_wire     , "", c, OutputFile, 0, 1);
  PlotAllIndividually(h_singled_nhit_sadc_wire , "COLZ", c, OutputFile);
  PlotAllIndividually(h_singled_sadc_time_wire , "COLZ", c, OutputFile);
  PlotAllIndividually(h_singled_time_nhit_wire , "COLZ", c, OutputFile);
  PlotAllIndividually(h_singled_nchan_sadc_wire, "COLZ", c, OutputFile);
  PlotAllIndividually(h_singled_nchan_time_wire, "COLZ", c, OutputFile);
  PlotAllIndividually(h_singled_nchan_nhit_wire, "COLZ", c, OutputFile);

  eff_enu ->SetLineWidth(2.);
  eff_elep->SetLineWidth(2.);
  eff_dirx->SetLineWidth(2.);
  eff_diry->SetLineWidth(2.);
  eff_dirz->SetLineWidth(2.);
  eff_posx->SetLineWidth(2.);
  eff_posy->SetLineWidth(2.);
  eff_posz->SetLineWidth(2.);
  
  eff_enu ->Draw(); c.Print(OutputFile.c_str());
  eff_elep->Draw(); c.Print(OutputFile.c_str());
  eff_dirx->Draw(); c.Print(OutputFile.c_str());
  eff_diry->Draw(); c.Print(OutputFile.c_str());
  eff_dirz->Draw(); c.Print(OutputFile.c_str());
  eff_posx->Draw(); c.Print(OutputFile.c_str());
  eff_posy->Draw(); c.Print(OutputFile.c_str());
  eff_posz->Draw(); c.Print(OutputFile.c_str());

  gPad->Clear();
  TLegend* leg2 = GetNewLegend(0.1,0.1,0.9,0.9,h_singled_nhit_wire);
  leg2->Draw();
  c.Print(OutputFile.c_str());
  c.Print((OutputFile+"]").c_str());

  TFile f((OutputFile+".root").c_str(), "RECREATE");
  f.cd();
  leg->Write();
  p_gentype_sign_wire     ->Write();
  p_gentype_back_wire     ->Write();
  //p_gentype_sign_neut_wire->Write();
  //p_gentype_back_neut_wire->Write();
  h_rate_back             ->Write();
  h_nhit_sign_wire        ->Write();
  h_nhit_back_wire        ->Write();
  h_sadc_sign_wire        ->Write();
  h_sadc_back_wire        ->Write();
  h_time_sign_wire        ->Write();
  h_time_back_wire        ->Write();
  h_nchan_sign_wire       ->Write();
  h_nchan_back_wire       ->Write();
  h_sadc_hits_sign_wire   ->Write();
  h_sadc_hits_back_wire   ->Write();
  h_nhit_sadc_sign_wire   ->Write();
  h_nhit_sadc_back_wire   ->Write();
  h_sadc_time_sign_wire   ->Write();
  h_sadc_time_back_wire   ->Write();
  h_time_nhit_sign_wire   ->Write();
  h_time_nhit_back_wire   ->Write();
  h_nchan_sadc_sign_wire  ->Write();
  h_nchan_sadc_back_wire  ->Write();
  h_nchan_time_sign_wire  ->Write();
  h_nchan_time_back_wire  ->Write();
  h_nchan_nhit_sign_wire  ->Write();
  h_nchan_nhit_back_wire  ->Write();
  
  for (auto const& it: h_singled_sadc_nhit_wire ) {it.second->Write();}
  for (auto const& it: h_singled_maxADChit_wire ) {it.second->Write();}
  for (auto const& it: h_singled_nhit_wire      ) {it.second->Write();}
  for (auto const& it: h_singled_sadc_wire      ) {it.second->Write();}
  for (auto const& it: h_singled_time_wire      ) {it.second->Write();}
  for (auto const& it: h_singled_nchan_wire     ) {it.second->Write();}
  for (auto const& it: h_singled_nhit_sadc_wire ) {it.second->Write();}
  for (auto const& it: h_singled_sadc_time_wire ) {it.second->Write();}
  for (auto const& it: h_singled_time_nhit_wire ) {it.second->Write();}
  for (auto const& it: h_singled_nchan_sadc_wire) {it.second->Write();}
  for (auto const& it: h_singled_nchan_time_wire) {it.second->Write();}
  for (auto const& it: h_singled_nchan_nhit_wire) {it.second->Write();}
  leg2->Write();
  Efficiency.Write("Efficiency");
  BackgroundRate.Write("BackgroundRate");
  eff_enu ->Write();
  eff_elep->Write();
  eff_dirx->Write();
  eff_diry->Write();
  eff_dirz->Write();
  eff_posx->Write();
  eff_posy->Write();
  eff_posz->Write();

  f.Close();

  return 1;

}
