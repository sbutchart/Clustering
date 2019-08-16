#include <algorithm>    // std::min_element, std::max_element
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "Utils/Helper.h"

#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TPad.h"
#include "TLine.h"
#include "TTree.h"
#include "TLegend.h"
#include "TEfficiency.h"

std::map<int, int> GetMapOfHit(std::vector<int> const* GenType){
  std::map<int,int> map_;
  for(std::vector<int>::const_iterator it=GenType->begin(); it!=GenType->end(); ++it)
    map_[*it]++;
  
  return map_;
}

int main(int argc, char** argv){

  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  int RequestedConfig=0;
  int nEvent = 0;
  std::string InputFile="";
  std::string OutputFile="OpticalCluster.pdf";
  int nHitCut=0, nPECut=0;
  while ( (opt = getopt(argc, argv, "c:h:p:o:i:n:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'c':
      RequestedConfig = atoi(optarg);
      break;
    case 'h':
      nHitCut = atoi(optarg);
      break;
    case 'p':
      nPECut = atoi(optarg);
      break;
    case 'i':
      InputFile = optarg;
      break;
    case 'o':
      OutputFile = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  if (OutputFile == "OpticalCluster.pdf") {
    OutputFile = Form("OpticalCluster_c%i.pdf",RequestedConfig);
  }

  if (InputFile == "") {
    std::cout << "You need to provide an input file -i ClusteringOutput.root" << std::endl;
  }

  
  std::cout << "Saving output in " << OutputFile << std::endl;
  TFile *f_Input = new TFile(InputFile.c_str(), "READ");

  int    Cluster       ;
  int    Event         ;
  int    MarleyIndex   ;
  int    EventTrue     ;
  int    Config        ;
  double NChan         ;
  double Type          ;
  double NHits         ;
  double FirstTimeHit  ;
  double LastTimeHit   ;
  double TimeWidth     ;
  double RecClusterPosX;
  double RecClusterPosY;
  double RecClusterPosZ;
  double YWidth        ;
  double ZWidth        ;
  double SumPE         ;

  std::vector<int>    * PDSHit_GenType   = NULL;
  std::vector<double> * PDSHit_X         = NULL;
  std::vector<double> * PDSHit_Y         = NULL;
  std::vector<double> * PDSHit_Z         = NULL;
  std::vector<double> * PDSHit_PeakTime  = NULL;
  std::vector<double> * PDSHit_Width     = NULL;
  std::vector<double> * PDSHit_PE        = NULL;
  std::vector<double> * PDSHit_OpChannel = NULL;

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
  
  TTree* t_ClusteredOptHit = (TTree*)f_Input->Get("ClusteredOpticalHit");
  t_ClusteredOptHit->SetBranchAddress("Cluster",        &Cluster       );
  t_ClusteredOptHit->SetBranchAddress("Config",         &Config        );
  t_ClusteredOptHit->SetBranchAddress("Event",          &Event         );
  t_ClusteredOptHit->SetBranchAddress("MarleyIndex",    &MarleyIndex   );
  t_ClusteredOptHit->SetBranchAddress("YWidth",         &YWidth        );
  t_ClusteredOptHit->SetBranchAddress("ZWidth",         &ZWidth        );
  t_ClusteredOptHit->SetBranchAddress("NChan",          &NChan         );
  t_ClusteredOptHit->SetBranchAddress("Type",           &Type          );
  t_ClusteredOptHit->SetBranchAddress("NHits",          &NHits         );
  t_ClusteredOptHit->SetBranchAddress("SumPE",          &SumPE         );
  t_ClusteredOptHit->SetBranchAddress("FirstTimeHit",   &FirstTimeHit  );
  t_ClusteredOptHit->SetBranchAddress("LastTimeHit",    &LastTimeHit   );
  t_ClusteredOptHit->SetBranchAddress("TimeWidth",      &TimeWidth     );
  t_ClusteredOptHit->SetBranchAddress("RecClusterPosX", &RecClusterPosX);
  t_ClusteredOptHit->SetBranchAddress("RecClusterPosY", &RecClusterPosY);
  t_ClusteredOptHit->SetBranchAddress("RecClusterPosZ", &RecClusterPosZ);
  t_ClusteredOptHit->SetBranchAddress("Hit_GenType"   , &PDSHit_GenType  );
  t_ClusteredOptHit->SetBranchAddress("Hit_X"         , &PDSHit_X        );
  t_ClusteredOptHit->SetBranchAddress("Hit_Y"         , &PDSHit_Y        );
  t_ClusteredOptHit->SetBranchAddress("Hit_Z"         , &PDSHit_Z        );
  t_ClusteredOptHit->SetBranchAddress("Hit_PeakTime"  , &PDSHit_PeakTime );
  t_ClusteredOptHit->SetBranchAddress("Hit_Width"     , &PDSHit_Width    );
  t_ClusteredOptHit->SetBranchAddress("Hit_PE"        , &PDSHit_PE       );
  t_ClusteredOptHit->SetBranchAddress("Hit_OpChannel" , &PDSHit_OpChannel);

  TTree* t_TrueInfo = (TTree*)f_Input->Get("TrueInfo");
  t_TrueInfo->SetBranchAddress("Event",    &EventTrue);
  t_TrueInfo->SetBranchAddress("MarlTime", &MarlTime);
  t_TrueInfo->SetBranchAddress("ENu",      &ENu     );
  t_TrueInfo->SetBranchAddress("ENu_Lep",  &ENu_Lep );
  t_TrueInfo->SetBranchAddress("PosX",     &PosX    );
  t_TrueInfo->SetBranchAddress("PosY",     &PosY    );
  t_TrueInfo->SetBranchAddress("PosZ",     &PosZ    );
  t_TrueInfo->SetBranchAddress("PosT",     &PosT    );
  t_TrueInfo->SetBranchAddress("DirX",     &DirX    );
  t_TrueInfo->SetBranchAddress("DirY",     &DirY    );
  t_TrueInfo->SetBranchAddress("DirZ",     &DirZ    );
  
  if (nEvent == 0) {
    nEvent = t_TrueInfo->GetEntries();
  } else {
    nEvent = std::min(nEvent,(int)t_TrueInfo->GetEntries());
  }
  
  std::map<int,int> map_Event_TrueEntry;
  int nMarleyEvent=0;
  for (int i=0; i<nEvent; ++i) {
    t_TrueInfo->GetEntry(i);
    map_Event_TrueEntry[EventTrue] = i;
    nMarleyEvent += (int)ENu->size();
  }
    std::cout << "Done map of true events" << std::endl;

  std::map<int,std::vector<int> > map_event_entry_opti;
  for (int i=0; i<t_ClusteredOptHit->GetEntries(); ++i) {
    PrintProgress(i,t_ClusteredOptHit->GetEntries());
    t_ClusteredOptHit->GetEntry(i);
    if (Config==RequestedConfig && map_Event_TrueEntry.find(Event)!=map_Event_TrueEntry.end())
      map_event_entry_opti[Event].push_back(i);
  }
  
  std::cout << "Now starting to run over " << nEvent << " events." << std::endl;

  std::map<GenType,TH1D*> histo_hit = GetHistos("h_nhit_back_opti", ";n Hits;n Clusters",
                                                200, 0, 1000);
  std::map<GenType,TH1D*> histo_pe = GetHistos("h_npe_back_opti", ";n PE;n Clusters",
                                               200, 0, 1000);

  
  for(auto const& it : map_Event_TrueEntry) {
    for (auto const& it2 : map_event_entry_opti[it.first]) {
      t_ClusteredOptHit->GetEntry(it2);
      
      if (NHits<nHitCut || SumPE<nPECut) continue;
      if (Type==0) {
        std::map<int, int> hits = GetMapOfHit(PDSHit_GenType);
        int max = (int)GetMax(hits).first;
        histo_hit.at(ConvertIntToGenType(max))->Fill(NHits);
        histo_pe .at(ConvertIntToGenType(max))->Fill(SumPE);
      }
    }
  }
  TCanvas c;
  gPad->SetLogy();
  c.Print((OutputFile+"[").c_str());
  SetMaxHisto(histo_hit,10);
  PlotAll(histo_hit);
  c.Print(OutputFile.c_str());

  SetMaxHisto(histo_pe,10);
  PlotAll(histo_pe);
  c.Print(OutputFile.c_str());
  
  gPad->Clear();
  GetNewLegend(0.1,0.1,0.9,0.9,histo_hit)->Draw();
  c.Print(OutputFile.c_str());
  c.Print((OutputFile+"]").c_str());


  TFile f((OutputFile+".root").c_str(), "RECREATE");
  f.cd();
  for (auto const& it: histo_hit) {
    it.second->Write();
  }
  for (auto const& it: histo_pe) {
    it.second->Write();
  }
  f.Close();
  return 1;

}
