#include <iostream>
#include <map>
#include <memory>

#include "TH1D.h"
#include "TChain.h"
#include "TTree.h"
#include "TFile.h"
#include "TVectorT.h"

#include "Utils/CLI11.hpp"
#include "Utils/Helper.h"

using namespace std;

int GetGenType(vector<int> GenType) {
  map<int,int> counts;
  for (auto const& it: GenType) {
    counts[it]++;
  }
  int max=-1;
  int type=ConvertIDStringToInt("Other");
  for (auto const& it: counts) {
    if (it.second>max){
      max = it.second;
      type = it.first;
    }
  }
  return type;
}

int main(int argc, char** argv) {
  CLI::App app{"SmartTrigger"};

  string InputChainName = "ClusteredWireHit";
  string InputTrueChainName = "TrueInfo";
  string Feature = "SumADC";
  string InputListFile = "";
  string OutputFileName = "LikelihoodTrigger.root";

  int nConfig;
  app.add_option("--feature", Feature, "Feature");
  app.add_option("-l,--list",  InputListFile, "Input file list")->required();
  app.add_option("-o,--output", OutputFileName, "Output file name");
  app.add_option("-n,--nconfig", nConfig, "Number of configurations in the input file")->required();
  CLI11_PARSE(app, argc, argv);

  unique_ptr<TChain> InputChain     = make_unique<TChain>(InputChainName    .c_str());
  unique_ptr<TChain> InputTrueChain = make_unique<TChain>(InputTrueChainName.c_str());

  string line;
  ifstream myfile (InputListFile);
  if (myfile.is_open()){
    cout << "Parsing list of files...\n";
    while ( getline (myfile,line) ){
      InputChain->Add(line.c_str());
      InputTrueChain->Add(line.c_str());
    }
  } else {
    cerr << "File " << InputListFile << " doesnt exist.\n";
    throw;
  }

  //DYNAMIC BACKGROUNDS
  std::map<std::string, int> ID_map;
  std::vector<std::string> AllGenTypeDynamic;
  std::vector<int> pur_dynamic;
  std::cout << "[Analyser] Dynamic: Loading IDs" << std::endl;
  std::string delim = "/";
  std::string ID_tree = "fIDs";

  if (InputChain->GetFile()->Get( ID_tree.c_str() )) {
    TTree *t_IDs = (TTree*)InputChain->GetFile()->Get( ID_tree.c_str() );
    TObjArray *branchList; 
    branchList  = t_IDs->GetListOfBranches();
    int nBranch = t_IDs->GetNbranches();
    TString IDtreenames[nBranch];
  
    std::cout << "ID trees: " << nBranch << std::endl;
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

   std::cout << "[Analyser] Loaded Dynamic IDs" << std::endl;
   for (auto const& x : ID_map){
     std::cout << x.first << " : " << x.second << std::endl;
   }
  
   // setup some vars in helper that will be used throughout
   SetDynamicVars(ID_map); 
 } else {
   std::cerr << "The requested tree 'fIDs' does not exist in file " << InputListFile << std::endl;
 }

  map<int,map<int,shared_ptr<TH1D>>> PDF_Background_config;// [config][type]
  for (int iConfig=0; iConfig<nConfig; ++iConfig) {
    for (auto const& x : ID_map ) {  
      PDF_Background_config[iConfig][x.second] = make_shared<TH1D>(("PDF_Background_"+x.first+"_config"+to_string(iConfig)).c_str(), "PDF;SADC;", 20, 0, 200); 
    }
  }
  
  map<TFile*,map<int,map<int,shared_ptr<TH1D>>>> PDF_Background_config_perfile;// [file][config][type]
  map<int,map<TFile*,map<int,set<int>>>> nMarleyEventDetected;// [config][file][event][marleyindex]
  map<int,shared_ptr<TH1D>> PDF_Background_config_notype;
  for (int iConfig=0; iConfig<nConfig; ++iConfig) {
    PDF_Background_config_notype[iConfig] = make_shared<TH1D>(("PDF_Background_config"+to_string(iConfig)).c_str(), "PDF;SADC;", 20, 0, 200); 
  }
  
  map<TFile*,map<int,shared_ptr<TH1D>>> PDF_Background_config_perfile_notype;
  int Config=0;
  int Event=0;
  double SumADC=0;
  int MarleyIndex=0;

  vector<int> *GenType = NULL;
  vector<double> *ENu     = NULL;
  vector<double> *Time    = NULL;
  int Event_true = 0;

  InputChain->SetBranchAddress("Config",        &Config     );
  InputChain->SetBranchAddress("Event",         &Event      );
  InputChain->SetBranchAddress(Feature.c_str(), &SumADC     );
  InputChain->SetBranchAddress("GenType",       &GenType    );
  InputChain->SetBranchAddress("MarleyIndex",   &MarleyIndex);
  
  InputTrueChain->SetBranchAddress("ENu",      &ENu       );
  InputTrueChain->SetBranchAddress("MarlTime", &Time      );
  InputTrueChain->SetBranchAddress("Event",    &Event_true);

  size_t nMarleyEventGenerated=0;
  size_t nBackgroundEventGenerated=0;

  unique_ptr<TFile> OutputFile = make_unique<TFile>(OutputFileName.c_str(), "RECREATE");
  OutputFile->cd();
 
  TTree* sum_adc_enu = new TTree("mapping", "mapping");
  double enu_tree=0, sum_adc_tree=0, time_tree=0;
  int config_tree=0;
  sum_adc_enu->Branch("Time",          &time_tree   );
  sum_adc_enu->Branch("ENu",           &enu_tree    );
  sum_adc_enu->Branch(Feature.c_str(), &sum_adc_tree);
  sum_adc_enu->Branch("Config",        &config_tree );

  map<string, map<int, map<int, double> > > enu_mapping;//[filename(ouch)][event][marleyindex] -> enu
  map<string, map<int, map<int, double> > > time_mapping;//[filename(ouch)][event][marleyindex] -> time
  
  for (int iEntry = 0; iEntry<InputTrueChain->GetEntries(); ++iEntry) {
    InputTrueChain->GetEntry(iEntry);
    nMarleyEventGenerated+=ENu->size();
    int i=0;
    for (auto const& enu: (*ENu)) {
      enu_mapping[InputTrueChain->GetFile()->GetName()][Event_true][i++] = enu;
    }
    i=0;
    for (auto const& timeit: (*Time)) {
      time_mapping[InputTrueChain->GetFile()->GetName()][Event_true][i++] = timeit;
    }
    size_t pos=string(InputTrueChain->GetFile()->GetName()).find("prodbackground_5x_radiological");
    if (pos == string::npos) {
      nBackgroundEventGenerated++;
    } else {
      nBackgroundEventGenerated+=5;
    }
  }

  cout << "Filled the map for true\n";
  
  int nEntries = InputChain->GetEntries();

  for (int iEntry=0; iEntry<nEntries; ++iEntry) {
    InputChain->GetEntry(iEntry);
    PrintProgress(iEntry,nEntries);
    if (Config>=nConfig) {
      std::cout << "Config : " << Config << "\n";
      std::cout << "nConfig : " << nConfig << "\n";
      std::cout << "number of config is inconsistent! change nconfig to be Config+1\n";
      throw;
    }


    int type = GetGenType(*GenType);
    string type_str = ConvertIDIntToString(type); 
    if (type_str == "marley") {
      nMarleyEventDetected[Config][InputChain->GetFile()][Event].insert(MarleyIndex);
      sum_adc_tree = SumADC/100;
      enu_tree    = enu_mapping [InputChain->GetFile()->GetName()][Event][MarleyIndex];
      time_tree   = time_mapping[InputChain->GetFile()->GetName()][Event][MarleyIndex];
      config_tree = Config;
      sum_adc_enu->Fill();
      //cout << MarleyIndex << "\n";
    } else {
      PDF_Background_config_notype[Config]->Fill(SumADC/100.);
      if (PDF_Background_config_perfile_notype[InputChain->GetFile()][Config]) {
        PDF_Background_config_perfile_notype[InputChain->GetFile()][Config]->Fill(SumADC/100.);
      } else {
        for (int iConfig=0; iConfig<nConfig; ++iConfig) {
          // std::cout << "Creating for " << InputChain->GetFile()->GetName() << " " << iConfig << "\n";
          PDF_Background_config_perfile_notype[InputChain->GetFile()][iConfig] =  make_shared<TH1D>(("PDF_Background_perfile_config"+to_string(iConfig)+"_file_"+InputChain->GetFile()->GetName()).c_str(), "PDF;SADC;", 20, 0, 200);
        }
      }
    }
    PDF_Background_config[Config][type]->Fill(SumADC/100.);

    if (PDF_Background_config_perfile[InputChain->GetFile()].size()>0) {
      PDF_Background_config_perfile[InputChain->GetFile()][Config][type]->Fill(SumADC/100.);
    } else {
      for (int iConfig=0; iConfig<nConfig; ++iConfig) {
        for (auto const& x : ID_map ) {
          // std::cout << "Creating for " << InputChain->GetFile()->GetName() << " " << iConfig << " " << iBack << "\n";
          PDF_Background_config_perfile[InputChain->GetFile()][iConfig][x.second] = make_shared<TH1D>(("PDF_Background_perfile"+x.first+"_config"+to_string(iConfig)+"_file_"+InputChain->GetFile()->GetName()).c_str(), "PDF;SADC;", 20, 0, 200);
        }
      }
    }
  }

  TVectorD efficiencies(nConfig);
  for (auto const& it: nMarleyEventDetected) {
    efficiencies[it.first] = 0;
    for (auto const& it2: it.second) {
      for (auto const& it3: it2.second) {
        //cout << it.first << " " << it3.second.size() << "\n";
        efficiencies[it.first] += it3.second.size();
      }
    }
    efficiencies[it.first] /= nMarleyEventGenerated;
  }
  OutputFile->cd();
  efficiencies.Print();
  efficiencies.Write("Efficiencies");
  sum_adc_enu->Write();

  double scaleBackground = 1. / 2.2e-3 / nBackgroundEventGenerated / 0.12;
  for (auto const& it: PDF_Background_config_notype) {
    it.second->Scale(scaleBackground);
    it.second->Write();
  }
  for (auto const& it: PDF_Background_config_perfile_notype) {
    for (auto const& it2: it.second) {
      it2.second->Write();
    }
  }

  
  for (auto& it: PDF_Background_config) {
    for(auto& it2: it.second) {
      it2.second->Scale(scaleBackground);
      it2.second->Write();
    }
  }

  for (auto& it: PDF_Background_config_perfile) {
    for(auto& it2: it.second) {
      for(auto& it3: it2.second) {
        it3.second->Write();
      }
    }
  }
    OutputFile->Close();
}
