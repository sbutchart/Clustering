#include <algorithm>    // std::min_element, std::max_element
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "Helper.h"
#include "BurstEfficiencyCalculator.hh"

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
  int nHitCutMin=0;
  while ( (opt = getopt(argc, argv, "c:h:o:i:n:s:r:b")) != -1 ) {  // for each option...
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
  if(Start == 0 ) Start =  nHitCutMin;
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

  TH1D* h_nhit_sign_opti = new TH1D("h_nhit_sign_opti", ";n Hits;Clusters", Range, 0, Range);
  TH1D* h_nhit_back_opti = new TH1D("h_nhit_back_opti", ";n Hits;Clusters", Range, 0, Range);
  h_nhit_sign_opti->SetLineColor(kRed);
  h_nhit_back_opti->SetLineColor(kBlue);
  h_nhit_sign_opti->SetMinimum(0.1);
  h_nhit_back_opti->SetMinimum(0.1);
  h_nhit_sign_opti->SetLineStyle(1);
  h_nhit_back_opti->SetLineStyle(1);
  h_nhit_sign_opti->SetLineWidth(2);
  h_nhit_back_opti->SetLineWidth(2);

  t_ClusteredOptHit->Project("h_nhit_sign_opti", "NHits", Form("Type==1 && Config==%i", RequestedConfig));
  t_ClusteredOptHit->Project("h_nhit_back_opti", "NHits", Form("Type==0 && Config==%i", RequestedConfig));
  double threshold = GetStatThreshold(h_nhit_sign_opti,h_nhit_back_opti);

  TF1 *f1 = new TF1("double_exp","[0]*exp([1]*x)",0,Range);
  f1->SetLineWidth(2);
  f1->SetLineColor(kViolet);
  f1->SetParameter(0, 100);
  f1->SetParameter(1, -0.01);
    
  TFitResultPtr r = h_nhit_back_opti->Fit("double_exp", "", "", nHitCutMin, Range);

  TF1 *f2 = new TF1("double_exp","[0]*exp([1]*x)",0,Range);
  f2->SetLineWidth(2);
  f2->SetLineColor(kRed);
  f2->SetParameter(0, 2.*f1->GetParameter(0));
  f2->SetParameter(1,    f1->GetParameter(1));

  std::cout << "Fitting from " << std::endl;
  std::cout << " - Min " << nHitCutMin << std::endl;
  std::cout << " - Max " << threshold  << std::endl;
  
  TCanvas c;
  c.Print((OutputFile+"[").c_str());
  h_nhit_back_opti->Draw("");
  f1->Draw("SAME");
  f2->Draw("SAME");
  
  TLine line2(threshold,  h_nhit_back_opti->GetMinimum(), threshold,  h_nhit_back_opti->GetMaximum());
  TLine line3(nHitCutMin, h_nhit_back_opti->GetMinimum(), nHitCutMin, h_nhit_back_opti->GetMaximum());
  line2.SetLineWidth(2);
  line3.SetLineWidth(2);
  line2.Draw();
  line3.Draw();
  gPad->SetLogy();
  c.Print(OutputFile.c_str());
  c.Print("Fit.pdf");
  if (r) {
    std::cout << "Fit failed!" << std::endl;
    c.Print((OutputFile+"]").c_str());
    exit(1);
  }

  std::vector <double> max;
  int EventIterator = 0;
  std::map<int,bool> totalDetectedEvent;
  std::map<int,int>  totalBackCluster;
  std::map<int,int>  totalSignCluster;
  std::map<int,bool> detectedEvent;
  std::map<int,int>  backCluster;
  std::map<int,int>  signCluster;

  for (int i=0; i<100; i++) {
    detectedEvent[i] = false;
    backCluster[i] = 0;
    signCluster[i] = 0;
  }
  
  std::map<int, TArrow*> ArrowMap;
  TH1D* h_nhit_LMCEff = new TH1D("h_nhit_LMCEff", ";n Hits cut;LMC efficiency", Range, 0, Range);
  
  for (int nHitCut=Start; nHitCut<Range; nHitCut=nHitCut+1) {
    std::cout << "Trying nhit cut = " << nHitCut << std::endl;
    totalDetectedEvent.clear();
    totalBackCluster  .clear();
    totalSignCluster  .clear();
    detectedEvent     .clear();
    backCluster       .clear();
    signCluster       .clear();
    bool resetfirst=false;
    int Divisor=1;
  
    //Divisor=100;
    //if (nHitCut<10) Divisor=100;
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
          detectedEvent[i] = false;
          backCluster[i] = 0;
          signCluster[i] = 0;
          EventIterator++;
        }
      }
    
      if (NHits<nHitCut) continue;

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
    }
  
    size_t nTotalEvent=0;
    size_t nDetectedEvent=0;
    size_t nBackgroundCluster=0;
    for (auto const& it:totalDetectedEvent) {
      nTotalEvent++;
      if (it.second) {
        nDetectedEvent++;
      }
    }
    if (!BadFile) nTotalEvent=nEvent;
    std::cout << "nTotalEvent " << nTotalEvent << std::endl;
    std::cout << "nDetectedEvent " << nDetectedEvent << std::endl;
    if (nDetectedEvent<1) break;
    std::cout << "Overall efficiency " << (double)nDetectedEvent / (double)nTotalEvent << std::endl;
  
    for (auto const& it:totalBackCluster)
      nBackgroundCluster += it.second;

    std::cout << "Background rate from counting " << (double)nBackgroundCluster / (double)nTotalEvent / 4.492e-3 / 0.12 << std::endl;
    std::cout << "Background rate from histo " <<  h_nhit_back_opti->Integral(nHitCut+1, Range) / (double)nTotalEvent / 4.492e-3 / 0.12 << std::endl;
    std::cout << "Background rate from f1 " <<  f1->Integral(nHitCut+1, Range) / (double)nTotalEvent / 4.492e-3 / 0.12 << std::endl;
    std::cout << "Background rate from f2 " <<  f2->Integral(nHitCut+1, Range) / (double)nTotalEvent / 4.492e-3 / 0.12 << std::endl;

    BurstEfficiencyCalculator bec;
    bec.SetMaxFake(1./3600./24./31);
    bec.SetEfficiencyMarley((double)nDetectedEvent / (double)nTotalEvent);
    if (nHitCut<threshold) {
      bec.SetBackgroundRate  ((double)nBackgroundCluster / (double)nTotalEvent / 4.492e-3 / 0.12);
    } else {
      bec.SetBackgroundRate  (f2->Integral(nHitCut+1, Range) / (double)nTotalEvent / 4.492e-3 / 0.12);
    }
    bec.SetTimeWindow(10.);
    double eff = bec.GetEfficiencyAtNBurst();
    if (bec.GetThreshold()>25)
      nHitCut=nHitCut+10;
    
    std::cout << "Eff at nhit cut = " << nHitCut << " is " << eff << std::endl;

    if (!ArrowMap[bec.GetThreshold()] && bec.GetThreshold() < 10)
      ArrowMap[bec.GetThreshold()] = new TArrow(nHitCut, 1, nHitCut, 10, 0.05, "|>");
    
    if (eff < 0.99) {
      h_nhit_LMCEff->SetBinContent(nHitCut, eff);
    } else {
      break;
    }
    h_nhit_LMCEff->SetBinError(nHitCut, 0);
    if (bec.GetThreshold() == 0) break;
  }
  TH1D* son = DoIntegratedSignalOverNoise(h_nhit_sign_opti, h_nhit_back_opti);
  max = {h_nhit_sign_opti->GetMaximum(),
         h_nhit_back_opti->GetMaximum()};
  h_nhit_sign_opti->SetMaximum((*std::max_element(max.begin(),max.end()))*2);
  h_nhit_sign_opti->SetMinimum(0.1);
  gPad->SetLogy();
  son->SetLineWidth(2);
  son->SetLineColor(kCyan);
  son->SetMarkerColor(kCyan);
  h_nhit_LMCEff->SetLineWidth(2);
  h_nhit_LMCEff->SetLineColor(kViolet);  
  h_nhit_LMCEff->SetMarkerColor(kViolet);
  
  std::cout << "Threshold for nhit cut: " << threshold << std::endl;
  std::cout << "Corresponding to a S/N: " << son->GetBinContent(son->FindBin(threshold)) << std::endl;
  c.Clear();
  c.cd();
  TPad *pad1 = new TPad("pad1", "The pad 80% of the height",0.0,0.5,1.0,1.0);
  pad1->SetLogy();
  pad1->Draw();
  pad1->SetGridx();
  pad1->SetGridy();
  pad1->cd();
  pad1->SetBottomMargin(0);
  h_nhit_sign_opti->SetStats(0);
  h_nhit_back_opti->SetStats(0);
  h_nhit_sign_opti->GetXaxis()->SetTitleSize(h_nhit_sign_opti->GetXaxis()->GetTitleSize()/.5);
  h_nhit_sign_opti->GetXaxis()->SetLabelSize(h_nhit_sign_opti->GetXaxis()->GetLabelSize()/.5);
  h_nhit_sign_opti->GetYaxis()->SetTitleSize(h_nhit_sign_opti->GetYaxis()->GetTitleSize()/.5);
  h_nhit_sign_opti->GetYaxis()->SetLabelSize(h_nhit_sign_opti->GetYaxis()->GetLabelSize()/.5);
  h_nhit_sign_opti->GetYaxis()->SetTitleOffset(h_nhit_sign_opti->GetYaxis()->GetTitleOffset()*.3);
  h_nhit_sign_opti->Draw("");
  h_nhit_back_opti->Draw("SAME");
  f1->Draw("SAME");
  f2->Draw("SAME");
  son->Draw("SAME E");
  TLine* line = new TLine(threshold,h_nhit_sign_opti->GetMinimum(),
                          threshold,h_nhit_sign_opti->GetMaximum());
  line->SetLineColor(kBlack);
  line->SetLineWidth(2);
  line->Draw();
  c.cd();

  TPad *pad2 = new TPad("pad2", "The pad 20% of the height",0.0,0.0,1.0,0.5);
  pad2->Draw();
  pad2->SetGridx();
  pad2->SetGridy();
  pad2->SetLogy();
  pad2->cd();
  pad2->SetTopMargin(0);
  pad2->SetBottomMargin(pad2->GetBottomMargin()/0.5);
  h_nhit_LMCEff->SetStats(0);
  h_nhit_LMCEff->SetMinimum(0.01);
  h_nhit_LMCEff->SetMaximum(2);
  h_nhit_LMCEff->GetXaxis()->SetTitleSize(h_nhit_LMCEff->GetXaxis()->GetTitleSize()/.5);
  h_nhit_LMCEff->GetXaxis()->SetLabelSize(h_nhit_LMCEff->GetXaxis()->GetLabelSize()/.5);
  h_nhit_LMCEff->GetYaxis()->SetTitleSize(h_nhit_LMCEff->GetYaxis()->GetTitleSize()/.5);
  h_nhit_LMCEff->GetYaxis()->SetTitleOffset(h_nhit_LMCEff->GetYaxis()->GetTitleOffset()*.01);
  h_nhit_LMCEff->GetYaxis()->SetNoExponent();
  h_nhit_LMCEff->GetYaxis()->SetLabelSize(h_nhit_LMCEff->GetYaxis()->GetLabelSize()/.5);
  h_nhit_LMCEff->Draw();
  // for (auto& it: ArrowMap) {
  //   if (it.second) {
  //     it.second->SetY1(h_nhit_LMCEff->GetMinimum());
  //     it.second->SetY2(h_nhit_LMCEff->GetMinimum()*2);
  //     it.second->Draw();
  //     TText* text = new TText(it.second->GetX1(), it.second->GetY1()*4,Form("%i",it.first));
  //     text->SetTextAngle(90);
  //     text->Draw();
  //   }
  // }
  c.Print(OutputFile.c_str());
  c.Clear();
  c.cd(1);
  gPad->Clear();
  TLegend* leg = new TLegend(0.1, 0.1, 0.9, 0.9);
  leg->AddEntry(h_nhit_sign_opti, "Signal optical clusters");
  leg->AddEntry(h_nhit_back_opti, "Background optical clusters");
  leg->AddEntry(f1, "Background optical clusters fit");
  leg->AddEntry(f2, "Background optical clusters fit * 2");
  leg->AddEntry(son, "Signal over noise ratio (not extrapolated)");
  leg->AddEntry(h_nhit_LMCEff, "Efficiency to detect a SN of 10 events (extrapolated)");
  leg->Draw();
  c.Print(OutputFile.c_str());
  c.Print((OutputFile+"]").c_str());

  TFile f((OutputFile+".root").c_str(), "RECREATE");
  f.cd();
  h_nhit_sign_opti->Write();
  h_nhit_back_opti->Write();
  f1->Write();
  son->Write();
  h_nhit_LMCEff->Write();
  f.Close();
  
  return 1;

}

