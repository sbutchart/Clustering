#include "SmartTrigger.hh"


SmartTrigger::SmartTrigger(const std::vector<std::string> f):
  fLikelihood_Sign(NULL),
  fLikelihood_Back(NULL),
  fFeature(f),
  fConfig(-1){
  int dim = fFeature.size();
  double* min = new double[dim];
  double* max = new double[dim];
  int* nbin = new int[dim];
  
  for (int i=0; i<dim; ++i) {
    min [i] = 0;
    max [i] = 1;
    nbin[i] = 1;
  }
  
  std::string title = "Likelihood";
  for (auto const& it : fFeature) title += ";"+it;
  fLikelihood_Sign = new THnD("Likelihood_Sign", title.c_str(), dim, nbin, min, max);
  fLikelihood_Back = new THnD("Likelihood_Back", title.c_str(), dim, nbin, min, max);

  delete [] min;  min  = NULL; 
  delete [] max;  max  = NULL; 
  delete [] nbin; nbin = NULL;
};


void SmartTrigger::SetBinning(const std::string feat,
                              const std::vector<double> binning){

  if (std::find(fFeature.begin(), fFeature.end(), feat) == fFeature.end()) {
    std::cout << "Error, this feature is not included in the current definition of the smart trigger" << std::endl;
    exit(1);
  }

  int i = 0;
  for (auto const& it : fFeature) {
    if(feat == it) break;
    i++;
  }
  int nfeature = i;
  
  int nbin = binning.size()-1;
  double* bin = new double[nbin+1];
  
  i = 0;
  for (auto const& it : binning) bin[i++] = it;

  fLikelihood_Sign->GetAxis(nfeature)->Set(nbin,bin);
  fLikelihood_Back->GetAxis(nfeature)->Set(nbin,bin);
  
};


void SmartTrigger::ConstructLikelihood(const std::string InputFile,
                                       const std::string OutputFile){

  if (fConfig == -1) {
    std::cout << "Configuration wasn't set" << std::endl;
    exit(0);
  }

  TFile* InFile = new TFile(InputFile.c_str(), "READ");
  TTree* Tree = (TTree*)InFile->Get("ClusteredWireHit");

  
  //std::string feat = "";
  // if (fFeature.size() == 1) {
  //   feat = fFeature[0];
  //   Tree->Project(fLikelihood_Sign->GetName(), feat.c_str(), Form("Type==1 && Config == %i",fConfig));
  //   Tree->Project(fLikelihood_Back->GetName(), feat.c_str(), Form("Type==0 && Config == %i",fConfig));
    
  // } else if (fFeature.size() == 2) {
  //   feat = fFeature[0]+":"+fFeature[1];
  //   Tree->Project(fLikelihood_Sign->GetName(), feat.c_str(), Form("Type==1 && Config == %i",fConfig));
  //   Tree->Project(fLikelihood_Back->GetName(), feat.c_str(), Form("Type==0 && Config == %i",fConfig));

  // } else if (fFeature.size() == 3) {
  //   feat = fFeature[0]+":"+fFeature[1]+":"+fFeature[2];
  //   Tree->Project(fLikelihood_Sign->GetName(), feat.c_str(), Form("Type==1 && Config==%i",fConfig));
  //   Tree->Project(fLikelihood_Back->GetName(), feat.c_str(), Form("Type==0 && Config==%i",fConfig));

  // } else {
    float* feature = new float[fFeature.size()];
    int i=0;
    for (auto const& it : fFeature)
      Tree->SetBranchAddress(it.c_str(), &feature[i++]);
    int gentype = 0;
    int config = 0;
    Tree->SetBranchAddress("Type",   &gentype);
    Tree->SetBranchAddress("Config", &config);
      
    for (int i=0; i<Tree->GetEntries(); ++i){
      Tree->GetEntry(i);
      if (config != fConfig) continue;
      double* feature_d = new double[fFeature.size()];
      for (int f=0; f<fFeature.size(); ++f) {
        feature_d[f] = feature[f];
        std::cout << feature[f] << std::endl;
      }
      if (gentype == 1) {
        fLikelihood_Sign->Fill(feature_d);
      } else {
        fLikelihood_Back->Fill(feature_d);
      }
    }
//}
  InFile ->Close();

  TFile* OutFile = new TFile(OutputFile.c_str(), "RECREATE");
  fLikelihood_Sign->Write();
  fLikelihood_Back->Write();
  OutFile->Close();

};



