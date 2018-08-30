#include "BiTrigger.hh"


void BiTrigger::AddEventToStudy(const std::vector<std::unique_ptr<WireCluster>>& wc,
                                const std::vector<std::unique_ptr<OpticalCluster>>& oc) {
  std::map<size_t,std::vector<ClusterEnsemble>> apa_cluster_sign;
  std::map<size_t,std::vector<ClusterEnsemble>> apa_cluster_back;

  for (int iAPA=0; iAPA<12; ++iAPA) {
    apa_cluster_sign[iAPA].push_back(ClusterEnsemble());
    apa_cluster_back[iAPA].push_back(ClusterEnsemble());
  }
  
  for (auto const& it: wc) {
    if (it->GetType() == 1) {
      apa_cluster_sign[it->GetAPA()].back().AddCluster(*it);
    } else {
      apa_cluster_back[it->GetAPA()].back().AddCluster(*it);
    }
  }

  for (auto const& it: oc) {
    if (it->GetType() == 1) {
      apa_cluster_sign[it->GetAPA()].back().AddCluster(*it);
    } else {
      apa_cluster_back[it->GetAPA()].back().AddCluster(*it);
    }
  }

  for (auto const& it: apa_cluster_sign) {
    for (auto const& it_sign: it.second)
      fSignCluster.push_back(it_sign);
    for (auto const& it_back: it.second)
      fBackCluster.push_back(it_back);
  }
};

void BiTrigger::FinishStudy() {
  fPt_nClu_Sign = new TGraph(fSignCluster.size());
  fPt_nClu_Back = new TGraph(fBackCluster.size());
  fPt_Peak_Sign = new TGraph(fSignCluster.size());
  fPt_Peak_Back = new TGraph(fBackCluster.size());
  fPt_nClu_Sign->SetTitle(""); fPt_nClu_Sign->SetName("nClu_Sign");
  fPt_nClu_Back->SetTitle(""); fPt_nClu_Back->SetName("nClu_Back");
  fPt_Peak_Sign->SetTitle(""); fPt_Peak_Sign->SetName("Peak_Sign");
  fPt_Peak_Back->SetTitle(""); fPt_Peak_Back->SetName("Peak_Back");

  fLikelihood_nClu_Sign = new TH2D("Likelihood_nClu_Sign", "", 100, 0, 100, 100, 0, 100);
  fLikelihood_nClu_Back = new TH2D("Likelihood_nClu_Back", "", 100, 0, 100, 100, 0, 100);
  fLikelihood_Peak_Sign = new TH2D("Likelihood_Peak_Sign", "", 100, 0, 1000, 100, 0, 1000);
  fLikelihood_Peak_Back = new TH2D("Likelihood_Peak_Back", "", 100, 0, 1000, 100, 0, 1000);

  int i=0;
  for (auto const& it: fSignCluster) {
    fPt_nClu_Sign->SetPoint(i,  it.nWireCluster   , it.nOpticalCluster   );
    fPt_Peak_Sign->SetPoint(i,  it.PeakWireCluster, it.PeakOpticalCluster);
    fLikelihood_nClu_Sign->Fill(it.nWireCluster   , it.nOpticalCluster   );
    fLikelihood_Peak_Sign->Fill(it.PeakWireCluster, it.PeakOpticalCluster);
    ++i;
  }
  
  i=0;
  for (auto const& it: fBackCluster) {
    fPt_nClu_Back->SetPoint(i,  it.nWireCluster   , it.nOpticalCluster   );
    fPt_Peak_Back->SetPoint(i,  it.PeakWireCluster, it.PeakOpticalCluster);
    fLikelihood_nClu_Back->Fill(it.nWireCluster   , it.nOpticalCluster   );
    fLikelihood_Peak_Back->Fill(it.PeakWireCluster, it.PeakOpticalCluster);
    ++i;
  }
  
  fPt_nClu_Sign->SetMarkerColor(kRed );
  fPt_Peak_Sign->SetMarkerColor(kRed );
  fPt_nClu_Back->SetMarkerColor(kBlue);
  fPt_Peak_Back->SetMarkerColor(kBlue);

  TCanvas c;
  c.Print("BiTrigger.pdf[");
  fPt_nClu_Sign->Draw("AP");
  fPt_nClu_Back->Draw("P");
  c.Print("BiTrigger.pdf");
  
  fPt_Peak_Sign->Draw("AP");
  fPt_Peak_Back->Draw("P");
  c.Print("BiTrigger.pdf");
  c.Print("BiTrigger.pdf]");
  
};
