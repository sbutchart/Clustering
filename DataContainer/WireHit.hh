#ifndef WIREHIT_HH
#define WIREHIT_HH
#include "Helper.h"
#include <iomanip>
#include "Hit.hh"

class WireHit: public Hit {
public:
  WireHit(int   cHitView,      int   cGenType, int   cHitChan,
          float cHitTime,      float cHitSADC, float cHitRMS,
          float cTrDepositedE, float cTrEPart, int   cTrIDPart,
          float cRecPosX,      float cRecPosY, float cRecPosZ,
          float cTrPosX,       float cTrPosY,  float cTrPosZ,
          int   cMarleyIndex,  float cNelec):
    Hit() {
    fHitView = cHitView;
    fTrueGenType = ConvertIntToGenType(cGenType);
    fChannel = cHitChan;
    fPeak = cHitSADC;
    fSize[kT] = cHitRMS;
    fPosition[kT] = cHitTime;
    if (cGenType == 1)fTrueMarleyIndex = cMarleyIndex;
    fTrDepositedE = cTrDepositedE;
    fNElectron = cNelec;
    fTrEPart = cTrEPart;
    fTrIDPart = cTrIDPart;
    fTruePosition[kX] = cTrPosX;
    fTruePosition[kY] = cTrPosY;
    fTruePosition[kZ] = cTrPosZ;
    fTruePosition[kT] = 0;
    fPosition[kX] = cRecPosX;
    fPosition[kY] = cRecPosY;
    fPosition[kZ] = cRecPosZ;
    fPosition[kT] = cHitTime;
    fAPA = fGeom.GetAPAFromPosition(fPosition);
  };
  
  WireHit(): Hit() { };
private:
  WireHit(const WireHit&hit):
    Hit(hit),
    fHitView     (hit.fHitView     ),
    fTrDepositedE(hit.fTrDepositedE),
    fNElectron   (hit.fNElectron   ),
    fTrEPart     (hit.fTrEPart     ),
    fTrIDPart    (hit.fTrIDPart    ) { };
public:
  ~WireHit() {};
    
  
  size_t GetHitView     () const { return fHitView;         };
  int    GetHitChan     () const { return (int)fChannel;    };
  double GetHitTime     () const { return fPosition.at(kT); };
  double GetHitSADC     () const { return fPeak;            };
  double GetHitRMS      () const { return fSize.at(kT);     };
  double GetTrDepositedE() const { return fTrDepositedE;    };
  double GetNElectron   () const { return fNElectron;       };
  double GetTrEPart     () const { return fTrEPart;         };
  double GetTrIDPart    () const { return fTrIDPart;        };
  
  using Hit::Print;

  
private:
  size_t fHitView;
  double fTrDepositedE;
  double fNElectron;
  double fTrEPart;
  size_t fTrIDPart;
 
};

#endif
