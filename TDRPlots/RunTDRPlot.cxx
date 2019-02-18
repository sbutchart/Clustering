#include "EfficiencyPlot.hh"
#include "SimpleEfficiencyPlot.hh"
#include "EfficiencyPlotMarley.hh"

int main(int argc, char** argv){

  int opt;
  extern char *optarg;
  extern int  optopt;

  std::string InputFile="";
  bool Simple=0;
  
  while ( (opt = getopt(argc, argv, "i:s")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'i':
      InputFile = optarg;
      break;
    case 's':
      Simple=1;
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
  if (InputFile == "") {
    std::cout << "Need to provide an input file with -i" << std::endl;
    exit(1);
  }
  
  if (Simple==1) {
    SimpleEfficiencyPlot e(InputFile);
    e.Run();
    return 0;
  }
  
  
  try {
    EfficiencyPlot e(InputFile);
    e.Run();
  } catch(WrongFileException& e) {
    std::cout << "Probably not the right file, trying with another input format." << std::endl;
    EfficiencyPlotMarley r(InputFile);
    r.Run();
  }
  return 0;
}
