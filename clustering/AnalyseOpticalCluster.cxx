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
#include "TLine.h"
#include "TTree.h"
#include "TLegend.h"
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

  std::cout << "Running over " << nEvent << " events." << std::endl;

  
  std::map<int,int> map_Event_TrueEntry;
  int nMarleyEvent=0;
  for (int i=0; i<nEvent; ++i) {
    t_TrueInfo->GetEntry(i);
    map_Event_TrueEntry[EventTrue] = i;
    nMarleyEvent += (int)ENu->size();
  }
  
  std::cout << "I will look at the config " << RequestedConfig << std::endl;
  std::cout << "There were " << nMarleyEvent << " Marley events in " << map_Event_TrueEntry.size() << " Larsoft events." << std::endl;

  std::map<int,std::vector<int> > map_event_entry_opti;

  std::cout << t_ClusteredOptHit->GetEntries() << " optical clusters were saved." << std::endl;
  for (int i=0; i<t_ClusteredOptHit->GetEntries(); ++i) {
    t_ClusteredOptHit->GetEntry(i);
    if (Config==RequestedConfig && map_Event_TrueEntry.find(Event)!=map_Event_TrueEntry.end())
      map_event_entry_opti[Event].push_back(i);
  }
  
  std::cout << "There were " << map_event_entry_opti.size()
            << " events in the optical cluster tree." << std::endl;
  TCanvas c;
  c.Print((OutputFile+"[").c_str());
  TProfile* p_gentype_sign_opti = SetUpTProfileGenType("p_gentype_sign_opti", ";;average number of hits in cluster");
  TProfile* p_gentype_back_opti = SetUpTProfileGenType("p_gentype_back_opti", ";;average number of hits in cluster");
  p_gentype_sign_opti->SetLineColor(kRed);
  p_gentype_back_opti->SetLineColor(kBlue);
  p_gentype_sign_opti->SetLineStyle(1);
  p_gentype_back_opti->SetLineStyle(1);
  p_gentype_sign_opti->SetLineWidth(2);
  p_gentype_back_opti->SetLineWidth(2);


  TH1D* h_ncluster_sign_opti = new TH1D("h_ncluster_sign_opti", ";n clusters;Events", 10, -0.5, 9.5);
  TH1D* h_ncluster_back_opti = new TH1D("h_ncluster_back_opti", ";n clusters;Events", 10, -0.5, 9.5);
  h_ncluster_sign_opti->SetLineColor(kRed);
  h_ncluster_back_opti->SetLineColor(kBlue);
  h_ncluster_sign_opti->SetLineStyle(1);
  h_ncluster_back_opti->SetLineStyle(1);
  h_ncluster_sign_opti->SetLineWidth(2);
  h_ncluster_back_opti->SetLineWidth(2);

  TH1D* h_nhit_sign_opti = new TH1D("h_nhit_sign_opti", ";n Hits;Clusters", 300, -0.5, 299.5);
  TH1D* h_nhit_back_opti = new TH1D("h_nhit_back_opti", ";n Hits;Clusters", 300, -0.5, 299.5);
  h_nhit_sign_opti->SetLineColor(kRed);
  h_nhit_back_opti->SetLineColor(kBlue);
  h_nhit_sign_opti->SetLineStyle(1);
  h_nhit_back_opti->SetLineStyle(1);
  h_nhit_sign_opti->SetLineWidth(2);
  h_nhit_back_opti->SetLineWidth(2);

  TEfficiency* eff_enu_opti  = new TEfficiency("eff_enu_opti",  ";E#nu [MeV];Efficiency", 30, 0, 60);

  t_ClusteredOptHit->Project("h_nhit_sign_opti", "NHits", Form("Type==1 && Config==%i && NHits>=%i && SumPE>=%i", RequestedConfig, nHitCut, nPECut));
  t_ClusteredOptHit->Project("h_nhit_back_opti", "NHits", Form("Type==0 && Config==%i && NHits>=%i && SumPE>=%i", RequestedConfig, nHitCut, nPECut));
  TH1D* son = DoIntegratedSignalOverNoise(h_nhit_sign_opti, h_nhit_back_opti);
  std::vector<double> max = {h_nhit_sign_opti->GetMaximum(),
                             h_nhit_back_opti->GetMaximum()};
  h_nhit_sign_opti->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  h_nhit_sign_opti->SetMinimum(0.1);
  gPad->SetLogy();
  son->SetLineWidth(2);
  son->SetLineColor(kCyan);
  son->SetMarkerColor(kCyan);
  double threshold = GetStatThreshold(h_nhit_sign_opti, h_nhit_back_opti);
  std::cout << "Threshold for nhit cut: " << threshold << std::endl;
  std::cout << "Corresponding to a S/N: " << son->GetBinContent(son->FindBin(threshold)) << std::endl;
  h_nhit_sign_opti->Draw("");
  h_nhit_back_opti->Draw("SAME");
  son->Draw("SAME E");
  TLine* line = new TLine(threshold,h_nhit_sign_opti->GetMinimum(),
                          threshold,h_nhit_sign_opti->GetMaximum());
  line->SetLineColor(kBlack);
  line->SetLineWidth(2);
  line->Draw();
  c.Print(OutputFile.c_str());

  TH1D* h_npe_sign_opti = new TH1D("h_npe_sign_opti", ";n PEs;Clusters", 200, -0.5, 199.5);
  TH1D* h_npe_back_opti = new TH1D("h_npe_back_opti", ";n PEs;Clusters", 200, -0.5, 199.5);
  h_npe_sign_opti->SetLineColor(kRed);
  h_npe_back_opti->SetLineColor(kBlue);
  h_npe_sign_opti->SetLineStyle(1);
  h_npe_back_opti->SetLineStyle(1);
  h_npe_sign_opti->SetLineWidth(2);
  h_npe_back_opti->SetLineWidth(2);

  t_ClusteredOptHit->Project("h_npe_sign_opti", "SumPE", Form("Type==1 && Config==%i && NHits>=%i && SumPE>=%i", RequestedConfig, nHitCut, nPECut));
  t_ClusteredOptHit->Project("h_npe_back_opti", "SumPE", Form("Type==0 && Config==%i && NHits>=%i && SumPE>=%i", RequestedConfig, nHitCut, nPECut));
  son = DoIntegratedSignalOverNoise(h_npe_sign_opti, h_npe_back_opti);
  son->SetLineWidth(2);
  son->SetLineColor(kCyan);
  son->SetMarkerColor(kCyan);
  max = {h_npe_sign_opti->GetMaximum(),
         h_npe_back_opti->GetMaximum()};
  h_npe_sign_opti->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  h_npe_sign_opti->SetMinimum(0.1);
  gPad->SetLogy();
  h_npe_sign_opti->SetBinContent(h_npe_sign_opti->GetXaxis()->GetNbins(),
                                 h_npe_sign_opti->GetBinContent(h_npe_sign_opti->GetXaxis()->GetNbins()+1));
  threshold = GetStatThreshold(h_npe_sign_opti, h_npe_back_opti);
  std::cout << "Threshold for nPE cut: " << threshold << std::endl;
  std::cout << "Corresponding to a S/N: " << son->GetBinContent(son->FindBin(threshold)) << std::endl;
  delete line;
  h_npe_sign_opti->Draw("");
  h_npe_back_opti->Draw("SAME");
  son->Draw("SAME E");
  line = new TLine(threshold,h_npe_sign_opti->GetMinimum(),
                   threshold,h_npe_sign_opti->GetMaximum());
  line->SetLineColor(kBlack);
  line->SetLineWidth(2);
  line->Draw();
  c.Print(OutputFile.c_str());
  
  TH1D* h_twidth_sign_opti = new TH1D("h_twidth_sign_opti", ";Time Width [#mus];Clusters", 50,0, 20);
  TH1D* h_twidth_back_opti = new TH1D("h_twidth_back_opti", ";Time Width [#mus];Clusters", 50,0, 20);
  h_twidth_sign_opti->SetLineColor(kRed);
  h_twidth_back_opti->SetLineColor(kBlue);
  h_twidth_sign_opti->SetLineStyle(1);
  h_twidth_back_opti->SetLineStyle(1);
  h_twidth_sign_opti->SetLineWidth(2);
  h_twidth_back_opti->SetLineWidth(2);

  t_ClusteredOptHit->Project("h_twidth_sign_opti", "TimeWidth", Form("Type==1 && Config==%i && NHits>=%i && SumPE>=%i", RequestedConfig, nHitCut, nPECut));
  t_ClusteredOptHit->Project("h_twidth_back_opti", "TimeWidth", Form("Type==0 && Config==%i && NHits>=%i && SumPE>=%i", RequestedConfig, nHitCut, nPECut));
  h_twidth_sign_opti->SetStats(0);
  h_twidth_back_opti->SetStats(0);
  AddOverflow(h_twidth_sign_opti);
  AddOverflow(h_twidth_back_opti);
  max = {h_twidth_sign_opti->GetMaximum(),
         h_twidth_back_opti->GetMaximum()};
  h_twidth_sign_opti->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  h_twidth_sign_opti->SetMinimum(0.1);
  gPad->SetLogy();
  h_twidth_sign_opti->Draw("");
  h_twidth_back_opti->Draw("SAME");
  c.Print(OutputFile.c_str());
  
  TH1D* h_width_sign_opti = new TH1D("h_width_sign_opti", ";Z Width [cm];Clusters", 6, 0., 6.*232.39);
  TH1D* h_width_back_opti = new TH1D("h_width_back_opti", ";Z Width [cm];Clusters", 6, 0., 6.*232.39);
  h_width_sign_opti->SetLineColor(kRed);
  h_width_back_opti->SetLineColor(kBlue);
  h_width_sign_opti->SetLineStyle(1);
  h_width_back_opti->SetLineStyle(1);
  h_width_sign_opti->SetLineWidth(2);
  h_width_back_opti->SetLineWidth(2);

  t_ClusteredOptHit->Project("h_width_sign_opti", "ZWidth", Form("Type==1 && Config==%i && NHits>=%i && SumPE>=%i", RequestedConfig, nHitCut, nPECut));
  t_ClusteredOptHit->Project("h_width_back_opti", "ZWidth", Form("Type==0 && Config==%i && NHits>=%i && SumPE>=%i", RequestedConfig, nHitCut, nPECut));
  max = {h_width_sign_opti->GetMaximum(),
         h_width_back_opti->GetMaximum()};
  h_width_sign_opti->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  h_width_sign_opti->SetMinimum(0.1);
  gPad->SetLogy();
  h_width_sign_opti->Draw("");
  h_width_back_opti->Draw("SAME");
  c.Print(OutputFile.c_str());

  TH1D* h_ywidth_sign_opti = new TH1D("h_ywidth_sign_opti", ";Y Width [cm];Clusters", 25, 0., 25.*62.3567);
  TH1D* h_ywidth_back_opti = new TH1D("h_ywidth_back_opti", ";Y Width [cm];Clusters", 25, 0., 25.*62.3567);
  h_ywidth_sign_opti->SetLineColor(kRed);
  h_ywidth_back_opti->SetLineColor(kBlue);
  h_ywidth_sign_opti->SetLineStyle(1);
  h_ywidth_back_opti->SetLineStyle(1);
  h_ywidth_sign_opti->SetLineWidth(2);
  h_ywidth_back_opti->SetLineWidth(2);

  t_ClusteredOptHit->Project("h_ywidth_sign_opti", "YWidth", Form("Type==1 && Config==%i && NHits>=%i && SumPE>=%i", RequestedConfig, nHitCut, nPECut));
  t_ClusteredOptHit->Project("h_ywidth_back_opti", "YWidth", Form("Type==0 && Config==%i && NHits>=%i && SumPE>=%i", RequestedConfig, nHitCut, nPECut));
  max = {h_ywidth_sign_opti->GetMaximum(),
         h_ywidth_back_opti->GetMaximum()};
  h_ywidth_sign_opti->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  h_ywidth_sign_opti->SetMinimum(0.1);
  h_ywidth_sign_opti->Draw("");
  h_ywidth_back_opti->Draw("SAME");
  c.Print(OutputFile.c_str());

  int total_selected = 0;
  for(auto const& it : map_Event_TrueEntry) {
    int ncluster_sign=0;
    int ncluster_back=0;
    bool SignDetected=false;
    if (map_event_entry_opti[it.first].size() == 0)
      std::cout << "No optical cluster in this event (" << it.first << ") -> if this happens very often there is probably something wrong" <<  std::endl;

    for (auto const& it2 : map_event_entry_opti[it.first]) {
      std::map<int, int> map_gentype_nhit_sign;
      std::map<int, int> map_gentype_nhit_back;
      t_ClusteredOptHit->GetEntry(it2);
      if (NHits<nHitCut || SumPE<nPECut) continue;
      if (Type==0) {
        map_gentype_nhit_back = GetMapOfHit(PDSHit_GenType);
        ++ncluster_back;
      } else {
        SignDetected=true;
        map_gentype_nhit_sign = GetMapOfHit(PDSHit_GenType);
        ++ncluster_sign;
      }
      for (auto const& genhit: map_gentype_nhit_sign) p_gentype_sign_opti->Fill(genhit.first, genhit.second);
      for (auto const& genhit: map_gentype_nhit_back) p_gentype_back_opti->Fill(genhit.first, genhit.second);
    }
    t_TrueInfo->GetEntry(it.second);
    double Energy_Neutrino = ENu->at(0);
    eff_enu_opti->Fill(SignDetected, Energy_Neutrino*1000.);
    h_ncluster_sign_opti->Fill(ncluster_sign);
    h_ncluster_back_opti->Fill(ncluster_back);

    if (ncluster_sign>0)
      total_selected++;
  }
  std::cout << "Overall efficiency " << (double)total_selected / map_Event_TrueEntry.size() << std::endl;
  gPad->SetLogy(false);
  eff_enu_opti->Draw();
  c.Print(OutputFile.c_str());
  gPad->SetLogy(true);

  max = {p_gentype_sign_opti->GetMaximum(),
         p_gentype_back_opti->GetMaximum()};
  
  p_gentype_sign_opti->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  p_gentype_sign_opti->SetMinimum(0.1);
  gPad->SetGridx();
  gPad->SetGridy();
  p_gentype_sign_opti->SetStats(0);
  p_gentype_sign_opti->Draw("E");
  p_gentype_back_opti->Draw("E SAME");
  c.Print(OutputFile.c_str());
  gPad->SetGridx(false);
  gPad->SetGridy(false);


  max = {h_ncluster_sign_opti->GetMaximum(),
         h_ncluster_back_opti->GetMaximum()};

  h_ncluster_sign_opti->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  h_ncluster_sign_opti->SetMinimum(0.1);
  // gPad->SetLogx();

  // auto axis = h_ncluster_sign_opti->GetXaxis();
  // axis->SetLimits(0.5,1000); 
  h_ncluster_sign_opti->Draw("");
  h_ncluster_back_opti->Draw("SAME");
  c.Print(OutputFile.c_str());
  
  gPad->Clear();
  TLegend* leg = new TLegend(0.1, 0.1, 0.9, 0.9);
  leg->AddEntry(p_gentype_sign_opti, "signal optical clusters");
  leg->AddEntry(p_gentype_back_opti, "background optical clusters");
  leg->Draw();
  c.Print(OutputFile.c_str());
  c.Print((OutputFile+"]").c_str());

  TFile f((OutputFile+".root").c_str(), "RECREATE");
  f.cd();
  leg->Write();
  


  
  return 1;

}
