#ifndef SIMPLEGEOM_HH
#define SIMPLEGEOM_HH

#include "Helper.h"




class SimpleGeom {
public:
  SimpleGeom() { };

  size_t GetAPAFromPosition(const std::map<Direction,double> po) {
    return GetAPAFromPosition(po.at(kX), po.at(kY), po.at(kZ));
  }
  
  size_t GetAPAFromPosition(const double x, const double y, const double z) {
    (void) x;
    size_t row;
    size_t column;
    if (y<0) { row=0; }
    else     { row=1; }

    if      (0    <= z && z <= 231.5) { column = 0; }
    else if (231.5<  z && z <= 464 ) { column = 1; }
    else if (464  <  z && z <= 696 ) { column = 2; }
    else if (696  <  z && z <= 928 ) { column = 3; }
    else if (928  <  z && z <= 1160) { column = 4; }
    else if (1160 <  z && z <= 1392) { column = 5; }
    else {
      std::cout << "Unknown z position -> " << z <<  std::endl;
      column=0;
    }
    return row+column*2;
  };

  
  size_t GetAPAFromWireChannelNumber(const size_t c) {
    if      (1600  <= c && c <= 2560 ) { return  0; }
    else if (4160  <= c && c <= 5120 ) { return  1; }
    else if (6720  <= c && c <= 7680 ) { return  2; }
    else if (9280  <= c && c <= 10240) { return  3; }
    else if (11840 <= c && c <= 12800) { return  4; }
    else if (14400 <= c && c <= 15360) { return  5; }
    else if (16960 <= c && c <= 17920) { return  6; }
    else if (19520 <= c && c <= 20480) { return  7; }
    else if (22080 <= c && c <= 23040) { return  8; }
    else if (24640 <= c && c <= 25600) { return  9; }
    else if (27200 <= c && c <= 28160) { return 10; }
    else if (29760 <= c && c <= 30720) { return 11; }
    else {
      std::cout << "Unknown channel " << c << std::endl;
    }
    return 0;
  };
  
private:
 
};

#endif
