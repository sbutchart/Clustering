#include <algorithm>    // std::min_element, std::max_element
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "Utils/Helper.h"
#include "Analyser/BurstEfficiencyCalculator.hh"

#include "TFile.h"
#include "TCanvas.h"
#include "TArrow.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TPad.h"
#include "TLine.h"
#include "TTree.h"
#include "TLegend.h"
#include "TF1.h"
#include "TEfficiency.h"

TProfile* SetUpTProfileGenType(std::string name, std::string title) {
  Helper h;
  TProfile* p_ = new TProfile(name.c_str(), title.c_str(), h.GenName.size(), -0.5, (double)h.GenName.size()-0.5);
  for(auto const& it : h.ShortGenName)
    p_->GetXaxis()->SetBinLabel(it.first+1, it.second.c_str());
  return p_;
}

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
  int Range = 800;
  int Start = 0;
  bool BadFile = 0;
  std::string InputFile="";
  std::string OutputFile="OpticalCluster.pdf";
  int nPECutMin=0;
  int nHitCutMin=0;
  while ( (opt = getopt(argc, argv, "c:p:o:h:i:n:s:r:b")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'c':
      RequestedConfig = atoi(optarg);
      break;
    case 'b':
      BadFile = 1;
      break;
    case 'h':
      nHitCutMin = atoi(optarg);
      break;
    case 'p':
      nPECutMin = atoi(optarg);
      break;
    case 's':
      Start = atoi(optarg);
      break;
    case 'i':
      InputFile = optarg;
      break;
    case 'r':
      Range = atoi(optarg);
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
  if(Start == 0 ) Start =  nPECutMin;
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

  std::cout << "Running over " << nEvent << " events." << std::endl;

  
  std::map<int,int> map_Event_TrueEntry;
  int nMarleyEvent=t_TrueInfo->GetEntries();
  
  std::cout << "I will look at the config " << RequestedConfig << std::endl;
  std::cout << "There were " << nMarleyEvent << " Marley events in " << nMarleyEvent << " Larsoft events." << std::endl;

  std::map<int,std::vector<int> > map_event_entry_opti;

  std::cout << t_ClusteredOptHit->GetEntries() << " optical clusters were saved." << std::endl;
  TEfficiency* Efficiency = new TEfficiency("Efficiency_ENu", ";Efficiency;E_{#nu} [MeV]", 30, 0, 30);
  
  TH1D* h_nPE_back_opti = new TH1D("h_nPE_back_opti", ";n PEs;Clusters", Range, 0, Range);
  h_nPE_back_opti->SetLineColor(kBlue);
  h_nPE_back_opti->SetMinimum(0.1);
  h_nPE_back_opti->SetLineStyle(1);
  h_nPE_back_opti->SetLineWidth(2);

  t_ClusteredOptHit->Project("h_nPE_back_opti", "SumPE", Form("Type==0 && Config==%i", RequestedConfig));

  TF1 *f1 = new TF1("double_exp","[0]*exp([1]*x)",0,Range);
  f1->SetLineWidth(2);
  f1->SetLineColor(kViolet);
  f1->SetParameter(0, 100);
  f1->SetParameter(1, -0.01);
    
  TFitResultPtr r = h_nPE_back_opti->Fit("double_exp", "", "", nPECutMin, Range);

  TF1 *f2 = new TF1("double_exp","[0]*exp([1]*x)",0,Range);
  f2->SetLineWidth(2);
  f2->SetLineColor(kRed);
  f2->SetParameter(0, 2.*f1->GetParameter(0));
  f2->SetParameter(1,    f1->GetParameter(1));

  std::cout << "Fitting from " << std::endl;
  std::cout << " - Min " << nPECutMin << std::endl;
  std::cout << " - Max " << Range << std::endl;
  
  TCanvas c;
  c.Print((OutputFile+"[").c_str());
  h_nPE_back_opti->Draw("");
  f1->Draw("SAME");
  f2->Draw("SAME");
  
  TLine line3(nPECutMin, h_nPE_back_opti->GetMinimum(), nPECutMin, h_nPE_back_opti->GetMaximum());
  line3.SetLineWidth(2);
  line3.Draw();
  gPad->SetLogy();
  c.Print(OutputFile.c_str());
  c.Print("Fit.pdf");
  // if (r) {
  //   std::cout << "Fit failed!" << std::endl;
  //   c.Print((OutputFile+"]").c_str());
  //   exit(1);
  // }
  
  std::vector<double> max;
  int EventIterator = 0;
  std::map<int,bool>   totalDetectedEvent;
  std::map<int,int>    totalBackCluster;
  std::map<int,int>    totalSignCluster;
  std::map<int,double> totalEventEnergy;
  std::map<int,bool>   detectedEvent;
  std::map<int,double> eventEnergy;
  std::map<int,int>    backCluster;
  std::map<int,int>    signCluster;

  for (int i=0; i<100; i++) {
    detectedEvent[i] = false;
    backCluster[i] = 0;
    signCluster[i] = 0;
  }
  
  std::cout << "nPE cut = " << nPECutMin << std::endl;
  std::cout << "nHit cut = " << nHitCutMin << std::endl;
  totalDetectedEvent.clear();
  totalBackCluster  .clear();
  totalSignCluster  .clear();
  totalEventEnergy  .clear();
  detectedEvent     .clear();
  backCluster       .clear();
  signCluster       .clear();
  bool resetfirst=false;
  int Divisor=1;
  
  //Divisor=100;
  //if (nPECut<10) Divisor=100;
  CurrentProg=0;
  for (int iEntry=0; iEntry<t_ClusteredOptHit->GetEntries()/Divisor; iEntry++) {
    t_ClusteredOptHit->GetEntry(iEntry);
    PrintProgress(iEntry, (int)(t_ClusteredOptHit->GetEntries()/Divisor));
    if (Event==99 && BadFile) {
      resetfirst=0;
      continue;
    }

    if (Event==0 && !resetfirst && iEntry!=0 && BadFile) {
      resetfirst=true;

      for (int i=0; i<100; i++) {
        totalDetectedEvent[EventIterator] = detectedEvent[i];
        totalBackCluster[EventIterator] = backCluster[i];
        totalSignCluster[EventIterator] = signCluster[i];
        totalEventEnergy[EventIterator] = eventEnergy[i];
        detectedEvent[i] = false;
        backCluster[i] = 0;
        signCluster[i] = 0;
        eventEnergy[i] = 0;
        EventIterator++;
      }
    }
    
    if (SumPE<nPECutMin || NHits<nHitCutMin) continue;
    eventEnergy[Event] = 0;
    if (Type==0) {
      backCluster[Event]++;
    } else {
      detectedEvent[Event]=true;
      signCluster[Event]++;
    }
  }
  
  if (!BadFile) {
    totalDetectedEvent = detectedEvent;
    totalBackCluster = backCluster;
    totalSignCluster = signCluster;
    totalEventEnergy = eventEnergy;
  }
  
  size_t nTotalEvent=0;
  size_t nDetectedEvent=0;
  size_t nBackgroundCluster=0;
  for (auto const& it:totalDetectedEvent) {
    nTotalEvent++;
    if (it.second) {
      nDetectedEvent++;
    }
    Efficiency->Fill(it.second, totalEventEnergy.at(it.first));
  }
  if (!BadFile) nTotalEvent=nEvent;
  std::cout << "nTotalEvent " << nTotalEvent << std::endl;
  std::cout << "nDetectedEvent " << nDetectedEvent << std::endl;
  std::cout << "Overall efficiency " << (double)nDetectedEvent / (double)nTotalEvent << std::endl;
  
  for (auto const& it:totalBackCluster)
    nBackgroundCluster += it.second;
  Efficiency->Draw();
  
  std::cout << "Background rate from counting " << (double)nBackgroundCluster / (double)nTotalEvent / 4.492e-3 / 0.12 << std::endl;
  std::cout << "Background rate from histo " <<  h_nPE_back_opti->Integral(nPECutMin+1, Range) / (double)nTotalEvent / 4.492e-3 / 0.12 << std::endl;
  std::cout << "Background rate from f1 " <<  f1->Integral(nPECutMin+1, Range) / (double)nTotalEvent / 4.492e-3 / 0.12 << std::endl;
  std::cout << "Background rate from f2 " <<  f2->Integral(nPECutMin+1, Range) / (double)nTotalEvent / 4.492e-3 / 0.12 << std::endl;

  c.Print(OutputFile.c_str());

  TFile f((OutputFile+".root").c_str(), "RECREATE");
  f.cd();
  Efficiency->Write();
  f.Close();
  
  return 1;

}

