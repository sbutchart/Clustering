#include <algorithm>    // std::min_element, std::max_element
#include <iostream>
#include <fstream>
#include <unistd.h>


#include "Helper.h"

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

  int nHitCut=-1, RequestedConfig=0;
  std::string InputFile;
  std::string OutputFile;
  int nEvent = 0;

  while ( (opt = getopt(argc, argv, "h:o:i:n:")) != -1 ) {  // for each option...
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
  double pur_APA       ;
  double pur_CPA       ;
  double pur_Ar39      ;
  double pur_Neutron   ;
  double pur_Krypton   ;
  double pur_Polonium  ;
  double pur_Radon     ;
  double pur_Ar42      ;
  
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
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_APA"     ,   &pur_APA     );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_CPA"     ,   &pur_CPA     );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar39"    ,   &pur_Ar39    );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Neutron" ,   &pur_Neutron );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Krypton" ,   &pur_Krypton );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Polonium",   &pur_Polonium);
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Radon"   ,   &pur_Radon   );
  t_Output_triggeredclusteredhits->SetBranchAddress("pur_Ar42"    ,   &pur_Ar42    );
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
  TCanvas c;
  c.Print((OutputFile+"[").c_str());
  TProfile* p_gentype_sign_wire = SetUpTProfileGenType("p_gentype_sign_wire", ";;average number of hits in cluster");
  TProfile* p_gentype_back_wire = SetUpTProfileGenType("p_gentype_back_wire", ";;average number of hits in cluster");
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

  TH1D* h_rate_back = new TH1D("rates", ";;Rate [Hz]", h.GenName.size(), -0.5, (double)h.GenName.size()-0.5);
  //h_rate_back->SetStats(0);
  h_rate_back->SetLineWidth(2);
  for(auto const& it : h.ShortGenName)
    h_rate_back->GetXaxis()->SetBinLabel(it.first+1, it.second.c_str());
  
  std::map<int,std::vector<int> > map_event_entry_wire;
  for(int i=0; i<t_Output_triggeredclusteredhits->GetEntries();++i) {
    t_Output_triggeredclusteredhits->GetEntry(i);
    if (Config==RequestedConfig) map_event_entry_wire[Event].push_back(i);
  }

  TH1D* h_ncluster_sign_wire = new TH1D("h_ncluster_sign_wire", ";n clusters;Events", 1500, 0, 1500);
  TH1D* h_ncluster_back_wire = new TH1D("h_ncluster_back_wire", ";n clusters;Events", 1500, 0, 1500);
  h_ncluster_sign_wire->SetLineColor(kRed);
  h_ncluster_back_wire->SetLineColor(kBlue);
  h_ncluster_sign_wire->SetLineStyle(1);
  h_ncluster_back_wire->SetLineStyle(1);
  h_ncluster_sign_wire->SetLineWidth(2);
  h_ncluster_back_wire->SetLineWidth(2);

  TH1D* h_nhit_sign_wire = new TH1D("h_nhit_sign_wire", ";n Hits;Clusters", 100, -0.5, 99.5);
  TH1D* h_nhit_back_wire = new TH1D("h_nhit_back_wire", ";n Hits;Clusters", 100, -0.5, 99.5);
  h_nhit_sign_wire->SetLineColor(kRed);
  h_nhit_back_wire->SetLineColor(kBlue);
  h_nhit_sign_wire->SetLineStyle(1);
  h_nhit_back_wire->SetLineStyle(1);
  h_nhit_sign_wire->SetLineWidth(2);
  h_nhit_back_wire->SetLineWidth(2);
  
  t_Output_triggeredclusteredhits->Project("h_nhit_sign_wire", "NHits", Form("Type==1 && Config==%i && NHits>=%i", RequestedConfig, nHitCut));
  t_Output_triggeredclusteredhits->Project("h_nhit_back_wire", "NHits", Form("Type==0 && Config==%i && NHits>=%i", RequestedConfig, nHitCut));
  std::cout << h_nhit_back_wire->GetEntries() << std::endl;
  std::vector<double> max = {h_nhit_sign_wire->GetMaximum(),
                             h_nhit_back_wire->GetMaximum()};
  
  h_nhit_sign_wire->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  h_nhit_sign_wire->SetMinimum(0.1);
  gPad->SetLogy();
  h_nhit_sign_wire->Draw();
  h_nhit_back_wire->Draw("SAME");
  c.Print(OutputFile.c_str());

  TH1D* h_maxADChit_sign_wire = new TH1D("h_maxADChit_sign_wire", ";Max hit ADC;Clusters", 150, 0, 15000);
  TH1D* h_maxADChit_back_wire = new TH1D("h_maxADChit_back_wire", ";Max hit ADC;Clusters", 150, 0, 15000);
  h_maxADChit_sign_wire->SetLineColor(kRed);
  h_maxADChit_back_wire->SetLineColor(kBlue);
  h_maxADChit_sign_wire->SetLineStyle(1);
  h_maxADChit_back_wire->SetLineStyle(1);
  h_maxADChit_sign_wire->SetLineWidth(2);
  h_maxADChit_back_wire->SetLineWidth(2);

  int nBackgroundEventWire=0;
  int ncluster_sign=0;
  int ncluster_back=0;

  std::map<std::pair<int,int>,bool> nDetectedEvent;
  for(int i=0; i<t_Output_triggeredclusteredhits->GetEntries();++i) {
    t_Output_triggeredclusteredhits->GetEntry(i);
    std::map<int, int> map_gentype_nhit_sign;
    std::map<int, int> map_gentype_nhit_back;
    //t_Output_triggeredclusteredhits->GetEntry(it2);
    if (Type==0) {
      std::cout << "----------" << std::endl;
      std::cout << "Entry " << i << std::endl;
      std::cout << "Event " << Event << std::endl;
      std::cout << "StartChan " << StartChan << std::endl;
      //map_gentype_nhit_back = GetMapOfHit(HitGenType);
      std::map<int, int> m_gentype;
      for (auto const& it:(*HitGenType))
        m_gentype[it]++;
      int tpe = GetMax(m_gentype).first;
      ++ncluster_back;
      ++nBackgroundEventWire;
      h_rate_back->Fill(tpe);
      double MaxADC=0;
      for (auto const& adc:(*HitSADC))
        if (MaxADC<adc)MaxADC=adc;
      h_maxADChit_back_wire->Fill(MaxADC);
    } else {
      nDetectedEvent[std::make_pair(Event,MarleyIndex)] = true;
      //map_gentype_nhit_sign = GetMapOfHit(HitGenType);
      double MaxADC=0;
      for (auto const& adc:(*HitSADC))
        if (MaxADC<adc)MaxADC=adc;
      h_maxADChit_sign_wire->Fill(MaxADC);
      ++ncluster_sign;
    }
    // for (auto const& genhit: map_gentype_nhit_sign) p_gentype_sign_wire->Fill(genhit.first, genhit.second);
    // for (auto const& genhit: map_gentype_nhit_back) p_gentype_back_wire->Fill(genhit.first, genhit.second);

    bool fillneutron=map_gentype_nhit_sign[kNeutron]>0;
    if (fillneutron)
      for (auto const& genhit: map_gentype_nhit_back)
        p_gentype_back_neut_wire->Fill(genhit.first, genhit.second);

    fillneutron=map_gentype_nhit_sign[kNeutron]>0;
    if (fillneutron)
      for (auto const& genhit: map_gentype_nhit_sign)
        p_gentype_sign_neut_wire->Fill(genhit.first, genhit.second);
  }

  int ntotaldetected=0;
  for (auto const& it: nDetectedEvent) {
    (void)it;
    ntotaldetected++;
  }
  std::cout <<"totaldetected " << ntotaldetected << std::endl;
  std::cout <<"Efficiency " << (double)ntotaldetected / nMarleyEvent << std::endl;
  h_ncluster_sign_wire->Fill(ncluster_sign);
  h_ncluster_back_wire->Fill(ncluster_back);
  
  gPad->SetLogy(true);
  
  max = {p_gentype_sign_wire->GetMaximum(),
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

  p_gentype_sign_neut_wire->Draw("E");
  p_gentype_back_neut_wire->Draw("E SAME");
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
  for (int i=3; i<=10; i++){
    std::cout <<h_rate_back->GetXaxis()->GetBinLabel(i) << " : " << h_rate_back->GetBinContent(i) << std::endl;
  }
  h_rate_back->GetXaxis()->SetRangeUser(2,10);
  c.Print(OutputFile.c_str());

  max = {h_ncluster_sign_wire->GetMaximum(),
         h_ncluster_back_wire->GetMaximum()};

  h_ncluster_sign_wire->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  h_ncluster_sign_wire->SetMinimum(0.1);
  gPad->SetLogx();

  h_ncluster_sign_wire->Draw("");
  h_ncluster_back_wire->Draw("SAME");
  c.Print(OutputFile.c_str());

  gPad->SetLogx(false);

  h_maxADChit_sign_wire->Draw();
  h_maxADChit_back_wire->Draw("SAME");
  c.Print(OutputFile.c_str());

  gPad->Clear();
  TLegend* leg = new TLegend(0.1, 0.1, 0.9, 0.9);
  leg->AddEntry(p_gentype_sign_wire, "signal wire clusters");
  leg->AddEntry(p_gentype_back_wire, "background wire clusters");
  leg->Draw();
  c.Print(OutputFile.c_str());
  c.Print((OutputFile+"]").c_str());
  return 1;

}
