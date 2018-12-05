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

  // int TotGen_Marl;//1 color/legend code
  // int TotGen_APA;//2
  // int TotGen_CPA;//3
  // int TotGen_Ar39;//4
  // int TotGen_Neut;//5
  // int TotGen_Kryp;//6
  // int TotGen_Plon;//7
  // int TotGen_Rdon;//8
  // int TotGen_Ar42;//9
  int bckgcounter[9];
  int colorequivalence[9]={1,2,3,4,5,6,7,8,9};
  tree->SetBranchAddress("TotGen_Marl", &bckgcounter[0]);
  tree->SetBranchAddress("TotGen_APA", &bckgcounter[1]);
  tree->SetBranchAddress("TotGen_CPA", &bckgcounter[2]);
  tree->SetBranchAddress("TotGen_Ar39", &bckgcounter[3]);
  tree->SetBranchAddress("TotGen_Neut", &bckgcounter[4]);
  tree->SetBranchAddress("TotGen_Kryp", &bckgcounter[5]);
  tree->SetBranchAddress("TotGen_Plon", &bckgcounter[6]);
  tree->SetBranchAddress("TotGen_Rdon", &bckgcounter[7]);
  tree->SetBranchAddress("TotGen_Ar42", &bckgcounter[8]);

  TCanvas c;
  std::map<int,TH1I*> map_PE_Background_histo; std::map<int,double> map_PE_Background;
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
  
  for (int i=0; i<9; i++) {
    map_PE_Background_histo[i] = new TH1I(Form("%i_back",i), ";PE;Event", 50000, 0, 50000);
    map_PE_Background_histo[i]->SetLineColor(color[colorequivalence[i]]);
    map_PE_Background_histo[i]->SetLineWidth(2);
    map_PE_Background_histo[i]->SetStats(0);
  }
  
  for (int i=0; i<tree->GetEntries()/10; i++) {
    if (i%100==0) std::cout << i << " / " << tree->GetEntries() << std::endl;
    tree->GetEntry(i);
    for (int e=0; e<9; e++) {
      map_PE_Background_histo[e]->Fill(bckgcounter[e]); //cout << legstr[colorequivalence[i]] << " "<<bckgcounter[e] << endl;	
    }
  }

  c.Print("ForJose.pdf[");
  TLegend* leg = new TLegend(0.1,0.1,0.9,0.9);

  std::vector<double> max;
  for (int i=0; i<9; i++) {
    max.push_back(map_PE_Background_histo[i]->GetMaximum());
    leg->AddEntry(map_PE_Background_histo[i], legstr[colorequivalence[i]].c_str(), "L");
  }
  leg->Draw();
  c.Print("ForJose.pdf[");

 double maxresult = *std::max_element(max.begin(), max.end());
 map_PE_Background_histo[0]->SetMaximum(maxresult*1.2);
  map_PE_Background_histo[0]->Draw();
  gPad->SetLogx();
    gPad->SetLogy();
  for (int i=1; i<9; i++) {
    map_PE_Background_histo[i]->Draw("SAME");
  }
  gPad->RedrawAxis();
  //leg->Draw("same");
 
  c.Print("ForJose.pdf");
  c.Print("ForJose.pdf]");
  return 1;
  
}
