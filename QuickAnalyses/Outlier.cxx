#include "TFile.h"
#include "TCanvas.h"
#include "TH1D.h"
#include "TRandom.h"
#include "TRandom3.h"
#include "TGraph.h"
#include "Helper.h"
#include "TVector3.h"
#include "TMath.h"
//#include <numeric>
#include <iostream>
#include <thread>
#include <functional>
#include <atomic>
#include <random>
#include <time.h>

#include "CLI11.hpp"

struct clust {
  double adc;
  double nhit;
  double timep;
};

double GetDistance(std::vector<double> one, std::vector<double> two) {
  if (one.size() == two.size())
    return -1;
  
  double d = 0;
  for (size_t it=0; it<one.size(); ++it) {
    d += (one[it] - two[it]) * (one[it] - two[it]);
  }
  return sqrt(d);
}

std::vector<double> GetDistances(std::vector<clust> v) {
  std::vector<clust> vect = v;
  std::vector<double> distances;
  
  while (vect.size() >= 2) {
    auto& last = vect.rbegin();
    clust c = *last;
    for (auto const it=vect.begin(); it!=last; ++it) {
      std::vector<double> one = {it.adc, it.nhit, it.timep};
      std::vector<double> two = {c .adc, c .nhit, c .timep};
      distances.push_back(GetDistance(one, two));
    }
    vect.erase(last, last+1);
  }
}


int main(int argc, char *argv[]) {

  CLI::App app{"Optimiser"};

  std::string filename = "";
  int nThread = 4;
  std::string outputfilename = "Cluster.pdf";
  app.add_option("-f,--file", filename, "Input filename");
  app.add_option("-t,--thread", nThread, "Number of thread this will run on");
  app.add_option("-p,--param", param, "Parameter name");
  app.add_option("-o,--output", outputfilename, "Output file name");

  CLI11_PARSE(app, argc, argv);

  gRandom = new TRandom3();

  std::vector<KS> KSs;
  std::vector<std::thread> threads;
  TFile* f = new TFile(filename.c_str(), "READ");
  TH1D* back_pdf = (TH1D*)f->Get((std::string("h_")+param+"_back_wire").c_str());
  TH1D* sign_pdf = (TH1D*)f->Get((std::string("h_")+param+"_sign_wire").c_str());
  TVector3* Efficiency = (TVector3*)f->Get("Efficiency");
  TVector3* BackgroundRate = (TVector3*)f->Get("BackgroundRate");

  AddOverflow(back_pdf);
  AddOverflow(sign_pdf);
 
  double BackgroundEvent = BackgroundRate->X() * 10.;
  back_pdf->Scale(back_pdf->GetEntries() / back_pdf->Integral());
  sign_pdf->Scale(sign_pdf->GetEntries() / sign_pdf->Integral());
  double SignalEvent = 10 * Efficiency->X();
  std::cout << "BackgroundEvent " << BackgroundEvent << std::endl;
  std::cout << "Entries " << back_pdf->GetEntries() << std::endl;

  int nToys = (double)back_pdf->GetEntries() / 10. / (double) nThread;
  if (nToys == 0) {
    nToys = (double)back_pdf->GetEntries() / 10.;
    nThread = 1;
  }
  if (nToys < 10) {
    std::cout << "not enough stats to make toys!" << std::endl;
    exit(0);
  }
  
  for (int iThread=0; iThread<nThread; ++iThread) {
    KSs.push_back(KS());
    KSs.back().UniqueID = iThread;
    KSs.back().SetSeed((iThread+1)*2968);
    KSs.back().SetBackground(back_pdf);
    KSs.back().SetSignal    (sign_pdf);
    KSs.back().SetNBackgroundEvent(BackgroundEvent);
    KSs.back().SetNSignalEvent    (SignalEvent);
    KSs.back().SetNToys(nToys);
  }

  for (std::vector<KS>::iterator it = KSs.begin(); it != KSs.end(); ++it) {
    threads.push_back(std::thread(&KS::GetKSStatROOT, it));
  }

  std::cout << "threads setup" << std::endl;
  for (auto& it: threads) {
    it.join();
  }
  double ntoys=0;
  for (auto const& k: KSs) {
    std::cout << "once thread is done " << k.SignalKS.size() << std::endl;
    std::cout << "once thread is done " << k.BackgroundKS.size() << std::endl;
    for (auto const& it: k.SignalKS) {
      KSTest_sign->Fill(it);
      ntoys++;
    }
    for (auto const& it: k.BackgroundKS) {
      KSTest_back->Fill(it);
    }
  }
  KSTest_sign->Scale(0.1 /ntoys);
  KSTest_back->Scale(0.1 /ntoys);
  
  KSTest_sign->SetBinContent(100, KSTest_sign->GetBinContent(100) +  KSTest_sign->GetBinContent(101));
  KSTest_back->SetBinContent(100, KSTest_back->GetBinContent(100) +  KSTest_back->GetBinContent(101));

  KSTest_sign->SetBinContent(101, 0);
  KSTest_back->SetBinContent(101, 0);
  std::vector<double> max = {KSTest_sign->GetMaximum(),KSTest_back->GetMaximum()};
  std::vector<double> min = {KSTest_sign->GetMinimum(),KSTest_back->GetMinimum()};
  
  TCanvas c;
  gPad->SetLogy();
  
  //KSTest_sign->SetMinimum(1.e-1 *);
  KSTest_sign->SetMaximum(10. * (*std::max_element(max.begin(), max.end())));
  c.SaveAs((outputfilename+"[").c_str());
  KSTest_sign->Draw();
  KSTest_back->Draw("SAME");
  c.SaveAs(outputfilename.c_str());

  TGraph* gr = new TGraph(KSTest_sign->GetNbinsX());
  for (int i=0; i<gr->GetN(); i++) {
    double eff = KSTest_sign->Integral(0, i) * 0.791258 / KSTest_sign->Integral();
    double background = KSTest_back->Integral(0, i);
    gr->SetPoint(i, background,eff);
  }
  gPad->SetLogy(0);
  gPad->SetLogx(1);

  gr->SetTitle(";Fake rate [Hz];Efficiency");
  gr->Draw("ALP");
  
  c.SaveAs(outputfilename.c_str());
  c.SaveAs((outputfilename+"]").c_str());


  TFile f2((outputfilename+".root").c_str(), "RECREATE");
  gr->Write();
  KSTest_sign->Write();
  KSTest_back->Write();
  f2.Close();
  
  return 0;
  
}
