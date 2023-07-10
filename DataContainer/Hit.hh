#ifndef HIT_HH
#define HIT_HH

#include "Utils/SimpleGeom.hh"

#include "Utils/Helper.h"
#include <set>

typedef std::pair<double,double> extent;

class Hit {
protected:
  Hit():
    fPosition(),
    fSize    (),
    fExtent  (),
    fPeak    (0),
    fChannel (0),
    fAPA     (0),
    fTrueMarleyIndex(-1),
    fGeom    (){
    fTruePosition[kX] = 0.;
    fTruePosition[kY] = 0.;
    fTruePosition[kZ] = 0.;
    fTruePosition[kT] = 0.;
    fPosition[kX] = 0.;
    fPosition[kY] = 0.;
    fPosition[kZ] = 0.;
    fPosition[kT] = 0.;
    fSize[kX] = 0.;
    fSize[kY] = 0.;
    fSize[kZ] = 0.;
    fSize[kT] = 0.;
    fExtent[kX] = extent(0.,0.);
    fExtent[kY] = extent(0.,0.);
    fExtent[kZ] = extent(0.,0.);
    fExtent[kT] = extent(0.,0.);
  };

  Hit(const Hit& h):
    fPosition(h.fPosition),
    fSize    (h.fSize    ),
    fExtent  (h.fExtent  ),
    fPeak    (h.fPeak    ),
    fChannel (h.fChannel ),
    fAPA     (h.fAPA     ){
  };
  
  virtual ~Hit() = 0;

public:

  friend bool HitOrderedInPosX(const Hit*, const Hit*);
  friend bool HitOrderedInPosY(const Hit*, const Hit*);
  friend bool HitOrderedInPosZ(const Hit*, const Hit*);
  friend bool HitOrderedInChan(const Hit*, const Hit*);
  friend bool HitOrderedInTime(const Hit*, const Hit*);

public:
  double  GetPosition    (const Direction d) const { return fPosition    .at(d); };
  double  GetTruePosition(const Direction d) const { return fTruePosition.at(d); };
  double  GetSize        (const Direction d) const { return fSize        .at(d); };
  extent  GetExtent      (const Direction d) const { return fExtent      .at(d); };
  double  GetPeak        () const { return fPeak;            };
  double  GetTime        () const { return fPosition.at(kT); };
  double  GetWidth       () const { return fSize.at(kT);     };
  double  GetRecoPosition(const int       d) const {
    switch(d) {
    case 0: return fPosition.at(kX);
    case 1: return fPosition.at(kY);
    case 2: return fPosition.at(kZ);
    case 3: return fPosition.at(kT);
    default:
      std::cout << "Unknown position!!" << std::endl;
      exit(1);
    }
  };
  
  size_t  GetChannel     () const { return fChannel;         };
  size_t  GetAPA         () const { return fAPA;             };
  GenType GetGenType     () const { return fTrueGenType;     };
  double  GetTrueEnergy  () const { return fTrueEnergy;      };
  int  GetMarleyIndex () const { return fTrueMarleyIndex; };
  
  void SetPosition    (const Direction d, const double p) { fPosition    [d] = p; };
  void SetTruePosition(const Direction d, const double p) { fTruePosition[d] = p; };
  void SetSize        (const Direction d, const double s) { fSize        [d] = s; };
  void SetExtent      (const Direction d, const extent e) { fExtent      [d] = e; };
  void SetPeak        (const double  i) { fPeak            = i; };
  void SetChannel     (const size_t  i) { fChannel         = i; };
  void SetAPA         (const size_t  i) { fAPA             = i; };
  void SetGenType     (const GenType i) { fTrueGenType     = i; };
  void SetTrueEnergy  (const double  i) { fTrueEnergy      = i; };
  void SetMarleyIndex (const int     i) { fTrueMarleyIndex = i; };
  
  virtual void Print() const {
    std::cout << "- - - - - - - - - - - - - - - - - - - - - " << std::endl;
    std::cout << "     TrueGenType: "     << fTrueGenType     << std::endl;
    std::cout << "     TrueEnergy: "      << fTrueEnergy      << std::endl;
    std::cout << "     TrueMarleyIndex: " << fTrueMarleyIndex << std::endl;
    std::cout << "     Peak: "            << fPeak            << std::endl;
    std::cout << "     Channel: "         << fChannel         << std::endl;
    std::cout << "     APA: "             << fAPA             << std::endl;
    std::cout << "     Position[X] " << fPosition.at(kX)
              << " Position[Y] " << fPosition.at(kY)
              << " Position[Z] " << fPosition.at(kZ)
              << " Position[T] " << fPosition.at(kT) << std::endl;;
    std::cout << "     Extent[X] " << fExtent.at(kX).first << "-" << fExtent.at(kX).second
              << " Extent[Y] " << fExtent.at(kY).first << "-" << fExtent.at(kY).second
              << " Extent[Z] " << fExtent.at(kZ).first << "-" << fExtent.at(kZ).second
              << " Extent[T] " << fExtent.at(kT).first << "-" << fExtent.at(kT).second << std::endl;;
    std::cout << "     Size[X] " << fSize.at(kX)
              << " Size[Y] " << fSize.at(kY)
              << " Size[Z] " << fSize.at(kZ)
              << " Size[T] " << fSize.at(kT) << std::endl;;
  };

protected:
  std::map<Direction,double> fPosition;
  std::map<Direction,double> fTruePosition;
  std::map<Direction,double> fSize;
  std::map<Direction,extent> fExtent;
  double  fPeak;
  size_t  fChannel;
  size_t  fAPA;
  GenType fTrueGenType;
  double  fTrueEnergy;
  int     fTrueMarleyIndex;
  SimpleGeom fGeom;
};

inline Hit::~Hit() {
  fPosition.clear();
  fSize    .clear();
  fExtent  .clear();
}

#endif
