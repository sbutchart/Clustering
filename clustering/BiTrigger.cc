#include "BiTrigger.hh"


void BiTrigger::AddEventToStudy(const std::vector<std::unique_ptr<WireCluster>>& wc,
                                const std::vector<std::unique_ptr<OpticalCluster>>& oc) {

  std::map<size_t,std::vector<ClusterEnsemble>> apa_cluster_sign;
  std::map<size_t,std::vector<ClusterEnsemble>> apa_cluster_back;

  // for (int iAPA=0; iAPA<12; ++iAPA) {
  //   apa_cluster_sign[iAPA].push_back(ClusterEnsemble());
  //   apa_cluster_back[iAPA].push_back(ClusterEnsemble());
  // }
  apa_cluster_sign[0].push_back(ClusterEnsemble());
  apa_cluster_back[0].push_back(ClusterEnsemble());
  
  // for (auto const& it: wc) {
  //   if (it->GetType() == 1) {
  //     apa_cluster_sign[it->GetAPA()].back().AddCluster(*it);
  //   } else {
  //     apa_cluster_sign[it->GetAPA()].back().AddCluster(*it);
  //     apa_cluster_back[it->GetAPA()].back().AddCluster(*it);
  //   }
  // }

  // for (auto const& it: oc) {
  //   if (it->GetType() == 1) {
  //     apa_cluster_sign[it->GetAPA()].back().AddCluster(*it);
  //   } else {
  //     apa_cluster_sign[it->GetAPA()].back().AddCluster(*it);
  //     apa_cluster_back[it->GetAPA()].back().AddCluster(*it);
  //   }
  // }

  for (auto const& it: wc) {
    if (it->GetType() == 1) {
      apa_cluster_sign[0].back().AddCluster(*it);
      fTot_WireNCluster_Sign++;
    } else {
      fTot_WireNCluster_Back++;
      apa_cluster_sign[0].back().AddCluster(*it);
      apa_cluster_back[0].back().AddCluster(*it);
    }
  }

  for (auto const& it: oc) {
    if (it->GetType() == 1) {
      apa_cluster_sign[0].back().AddCluster(*it);
      fTot_OptiNCluster_Sign++;
    } else {
      fTot_OptiNCluster_Back++;
      apa_cluster_sign[0].back().AddCluster(*it);
      apa_cluster_back[0].back().AddCluster(*it);
    }
  }
  
  for (auto const& it: apa_cluster_sign) {
    for (auto const& it_sign: it.second)
      fSignCluster.push_back(it_sign);
  }
  
  for (auto const& it: apa_cluster_back) {
    for (auto const& it_back: it.second)
      fBackCluster.push_back(it_back);
  }
};

void BiTrigger::FinishStudy() {
  fPt_nClu_Sign = new TGraph(fSignCluster.size());
  fPt_nClu_Back = new TGraph(fBackCluster.size());
  fPt_Peak_Sign = new TGraph(fSignCluster.size());
  fPt_Peak_Back = new TGraph(fBackCluster.size());
  fPt_nHit_Sign = new TGraph(fSignCluster.size());
  fPt_nHit_Back = new TGraph(fBackCluster.size());

  
  fLikelihood_nClu_Sign = new TH2D("Likelihood_nClu_Sign", "",   10, -0.5,   9.5,  10, -0.5,   9.5);
  fLikelihood_nClu_Back = new TH2D("Likelihood_nClu_Back", "",   10, -0.5,   9.5,  10, -0.5,   9.5);
  fLikelihood_Peak_Sign = new TH2D("Likelihood_Peak_Sign", "", 1000, -0.5, 999.5, 300, -0.5, 299.5);
  fLikelihood_Peak_Back = new TH2D("Likelihood_Peak_Back", "", 1000, -0.5, 999.5, 300, -0.5, 299.5);
  fLikelihood_nHit_Sign = new TH2D("Likelihood_nHit_Sign", "",  100, -0.5,  99  , 100, -0.5,  99  );
  fLikelihood_nHit_Back = new TH2D("Likelihood_nHit_Back", "",  100, -0.5,  99  , 100, -0.5,  99  );

  fLikelihood_nClu_Sign->SetStats(0);
  fLikelihood_nClu_Back->SetStats(0);
  fLikelihood_Peak_Sign->SetStats(0);
  fLikelihood_Peak_Back->SetStats(0);
  fLikelihood_nHit_Sign->SetStats(0);
  fLikelihood_nHit_Back->SetStats(0);

  
  int i=0;
  for (auto const& it: fSignCluster) {
    fPt_nClu_Sign->SetPoint(i,  it.nWireCluster   , it.nOpticalCluster   );
    fPt_Peak_Sign->SetPoint(i,  it.PeakWireCluster, it.PeakOpticalCluster);
    fPt_nHit_Sign->SetPoint(i,  it.nHitWireCluster, it.nHitOpticalCluster);
    fLikelihood_nClu_Sign->Fill(it.nWireCluster   , it.nOpticalCluster   );
    fLikelihood_Peak_Sign->Fill(it.PeakWireCluster, it.PeakOpticalCluster);
    fLikelihood_nHit_Sign->Fill(it.nHitWireCluster, it.nHitOpticalCluster);
    ++i;
  }
  
  i=0;
  for (auto const& it: fBackCluster) {
    fPt_nClu_Back->SetPoint(i,  it.nWireCluster   , it.nOpticalCluster   );
    fPt_Peak_Back->SetPoint(i,  it.PeakWireCluster, it.PeakOpticalCluster);
    fPt_nHit_Back->SetPoint(i,  it.nHitWireCluster, it.nHitOpticalCluster);
    fLikelihood_nClu_Back->Fill(it.nWireCluster   , it.nOpticalCluster   );
    fLikelihood_Peak_Back->Fill(it.PeakWireCluster, it.PeakOpticalCluster);
    fLikelihood_nHit_Back->Fill(it.nHitWireCluster, it.nHitOpticalCluster);
    ++i;
  }
  
  fPt_nClu_Sign->SetMarkerColor(kRed );
  fPt_Peak_Sign->SetMarkerColor(kRed );
  fPt_nHit_Sign->SetMarkerColor(kRed );
  fPt_nClu_Back->SetMarkerColor(kBlue);
  fPt_Peak_Back->SetMarkerColor(kBlue);
  fPt_nHit_Back->SetMarkerColor(kBlue);
  
  TCanvas c;
  gPad->SetLogz();
    
  fPt_nClu_Sign->SetMarkerSize(2);
  fPt_nClu_Back->SetMarkerSize(2);
  fPt_Peak_Sign->SetMarkerSize(2);
  fPt_Peak_Back->SetMarkerSize(2);
  fPt_nHit_Sign->SetMarkerSize(2);
  fPt_nHit_Back->SetMarkerSize(2);
  
  fPt_nClu_Sign->SetTitle(""); fPt_nClu_Sign->SetName("nClu_Sign");
  fPt_nClu_Back->SetTitle(""); fPt_nClu_Back->SetName("nClu_Back");
  fPt_Peak_Sign->SetTitle(""); fPt_Peak_Sign->SetName("Peak_Sign");
  fPt_Peak_Back->SetTitle(""); fPt_Peak_Back->SetName("Peak_Back");
  fPt_nHit_Sign->SetTitle(""); fPt_nHit_Sign->SetName("nHit_Sign");
  fPt_nHit_Back->SetTitle(""); fPt_nHit_Back->SetName("nHit_Back");

  fPt_nClu_Sign->GetXaxis()->SetTitle("number of wire clusters / 1#times2#times6");
  fPt_nClu_Back->GetXaxis()->SetTitle("number of wire clusters / 1#times2#times6");
  fPt_nClu_Sign->GetYaxis()->SetTitle("number of optical clusters / 1#times2#times6");
  fPt_nClu_Back->GetYaxis()->SetTitle("number of optical clusters / 1#times2#times6");

  fPt_Peak_Sign->GetXaxis()->SetTitle("Sum ADC wire cluster / 1#times2#times6");
  fPt_Peak_Back->GetXaxis()->SetTitle("Sum ADC wire cluster / 1#times2#times6");
  fPt_Peak_Sign->GetYaxis()->SetTitle("Sum PE optical cluster / 1#times2#times6");
  fPt_Peak_Back->GetYaxis()->SetTitle("Sum PE optical cluster / 1#times2#times6");

  fPt_nHit_Sign->GetXaxis()->SetTitle("number of hits in wire clusters / 1#times2#times6");
  fPt_nHit_Back->GetXaxis()->SetTitle("number of hits in wire clusters / 1#times2#times6");
  fPt_nHit_Sign->GetYaxis()->SetTitle("number of hits in optical clusters / 1#times2#times6");
  fPt_nHit_Back->GetYaxis()->SetTitle("number of hits in optical clusters / 1#times2#times6");

  fLikelihood_nClu_Sign->GetXaxis()->SetTitle("number of wire clusters / 1#times2#times6");
  fLikelihood_nClu_Back->GetXaxis()->SetTitle("number of wire clusters / 1#times2#times6");
  fLikelihood_nClu_Sign->GetYaxis()->SetTitle("number of optical clusters / 1#times2#times6");
  fLikelihood_nClu_Back->GetYaxis()->SetTitle("number of optical clusters / 1#times2#times6");

  fLikelihood_Peak_Sign->GetXaxis()->SetTitle("Sum ADC wire cluster / 1#times2#times6");
  fLikelihood_Peak_Back->GetXaxis()->SetTitle("Sum ADC wire cluster / 1#times2#times6");
  fLikelihood_Peak_Sign->GetYaxis()->SetTitle("Sum PE optical cluster / 1#times2#times6");
  fLikelihood_Peak_Back->GetYaxis()->SetTitle("Sum PE optical cluster / 1#times2#times6");

  fLikelihood_nHit_Sign->GetXaxis()->SetTitle("number of hits in wire clusters / 1#times2#times6");
  fLikelihood_nHit_Back->GetXaxis()->SetTitle("number of hits in wire clusters / 1#times2#times6");
  fLikelihood_nHit_Sign->GetYaxis()->SetTitle("number of hits in optical clusters / 1#times2#times6");
  fLikelihood_nHit_Back->GetYaxis()->SetTitle("number of hits in optical clusters / 1#times2#times6");

  fPt_nClu_Sign->Draw("AP");
  fPt_nClu_Back->Draw("P");
  c.SaveAs("BiTrigger0.png");
  
  fPt_Peak_Sign->Draw("AP");
  fPt_Peak_Back->Draw("P");
  c.SaveAs("BiTrigger1.png");

  fPt_nHit_Sign->Draw("AP");
  fPt_nHit_Back->Draw("P");
  c.SaveAs("BiTrigger2.png");

  c.Print("BiTrigger.pdf[");
  fLikelihood_nClu_Sign->Draw("COLZ");
  c.Print("BiTrigger.pdf");
  fLikelihood_nClu_Back->Draw("COLZ");
  c.Print("BiTrigger.pdf");

  fLikelihood_Peak_Sign->Draw("COLZ");
  c.Print("BiTrigger.pdf");
  fLikelihood_Peak_Back->Draw("COLZ");
  c.Print("BiTrigger.pdf");
  
  fLikelihood_nHit_Back->Draw("COLZ");
  c.Print("BiTrigger.pdf");
  fLikelihood_nHit_Sign->Draw("COLZ");
  c.Print("BiTrigger.pdf");

  c.Print("BiTrigger.pdf]");


  std::cout << "Tot_WireNCluster_Sign " << fTot_WireNCluster_Sign << std::endl;
  std::cout << "Tot_WireNCluster_Back " << fTot_WireNCluster_Back << std::endl;
  std::cout << "Tot_OptiNCluster_Sign " << fTot_OptiNCluster_Sign << std::endl;
  std::cout << "Tot_OptiNCluster_Back " << fTot_OptiNCluster_Back << std::endl;

};
