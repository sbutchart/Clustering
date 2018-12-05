#include "RadiologicalCheatGenerator.hh"

#include <unistd.h>

int main(int argc, char** argv){

  int opt;
  extern char *optarg;
  extern int  optopt;

  std::string OutputFile="";
  std::string Generator="";
  int nEvent=1000;
  
  while ( (opt = getopt(argc, argv, "o:g:n:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'o':
      OutputFile = optarg;
      break;
    case 'g':
      Generator = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }
  
  if (OutputFile == "") {
    std::cout << "Need to provide an output file with -o" << std::endl;
    exit(1);
  }

  if (Generator == "") {
    std::cout << "Need to provide a background file with -g:" << std::endl;
    std::cout << "-g Ar39" << std::endl;
    std::cout << "-g Ar42" << std::endl;
    std::cout << "-g " << std::endl;
    std::cout << "-g " << std::endl;
    std::cout << "-g " << std::endl;
    exit(1);
  }

  RadiologicalCheatGenerator rcg;
  rcg.SetNEvent(nEvent);
  rcg.SetOutputFile(OutputFile);
  rcg.SetBackgroundType(Generator);
  return rcg.Run();
}
 
