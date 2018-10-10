#include <algorithm>    // std::min_element, std::max_element
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "Helper.h"

#include "TFile.h"

#include "TMath.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TPad.h"
#include "TTree.h"
#include "TLegend.h"
#include "TGraphErrors.h"


TProfile* SetUpTProfileGenType(std::string name, std::string title) {
  Helper h;
  TProfile* p_ = new TProfile(name.c_str(), title.c_str(), h.GenName.size(), -0.5, (double)h.GenName.size()-0.5);
  for(auto const& it : h.ShortGenName)
    p_->GetXaxis()->SetBinLabel(it.first+1, it.second.c_str());
  return p_;
  
};

std::map<int, int> GetMapOfHit(std::vector<int> const* GenType){
  std::map<int,int> map_;
  for(std::vector<int>::const_iterator it=GenType->begin(); it!=GenType->end(); ++it)
    map_[*it]++;
  return map_;
};

int main(int argc, char** argv){
  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  int nHitCut=-1, RequestedConfig=0;
  std::string InputFile;
  std::string OutputFile;
  std::string WeightFile;
  int nEvent = 0;

  while ( (opt = getopt(argc, argv, "h:o:i:n:w:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'h':
      nHitCut = atoi(optarg);
      break;
    case 'i':
      InputFile = optarg;
      break;
    case 'c':
      RequestedConfig = atoi(optarg);
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case 'w':
      WeightFile = optarg;
      break;
    case 'o':
      OutputFile = optarg;
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
  TFile *f_Weights = new TFile(WeightFile.c_str(), "READ");
  TH1D* hWeight = (TH1D*)f_Weights->Get("SolarNu_weight");
  
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
  double pur_APA       ;
  double pur_CPA       ;
  double pur_Ar39      ;
  double pur_Neutron   ;
  double pur_Krypton   ;
  double pur_Polonium  ;
  double pur_Radon     ;
  double pur_Ar42      ;
  double YWidth        ;
  double ZWidth        ;
  double SumPE         ;
  
  std::vector<int>    * HitView  = NULL;
  std::vector<int>    * GenType  = NULL;
  std::vector<int>    * HitChan  = NULL;
  std::vector<double> * HitTime  = NULL;
  std::vector<double> * HitSADC  = NULL;
  std::vector<double> * HitRMS   = NULL;
  std::vector<double> * TrPosX   = NULL; 
  std::vector<double> * TrPosY   = NULL; 
  std::vector<double> * TrPosZ   = NULL;

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
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_APA"     ,   &pur_APA     );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_CPA"     ,   &pur_CPA     );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar39"    ,   &pur_Ar39    );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Neutron" ,   &pur_Neutron );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Krypton" ,   &pur_Krypton );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Polonium",   &pur_Polonium);
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Radon"   ,   &pur_Radon   );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar42"    ,   &pur_Ar42    );
  t_Output_triggeredclusteredhits->SetBranchAddress("HitView",        &HitView);
  t_Output_triggeredclusteredhits->SetBranchAddress("GenType",        &GenType);
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
  std::map<int,double> map_Event_Weight;
  int nMarleyEvent=0;
  for (int i=0; i<nEvent; ++i) {
    t_TrueInfo->GetEntry(i);
    map_Event_TrueEntry[EventTrue] = i;
    ++nMarleyEvent;
    map_Event_Weight[EventTrue] = hWeight->GetBinContent(hWeight->FindBin(1000.*ENu->at(0)));
  }
  
  TCanvas c;
  c.Print((OutputFile+"[").c_str());

  std::map<int,std::vector<int> > map_event_entry_wire;
  for(int i=0; i<t_Output_triggeredclusteredhits->GetEntries();++i) {
    t_Output_triggeredclusteredhits->GetEntry(i);
    if (Config==RequestedConfig) map_event_entry_wire[Event].push_back(i);
  }

  TH1D* h_ncluster_sign_wire = new TH1D("h_ncluster_sign_wire", ";n clusters;Events", 1500, 0, 1500);
  std::cout << "here " << std::endl;


  std::vector<double> vec_SADC_Cut = {0,1000,2000,3000,4000,5000,6000};
  std::map<double,TEfficiency*> map_eff;
  TGraphErrors* BackgroundRate_SADC = new TGraphErrors(vec_SADC_Cut.size());
  TGraphErrors* Efficiency_SADC     = new TGraphErrors(vec_SADC_Cut.size());
  int pointcount=0;
  double ADCerror=500;
  for (auto const& ADC: vec_SADC_Cut) {
    std::cout << "ADC " << ADC << std::endl;
    double eff, efferror;
    double back, backerror;
    double nEventWire=0;
    double nDetectedSignalEventWire=0;
    double nBackgroundEventWire=0;
    map_eff[ADC] = new TEfficiency(Form("EffAtADC>%.0f",ADC),Form("Efficiency at ADC > %.0f;E_{#nu};Efficiency to cluster",ADC),30,0,30);
    for (auto const& it: map_Event_TrueEntry) {
      nEventWire += map_Event_Weight[it.first];
      bool detected = false;
      for (auto const& it2 : map_event_entry_wire[it.first]) {
        std::map<int, int> map_gentype_nhit_sign;
        std::map<int, int> map_gentype_nhit_back;
        t_Output_triggeredclusteredhits->GetEntry(it2);
        if (SumADC < ADC) continue;
        if (Type!=1) {
          std::cout << "nBackgroundEventWire " << nBackgroundEventWire <<std::endl;
          ++nBackgroundEventWire;
        } else {
          detected = true;
          nDetectedSignalEventWire += map_Event_Weight[it.first];
        }
      }
      t_TrueInfo->GetEntry(it.second);
      map_eff[ADC]->Fill(detected, 1000.*ENu->at(0));
    }
    std::cout << nEventWire << std::endl;
    std::cout << nBackgroundEventWire << std::endl;
    eff = nDetectedSignalEventWire / nEventWire;
    efferror = TMath::Sqrt(1./(nDetectedSignalEventWire) + 1./(nEventWire));
    efferror = eff*efferror;
    back = (double)nBackgroundEventWire / nEventWire / 2.246e-3 /0.12;
    backerror = TMath::Sqrt(1./nBackgroundEventWire + 1./ nEventWire);
    backerror = back*backerror;
    std::cout << "eff " << eff << " +- " << efferror << std::endl;
    std::cout << "back " << back << " +- " << backerror << std::endl;
    std::cout << "pc " << pointcount << std::endl;
    BackgroundRate_SADC->SetPoint(pointcount, ADC, back);
    Efficiency_SADC    ->SetPoint(pointcount, ADC, eff );
    BackgroundRate_SADC->SetPointError(pointcount, ADCerror, backerror);
    Efficiency_SADC    ->SetPointError(pointcount, ADCerror, efferror );

    ++pointcount;
    
  }
  
  BackgroundRate_SADC->Draw("AP");
  BackgroundRate_SADC->SetTitle("");
  c.Print(OutputFile.c_str());
  gPad->SetLogy();
  Efficiency_SADC->GetXaxis()->SetTitle("Sum ADC threshold");
  Efficiency_SADC->GetYaxis()->SetTitle("Efficiency for Solar #nu");
  Efficiency_SADC->SetTitle("");
  Efficiency_SADC->Draw("AP");
  c.Print(OutputFile.c_str());

  gPad->SetLogy(false);
  for (auto const& it:map_eff) {
    it.second->Draw();
    c.Print(OutputFile.c_str());
  }
  c.Print((OutputFile+"]").c_str());

  return 1;

};
