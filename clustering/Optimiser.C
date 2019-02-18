//____________________________________________________________________ 
//  
// $Id$ 
// Author: Pierre Lasorak <plasorak@Pierres-MacBook-Pro.local>
// Update: 2019-02-14 15:38:26+0000
// Copyright: 2019 (C) Pierre Lasorak
//
//
#ifndef __CINT__
// PUT HEADERS HERE
#endif

int Optimiser()
{
  // DEFINE YOUR MAIN FUNCTION HERE
  TFile* f = new TFile("~/Desktop/SmallTest.pdf.root", "READ");
  TH1D* h_nhit_back_wire = (TH1D*)f->Get("h_nhit_back_wire");
  TH1D* h_nhit_sign_wire = (TH1D*)f->Get("h_nhit_sign_wire");
  for (int)
  

  for (int i=0; i<h_nhit_back_wire->GetNbinsX(); ++i) {
    double rate = h_nhit_back_wire->Integral(i, h_nhit_back_wire->GetNbinsX());
    std::cout << "Cut " << h_nhit_back_wire->GetBinCenter(i) << " " << rate << std::endl;
  }
  
  return 0;
}

#ifndef __CINT__
int main(int argc, char** argv)
{
  return Optimiser();
}
#endif

//____________________________________________________________________ 
//  
// EOF
//
//____________________________________________________________________ 
//  
