#ifndef CLUSTERENERGYESTIMATOR_HH
#define CLUSTERENERGYESTIMATOR_HH
#include "WireCluster.hh"

#include "TXMLEngine.h"
#include "TMath.h"

#include <stdio.h>
#include <stdlib.h>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

class ClusterEnergyEstimator {

public:
  ClusterEnergyEstimator(const std::string filename);
  ~ClusterEnergyEstimator();
  double EstimateEnergy(const WireCluster&) const;
  void   EstimateEnergy(const std::vector<WireCluster*>&) const;
  void   EstimateEnergy(const std::vector<WireCluster*>*) const;
  
private:
  void ParseXMLFile();
  double ActivationFunctionRelu   (const double v) const { return TMath::Max(0.,v);   };
  double ActivationFunctionSigmoid(const double v) const { return 1. / (1. + TMath::Exp(-v)); };
  void LoadChildAndAssertExistance(const XMLNodePointer_t a,
                                   std::map<std::string,XMLNodePointer_t>& b,
                                   const std::vector<std::string>& c,
                                   const int i=1) const;
  double NeuralNetwork(const ublas::vector<double>&) const;
  void NormaliseFeature(ublas::vector<double>& ,
                        const ublas::vector<double>,
                        const ublas::vector<double>) const;
  void RecoverFeature(ublas::vector<double>&,
                      const ublas::vector<double>,
                      const ublas::vector<double>) const;
  TXMLEngine* xml;
  std::string filename;
  int nNeuron;
  int nParam;
  int nOutput;
  boost::numeric::ublas::vector<double> Weight_h1        ;
  boost::numeric::ublas::vector<double> Weight_out       ;
  boost::numeric::ublas::matrix<double> Bias_b1          ;
  boost::numeric::ublas::vector<double> Bias_out         ;
  boost::numeric::ublas::vector<double> InputData_Mean   ;
  boost::numeric::ublas::vector<double> InputData_StdDev ;
  boost::numeric::ublas::vector<double> OutputData_Mean  ;
  boost::numeric::ublas::vector<double> OutputData_StdDev;
  

};

#endif
