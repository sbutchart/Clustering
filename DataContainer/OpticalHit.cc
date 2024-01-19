#include "OpticalHit.hh"
#include "Utils/Helper.h"

OpticalHit::OpticalHit(int GenType,
                       double X, double Y, double Z,
                       double PeakTime, double Width, double PE, int chan):
  Hit(){
  fPeak = PE;
  fChannel = chan;
  fTrueGenType = ConvertIDIntToString(GenType);
  fPosition[kX] = X;
  fPosition[kY] = Y;
  fPosition[kZ] = Z;
  fPosition[kT] = PeakTime;
  fSize[kT] = Width;
  fAPA = fGeom.GetAPAFromPosition(fPosition);
 
}
