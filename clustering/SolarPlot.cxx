#include "InputManager.hh"
#include "ClusterEngine.hh"
#include "Trigger.hh"

#include "Helper.h"

#include "TCanvas.h"
#include "TEfficiency.h"
#include "TFile.h"
#include "TPad.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TTree.h"
#include "TMultiGraph.h"
#include "TLine.h"
#include <iostream>
#include <unistd.h>

int main (int argc, char** argv) {
  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  std::string InFileName ="";
  std::string OutFileName="";
  int nEvent=-1;
  int nHitsMinValue=-1;
  while ( (opt = getopt(argc, argv, "i:o:n:h:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'i':
      InFileName = optarg;
      break;
    case 'o':
      OutFileName = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case 'h':
      nHitsMinValue = atoi(optarg);
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  std::map<int,SNAnaInputManager*> aaim;

  TFile f("/dune/data/users/aborkum/WeightFile.root", "READ");
  TH1D *solarWeight = (TH1D*) f.Get("SolarNu_weight");

  aaim[10] = new SNAnaInputManager();
  aaim[10] -> SetInputTree("snanagaushit/SNSimTree");

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }

  TH1D* th1d_signalEnergy;
  TH1D* th1d_backgroundEnergy;
  TH1D* th1d_BackgroundFatElectron;
  TH1D* th1d_SignalFatElectron;
  TH1D* th1d_ElectronEnergies;
  TH1D* th1d_PhotonEnergies;
  TH1D* th1d_PositronEnergies;
  TH1D* th1d_NeutronEnergies;
  TH1D* th1d_ProtonEnergies;
  TH1D* th1d_nClusters;
  TH1D* th1d_nPartInCluster;
  TH1D* th1d_SignalElectronDiff;
  TH1D* th1d_background_truth;

  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  std::vector<double> Signals;
  std::vector<double> Electrons;
  std::vector<double> Differences;

  clusteng.SetTimeWindow   (20);
  clusteng.SetChannelWidth (2);
  clusteng.SetTimeWindowOpt(0.2);
  clusteng.SetPositionOpt  (300);
  clusteng.SetBucketSize   (1);
      
  wiretrigger.SetWireNHitsMin    (nHitsMinValue);
  wiretrigger.SetWireNChanMin    (2);
  wiretrigger.SetWireChanWidthMin(0);
  wiretrigger.SetWireSADCMin     (0);
  int fNEvent = nEvent;
  int numberOfEvents = 0;
  for (auto const& it: aaim) {

    int adc_threshold = it.first;
    SNAnaInputManager* im = it.second;
    
    if (fNEvent!=-1) {
      fNEvent = std::min(fNEvent,(int)im->GetEntries());
    } else {
      fNEvent = im->GetEntries();
    }


    
    th1d_signalEnergy          = new TH1D(Form("SignalClusterEnergies_Th%i"     , adc_threshold),
					  "Solar;Cluster Energy [MeV];Rate [Solar Event/100-kton year]", 
					  25, 0, 10);

    th1d_backgroundEnergy      = new TH1D(Form("BackgroundClusterEnergies_Th%i" , adc_threshold),
					  "Solar;Cluster Energy [MeV];Rate [Solar Event/100-kton year]", 
					  25, 0, 10);

    th1d_BackgroundFatElectron = new TH1D(Form("BackgroundLargestElectron_Th%i" , adc_threshold),
					  "Solar;Cluster Energy [MeV];Rate [Solar Event/100-kton year]", 
					  25, 0, 10);

    th1d_SignalFatElectron     = new TH1D(Form("SignalLargestElectron_Th%i"     , adc_threshold),
					  "Solar;Cluster Energy [MeV];Rate [Solar Event/100-kton year]", 
					  25, 0, 10);

    th1d_SignalElectronDiff    = new TH1D(Form("DeltaSignalElectron_TH%i"       , adc_threshold),
    					  "Solar;Cluster Energy [MeV];",
    					  100, 0, 10);

    th1d_ElectronEnergies      = new TH1D(Form("ElectronEnergies_Th%i"          , adc_threshold ),
					  "Solar, E_{e^{-}};Energy [MeV];"            , 50, 0, 50);

    th1d_PhotonEnergies        = new TH1D(Form("PhotonEnergies_Th%i"            , adc_threshold ),
					  "Solar, E_{#gamma};Energy [MeV];"           , 50, 0, 50);

    th1d_PositronEnergies      = new TH1D(Form("PositronEnergies_Th%i"          , adc_threshold ),
					  "Solar, E_{e^{+}};Energy [MeV];"            , 50, 0, 50);

    th1d_NeutronEnergies       = new TH1D(Form("NeutronEnergies_Th%i"           , adc_threshold ),
					  "Solar, E_{n};Energy [MeV];"                , 50, 0, 50);
    
    th1d_ProtonEnergies        = new TH1D(Form("ProtonEnergies_Th%i"            , adc_threshold ),
					  "Solar, E_{p};Energy [MeV];"                , 50, 0, 50);

    th1d_nPartInCluster        = new TH1D(Form("nPartInCluster_Th%i"             , adc_threshold ),
					  "Solar;nParticles [#];"           , 10, 0, 10);

    th1d_nClusters             = new TH1D(Form("nCluster_Th%i"             , adc_threshold ),
					  "Solar;nClusters [#];"            , 10, 0, 10);

    th1d_background_truth      = new TH1D("", "", 25, 0, 10);
    
    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);
      std::vector<WireHit*> vec_WireHit;
      for (size_t j=0; j<im->Hit_View->size(); ++j) {
        WireHit* hit = new WireHit((*im->Hit_View)[j],        (*im->Hit_True_GenType)[j],  (*im->Hit_Chan)[j],
                                   (*im->Hit_Time)[j],        (*im->Hit_SADC)[j],          (*im->Hit_RMS)[j],
                                   (*im->Hit_True_Energy)[j], (*im->Hit_True_EvEnergy)[j], (*im->Hit_True_MainTrID)[j],
                                   0.5, 0.5, 0.5,
                                   (*im->Hit_True_X)[j],      (*im->Hit_True_Y)[j],        (*im->Hit_True_Z)[j],
                                   0, (*im->Hit_True_nElec)[j]);

        vec_WireHit.push_back(hit);
      }

      std::map<int, int>    IDsToPDGs;
      std::map<int, double> IDsToEnergy;
      std::vector<WireCluster*> vec_WireCluster;
      clusteng.ClusterHits2(vec_WireHit, vec_WireCluster);
      wiretrigger.IsTriggering(vec_WireCluster);

      for (size_t it=0; it<(*im->True_Bck_IDAll).size(); ++it) {
	IDsToPDGs[(*im->True_Bck_IDAll)[it]] = (*im->True_Bck_PDGAll)[it];
	IDsToEnergy[(*im->True_Bck_IDAll)[it]] = (*im->True_Bck_EnergyAll)[it];
      }

      th1d_nClusters->Fill(vec_WireCluster.size());

      for (size_t c=0; c<vec_WireCluster.size(); ++c) {

      	WireCluster* clust = vec_WireCluster[c];

	if (!clust->GetIsTriggering()) 
	  continue;

	double ClusterSignalEnergy     = 0;
	double ClusterBackgroundEnergy = 0;
	double FatBackgroundElectron   = 0;
	double FatSignalElectron       = 0;       	
		
      	std::set   <int>    TrIDs    = clust->GetTrackIDs();
      	std::vector<int>    GenTypes = clust->GetPartGenType();
      	std::vector<double> PartE    = clust->GetPartE();
	
        std::map<int, double> TrIDToEnergy;
	std::map<int, double> PDGToEnergy;

	th1d_nPartInCluster->Fill(TrIDs.size());
	
	PDGToEnergy[ 11 ] = 0.511;
	PDGToEnergy[-11 ] = 0.511;
	PDGToEnergy[2112] = 938.27231;
	PDGToEnergy[ 22 ] = 0;

	if (1000*(*im->True_ENu)[0] < 20) {
	  numberOfEvents ++;
	}
	int WhichBin  = solarWeight->FindBin(1000*(*im->True_ENu)[0]);
	double weight = solarWeight->GetBinContent(WhichBin);

	for (auto const& ID: TrIDs) {
	  // THIS DOES ALL OF THE SIGNAL STUFF
	  if (clust->GetType()) {

	    if ( IDsToPDGs[ID] == 11   ) { th1d_ElectronEnergies->Fill( IDsToEnergy[ID]*1000 - 0.511    , weight ); }
	    if ( IDsToPDGs[ID] == -11  ) { th1d_PositronEnergies->Fill( IDsToEnergy[ID]*1000 - 0.511    , weight ); }
	    if ( IDsToPDGs[ID] == 2112 ) { th1d_NeutronEnergies ->Fill( IDsToEnergy[ID]*1000 - 939.56563, weight ); }
	    if ( IDsToPDGs[ID] == 2212 ) { th1d_ProtonEnergies  ->Fill( IDsToEnergy[ID]*1000 - 938.27231, weight ); }
	    if ( IDsToPDGs[ID] == 22   ) { th1d_PhotonEnergies  ->Fill( IDsToEnergy[ID]*1000            , weight ); }

	    // THIS IS SUMMING ALL OF THE ENERGIES OF THE PARTICLES IN THE CLUSTER 
	    ClusterSignalEnergy += IDsToEnergy[ID]*1000 - PDGToEnergy[ IDsToPDGs[ID] ];
	    // THIS TESTS TO SEE WHAT THE ENERGY OF THE MOST ENERGETIC ELECTRON IS
	    if (abs(IDsToPDGs[ID]) == 11 && IDsToEnergy[ID]*1000 > FatSignalElectron) {
	      FatSignalElectron = IDsToEnergy[ID]*1000;
	    }
	  } 

	  // THIS DOES ALL OF THE BACKGROUND STUFF
	  else {

	    if ( IDsToPDGs[ID] == 11   ) { th1d_ElectronEnergies->Fill( IDsToEnergy[ID]*1000 - 0.511     , weight); }
	    if ( IDsToPDGs[ID] == -11  ) { th1d_PositronEnergies->Fill( IDsToEnergy[ID]*1000 - 0.511     , weight); }
	    if ( IDsToPDGs[ID] == 2112 ) { th1d_NeutronEnergies ->Fill( IDsToEnergy[ID]*1000 - 939.56563 , weight); }
	    if ( IDsToPDGs[ID] == 2212 ) { th1d_ProtonEnergies  ->Fill( IDsToEnergy[ID]*1000 - 938.27231 , weight); }
	    if ( IDsToPDGs[ID] == 22   ) { th1d_PhotonEnergies  ->Fill( IDsToEnergy[ID]*1000             , weight); }

	    // THIS IS SUMMING ALL FO THE ENERGIES OF THE PARTICLES IN THE CLUSTER
	    ClusterBackgroundEnergy += IDsToEnergy[ID]*1000 - PDGToEnergy[ IDsToPDGs[ID] ];	    
	    // THIS TESTS TO SEE WHAT THE ENERGY OF THE MOST ENERGETIC ELECTRON IS
	    if (abs(IDsToPDGs[ID]) == 11 && IDsToEnergy[ID]*1000 > FatBackgroundElectron) {
	      FatBackgroundElectron = IDsToEnergy[ID]*1000;
	    }
	  }
	}

	th1d_SignalFatElectron     -> Fill( FatSignalElectron - 0.511, weight);
	th1d_signalEnergy          -> Fill( ClusterSignalEnergy      , weight);
	th1d_BackgroundFatElectron -> Fill( FatBackgroundElectron - 0.511    );
	th1d_backgroundEnergy      -> Fill( ClusterBackgroundEnergy          );
      }
      // for (int i=0; i<(*im->True_Bck_Energy).size(); ++i) {
      // 	if (abs((*im->True_Bck_PDG)[i]) == 11) {
      // 	  th1d_background_truth->Fill((*im->True_Bck_Energy)[i]);
      // 	}
      // }
      
      for (int i=0; i<th1d_SignalFatElectron->GetNbinsX(); ++i) {
	Differences.push_back(0);
	Signals.push_back(0);
	Electrons.push_back(0);
      }
      for (int i=0; i<th1d_SignalFatElectron->GetNbinsX(); ++i) {
	Signals[i]   += th1d_signalEnergy     ->GetBinContent(i);
	Electrons[i] += th1d_SignalFatElectron->GetBinContent(i);
      }

      // GET THE MOTHER F**KING MEMORY MANAGEMENT!!!!!!!!!
      for (auto& it: vec_WireCluster) {	delete it; it = NULL; } // CLEAN UP THE VECTOR OF WIRE HITS
      for (auto& it: vec_WireHit    ) { delete it; it = NULL; } // CLEAN UP THE VECTOR OF WIRE CLUSTERS
      vec_WireCluster.clear();
      vec_WireHit.clear();
    }
  }

  for (int i=0; i<th1d_SignalFatElectron->GetNbinsX(); ++i) {
    if (Signals[i] == 0) continue;
    Differences[i] += (Signals[i] - Electrons[i])/Signals[i];
    th1d_SignalElectronDiff->SetBinContent(i, Differences[i]);
  }

  for (int i=0; i<th1d_backgroundEnergy->GetXaxis()->GetNbins(); ++i) {
    std::cout << i << " " << th1d_backgroundEnergy->GetBinContent(i) << std::endl;
  }

  // THIS IS FOR 100kton-year: 10 years (in seconds) / vol-ratio * number of events * time of LArSoft Event 
  double ScaleFactor = (10.0*365.0*24.0*60.0*60.0)/(0.12*static_cast<double>(fNEvent)*2.246e-3);

  // THIS IS FOR 10kton-day: 1 day (in seconds) / vol-ratio * number of events * time of LArSoft Event 
  // double ScaleFactor = (1.0)/(0.12*static_cast<double>(fNEvent)*2.246e-3); [Hz]
  // double ScaleFactor = (24.0*60.0*60.0)/(0.12*static_cast<double>(fNEvent)*2.246e-3);
  // double ScaleFactor = (1.0/(10.0*365.0));

  TCanvas c;
  c.Print((OutFileName+"[").c_str());
  c.SetLogy();

  TPad *upperCanvas = new TPad("upperCanvas", "upperCanvas", 0.00, 0.00, 1.00, 1.00);
  // upperCanvas->SetBottomMargin(0.00);
  upperCanvas->Draw();
  c.cd();

  // TPad *lowerCanvas = new TPad("lowerCanvas", "lowerCanvas", 0.00, 0.00, 1.00, 0.30);
  // lowerCanvas->SetTopMargin(0.00);
  // lowerCanvas->SetBottomMargin(0.40);
  // lowerCanvas->Draw();
  // lowerCanvas->cd();
  upperCanvas->cd();
  upperCanvas->SetLogy();

  th1d_backgroundEnergy->Scale(ScaleFactor);
  th1d_backgroundEnergy->SetMaximum( 100000 );
  th1d_backgroundEnergy->SetMinimum( 0.1 );
  for (int i=0; i<th1d_backgroundEnergy->GetXaxis()->GetNbins(); ++i) {
    std::cout << i << " " << th1d_backgroundEnergy->GetBinContent(i) << std::endl;
  }
  th1d_backgroundEnergy->Draw(); 
  th1d_backgroundEnergy->SetLineColor(kRed); 
  th1d_backgroundEnergy->SetStats(0);

  th1d_BackgroundFatElectron->Scale(ScaleFactor);
  th1d_BackgroundFatElectron->Draw("SAME"); 
  th1d_BackgroundFatElectron->SetLineColor(kBlack); 
  th1d_BackgroundFatElectron->SetStats(0);

  // double SignalScaleFactor = th1d_backgroundEnergy->GetBinContent(th1d_backgroundEnergy->GetMaximumBin())/th1d_signalEnergy->GetMaximum();
  double SignalScaleFactor = (365.0*10.0*1.0)/static_cast<double>(numberOfEvents);
 
  th1d_signalEnergy->Scale(SignalScaleFactor);  
  th1d_signalEnergy->Draw("SAME"); 
  th1d_signalEnergy->SetLineColor(kBlue);
  th1d_signalEnergy->SetStats(0);

  th1d_SignalFatElectron->Scale(SignalScaleFactor);
  th1d_SignalFatElectron->Draw("SAME"); 
  th1d_SignalFatElectron->SetLineColor(kGreen); 
  th1d_SignalFatElectron->SetStats(0);

  // lowerCanvas->cd();
  // TLine *line = new TLine(5, 0, 20, 0);
  // line->SetLineColor(kBlack);

  // th1d_SignalElectronDiff->SetTitle("");
  // th1d_SignalElectronDiff->SetStats(0);
  // th1d_SignalElectronDiff->SetMinimum(-1.2);
  // th1d_SignalElectronDiff->SetMaximum(1.2);
  // th1d_SignalElectronDiff->SetMarkerStyle(2);
  // th1d_SignalElectronDiff->Draw("P");
  // th1d_SignalElectronDiff->SetNdivisions(505);
  // line->Draw("SAME");

  c.Print(OutFileName.c_str());  

  c.Clear();
  TLegend *legend = new TLegend(0.1, 0.1, 0.9, 0.9);
  legend->SetFillStyle(0);
  legend->AddEntry(th1d_signalEnergy         ,      "Arbitrary Normalization Signal Cluster Energy [MeV]");
  legend->AddEntry(th1d_SignalFatElectron    ,      "E_{e, max} signal [MeV]");
  legend->AddEntry(th1d_backgroundEnergy     , Form("Background Cluster Energy [MeV] (total rate = %.1f Hz)", th1d_backgroundEnergy->Integral()));
  legend->AddEntry(th1d_BackgroundFatElectron,      "E_{e, max} background [MeV]");
  legend->Draw();
  c.Print(OutFileName.c_str());  

  th1d_ElectronEnergies->Draw();
  th1d_ElectronEnergies->SetLineColor(kBlack);
  th1d_ElectronEnergies->SetStats(0);

  th1d_PositronEnergies->Draw("SAME");
  th1d_PositronEnergies->SetLineColor(kBlue);
  th1d_PositronEnergies->SetStats(0);

  th1d_PhotonEnergies->Draw("SAME");
  th1d_PhotonEnergies->SetLineColor(kRed);
  th1d_PhotonEnergies->SetStats(0);

  th1d_NeutronEnergies->Draw("SAME");
  th1d_NeutronEnergies->SetLineColor(kCyan);
  th1d_NeutronEnergies->SetStats(0);

  th1d_ProtonEnergies->Draw("SAME");
  th1d_ProtonEnergies->SetLineColor(kGreen);
  th1d_ProtonEnergies->SetStats(0);
  c.Print(OutFileName.c_str());  

  th1d_background_truth->Scale(ScaleFactor);
  th1d_background_truth->Draw();
  th1d_background_truth->SetStats(0);
  c.Print(OutFileName.c_str());  
  
  c.Clear();
  TLegend *legend1 = new TLegend(0.1, 0.1, 0.9, 0.9);
  legend1->SetFillStyle(0);
  legend1->AddEntry(th1d_ElectronEnergies, "Electron Energies [MeV]");
  legend1->AddEntry(th1d_PositronEnergies, "Positron Energies [MeV]");
  legend1->AddEntry(th1d_PhotonEnergies,   "Photon Energies   [MeV]");
  legend1->AddEntry(th1d_NeutronEnergies,  "Neutron Energies  [MeV]");
  legend1->AddEntry(th1d_ProtonEnergies,   "Proton Energies   [MeV]");
  legend1->Draw();
  c.Print(OutFileName.c_str());  
 
  th1d_nClusters->Draw();
  th1d_nClusters->SetStats(0);
  c.Print(OutFileName.c_str());  

  th1d_nPartInCluster->Draw();
  th1d_nPartInCluster->SetStats(0);
  c.Print(OutFileName.c_str());  

  c.Print((OutFileName+"]").c_str());
}
