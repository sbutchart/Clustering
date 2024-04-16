#ifndef CLUSTER_HH
#define CLUSTER_HH

#include "DataContainer/Hit.hh"

#include "Utils/Helper.h"

class Cluster {

private:

  void InitVecMap(){
    for (auto const& d : AllDirection) {
      fPosition     [d] = 0.;
      fSize         [d] = 0.;
      fExtent       [d] = extent(0.,0.);
      fTruePosition [d] = 0.;
      fTrueDirection[d] = 0.;
    }
   
      for(auto const& x : dyn_AllGenType){
        fTruePurity[x] = 0.;
      }

  };
  
  Cluster(const Cluster& c):
    fHit             (c.fHit            ),
    fNHit            (c.fNHit           ),
    fNChannel        (c.fNChannel       ),
    fChannelExtent   (c.fChannelExtent  ),
    fChannelWidth    (c.fChannelWidth   ),
    fPosition        (c.fPosition       ),
    fSize            (c.fSize           ),
    fExtent          (c.fExtent         ),
    fIsSelected      (c.fIsSelected     ),
    fTruePurity      (c.fTruePurity     ),
    fSumPeak         (c.fSumPeak        ),
    fRecoEnergy      (c.fRecoEnergy     ),
    fAPA             (c.fAPA            ),
    fTrueGenType     (c.fTrueGenType    ),
    fTrueEnergy      (c.fTrueEnergy     ),
    fTrueMarleyIndex (c.fTrueMarleyIndex),
    fTruePosition    (c.fTruePosition   ),
    fTrueDirection   (c.fTrueDirection  ),
    fIsTriggering    (c.fIsTriggering   )
    {}


protected:
  
  Cluster():
    fHit             (),
    fNHit            (0),
    fNChannel        (0),
    fChannelExtent   (),
    fChannelWidth    (0),
    fPosition        (),
    fSize            (),
    fExtent          (),
    fIsSelected      (0),
    fTruePurity      (),
    fSumPeak         (0),
    fRecoEnergy      (0),
    fAPA             (0),
    fTrueGenType     ("Other"),
    fTrueEnergy      (0),
    fTrueMarleyIndex (-1),
    fTruePosition    (),
    fTrueDirection   (),
    fIsTriggering    (0){
    InitVecMap();
  };


  Cluster(std::vector<Hit*>& v_hit):
    fHit             (v_hit),
    fNHit            (v_hit.size()),
    fNChannel        (0),
    fChannelExtent   (),
    fChannelWidth    (0),
    fPosition        (),
    fSize            (),
    fExtent          (),
    fIsSelected      (0),
    fTruePurity      (),
    fSumPeak         (0),
    fRecoEnergy      (0),
    fAPA             (0),
    fTrueGenType     (),
    fTrueEnergy      (0),
    fTrueMarleyIndex (-1),
    fTruePosition    (),
    fTrueDirection   (),
    fIsTriggering    (0){
  };
  
  void Initialise() {
    InitVecMap();
    
    std::map<Direction,std::set<double>> all_pos;
    std::map<size_t,size_t> apa;
    std::map<int,size_t> marleyIndices;
    std::set<int> channel;
    std::map<std::string,double> hittype_peak;

    for (auto const& it: fHit) {
      marleyIndices[it->GetMarleyIndex()]++;
      channel.insert(it->GetChannel());
      fSumPeak += it->GetPeak();

      for (auto const& d : AllDirection) {
        fPosition[d] += it->GetPosition(d) * it->GetPeak();
        fTruePosition[d] += it->GetTruePosition(d) * it->GetPeak();
        all_pos[d].insert(it->GetPosition(d));
      }

      apa[it->GetAPA()] += it->GetPeak();
      hittype_peak[it->GetGenType()] += it->GetPeak();
      //std::cout << "Gen Type of hit: " << it->GetGenType().c_str() << std::endl;
      //std::cout << "Peak: " << hittype_peak[it->GetGenType()].second << std::endl;
    }
    size_t max = 0;
    for (auto const& it:marleyIndices){
      if(it.second>max){
        fTrueMarleyIndex = it.first;
      }
    }
    fIsSelected = true;
    fAPA = GetMax(apa).first;
    fNChannel = channel.size();
    fChannelExtent.first  = *channel.begin();
    fChannelExtent.second = *channel.rbegin();
    fChannelWidth = (size_t)((int)fChannelExtent.second - (int)fChannelExtent.first);

    SetTypeFromSumHit(hittype_peak);
    
    //std::cout << "SetTypeFromSumHit: " << SetTypeFromSumHit(hittype_peak).c_str() << std::endl;
 
    for (auto const& d : dyn_AllGenType) {
      //std::cout << "GenType: " << d << std::endl;
      //std::cout << "hittype_peak: " << hittype_peak[d] << std::endl;
      //std::cout << "fSumPeak: " << fSumPeak << std::endl;
      fTruePurity[d] = hittype_peak[d] / fSumPeak;
    }

    for (auto const& d : AllDirection) {
      fPosition[d]   /= fSumPeak;
      fTruePosition[d] /= fSumPeak;
      fExtent[d].first  = *(all_pos[d].begin());
      fExtent[d].second = *(all_pos[d].rbegin());
      fSize[d] = fExtent[d].second - fExtent[d].first;
    }
  };

  
  virtual void Print(const bool printHit=false) {
    std::cout << "---------------------------" << std::endl;
    std::cout << "Cluster " << std::endl;
    std::cout << " Marley index "    << fTrueMarleyIndex << std::endl;
    std::cout << " ENu "             << fTrueEnergy      << std::endl;
    std::cout << " TruePos[X]: "     << fTruePosition[kX]
              << " TruePos[Y]: "     << fTruePosition[kY]
              << " TruePos[Z]: "     << fTruePosition[kZ]
              << " TruePos[T]: "     << fTruePosition[kT] << std::endl;
    std::cout << " TrueDir[X]: "     << fTrueDirection[kX]
              << " TrueDir[Y]: "     << fTrueDirection[kY]
              << " TrueDir[Z]: "     << fTrueDirection[kZ]
              << " TrueDir[T]: "     << fTrueDirection[kT] << std::endl;
    std::cout << " Position[X]: "    << fPosition[kX]
              << " Position[Y]: "    << fPosition[kY]
              << " Position[Z]: "    << fPosition[kZ]
              << " Position[T]: "    << fPosition[kT] << std::endl;
    std::cout << " Size[X]: "        << fSize[kX]
              << " Size[Y]: "        << fSize[kY]
              << " Size[Z]: "        << fSize[kZ]
              << " Size[T]: "        << fSize[kT]    << std::endl;
    std::cout << " Extent[X]: "      << fExtent[kX].first << "-" << fExtent[kX].second 
              << " Extent[Y]: "      << fExtent[kY].first << "-" << fExtent[kY].second 
              << " Extent[Z]: "      << fExtent[kZ].first << "-" << fExtent[kZ].second 
              << " Extent[T]: "      << fExtent[kT].first << "-" << fExtent[kT].second << std::endl;
    std::cout << " IsSelected: "     << fIsSelected  << std::endl;
    std::cout << " IsTriggering: "   << fIsTriggering  << std::endl;
    std::cout << " SumPeak: "        << fSumPeak     << std::endl;
    std::cout << " APA: "            << fAPA         << std::endl;
    std::cout << " TrueGenType: "    << fTrueGenType << std::endl;
    if (printHit)
      for (auto const& it:fHit)
        it->Print();
  }
  
public:
  double GetPosition     (const Direction d) const { return fPosition.at(d);               };
  double GetRecoPosition (const Direction d) const { return fPosition.at(d);               };
  size_t GetNChannel     ()                  const { return fNChannel;                     };
  double GetSumPeak      ()                  const { return fSumPeak;                      };
  size_t GetAPA          ()                  const { return fAPA;                          };
  double GetSize         (const Direction d) const { return fSize.at(d);                   };
  extent GetExtent       (const Direction d) const { return fExtent.at(d);                 };
  double GetPurity       (std::string     d) const { return fTruePurity.at(d);             };
  double GetTruePosition (const Direction d) const { return fTruePosition.at(d);           };
  double GetTrueDirection(const Direction d) const { return fTrueDirection.at(d);          };
  size_t GetNHit         ()                  const { return fNHit;                         };

//this is being used in the OutputManager:
  bool   GetType         ()                  const { return (fTrueGenType=="marley");      };

  double GetFirstHitTime ()                  const { return fExtent.at(kT).first;          };
  double GetLastHitTime  ()                  const { return fExtent.at(kT).second;         };
  double GetTimeWidth    ()                  const { return fSize.at(kT);                  };
  bool   GetIsSelected   ()                  const { return fIsSelected;                   };
  bool   GetIsTriggering ()                  const { return fIsTriggering;                 };
  double GetRecoEnergy   ()                  const { return fRecoEnergy;                   };
  int    GetMarleyIndex  ()                  const { return fTrueMarleyIndex;              };
  double GetStartChannel ()                  const { return (double)fChannelExtent.first ; };
  double GetEndChannel   ()                  const { return (double)fChannelExtent.second; };
  double GetChannelWidth ()                  const { return (double)fChannelWidth        ; };
  std::vector<Hit*>& GetHit()                      { return fHit                         ; };
  
  void SetPosition     (const Direction d, const double p) { fPosition     [d] = p; };
  void SetNChannel     (const size_t d)                    { fNChannel         = d; };
  void SetAPA          (const size_t d)                    { fAPA              = d; };
  void SetSize         (const Direction d, const double s) { fSize         [d] = s; };
  void SetGenType      (std::string   d)                   { fTrueGenType      = d; };
  void SetNHit         (const size_t  d)                   { fNHit             = d; };
  void SetSumPeak      (const double  s)                   { fSumPeak          = s; };
  void SetExtent       (const Direction d, const extent e) { fExtent       [d] = e; };
  void SetTruePosition (const Direction d, const double e) { fTruePosition [d] = e; };
  void SetTrueDirection(const Direction d, const double e) { fTrueDirection[d] = e; };
  void SetIsSelected   (const bool b=true)                 { fIsSelected       = b; };
  void SetIsTriggering (const bool b=true)                 { fIsTriggering     = b; };
  void SetRecoEnergy   (const double d)                    { fRecoEnergy       = d; };
  void SetMarleyIndex  (const int    d)                    { fTrueMarleyIndex  = d; };
  void SetHit          (const std::vector<Hit*>& h)        { fHit              = h; };

protected:
  virtual void SetTypeFromSumHit(const std::map<std::string,double>&) = 0;
  std::vector<Hit*> fHit;
  size_t fNHit;
  size_t fNChannel;
  std::pair<size_t,size_t> fChannelExtent;
  size_t fChannelWidth;
  std::map<Direction,double> fPosition;
  std::map<Direction,double> fSize;
  std::map<Direction,std::pair<double,double>> fExtent;
  bool fIsSelected;
  std::map<std::string,double> fTruePurity;
  double  fSumPeak;
  double  fRecoEnergy;
  size_t  fAPA;
  std::string fTrueGenType;
  double  fTrueEnergy;
  int     fTrueMarleyIndex;
  std::map<Direction,double> fTruePosition;
  std::map<Direction,double> fTrueDirection;
  bool fIsTriggering;
};


#endif
