#ifndef HELPER_HH
#define HELPER_HH

#include "TColor.h"
#include "TH1.h"
#include "TPad.h"
#include "TList.h"
#include "TClass.h"

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
#include <cassert>
#include <iostream>
#include <limits>
#include <unistd.h>
#include <vector>
#include <sstream>

class Helper;
inline const std::vector<int>& getColors(int);


inline double GetMinNonZeroBinContent(const TH1D* h_){
  double min = std::numeric_limits<double>::max();
  for (int i=0; i<h_->GetNbinsX(); ++i){
    if (h_->GetBinContent(i) <= 0) continue;
    if (h_->GetBinContent(i) < min) {
      min=h_->GetBinContent(i);
    }
  }
  return min;
  
}
inline void PlotThese2Histos(TH1D* h1_, TH1D* h2_, TCanvas& c_, std::string filename,
                             bool logx=false, bool logy=false, double maxv=10, double minv=0.1){
  
  std::vector<double> max = {h1_->GetBinContent(h1_->GetMaximumBin()),
                             h2_->GetBinContent(h2_->GetMaximumBin()),
                             maxv};
  std::vector<double> min = {GetMinNonZeroBinContent(h1_),
                             GetMinNonZeroBinContent(h2_),
                             minv};
  double ScaleMax=1.2, ScaleMin=0.;
  if (logy) {
    ScaleMax = 5;
    ScaleMin = 0.1;
    for (auto it = min.begin(); it!=min.end();)
      if (*it == 0) {
        it = min.erase(it);
      } else {
        it++;
      }
  }
  h1_->SetStats(0);
  h2_->SetStats(0);
  h1_->SetMaximum((*std::max_element(max.begin(),max.end()))*ScaleMax);
  h1_->SetMinimum((*std::min_element(min.begin(),min.end()))*ScaleMin);
  gPad->SetLogx(logx);
  gPad->SetLogy(logy);
  h1_->SetTitle("");
  h1_->Draw();
  h2_->Draw("SAME");
  c_.Print(filename.c_str());
  gPad->SetLogx(false);
  gPad->SetLogy(false);
}



inline void PlotThese2Histos(TH2D* h1_, TH2D* h2_, TCanvas& c_, std::string filename,
                             bool logx=false, bool logy=false, bool logz=false){
  h1_->SetStats(0);
  h2_->SetStats(0);
  gPad->SetLogx(logx);
  gPad->SetLogy(logy);
  gPad->SetLogz(logz);
  h1_->Draw("COLZ");
  h2_->Draw("SAME");
  c_.Print(filename.c_str());
  h1_->Draw("COLZ");
  c_.Print(filename.c_str());
  h2_->Draw("COLZ");
  c_.Print(filename.c_str());
  gPad->SetLogx(false);
  gPad->SetLogy(false);
  gPad->SetLogz(false);
}

template<typename T>
inline void ScaleTheseHistos(std::map<T,TH1D*> h_, const double s_) {
  for (auto& it: h_) {
    it.second->Scale(s_);
  }
}

template<typename T>
inline void ScaleTheseHistos(std::map<T,TH2D*> h_, const double s_) {
  for (auto& it: h_) {
    it.second->Scale(s_);
  }
}

template<typename T>
void PlotAll(const std::map<T, TH1D*>& m, std::string opt,
             TCanvas& c, std::string filename,
             bool logx=false, bool logy=false){
  gPad->SetLogx(logx);
  gPad->SetLogy(logy);
  std::vector<double> min;
  std::vector<double> max;
  double ScaleMax=1.2, ScaleMin=0.;

  if (logy) {
    ScaleMax = 5;
    ScaleMin = 0.1;
    for (auto const& it: m) {
      min.push_back(GetMinNonZeroBinContent(it.second));
      max.push_back(it.second->GetBinContent(it.second->GetMaximumBin()));
    }
  }
  
  m.begin()->second->SetStats(0);
  m.begin()->second->SetMaximum((*std::max_element(max.begin(),max.end()))*ScaleMax);
  m.begin()->second->SetMinimum((*std::min_element(min.begin(),min.end()))*ScaleMin);
  m.begin()->second->SetTitle("");
  m.begin()->second->Draw(opt.c_str());
  for (auto const& it: m)
    if (it != *m.begin())
      it.second->Draw((opt+"SAME").c_str());
  c.Print(filename.c_str());
  gPad->SetLogx(false);
  gPad->SetLogy(false);
}

template<typename T>
void PlotAllIndividually(const std::map<T, TH2D*>& m, std::string opt,
                         TCanvas& c, std::string filename,
                         bool logx=false, bool logy=false){
  gPad->SetLogx(logx);
  gPad->SetLogy(logy);
  for (auto const& it: m) {
    it.second->Draw(opt.c_str());
    c.Print(filename.c_str());
  }
  gPad->SetLogx(false);
  gPad->SetLogy(false);
}

template<typename T>
void PlotAll(const std::map<T, TH2D*>& m, std::string opt,
             TCanvas& c, std::string filename,
             bool logx=false, bool logy=false){
  gPad->SetLogx(logx);
  gPad->SetLogy(logy);
  m.begin()->second->Draw(opt.c_str());
  
  for (auto const& it: m)
    if (it != *m.begin())
      it.second->Draw((opt+"SAME").c_str());
  c.Print(filename.c_str());
  gPad->SetLogx(false);
  gPad->SetLogy(false);
}

template<typename T>
void PlotAll(const std::map<T, TH1D*>& m, std::string opt=""){
  m.begin()->second->Draw(opt.c_str());
  for (auto const& it: m)
    if (it != *m.begin())
      it.second->Draw((opt+"SAME").c_str());
}

template<typename T>
void PlotAll(const std::map<T, TH1D&>& m, std::string opt=""){
  m.begin()->second.Draw(opt.c_str());
  for (auto const& it: m)
    if (it != *m.begin())
      it.second.Draw((opt+"SAME").c_str());
}

template<typename T>
void PlotAll(const std::map<T, TProfile*>& m, std::string opt=""){
  m.begin()->second->Draw(opt.c_str());
  for (auto const& it: m)
    if (it != *m.begin())
      it.second->Draw((opt+"SAME").c_str());
}

template<typename T>
void PlotAll(const std::map<T, TProfile&>& m, std::string opt=""){
  m.begin()->second.Draw(opt.c_str());
  for (auto const& it: m)
    if (it != *m.begin())
      it.second.Draw((opt+"SAME").c_str());
}

template<typename T>
void PlotAll(const std::map<T, TEfficiency*>& m, std::string opt=""){
  m.begin()->second->Draw(opt.c_str());
  for (auto const& it: m)
    if (it != *m.begin())
      it.second->Draw((opt+"SAME").c_str());
}

template<typename T>
void PlotAll(const std::map<T, TEfficiency&>& m, std::string opt=""){
  m.begin()->second.Draw(opt.c_str());
  for (auto const& it: m)
    if (it != *m.begin())
      it.second.Draw((opt+"SAME").c_str());
}

template<typename T>
void PlotAll(const std::map<T, TGraph&>& m, std::string opt="LP"){
  m.begin()->second.Draw((opt+"A").c_str());
  for (auto const& it: m)
    if (it != *m.begin())
      it.second.Draw(opt.c_str());
}

template<typename T>
void PlotAll(const std::map<T, TGraph*>& m, std::string opt="LP"){
  m.begin()->second->Draw((opt+"A").c_str());
  for (auto const& it: m)
    if (it != *m.begin())
      it.second->Draw(opt.c_str());
}


template<typename T>
void SetMaxHisto(std::map<T,TH1D*>& map_, double fact=1.) {
  double max=0;
  for (auto const& it: map_)
    if (max< it.second->GetMaximum())
      max = it.second->GetMaximum();
  for (auto const& it: map_) {
    it.second->SetMaximum(max*fact);
  }
}


template<typename T>
inline void FormatTheseHistos(std::map<T,TH1D*> h_, double scale=1.2) {
  SetMaxHisto(h_, scale);
  std::vector<int>vec_colors = getColors(0);
  int i=0;
  for (auto& it: h_) {
    TH1D* histo = it.second;
    histo->SetLineWidth(2.);
    histo->SetLineColor(vec_colors[i++]);
    histo->SetStats(0);
  }
}


template<typename T>
inline void FormatTheseHistos(std::map<T,TH2D*> h_) {
  for (auto& it: h_) {
    TH2D* histo = it.second;
    histo->SetStats(0);
  }
}

template<typename T>
inline void FormatTheseHistos(std::map<T,TEfficiency*> h_) {
  std::vector<int>vec_colors = getColors(0);
  int i=0;
  for (auto& it: h_) {
    TEfficiency* histo = it.second;
    histo->SetLineWidth(2.);
    histo->SetLineColor(vec_colors[i++]);
  }
}

template<typename T>
inline void FormatTheseHistos(std::map<T,TProfile*> h_) {
  std::vector<int>vec_colors = getColors(0);
  int i=0;
  for (auto& it: h_) {
    TProfile* histo = it.second;
    histo->SetLineWidth(2.);
    histo->SetLineColor(vec_colors[i++]);
    histo->SetStats(0);
  }
}

template<typename T, typename U>
std::pair<T,U> GetMax(const std::map<T,U>& x) {
  using pairtype=std::pair<T,U>; 
  return *std::max_element(x.begin(), x.end(), [] (const pairtype & p1, const pairtype & p2) {
        return p1.second < p2.second;
  }); 
}

//======================================================================
inline TH1* getPadAxisHist(TPad* pad)
{
  TIter next(pad->GetListOfPrimitives());
  TObject *obj;
  while (( obj=next() )) {
    // cout << obj->GetName() << endl;
    if ( obj->IsA()->InheritsFrom(TH1::Class()) ) {
      // cout << "getPadAxisHist returning hist with name " << obj->GetName() << endl;
      return (TH1*)obj;
    }
  }
  return NULL;
}

//======================================================================
inline std::vector<TH1*> getPadHists(TPad* pad)
{  
  std::vector<TH1*> ret;
  TIter next(pad->GetListOfPrimitives());
  TObject *obj;
  while (( obj=next() )) {
    //cout << obj->GetName() << endl;
    if ( obj->IsA()->InheritsFrom(TH1::Class()) ) {
      //cout << "getPadAxisHist returning hist with name " << obj->GetName() << endl;
      ret.push_back((TH1*)obj);
    }
  }
  return ret;
}

//======================================================================
inline double getPadMax(TPad* pad)
{
  TIter next(pad->GetListOfPrimitives());
  TObject *obj;
  Double_t runningMax=-9e99;//Hparam.ymax;
  while (( obj=next() )) {
    if ( obj->IsA()->InheritsFrom(TH1::Class()) ) {
      TH1* curHist=(TH1*)obj;
      const double thisMax=curHist->GetBinContent(curHist->GetMaximumBin());
      if (thisMax > runningMax) {
        runningMax=thisMax;
      }
    }
  }
  return runningMax;
}

//======================================================================
inline void reMax(TPad* pad, double ymin=0, double headroom=1.1)
{
  TH1* firstHist=getPadAxisHist(pad);
  if(!firstHist){
    std::cerr << "reMax: No histogram in pad " << pad->GetName() << ". Can't reMax" << std::endl;
    return;
  }
  firstHist->GetYaxis()->SetRangeUser(ymin, headroom*getPadMax(pad));
  pad->Update();
}
 
//======================================================================
enum EColorPalettes
{
  kAlphabetPalette, kKellyPalette, k36Palette, kGlasbeyPalette, kBrewerSet1Palette, kBrewerDark2Palette
};

//======================================================================
inline const std::vector<int>& getColors(int whichColours=0)
{
  const int alphabetColors[]={
    TColor::GetColor(  25,  25,  25 ), //ebony
    TColor::GetColor( 240, 163, 255 ), //amethyst
    TColor::GetColor( 255,   0,  16 ), //red
    TColor::GetColor( 128, 128, 128 ), //iron
    TColor::GetColor(  25, 164,   5 ), //orpiment
    TColor::GetColor(   0, 153, 143 ), //turquoise
    TColor::GetColor(   0,  51, 128 ), //navy
    TColor::GetColor(  94, 241, 242 ), //sky
    TColor::GetColor( 157, 204,   0 ), //lime
    TColor::GetColor( 153,  63,   0 ), //caramel
    TColor::GetColor( 224, 255, 102 ), //uranium
    TColor::GetColor(  76,   0,  92 ), //damson
    TColor::GetColor( 255, 168, 187 ), //pink
    TColor::GetColor( 194,   0, 136 ), //mallow
    TColor::GetColor( 255, 204, 153 ), //honeydew
    TColor::GetColor(  43, 206,  72 ), //green
    TColor::GetColor(   0, 117, 220 ), //blue
    TColor::GetColor(   0,  92,  49 ), //forest
    TColor::GetColor( 255, 225,   0 ), //yellow
    TColor::GetColor( 153,   0,   0 ), //wine
    TColor::GetColor( 143, 124,   0 ), //khaki
    TColor::GetColor(  16,  10, 255 ), //violet
    TColor::GetColor( 255, 255, 128 ), //xanthin
    TColor::GetColor( 255,  80,   0 ), //zinnia
    TColor::GetColor( 148, 255, 181 ), //jade
    TColor::GetColor(  66, 102,   0 )  //quagmire
  };
  
  const int kellyColors[]={
    // TColor::GetColor("#f2f3f4"), // white,
    TColor::GetColor("#222222"), // black,
    TColor::GetColor("#f3c300"), // yellow,
    TColor::GetColor("#875692"), // purple,
    TColor::GetColor("#f38400"), // orange,
    TColor::GetColor("#a1caf1"), // lightblue,
    TColor::GetColor("#be0032"), // red,
    TColor::GetColor("#c2b280"), // buff,
    TColor::GetColor("#848482"), // gray,
    TColor::GetColor("#008856"), // green,
    TColor::GetColor("#e68fac"), // purplishpink,
    TColor::GetColor("#0067a5"), // blue,
    TColor::GetColor("#f99379"), // yellowishpink,
    TColor::GetColor("#604e97"), // violet,
    TColor::GetColor("#f6a600"), // orangeyellow,
    TColor::GetColor("#b3446c"), // purplishred,
    TColor::GetColor("#dcd300"), // greenishyellow,
    TColor::GetColor("#882d17"), // reddishbrown,
    TColor::GetColor("#8db600"), // yellowgreen,
    TColor::GetColor("#654522"), // yellowishbrown,
    TColor::GetColor("#e25822"), // reddishorange,
    TColor::GetColor("#2b3d26") // olivegreen
  };

  const int palette36Colors[]={
    TColor::GetColor("#5A5156"),
    // TColor::GetColor("#E4E1E3"), // much too light
    TColor::GetColor("#F6222E"),
    TColor::GetColor("#FE00FA"),
    TColor::GetColor("#16FF32"),
    TColor::GetColor("#3283FE"),
    TColor::GetColor("#FEAF16"),
    TColor::GetColor("#B00068"),
    TColor::GetColor("#1CFFCE"),
    TColor::GetColor("#90AD1C"),
    TColor::GetColor("#2ED9FF"),
    TColor::GetColor("#DEA0FD"),
    TColor::GetColor("#AA0DFE"),
    TColor::GetColor("#F8A19F"),
    TColor::GetColor("#325A9B"),
    TColor::GetColor("#C4451C"),
    TColor::GetColor("#1C8356"),
    TColor::GetColor("#85660D"),
    TColor::GetColor("#B10DA1"),
    TColor::GetColor("#FBE426"),
    TColor::GetColor("#1CBE4F"),
    TColor::GetColor("#FA0087"),
    TColor::GetColor("#FC1CBF"),
    TColor::GetColor("#F7E1A0"),
    TColor::GetColor("#C075A6"),
    TColor::GetColor("#782AB6"),
    TColor::GetColor("#AAF400"),
    TColor::GetColor("#BDCDFF"),
    TColor::GetColor("#822E1C"),
    TColor::GetColor("#B5EFB5"),
    TColor::GetColor("#7ED7D1"),
    TColor::GetColor("#1C7F93"),
    TColor::GetColor("#D85FF7"),
    TColor::GetColor("#683B79"),
    TColor::GetColor("#66B0FF"),
    TColor::GetColor("#3B00FB"),
  };

  const int glasbeyColors[]={
    TColor::GetColor("#0000FF"),
    TColor::GetColor("#FF0000"),
    TColor::GetColor("#00FF00"),
    TColor::GetColor("#000033"),
    TColor::GetColor("#FF00B6"),
    TColor::GetColor("#005300"),
    TColor::GetColor("#FFD300"),
    TColor::GetColor("#009FFF"),
    TColor::GetColor("#9A4D42"),
    TColor::GetColor("#00FFBE"),
    TColor::GetColor("#783FC1"),
    TColor::GetColor("#1F9698"),
    TColor::GetColor("#FFACFD"),
    TColor::GetColor("#B1CC71"),
    TColor::GetColor("#F1085C"),
    TColor::GetColor("#FE8F42"),
    TColor::GetColor("#DD00FF"),
    TColor::GetColor("#201A01"),
    TColor::GetColor("#720055"),
    TColor::GetColor("#766C95"),
    TColor::GetColor("#02AD24"),
    TColor::GetColor("#C8FF00"),
    TColor::GetColor("#886C00"),
    TColor::GetColor("#FFB79F"),
    TColor::GetColor("#858567"),
    TColor::GetColor("#A10300"),
    TColor::GetColor("#14F9FF"),
    TColor::GetColor("#00479E"),
    TColor::GetColor("#DC5E93"),
    TColor::GetColor("#93D4FF"),
    TColor::GetColor("#004CFF")
  };

  const int brewerSet1Colors[]={
    TColor::GetColor("#e41a1c"),
    TColor::GetColor("#377eb8"),
    TColor::GetColor("#4daf4a"),
    TColor::GetColor("#984ea3"),
    TColor::GetColor("#ff7f00"),
    TColor::GetColor("#ffff33"),
    TColor::GetColor("#a65628"),
    TColor::GetColor("#f781bf")
  };

  const int brewerDark2Colors[]={
    TColor::GetColor( 27, 158, 119),
    TColor::GetColor(217,  95,   2),
    TColor::GetColor(117, 112, 179),
    TColor::GetColor(231,  41, 138),
    TColor::GetColor(102, 166,  30),
    TColor::GetColor(230, 171,   2),
    TColor::GetColor(166, 118,  29),
    TColor::GetColor(102, 102, 102)
  };

  const int nPalettes=6;
  static std::vector<std::vector<int> > allPalettes(nPalettes);
  static bool firstCall=true;

  if(firstCall){
    for(int j=0; j<nPalettes; ++j){
      int nColors=0;
      const int* firstColor=NULL;

      switch(j){
      case 0:
        nColors=26;
        firstColor=alphabetColors;
        break;
      case 1:
        nColors=21;
        firstColor=kellyColors;
        break;
      case 2:
        nColors=35; // 36 because I commented one out
        firstColor=palette36Colors;
        break;
      case 3:
        nColors=30;
        firstColor=glasbeyColors;
        break;
      case 4:
        nColors=8;
        firstColor=brewerSet1Colors;
        break;
      case 5:
        nColors=8;
        firstColor=brewerDark2Colors;
        break;
      default:
        assert(0 && "whichColours must be 0-5");
      }

      for(int i=0; i<nColors; ++i){
        allPalettes[j].push_back(firstColor[i]);
      }
    }
    firstCall=false;
  }
  return allPalettes[whichColours];
}

//======================================================================
inline void autoColorHists(TPad* pad, int whichColours=kBrewerSet1Palette)
{
  const std::vector<int>& colours=getColors(whichColours);

  std::vector<TH1*> hists=getPadHists(pad);
  for(unsigned int i=0; i<hists.size(); ++i){
    hists[i]->SetLineColor(colours[i%colours.size()]);
  }
  pad->Draw();
}

//### Dynamic
// vars
inline std::map<std::string, int> dyn_GenType;
inline std::vector<std::string> dyn_AllGenType;

// functions
inline void setIDMap( std::map<std::string, int> ID_map )
{
  dyn_GenType = ID_map;
}

inline void setAllGenType()
{
  for (auto const& x : dyn_GenType)
  {
    dyn_AllGenType.push_back(x.first);
  }
}

inline std::string ConvertIDIntToString(int i)
{
  std::string key;
  for (auto &entry : dyn_GenType) {
    if (entry.second == i) {
       key = entry.first;
       break; // to stop searching
    }
  }
  return key;
}

inline int ConvertIDStringToInt(std::string id_name)
{
  return dyn_GenType[id_name];
}

inline void testDynamic()
{
 std::cout << "[Helper] Test of dynamic variables" << std::endl;
 std::cout << dyn_GenType["All"] << std::endl;
 std::cout << dyn_AllGenType[2] << std::endl;
 std::cout << ConvertIDIntToString(2) << std::endl;
 std::cout << ConvertIDStringToInt("AllBackground") << std::endl;
}

inline void SetDynamicVars( std::map<std::string, int> ID_map_from_IO )
{
  std::cout << "[Helper] Setting dynamic variables..." << std::endl;
  setIDMap( ID_map_from_IO );
  setAllGenType();
  testDynamic();
} 
//### Dynamic

//enum GenType{
//  kOther=0,
//  kSNnu=1,
//  kAPA=2,
//  kCPA=3,
//  kAr39=4,
//  kNeutron=5,
//  kKrypton=6,
//  kPolonium=7,
//  kRadon=8,
//  kAr42=9,
//  kAllBackground,
//  kAll
//};

enum Direction {
  kX=0,
  kY,
  kZ,
  kT
};

static const std::vector<Direction> AllDirection = {kX,kY,kZ,kT};

//static const std::vector<GenType>   AllGenType   = {kOther,
//                                                    kSNnu,
//                                                    kAPA,
//                                                    kCPA,
//                                                    kAr39,
//                                                    kNeutron,
//                                                    kKrypton,
//                                                    kPolonium,
//                                                    kRadon,
//                                                    kAr42,
//                                                    kAllBackground,
//                                                    kAll};

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
}

//inline GenType ConvertIntToGenType(int i) {
//  switch (i) {
//  case  0: return kOther;
//  case  1: return kSNnu;
//  case  2: return kAPA;
//  case  3: return kCPA;
//  case  4: return kAr39;
//  case  5: return kNeutron;
//  case  6: return kKrypton;
//  case  7: return kPolonium;
//  case  8: return kRadon;
//  case  9: return kAr42;         
//  case 10: return kAllBackground;
//  case 11: return kAll;
//  }
//  return kOther;
//}

//replace GenName and ShortGenName with dyn_AllGenType
class Helper{
public:
  Helper(){
  std::cout << "[HELPER] Constructor" << std::endl;
//    GenName[kSNnu] =          "SN#nu"         ;
//    GenName[kAPA] =           "APA"           ;
//    GenName[kCPA] =           "CPA"           ;
//    GenName[kAr39] =          "Ar39"          ;
//    GenName[kNeutron] =       "Neutron"       ;
//    GenName[kKrypton] =       "Krypton"       ;
//    GenName[kPolonium] =      "Polonium"      ;
//    GenName[kRadon] =         "Radon"         ;
//    GenName[kAr42] =          "Ar42"          ;
//    GenName[kOther] =         "Other (not BT)";
//    GenName[kAllBackground] = "AllBackground" ;
//    GenName[kAll] =           "All"           ;
//    
//    ShortGenName[kSNnu] =          "SNnu"          ;
//    ShortGenName[kAPA] =           "APA"           ;
//    ShortGenName[kCPA] =           "CPA"           ;
//    ShortGenName[kAr39] =          "Ar39"          ;
//    ShortGenName[kNeutron] =       "Neutron"       ;
//    ShortGenName[kKrypton] =       "Krypton"       ;
//    ShortGenName[kPolonium] =      "Polonium"      ;
//    ShortGenName[kRadon] =         "Radon"         ;
//    ShortGenName[kAr42] =          "Ar42"          ;
//    ShortGenName[kOther] =         "Other/Noise"   ;
//    ShortGenName[kAllBackground] = "AllBackground" ;
//    ShortGenName[kAll] =           "All"           ;

    for(auto const& x : dyn_GenType){
      std::pair temp_pair { x.second, x.first };
      dyn_GenName.insert( temp_pair );
    }

    for(auto const& x : dyn_GenType){
      std::pair temp_pair { x.second, x.first };
      dyn_ShortGenName.insert( temp_pair );
    }

  };
//  std::map<GenType,std::string> GenName;
//  std::map<GenType,std::string> ShortGenName;
   
  std::map<int, std::string> dyn_GenName;
  std::map<int, std::string> dyn_ShortGenName;
};


inline std::map<std::string,TH1D*> GetHistos(std::string name, std::string title,
                                         int nbins, double xbinmin, double xbinmax){
  std::map<std::string,TH1D*> map_h;
  std::vector<int>vec_colors = getColors(0);
  int i=0;
  Helper h;
  std::cout << "Getting histos" << std::endl;
  for(auto& it : h.dyn_ShortGenName){
    map_h[it.second] = new TH1D(Form("%s_%s",it.second.c_str(), name.c_str()),
                               Form("%s %s",it.second.c_str(), title.c_str()),
                               nbins, xbinmin, xbinmax);
    map_h[it.second]->SetLineWidth(2);
    map_h[it.second]->SetStats(0);
    map_h[it.second]->SetLineColor(vec_colors[i++]);
  }
  return map_h;
}

inline std::map<std::string,TH2D*> Get2DHistos(std::string name, std::string title,
                                           int nbinsx, double xbinmin, double xbinmax,
                                           int nbinsy, double ybinmin, double ybinmax){
  std::map<std::string,TH2D*> map_h;
  Helper h;
  std::cout << "Getting 2D histos" << std::endl;
  for(auto& it : h.dyn_ShortGenName){
    map_h[it.second] = new TH2D(Form("%s_%s",it.second.c_str(), name.c_str()),
                               Form("%s %s",it.second.c_str(), title.c_str()),
                               nbinsx, xbinmin, xbinmax,
                               nbinsy, ybinmin, ybinmax);
    map_h[it.second]->SetStats(0);
  }
  return map_h;
}


inline TH1D* GetSimpleHisto(std::string name, std::string title){
  Helper help;
  std::cout << "Getting simple histos" << std::endl;
  TH1D* h = new TH1D(name.c_str(), title.c_str(), help.dyn_GenName.size(), -0.5, (double)help.dyn_GenName.size()-0.5);

  for(auto const& it : help.dyn_ShortGenName)
    h->GetXaxis()->SetBinLabel(it.first+1, it.second.c_str());
  return h;
}

inline TLegend* GetNewLegend(double xmin=0.1, double ymin=0.1, double xmax=0.9, double ymax=0.9,
                      const std::map<std::string,TH1D*>& histos=std::map<std::string,TH1D*>()){

  // if(histos.size()==0)
  //   histos = GetHistos("","",1,0,1);
  Helper h;
  std::cout << "Getting new legend" << std::endl;
  TLegend* leg = new TLegend(xmin, ymin, xmax, ymax);
  for(auto const& it : histos)
    //std::string leg_name = h.dyn_GenName[ConvertIDStringToInt(it.first)];
    leg->AddEntry(it.second, it.first.c_str(), "L");
  return leg;
}

inline void AddOverflow(TH1D* histo){
  histo->SetBinContent(histo->GetXaxis()->GetNbins(),
                       histo->GetBinContent(histo->GetXaxis()->GetNbins()  )+
                       histo->GetBinContent(histo->GetXaxis()->GetNbins()+1));
  histo->SetBinContent(histo->GetXaxis()->GetNbins()+1,0);
}

inline void AddOverflow(TH1D& histo){
  histo.SetBinContent(histo.GetXaxis()->GetNbins(),
                      histo.GetBinContent(histo.GetXaxis()->GetNbins()  )+
                      histo.GetBinContent(histo.GetXaxis()->GetNbins()+1));
  histo.SetBinContent(histo.GetXaxis()->GetNbins()+1,0);
}

inline void AddUnderflow(TH1D* histo){
  histo->SetBinContent(1,
                       histo->GetBinContent(0)+
                       histo->GetBinContent(1));
  histo->SetBinContent(0,0);
}

inline void AddUnderflow(TH1D& histo){
  histo.SetBinContent(1,
                      histo.GetBinContent(0)+
                      histo.GetBinContent(1));
  histo.SetBinContent(0,0);
}

inline TH1D* DoSignalOverNoise(const TH1D* signal,
                               const TH1D* noise) {

  TH1D* s_over_n = (TH1D*)signal->Clone();
  s_over_n->Divide(noise);
  return s_over_n;
}

inline TH1D* DoIntegratedSignalOverNoise(const TH1D* signal,
                                         const TH1D* noise) {
  TH1D* s_over_n = (TH1D*)signal->Clone();
  s_over_n->Reset();
  int nbins = signal->GetXaxis()->GetNbins();
  for (int i=0; i<nbins; ++i) {
    double s = signal->Integral(i, nbins);
    double n = noise ->Integral(i, nbins);
    if (n!=0) {
      s_over_n->SetBinContent(i, s/n);
      s_over_n->SetBinError  (i, (s/n)*sqrt(1/s+1/n));
    } else {
      s_over_n->SetBinContent(i, 0);
    }
  }
  return s_over_n;
  
}

inline TH1D* DoIntegratedSignalOverNoiseUnitNorm(const TH1D* signal,
                                                 const TH1D* noise) {
  TH1D* signal2  = (TH1D*)signal->Clone();
  TH1D* noise2   = (TH1D*)noise->Clone();
  int nbins = signal->GetXaxis()->GetNbins();

  signal2->Scale(1./signal2->Integral());
  noise2 ->Scale(1./noise2 ->Integral());
  TH1D* s_over_n = DoIntegratedSignalOverNoise(signal2, noise2);
  for (int i=0; i<nbins; ++i) {
    double s = signal->Integral(i, nbins);
    double n = noise ->Integral(i, nbins);
    if (n!=0) {
      s_over_n->SetBinError(i, s_over_n->GetBinContent(i)*sqrt(1/s+1/n));
    } else {
      s_over_n->SetBinError(i, 0);
    }
  }
  return s_over_n;
}



inline double GetStatThreshold(const TH1D* signal,
                               const TH1D* noise,
                               const double threshold=20) {

  int nbins = signal->GetXaxis()->GetNbins();
  double max_sig = 0;
  bool haveseen = false;

  for (int i=0; i<nbins; ++i) {
    double s = signal->Integral(i, nbins);
    if (s <= threshold) {
      max_sig = signal->GetBinCenter(i);
      haveseen = true;
      break;
    }
  }

  if (!haveseen)
    max_sig = std::numeric_limits<double>::max();


  double max_back = 0;
  haveseen=false;
  for (int i=0; i<nbins; ++i) {
    double s = noise->Integral(i, nbins);
    if (s <= threshold) {
      max_back = noise->GetBinCenter(i);
      haveseen = true;
      break;
    }
  }

  if (!haveseen)
    max_back = std::numeric_limits<double>::max();

  return std::min(max_sig, max_back);

}



#endif
