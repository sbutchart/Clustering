#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <limits.h>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "ArbitraryAnaInputManager.hh"
#include "Helper.h"
#include "ClusterEngine.hh"
#include "Trigger.hh"

#include "TFile.h"
#include "TTree.h"
#include "TCanvas.h"
#include "TEfficiency.h"
#include "TProfile.h"
#include "TH1D.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TMath.h"

/*
|  Definition of the extremes of the detector for the
|  1x2x6 geometry
*/

// double xlowlim = -350; double xhighlim = 350 ;
// double ylowlim = -600; double yhighlim = 600 ;
// double zlowlim = 0   ; double zhighlim = 1395;

/*
|  Definition of the extremes of the detector for the
|  full 10kt geometry
*/

/*
|  Fiducial cuts to include only the liquid argon in the active region
|  These cuts are approximate as I am too lazy to find the actual values
|  Values being used are taken from digitized plot
*/
//-350,350 y to -600,600 and z to 0,1390
const double xlowlim = -350; const double xhighlim =  350;
const double ylowlim = -600; const double yhighlim =  600;
const double zlowlim =    0; const double zhighlim = 1390;

const int nbin = 20;
// double xlowlim = -740; double xhighlim =  740;
// double ylowlim = -650; double yhighlim =  650;
// double zlowlim = -290; double zhighlim = 5850;

// double xlowlim = -700; double xhighlim = 700 ;
// double ylowlim = -600; double yhighlim = 600 ;
// double zlowlim = -200; double zhighlim = 5800;

// double xlowlim = -927; double xhighlim = 927 ;
// double ylowlim = -788; double yhighlim = 830 ;
// double zlowlim = -483; double zhighlim = 6039;

// const double xlen = xhighlim - xlowlim;
// const double ylen = yhighlim - ylowlim;
// const double zlen = zhighlim - zlowlim;

void progress(float prog) {

  int barWidth = 60;
  std::cout << "[";
  int pos = barWidth * prog;

  for (int i=0; i<barWidth; ++i) {
    if      (i < pos)           { std::cout << "="; }
    else if (pos+1 == barWidth) { std::cout << "="; }
    else if (i == pos)          { std::cout << ">"; }
    else                        { std::cout << " "; }
  }
  std::cout << "] " << (int)round(100.0 * prog) << " %\r" << std::flush;
}

/*
|  progress is a function that displays a dynamic progress bar on the terminal
|  output. This has to be called in a specific way in the main script and 
|  gives a visual representation of how the script is running.
*/

bool StaysInside(double xend, double yend, double zend) {

  bool status = false;

  if (xend < xhighlim && xend > xlowlim) {
    if (yend < yhighlim && yend > ylowlim) {
      if (zend < zhighlim && zend > zlowlim) {
 	status = true;
      }
    }
  }
  
  return status;
  
}

bool StartsInside(double xstart, double ystart, double zstart) {

  bool status = false;

  if (xstart < xhighlim && xstart > xlowlim) {
    if (ystart < yhighlim && ystart > ylowlim) {
      if (zstart < zhighlim && zstart > zlowlim) {
 	status = true;
      }
    }
  }

  return status;
  
}

int SetColour(double x, double y, double z) {

  int Colour;
  if (x < xlowlim || x > xhighlim) Colour = 10;
  if (y < ylowlim || y > yhighlim) Colour = 20;
  if (z < zlowlim || z > zhighlim) Colour = 30;

  return Colour;
  
}

int main(int argc, char** argv) {

  std::ofstream OutputFile;
  std::ofstream OutputFileGamma;
  OutputFile.open ("NeutronEnergies.txt");
  OutputFileGamma.open ("PhotonEnergies.txt");

  int opt;
  extern char *optarg;
  extern int   optopt;

  std::string InFileName = "";
  std::string TreeName   = "";

  int nEvent = -1;

  while ( (opt = getopt(argc, argv, "i:o:t:n:")) != -1 ) {
    switch ( opt ) {
    case 'i':
      InFileName   = optarg;
      break;
    case 't':
      TreeName     = optarg;
      break;
    case 'n':
      nEvent       = atoi(optarg);
      break;
    case '?':
      std::cerr << "Unknown option: " << char(optopt) << "!" << std::endl;
    }
  }

/*
|  This is an interesting switch statement that handles any command line
|  input flags. They are defines as follows:
|    -i: the address of the input file, e.g. /dune/data/users/aborkum/hist.root
|    -t: the TTree inside the root file that you want to read from
|    -n: the number of events to scan. NOTE, if not defined the program will scan all events
*/

  TH1D *NeutronEnergies          = new TH1D("Neutron_Energies", "Neutron Energies", 1100, 0, 11);
  TH1D *PhotonEnergyAfterCapture = new TH1D("Sum_Gamma_Energy", "Sum Gamma Energy", 40, 5, 9);
  TH2D *xyNeutronCaptureLocation = new TH2D("xy_Capture_Point", "xy Capture Point", nbin, xlowlim, xhighlim, nbin, ylowlim, yhighlim);
  TH2D *zyNeutronCaptureLocation = new TH2D("zy_Capture_Point", "zy Capture Point", nbin, zlowlim, zhighlim, nbin, ylowlim, yhighlim);
  
/*
|  Setting up all of the plotting objects
|  Setting up the maps to do the slices of the capture locations
*/
  
  std::map<int, SNAnaInputManager*> aaim;
  aaim[1] = new SNAnaInputManager();
  aaim[1] -> SetInputTree(TreeName.c_str());

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName.c_str());
    it.second->LoadTree();
  }

/*
|  This script uses an input manager to load all of the variables in the TTree and
|  make the script a bit shorter. This region creates a map of these input manager
|  objects and loads all of the associated TTrees. This is particularly helpful if
|  you are using multiple TTrees in one root file and you want to complre them all
|  to one another.
*/

  int fNEvent = nEvent;

/*
|  Declaring the clustering algorithm (cluster engine) that is used here
|  as well as the trigger algorithm (wire trigger) that is used. Then setting
|  the parameters required by each of those algorithms.
*/

  int NumberOfCaptures         = 0;
  int NumberOfPrimaryNeutrons  = 0;
  int NumberOfInteriorNeutrons = 0;
  bool PrintTableOfResults     = false;
  
  for (auto const& it: aaim) {
      
    SNAnaInputManager* im = it.second;

    if(fNEvent != -1) {fNEvent = std::min(fNEvent, (int)im->GetEntries());}
    else              {fNEvent = im->GetEntries();                        }

/*
|  Starting the main event loop.
|  This is where all of the interesting things will be done/calculated
*/

    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {

      float prog = static_cast<float>(CurrentEvent)/static_cast<float>(fNEvent);
      progress(prog);
      
      im->GetEntry(CurrentEvent);

      int NeutID = 0;
      for (size_t it=0; it<(*im->True_Bck_Mode).size(); ++it){

	if ((*im->True_Bck_Mode      )[it] == 5    &&
	    (*im->True_Bck_PDG       )[it] == 2112 &&
	    (*im->True_Bck_EndProcess)[it] == 10) {

	  double xend = (*im->True_Bck_EndX)[it];
	  double yend = (*im->True_Bck_EndY)[it];
	  double zend = (*im->True_Bck_EndZ)[it];
	  
	  if (StaysInside(xend, yend, zend)) { NeutID = (*im->True_Bck_ID)[it]; } 
	}
      }
      
      double SumPhotonEnergy = 0;
      for (size_t it=0; it<(*im->True_Bck_Mode).size(); ++it){
	
	if (PrintTableOfResults) {
	  std::cout << std::setw(10);
	  std::cout << (*im->True_Bck_PDG)[it]         << "\t";
	  std::cout << std::setw(10);
	  std::cout << (*im->True_Bck_Energy)[it]*1000 << "\t";
	  std::cout << std::setw(10);
	  std::cout << (*im->True_Bck_ID)[it]          << "\t";
	  std::cout << std::setw(10);
	  std::cout << (*im->True_Bck_Mother)[it]      << std::endl;
	}

	if ((*im->True_Bck_PDG   )[it] == 22 &&
	    (*im->True_Bck_Mother)[it] == NeutID) {
	  SumPhotonEnergy += (*im->True_Bck_Energy)[it] * 1000;
	}
	
	if ((*im->True_Bck_PDG)[it] == 2112) {
	  if ((*im->True_Bck_ID)[it] == 1) { NumberOfPrimaryNeutrons++; }

	  double xStart = (*im->True_Bck_EndX)[it];
	  double yStart = (*im->True_Bck_EndY)[it];
	  double zStart = (*im->True_Bck_EndZ)[it];

	  if (StaysInside(xStart, yStart, zStart)) {
	    NumberOfInteriorNeutrons++;
	  }
	  
	}

	double NeutronKE = ((*im->True_Bck_Energy)[it]*1000) - 939.56541;

	NeutronEnergies->Fill(NeutronKE);
	
	// If the neutron captures
	if ((*im->True_Bck_Mode      )[it] == 5 &&
	    (*im->True_Bck_EndProcess)[it] == 10) {

	  double xend = (*im->True_Bck_EndX)[it];
	  double yend = (*im->True_Bck_EndY)[it];
	  double zend = (*im->True_Bck_EndZ)[it];
	  
	  if (StaysInside(xend, yend, zend)) {
	    NumberOfCaptures++;

	    xyNeutronCaptureLocation->Fill(xend, yend);
	    zyNeutronCaptureLocation->Fill(zend, yend);
	  } 
	}
      }
      if (SumPhotonEnergy != 0) 
	PhotonEnergyAfterCapture->Fill(SumPhotonEnergy);
      OutputFile << std::endl;
    }
  }

  OutputFile.close();
  OutputFileGamma.close();
  
/*
|  Calculation of neutron statistics, including:
|    - Neutron capture rate
|    - Neutron capture probability
|    - Time of flight for the neutron
*/

  std::cout << std::endl;
  std::cout << "NUMBER OF EVENTS           : " << fNEvent                  << std::endl;   
  std::cout << "NUMBER OF PRIMARY NEUTRONS : " << NumberOfPrimaryNeutrons  << std::endl;
  std::cout << "NUMBER OF INTERIOR NEUTRONS: " << NumberOfInteriorNeutrons << std::endl;
  std::cout << "NUMBER OF CAPTURES         : " << NumberOfCaptures         << std::endl;
  std::cout << "CAPTURE PROBABILITY        : " << 100 * (double)NumberOfCaptures/(double)NumberOfPrimaryNeutrons
	                                       << " %" <<  std::endl;
  std::cout << "CAPTURE RATE               : " << (double)NumberOfCaptures / 2 / fNEvent / 2.246e-3
	                                       << " [Hz]" << std::endl;

  std::cout << "DETECTOR VOLUME            : " << (xhighlim - xlowlim) * (yhighlim - ylowlim) * (zhighlim - zlowlim) << std::endl;

  
/*
|  Plots, bitch
*/

  TCanvas *c1 = new TCanvas("c1", "c1");
  c1->Print("NeutronEnergySpectrum.pdf[");
  c1->SetLogy(1);
  NeutronEnergies->Draw();
  NeutronEnergies->GetXaxis()->SetTitle("Neutron Kinetic Energy [MeV]");
  NeutronEnergies->GetYaxis()->SetTitle("Rate");  
  c1->Print("NeutronEnergySpectrum.pdf");

  c1->SetLogy(1);
  PhotonEnergyAfterCapture->Draw();
  PhotonEnergyAfterCapture->GetXaxis()->SetTitle("Sum Photon Energy [MeV]");
  PhotonEnergyAfterCapture->GetYaxis()->SetTitle("");
  c1->Print("NeutronEnergySpectrum.pdf");

  c1->SetLogy(0);
  PhotonEnergyAfterCapture->Draw();
  PhotonEnergyAfterCapture->GetXaxis()->SetTitle("Sum Photon Energy [MeV]");
  PhotonEnergyAfterCapture->GetYaxis()->SetTitle("");
  c1->Print("NeutronEnergySpectrum.pdf");

  xyNeutronCaptureLocation->Draw("COLZ");
  xyNeutronCaptureLocation->GetXaxis()->SetTitle("x");
  xyNeutronCaptureLocation->GetYaxis()->SetTitle("y");
  c1->Print("NeutronEnergySpectrum.pdf");

  zyNeutronCaptureLocation->Draw("COLZ");
  zyNeutronCaptureLocation->GetXaxis()->SetTitle("z");
  zyNeutronCaptureLocation->GetZaxis()->SetTitle("y");
  c1->Print("NeutronEnergySpectrum.pdf");
  c1->Print("NeutronEnergySpectrum.pdf]");

}
  
