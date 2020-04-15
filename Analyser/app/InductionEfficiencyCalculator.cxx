#include "IO/InputManager.hh"
#include "Algorithm/ClusterEngine.hh"
#include "Algorithm/Trigger.hh"

#include "Utils/Helper.h"

#include "TCanvas.h"
#include "TEfficiency.h"
#include "TFile.h"
#include "TPad.h"
#include "TH2D.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TTree.h"
#include "TError.h"

#include <iostream>
#include <unistd.h>

int main(int argc, char** argv) {

  int opt;
  // Shut GetOpt error messages down (return '?'): 
  extern char *optarg;
  extern int  optopt;

  std::string InFileName ="";
  std::string OutFileName="";
  int nEvent=-1;
  while ( (opt = getopt(argc, argv, "i:o:n:")) != -1 ) {  // for each option...
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
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  // --- Root information IO
  std::map<int, SNAnaInputManager*> aaim;
  // aaim[40 ] = new SNAnaInputManager(); aaim[40 ]->SetInputTree("snanarunsum800/SNSimTree" );
  // aaim[50 ] = new SNAnaInputManager(); aaim[50 ]->SetInputTree("snanarunsum1000/SNSimTree" );
  // aaim[60 ] = new SNAnaInputManager(); aaim[60 ]->SetInputTree("snanarunsum1200/SNSimTree" );
  // aaim[70 ] = new SNAnaInputManager(); aaim[70 ]->SetInputTree("snanarunsum1400/SNSimTree" );
  aaim[80 ] = new SNAnaInputManager(); aaim[80 ]->SetInputTree("snanarunsum1600/SNSimTree" );
  // aaim[90 ] = new SNAnaInputManager(); aaim[90 ]->SetInputTree("snanarunsum1800/SNSimTree" );
  // aaim[100] = new SNAnaInputManager(); aaim[100]->SetInputTree("snanarunsum2000/SNSimTree");
  // aaim[120] = new SNAnaInputManager(); aaim[120]->SetInputTree("snanarunsum2400/SNSimTree");
  // aaim[140] = new SNAnaInputManager(); aaim[140]->SetInputTree("snanarunsum2800/SNSimTree");
  // aaim[160] = new SNAnaInputManager(); aaim[160]->SetInputTree("snanarunsum3200/SNSimTree"); 

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }

  // --- Define plot containers
  std::map<int, TProfile*>    nNoiseHit;
  std::map<int, TProfile*>    nSignalHit;
  std::map<int, TProfile*>    nBackgroundHit;
  std::map<int, TProfile*>    nNoiseCluster;
  std::map<int, TProfile*>    nSignalCluster;
  std::map<int, TProfile*>    nBackgroundCluster;


  std::map<int, TH2D*>    th2d_nNoiseHit         ;
  std::map<int, TH2D*>    th2d_nSignalHit        ;
  std::map<int, TH2D*>    th2d_nBackgroundHit    ;

  // --- Set up cluster engine and trigger
  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  clusteng.SetTimeWindow   (20 ); wiretrigger.SetWireNHitsMin    (6);
  clusteng.SetChannelWidth (2  ); wiretrigger.SetWireNChanMin    (2);
  clusteng.SetTimeWindowOpt(0.2); wiretrigger.SetWireChanWidthMin(0);
  clusteng.SetPositionOpt  (300); wiretrigger.SetWireSADCMin     (0);
  clusteng.SetBucketSize   (1  );
      
  // --- Running through all input TTrees
  for (auto const& it: aaim) {
    int fNEvent           = nEvent;
    int adc_threshold     = it.first; 
    SNAnaInputManager* im = it.second;

    PrintProgress(adc_threshold, 160);

    if (fNEvent != -1) fNEvent = std::min(fNEvent, (int)im->GetEntries());
    else               fNEvent = im->GetEntries();

    // --- Define each plot object in the containers
    nNoiseHit     [adc_threshold] = new TProfile(Form("nNoiseHit_Th%i", adc_threshold),
                                                 ";electron KE [MeV];nNoiseHit", 20, 0, 3);
    nSignalHit    [adc_threshold] = new TProfile(Form("nSignalHit_Th%i", adc_threshold),
                                                 ";electron KE [MeV];nSignalHit", 20, 0, 3);
    nBackgroundHit[adc_threshold] = new TProfile(Form("nBackgroundHit_Th%i", adc_threshold),
                                                 ";electron KE [MeV];nBackgroundHit", 20, 0, 3);
    nNoiseCluster     [adc_threshold] = new TProfile(Form("nNoiseCluster_Th%i", adc_threshold),
                                                 ";electron KE [MeV];nNoiseCluster", 20, 0, 3);
    nSignalCluster    [adc_threshold] = new TProfile(Form("nSignalCluster_Th%i", adc_threshold),
                                                 ";electron KE [MeV];nSignalCluster", 20, 0, 3);

    // --- Run through each entry in the active TTree
    for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
      im->GetEntry(CurrentEvent);

      // --- Wire Hit data containers
      std::vector<WireHit*> vec_WireHit;
      // Key:      
      //  - nhit[0] noise 
      //  - nhit[1] SN-neutrino
      //  - nhit[2] backgrounds
      std::vector<double> nhit = {0, 0, 0};
      // --- Run through hits in the event
      for (size_t j=0; j<im->Hit_View->size(); ++j) {
	if ((*im->Hit_View)[j] == 2) continue;
        WireHit* hit = new WireHit((*im->Hit_View)[j],        (*im->Hit_True_GenType)[j],  (*im->Hit_Chan)[j],
                                   (*im->Hit_Time)[j],        (*im->Hit_SADC)[j],          (*im->Hit_RMS)[j],
                                   (*im->Hit_True_Energy)[j], (*im->Hit_True_EvEnergy)[j], (*im->Hit_True_MainTrID)[j],
                                   0.5, 0.5, 0.5,
                                   (*im->Hit_True_X)[j],      (*im->Hit_True_Y)[j],        (*im->Hit_True_Z)[j],
                                   0, (*im->Hit_True_nElec)[j]);

	if      ((*im->Hit_True_GenType)[j] == 0) nhit[0]++;
	else if ((*im->Hit_True_GenType)[j] == 1) nhit[1]++;
	else                                      nhit[2]++;
        vec_WireHit.push_back(hit);
      } // Looping through hits

      // bool selected           = false;
      int  ncluster           = 0;
      int  nnoisecluster      = 0;
      
      // --- Hit clustering and triggering
      std::vector<WireCluster*> vec_WireCluster;
      clusteng.ClusterHits2(vec_WireHit, vec_WireCluster);
      wiretrigger.IsTriggering(vec_WireCluster);

      // --- Analyze clusters
      for (size_t c=0; c<vec_WireCluster.size(); ++c) {
      	WireCluster* clust = vec_WireCluster[c];
      	if (clust->GetIsSelected()) {
      	  if      (clust->GetType() == 0) { ++nnoisecluster; }
      	  else if (clust->GetType() == 1) { ++ncluster; }
      	}
      } // Looping through clusters

      std::cout << "Noise     : " << nnoisecluster      << "\t"
      		<< "Signal    : " << ncluster           << std::endl;

      double KE = im->True_Bck_Energy->at(0) * 1000. - 0.511;
      nNoiseHit          [adc_threshold]->Fill(KE, nhit[0]           );
      nSignalHit         [adc_threshold]->Fill(KE, nhit[1]           );
      nBackgroundHit     [adc_threshold]->Fill(KE, nhit[2]           );
      nNoiseCluster      [adc_threshold]->Fill(KE, nnoisecluster     );
      nSignalCluster     [adc_threshold]->Fill(KE, ncluster          );

      for (auto& it: vec_WireHit    ) { delete it; it = NULL; }
      for (auto& it: vec_WireCluster) { delete it; it = NULL; }
      vec_WireHit    .clear();
      vec_WireCluster.clear();

    } // Looping through TTree entries

  } // Looping through TTrees

  const std::vector<int> color = getColors();
  int i=0;
  TLegend* legend = new TLegend(0.1,0.1,0.9,0.9);
  for (auto const& it: nNoiseHit) {
    nNoiseHit          [it.first]->SetStats(0); 
    nSignalHit         [it.first]->SetStats(0);	
    nBackgroundHit     [it.first]->SetStats(0);	
    nNoiseCluster      [it.first]->SetStats(0);
    nSignalCluster     [it.first]->SetStats(0);
    nNoiseHit          [it.first]->SetLineColor  (color.at(i));
    nSignalHit         [it.first]->SetLineColor  (color.at(i));
    nBackgroundHit     [it.first]->SetLineColor  (color.at(i));
    nNoiseCluster      [it.first]->SetLineColor  (color.at(i));
    nSignalCluster     [it.first]->SetLineColor  (color.at(i));
    nNoiseHit          [it.first]->SetMarkerColor(color.at(i));
    nSignalHit         [it.first]->SetMarkerColor(color.at(i));
    nBackgroundHit     [it.first]->SetMarkerColor(color.at(i));
    nNoiseCluster      [it.first]->SetMarkerColor(color.at(i));
    nSignalCluster     [it.first]->SetMarkerColor(color.at(i));
    legend->AddEntry(nNoiseHit[it.first], Form("ADC Threshold = %i",it.first/1), "L");
    i++;
  }
  
  TCanvas c;
  c.Print((OutFileName+"[").c_str());
  PlotAll(nNoiseHit);
  c.Print(OutFileName.c_str());

  PlotAll(nSignalHit);
  c.Print(OutFileName.c_str());

  PlotAll(nBackgroundHit);
  c.Print(OutFileName.c_str());

  PlotAll(nNoiseCluster);
  c.Print(OutFileName.c_str());

  PlotAll(nSignalCluster);
  c.Print(OutFileName.c_str());

  
  c.Clear();
  legend->Draw();
  c.Print(OutFileName.c_str());

  c.Print((OutFileName+"]").c_str());

  return 0;
}
