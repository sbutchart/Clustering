#include "TCanvas.h"
#include "TH1D.h"
#include "TColor.h"
#include "TTree.h"
#include "TPad.h"
#include "TFile.h"
#include "TLegend.h"

#include <map>
#include <iostream>
#include <vector>
#include <algorithm>



int main() {

  TFile * f = new TFile("../../SNAna.root", "READ");
  TTree* tree = (TTree*)f->Get("snanagaushit/SNSimTree");
  std::vector<int>    *PDS_OpHit_True_GenType = NULL;
  std::vector<double> *PDS_OpHit_PE = NULL;
  tree->SetBranchAddress("PDS_OpHit_True_GenType", &PDS_OpHit_True_GenType);
  tree->SetBranchAddress("PDS_OpHit_PE", &PDS_OpHit_PE);

  TCanvas c;
  std::map<int,TH1D*> map_PE_Background_histo; std::map<int,double> map_PE_Background;
  std::vector<std::string> legstr = {"Noise",
                                     "SN#nu",
                                     "APA",
                                     "CPA",
                                     "Ar39",
                                     "Neutron",
                                     "Krypton",
                                     "Polonium",
                                     "Radon",
                                     "Ar42"};
  int nmax=2000;
  std::vector<int> color = {TColor::GetColor(  25,  25,  25 ), //ebony
                            TColor::GetColor( 240, 163, 255 ), //amethyst
                            TColor::GetColor( 255,   0,  16 ), //red
                            TColor::GetColor( 128, 128, 128 ), //iron
                            TColor::GetColor(  25, 164,   5 ), //orpiment
                            TColor::GetColor(   0, 153, 143 ), //turquoise
                            TColor::GetColor(   0,  51, 128 ), //navy
                            TColor::GetColor(  94, 241, 242 ), //sky
                            TColor::GetColor( 157, 204,   0 ), //lime
                            TColor::GetColor( 153,  63,   0 ), //caramel
                            TColor::GetColor( 224, 255, 102 ), //uranium
                            TColor::GetColor(  76,   0,  92 ), //damson
                            TColor::GetColor( 255, 168, 187 ), //pink
                            TColor::GetColor( 194,   0, 136 ), //mallow
                            TColor::GetColor( 255, 204, 153 ), //honeydew
                            TColor::GetColor(  43, 206,  72 ), //green
                            TColor::GetColor(   0, 117, 220 ), //blue
                            TColor::GetColor(   0,  92,  49 ), //forest
                            TColor::GetColor( 255, 225,   0 ), //yellow
                            TColor::GetColor( 153,   0,   0 ), //wine
                            TColor::GetColor( 143, 124,   0 ), //khaki
                            TColor::GetColor(  16,  10, 255 ), //violet
                            TColor::GetColor( 255, 255, 128 ), //xanthin
                            TColor::GetColor( 255,  80,   0 ), //zinnia
                            TColor::GetColor( 148, 255, 181 ), //jade
                            TColor::GetColor(  66, 102,   0 )};  //quagmire
  
  for (int i=0; i<10; i++) {
    map_PE_Background_histo[i] = new TH1D(Form("%i_back",i), ";PE;Event", nmax, 0, nmax);
    map_PE_Background_histo[i]->SetLineColor(color[i]);
    map_PE_Background_histo[i]->SetLineWidth(2);
    map_PE_Background_histo[i]->SetStats(0);
  }
  
  for (int i=0; i<tree->GetEntries()/10; i++) {
    if (i%100==0) std::cout << i << " / " << tree->GetEntries() << std::endl;
    tree->GetEntry(i);
    for (int e=0; e<10; e++) {
      map_PE_Background[e]=0;
    }
    for (size_t hit=0; hit<PDS_OpHit_True_GenType->size(); ++hit) {
      map_PE_Background[(*PDS_OpHit_True_GenType)[hit]] += (*PDS_OpHit_PE)[hit];
    }
    for (auto const&it: map_PE_Background) {
      if (it.second>nmax) std::cout << "MAximum exceeded "<<it.second << std::endl;
      map_PE_Background_histo[it.first]->Fill(it.second);
    }
  }

  c.Print("ForJose.pdf[");
  TLegend* leg = new TLegend(0.1,0.1,0.9,0.9);

  std::vector<double> max;
  for (int i=0; i<10; i++) {
    max.push_back(map_PE_Background_histo[i]->GetMaximum());
    leg->AddEntry(map_PE_Background_histo[i], legstr[i].c_str(), "L");
  }
  leg->Draw();
  c.Print("ForJose.pdf[");

  double maxresult = *std::max_element(max.begin(), max.end());
  map_PE_Background_histo[0]->SetMaximum(maxresult*1.2);
  map_PE_Background_histo[0]->Draw();
  gPad->SetLogx();
    gPad->SetLogy();
  for (int i=1; i<10; i++) {
    map_PE_Background_histo[i]->Draw("SAME");
  }
  gPad->RedrawAxis();
  c.Print("ForJose.pdf");
  c.Print("ForJose.pdf]");
  return 1;
  
}
