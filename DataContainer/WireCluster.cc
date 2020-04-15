#include "WireCluster.hh"
#include "WireHit.hh"

int WhichPlane(int Channel) {
  if (Channel%2560 < 800)                        return 0;
  if (Channel%2560 < 1600 && Channel%2560 > 800) return 1;
  else                                           return 2;
}

WireCluster::WireCluster(std::vector<Hit*>& vec_Hits):
  Cluster(vec_Hits),
  fWirePlane(WhichPlane(fHit[0]->GetChannel())),
  fMC_UnderlyingE(0),
  fNElectron(0){
  Initialise();
  for (auto const& it: fHit) {
    WireHit* hit = (WireHit*)it;
    fNElectron += hit->GetNElectron();
  }
}
