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

  aaim[10] = new SNAnaInputManager();
  aaim[10] -> SetInputTree("snanagaushit/SNSimTree");

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }
  // TEfficiency*> effEnergy;
  // TProfile*>    nNoiseHit;
  // TProfile*>    nSignalHit;
  // TProfile*>    nNoiseCluster;
  // TProfile*>    nSignalCluster;

  // TH2D*    th2d_nNoiseHit     ;
  // TH2D*    th2d_nSignalHit    ;
  // TH2D*    th2d_nNoiseCluster ;
  // TH2D*    th2d_nSignalCluster;

  TH1D*    th1d_signalEnergy;
  TH1D*    th1d_backgroundEnergy;
  TH1D*    th1d_BackgroundFatElectron;
  TH1D*    th1d_SignalFatElectron;
  // TH1D*    th1d_electronEnergies;
  // TH1D*    th1d_photonEnergies;
  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

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
  for (auto const& it: aaim) {

    int adc_threshold = it.first;
    SNAnaInputManager* im = it.second;
    
    if (fNEvent!=-1) {
      fNEvent = std::min(fNEvent,(int)im->GetEntries());
    } else {
      fNEvent = im->GetEntries();
    }

    th1d_signalEnergy          = new TH1D(Form("SignalClusterEnergies_Th%i"     , adc_threshold),
					  "Supernova;Cluster Energy [MeV];Rate [SN Event / 10-kton / day]", 160, 0, 80);
    th1d_backgroundEnergy      = new TH1D(Form("BackgroundClusterEnergies_Th%i" , adc_threshold),
					  "Supernova;Cluster Energy [MeV];Rate [SN Event / 10-kton / day]", 160, 0, 80);
    th1d_BackgroundFatElectron = new TH1D(Form("BackgroundLargestElectron_Th%i" , adc_threshold),
					  "Supernova;Cluster Energy [MeV];Rate [SN Event / 10-kton / day]", 160, 0, 80);
    th1d_SignalFatElectron     = new TH1D(Form("SignalLargestElectron_Th%i"     , adc_threshold),
					  "Supernova;Cluster Energy [MeV];Rate [SN Event / 10-kton / day]", 160, 0, 80);



    // th1d_electronEnergies = new TH1D(Form("ElectronEnergies_Th%i", adc_threshold),
    // 						    Form("Threshold %i ADC;Energy;Frequency",
    // 							 adc_threshold), 120, 0, 60);
    // th1d_photonEnergies   = new TH1D(Form("PhotonEnergies_Th%i", adc_threshold),
    // 						    Form("Threshold %i ADC;Energy;Frequency",
    // 							 adc_threshold), 120, 0, 60);







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
      // bool selected = false;
      int ncluster = 0;
      int nnoisecluster = 0;
      std::vector<WireCluster*> vec_WireCluster;
      clusteng.ClusterHits2(vec_WireHit, vec_WireCluster);
      wiretrigger.IsTriggering(vec_WireCluster);

      for (size_t c=0; c<vec_WireCluster.size(); ++c) {
        WireCluster* clust = vec_WireCluster[c];
        if (clust->GetIsTriggering()) {
          if (clust->GetType()) {
            // selected = true;
            ++ncluster;
          } else {
            ++nnoisecluster;
          }
        }
      }



      std::map<int, int>    IDsToPDGs;
      std::map<int, double> IDsToEnergy;
      for (size_t it=0; it<(*im->True_Bck_IDAll).size(); ++it) {
	IDsToPDGs[(*im->True_Bck_IDAll)[it]] = (*im->True_Bck_PDGAll)[it];
	IDsToEnergy[(*im->True_Bck_IDAll)[it]] = (*im->True_Bck_EnergyAll)[it];
      }

      for (size_t c=0; c<vec_WireCluster.size(); ++c) {

	double ClusterSignalEnergy     = 0;
	double ClusterBackgroundEnergy = 0;

       
      	WireCluster* clust = vec_WireCluster[c];
	
	if (!clust->GetIsTriggering()) 
	  continue;
	
	double BiggestElectron = 0;
	double SignalElectron  = 0;
      	std::set<int>       TrIDs    = clust->GetTrackIDs();
      	std::vector<double> PartE    = clust->GetPartE();
      	std::vector<int>    GenTypes = clust->GetPartGenType();

        std::map<int, double> TrIDToEnergy;
	std::map<int, double> PDGToEnergy;
	
	PDGToEnergy[11]   = 0.511;
	PDGToEnergy[-11]  = 0.511;
	PDGToEnergy[2112] = 938.27231;
	PDGToEnergy[22]   = 0;

	for (auto const& ID: TrIDs) {
	  if (clust->GetType()) {

	    if (IDsToEnergy[ID]*1000 > 1e-200) {
	      ClusterSignalEnergy += IDsToEnergy[ID]*1000 - PDGToEnergy[ IDsToPDGs[ID] ];
	      if (abs(IDsToPDGs[ID]) == 11 && IDsToEnergy[ID]*1000 > SignalElectron) {
                SignalElectron = IDsToEnergy[ID]*1000;
	      }
	    }
	  } else {
	    if (IDsToEnergy[ID]*1000 > 1e-200) {
              ClusterBackgroundEnergy += IDsToEnergy[ID]*1000 - PDGToEnergy[ IDsToPDGs[ID] ];	    
	      if (abs(IDsToPDGs[ID]) == 11 && IDsToEnergy[ID]*1000 > BiggestElectron) {
	        BiggestElectron = IDsToEnergy[ID]*1000;
	      }
	    }
	  }
	}

	if (SignalElectron != 0) {
          th1d_SignalFatElectron->Fill(SignalElectron - 0.511);

        }
	if (BiggestElectron != 0) {
          th1d_BackgroundFatElectron->Fill(BiggestElectron - 0.511);

        }
	if (ClusterSignalEnergy != 0) {
	  th1d_signalEnergy->Fill(ClusterSignalEnergy);

	}
	if (ClusterBackgroundEnergy != 0 ) {
	  th1d_backgroundEnergy->Fill(ClusterBackgroundEnergy);

	}
      }


      // effEnergy     [adc_threshold]->Fill(selected     , KE);
      // nNoiseHit     [adc_threshold]->Fill(KE, nhit[1]      );
      // nSignalHit    [adc_threshold]->Fill(KE, nhit[0]      );
      // nNoiseCluster [adc_threshold]->Fill(KE, nnoisecluster);
      // nSignalCluster[adc_threshold]->Fill(KE, ncluster     );
      // th2d_nNoiseHit     [adc_threshold]->Fill(KE, nhit[1]      );
      // th2d_nSignalHit    [adc_threshold]->Fill(KE, nhit[0]      );   
      // th2d_nNoiseCluster [adc_threshold]->Fill(KE, nnoisecluster); 
      // th2d_nSignalCluster[adc_threshold]->Fill(KE, ncluster     );

      for (auto& it: vec_WireCluster) {
	delete it; 
	it = NULL;
      }
      vec_WireCluster.clear();

      for (auto& it: vec_WireHit) {
        delete it;
        it = NULL;
      }
      vec_WireHit.clear();
    }
  }

  // const std::vector<int> color = getColors();
  // int i=0;
  // TLegend* legend = new TLegend(0.1,0.1,0.9,0.9);
  // for (auto const& it: effEnergy) {
  //   nNoiseHit     [it.first]->SetStats(0);
  //   nSignalHit    [it.first]->SetStats(0);
  //   nNoiseCluster [it.first]->SetStats(0);
  //   nSignalCluster[it.first]->SetStats(0);
  //   effEnergy     [it.first]->SetLineColor  (color.at(i));
  //   nNoiseHit     [it.first]->SetLineColor  (color.at(i));
  //   nSignalHit    [it.first]->SetLineColor  (color.at(i));
  //   nNoiseCluster [it.first]->SetLineColor  (color.at(i));
  //   nSignalCluster[it.first]->SetLineColor  (color.at(i));
  //   effEnergy     [it.first]->SetMarkerColor(color.at(i));
  //   nNoiseHit     [it.first]->SetMarkerColor(color.at(i));
  //   nSignalHit    [it.first]->SetMarkerColor(color.at(i));
  //   nNoiseCluster [it.first]->SetMarkerColor(color.at(i));
  //   nSignalCluster[it.first]->SetMarkerColor(color.at(i));
  //   th2d_nNoiseHit     [it.first]->SetStats(0);
  //   th2d_nSignalHit    [it.first]->SetStats(0);
  //   th2d_nNoiseCluster [it.first]->SetStats(0);
  //   th2d_nSignalCluster[it.first]->SetStats(0);    
  //   legend->AddEntry(nNoiseCluster[it.first], Form("ADC Threshold = %i",it.first/100), "L");
  //   i++;
  // }

  TCanvas c;
  c.Print((OutFileName+"[").c_str());

  c.SetLogy();



  th1d_backgroundEnergy->Scale((24.0*60.0*60.0)/(0.12*static_cast<double>(fNEvent)*2.246e-3));
  th1d_backgroundEnergy->SetMaximum(th1d_backgroundEnergy->GetMaximum()*100);
  th1d_backgroundEnergy->Draw(); 
  th1d_backgroundEnergy->SetLineColor(kRed); 
  th1d_backgroundEnergy->SetStats(0);

  double SignalScaleFactor = th1d_backgroundEnergy->GetBinContent(th1d_backgroundEnergy->GetMaximumBin())/th1d_signalEnergy->GetMaximum();

  th1d_signalEnergy->Scale(SignalScaleFactor);
  th1d_signalEnergy->Draw("SAME"); 
  th1d_signalEnergy->SetLineColor(kBlue);
  th1d_signalEnergy->SetStats(0);

  th1d_BackgroundFatElectron->Scale((24.0*60.0*60.0)/(0.12*fNEvent*2.246e-3));
  th1d_BackgroundFatElectron->Draw("SAME"); 
  th1d_BackgroundFatElectron->SetLineColor(kBlack); 
  th1d_BackgroundFatElectron->SetStats(0);

  th1d_SignalFatElectron->Scale(SignalScaleFactor);
  th1d_SignalFatElectron->Draw("SAME"); 
  th1d_SignalFatElectron->SetLineColor(kGreen); 
  th1d_SignalFatElectron->SetStats(0);

  c.Print(OutFileName.c_str());  

  TLegend *legend = new TLegend(0.1, 0.1, 0.9, 0.9);
  legend->SetFillStyle(0);
  legend->AddEntry(th1d_backgroundEnergy     , "Background Cluster Energy [MeV]");
  legend->AddEntry(th1d_backgroundEnergy     , Form("Total Rate = %.1f [SNEvent/10-kt/day]", 
						    th1d_backgroundEnergy->Integral()));
  legend->AddEntry(th1d_signalEnergy         , "Arbitrary Normalization Signal Cluster Energy [MeV]"    );
  legend->AddEntry(th1d_SignalFatElectron    , "E_{e, max} signal [MeV]"        );
  legend->AddEntry(th1d_BackgroundFatElectron, "E_{e, max} background [MeV]"    );

  c.Clear();
  legend->Draw();
  c.Print(OutFileName.c_str());  
 
  // 37690



  c.Print((OutFileName+"]").c_str());

}

  // PlotAll(effEnergy);
  // c.Print(OutFileName.c_str());
  
  // PlotAll(nNoiseHit);
  // c.Print(OutFileName.c_str());

  // PlotAll(nSignalHit);
  // c.Print(OutFileName.c_str());

  // PlotAll(nNoiseCluster);
  // c.Print(OutFileName.c_str());

  // PlotAll(nSignalCluster);
  // c.Print(OutFileName.c_str());

  // c.Clear();
  // legend->Draw();
  // c.Print(OutFileName.c_str());

  // gPad->SetRightMargin(1.5*gPad->GetRightMargin());
  // for (auto const& it: th2d_nNoiseHit     ) { it.second->Draw("COLZ"); c.Print(OutFileName.c_str()); }
  // for (auto const& it: th2d_nSignalHit    ) { it.second->Draw("COLZ"); c.Print(OutFileName.c_str()); }
  // for (auto const& it: th2d_nNoiseCluster ) { it.second->Draw("COLZ"); c.Print(OutFileName.c_str()); }
  // for (auto const& it: th2d_nSignalCluster) { it.second->Draw("COLZ"); c.Print(OutFileName.c_str()); }

    // effEnergy             = new TEfficiency(Form("Eff_Th%i", adc_threshold),
    // 							   ";electron KE [MeV];Efficiency", 25, 0, 25);
    // nNoiseHit             = new TProfile(Form("nNoiseHit_Th%i", adc_threshold),
    // 							";electron KE [MeV];nNoiseHit", 25, 0, 25);
    // nSignalHit            = new TProfile(Form("nSignalHit_Th%i", adc_threshold),
    // 							";electron KE [MeV];nSignalHit", 25, 0, 25);
    // nNoiseCluster         = new TProfile(Form("nNoiseCluster_Th%i", adc_threshold),
    // 							";electron KE [MeV];nNoiseCluster", 25, 0, 25);
    // nSignalCluster        = new TProfile(Form("nSignalCluster_Th%i", adc_threshold),
    // 							";electron KE [MeV];nSignalCluster", 25, 0, 25);
    // th2d_nNoiseHit        = new TH2D(Form("nNoiseHit_Th%i_th2", adc_threshold),
    //                                                 Form("Threshold %i ADC;electron KE [MeV];nNoiseHit;Event", 
    // 							 adc_threshold/100),25, 0, 25, 20, 0, 20);
    // th2d_nSignalHit       = new TH2D(Form("nSignalHit_Th%i_th2", adc_threshold),
    //                                                 Form("Threshold %i ADC;electron KE [MeV];nSignalHit;Event",
    // 							 adc_threshold/100),25, 0, 25, 20, 0, 20);
    // th2d_nNoiseCluster    = new TH2D(Form("nNoiseCluster_Th%i_th2", adc_threshold),
    //                                               Form("Threshold %i ADC;electron KE [MeV];nNoiseCluster;Event"
    // 						       ,adc_threshold/100),25, 0, 25, 20, 0, 20);
    // th2d_nSignalCluster   = new TH2D(Form("nSignalCluster_Th%i_th2", adc_threshold),
    //                                               Form("Threshold %i ADC;electron KE [MeV];nSignalCluster;Event",
    // 						       adc_threshold/100), 25, 0, 25, 20, 0, 20);
