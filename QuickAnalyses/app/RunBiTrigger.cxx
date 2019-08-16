#include <memory>

#include "TFile.h"
#include "TTree.h"

#include "QuickAnalyses/BiTrigger.hh"
#include "DataContainer/OpticalCluster.hh"
#include "DataContainer/WireCluster.hh"


int main(int argc, char** argv) {

  int opt;
  std::string InputFile = "";

  extern char *optarg;
  extern int  optopt;
  // Retrieve the options:
  while ( (opt = getopt(argc, argv, "i:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'i':
      InputFile = optarg;
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
  
  if (InputFile == "") {
    std::cout << "No input file!!" << std::endl;
    exit(1);
  }
 
  std::vector<std::unique_ptr<WireCluster   >> wire_cluster;
  std::vector<std::unique_ptr<OpticalCluster>> opti_cluster;

  TFile f(InputFile.c_str(), "READ");
  TTree* wire_tree = (TTree*)f.Get("ClusteredWireHit");
  TTree* opti_tree = (TTree*)f.Get("ClusteredOpticalHit");
  int    Event   = 0;
  double nHit    = 0;
  double APA     = 0;
  double SumPeak = 0;
  double Type    = 0;

  BiTrigger bt;
  
  wire_tree->SetBranchAddress("Event",   &Event  );
  wire_tree->SetBranchAddress("APA",     &APA    );
  wire_tree->SetBranchAddress("SumADC",  &SumPeak);
  wire_tree->SetBranchAddress("NHits",   &nHit   );
  wire_tree->SetBranchAddress("Type",    &Type   );
  opti_tree->SetBranchAddress("Event",   &Event  );
  opti_tree->SetBranchAddress("APA",     &APA    );
  opti_tree->SetBranchAddress("SumPE",   &SumPeak);
  opti_tree->SetBranchAddress("NHits",   &nHit   );
  opti_tree->SetBranchAddress("Type",    &Type   );
  
  std::map<int,std::vector<int>> event_entry_wire;
  std::map<int,std::vector<int>> event_entry_opti;
  std::set<int> set_event;
  
  for (int i=0; i<wire_tree->GetEntries(); ++i) {
    wire_tree->GetEntry(i);
    event_entry_wire[Event].push_back(i);
    set_event.insert(Event);
  }

  for (int i=0; i<opti_tree->GetEntries(); ++i) {
    opti_tree->GetEntry(i);
    event_entry_opti[Event].push_back(i);
    set_event.insert(Event);
  }

  int nevent=0;
  int totevent=set_event.size();
  for (auto const& it: set_event) {
    nevent++;
    PrintProgress(nevent,totevent);
    
    wire_cluster.clear();
    opti_cluster.clear();
    for (auto const& entry_wire: event_entry_wire[it]) {
      wire_tree->GetEntry(entry_wire);
      auto wc = std::unique_ptr<WireCluster>(new WireCluster);
      wc->SetSumPeak(SumPeak);
      wc->SetNHit(nHit);
      wc->SetGenType(ConvertIntToGenType((int)Type));
      wc->SetAPA(APA);
      if(nHit>3)
        wire_cluster.push_back(std::move(wc));
    }
    for (auto const& entry_opti: event_entry_opti[it]) {
      opti_tree->GetEntry(entry_opti);
      auto oc = std::unique_ptr<OpticalCluster>(new OpticalCluster);
      oc->SetSumPeak(SumPeak);
      oc->SetNHit(nHit);
      oc->SetGenType(ConvertIntToGenType((int)Type));
      oc->SetAPA(APA);
      if(nHit>20)
        opti_cluster.push_back(std::move(oc));
    }
    bt.AddEventToStudy(wire_cluster, opti_cluster);
  }
  bt.FinishStudy(); 
  

  return 0;
}
