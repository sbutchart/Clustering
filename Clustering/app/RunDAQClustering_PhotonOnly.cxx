#include "Clustering/Clustering.hh"
#include "Utils/CLI11.hpp"

int main(int argc, char** argv){
  CLI::App app{"A program to run the clustering"};

  int nEvent=-1;
  int PrintLevel=-1;
  int nAPA=-1;
  int Config=-1;
  int Offset=0;
  int OutputOffset=0;
  std::string InputFile    = "";
  std::string OutputFile   = "";
  std::string ERecoXMLFile = "";
  std::string InputTree    = "";

  app.add_option("-a,--n-apa",  nAPA,            "Number of APA to consider, this can be used for time tests, default to all of the APA");
  app.add_option("-c,--config", Config,          "What configuration of the clustering to run (default, run all the config)");
  app.add_option("--e-reco",    ERecoXMLFile,    "The XML file containing all the weights for the neural network for energy reconstruction (default, doesn't do energy reconstruction)");
  app.add_option("-o,--output", OutputFile,      "Output file name (with root extension)")->required();
  app.add_option("-i,--input",  InputFile,       "Input filename (root file, the output file of SNAna_Module)")->required();
  app.add_option("-e,--event",  nEvent,          "The number of events to run on (not sure this is working properly, use with caution)");
  app.add_option("-p,--print",  PrintLevel,      "Print level, (number between -1 and 2 for increasing debug messages)");
  app.add_option("-t,--tree",   InputTree,       "The name of the tree in the input file")->required();
  app.add_option("--offset",    OutputOffset,    "The offset in event numbers for the output file");
  CLI11_PARSE(app, argc, argv);


  
  Clustering c;
  c.SetOutputOffset(OutputOffset);
  c.SetInputFile   (InputFile   );
  c.SetConfig      (Config      );
  c.SetOffset      (Offset      );
  c.SetNAPA        (nAPA        );
  c.SetERecoXMLFile(ERecoXMLFile);
  c.SetInputTree   (InputTree   );
  c.SetOutputFile  (OutputFile  );
  c.SetPrintLevel  (PrintLevel  );
  c.SetUsePDS      (true);
  c.SetUseTPC      (false);
  c.Initialise();
  c.ClusterAll(nEvent);
  
  return 0;
}
