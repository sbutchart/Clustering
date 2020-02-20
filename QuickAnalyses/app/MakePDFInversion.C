//____________________________________________________________________ 
//  
// $Id$ 
// Author: Pierre Lasorak <plasorak@pa-241-90.byodstaff.susx.ac.uk>
// Update: 2019-09-06 16:56:36+0100
// Copyright: 2019 (C) Pierre Lasorak
//
//
#ifndef __CINT__
// PUT HEADERS HERE
#endif

int MakePDFInversion()
{
  // DEFINE YOUR MAIN FUNCTION HERE
  TFile* f1 = new TFile("~/Desktop/LikelihoodTrigger.root");
  TFile* f2 = new TFile("ComptonElectron.root");
  TTree* data    = (TTree*)f1->Get("mapping");
  TH1D* spectrum = (TH1D*) f2->Get("elect_maximum_energy");
  TCanvas c;
  c.Print("new_sum_adc.pdf[");
  spectrum->Draw();
  spectrum->Scale(1./spectrum->Integral());
  c.Print("new_sum_adc.pdf");
  double  sumadc=0,elep=0;

  data->SetBranchAddress("ELep",   &elep);
  data->SetBranchAddress("SumADC", &sumadc);
  
  TH1D* spectrum_2 = (TH1D*)spectrum->Clone();

  spectrum_2->Reset();

  for (int i=0; i<data->GetEntries(); ++i) {
    data->GetEntry(i);
    spectrum_2->Fill(elep*1000.);
  }
  spectrum_2->Scale(1./spectrum_2->Integral());
  spectrum_2->Draw();
  c.Print("new_sum_adc.pdf");
  spectrum->Divide(spectrum_2);
  spectrum->Draw();
  c.Print("new_sum_adc.pdf");

  TH1D* sum_adc_spectrum = new TH1D("","SN;SumADC;Count", 20, 0, 200);
  TH1D* sum_adc_spectrum_neutron = new TH1D("neutron","neutron;SumADC;Count", 20, 0, 200);

  for (int i=0; i<data->GetEntries(); ++i) {
    data->GetEntry(i);
    double w = spectrum->GetBinContent(spectrum->FindBin(elep*1000));

    sum_adc_spectrum_neutron->Fill(sumadc, w);
    sum_adc_spectrum        ->Fill(sumadc);
  }
  
  sum_adc_spectrum_neutron->Draw();
  c.Print("new_sum_adc.pdf");
  sum_adc_spectrum->Draw();
  c.Print("new_sum_adc.pdf");

  sum_adc_spectrum_neutron->Scale(1./sum_adc_spectrum_neutron->Integral());
  sum_adc_spectrum        ->Scale(1./sum_adc_spectrum        ->Integral());
  sum_adc_spectrum        ->SetLineColor(kRed);
  
  sum_adc_spectrum_neutron->Draw();
  sum_adc_spectrum        ->Draw("SAME");
  c.Print("new_sum_adc.pdf");
  c.Print("new_sum_adc.pdf]");

  return 0;
}

#ifndef __CINT__
int main(int argc, char** argv)
{
  return MakePDFInversion();
}
#endif

//____________________________________________________________________ 
//  
// EOF
//
