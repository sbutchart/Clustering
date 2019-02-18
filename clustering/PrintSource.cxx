#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <limits.h>

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

std::vector<std::string> GenTypes = {"Other"        ,
				     "SNnu"         ,          
				     "APA"          ,
				     "CPA"          ,
				     "Ar39"         ,
				     "Neutron"      ,
				     "Krypton"      ,
				     "Polonium"     ,
				     "Radon"        ,
				     "Ar42"         ,
				     "AllBackground",
				     "All"};

/*
|  Definition of a global variable of type std::vector<std::string>
|  to contain all of the names of the GenTypes
*/

void StackPlot(std::map<int, TH1D*> map, TLegend* leg, std::string xlabel) {
  int LengthOfMap = map.size();

  std::vector<int> colors = {2, 1, 3, 4, 5, 6, 7, 8, 9, 11, 30};
  
  map[1]->Draw();
  map[1]->SetLineColor(1);
  map[1]->GetYaxis()->SetRangeUser(10e-2, 10e7);
  map[1]->GetXaxis()->SetTitle(xlabel.c_str());
  map[1]->SetTitle("");
  map[1]->SetStats(0);
  
  leg->AddEntry(map[1], (GenTypes[1]).c_str());
  
  for (auto i=0; i<LengthOfMap; ++i) {
    if (i == 1) continue;
    map[i]->Draw("SAME");
    map[i]->SetLineColor(colors[i]);
    leg->AddEntry(map[i], (GenTypes[i]).c_str());
  }
}

/*
|  StackPlot is a small function that takes in a map of TH1Ds and a TLegend
|  object and creates an output of all of the the histograms in the map stacked
|  on top of one another. It also fills out the legend so you can see which
|  line is which.
*/

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

int main(int argc, char** argv) {

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

  Helper h;
  std::map<int, TH1D*> hitSADC    ;
  std::map<int, TH1D*> hitRMS     ;
  std::map<int, TH1D*> clustTime  ;
  std::map<int, TH1D*> clustChan  ;
  std::map<int, TH2D*> SADCvsTime ;

  TH1D *capturevsenergy   = new TH1D("CaptureVsEnergy"  , "", 100, 0, 5);
  TH1D *neutronstarttimes = new TH1D("NeutronStartTimes", "", 100, 0, 2e3);
 

  int fNEvent = nEvent;

/*
|  Initialising and loading relevant storage variables, maps and modules
*/

  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  clusteng.SetTimeWindow   (20 );
  clusteng.SetChannelWidth (2  );
  clusteng.SetTimeWindowOpt(0.2);
  clusteng.SetPositionOpt  (300);
  clusteng.SetBucketSize   (1  );

  wiretrigger.SetWireNHitsMin    (6);
  wiretrigger.SetWireNChanMin  	 (3);
  wiretrigger.SetWireChanWidthMin(0);
  wiretrigger.SetWireSADCMin     (0);

/*
|  Declaring the clustering algorithm (cluster engine) that is used here
|  as well as the trigger algorithm (wire trigger) that is used. Then setting
|  the parameters required by each of those algorithms.
*/

  int NumberOfNeutrons = 0;
  int NumberOfCaptures = 0;

  std::vector<double> NeutronCaptureLocationX;
  std::vector<double> NeutronCaptureLocationY;
  std::vector<double> NeutronCaptureLocationZ;
  
  for (auto const& it: aaim) {
      
    SNAnaInputManager* im = it.second;

    if(fNEvent != -1) {fNEvent = std::min(fNEvent, (int)im->GetEntries());}
    else              {fNEvent = im->GetEntries();                        }

    for (int i=0; i<11; i++) {
      hitSADC     [i] = new TH1D( ("hitSADC_" + h.ShortGenName[ConvertIntToGenType(i)]).c_str(),
				  ("hitSADC_" + h.ShortGenName[ConvertIntToGenType(i)]).c_str(),
				  100, 0, 500 );
      hitRMS      [i] = new TH1D( ("hitRMS_" + h.ShortGenName[ConvertIntToGenType(i)]).c_str(),
				  ("hitRMS_" + h.ShortGenName[ConvertIntToGenType(i)]).c_str(),
				  100, 0, 50 );
      clustTime   [i] = new TH1D( ("clustTime_" + h.ShortGenName[ConvertIntToGenType(i)]).c_str(),
				  ("clustTime_" + h.ShortGenName[ConvertIntToGenType(i)]).c_str(),
				  100, 0, 200 );
      clustChan   [i] = new TH1D( ("clustChan_" + h.ShortGenName[ConvertIntToGenType(i)]).c_str(),
				  ("clustChan_" + h.ShortGenName[ConvertIntToGenType(i)]).c_str(),
				  25, 0, 50 );
      SADCvsTime  [i] = new TH2D( ("SADCvsTime_" + h.ShortGenName[ConvertIntToGenType(i)]).c_str(),
				  ("SADCvsTime_" + h.ShortGenName[ConvertIntToGenType(i)]).c_str(),
				  100, 0, 500, 100, 0, 10 );
    }

/*
|  This starts the loop through all of the input manager objects and declares an
|  object, im, that is the usable input manager object. The usage is im->Variable
|  which gives access to whatever variable you've chosen.
|
|  Here also sets up how many events in the TTree to loop over and initialises any
|  plots that you have defined previously.
*/
   
    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {

      float prog = static_cast<float>(CurrentEvent)/static_cast<float>(fNEvent);
      progress(prog);
	
      im->GetEntry(CurrentEvent);

      NumberOfNeutrons += im->TotGen_Neut;

      // for (size_t i=0; i<im->True_Bck_Time->size(); ++i) {
      // 	if ((*im->True_Bck_PDG)[i] == 2112)
      // 	std::cout << (*im->True_Bck_Time)[i] << "\t" << (*im->True_Bck_EndT)[i] << "\t" << (*im->True_Bck_EndT)[i] - (*im->True_Bck_Time)[i] << std::endl;
      // }
      
      // double StartTime = std::numeric_limits<double>::max();
      // double EndTime   = std::numeric_limits<double>::min();

      for (size_t it=0; it<(*im->True_Bck_Mode).size(); ++it) {
	if ((*im->True_Bck_Mode)[it] == 5) {
	  if ((*im->True_Bck_PDG)[it] == 2112) {	    
	    if ((*im->True_Bck_EndProcess)[it] == 10) {
	      if (((*im->True_Bck_EndX)[it] > -360) && ((*im->True_Bck_EndX)[it] < 360)) {
		if (((*im->True_Bck_EndY)[it] > -600) && ((*im->True_Bck_EndY)[it] < 600)) {
		  if (((*im->True_Bck_EndZ)[it] > 0)    && ((*im->True_Bck_EndZ)[it] < 1390)) {
		    NumberOfCaptures++;
		    capturevsenergy->Fill(((*im->True_Bck_Energy)[it]*1000) - 938);
		    neutronstarttimes->Fill(((*im->True_Bck_EndT)[it] - (*im->True_Bck_Time)[it])/1000);
		    NeutronCaptureLocationX.push_back((*im->True_Bck_EndX)[it]);
		    NeutronCaptureLocationY.push_back((*im->True_Bck_EndY)[it]);
		    NeutronCaptureLocationZ.push_back((*im->True_Bck_EndZ)[it]);
		  } 
		}
	      }
	    }
	  }
	}
      }

/*
|  Calculation of neutron statistics, including:
|    - Neutron capture rate
|    - Neutron capture probability
|    - Time of flight for the neutron
*/

      std::vector<WireHit*> vec_WireHit;
      for (size_t j=0; j<im->Hit_View->size(); ++j) {
	WireHit* hit = new WireHit((*im->Hit_View         )[j],
				   (*im->Hit_True_GenType )[j],
				   (*im->Hit_Chan         )[j],
				   (*im->Hit_Time         )[j],
				   (*im->Hit_SADC         )[j],
				   (*im->Hit_RMS          )[j],
				   (*im->Hit_True_Energy  )[j],
				   (*im->Hit_True_EvEnergy)[j],
				   (*im->Hit_True_MainTrID)[j],
				   0.5, 0.5, 0.5,
				   (*im->Hit_True_X       )[j],
				   (*im->Hit_True_Y       )[j],
				   (*im->Hit_True_Z       )[j],
				   0,
				   (*im->Hit_True_nElec   )[j]);
	vec_WireHit.push_back(hit);
      }

/*
|  Firstly, the event loop within a TTree is started. The counter used to 
|  identify a singular event is 'CurrentEvent'. A variable 'prog' is declared
|  and then used in the progress function so that a percentage progress
|  can be seen on the terminal output while running.
|
|  A vector of objects of type WireHit it declared and then filled with
|  WireHit objects.
*/
      
      std::vector<WireCluster*> vec_WireCluster;
      clusteng.ClusterHits2(vec_WireHit, vec_WireCluster);

      for (auto const& hit: vec_WireHit) {
	hitSADC[hit->GetGenType()]->Fill(hit->GetHitSADC());
	hitRMS [hit->GetGenType()]->Fill(hit->GetHitRMS ());
	if (hit->GetGenType() != 1) {
	  hitSADC[10]->Fill(hit->GetHitSADC());
	  hitRMS [10]->Fill(hit->GetHitRMS ());
	  SADCvsTime[1]->Fill(hit->GetHitSADC(), hit->GetHitRMS());
	} else {
	  SADCvsTime[0]->Fill(hit->GetHitSADC(), hit->GetHitRMS());
	}
      }

/*
|  This loop iterates over every wire hit in the vector of WireHit objects
|  and grabs the relevant information about a particular variable. Then
|  that information is used to fill a plot in the map of plots for that
|  particular variable.
*/
      
      for (auto const& clust: vec_WireCluster) {
	std::vector<float> hitTimes = clust->GetPartTimes   ();
	std::vector<int>   hitChans = clust->GetPartChannels();
	std::vector<int>   hitTypes = clust->GetPartGenTypes();

	double deltaT = hitTimes[hitTimes.size()-1] - hitTimes[0];
	double deltaC = hitChans[hitChans.size()-1] - hitChans[0];

	clustTime[hitTypes[0]]->Fill(deltaT);
	clustChan[hitTypes[0]]->Fill(deltaC);
	if (hitTypes[0] != 1) {
	  clustTime[10]->Fill(deltaT);
	  clustChan[10]->Fill(deltaC);
	}
      }
      
/*
|  This loop iterates over every wire cluster in the vector of WireCluster 
|  objects and grabs the relevant information about a particular variable. Then
|  that information is used to fill a plot in the map of plots for that
|  particular variable.
*/
      
    for (auto& it: vec_WireHit    ) {delete it; it=NULL;}
    for (auto& it: vec_WireCluster) {delete it; it=NULL;}
    vec_WireHit    .clear();
    vec_WireCluster.clear();
    }
  }

/*
|  This loops removes all of the elements from the vector of WireHit objects
|  and then clears the vector from memory. This is important for memory
|  management. This is also repeated for the WireCluster objects
*/
  
  std::vector<std::string> GenTypes = {"Other"        ,
				       "SNnu"         ,          
				       "APA"          ,
				       "CPA"          ,
				       "Ar39"         ,
				       "Neutron"      ,
				       "Krypton"      ,
				       "Polonium"     ,
				       "Radon"        ,
				       "Ar42"         ,
				       "AllBackground",
				       "All"};
  

  // double SADCznx[(int)hitSADC  [1]->GetNbinsX()];
  // double RMSznx [(int)hitRMS   [1]->GetNbinsX()];
  // double Timeznx[(int)clustTime[1]->GetNbinsX()];
  // double Chanznx[(int)clustChan[1]->GetNbinsX()];

  // double SADCzny[(int)hitSADC  [1]->GetNbinsX()];
  // double RMSzny [(int)hitRMS   [1]->GetNbinsX()];
  // double Timezny[(int)clustTime[1]->GetNbinsX()];
  // double Chanzny[(int)clustChan[1]->GetNbinsX()];
    
  // for (auto it=0; it<hitSADC[1]->GetNbinsX(); ++it) {

  //   if (hitSADC[1]->GetBinContent(it) != 0) {
  //   SADCzny[it] = hitSADC[1]->GetBinContent(it)/
  //     (TMath::Sqrt(hitSADC[10]->GetBinContent(it)) +
  //      hitSADC[1]->GetBinContent(it));
  //   } else {
  //     SADCzny[it] = 0;
  //   }
  //   SADCznx[it] = it*5;

  // }

  // for (auto it=0; it<hitRMS[1]->GetNbinsX(); ++it) {

  //   if (hitRMS[1]->GetBinContent(it) != 0) {
  //   RMSzny[it] = hitRMS[1]->GetBinContent(it)/
  //     (TMath::Sqrt(hitRMS[10]->GetBinContent(it)) +
  //      hitRMS[1]->GetBinContent(it));
  //   } else {
  //     RMSzny[it] = 0;
  //   }
  //   RMSznx[it] = it/2;

  // }

  // for (auto it=0; it<clustTime[1]->GetNbinsX(); ++it) {

  //   if (clustTime[1]->GetBinContent(it) != 0) {
  //   Timezny[it] = clustTime[1]->GetBinContent(it)/
  //     (TMath::Sqrt(clustTime[10]->GetBinContent(it)) +
  //      clustTime[1]->GetBinContent(it));
  //   } else {
  //     Timezny[it] = 0;
  //   }
  //   Timeznx[it] = it*2;

  // }

  // for (auto it=0; it<clustChan[1]->GetNbinsX(); ++it) {

  //   if (clustChan[1]->GetBinContent(it) != 0) {
  //   Chanzny[it] = clustChan[1]->GetBinContent(it)/
  //     (TMath::Sqrt(clustChan[10]->GetBinContent(it)) +
  //      clustChan[1]->GetBinContent(it));
  //   } else {
  //     Chanzny[it] = 0;
  //   }
  //   Chanznx[it] = it*2;

  // }

/*
|  Calculating the significance of the SNnu signal with respect to the total
|  background signal. In this case I am using the formula
|
|                  Signal
|   Zn =  _________________________
|         sqrt(Background) + Signal
|
*/
  
  TLegend *legend = new TLegend(0.1, 0.1, 0.9, 0.9);
  
  TCanvas c;
  c.Print("HitSADC.pdf[");
  c.SetLogy();
  StackPlot(hitSADC, legend, "hit SADC");
  c.Print("HitSADC.pdf");

  c.Clear();
  legend->Draw();
  c.Print("HitSADC.pdf");

  c.Clear();
  StackPlot(hitRMS, legend, "hit RMS");
  c.Print("HitSADC.pdf");

  c.Clear();
  StackPlot(clustTime, legend, "cluster #DeltaT");
  c.Print("HitSADC.pdf");

  c.Clear();
  StackPlot(clustChan, legend, "cluster #DeltaChannel");
  c.Print("HitSADC.pdf");

  c.SetLogy(0);  
  c.Clear();
  SADCvsTime[0]->Draw("COLZ");
  SADCvsTime[0]->GetXaxis()->SetTitle("hit SADC");
  SADCvsTime[0]->GetYaxis()->SetTitle("hit RMS" );
  c.Print("HitSADC.pdf");

  c.Clear();
  SADCvsTime[1]->Draw("COLZ");
  SADCvsTime[1]->GetXaxis()->SetTitle("hit SADC");
  SADCvsTime[1]->GetYaxis()->SetTitle("hit RMS" );
  c.Print("HitSADC.pdf");

/*  
|  Canvas 2:
|
|  This is going to be a secondary output for this script that lays out
|  the X-Y and X-Z veiws of where the neutron capture occurred. This is
|  for the purpose of determining a potential fiducial volume cut on the
|  system.
|  There is also some information on the time of flight for the neutron
|  before it captures. 
*/
  
}
