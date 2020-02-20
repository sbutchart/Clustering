//____________________________________________________________________ 
//  
// $Id$ 
// Author: Pierre Lasorak <plasorak@pa-240-81.byodstaff.susx.ac.uk>
// Update: 2019-11-14 10:48:19+0000
// Copyright: 2019 (C) Pierre Lasorak
//
//
#ifndef __CINT__
// PUT HEADERS HERE
#include <chrono>
#include <cmath>
#include <iostream>
#include "TGraph.h"
#include "TFile.h"
#endif

TGraph* gr_sin;
TGraph* gr_cos;

double factorial(const double f) {
  if (f==1 || f==0) {
    return 1;
  }
  return factorial(f-1)*f;
}

void sincos(const double x_large_no_2pi, double& sin, double& cos, int max_rank=10) {
  double interval = x_large_no_2pi - floor(x_large_no_2pi); // this is between 0->1 
  double x = interval * 6.2831853072;
  double x_sq = x*x;
  sin = 0;
  cos = 0;
  double x_2n = 1;

  for (int rank=0; rank<=max_rank; ++rank) {
    int minus_one=-1;
    if (rank%2==0) minus_one=1;
    if (rank>=1) x_2n = x_2n * x_sq;
    double fact = factorial(2*rank);
    sin += minus_one * x*x_2n / fact * ((double)(2*rank+1));
    cos += minus_one *   x_2n / fact;
  }

}

int Cos() {
  double x = 0.25/2;
  double two_pi = 6.2831853072;
  
  auto start = std::chrono::steady_clock::now();
  double sin_r = sin(x*two_pi);
  double cos_r = cos(x*two_pi);
  auto end = std::chrono::steady_clock::now();
  std::cout << "It takes " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns\t";
  std::cout << "sin=" << sin_r << " cos=" << cos_r << "\n\n";

  std::vector<int> npts{100};
  TFile f("Graphs.root", "RECREATE");
  for (auto const& npt: npts) {
    const int nptc = 100;
    double xs[nptc], cos_s[nptc], sin_s[nptc];
    for (int ipt=0; ipt<npt; ++ipt) {
      xs   [ipt] = 6.2831853072 * (double)ipt / npt;
      sin_s[ipt] = sin(xs[ipt]);
      cos_s[ipt] = cos(xs[ipt]);
      // double x_gr = 6.2831853072 * (double)ipt / npt;
      // gr_sin->SetPoint(ipt, x_gr, sin(x_gr));
      // gr_cos->SetPoint(ipt, x_gr, cos(x_gr));
    }
    gr_sin = new TGraph(npt, xs, sin_s);
    gr_cos = new TGraph(npt, xs, cos_s);
    
    auto start = std::chrono::steady_clock::now();
    double sin_gr = gr_sin->Eval(x*two_pi);
    double cos_gr = gr_cos->Eval(x*two_pi);
    auto end = std::chrono::steady_clock::now();
    std::cout << "It takes " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns with " << npt <<" points.\n";
    std::cout << "sin=" << sin_gr << " cos=" << cos_gr << "\t\t\t";
    std::cout << "Errors: sin=" << sin_r - sin_gr << " cos=" << cos_r - cos_gr << "\n";
    gr_sin->SetLineColor(kBlack);
    gr_cos->SetLineColor(kBlack);
    gr_sin->Write(Form("gr_sin_%i", npt));
    gr_cos->Write(Form("gr_cos_%i", npt));
  }
  f.Close();
  std::cout <<"\n";
  for (int rank=0; rank<=20; ++rank) {
    start = std::chrono::steady_clock::now();
    double sin_t = 0;
    double cos_t = 0;
    sincos(1, sin_t, cos_t, rank);
    end = std::chrono::steady_clock::now();
    std::cout << "It takes " << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << " ns with " << rank <<" iterations.\n";
    std::cout << "sin=" << sin_t << " cos=" << cos_t << "\t\t\t";
    std::cout << "Errors: sin=" << sin_r - sin_t << " cos=" << cos_r - cos_t << "\n";
  }
  
  return 0;
}

#ifndef __CINT__
int main(int, char**)
{
  return Cos();
}
#endif

//____________________________________________________________________ 
//  
// EOF
//
