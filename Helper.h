#ifndef HELPER_HH
#define HELPER_HH

#include "TCanvas.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TFile.h"
#include "TRandom.h"
#include "TTree.h"
#include "TGraph.h"
#include "TGraphAsymmErrors.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGaxis.h"
#include "TPad.h"
#include "TProfile.h"
#include "TEfficiency.h"

#include <algorithm>
#include <iostream>

enum GenType{
  kOther=0,
  kSNnu=1,
  kAPA,
  kCPA,
  kAr39,
  kNeutron,
  kKrypton,
  kPolonium,
  kRadon,
  kAr42,
  kAllBackground,
  kAll            
};
static int CurrentProg=0;

inline void PrintProgress(int iter, int nloop){

  if(iter+1==nloop){
    std::cout << std::endl;
    return;
  }
  double progress = (double)iter/(double)nloop;

  if(100.*progress >= CurrentProg)
  {
    CurrentProg=(int)ceil(100.*progress);
    int barWidth = 70;
    std::cout << "[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
      if (i < pos) std::cout << "=";
      else if (i == pos) std::cout << ">";
      else std::cout << " ";
    }
    std::cout << "] " << int(progress * 100.0) << " % (" << iter << " of " << nloop << ")\r";
    std::cout.flush();
  }
};

inline GenType ConvertIntToGenType(int i) {
  switch (i) {
  case  0: return kOther;
  case  1: return kSNnu;
  case  2: return kAPA;
  case  3: return kCPA;
  case  4: return kAr39;
  case  5: return kNeutron;
  case  6: return kKrypton;
  case  7: return kPolonium;
  case  8: return kRadon;
  case  9: return kAr42;         
  case 10: return kAllBackground;
  case 11: return kAll;
  }
  return kOther;
};


class Helper{
public:
  Helper(){
    GenName[kSNnu] =          "SN#nu"         ;
    GenName[kAPA] =           "APA"           ;
    GenName[kCPA] =           "CPA"           ;
    GenName[kAr39] =          "Ar39"          ;
    GenName[kNeutron] =       "Neutron"       ;
    GenName[kKrypton] =       "Krypton"       ;
    GenName[kPolonium] =      "Polonium"      ;
    GenName[kRadon] =         "Radon"         ;
    GenName[kAr42] =          "Ar42"          ;
    GenName[kOther] =         "Other (not BT)";
    GenName[kAllBackground] = "AllBackground" ;
    GenName[kAll] =           "All"           ;
    
    ShortGenName[kSNnu] =          "SNnu"          ;
    ShortGenName[kAPA] =           "APA"           ;
    ShortGenName[kCPA] =           "CPA"           ;
    ShortGenName[kAr39] =          "Ar39"          ;
    ShortGenName[kNeutron] =       "Neutron"       ;
    ShortGenName[kKrypton] =       "Krypton"       ;
    ShortGenName[kPolonium] =      "Polonium"      ;
    ShortGenName[kRadon] =         "Radon"         ;
    ShortGenName[kAr42] =          "Ar42"          ;
    ShortGenName[kOther] =         "Other"         ;
    ShortGenName[kAllBackground] = "AllBackground" ;
    ShortGenName[kAll] =           "All"           ;
  };
  std::map<GenType,std::string> GenName;
  std::map<GenType,std::string> ShortGenName;
   

};


inline std::map<GenType,TH1D*> GetHistos(std::string name, std::string title,
                                         int nbins, double xbinmin, double xbinmax){
  std::map<GenType,TH1D*> map_h;
  std::vector<int>vec_colors = getColors(0);
  int i=0;
  Helper h;
  for(auto& it : h.ShortGenName){
    map_h[it.first] = new TH1D(Form("%s_%s",it.second.c_str(), name.c_str()),
                               Form("%s %s",it.second.c_str(), title.c_str()),
                               nbins, xbinmin, xbinmax);
    map_h[it.first]->SetLineWidth(2);
    map_h[it.first]->SetStats(0);
    map_h[it.first]->SetLineColor(vec_colors[i++]);
  }
  return map_h;
};

inline std::map<GenType,TH2D*> Get2DHistos(std::string name, std::string title,
                                           int nbinsx, double xbinmin, double xbinmax,
                                           int nbinsy, double ybinmin, double ybinmax){
  std::map<GenType,TH2D*> map_h;
  int i=0;
  Helper h;
  for(auto& it : h.ShortGenName){
    map_h[it.first] = new TH2D(Form("%s_%s",it.second.c_str(), name.c_str()),
                               Form("%s %s",it.second.c_str(), title.c_str()),
                               nbinsx, xbinmin, xbinmax,
                               nbinsy, ybinmin, ybinmax);
    map_h[it.first]->SetStats(0);
  }
  return map_h;
};


inline TH1D* GetSimpleHisto(std::string name, std::string title){
  Helper help;
  TH1D* h = new TH1D(name.c_str(), title.c_str(), help.GenName.size(), -0.5, (double)help.GenName.size()-0.5);

  for(auto const& it : help.ShortGenName)
    h->GetXaxis()->SetBinLabel(it.first+1, it.second.c_str());
  return h;
};

inline TLegend* GetNewLegend(double xmin=0.1, double ymin=0.1, double xmax=0.9, double ymax=0.9,
                      const std::map<GenType,TH1D*>& histos=std::map<GenType,TH1D*>()){

  // if(histos.size()==0)
  //   histos = GetHistos("","",1,0,1);
  Helper h;

  TLegend* leg = new TLegend(xmin, ymin, xmax, ymax);
  for(auto const& it : histos)
    leg->AddEntry(it.second, h.GenName[it.first].c_str(), "L");
  return leg;
};


inline void AddOverflow(TH1D* histo){
  histo->SetBinContent(histo->GetXaxis()->GetNbins(),
                       histo->GetBinContent(histo->GetXaxis()->GetNbins()  )+
                       histo->GetBinContent(histo->GetXaxis()->GetNbins()+1));
  histo->SetBinContent(histo->GetXaxis()->GetNbins()+1,0);
};
inline void AddOverflow(TH1D& histo){
  histo.SetBinContent(histo.GetXaxis()->GetNbins(),
                      histo.GetBinContent(histo.GetXaxis()->GetNbins()  )+
                      histo.GetBinContent(histo.GetXaxis()->GetNbins()+1));
  histo.SetBinContent(histo.GetXaxis()->GetNbins()+1,0);
};
#endif
