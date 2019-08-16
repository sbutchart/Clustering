


#include "AdvancedClustering.hh"

int main(int argc, char** argv){
    int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  int nEvent=-1;
  int PrintLevel=-1;
  int Config=-1;
  int Offset=0;
  std::string InputFile    = "";
  std::string OutputFile   = "";
  std::string ERecoXMLFile = "";
  std::string InputTree    = "";
  while ( (opt = getopt(argc, argv, "n:p:i:o:t:e:a:c:s:f:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'n':
      nEvent = atoi(optarg);
      break;
    case 'c':
      Config = atoi(optarg);
      break;
    case 's':
      Offset = atoi(optarg);
      break;
    case 'p':
      PrintLevel = atoi(optarg);
      break;
    case 'i':
      InputFile  = std::string(optarg);
      break;
    case 'o':
      OutputFile = std::string(optarg);
      break;
    case 't':
      InputTree = std::string(optarg);
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  AdvancedClustering c;
  c.SetInputFile   (InputFile   );
  c.SetOffset      (Offset      );
  c.SetInputTree   (InputTree   );
  c.SetOutputFile  (OutputFile  );
  c.SetPrintLevel  (PrintLevel  );
  return c.ClusterAll(nEvent  );
};


