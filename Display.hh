#ifndef DISPLAY_HH
#define DISPLAY_HH


#include "TCanvas.h"
#include "TH1D.h"
#include "TH2D.h"

#include "Helper.h"
#include "InputManager.h"


class Display {
public:
  Display(){c = new TCanvas(); };
  virtual void DisplayEvent(int, int)=0;
  ~Display(){
    delete c;
    c = NULL;
  };
protected:
  TCanvas* c;
  InputManager im;
};


#endif
