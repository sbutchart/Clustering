#ifndef PHOTONTIMINGDISPLAY_HH
#define PHOTONTIMINGDISPLAY_HH

#include "Display.hh"
#include "TArrow.h"
#include "TLatex.h"





class PhotonTimingDisplay : public Display {

public:
  PhotonTimingDisplay(std::string F, std::string T);
  void DisplayEvent(int event,    int type, double time);
  void DisplayEvent(int event=-1, int type=-1) { DisplayEvent(event,type,0); };
  ~PhotonTimingDisplay();
  void SetPEMode(bool PE=true) {
    fPE_Mode = PE;
    if(PE) for(auto& it: f_map_gen_th1_timing) it.second->GetXaxis()->SetTitle("PE");
    else   for(auto& it: f_map_gen_th1_timing) it.second->GetXaxis()->SetTitle("n Hits");
  };
  bool GetPEMode() const { return fPE_Mode; };
  void SetTimeExtent(double maxX=2) {
    for(auto& it: f_map_gen_th1_timing)
      delete it.second;
    f_map_gen_th1_timing = GetHistos("timing", "timing;Time [#mus];n Hits", 100, 0, maxX);
  }
private:
  bool fPE_Mode;
  std::map<GenType,TH1D*> f_map_gen_th1_timing;
  std::map<GenType,std::vector<TArrow*> > f_map_gen_arrow_truthtiming;
  void AddArrow(double, GenType);
  void PlotAll();
  void ResetAll();
  TLatex* position;
  TLatex* energy;
  
};


#endif
