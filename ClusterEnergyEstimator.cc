#include "ClusterEnergyEstimator.hh"

using namespace boost::numeric::ublas;

ClusterEnergyEstimator::ClusterEnergyEstimator(std::string f):
  xml(NULL),
  filename(f),
  nNeuron(0),
  nParam(4),
  nOutput(1),
  Weight_h1 (),
  Weight_out(),
  Bias_b1   (),
  Bias_out  (),
  InputData_Mean   (),
  InputData_StdDev (),
  OutputData_Mean  (),
  OutputData_StdDev()
{
  ParseXMLFile();
};

ClusterEnergyEstimator::~ClusterEnergyEstimator() {
  delete xml;
  xml=NULL;
  Weight_h1 .clear();
  Weight_out.clear();
  Bias_b1   .clear();
  Bias_out  .clear();
  InputData_Mean   .clear();
  InputData_StdDev .clear();
  OutputData_Mean  .clear();
  OutputData_StdDev.clear();
};

double ClusterEnergyEstimator::EstimateEnergy(const WireCluster& cluster) const {

  vector<double> Feature(nParam);
  Feature[0] = static_cast<double>(cluster.GetChannelWidth());
  Feature[1] = static_cast<double>(cluster.GetNHit());
  Feature[2] = cluster.GetHitSumADC();
  Feature[3] = cluster.GetTimeWidth();
  NormaliseFeature(Feature, InputData_Mean, InputData_StdDev);
  vector<double> ereco(1);
  ereco = NeuralNetwork(Feature);
  RecoverFeature(ereco, OutputData_Mean, OutputData_StdDev);
  return ereco(0);
};

vector<double> ClusterEnergyEstimator::NeuralNetwork(const vector<double>& Feature) const {

  vector<double> layer_1(100);
  // layer_1 = prod(Feature,Weight_h1)+Bias_b1;
  ActivationFunctionRelu(layer_1);
  vector<double> output_layer(nOutput);
  // output_layer = prod(layer_1,Weight_out)+Bias_out;
  return output_layer;
};

void ClusterEnergyEstimator::NormaliseFeature(vector<double>& Feature,
                                              const vector<double> Mean,
                                              const vector<double> StdDev) const {

  Feature = element_div(Feature - Mean,StdDev);
};

void ClusterEnergyEstimator::RecoverFeature(vector<double>& Feature,
                                            const vector<double> Mean,
                                            const vector<double> StdDev) const {
  

  Feature = element_prod(Feature, StdDev) + Mean;
};


void ClusterEnergyEstimator::EstimateEnergy(const std::vector<WireCluster*>& clusters) const {
  for (auto& it : clusters)
    it->SetEReco(EstimateEnergy(*it));
  return;
};

void ClusterEnergyEstimator::EstimateEnergy(const std::vector<WireCluster*>* clusters) const{
  for (auto& it : (*clusters))
    it->SetEReco(EstimateEnergy(*it));
  return;
};

void ClusterEnergyEstimator::ParseXMLFile() {

  std::cout << "Parsing the XML file" << std::endl;
  // First create engine
  xml = new TXMLEngine();

  // Now try to parse xml file
  // Only file with restricted xml syntax are supported
  XMLDocPointer_t xmldoc = xml->ParseFile(filename.c_str());
  if (!xmldoc) return;
  
  // take access to main node
  XMLNodePointer_t mainnode = xml->DocGetRootElement(xmldoc);

  std::map<std::string,XMLNodePointer_t> map_main;
  std::vector<std::string> requiredchild = {"Param","InputData","OutputData","Bias","Weight"};
  LoadChildAndAssertExistance(mainnode, map_main, requiredchild);
  std::map<std::string,XMLNodePointer_t> map_param;
  requiredchild = {"nNeuron","Activation"};
  LoadChildAndAssertExistance(map_main["Param"], map_param, requiredchild);
  std::string content = xml->GetNodeContent(map_param.at("nNeuron"));
  nNeuron = stoi(content);

  std::map<std::string,XMLNodePointer_t> map_inputdata;
  std::map<std::string,XMLNodePointer_t> map_outputdata;
  requiredchild = {"Mean","StdDev"};
  LoadChildAndAssertExistance(map_main["InputData"] , map_inputdata,  requiredchild, nParam);
  LoadChildAndAssertExistance(map_main["OutputData"], map_outputdata, requiredchild, nOutput);
  InputData_Mean    = vector<double>(nParam);
  InputData_StdDev  = vector<double>(nParam);
  OutputData_Mean   = vector<double>(nOutput);
  OutputData_StdDev = vector<double>(nOutput);
  
  for (int param=0; param<nParam; ++param) {
    std::string meanstr= Form("Mean_%i",param);
    if (map_inputdata.find(meanstr) != map_inputdata.end()) {
      content = xml->GetNodeContent(map_inputdata.at(meanstr));
      double d = stod(content);
      InputData_Mean(param) = d;
    }

    std::string stddevstr= Form("StdDev_%i",param);
    if (map_inputdata.find(stddevstr) != map_inputdata.end()) {
      content = xml->GetNodeContent(map_inputdata.at(stddevstr));
      double d = stod(content);
      InputData_StdDev(param) = d;
    }
  }

  std::string meanstr= "Mean";
  if (map_outputdata.find(meanstr) != map_outputdata.end()) {
    content = xml->GetNodeContent(map_outputdata.at(meanstr));
    double d = stod(content);
    OutputData_Mean(0) = d;
  }

  std::string stddevstr= "StdDev";
  if (map_outputdata.find(stddevstr) != map_outputdata.end()) {
    content = xml->GetNodeContent(map_outputdata.at(stddevstr));
    double d = stod(content);
    OutputData_StdDev(0) = d;
  }
  
  std::map<std::string,XMLNodePointer_t> map_bias;
  requiredchild = {"b1","out"};
  LoadChildAndAssertExistance(map_main["Bias"], map_bias, requiredchild);

  std::map<std::string,XMLNodePointer_t> map_bias_b1_param;
  requiredchild = {"Param"};
  LoadChildAndAssertExistance(map_bias["b1"], map_bias_b1_param, requiredchild, nParam);

  std::map<std::string,std::map<std::string,XMLNodePointer_t>> map_bias_b1_param_layer;
  requiredchild = {"Layer_1"};
  for(int i=0; i<nParam; ++i) {
    std::map<std::string,XMLNodePointer_t> dummy;
    LoadChildAndAssertExistance(map_bias_b1_param.at(Form("Param_%i",i)),
                                 dummy, requiredchild, nNeuron);
    map_bias_b1_param_layer[Form("Param_%i",i)] = dummy;
  }

  std::map<std::string,XMLNodePointer_t> map_bias_out_param;
  requiredchild = {"Layer_1"};
  LoadChildAndAssertExistance(map_bias["out"], map_bias_out_param, requiredchild, nNeuron);

  std::map<std::string,XMLNodePointer_t> map_weight;
  requiredchild = {"h1","out"};
  LoadChildAndAssertExistance(map_main["Weight"], map_weight, requiredchild);

  std::map<std::string,XMLNodePointer_t> map_weight_h1_param;
  requiredchild = {"Layer_1"};
  LoadChildAndAssertExistance(map_weight["h1"], map_weight_h1_param, requiredchild, nNeuron);

  std::map<std::string,XMLNodePointer_t> map_weight_out_param;
  requiredchild = {"Layer_1_0"};
  LoadChildAndAssertExistance(map_weight["out"], map_weight_out_param, requiredchild);

  Weight_h1   = vector<double>(nNeuron);
  Weight_out  = vector<double>(nOutput);
  Bias_b1     = matrix<double>(nParam,nNeuron);
  Bias_out    = vector<double>(nNeuron);

  for (int param=0; param<nParam; ++param) {
    std::string paramstr= Form("Param_%i",param);
    
    if (map_bias_b1_param_layer.find(paramstr) != map_bias_b1_param_layer.end()) {
      
      for (int neur=0; neur<nNeuron; ++neur) {
        
        std::string neurstr = Form("Layer_1_%i",neur);

        if (map_bias_b1_param_layer.at(paramstr).at(neurstr)) {
          content = xml->GetNodeContent(map_bias_b1_param_layer.at(paramstr).at(neurstr));
          double d = stod(content);
          Bias_b1(param,neur) = d;
        }
      }
    }
  }
  
  for (int neur=0; neur<nNeuron; ++neur) {
    std::string neurstr = Form("Layer_1_%i",neur);
    content = xml->GetNodeContent(map_bias_out_param.at(neurstr));
    double d = stod(content);
    Bias_out(neur)=d;
  }
  
  for (int neur=0; neur<nNeuron; ++neur) {
    std::string neurstr = Form("Layer_1_%i",neur);
    content = xml->GetNodeContent(map_weight_h1_param.at(neurstr));
    double d = stod(content);
    Weight_h1(neur) = d;
  }

  for (auto const& it: map_weight_out_param) {
    content = xml->GetNodeContent(it.second);
    double d = stod(content);
    Weight_out(0) = d;
  }

  xml->FreeDoc(xmldoc);
};


void ClusterEnergyEstimator::LoadChildAndAssertExistance(const XMLNodePointer_t node,
                                                         std::map<std::string,XMLNodePointer_t>& map_name_node,
                                                         const std::vector<std::string>& requiredchild,
                                                         const int nchild) const {
  
  std::cout << "Checking that node "  << xml->GetNodeName(node) << " contains the child :" << std::endl;
  for (auto const& it:requiredchild)
    std::cout << " - " << it << std::endl;
  std::cout << nchild << " times." << std::endl;

  XMLNodePointer_t child = xml->GetChild(node);
  while (child != 0) {
    map_name_node[xml->GetNodeName(child)] = child;
    child = xml->GetNext(child);
  }
  for (int i=0; i<nchild; ++i) {
    std::string addon = Form("_%i",i);
    if (nchild==1) addon="";
    for (auto const& it:requiredchild)
      if (map_name_node.find(it+addon) == map_name_node.end()) {
        std::cout << "XML File is corrupted!!" << std::endl;
        std::cout << "Node " << xml->GetNodeName(node) << " doesn't contain the child " << it+addon << std::endl;
        std::cout << "Exiting..." << std::endl;
        exit(1);
      }
  }
  std::cout << "DONE Checking node "  << xml->GetNodeName(node) << std::endl;
    
};



