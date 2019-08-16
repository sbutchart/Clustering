//____________________________________________________________________ 
//  
// $Id$ 
// Author: Pierre Lasorak <plasorak@pa-241-176.byodstaff.susx.ac.uk>
// Update: 2019-05-02 10:12:52+0100
// Copyright: 2019 (C) Pierre Lasorak
//
//
#ifndef __CINT__
// PUT HEADERS HERE
#endif

int TestBackgroundRates()
{
  gErrorIgnoreLevel = kWarning;
  // DEFINE YOUR MAIN FUNCTION HERE
  TFile* f0 = new TFile("../../build/5x_radio_pdfs.root", "READ");
  TFile* f1 = new TFile("../../build/signal_radio_pdfs.root", "READ");

  TCanvas c1, c2;
  c1.Print("TestBackgroundRates.pdf[");
  c2.Print("TestBackgroundRatesIndividual.pdf[");
  
  for (int i=0; i<48; ++i) {
    std::string name = "PDF_Background_config"+std::to_string(i);
    TH1D* radio        = (TH1D*)f0->Get(name.c_str());
    TH1D* signal_radio = (TH1D*)f1->Get(name.c_str());

    radio       ->Scale(1./0.12);
    signal_radio->Scale(1./0.12);
    // std::cout << radio       ->GetEntries() << "\n";
    std::cout << radio       ->Integral() << " +/- " << radio       ->Integral() / sqrt((double)radio       ->GetEntries()) << "\t"
              << signal_radio->Integral() << " +/- " << signal_radio->Integral() / sqrt((double)signal_radio->GetEntries()) << "\n";
    double max1 = radio->GetMaximum();
    double max2 = signal_radio->GetMaximum();
    radio->SetMaximum(1.2 * max(max1, max2));
    radio->SetMinimum(0);
    radio->SetLineColor(kBlue);
    signal_radio->SetLineColor(kRed);
    c1.cd();
    radio->Draw();
    signal_radio->Draw("SAME");
    c1.Print("TestBackgroundRates.pdf");
    
    for (int back=0; back<10; ++back) {
      std::string name = "PDF_Background_"+std::to_string(back)+"_config"+std::to_string(i);
      TH1D* inradio        = (TH1D*)f0->Get(name.c_str());
      TH1D* insignal_radio = (TH1D*)f1->Get(name.c_str());
      inradio       ->Scale(1./0.12);
      insignal_radio->Scale(1./0.12);
      std::cout << "\t" << back << "\t";
      std::cout << inradio       ->Integral() << " +/- " << inradio       ->Integral() / sqrt((double)inradio       ->GetEntries()) << "\t"
                << insignal_radio->Integral() << " +/- " << insignal_radio->Integral() / sqrt((double)insignal_radio->GetEntries()) << "\n";
      double max1 = inradio->GetMaximum();
      double max2 = insignal_radio->GetMaximum();
      inradio->SetMaximum(1.2 * max(max1, max2));
      inradio->SetMinimum(0);
      inradio->SetLineColor(kBlue);
      insignal_radio->SetLineColor(kRed);
      c2.cd();
      inradio->Draw();
      insignal_radio->Draw("SAME");
      c2.Print("TestBackgroundRatesIndividual.pdf");
    }
  }
  
  c1.Print("TestBackgroundRates.pdf]");
  c2.Print("TestBackgroundRatesIndividual.pdf]");
  f0->Close();
  f1->Close();

  return 0;
}

#ifndef __CINT__
int main(int argc, char** argv)
{
  int retVal = TestBackgroundRates();
  return retVal;
}
#endif

//____________________________________________________________________ 
//  
// EOF
//
