#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <unistd.h>

#include "TString.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TMath.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TLine.h"
#include "TRandom3.h"
#include "TStopwatch.h"

using namespace std;
int Config=3;
// fTrueECoV         mean 0
// fRecoSumADC       mean 1779.51
// fRecoChannelWidth mean 4.9695
// fRecoTimeWidth    mean 24.6698
// fTrueECoV         stddev 1
// fRecoSumADC       stddev 1.93387e+06
// fRecoChannelWidth stddev 9.38534
// fRecoTimeWidth    stddev 333.32

// const int nbins1 = 24;
// double bins1[nbins+1] = {0,20,400,420,440,460,480,500,520,540,560,580,600,620,640,660,680,700,750,800,900,1000,1200,2000,5000};

const int nbins_e = 6;
double bins_e[nbins_e+1] = {0,1,2,3,4,5,50};
const int nbins = 13;
double bins[nbins+1] = {0,100,400,500,600,700,800,900,1000,1100,1200,1500,2000,5000};
const int nbins3 = 14;
double bins3[nbins3+1] = {0,200,400,600,700,800,900,1000,1100,1200,1500,2000,3000,4000,5000};
const int nbins2 = 15;
double bins2[nbins2+1] = {0,100,200,300,400,500,600,700,800,900,1000,1100,1200,1500,2000,5000};

const int nbins4 = 11;
double bins4[nbins4+1] = {0,1,2,3,4,5,6,7,8,9,10,30};

const int nbins5 = 17;
double bins5[nbins5+1] = {0,2,4,6,8,10,12,15,20,25,30,40,50,60,70,80,100,200};


int main(int argc, char** argv) {
  int opt;
  int Config, Energy=-1;
  double nNeutrino=10;
  double TimeWindow=10;
  bool useADC  = true;
  bool useTime = false;
  bool useChan = false;
  bool useEner = false;
  std::string InputFile = "";
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;
  // Retrieve the options:
  while ( (opt = getopt(argc, argv, "c:e:d:t:f:n:i:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'c':
      std::cout <<optarg<<std::endl;
      Config = atoi(optarg);
      break;
    case 'e':
      Energy = atoi(optarg);
      break;
    case 'd':
      Distance = atof(optarg);
      break;
    case 't':
      TimeWindow = atof(optarg);
      break;
    case 'i':
      InputFile = optarg;
      break;
    case 'n':
      nNeutrino = atof(optarg);
      break;
    case 'f':
      useADC  = (std::string(optarg).find("a") != std::string::npos);
      useTime = (std::string(optarg).find("t") != std::string::npos);
      useChan = (std::string(optarg).find("c") != std::string::npos);
      useEner = (std::string(optarg).find("e") != std::string::npos);
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
  std::cout << "Using file " << InputFile << std::endl;
  
  if (Config<0 || Config>5) {
    std::cout << "Invalid config " << Config << std::endl;
    exit(1);
  }
  if (Energy<-1 || Energy>3) {
    std::cout << "Invalid Energy " << Energy << std::endl;
    exit(1);
  }

  std::map<std::pair<int, int>, TH1* >  map_samplehisto;
  std::map<std::pair<int, int>, std::pair<int, double> >  map_discriminatorbins;

  if (useADC)  std::cout << "Using ADC"  << std::endl;
  if (useTime) std::cout << "Using Time width" << std::endl;
  if (useChan) std::cout << "Using Channel width" << std::endl;
  if (useEner) std::cout << "Using Energy" << std::endl;

  int nFeatures = (int)useADC + (int)useTime + (int)useChan;
  
  if(useADC && !useTime && !useChan) {
    map_samplehisto[std::make_pair(-1,0)] = new TH1D("samplehisto-1,0","samplehisto", nbins2, bins2);
    map_samplehisto[std::make_pair(-1,1)] = new TH1D("samplehisto-1,1","samplehisto", nbins, bins);
    map_samplehisto[std::make_pair(-1,2)] = new TH1D("samplehisto-1,2","samplehisto", nbins, bins);
    map_samplehisto[std::make_pair(-1,3)] = new TH1D("samplehisto-1,3","samplehisto", nbins, bins);
    map_samplehisto[std::make_pair(-1,4)] = new TH1D("samplehisto-1,4","samplehisto", nbins, bins);
    map_samplehisto[std::make_pair(-1,5)] = new TH1D("samplehisto-1,5","samplehisto", nbins3, bins3);
    map_samplehisto[std::make_pair( 0,0)] = new TH1D("samplehisto0,0", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 0,1)] = new TH1D("samplehisto0,1", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 0,2)] = new TH1D("samplehisto0,2", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 0,3)] = new TH1D("samplehisto0,3", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 0,4)] = new TH1D("samplehisto0,4", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 0,5)] = new TH1D("samplehisto0,5", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 1,0)] = new TH1D("samplehisto1,0", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 1,1)] = new TH1D("samplehisto1,1", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 1,2)] = new TH1D("samplehisto1,2", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 1,3)] = new TH1D("samplehisto1,3", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 1,4)] = new TH1D("samplehisto1,4", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 1,5)] = new TH1D("samplehisto1,5", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 2,0)] = new TH1D("samplehisto2,0", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 2,1)] = new TH1D("samplehisto2,1", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 2,2)] = new TH1D("samplehisto2,2", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 2,3)] = new TH1D("samplehisto2,3", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 2,4)] = new TH1D("samplehisto2,4", "samplehisto", nbins_e, bins_e);
    map_samplehisto[std::make_pair( 2,5)] = new TH1D("samplehisto2,5", "samplehisto", nbins_e, bins_e);
    map_discriminatorbins[std::make_pair(-1,0)] = std::make_pair(100, 0.5);
    map_discriminatorbins[std::make_pair(-1,1)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,2)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,3)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,4)] = std::make_pair(100, 0.02);
    map_discriminatorbins[std::make_pair(-1,5)] = std::make_pair(100, 0.5);
    map_discriminatorbins[std::make_pair( 0,0)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 0,1)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 0,2)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 0,3)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 0,4)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 0,5)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 1,0)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 1,1)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 1,2)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 1,3)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 1,4)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 1,5)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 2,0)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 2,1)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 2,2)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 2,3)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 2,4)] = std::make_pair(500, 2000);
    map_discriminatorbins[std::make_pair( 2,5)] = std::make_pair(500, 2000);

  } else if (!useADC && useTime && !useChan) {
    map_samplehisto[std::make_pair(-1,0)] = new TH1D("samplehisto-1,0","samplehisto", nbins5, bins5);
    map_samplehisto[std::make_pair(-1,1)] = new TH1D("samplehisto-1,1","samplehisto", nbins5, bins5);
    map_samplehisto[std::make_pair(-1,2)] = new TH1D("samplehisto-1,2","samplehisto", nbins5, bins5);
    map_samplehisto[std::make_pair(-1,3)] = new TH1D("samplehisto-1,3","samplehisto", nbins5, bins5);
    map_samplehisto[std::make_pair(-1,4)] = new TH1D("samplehisto-1,4","samplehisto", nbins5, bins5);
    map_samplehisto[std::make_pair(-1,5)] = new TH1D("samplehisto-1,5","samplehisto", nbins5, bins5);
    map_discriminatorbins[std::make_pair(-1,0)] = std::make_pair(100, 0.5);
    map_discriminatorbins[std::make_pair(-1,1)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,2)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,3)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,4)] = std::make_pair(100, 0.02);
    map_discriminatorbins[std::make_pair(-1,5)] = std::make_pair(100, 15);

  } else if (useADC && !useTime && useChan) {
    map_samplehisto[std::make_pair(-1,0)] = new TH2D("samplehisto-1,0","samplehisto", nbins4, bins4, nbins5, bins5);
    map_samplehisto[std::make_pair(-1,1)] = new TH2D("samplehisto-1,1","samplehisto", nbins4, bins4, nbins5, bins5);
    map_samplehisto[std::make_pair(-1,2)] = new TH2D("samplehisto-1,2","samplehisto", nbins4, bins4, nbins5, bins5);
    map_samplehisto[std::make_pair(-1,3)] = new TH2D("samplehisto-1,3","samplehisto", nbins4, bins4, nbins5, bins5);
    map_samplehisto[std::make_pair(-1,4)] = new TH2D("samplehisto-1,4","samplehisto", nbins4, bins4, nbins5, bins5);
    map_samplehisto[std::make_pair(-1,5)] = new TH2D("samplehisto-1,5","samplehisto", nbins4, bins4, nbins5, bins5);
    map_discriminatorbins[std::make_pair(-1,0)] = std::make_pair(100, 0.5);
    map_discriminatorbins[std::make_pair(-1,1)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,2)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,3)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,4)] = std::make_pair(100, 0.02);
    map_discriminatorbins[std::make_pair(-1,5)] = std::make_pair(100, 100);
  
  } else if (!useADC && !useTime && useChan) {
    map_samplehisto[std::make_pair(-1,0)] = new TH1D("samplehisto-1,0","samplehisto", nbins4, bins4);
    map_samplehisto[std::make_pair(-1,1)] = new TH1D("samplehisto-1,1","samplehisto", nbins4, bins4);
    map_samplehisto[std::make_pair(-1,2)] = new TH1D("samplehisto-1,2","samplehisto", nbins4, bins4);
    map_samplehisto[std::make_pair(-1,3)] = new TH1D("samplehisto-1,3","samplehisto", nbins4, bins4);
    map_samplehisto[std::make_pair(-1,4)] = new TH1D("samplehisto-1,4","samplehisto", nbins4, bins4);
    map_samplehisto[std::make_pair(-1,5)] = new TH1D("samplehisto-1,5","samplehisto", nbins4, bins4);
    map_discriminatorbins[std::make_pair(-1,0)] = std::make_pair(100, 0.5);
    map_discriminatorbins[std::make_pair(-1,1)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,2)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,3)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,4)] = std::make_pair(100, 0.02);
    map_discriminatorbins[std::make_pair(-1,5)] = std::make_pair(100, 100);
  
  } else if (useADC && useTime && useChan) {
    map_samplehisto[std::make_pair(-1,0)] = new TH3D("samplehisto-1,0","samplehisto", nbins2, bins2, nbins2, bins2, nbins2, bins2);
    map_samplehisto[std::make_pair(-1,1)] = new TH3D("samplehisto-1,1","samplehisto", nbins,  bins , nbins,  bins , nbins,  bins );
    map_samplehisto[std::make_pair(-1,2)] = new TH3D("samplehisto-1,2","samplehisto", nbins,  bins , nbins,  bins , nbins,  bins );
    map_samplehisto[std::make_pair(-1,3)] = new TH3D("samplehisto-1,3","samplehisto", nbins,  bins , nbins,  bins , nbins,  bins );
    map_samplehisto[std::make_pair(-1,4)] = new TH3D("samplehisto-1,4","samplehisto", nbins,  bins , nbins,  bins , nbins,  bins );
    map_samplehisto[std::make_pair(-1,5)] = new TH3D("samplehisto-1,5","samplehisto", nbins3, bins3, nbins3, bins3, nbins3, bins3);
    map_discriminatorbins[std::make_pair(-1,0)] = std::make_pair(100, 0.5);
    map_discriminatorbins[std::make_pair(-1,1)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,2)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,3)] = std::make_pair(100, 0.05);
    map_discriminatorbins[std::make_pair(-1,4)] = std::make_pair(100, 0.02);
    map_discriminatorbins[std::make_pair(-1,5)] = std::make_pair(100, 100);
  }
  
  std::map<std::pair<int,string>, std::pair<double,double> > map_meanstddev;
  map_meanstddev[std::make_pair(3,"SumADC")]    = std::make_pair(1779.51,1.93387e+06);
  map_meanstddev[std::make_pair(3,"ChanWidth")] = std::make_pair(4.9695,9.38534);
  map_meanstddev[std::make_pair(3,"TimeWidth")] = std::make_pair(24.6698,9.3853);
  map_meanstddev[std::make_pair(3,"ETrue")]     = std::make_pair(0,1);

  std::map<int, std::vector<double> > map_parameter;
  map_parameter[3] = {0.0193905, 4.32653, 0.0110564, 0.00949334, 109.38, -0.00152533, 0.276842, 1.81574, -0.0199486, -24.8391};
  
  std::string feature_string = "SumADC";
  std::string feature = "SumADC";
  std::string feature_filename = "SumADC";

  if (useADC && useTime && useChan) {
    feature = "SumADC:TimeWidth:ChanWidth";
    feature_string = "Sum ADC; Time width; Channel width";
    feature_filename = "SADCTimeWChanW";
  }
  if (useADC && !useTime && useChan) {
    feature = "SumADC:ChanWidth";
    feature_string = "Sum ADC; Channel width";
    feature_filename = "SADCChanW";
  }
  
  if (useChan) {
    feature = "ChanWidth";
    feature_string = "Channel width";
    feature_filename = "ChanW";
  }

  if (useTime) {
    feature = "TimeWidth";
    feature_string = "Time width";
    feature_filename = "TimeW";
  }
    
  std::string SumADC;
  std::string ChannelWidth;
  std::string TimeWidth;   
  
  if (Energy>-1) {
    feature_string="\'Reconstructed energy\' [MeV]";
    SumADC       = (Form("(SumADC - %f) / %f"   , map_meanstddev[std::make_pair(Config,"SumADC")   ].first, map_meanstddev[std::make_pair(Config,"SumADC")   ].second));
    ChannelWidth = (Form("(ChanWidth - %f) / %f", map_meanstddev[std::make_pair(Config,"ChanWidth")].first, map_meanstddev[std::make_pair(Config,"ChanWidth")].second));
    TimeWidth    = (Form("(TimeWidth - %f) / %f", map_meanstddev[std::make_pair(Config,"TimeWidth")].first, map_meanstddev[std::make_pair(Config,"TimeWidth")].second));
    feature_filename = Form("Ener%i",Energy);
  }

  if (Energy==2) {
    feature = (Form("%f + (%f) * (%f + (%f * %s) + (%f * %s) + (%f * %s) + (%f * %s * %s) + (%f * %s * %s) + (%f * %s * %s) + (%f * %s * %s) + (%f * %s * %s) + (%f * %s * %s))",
                    map_meanstddev[std::make_pair(Config,"ETrue")].first, map_meanstddev[std::make_pair(Config,"ETrue")].second,
                    map_parameter[Config][0],
                    map_parameter[Config][1], SumADC.c_str(), map_parameter[Config][2], ChannelWidth.c_str(), map_parameter[Config][3], TimeWidth.c_str(),
                    map_parameter[Config][4], SumADC.c_str(), SumADC.c_str(),
                    map_parameter[Config][5], ChannelWidth.c_str(), ChannelWidth.c_str(),
                    map_parameter[Config][6], TimeWidth.c_str(), TimeWidth.c_str(),
                    map_parameter[Config][7], SumADC.c_str(), ChannelWidth.c_str(),
                    map_parameter[Config][8], ChannelWidth.c_str(), TimeWidth.c_str(),
                    map_parameter[Config][9], TimeWidth.c_str(), SumADC.c_str()));
  }

  double efficiency[6] = {0.912774, 0.877176, 0.859105, 0.810592, 0.702686, 0.581915};
  double backrate  [6] = {19.1058,  5.81351,  3.86746,  1.6603,   0.43355,  0.0985341};

  double nExpectedNeutrino = 1;

  TFile* fileIn = new TFile("", "READ");
  TTree* treeClusters = (TTree*)fileIn->Get("t_Output");

  TH1* h_Back_feat     = NULL;
  TH1* h_Sign_feat     = NULL;
  TH1* h_SignBack_feat = NULL;
  if (nFeatures == 1) {
    h_Back_feat     = (TH1D*)map_samplehisto[std::make_pair(Energy,Config)]->Clone();
    h_Sign_feat     = (TH1D*)map_samplehisto[std::make_pair(Energy,Config)]->Clone();
    h_SignBack_feat = (TH1D*)map_samplehisto[std::make_pair(Energy,Config)]->Clone();
  } else if (nFeatures == 2) {
    h_Back_feat     = (TH2D*)map_samplehisto[std::make_pair(Energy,Config)]->Clone();
    h_Sign_feat     = (TH2D*)map_samplehisto[std::make_pair(Energy,Config)]->Clone();
    h_SignBack_feat = (TH2D*)map_samplehisto[std::make_pair(Energy,Config)]->Clone();
  } else if (nFeatures == 3) {
    h_Back_feat     = (TH3D*)map_samplehisto[std::make_pair(Energy,Config)]->Clone();
    h_Sign_feat     = (TH3D*)map_samplehisto[std::make_pair(Energy,Config)]->Clone();
    h_SignBack_feat = (TH3D*)map_samplehisto[std::make_pair(Energy,Config)]->Clone();
  }

  h_Back_feat->SetName("h_Back_feat");
  h_Back_feat->SetTitle(Form("Area normalised;%s;n events",feature_string.c_str()));
  h_Back_feat->Sumw2();
  h_Sign_feat->SetName("h_Sign_feat");
  h_Sign_feat->SetTitle(Form("Area normalised;%s;n events",feature_string.c_str()));
  h_Sign_feat->Sumw2();

  treeClusters->Project("h_Back_feat", feature.c_str(), Form("Config==%i && Type==0",Config));
  treeClusters->Project("h_Sign_feat", feature.c_str(), Form("Config==%i && Type==1",Config)); 
  
  h_Back_feat->SetLineColor(kRed);
  h_Sign_feat->SetLineColor(kBlue);

  std::string FileName = Form("Output_Conf%i_Feat%s_Dist%.2f", Config, feature_filename.c_str(), Distance);
  TCanvas c;
  c.Print((FileName+".pdf[").c_str());
  gPad->SetLeftMargin(1.2*gPad->GetLeftMargin());
  gPad->SetLogy();
  h_Back_feat->SetLineWidth(2);
  h_Sign_feat->SetLineWidth(2);
  
  double nExpectedBackground = backrate[Config]*TimeWindow;
  std::cout << "Number of expected background: " << nExpectedBackground << std::endl;
  h_Back_feat->Scale(1. / h_Back_feat->Integral(""), "width");
  h_Sign_feat->Scale(1. / h_Sign_feat->Integral(""), "width");
  TLegend * leg = new TLegend(0.7,0.7,0.9,0.9);
  leg->AddEntry(h_Back_feat, "background", "L");
  leg->AddEntry(h_Sign_feat, "signal",     "L");
  h_Back_feat->SetStats(0);
  h_Sign_feat->SetStats(0);
  h_Back_feat->SetMinimum(0.00001);
  h_Back_feat->SetMaximum(1);
  h_Back_feat->Draw("HIST");
  h_Sign_feat->Draw("HIST SAME");
  gPad->RedrawAxis();
  leg->Draw();
  c.Print((FileName+".pdf").c_str());

  h_Back_feat->SetTitle(Form("50kpc SN (%i #nu) and %.2f Hz background in 10s window", (int)nExpectedNeutrino,backrate[Config]));
  h_Back_feat->GetYaxis()->SetTitle(Form("n events / %.1f %s", h_Back_feat->GetBinWidth(1), feature_string.c_str()));
  h_Back_feat->Scale(nExpectedBackground);
  h_Sign_feat->Scale(nExpectedNeutrino);
  h_Back_feat->SetMinimum(0.000001);
  h_Back_feat->SetMaximum(1);
  h_Back_feat->Draw("HIST");
  h_Sign_feat->Draw("HIST SAME");
  gPad->RedrawAxis();
  leg->Draw();
  c.Print((FileName+".pdf").c_str());
  

  h_SignBack_feat->Add(h_Sign_feat);
  h_SignBack_feat->Add(h_Back_feat);
  
  TH1* LookupTable = (TH1*)map_samplehisto[std::make_pair(Energy,Config)]->Clone();
  LookupTable->SetName("LookupTable");
  LookupTable->SetTitle(Form(";%s;Discriminator value",feature_string.c_str()));
  LookupTable->SetStats(0);
  LookupTable->SetLineWidth(2);
  if (nFeatures==1) {
    for(int i=0; i<=h_Back_feat->GetXaxis()->GetNbins(); ++i){
      double Back     = h_Back_feat->GetBinContent(i);
      double SignBack = h_Sign_feat->GetBinContent(i)+Back;
      if (Back > 0) {
        LookupTable->SetBinContent(i, 2.*(Back + SignBack * (-1 + TMath::Log(SignBack / Back))));
      }
      else if (Back <= 0 && SignBack <= 0) {
        LookupTable->SetBinContent(i, 0);
      }
      else if (Back <= 0) {
        LookupTable->SetBinContent(i, 100);
      }
    }
  LookupTable->Draw();
  } else if (nFeatures==2) {
    for(int i=0; i<=h_Back_feat->GetXaxis()->GetNbins(); ++i){
      for(int j=0; j<=h_Back_feat->GetYaxis()->GetNbins(); ++j){
        double Back     = h_Back_feat->GetBinContent(j);
        double SignBack = h_Sign_feat->GetBinContent(j)+Back;
        if (Back > 0) {
          LookupTable->SetBinContent(i, j, 2.*(Back + SignBack * (-1 + TMath::Log(SignBack / Back))));
        }
        else if (Back <= 0 && SignBack <= 0) {
          LookupTable->SetBinContent(i, j, 0);
        }
        else if (Back <= 0) {
          LookupTable->SetBinContent(i, j, 100);
        }
      }
    }
    LookupTable->Draw("COLZ");
  }
  c.Print((FileName+".pdf").c_str());
  // c.Print((FileName+".pdf]").c_str());
  // return 1;
  gPad->SetLogy(false);
  TRandom3 rand(15643);
  double MeanBack     = nExpectedBackground;
  double MeanSignBack = nExpectedBackground+nExpectedNeutrino;
  double StdDevSignBack = TMath::Sqrt(MeanSignBack);

  TH1D* h_SignBack_norm = new TH1D("h_SigBack_Norm",";N events;PDF", (int)(10*StdDevSignBack),(int)(MeanSignBack-5*StdDevSignBack),(int)(MeanSignBack+1*StdDevSignBack));
  TH1D* h_Back_norm     = new TH1D("h_Back_Norm"   ,";N events;PDF", (int)(10*StdDevSignBack),(int)(MeanSignBack-5*StdDevSignBack),(int)(MeanSignBack+1*StdDevSignBack));
  
  std::cout << "Setting up the binning to be " << map_discriminatorbins[std::make_pair(Energy,Config)].first << std::endl;
  
  TH1D* h_SignBack_disc = NULL;
  TH1D* h_Back_disc     = NULL;
  h_SignBack_disc = new TH1D("h_SignBack_disc", Form(";%s discriminator;PDF",feature_string.c_str()),
                             map_discriminatorbins[std::make_pair(Energy, Config)].first,
                             0, map_discriminatorbins[std::make_pair(Energy, Config)].second);
  h_Back_disc     = new TH1D("h_Back_disc",     Form(";%s discriminator;PDF",feature_string.c_str()),
                             map_discriminatorbins[std::make_pair(Energy, Config)].first,
                             0, map_discriminatorbins[std::make_pair(Energy, Config)].second);
  
  TH1* h_SignBack_feat_empty = (TH1*)h_Back_feat->Clone();
  TH1* h_Back_feat_empty     = (TH1*)h_Back_feat->Clone();
  h_SignBack_feat_empty->Reset();
  h_Back_feat_empty    ->Reset();

  gRandom = new TRandom3(5056);
  double LookupValueMax=0;
  gRandom->SetSeed(rand.Integer(421422));

  //TStopwatch st();
//  double init = st.RealTime();
#define NEVENT 10000000
//#pragma omp parallel for
  for (int i=0; i<NEVENT; ++i) {

    if (i%1000000==0)
      std::cout << "done: " << 100.*(double)i/NEVENT << "%" << std::endl;


    double NormBack     = gRandom->Poisson(MeanBack);
    double NormSignBack = gRandom->Poisson(MeanSignBack);
    h_Back_norm    ->Fill(NormBack);
    h_SignBack_norm->Fill(NormSignBack);


    double LookupValue=0;
    for (int ievents=0; ievents<NormBack;++ievents) {
      double random = h_Back_feat->GetRandom();
      h_Back_feat_empty->Fill(random);
      int bin = LookupTable->FindBin(random);
      LookupValue+=LookupTable->GetBinContent(bin);
    }
    h_Back_disc->Fill(LookupValue);
    if (LookupValueMax<LookupValue)
      LookupValueMax=LookupValue;
    
    LookupValue=0;
    for (int ievents=0; ievents<NormSignBack;++ievents) {
      double random = h_SignBack_feat->GetRandom();
      h_SignBack_feat_empty->Fill(random);
      int bin = LookupTable->FindBin(random);
      LookupValue+=LookupTable->GetBinContent(bin);
    }
    h_SignBack_disc->Fill(LookupValue);
    if (LookupValueMax<LookupValue)
      LookupValueMax=LookupValue;
  }
  //double end = st.RealTime();
  //std::cout << "TIMING " << end-init << std::endl;
  if(LookupValueMax>map_discriminatorbins[std::make_pair(Energy,Config)].second)
    std::cout << "The discriminator values extend more than the discriminator binning!" << std::endl;
  gPad->SetLogy();

  h_SignBack_norm->SetMaximum(h_Back_norm->GetMaximum()*1.1);
  h_Back_norm    ->SetStats(0);
  h_SignBack_norm->SetStats(0);
  h_Back_norm    ->SetLineColor(kRed);
  h_SignBack_norm->SetLineColor(kBlue);
  h_Back_norm    ->SetLineWidth(2);
  h_SignBack_norm->SetLineWidth(2);
  TLegend * leg2 = new TLegend(0.7,0.7,0.9,0.9);
  leg2->AddEntry(h_Back_norm,     "background only",   "L");
  leg2->AddEntry(h_SignBack_norm, "signal+background", "L");
  h_SignBack_norm->Draw("");
  h_Back_norm    ->Draw("SAME");
  gPad->RedrawAxis();
  leg2->Draw();
  c.Print((FileName+".pdf").c_str());
  
  h_SignBack_disc->SetMaximum(h_Back_disc->GetMaximum()*1.1);
  h_SignBack_disc->GetXaxis()->SetTitle("Discriminator");
  h_SignBack_disc->GetYaxis()->SetTitle("Events");
  h_Back_disc ->SetStats(0);
  h_SignBack_disc->SetStats(0);
  h_Back_disc ->SetLineColor(kRed);
  h_SignBack_disc->SetLineColor(kBlue);
  h_Back_disc ->SetLineWidth(2);
  h_SignBack_disc->SetLineWidth(2);
  TLegend * leg3 = new TLegend(0.7,0.7,0.9,0.9);
  leg3->AddEntry(h_Back_disc,     "background only",   "L");
  leg3->AddEntry(h_SignBack_disc, "signal+background", "L");
  h_SignBack_disc->Draw("");
  h_Back_disc    ->Draw("SAME");
  gPad->RedrawAxis();
  leg3->Draw();
  c.Print((FileName+".pdf").c_str());


  h_SignBack_feat_empty->SetTitle("ForDEBUG");
  h_SignBack_feat_empty->GetYaxis()->SetTitle("PDF");

  h_Back_feat_empty    ->SetStats(0);
  h_SignBack_feat_empty->SetStats(0);
  h_Back_feat_empty    ->SetLineColor(kRed);
  h_SignBack_feat_empty->SetLineColor(kBlue);
  h_Back_feat_empty    ->SetLineWidth(2);
  h_SignBack_feat_empty->SetLineWidth(2);

  h_Back_feat    ->SetLineStyle(2);
  h_SignBack_feat->SetLineStyle(2);

  h_SignBack_feat      ->SetLineColor(kBlue);
  h_SignBack_feat      ->SetLineWidth(2);
  TLegend * leg4 = new TLegend(0.6,0.6,0.9,0.9);
  h_Back_feat_empty->SetTitle("DEBUG");
  h_Back_feat_empty    ->Scale(1. / h_Back_feat_empty    ->Integral(""), "width");
  h_SignBack_feat_empty->Scale(1. / h_SignBack_feat_empty->Integral(""), "width");
  h_Back_feat          ->Scale(1. / h_Back_feat          ->Integral(""), "width");
  h_SignBack_feat      ->Scale(1. / h_SignBack_feat      ->Integral(""), "width");
  leg4->AddEntry(h_Back_feat,           "background only (Original)",   "L");
  leg4->AddEntry(h_SignBack_feat,       "signal+background (Original)", "L");
  leg4->AddEntry(h_Back_feat_empty,     "background only (Thrown)",     "L");
  leg4->AddEntry(h_SignBack_feat_empty, "signal+background (Thrown)",   "L");
  h_Back_feat          ->Draw("HIST");
  h_SignBack_feat      ->Draw("HIST SAME");
  h_Back_feat_empty    ->Draw("SAME");
  h_SignBack_feat_empty->Draw("SAME");
  gPad->RedrawAxis();
  leg4->Draw();
  c.Print((FileName+".pdf").c_str());

  TGraph* gr_ROC_Count = new TGraph(h_SignBack_norm->GetXaxis()->GetNbins());
  TGraph* gr_ROC_Disc  = new TGraph(h_SignBack_disc->GetXaxis()->GetNbins());

  gr_ROC_Count->SetName("gr_ROC_Count");
  gr_ROC_Disc ->SetName("gr_ROC_Disc");
  gr_ROC_Count->SetTitle(";Efficiency;Background rate [Hz]");
  h_SignBack_disc->Scale(1. / h_SignBack_disc->Integral());
  h_Back_disc    ->Scale(1. / h_Back_disc    ->Integral());
  h_SignBack_norm->Scale(1. / h_SignBack_norm->Integral());
  h_Back_norm    ->Scale(1. / h_Back_norm    ->Integral());

  for(int i=0; i<h_SignBack_norm->GetXaxis()->GetNbins(); ++i) {
    double Efficiency = h_SignBack_norm->Integral(i,h_SignBack_norm->GetXaxis()->GetNbins());
    double BackRate   = backrate[Config]*h_Back_norm->Integral(i,h_Back_norm->GetXaxis()->GetNbins());
    gr_ROC_Count->SetPoint(h_SignBack_norm->GetXaxis()->GetNbins()-i,Efficiency,BackRate);
  }
  for(int i=0; i<h_SignBack_disc->GetXaxis()->GetNbins(); ++i) {
    double Efficiency = h_SignBack_disc->Integral(i,h_SignBack_disc->GetXaxis()->GetNbins());
    double BackRate   = backrate[Config]*h_Back_disc->Integral(i,h_Back_disc->GetXaxis()->GetNbins());
    gr_ROC_Disc->SetPoint(h_SignBack_disc->GetXaxis()->GetNbins()-i,Efficiency,BackRate);
  }
  
  gr_ROC_Count->SetMarkerStyle(2);
  gr_ROC_Disc ->SetMarkerStyle(2);
  gr_ROC_Count->SetMarkerColor(kBlack);
  gr_ROC_Disc ->SetMarkerColor(kBlue);
  gr_ROC_Count->SetLineColor(kBlack);
  gr_ROC_Disc ->SetLineColor(kBlue);
  gPad->SetLogy(true);
  gPad->SetLogx(false);
  gr_ROC_Count->SetMinimum(1e-9);
  gr_ROC_Count->Draw("ALP");
  gr_ROC_Disc ->Draw("LP");
  TLine* line = new TLine(0,2e-7,1,2e-7);
  line->SetLineColor(kRed);
  line->SetLineWidth(2);
  line->Draw();
  TLegend * leg5 = new TLegend(0.12,0.7,0.4,0.9);
  leg5->AddEntry(gr_ROC_Count, "Counting method", "PL");
  leg5->AddEntry(gr_ROC_Disc,  Form("%s weighting method",feature_string.c_str()), "PL");
  leg5->Draw();
  c.Print((FileName+".pdf").c_str());
  c.Print((FileName+".pdf]").c_str());

  TFile* f_out = new TFile((FileName+".root").c_str(), "RECREATE");
  h_Back_feat    ->Write();
  h_Sign_feat    ->Write();
  h_SignBack_feat->Write();
  LookupTable    ->Write();
  
  h_Back_norm    ->Write();
  h_SignBack_norm->Write();

  h_SignBack_disc->Write();
  h_Back_disc    ->Write();

  gr_ROC_Count->Write();
  gr_ROC_Disc ->Write();
  f_out->Close();
  return 1;
}


