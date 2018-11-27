#include "OpticalCluster.hh"

OpticalCluster::OpticalCluster(std::vector<Hit*>& vec_Hits):
  Cluster(vec_Hits) {
  Initialise();
}
