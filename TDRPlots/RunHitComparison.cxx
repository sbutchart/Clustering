#include "HitComparison.hh"

int main(int argc, char** argv){

  int opt;
  extern char *optarg;
  extern int  optopt;

  std::string InputFile="";

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
    std::cout << "Need to provide an input file with -i" << std::endl;
    exit(1);
  }
  try{
    HitComparison e(InputFile);
    e.Run();
  }catch(...) {
    std::cout << "Probably not the right file." << std::endl;
  }
  return 0;
}
