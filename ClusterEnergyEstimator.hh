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
  void ActivationFunctionRelu(boost::numeric::ublas::vector<double>& v) const {
    for (size_t i=0; i<v.size(); ++i) {
      v[i] = TMath::Max(0.,v[i]);
    }
  };
  void ActivationFunctionSigmoid(boost::numeric::ublas::vector<double>& v) const {
    for (size_t i=0; i<v.size(); ++i) {
      v[i] = 1. / (1. + TMath::Exp(-v[i]));
    }
  };
  
  void LoadChildAndAssertExistance(const XMLNodePointer_t a,
                                   std::map<std::string,XMLNodePointer_t>& b,
                                   const std::vector<std::string>& c,
                                   const int i=1) const;
  boost::numeric::ublas::vector<double> NeuralNetwork(const boost::numeric::ublas::vector<double>&) const;
  void NormaliseFeature(boost::numeric::ublas::vector<double>& ,
                        const boost::numeric::ublas::vector<double>,
                        const boost::numeric::ublas::vector<double>) const;
  void RecoverFeature(boost::numeric::ublas::vector<double>&,
                      const boost::numeric::ublas::vector<double>,
                      const boost::numeric::ublas::vector<double>) const;
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
  TStopwatch *fStopwatch;
  

};

#endif
