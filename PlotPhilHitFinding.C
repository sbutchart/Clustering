#include <algorithm>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <vector>

#include "TCanvas.h"
#include "TLegend.h"
#include "TGraphErrors.h"

#include "Helper.h"

struct simpledata {
  double eff      ;
  double efferror ;
  double back     ;
  double backerror;
  int    config   ;
  int    threshold;
  simpledata():
    eff      (0),
    efferror (0),
    back     (0),
    backerror(0),
    config   (0),
    threshold(0){ };
};

void SetMaximum(std::map<int,TGraphErrors*> &tg) {
  double max = std::numeric_limits<double>::lowest();
  for (auto const& it: tg) {
    for (int i=0; i<it.second->GetN(); ++i) {
      double x, y;
      it.second->GetPoint(i,x,y);
      if (y > max) {
        max = y;
      }
    }
  }
  std::cout << max << std::endl;
  for (auto& it: tg) {
    it.second->SetMaximum(max*1.2);
  }
  
};

void SetMinimum(std::map<int,TGraphErrors*> &tg) {
  double min = std::numeric_limits<double>::max();
  for (auto const& it: tg) {
    for (int i=0; i<it.second->GetN(); ++i) {
      double x, y;
      it.second->GetPoint(i,x,y);
      if (y < min && y>0.000000005) {
        min = y;
      }
    }
  }
  std::cout << min << std::endl;
  for (auto& it: tg) {
    it.second->SetMinimum(min*0.5);
  }
  
};


int main() {
  std::map<int,std::ifstream> input;
  std::set<int> threshold = {800,1000,1200,1400,1600,1800,2000,2400};
  std::set<int> configs = {0,1,2,3,4,5};
  for (auto const& it: threshold)
    input[it].open(Form("../snanatrigprim%i/trigprim.txt", it));
  std::vector<simpledata> data;
  std::map<int,TGraphErrors*> back_tr;
  std::map<int,TGraphErrors*> eff_tr;

  std::map<int,TGraphErrors*> back_con;
  std::map<int,TGraphErrors*> eff_con;

  for (auto& it: input) {
    std::cout << "Trying threshold " << it.first << std::endl;
    int threshold, config;
    double eff, efferror, back, backerror;
    char delim0,delim1,delim2,delim3,delim4;
    while(it.second >>
          config >> delim1 >> delim1 >>
          eff >> delim2 >> delim2 >>
          efferror >> delim3 >>delim3 >>
          back >> delim3 >>delim3 >>
          backerror){
      simpledata d;
      d.threshold = it.first ;
      d.config    = config   ;
      d.eff       = eff      ;
      d.efferror  = efferror ;
      d.back      = back     ;
      d.backerror = backerror;
      data.push_back(d);
    }
    
  }
  
  for (auto const& it: threshold) {
    back_con[it] = new TGraphErrors(6);
    eff_con [it] = new TGraphErrors(6);
    for (auto const& d: data) {
      if (d.threshold == it) {
        back_con[it]->SetPoint(d.config, d.config, d.back);
        eff_con [it]->SetPoint(d.config, d.config, d.eff);
        back_con[it]->SetPointError(d.config, 0.5, d.backerror);
        eff_con [it]->SetPointError(d.config, 0.5, d.efferror);
      }
    }
  }
 
  for (auto const& it: configs) {
    back_tr[it] = new TGraphErrors(8);
    eff_tr [it] = new TGraphErrors(8);
    int i=0;
    for (auto const& d: data) {
      if (d.config == it) {
        std::cout << " d.config " << d.config
                  << " d.threshold " << d.threshold
                  << " d.back " << d.back << std::endl;
        back_tr[it]->SetPoint(i, d.threshold, d.back);
        eff_tr [it]->SetPoint(i, d.threshold, d.eff);
        back_tr[it]->SetPointError(i, 100, d.backerror);
        eff_tr [it]->SetPointError(i, 100, d.efferror);
        ++i;
      }
    }
  }
  std::vector<int> vec_Colors = getColors(0);
  TCanvas c;
  c.Print("Threshold_studies.pdf[");

  SetMaximum(back_con);
  SetMaximum(back_tr);
  SetMaximum(eff_con);
  SetMaximum(eff_tr);

  SetMinimum(back_con);
  SetMinimum(back_tr);
  SetMinimum(eff_con);
  SetMinimum(eff_tr);

  gPad->SetLogy(true);
  int i=0;
  for (auto& it: back_con) {
    int color = vec_Colors.at(i);
    it.second->SetLineColor(color);
    it.second->SetLineWidth(2);
    it.second->GetXaxis()->SetTitle("Cluster configuration");
    it.second->GetYaxis()->SetTitle("Background rate");
    if (i==0) { it.second->Draw("AP"); }
    else      { it.second->Draw("P");  }
    ++i;
  }
  c.Print("Threshold_studies.pdf");

  i=0;
  for (auto& it: back_tr) {
    int color = vec_Colors.at(i);
    it.second->SetLineColor(color);
    it.second->SetLineWidth(2);
    it.second->GetXaxis()->SetTitle("Threshold");
    it.second->GetYaxis()->SetTitle("Background rate");
    if (i==0) { it.second->Draw("AP"); }
    else      { it.second->Draw("P");  }
    ++i;
  }
  c.Print("Threshold_studies.pdf");
  gPad->SetLogy(false);

  i=0;
  for (auto& it: eff_con) {
    int color = vec_Colors.at(i);
    it.second->SetLineColor(color);
    it.second->SetLineWidth(2);
    it.second->GetXaxis()->SetTitle("Cluster configuration");
    it.second->GetYaxis()->SetTitle("Efficiency");
    if (i==0) { it.second->Draw("AP"); }
    else      { it.second->Draw("P");  }
    ++i;
  }
  c.Print("Threshold_studies.pdf");

  i=0;
  for (auto& it: eff_tr) {
    int color = vec_Colors.at(i);
    it.second->SetLineColor(color);
    it.second->SetLineWidth(2);
    it.second->GetXaxis()->SetTitle("Threshold");
    it.second->GetYaxis()->SetTitle("Efficiency");
    if (i==0) { it.second->Draw("AP"); }
    else      { it.second->Draw("P");  }
    ++i;
  }
  c.Print("Threshold_studies.pdf");

  gPad->Clear();
  
  TLegend* leg_con = new TLegend(0.1,0.1,0.9,0.9);
  for (auto const& it: eff_con) {
    leg_con->AddEntry(it.second, Form("Threshold %i",it.first));
  }
  leg_con->Draw();
  c.Print("Threshold_studies.pdf");
  TLegend* leg_tr = new TLegend(0.1,0.1,0.9,0.9);
  for (auto const& it: eff_tr) {
    leg_tr->AddEntry(it.second, Form("Cluster configuration %i",it.first));
  }
  leg_tr->Draw();
  c.Print("Threshold_studies.pdf");
  c.Print("Threshold_studies.pdf]");
  
  return 1; 
};



