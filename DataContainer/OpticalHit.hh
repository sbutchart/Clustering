#ifndef OPTICALHIT_HH
#define OPTICALHIT_HH
#include "DataContainer/Hit.hh"
#include "Utils/SimpleGeom.hh"

#include <iostream>

class OpticalHit: public Hit {
public:
  OpticalHit(int GenType,
             double X, double Y, double Z,
             double PeakTime, double Width, double PE, int chan);
  OpticalHit(): Hit() { };

  ~OpticalHit(){};
  using Hit::Print;
  double GetSPE() const { return fPeak; };
  
};

#endif
