#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <iomanip>

#include "TTree.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TGraph.h"
#include "TString.h"
#include "TROOT.h"

#include "Utils/CLI11.hpp"

#include "Clustering/Clustering.hh"
#include "Utils/Helper.h"

//for now: assume input file contains all configs. in future: read in no. of configs without skipping over those with 0 clusters

std::map<int,double> map_TypeToWeight;

double getWeight(const int ClusterType) {
  return map_TypeToWeight[ClusterType];
}


int getClusterType(const std::vector<int>* vec_GenType) {
  int type = -1;
  // background = 0, marley = 1

  std::map<int,int> map_GenTypeToCount;

  //cycle through gentypes in input vector, count instances of each gentype, store counts for each gentype in map_GenTypeToCount
  for(auto const & it : (*vec_GenType))
    map_GenTypeToCount[it]++;  

  int largestCount = 0;
  for(auto const& it : map_GenTypeToCount) {
 
    //find marley gentype and exclude it - why?
 
    std::string it_GenType = ConvertIDIntToString(it.first);
    
    //std::cout << it_GenType << std::endl;
    //std::cout << "GenType " << it.first << ": " << it_GenType << std::endl;
    if(it_GenType != "marley") { 
      if(it.second>largestCount) {
        largestCount = it.second;
        type         = it.first;
      }
    }
  }

  //std::cout << "Largest count: " << largestCount << std::endl;
  //std::cout << "Cluster type: " << type << std::endl;

  return type;
}


void FillEventCountMap(int nhit,
                       TTree* ClusteredHit,
                       std::map<int,double>& map_Config_nBackCluster,
                       std::map<int,std::map<int,std::map<int,int>>>& map_ConfigEventIndex_nSignCluster){
  
  int               in_Event;
  int               in_Config;
  double            in_Type;
  int               in_MarleyIndex;
  std::vector<int>* in_GenType = NULL; //input to getClusterType
 
  //ClusteredHit will either be ClusteredWireHit or ClusteredOpticalHit
  ClusteredHit->SetBranchAddress("Event",        &in_Event      );
  ClusteredHit->SetBranchAddress("Config",       &in_Config     );
  ClusteredHit->SetBranchAddress("Type",         &in_Type       );
  ClusteredHit->SetBranchAddress("MarleyIndex",  &in_MarleyIndex);
  
  if (ClusteredHit->GetListOfBranches()->FindObject("GenType")) {
    //GenType in case of wire hits
    ClusteredHit->SetBranchAddress("GenType", &in_GenType);
  } else {
    //Hit_GenType in case of optical hits
    ClusteredHit->SetBranchAddress("Hit_GenType", &in_GenType);
  }

  CurrentProg = 0;

  for(int i = 0; i < ClusteredHit->GetEntries(); i++) {
    ClusteredHit->GetEntry(i);
    PrintProgress(i, ClusteredHit->GetEntries());

    //if size of element in_GenType is less than min. number of hits per cluster ?
    if ((int)in_GenType->size() < nhit) continue;
    
    if (in_Type == 1) {
      //for marley clusters
      map_ConfigEventIndex_nSignCluster[in_Config][in_Event][in_MarleyIndex]++;
    } else {
      //find gentype for background clusters, find weight and add it to count (?)
      int    ClusterType = getClusterType(in_GenType);
      double weight = getWeight(ClusterType);
      map_Config_nBackCluster[in_Config] += weight;
    }
  }
  return;
  
}


void FillMarleyEventCountMap(int nhit,
                             TTree* ClusteredHit,
                             std::map<int,std::map<int,std::map<int,int>>>& map_ConfigEventIndex_nSignCluster){

  int               in_Event;
  int               in_Config;
  double            in_Type;
  int               in_MarleyIndex;
  std::vector<int>* in_GenType = NULL; //input to getClusterType

  ClusteredHit->SetBranchAddress("Event",        &in_Event      );
  ClusteredHit->SetBranchAddress("Config",       &in_Config     );
  ClusteredHit->SetBranchAddress("Type",         &in_Type       );
  ClusteredHit->SetBranchAddress("MarleyIndex",  &in_MarleyIndex);

  if (ClusteredHit->GetListOfBranches()->FindObject("GenType")) {
    //GenType in case of wire hits
    ClusteredHit->SetBranchAddress("GenType", &in_GenType);
  } else {
    //Hit_GenType in case of optical hits
    ClusteredHit->SetBranchAddress("Hit_GenType", &in_GenType);
  }

  CurrentProg = 0;

  for(int i = 0; i < ClusteredHit->GetEntries(); i++) {
    ClusteredHit->GetEntry(i);
    PrintProgress(i, ClusteredHit->GetEntries());

    if ((int)in_GenType->size() < nhit) continue;

    if (in_Type == 1) {
      map_ConfigEventIndex_nSignCluster[in_Config][in_Event][in_MarleyIndex]++;
    }
  }
  return;

}


void FillBackgroundEventCountMap(int nhit,
                       TTree* ClusteredHit,
                       std::map<int,double>& map_Config_nBackCluster){

  int               in_Event;
  int               in_Config;
  double            in_Type;
  std::vector<int>* in_GenType = NULL; //input to getClusterType

  //ClusteredHit will either be ClusteredWireHit or ClusteredOpticalHit
  ClusteredHit->SetBranchAddress("Event",        &in_Event      );
  ClusteredHit->SetBranchAddress("Config",       &in_Config     );
  ClusteredHit->SetBranchAddress("Type",         &in_Type       );

  if (ClusteredHit->GetListOfBranches()->FindObject("GenType")) {
    //GenType in case of wire hits
    ClusteredHit->SetBranchAddress("GenType", &in_GenType);
  } else {
    //Hit_GenType in case of optical hits
    ClusteredHit->SetBranchAddress("Hit_GenType", &in_GenType);
  }

  CurrentProg = 0;

  for(int i = 0; i < ClusteredHit->GetEntries(); i++) {
    ClusteredHit->GetEntry(i);
    PrintProgress(i, ClusteredHit->GetEntries());

    if ((int)in_GenType->size() < nhit) continue;

    if (in_Type != 1) {
      //find gentype for background clusters, find weight and add it to count (?)
      int    ClusterType = getClusterType(in_GenType);
      double weight = getWeight(ClusterType);
      map_Config_nBackCluster[in_Config] += weight;
    }
  }
  return;

}



void GetEfficiency(const int Config,
                   std::map<int,std::map<int,std::map<int,int>>> const& map_ConfigEventIndex_nSignCluster,
                   std::map<int,int> const& nMarleyEvent,
                   std::pair<double,double>& efficiency) {

  for (auto const& it0 : map_ConfigEventIndex_nSignCluster) {
    
    int ThisConfig = it0.first;
    if (ThisConfig != Config) continue;
    std::map<int, std::map<int,int>> const& map_EventIndex_nSignCluster = it0.second;
    efficiency.first = 0;
    double nMarley=0;

    for (auto const& it1: nMarleyEvent) {
      int ThisEvent = it1.first;
      int nMarley_ThisEvent = it1.second;
      nMarley += nMarley_ThisEvent;
      
      if (map_EventIndex_nSignCluster.find(ThisEvent) != map_EventIndex_nSignCluster.end()) {
        std::map<int,int> const& map_Index_nSignCluster = map_EventIndex_nSignCluster.at(ThisEvent);

        for (int iMarley=0; iMarley<nMarley_ThisEvent; ++iMarley) {
          if (map_Index_nSignCluster.find(iMarley) != map_Index_nSignCluster.end())
            if (map_Index_nSignCluster.at(iMarley) > 0)
              efficiency.first++;
        }
      }
    }

    std::cout << efficiency.first << " signal events in config " << Config
              << " (" << nMarley << " were generated)." << std::endl;
    efficiency.second = sqrt(1./efficiency.first+1./((double)nMarley));
    efficiency.first /=(double)nMarley;
    efficiency.second *= efficiency.first; 
    return;
  }
  
}

void GetBackgroundRate(const int Config,
                       const std::map<int,double> map_Config_nBackCluster,
                       const int nEvent,
                       const double EventTime,
                       const double DetectorScaling,
                       std::pair<double,double>& BackgroundRate) {

  BackgroundRate.first = map_Config_nBackCluster.at(Config) / ((double) nEvent * EventTime) / (DetectorScaling); 
  std::cout << BackgroundRate.first << std::endl;
  BackgroundRate.second = sqrt(1./map_Config_nBackCluster.at(Config)+1./((double)nEvent)) * BackgroundRate.first;
  std::cout << BackgroundRate.second << std::endl;
  return;
}


int main(int argc, char** argv) {

  std::string BkgInputFile  = "";
  std::string MarleyInputFile = "";
  std::string OutputFile = "BackEff.txt";
  int Config = -1;
  int nHit = -1;
  double DetectorScaling = 0.12;
  CLI::App app{"A program to calculate the efficiency of the clustering and the background rate"};

  //app.add_option("-i,--input",   InputFile,       "Input filename (root file, the output file of RunDAQClustering)")->required();
  app.add_option("-b, --background", BkgInputFile,  "Input filename for backgrounds (root file, the output of RunDAQClustering)")->required();
  app.add_option("-m, --marley", MarleyInputFile, "Input filename for MARLEY (root file, the output of RunDAQClustering)")->required();
  app.add_option("-c,--config",  Config,          "What configuration of the clustering to analyse, default runs over all configs");
  app.add_option("-o,--output",  OutputFile,      "Output file name (text file, default: BackEff.txt)");
  app.add_option("-s,--scaling", DetectorScaling, "Detector scaling to go from workspace to 10kT. Default is 0.12 which corresponds to the ratio of volume of the 1x2x6 and the 10kT");
  app.add_option("--hitcut",     nHit,            "An extra hit cut that can be applied here. Minimum number of hits a cluster should have to be considered");
  CLI11_PARSE(app, argc, argv);

  if (nHit < 0) {
    std::cout << "You have not set an nhit cut (option -h)" << std::endl;
    std::cout << "For now, use all the cluster without any restriction on the number of hits." << std::endl;
  }

  
  TFile *f_Input_Bkg = new TFile(BkgInputFile.c_str(), "READ");
  TTree *BkgClusteredWireHit    = (TTree*)f_Input_Bkg->Get("ClusteredWireHit");
  TTree *BkgClusteredOpticalHit = (TTree*)f_Input_Bkg->Get("ClusteredOpticalHit");
  TTree *BkgTrueInfo            = (TTree*)f_Input_Bkg->Get("TrueInfo");

  TFile *f_Input_Marl = new TFile(MarleyInputFile.c_str(), "READ");
  TTree *MarlClusteredWireHit    = (TTree*)f_Input_Marl->Get("ClusteredWireHit");
  TTree *MarlClusteredOpticalHit = (TTree*)f_Input_Marl->Get("ClusteredOpticalHit");
  TTree *MarlTrueInfo            = (TTree*)f_Input_Marl->Get("TrueInfo");


  //TFile *f_Input = new TFile(InputFile.c_str(), "READ");
  //TTree *ClusteredWireHit    = (TTree*)f_Input->Get("ClusteredWireHit");
  //TTree *ClusteredOpticalHit = (TTree*)f_Input->Get("ClusteredOpticalHit");
  //TTree *TrueInfo            = (TTree*)f_Input->Get("TrueInfo");
  //TH1D* config_histo = new TH1D("config_histo",";Config;Clusters", 200, 0, 200);


  //DYNAMIC BACKGROUNDS	
  std::map<std::string, int> ID_map;
  std::vector<std::string> AllGenTypeDynamic;
  std::vector<int> pur_dynamic;
  std::cout << "[Analyser] Dynamic: Loading IDs" << std::endl;
  std::string delim = "/";
  std::string ID_tree = "fIDs";
  
  if (f_Input_Bkg->Get( ID_tree.c_str() )) {
    TTree *t_IDs = (TTree*)f_Input_Bkg->Get( ID_tree.c_str() );
    TObjArray *branchList;
    branchList  = t_IDs->GetListOfBranches();
    int nBranch = t_IDs->GetNbranches();
    TString IDtreenames[nBranch];

    //std::cout << "ID trees: " << nBranch << std::endl;
    for(int i=0;i<nBranch;i++){
      IDtreenames[i] = branchList->At(i)->GetName();

      int temp_id;
      t_IDs->SetBranchAddress(IDtreenames[i], &temp_id);
      t_IDs->GetEntry(0);
      std::string delimiter = "_";
      std::string temp_string = IDtreenames[i].Data();
      std::string token = temp_string.substr(0, temp_string.find(delimiter));
      std::pair<std::string, int> temp_pair {token, temp_id};
      ID_map.insert( temp_pair );
      AllGenTypeDynamic.push_back(token);
    }
    std::pair<std::string, int> temp_pair_allbckg {"AllBackground", 99};
    ID_map.insert( temp_pair_allbckg );
    AllGenTypeDynamic.push_back("AllBackground");
    std::pair<std::string, int> temp_pair_all {"All", 100};
    ID_map.insert( temp_pair_all );
    AllGenTypeDynamic.push_back("All");

    //std::cout << "[Analyser] Loaded Dynamic IDs" << std::endl;
    //for (auto const& x : ID_map){
    //  std::cout << x.first << " : " << x.second << std::endl;
    //}


    SetDynamicVars(ID_map); 
  } else {
    std::cerr << "The requested tree 'fIDs' does not exist in file " << BkgInputFile << std::endl;
  }


  //ClusteredWireHit->Project("config_histo", "Config");
  //ClusteredOpticalHit->Project("config_histo", "Config");
  int nMaxConfig=47;
  //for (int i=0; i<48; ++i) {
  //  if (config_histo->GetBinContent(i)>0){}
  //  ++nMaxConfig;
  //}
  std::cout << "This file contains " << nMaxConfig << " configs." << std::endl;

  //if Config=-1, nConfig = 47, if not, nConfig = Config+1. Why?
  int nConfig = (Config==-1? nMaxConfig: Config+1);
  //if Config=-1, iterConfig=0, else iterConfig = Config
  int iterConfig = (Config==-1? 0:Config);

  //std::cout << nConfig << std::endl;
  //std::cout << iterConfig << std::endl;
  
  if (Config > nMaxConfig) {
    std::cerr << "Requested config (" << Config << ") doesn't exist (Config can be from 0 to " << nMaxConfig << ")\n";
    exit(1);
  }
  
  int nBkgEventOriginally = (int)BkgTrueInfo->GetEntries();
  std::cout << "THERE WERE " << nBkgEventOriginally << " EVENTS IN THE BACKGROUND SAMPLE AND " << nConfig << " CONFIGURATIONS\n";
 
  int AllBackgroundID = ConvertIDStringToInt("AllBackground");
  //std::cout << "AllBackground GenType: " << AllBackgroundID;

  for(int i=0;i<AllBackgroundID;++i) {
    map_TypeToWeight[i] = 1;
    //std::cout << "Setting weight for GenType " << i << std::endl;
  }

  std::vector<double>* dummy=NULL;

  int nMarlEvent = MarlTrueInfo->GetEntries();
  int in_Event;
  MarlTrueInfo->SetBranchAddress("Event", &in_Event);
  //MarlTrueInfo->SetBranchAddress("MarlTime", &dummy);
  MarlTrueInfo->SetBranchAddress("ENu", &dummy);


  //find the marley events

  std::map<int,int> map_Event_nMarley;
  int nMarleyEvent=0;
  for(int i = 0; i < nMarlEvent; i++) {
    MarlTrueInfo->GetEntry(i);
    map_Event_nMarley[in_Event] = (int)dummy->size();
    nMarleyEvent += (int)dummy->size();
  }
  
  std::cout << "There were " << nMarleyEvent << " Marley events in " << map_Event_nMarley.size() << " Larsoft events." << std::endl;


  //OVERALL EFFICIENCIES AND BACKGROUND RATES.
  std::map<int,double> map_Config_nWireBackCluster; //no. of background wire clusters in each config, used to calculate overall BKG rate
  std::map<int,std::map<int,std::map<int,int>>> map_ConfigEventIndex_nWireSignCluster; //no. of signal wire clusters per event index (?) per config, used to calculate SN efficiency


//  FillEventCountMap(nHit, ClusteredWireHit,
//                    map_Config_nWireBackCluster,
//                    map_ConfigEventIndex_nWireSignCluster);

  FillMarleyEventCountMap(nHit, MarlClusteredWireHit, map_ConfigEventIndex_nWireSignCluster);

  FillBackgroundEventCountMap(nHit, BkgClusteredWireHit, map_Config_nWireBackCluster);

  std::map<int,double> map_Config_nOpticalBackCluster; //used for BKG rate
  std::map<int,std::map<int,std::map<int,int>>> map_ConfigEventIndex_nOpticalSignCluster; //used for SN efficiencies

//  FillEventCountMap(nHit, ClusteredOpticalHit,
//                    map_Config_nOpticalBackCluster,
//                    map_ConfigEventIndex_nOpticalSignCluster);

  FillMarleyEventCountMap(nHit, MarlClusteredOpticalHit, map_ConfigEventIndex_nOpticalSignCluster);

  FillBackgroundEventCountMap(nHit, BkgClusteredOpticalHit, map_Config_nOpticalBackCluster);

  //maps of efficiencies and bkg rates for each config
  std::map<int,std::pair<double,double>> map_Config_WireEff;
  std::map<int,std::pair<double,double>> map_Config_WireBackRate;
  std::map<int,std::pair<double,double>> map_Config_OpticalEff;
  std::map<int,std::pair<double,double>> map_Config_OpticalBackRate;

  for (int c=iterConfig; c<48; ++c) {
    std::cout << "N background wire clusters " << map_Config_nWireBackCluster[c] << " in config " << c << std::endl;
    std::cout << "N background opti clusters " << map_Config_nOpticalBackCluster[c] << " in config " << c << std::endl;

    //SN efficiency - wires
    GetEfficiency(c,
                  map_ConfigEventIndex_nWireSignCluster,
                  map_Event_nMarley,
                  map_Config_WireEff[c]);

    //SN efficiency - optical
    GetEfficiency(c,
                  map_ConfigEventIndex_nOpticalSignCluster,
                  map_Event_nMarley,
                  map_Config_OpticalEff[c]);

    //background rate - wires
    GetBackgroundRate(c,
                      map_Config_nWireBackCluster,
                      nBkgEventOriginally,
                      (8500/2e6),
                      DetectorScaling,
                      map_Config_WireBackRate[c]);

    //background rate - optical
    //NOT WORKING!!
    GetBackgroundRate(c,
                      map_Config_nOpticalBackCluster,
                      nBkgEventOriginally,
                      3.2e-3*3.,
                      DetectorScaling,
                      map_Config_OpticalBackRate[c]);
  }
  
  std::ofstream txt_Result;
  txt_Result.open(OutputFile.c_str());
  for(auto const& it : map_Config_WireEff) {
    int c = it.first;
    std::cout << std::setprecision(4);
    std::cout  << "Config (Wire) " << c
               << ", SN efficiency: " << it.second.first << " +/- " << it.second.second
               << " Background rate in 10kt (Hz): " << map_Config_WireBackRate[c].first << " +/- " << map_Config_WireBackRate[c].second << std::endl;
    txt_Result << c 
               << " 10 " << it.second.first << " " << it.second.second
               << " " << map_Config_WireBackRate[c].first
               << " " << map_Config_WireBackRate[c].second << " Config_" << c << std::endl;
  }
  
  for(auto const& it : map_Config_WireEff) {
    int c = it.first;
    std::cout  << "Config (Opti) " << c
               << ", SN efficiency: " << map_Config_OpticalEff[c].first << " +/- " << map_Config_OpticalEff[c].second
               << " Background rate in 10kt (Hz): " << map_Config_OpticalBackRate[c].first << " +/- " << map_Config_OpticalBackRate[c].second << std::endl;
  }

  

  return 0;

}
