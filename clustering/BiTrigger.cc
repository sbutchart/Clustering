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
  fLikelihood_nClu_Sign = new TH2D("Likelihood_nClu_Sign", "", 10, 0, 10,  10, 0, 10);
  fLikelihood_nClu_Back = new TH2D("Likelihood_nClu_Back", "", 10, 0, 10,  10, 0, 10);
  fLikelihood_Peak_Sign = new TH2D("Likelihood_Peak_Sign", "", 100, 0, 1000, 100, 0, 1000);
  fLikelihood_Peak_Back = new TH2D("Likelihood_Peak_Back", "", 100, 0, 1000, 100, 0, 1000);

  
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
  fPt_nClu_Sign->SetMarkerSize(2);
  fPt_nClu_Back->SetMarkerSize(2);
  fPt_Peak_Sign->SetMarkerSize(2);
  fPt_Peak_Back->SetMarkerSize(2);
  fPt_nClu_Sign->SetTitle(""); fPt_nClu_Sign->SetName("nClu_Sign");
  fPt_nClu_Back->SetTitle(""); fPt_nClu_Back->SetName("nClu_Back");
  fPt_Peak_Sign->SetTitle(""); fPt_Peak_Sign->SetName("Peak_Sign");
  fPt_Peak_Back->SetTitle(""); fPt_Peak_Back->SetName("Peak_Back");

  fPt_nClu_Sign->GetXaxis()->SetTitle("number of wire clusters / APA");
  fPt_nClu_Back->GetXaxis()->SetTitle("number of wire clusters / APA");
  fPt_nClu_Sign->GetYaxis()->SetTitle("number of optical clusters / APA");
  fPt_nClu_Back->GetYaxis()->SetTitle("number of optical clusters / APA");

  fPt_Peak_Sign->GetXaxis()->SetTitle("Sum ADC wire cluster / APA");
  fPt_Peak_Back->GetXaxis()->SetTitle("Sum ADC wire cluster / APA");
  fPt_Peak_Sign->GetYaxis()->SetTitle("Sum PE optical cluster / APA");
  fPt_Peak_Back->GetYaxis()->SetTitle("Sum PE optical cluster / APA");


  fLikelihood_nClu_Sign->GetXaxis()->SetTitle("number of wire clusters / APA");
  fLikelihood_nClu_Back->GetXaxis()->SetTitle("number of wire clusters / APA");
  fLikelihood_nClu_Sign->GetYaxis()->SetTitle("number of optical clusters / APA");
  fLikelihood_nClu_Back->GetYaxis()->SetTitle("number of optical clusters / APA");
  fLikelihood_Peak_Sign->GetXaxis()->SetTitle("Sum ADC wire cluster / APA");
  fLikelihood_Peak_Back->GetXaxis()->SetTitle("Sum ADC wire cluster / APA");
  fLikelihood_Peak_Sign->GetYaxis()->SetTitle("Sum PE optical cluster / APA");
  fLikelihood_Peak_Back->GetYaxis()->SetTitle("Sum PE optical cluster / APA");
  fPt_nClu_Sign->Draw("AP");
  fPt_nClu_Back->Draw("P");
  c.SaveAs("BiTrigger0.png");
  
  fPt_Peak_Sign->Draw("AP");
  fPt_Peak_Back->Draw("P");
  c.SaveAs("BiTrigger1.png");

  fLikelihood_nClu_Sign->Draw("COLZ");
  c.SaveAs("BiTrigger2.png");

  fLikelihood_nClu_Back->Draw("COLZ");
  c.SaveAs("BiTrigger3.png");

  fLikelihood_Peak_Sign->Draw("COLZ");
  c.SaveAs("BiTrigger4.png");
  fLikelihood_Peak_Back->Draw("COLZ");
  c.SaveAs("BiTrigger5.png");
  
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
