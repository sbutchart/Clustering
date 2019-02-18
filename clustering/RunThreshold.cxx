#include "ArbitraryAnaInputManager.hh"
#include "ClusterEngine.hh"
#include "Trigger.hh"

#include "Helper.h"

#include "TCanvas.h"
#include "TEfficiency.h"
#include "TFile.h"
#include "TPad.h"
#include "TH2D.h"
#include "TF1.h"
#include "TLegend.h"
#include "TProfile.h"
#include "TTree.h"
#include "TGraph.h"

#include <iostream>
#include <unistd.h>

int main (int argc, char** argv) {

  int opt;
  // Shut GetOpt error messages down (return '?'):
  extern char *optarg;
  extern int  optopt;
  
  std::string InFileName   ="";
  std::string OutFileName  ="test.root";
  std::string TreeName     ="";
  std::string ModelFileName="";
  int nEvent=-1;
  while ( (opt = getopt(argc, argv, "i:o:t:n:m:")) != -1 ) {  // for each option...
    switch ( opt ) {
    case 'i':
      InFileName = optarg;
      break;
    case 'o':
      OutFileName = optarg;
      break;
    case 't':
      TreeName = optarg;
      break;
    case 'n':
      nEvent = atoi(optarg);
      break;
    case 'm':
      ModelFileName = optarg;
      break;
    case '?':  // unknown option...
      std::cerr << "Unknown option: '" << char(optopt) << "'!" << std::endl;
      break;
    }
  }

  TFile weights(ModelFileName.c_str(),"READ");
  TH1D* TotalSun = (TH1D*) weights.Get("SolarNu_weight");
  TH1D* B8Sun    = (TH1D*) weights.Get("SolarB8Nu_weight");
  TH1D* HEPSun   = (TH1D*) weights.Get("SolarHEPNu_weight");
  TotalSun->SetDirectory(0);
  B8Sun   ->SetDirectory(0);
  HEPSun  ->SetDirectory(0);
  TFile hfile(OutFileName.c_str(), "RECREATE");

  std::map<int, SNAnaInputManager*> aaim;
  aaim[1] = new SNAnaInputManager();
  aaim[1] -> SetInputTree(TreeName.c_str());

  for (auto& it: aaim) {
    it.second->SetInputFile(InFileName);
    it.second->LoadTree();
  }

  ClusterEngine clusteng;
  SimpleTrigger wiretrigger;

  clusteng.SetTimeWindow   (20);
  clusteng.SetChannelWidth (2);
  clusteng.SetTimeWindowOpt(0.2);
  clusteng.SetPositionOpt  (50);
  clusteng.SetBucketSize   (1);
  
  SimpleGeom sg;
  std::vector<std::pair<int,double>> Threshold = {std::make_pair(3,    0.),
                                                  //std::make_pair(3,  100.),
                                                  std::make_pair(3,  200.),
                                                  //std::make_pair(3,  300.),
                                                  std::make_pair(3,  400.),
                                                  std::make_pair(3,  500.),
                                                  std::make_pair(3,  550.),
                                                  std::make_pair(3,  600.),
                                                  std::make_pair(3,  650.),
                                                  std::make_pair(3,  700.),
                                                  std::make_pair(3,  750.),
                                                  std::make_pair(3,  800.),
                                                  std::make_pair(3,  850.),
                                                  std::make_pair(3,  900.),
                                                  std::make_pair(3, 1000.),
                                                  //std::make_pair(3, 1100.),
                                                  std::make_pair(3, 1200.),
                                                  //std::make_pair(3, 1300.),
                                                  std::make_pair(3, 1400.)
                                                  //std::make_pair(3, 1500.)
  };

  TH1D* TotalSpectrum = new TH1D    ("TotalSpectrum", ";Electron KE;Total Solar Spectrum [a.u.]", 20,0,20);
  TH1D* B8Spectrum    = new TH1D    ("B8Spectrum",    ";Electron KE;B8 Spectrum [a.u.]",          20,0,20);
  TH1D* HEPSpectrum   = new TH1D    ("HEPSpectrum",   ";Electron KE;HEP Spectrum [a.u.]",         20,0,20);

  TH1D*     TotalResolution = new TH1D    ("TotalResolution", ";X_{Reconstructed} - X_{True} [cm];NEvent",100,-500,500);
  TH1D*     Bias            = new TH1D    ("Bias",            ";X True Position [cm];(X_{Reconstructed} - X_{True}) / X_{True}",100,0,400);
  TH1D*     Resolution      = new TH1D    ("Resolution",      ";X True Position [cm];Relative resolution",100,0,400);
  TProfile* Resolution_prof = new TProfile("Resolution_prof", ";X True Position [cm];X Reco Position[cm]",100,0,400);
  TH2D*     Resolution_th2d = new TH2D    ("Resolution_th2d", ";X True Position [cm];X Reco Position[cm]",100,0,400, 100,0,400);
  
  Resolution_th2d->SetStats(0);
  Bias      ->SetStats(0);
  Resolution->SetStats(0);

  TH2D*     ADCEnergy_total_th2d   = new TH2D    ("ADCDistance_total_th2d_%i",   "No correction;ADC Cluster;Lepton energy [MeV]", 50, 0, 5000, 20, 0, 20);
  TProfile* ADCEnergy_total_prof   = new TProfile("ADCDistance_total_prof_%i",   ";ADC Cluster;Lepton energy [MeV]", 50, 0, 5000);
  TH2D*     ADCEnergy_recalib_th2d = new TH2D    ("ADCDistance_recalib_th2d_%i", "PDS correction;ADC Cluster;Lepton energy [MeV]", 50, 0, 5000, 20, 0, 20);
  TProfile* ADCEnergy_recalib_prof = new TProfile("ADCDistance_recalib_prof_%i", ";ADC Cluster;Lepton energy [MeV]", 50, 0, 5000);
  TH2D*     ADCEnergy_region_th2d  = new TH2D    ("ADCDistance_region_th2d_%i",  "x slice;ADC Cluster;Lepton energy [MeV]", 50, 0, 5000, 20, 0, 20);
  TProfile* ADCEnergy_region_prof  = new TProfile("ADCDistance_region_prof_%i",  ";ADC Cluster;Lepton energy [MeV]", 50, 0, 5000);
  TH2D*     ADCEnergy_cheat_th2d   = new TH2D    ("ADCDistance_cheat_th2d_%i",   "Perfect correction;ADC Cluster;Lepton energy [MeV]", 50, 0, 5000, 20, 0, 20);
  TProfile* ADCEnergy_cheat_prof   = new TProfile("ADCDistance_cheat_prof_%i",   ";ADC Cluster;Lepton energy [MeV]", 50, 0, 5000);
  ADCEnergy_total_th2d  ->SetStats(0);
  ADCEnergy_total_prof  ->SetStats(0);
  ADCEnergy_recalib_th2d->SetStats(0);
  ADCEnergy_recalib_prof->SetStats(0);
  ADCEnergy_region_th2d ->SetStats(0);
  ADCEnergy_region_prof ->SetStats(0);
  ADCEnergy_cheat_th2d  ->SetStats(0);
  ADCEnergy_cheat_prof  ->SetStats(0);

  std::map<std::pair<int,double>,TEfficiency*> Efficiency;
  std::map<std::pair<int,double>,TEfficiency*> Efficiency_PDS;
  std::map<std::pair<int,double>,TEfficiency*> Efficiency_Cheat;
    
  TGraph* ROC_Curve_5MeV           = new TGraph(Threshold.size());
  TGraph* ROC_Curve_10MeV          = new TGraph(Threshold.size());
  TGraph* ROC_Curve_HEP            = new TGraph(Threshold.size());
  TGraph* ROC_Curve_B8             = new TGraph(Threshold.size());
  TGraph* ROC_Curve_AllSolar       = new TGraph(Threshold.size());
  TGraph* ROC_Curve_PDS_5MeV       = new TGraph(Threshold.size());
  TGraph* ROC_Curve_PDS_10MeV      = new TGraph(Threshold.size());
  TGraph* ROC_Curve_PDS_HEP        = new TGraph(Threshold.size());
  TGraph* ROC_Curve_PDS_B8         = new TGraph(Threshold.size());
  TGraph* ROC_Curve_PDS_AllSolar   = new TGraph(Threshold.size());
  TGraph* ROC_Curve_Cheat_5MeV     = new TGraph(Threshold.size());
  TGraph* ROC_Curve_Cheat_10MeV    = new TGraph(Threshold.size());
  TGraph* ROC_Curve_Cheat_HEP      = new TGraph(Threshold.size());
  TGraph* ROC_Curve_Cheat_B8       = new TGraph(Threshold.size());
  TGraph* ROC_Curve_Cheat_AllSolar = new TGraph(Threshold.size());

  TF1 ADC_Calib ("ADC_Calib", "[0] * TMath::Exp(-[1] * TMath::Abs(x - [2]));", 0, 400);
  ADC_Calib.SetParameters(1286.92,0.128495/90,0);

  int fNEvent = nEvent;

  TF1 Parametrisation ("Parametrisation", "[0]+[1]*x+[2]*sin(x/[3])", 0, 6000);
  Parametrisation.SetParameters(6.24901e+00,
                                6.55628e-02,
                                4.37581e+01,
                                1.43180e+03);

  for (auto const& it: aaim) {
    SNAnaInputManager* im = it.second;
    
    if (fNEvent!=-1) {
      fNEvent = std::min(fNEvent,(int)im->GetEntries());
    } else {
      fNEvent = im->GetEntries();
    }
    
    for (size_t iThreshold = 0; iThreshold<Threshold.size(); iThreshold++) {
      std::cout << iThreshold << " / " << Threshold.size() << std::endl;
      
      bool first = (Threshold[iThreshold] == Threshold.front());
      double ADC  = Threshold[iThreshold].second;
      int    NHit = Threshold[iThreshold].first;
      Efficiency    [Threshold[iThreshold]]   = new TEfficiency(Form("Efficiency_ADC%f_NHit%i",      ADC, NHit),";Electron KE [MeV];Efficiency", 20, 0, 20);
      Efficiency_PDS[Threshold[iThreshold]]   = new TEfficiency(Form("Efficiency_ADC%f_NHit%i_PDS",  ADC, NHit),";Electron KE [MeV];Efficiency", 20, 0, 20);
      Efficiency_Cheat[Threshold[iThreshold]] = new TEfficiency(Form("Efficiency_ADC%f_NHit%i_Cheat",ADC, NHit),";Electron KE [MeV];Efficiency", 20, 0, 20);
      
      wiretrigger.SetWireNHitsMin    (NHit);
      wiretrigger.SetWireNChanMin    (2);
      wiretrigger.SetWireChanWidthMin(0);
      wiretrigger.SetWireSADCMin     (0);
      
      double nTotalEvent=0;
      double nTotalSolar=0;
      double nHEPSolar=0;
      double nB8Solar=0;
      
      double nTrigTotalSolar=0;
      double nTrigHEPSolar=0;
      double nTrigB8Solar=0;
      double nTrigBackground=0;
      
      double nTrigTotalSolarPDS=0;
      double nTrigHEPSolarPDS=0;
      double nTrigB8SolarPDS=0;
      double nTrigBackgroundPDS=0;
      
      double nTrigTotalSolarCheat=0;
      double nTrigHEPSolarCheat=0;
      double nTrigB8SolarCheat=0;
      double nTrigBackgroundCheat=0;
      CurrentProg=0;

      double NominalADCWeight = ADC_Calib.Eval(50);

      for (int CurrentEvent=0; CurrentEvent<fNEvent; ++CurrentEvent) {
        PrintProgress(CurrentEvent, fNEvent);
        im->GetEntry(CurrentEvent);

        double weight_Total_Sun = TotalSun->GetBinContent(TotalSun->FindBin(im->True_ENu->at(0) * 1000.));
        double weight_HEP_Sun   = HEPSun  ->GetBinContent(HEPSun  ->FindBin(im->True_ENu->at(0) * 1000.));
        double weight_B8_Sun    = B8Sun   ->GetBinContent(B8Sun   ->FindBin(im->True_ENu->at(0) * 1000.));

        nTotalEvent += 1;
        nTotalSolar += weight_Total_Sun;
        nHEPSolar   += weight_HEP_Sun  ;
        nB8Solar    += weight_B8_Sun   ;
        
        std::vector<WireHit*> vec_WireHit;
        im->GetWireHits(vec_WireHit);
        std::vector<OpticalHit*> vec_OptiHit;
        im->GetOpticalHits(vec_OptiHit);
      
        std::vector<WireCluster*> vec_WireCluster;
        std::vector<OpticalCluster*> vec_OptiCluster;
        clusteng.ClusterHits2      (vec_WireHit, vec_WireCluster);
        clusteng.ClusterOpticalHits(vec_OptiHit, vec_OptiCluster);
        wiretrigger.IsTriggering   (vec_WireCluster);

        bool trigger=false;
        bool triggerPDS=false;
        bool triggerCheat=false;

        if (first) {
          TotalSpectrum ->Fill(im->True_ENu_Lep->at(0) * 1000. - 0.511, weight_Total_Sun);
          B8Spectrum    ->Fill(im->True_ENu_Lep->at(0) * 1000. - 0.511, weight_B8_Sun   );
          HEPSpectrum   ->Fill(im->True_ENu_Lep->at(0) * 1000. - 0.511, weight_HEP_Sun  );
        }

        for (size_t c=0; c<vec_WireCluster.size(); ++c) {
          WireCluster* wire_clust = vec_WireCluster[c];

          if (wire_clust->GetIsTriggering() && wire_clust->GetSumPeak() > ADC) {
            if (wire_clust->GetType()) {

              trigger=true;
              nTrigTotalSolar += weight_Total_Sun;
              nTrigHEPSolar   += weight_HEP_Sun  ;
              nTrigB8Solar    += weight_B8_Sun   ;
              
              if (first) {
                ADCEnergy_total_th2d->Fill(wire_clust->GetSumPeak(), im->True_ENu_Lep->at(0)*1000.);
                ADCEnergy_total_prof->Fill(wire_clust->GetSumPeak(), im->True_ENu_Lep->at(0)*1000.);
                
                if (abs((*im->True_VertX)[0]) > 15 && abs((*im->True_VertX)[0]) < 90) {
                  ADCEnergy_region_th2d->Fill(wire_clust->GetSumPeak(), im->True_ENu_Lep->at(0)*1000.);
                  ADCEnergy_region_prof->Fill(wire_clust->GetSumPeak(), im->True_ENu_Lep->at(0)*1000.);
                }
              }
            } else {
              nTrigBackground++;
            }
          }
          
          if (wire_clust->GetIsTriggering()) {
            size_t apa_wire = sg.GetAPAFromPosition(wire_clust->GetTruePosition(kX),
                                                    wire_clust->GetTruePosition(kY),
                                                    wire_clust->GetTruePosition(kZ));
              
            OpticalCluster* interesting_optiClust = new OpticalCluster();
            OpticalCluster* deleteme = interesting_optiClust;
            interesting_optiClust->SetSumPeak(-1);
            for (auto const& opti_clust: vec_OptiCluster) {
              size_t apa_opti = sg.GetAPAFromPosition(opti_clust->GetPosition(kX),
                                                      opti_clust->GetPosition(kY),
                                                      opti_clust->GetPosition(kZ));

              if (apa_opti == apa_wire &&
                  opti_clust->GetSumPeak() > interesting_optiClust->GetSumPeak()) {
                interesting_optiClust = opti_clust;
              }
            }
            double reco_x = Parametrisation.Eval(wire_clust->GetPosition(kT) - interesting_optiClust->GetPosition(kT));
            
            if (first && wire_clust->GetType()) {
              TotalResolution->Fill(reco_x - abs((*im->True_VertX)[0]));
              Resolution_prof->Fill(abs((*im->True_VertX)[0]), reco_x);
              Resolution_th2d->Fill(abs((*im->True_VertX)[0]), reco_x);
            }
           
            double ThisClusterWeight =  NominalADCWeight / ADC_Calib.Eval(reco_x);
            double NewADC = wire_clust->GetSumPeak() * ThisClusterWeight;
            if (first) {
              ADCEnergy_recalib_th2d->Fill(NewADC, im->True_ENu_Lep->at(0)*1000.);
              ADCEnergy_recalib_prof->Fill(NewADC, im->True_ENu_Lep->at(0)*1000.);
            }
            double ThisClusterTrueWeight =  NominalADCWeight / ADC_Calib.Eval((*im->True_VertX)[0]);
            double RealADC = wire_clust->GetSumPeak() * ThisClusterTrueWeight;
            
            // std::cout << "\nbare ADC " << wire_clust->GetSumPeak() << std::endl;
            // std::cout << "reco_x " << reco_x << std::endl;
            // std::cout << "(*im->True_VertX)[0] " << (*im->True_VertX)[0] << std::endl;
            // std::cout << "NominalADCWeight " << NominalADCWeight << std::endl;
            // std::cout << "ADC_Calib.Eval(reco_x) " << ADC_Calib.Eval(reco_x) << std::endl;
            // std::cout << "corrected ADC " << newADC << std::endl;

            
            if (NewADC > ADC) {
              if (wire_clust->GetType()) {
                triggerPDS = true;
                nTrigTotalSolarPDS += weight_Total_Sun;
                nTrigHEPSolarPDS   += weight_HEP_Sun  ;
                nTrigB8SolarPDS    += weight_B8_Sun   ;
              } else {

                nTrigBackgroundPDS++;
              }
            }
            if (RealADC > ADC) {
              if (wire_clust->GetType()) {
                triggerCheat = true;
                nTrigTotalSolarCheat += weight_Total_Sun;
                nTrigHEPSolarCheat   += weight_HEP_Sun  ;
                nTrigB8SolarCheat    += weight_B8_Sun   ;
              } else {

                nTrigBackgroundCheat++;
              }
            }
            delete deleteme;
          }
        }

        double KE = im->True_ENu_Lep->at(0) * 1000. - 0.511;
        Efficiency      [Threshold[iThreshold]]->Fill(trigger     , KE);
        Efficiency_PDS  [Threshold[iThreshold]]->Fill(triggerPDS  , KE);
        Efficiency_Cheat[Threshold[iThreshold]]->Fill(triggerCheat, KE);
        
        for (auto& it: vec_WireHit)     { delete it; it = NULL; }
        for (auto& it: vec_WireCluster) { delete it; it = NULL; }
        for (auto& it: vec_OptiHit)     { delete it; it = NULL; }
        for (auto& it: vec_OptiCluster) { delete it; it = NULL; }
        vec_WireHit    .clear();
        vec_WireCluster.clear();
        vec_OptiHit    .clear();
        vec_OptiCluster.clear();
      }
      //std::cout << itadc << " "<< n_background_cluster << std::endl;
      // To get to the background events weights, one has to multiply the number of backgorund events by:
      // Multiply by the number of seconds in a year
      // Multiply by the weight (number of GBytes) of an event in GB
      // Divide by the total number of events
      // Divide by the larsoft time
      // Divide by the number of APA
      // Divide by the 1024x1024 for converting GB->PB
      std::cout << "nTrigBackground                    " << nTrigBackground      << std::endl; 
      std::cout << "nTrigBackgroundPDS                 " << nTrigBackgroundPDS   << std::endl;
      std::cout << "nTrigBackgroundCheat               " << nTrigBackgroundCheat << std::endl;
      std::cout << "nTrigB8Solar / nB8Solar            " << nTrigB8Solar         << " / " << nB8Solar    << std::endl;
      std::cout << "nTrigTotalSolar / nTotalSolar      " << nTrigTotalSolar      << " / " << nTotalSolar << std::endl;
      std::cout << "nTrigHEPSolarPDS / nHEPSolar       " << nTrigHEPSolarPDS     << " / " << nHEPSolar   << std::endl;
      std::cout << "nTrigB8SolarPDS / nB8Solar         " << nTrigB8SolarPDS      << " / " << nB8Solar    << std::endl;
      std::cout << "nTrigTotalSolarPDS / nTotalSolar   " << nTrigTotalSolarPDS   << " / " << nTotalSolar << std::endl;
      std::cout << "nTrigHEPSolarCheat / nHEPSolar     " << nTrigHEPSolarCheat   << " / " << nHEPSolar   << std::endl;
      std::cout << "nTrigB8SolarCheat / nB8Solar       " << nTrigB8SolarCheat    << " / " << nB8Solar    << std::endl;
      std::cout << "nTrigTotalSolarCheat / nTotalSolar " << nTrigTotalSolarCheat << " / " << nTotalSolar << std::endl;

      double factor = 6.22 * 3600. * 24. * 365. / nTotalEvent / 2.245e-3 / 150. / 1024. / 1024.;
      ROC_Curve_5MeV          ->SetPoint(iThreshold, Efficiency[Threshold[iThreshold]]->GetEfficiency(Efficiency[Threshold[iThreshold]]->FindFixBin( 5.))            , nTrigBackground      * factor);
      ROC_Curve_10MeV         ->SetPoint(iThreshold, Efficiency[Threshold[iThreshold]]->GetEfficiency(Efficiency[Threshold[iThreshold]]->FindFixBin(10.))            , nTrigBackground      * factor);
      ROC_Curve_HEP           ->SetPoint(iThreshold, nTrigHEPSolar / nHEPSolar                                                                                       , nTrigBackground      * factor);
      ROC_Curve_B8            ->SetPoint(iThreshold, nTrigB8Solar / nB8Solar                                                                                         , nTrigBackground      * factor);
      ROC_Curve_AllSolar      ->SetPoint(iThreshold, nTrigTotalSolar / nTotalSolar                                                                                   , nTrigBackground      * factor);
      ROC_Curve_PDS_5MeV      ->SetPoint(iThreshold, Efficiency_PDS[Threshold[iThreshold]]->GetEfficiency(Efficiency_PDS[Threshold[iThreshold]]->FindFixBin( 5.))    , nTrigBackgroundPDS   * factor);
      ROC_Curve_PDS_10MeV     ->SetPoint(iThreshold, Efficiency_PDS[Threshold[iThreshold]]->GetEfficiency(Efficiency_PDS[Threshold[iThreshold]]->FindFixBin(10.))    , nTrigBackgroundPDS   * factor);
      ROC_Curve_PDS_HEP       ->SetPoint(iThreshold, nTrigHEPSolarPDS / nHEPSolar                                                                                    , nTrigBackgroundPDS   * factor);
      ROC_Curve_PDS_B8        ->SetPoint(iThreshold, nTrigB8SolarPDS / nB8Solar                                                                                      , nTrigBackgroundPDS   * factor);
      ROC_Curve_PDS_AllSolar  ->SetPoint(iThreshold, nTrigTotalSolarPDS / nTotalSolar                                                                                , nTrigBackgroundPDS   * factor);
      ROC_Curve_Cheat_5MeV    ->SetPoint(iThreshold, Efficiency_Cheat[Threshold[iThreshold]]->GetEfficiency(Efficiency_Cheat[Threshold[iThreshold]]->FindFixBin( 5.)), nTrigBackgroundCheat * factor);
      ROC_Curve_Cheat_10MeV   ->SetPoint(iThreshold, Efficiency_Cheat[Threshold[iThreshold]]->GetEfficiency(Efficiency_Cheat[Threshold[iThreshold]]->FindFixBin(10.)), nTrigBackgroundCheat * factor);
      ROC_Curve_Cheat_HEP     ->SetPoint(iThreshold, nTrigHEPSolarCheat / nHEPSolar                                                                                  , nTrigBackgroundCheat * factor);
      ROC_Curve_Cheat_B8      ->SetPoint(iThreshold, nTrigB8SolarCheat / nB8Solar                                                                                    , nTrigBackgroundCheat * factor);
      ROC_Curve_Cheat_AllSolar->SetPoint(iThreshold, nTrigTotalSolarCheat / nTotalSolar                                                                              , nTrigBackgroundCheat * factor);
    }
  }
  
  for (int i=0; i<Resolution_prof->GetXaxis()->GetNbins(); ++i) {
    double BinContent = Resolution_prof->GetBinContent(i);
    if (BinContent == 0) continue;
    double BinCenter  = Resolution_prof->GetBinCenter(i);
    double BinError   = Resolution_prof->GetBinError(i);
    Bias->SetBinContent(i, (BinCenter - BinContent) / BinCenter);
    Resolution->SetBinContent(i, BinError  /BinCenter);
    Bias->SetBinError(i,0.);
    Resolution->SetBinError(i,0.);
  }

  TCanvas c;
  c.Print("XResolution.pdf[");
  TotalResolution->Draw(); c.Print("XResolution.pdf");
  Bias->Draw();            c.Print("XResolution.pdf");
  Bias->SetMinimum(-1);
  Bias->Draw();            c.Print("XResolution.pdf");
  Resolution->Draw();      c.Print("XResolution.pdf");
  gPad->SetLogy();
  Resolution->Draw();      c.Print("XResolution.pdf");
  gPad->SetLogy(false);
  Resolution_prof->SetLineColor(kBlack);
  Resolution_prof->SetLineWidth(2);
  Resolution_th2d->Draw("COLZ");
  Resolution_prof->Draw("SAME");
  c.Print("XResolution.pdf");
  c.Print("XResolution.pdf]");

  TCanvas c1;
  c1.Print("ADCCalib.pdf[");
  ADCEnergy_total_prof  ->SetLineWidth(2);
  ADCEnergy_recalib_prof->SetLineWidth(2);
  ADCEnergy_region_prof ->SetLineWidth(2);
  ADCEnergy_total_prof  ->SetLineColor(kBlack);
  ADCEnergy_recalib_prof->SetLineColor(kBlack);
  ADCEnergy_region_prof ->SetLineColor(kBlack);
  
  ADCEnergy_total_th2d  ->Draw("COLZ"); 
  ADCEnergy_total_prof  ->Draw("SAME"); c1.Print("ADCCalib.pdf");
  ADCEnergy_recalib_th2d->Draw("COLZ");
  ADCEnergy_recalib_prof->Draw("SAME"); c1.Print("ADCCalib.pdf");
  ADCEnergy_region_th2d ->Draw("COLZ");
  ADCEnergy_region_prof ->Draw("SAME"); c1.Print("ADCCalib.pdf");
  c1.Print("ADCCalib.pdf]");

  TCanvas c2;
  c2.Print("ROCCurve.pdf[");
  ROC_Curve_5MeV          ->SetLineColor(kRed ); ROC_Curve_5MeV          ->SetLineWidth(2);
  ROC_Curve_10MeV         ->SetLineColor(kRed ); ROC_Curve_10MeV         ->SetLineWidth(2);
  ROC_Curve_HEP           ->SetLineColor(kRed ); ROC_Curve_HEP           ->SetLineWidth(2);
  ROC_Curve_B8            ->SetLineColor(kRed ); ROC_Curve_B8            ->SetLineWidth(2);
  ROC_Curve_AllSolar      ->SetLineColor(kRed ); ROC_Curve_AllSolar      ->SetLineWidth(2);
  ROC_Curve_PDS_5MeV      ->SetLineColor(kBlue); ROC_Curve_PDS_5MeV      ->SetLineWidth(2);
  ROC_Curve_PDS_10MeV     ->SetLineColor(kBlue); ROC_Curve_PDS_10MeV     ->SetLineWidth(2);
  ROC_Curve_PDS_HEP       ->SetLineColor(kBlue); ROC_Curve_PDS_HEP       ->SetLineWidth(2);
  ROC_Curve_PDS_B8        ->SetLineColor(kBlue); ROC_Curve_PDS_B8        ->SetLineWidth(2);
  ROC_Curve_PDS_AllSolar  ->SetLineColor(kBlue); ROC_Curve_PDS_AllSolar  ->SetLineWidth(2);
  ROC_Curve_Cheat_5MeV    ->SetLineColor(kBlue); ROC_Curve_Cheat_5MeV    ->SetLineWidth(2); ROC_Curve_Cheat_5MeV    ->SetLineStyle(2);
  ROC_Curve_Cheat_10MeV   ->SetLineColor(kBlue); ROC_Curve_Cheat_10MeV   ->SetLineWidth(2); ROC_Curve_Cheat_10MeV   ->SetLineStyle(2);
  ROC_Curve_Cheat_HEP     ->SetLineColor(kBlue); ROC_Curve_Cheat_HEP     ->SetLineWidth(2); ROC_Curve_Cheat_HEP     ->SetLineStyle(2);
  ROC_Curve_Cheat_B8      ->SetLineColor(kBlue); ROC_Curve_Cheat_B8      ->SetLineWidth(2); ROC_Curve_Cheat_B8      ->SetLineStyle(2);
  ROC_Curve_Cheat_AllSolar->SetLineColor(kBlue); ROC_Curve_Cheat_AllSolar->SetLineWidth(2); ROC_Curve_Cheat_AllSolar->SetLineStyle(2);
  
  ROC_Curve_5MeV          ->SetTitle("ROC curve 5MeV electron");             
  ROC_Curve_10MeV         ->SetTitle("ROC curve 10MeV electron");            
  ROC_Curve_HEP           ->SetTitle("ROC curve HEP #nu");                   
  ROC_Curve_B8            ->SetTitle("ROC curve B8 #nu");                    
  ROC_Curve_AllSolar      ->SetTitle("ROC curve All solar #nu");             
  ROC_Curve_PDS_5MeV      ->SetTitle("ROC curve (with PDS) 5MeV electron");  
  ROC_Curve_PDS_10MeV     ->SetTitle("ROC curve (with PDS) 10MeV electron"); 
  ROC_Curve_PDS_HEP       ->SetTitle("ROC curve (with PDS) HEP #nu");        
  ROC_Curve_PDS_B8        ->SetTitle("ROC curve (with PDS) B8 #nu");         
  ROC_Curve_PDS_AllSolar  ->SetTitle("ROC curve (with PDS) All solar #nu");
  ROC_Curve_Cheat_5MeV    ->SetTitle("ROC curve (with Cheat) 5MeV electron");  
  ROC_Curve_Cheat_10MeV   ->SetTitle("ROC curve (with Cheat) 10MeV electron"); 
  ROC_Curve_Cheat_HEP     ->SetTitle("ROC curve (with Cheat) HEP #nu");        
  ROC_Curve_Cheat_B8      ->SetTitle("ROC curve (with Cheat) B8 #nu");         
  ROC_Curve_Cheat_AllSolar->SetTitle("ROC curve (with Cheat) All solar #nu");
  
  ROC_Curve_5MeV          ->GetXaxis()->SetTitle("Efficiency 5MeV"); 
  ROC_Curve_10MeV         ->GetXaxis()->SetTitle("Efficiency 10MeV"); 
  ROC_Curve_HEP           ->GetXaxis()->SetTitle("Efficiency HEP"); 
  ROC_Curve_B8            ->GetXaxis()->SetTitle("Efficiency B8"); 
  ROC_Curve_AllSolar      ->GetXaxis()->SetTitle("Efficiency AllSolar"); 
  ROC_Curve_PDS_5MeV      ->GetXaxis()->SetTitle("Efficiency PDS 5MeV"); 
  ROC_Curve_PDS_10MeV     ->GetXaxis()->SetTitle("Efficiency PDS 10MeV"); 
  ROC_Curve_PDS_HEP       ->GetXaxis()->SetTitle("Efficiency PDS HEP"); 
  ROC_Curve_PDS_B8        ->GetXaxis()->SetTitle("Efficiency PDS B8"); 
  ROC_Curve_PDS_AllSolar  ->GetXaxis()->SetTitle("Efficiency AllSolar");
  ROC_Curve_Cheat_5MeV    ->GetXaxis()->SetTitle("Efficiency Cheat 5MeV"); 
  ROC_Curve_Cheat_10MeV   ->GetXaxis()->SetTitle("Efficiency Cheat 10MeV"); 
  ROC_Curve_Cheat_HEP     ->GetXaxis()->SetTitle("Efficiency Cheat HEP"); 
  ROC_Curve_Cheat_B8      ->GetXaxis()->SetTitle("Efficiency Cheat B8"); 
  ROC_Curve_Cheat_AllSolar->GetXaxis()->SetTitle("Efficiency Cheat AllSolar");
  
  ROC_Curve_5MeV          ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_10MeV         ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_HEP           ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_B8            ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_AllSolar      ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_PDS_5MeV      ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_PDS_10MeV     ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_PDS_HEP       ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_PDS_B8        ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_PDS_AllSolar  ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_Cheat_5MeV    ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_Cheat_10MeV   ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_Cheat_HEP     ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_Cheat_B8      ->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");
  ROC_Curve_Cheat_AllSolar->GetYaxis()->SetTitle("PB of background / year (1 APA reading / trigger)");

  gPad->SetLogy();
  ROC_Curve_5MeV          ->Draw("ALP");
  ROC_Curve_PDS_5MeV      ->Draw("LP");
  ROC_Curve_Cheat_5MeV    ->Draw("LP"); c2.Print("ROCCurve.pdf");
  ROC_Curve_10MeV         ->Draw("ALP");
  ROC_Curve_PDS_10MeV     ->Draw("LP");
  ROC_Curve_Cheat_10MeV   ->Draw("LP"); c2.Print("ROCCurve.pdf");
  ROC_Curve_HEP           ->Draw("ALP");
  ROC_Curve_PDS_HEP       ->Draw("LP");
  ROC_Curve_Cheat_HEP     ->Draw("LP"); c2.Print("ROCCurve.pdf");
  ROC_Curve_B8            ->Draw("ALP");
  ROC_Curve_PDS_B8        ->Draw("LP");
  ROC_Curve_Cheat_B8      ->Draw("LP"); c2.Print("ROCCurve.pdf");
  ROC_Curve_AllSolar      ->Draw("ALP");
  ROC_Curve_PDS_AllSolar  ->Draw("LP");
  ROC_Curve_Cheat_AllSolar->Draw("LP"); c2.Print("ROCCurve.pdf");
  c2.Print("ROCCurve.pdf]");
  
  gPad->SetLogy(0);

  TCanvas c3;
  c3.Print("EfficiencyCurve.pdf[");
  for (auto& it: Efficiency) {
    it.second->SetTitle(it.second->GetName());
    it.second->SetLineColor(kBlue);
    it.second->SetLineWidth(2);
    TEfficiency* ef = Efficiency_PDS.at(it.first);
    ef->SetTitle(ef->GetName());
    ef->SetLineColor(kRed);
    ef->SetLineWidth(2);
    TEfficiency* ef2 = Efficiency_Cheat.at(it.first);
    ef2->SetTitle(ef2->GetName());
    ef2->SetLineColor(kRed);
    ef2->SetLineStyle(2);
    ef2->SetLineWidth(2);
    it.second->Draw("");
    ef ->Draw("SAME");
    ef2->Draw("SAME");
    c3.Print("EfficiencyCurve.pdf");
  }
  c3.Print("EfficiencyCurve.pdf]");

  TotalSpectrum->Scale(1. / TotalSpectrum->Integral());
  B8Spectrum   ->Scale(1. / B8Spectrum   ->Integral());
  HEPSpectrum  ->Scale(1. / HEPSpectrum  ->Integral());
  TCanvas c4;
  c4.Print("ElectronSpectrum.pdf[");
  TotalSpectrum->Draw(); c4.Print("ElectronSpectrum.pdf");
  B8Spectrum   ->Draw(); c4.Print("ElectronSpectrum.pdf");
  HEPSpectrum  ->Draw(); c4.Print("ElectronSpectrum.pdf");
  c4.Print("ElectronSpectrum.pdf]");

  



  

  
  hfile.cd();
 
  TotalResolution->Write();
  Bias           ->Write();
  Bias           ->Write();
  Resolution     ->Write();
  Resolution_prof->Write();
  ADCEnergy_total_th2d  ->Write();
  ADCEnergy_total_prof  ->Write();
  ADCEnergy_recalib_th2d->Write();
  ADCEnergy_recalib_prof->Write();
  ADCEnergy_region_th2d ->Write();
  ADCEnergy_region_prof ->Write();
  ADCEnergy_cheat_th2d  ->Write();
  ADCEnergy_cheat_prof  ->Write();
 
  ROC_Curve_5MeV          ->Write();
  ROC_Curve_10MeV         ->Write();
  ROC_Curve_HEP           ->Write();
  ROC_Curve_B8            ->Write();
  ROC_Curve_AllSolar      ->Write();
  ROC_Curve_PDS_5MeV      ->Write();
  ROC_Curve_PDS_10MeV     ->Write();
  ROC_Curve_PDS_HEP       ->Write();
  ROC_Curve_PDS_B8        ->Write();
  ROC_Curve_PDS_AllSolar  ->Write();
  ROC_Curve_Cheat_5MeV    ->Write();
  ROC_Curve_Cheat_10MeV   ->Write();
  ROC_Curve_Cheat_HEP     ->Write();
  ROC_Curve_Cheat_B8      ->Write();
  ROC_Curve_Cheat_AllSolar->Write();

  for (auto& it: Efficiency) {
    it.second->Write();
  }
  for (auto const& it: Efficiency_PDS) {
    it.second->Write();
  }
  for (auto const& it: Efficiency_Cheat) {
    it.second->Write();
  }

  TotalSpectrum->Write();
  B8Spectrum   ->Write();
  HEPSpectrum  ->Write();

  hfile.Close();


  
  // hfile.Write();
  // for (auto const& it: effEnergy) {
  //   it.second->Write();
  // }
  // TCanvas c;
  // c.Print((OutFileName+".pdf[").c_str());
  // timeDifDistance_th2d->Draw("COLZ");
  // timeDifDistance_prof->SetLineWidth(2);
  // timeDifDistance_prof->SetLineColor(kBlack);
  // timeDifDistance_prof->Draw("SAME");
  // c.Print((OutFileName+".pdf").c_str());
  
  // nhit_elep_th2d->Draw("COLZ");
  // nhit_elep_prof->SetLineWidth(2);     
  // nhit_elep_prof->SetLineColor(kBlack);  
  // nhit_elep_prof->Draw("SAME");
  // c.Print((OutFileName+".pdf").c_str());
  
  // npe_elep_th2d->Draw("COLZ");
  // npe_elep_prof->SetLineWidth(2);     
  // npe_elep_prof->SetLineColor(kBlack);
  // npe_elep_prof->Draw("SAME");
  // c.Print((OutFileName+".pdf").c_str());

  // sadc_elep_th2d->Draw("COLZ");
  // sadc_elep_prof->SetLineWidth(2); 
  // sadc_elep_prof->SetLineColor(kBlack);
  // sadc_elep_prof->Draw("SAME");
  // c.Print((OutFileName+".pdf").c_str());
  // ROC_Curve->SetTitle("Solar efficiency");
  // ROC_Curve->GetXaxis()->SetTitle("PB / year (1APA reading / event)");
  // ROC_Curve->GetYaxis()->SetTitle("Efficiency of 5 MeV electron");
  
  // ROC_Curve->Draw("ALP");
  // c.Print((OutFileName+".pdf").c_str());

  // c.Print((OutFileName+".pdf]").c_str());
  

}

