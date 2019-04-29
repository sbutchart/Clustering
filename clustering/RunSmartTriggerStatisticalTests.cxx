//____________________________________________________________________ 
//  
// $Id$ 
// Author: Pierre Lasorak <plasorak@Pierres-MacBook-Pro.local>
// Update: 2019-02-14 15:38:26+0000
// Copyright: 2019 (C) Pierre Lasorak
//
//
#include "Helper.h"
#include "CLI11.hpp"

#include "TCanvas.h"
#include "TColor.h"
#include "TFile.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TLegend.h"
#include <TMath.h>
#include "TPad.h"
#include "TRandom3.h"
#include "TText.h"
#include "TVectorT.h"

#include <chrono>
#include <iostream>
#include <vector>

//#include <gperftools/profiler.h>

using namespace std;

TCanvas c;
string PDFFileName;
string ROOTFileName;

vector<pair<TH1D*,TH1D*>> PlotHistos(vector<pair<vector<double>,vector<double>>> vec_to_plot,
                                     vector<string> titles,
                                     vector<string> names,
                                     vector<string> xaxis) {

  vector<int> ColorBank {kBlack};
  vector<pair<TH1D*,TH1D*>> histo_to_plot;

  for (auto const& it: vec_to_plot) {
    if (it.first.size()==0) continue;
    double max1 = *max_element(it.first.begin(), it.first.end());
    double min1 = *min_element(it.first.begin(), it.first.end());
    //double max2 = *max_element(it.second.begin(), it.second.end());
    double min2 = *min_element(it.second.begin(), it.second.end());

    pair<TH1D*,TH1D*> histo_pair = make_pair(new TH1D("", "", 50, min(min1, min2)*0.8, max1*1.2),
                                             new TH1D("", "", 50, min(min1, min2)*0.8, max1*1.2));
    for (auto const& it2: it.first) {
      histo_pair.first->Fill(it2);
    }
    for (auto const& it2: it.second) {
      histo_pair.second->Fill(it2);
    }
    AddOverflow(histo_pair.second);
    histo_to_plot.push_back(histo_pair);
  }

  
  for (size_t it=0; it<histo_to_plot.size(); ++it) {
    string title = titles[it]+";"+xaxis[it]+";nToys";
    TLegend* leg = new TLegend(0.6, 0.7, 0.9, 0.9);

    histo_to_plot[it].first ->SetLineColor(ColorBank[it%ColorBank.size()]);
    histo_to_plot[it].second->SetLineColor(ColorBank[it%ColorBank.size()]);
    histo_to_plot[it].first ->SetLineStyle(1);
    histo_to_plot[it].second->SetLineStyle(2);
    
    histo_to_plot[it].first->SetMaximum(10.*max(histo_to_plot[it].first->GetMaximum(), histo_to_plot[it].second->GetMaximum()));
    histo_to_plot[it].first->SetMinimum(0.8*min(histo_to_plot[it].first->GetMinimum(0.0), histo_to_plot[it].second->GetMinimum(0.0)));
    histo_to_plot[it].first ->SetTitle(title.c_str());
    histo_to_plot[it].second->SetTitle(title.c_str());
    histo_to_plot[it].first ->SetName((names[it]+"_back"    ).c_str());
    histo_to_plot[it].second->SetName((names[it]+"_backsign").c_str());
    histo_to_plot[it].first->SetStats(0);
    histo_to_plot[it].first ->Draw();
    histo_to_plot[it].second->Draw("SAME");
    leg->AddEntry(histo_to_plot[it].first,  "Background");
    leg->AddEntry(histo_to_plot[it].second, "Signal+Background");
    leg->Draw();
    gPad->SetLogy(true);
    c.Print(PDFFileName.c_str());
    gPad->SetLogy(false);
    delete leg;

  }

  return histo_to_plot;
}

void AssertGoodHistos(TH1D* test, TH1D* pdf) {
  if (test->GetNcells() != pdf->GetNcells()) {
    cerr << "Bins are different for test and pdf\n";
    exit(1);
  }
}

double GetBinomialTest(TH1D* test, TH1D* pdf) {
  int ndetected = floor(test->Integral());
  int nexpected = ceil(pdf ->Integral());
  double binomial = TMath::Factorial(ndetected) / (TMath::Factorial(nexpected)*TMath::Factorial(ndetected-nexpected));
  if (binomial != binomial) binomial = 0;
  return binomial * TMath::Power(0.5,ndetected);
}


double GetGeometricalTest(TH1D* test, TH1D* pdf) {
  
  TH1D* test_shape = NULL;
  TH1D* pdf_shape  = NULL;
  
  if(test->Integral()>0) {
    test_shape = (TH1D*)test->Clone();
    pdf_shape  = (TH1D*)pdf ->Clone();
    test_shape->Scale(1./test_shape->Integral());
    pdf_shape ->Scale(1./pdf_shape ->Integral());
  } else {
    return 1;
  }
  
  double mean = 0;
  for (int iBin=1; iBin<test->GetNcells(); ++iBin) {
    mean+=test_shape->GetBinContent(iBin) * pdf_shape->GetBinContent(iBin);
  }

  delete test_shape;
  delete pdf_shape;
  return TMath::Sqrt(mean);
}

double GetKSStat(TH1D* test, TH1D* pdf) {

  AssertGoodHistos(test,pdf);
  
  TH1D* test_shape = NULL;
  TH1D* pdf_shape  = NULL;
  
  if(test->Integral()>0) {
    test_shape = (TH1D*)test->Clone();
    pdf_shape  = (TH1D*)pdf ->Clone();
    test_shape->Scale(1./test_shape->Integral());
    pdf_shape ->Scale(1./pdf_shape ->Integral());
  } else {
    return 1;
  }

  vector<double> spread;
  for (int iBin=0; iBin<=test_shape->GetNcells(); ++iBin) {
    spread.push_back(abs(test_shape->Integral(1,iBin)-pdf_shape ->Integral(1,iBin)));
  }
  
  delete test_shape;
  delete pdf_shape;
  return *max_element(spread.begin(), spread.end());
}

double GetCVMStat(TH1D* test, TH1D* pdf) {

  AssertGoodHistos(test,pdf);
  
  TH1D* test_shape = NULL;
  TH1D* pdf_shape  = NULL;
  
  if(test->Integral()>0) {
    test_shape = (TH1D*)test->Clone();
    pdf_shape  = (TH1D*)pdf ->Clone();
    test_shape->Scale(1. / test_shape->Integral());
    pdf_shape ->Scale(1. / pdf_shape ->Integral());
  } else {
    return 0;
  }

  TH1D* test_cdf = (TH1D*)test_shape->Clone();
  TH1D* pdf_cdf  = (TH1D*)pdf_shape ->Clone();

  vector<double> spread;
  for (int iBin=0; iBin<=test_shape->GetNcells(); ++iBin) {
    test_cdf->SetBinContent(iBin, test_shape->Integral(1,iBin));
    pdf_cdf ->SetBinContent(iBin, pdf_shape ->Integral(1,iBin));
  }

  double cvm=0;
  for (int iBin=0; iBin<=test_cdf->GetNcells(); ++iBin) {
    cvm += (test->GetBinContent(iBin)+pdf->GetBinContent(iBin)) * TMath::Power(test_cdf->GetBinContent(iBin) - pdf_cdf->GetBinContent(iBin), 2.);
  }
  
  delete test_shape;
  delete pdf_shape;
  delete test_cdf;
  delete pdf_cdf;
  return cvm * test->Integral() * pdf->Integral() / TMath::Power(test->Integral() + pdf->Integral(), 2.);
}

double GetAndersonDarlingStat(TH1D* test, TH1D* pdf) {
  AssertGoodHistos(test,pdf);
  
  if(test->Integral() == 0) {
    return 0;
  }
  
  double ad=0;
  int kmin=0, kmax=0;
    
  for (int iBin=1; iBin<test->GetNcells(); ++iBin) {
    kmin=iBin;
    if (pdf->GetBinContent(iBin) > 0 || test->GetBinContent(iBin) > 0) break;
  }

  for (int iBin=test->GetNcells(); iBin>0; --iBin) {
    kmax=iBin;
    if (pdf->GetBinContent(iBin) > 0 || test->GetBinContent(iBin) > 0) break;
  }
  
  double N_u = test->Integral();
  double N_v = pdf ->Integral();
  
  for (int k=kmin; k<kmax; ++k) {
      
    double eps_u_j = 0;
    double eps_v_j = 0;
    for (int iBin=1; iBin<k; ++iBin) {
      eps_u_j += test->GetBinContent(iBin);
      eps_v_j += pdf->GetBinContent(iBin);
    }
    double eps_t_j = eps_u_j + eps_v_j;
    double denom=0;

    for (int iBin=kmin; iBin<kmax; ++iBin) {

      double eps_t_j2 = 0;
      for (int iBin2=1; iBin2<iBin; ++iBin2) {
        eps_t_j2 += test->GetBinContent(iBin2) + pdf->GetBinContent(iBin2);
      }
      denom += N_u + N_v - eps_t_j2;
    }
    ad += eps_t_j / denom * (TMath::Power((N_u + N_v) * eps_u_j - N_u * eps_t_j,2) / N_u +
                             TMath::Power((N_u + N_v) * eps_v_j - N_v * eps_t_j,2) / N_v);
  }
  return ad / (N_u + N_v);
}


double GetChiSquare(TH1D* test, TH1D* pdf) {
  
  AssertGoodHistos(test,pdf);

  double Chi2=0;
  
  for (int iBin=1; iBin<test->GetNcells(); ++iBin) {
    double t  = test->GetBinContent(iBin);
    double p  = pdf ->GetBinContent(iBin);
    double te = test->GetBinError(iBin);
    double pe = pdf ->GetBinError(iBin);
    if (t == 0 && p == 0) continue;

    if (p != 0) Chi2 += (t-p)*(t-p)/(te+pe);
    else Chi2 +=100;
  }
  
  return Chi2 / test->GetNcells();
}

double GetChiSquareNormalisation(TH1D* test, TH1D* pdf) {
  
  AssertGoodHistos(test,pdf);
  
  TH1D* test_norm = new TH1D("test_norm", "test_norm", 1, 0, 1);
  TH1D* pdf_norm  = new TH1D("pdf_norm",  "pdf_norm",  1, 0, 1);

  test_norm->SetBinContent(1, test->Integral());
  pdf_norm ->SetBinContent(1, pdf ->Integral());
  
  double Chi2 = GetChiSquare(test_norm, pdf_norm);
  
  delete test_norm;
  delete pdf_norm;
  
  return Chi2;
}

double GetLikelihoodStat(TH1D* test, TH1D* pdf) {
  double llh=0;
  for (int iBin=0; iBin<=test->GetNcells(); ++iBin) {
    if (pdf->GetBinContent(iBin) == 0)
      llh += +50;
    else {
      double f= TMath::Factorial(test->GetBinContent(iBin));
      if (f != f) continue;
      double contrib = -2*TMath::Log(TMath::Power(pdf->GetBinContent(iBin), test->GetBinContent(iBin)) * TMath::Exp(-pdf->GetBinContent(iBin)) / f);
      if (contrib != contrib || isinf(contrib)) contrib = 0;
      llh += contrib;
    }
  }
  return llh;
}

double GetChiSquareShape(TH1D* test, TH1D* pdf) {
  
  AssertGoodHistos(test,pdf);
  
  TH1D* test_shape = (TH1D*)test->Clone();
  TH1D* pdf_shape = (TH1D*)pdf->Clone();
  
  if(test_shape->Integral()>0) {
    test_shape->Scale(1./test_shape->Integral());
  } else {
    delete test_shape;
    delete pdf_shape;
    return 0;
  }
  pdf_shape ->Scale(1./pdf_shape ->Integral());

  double Chi2 = GetChiSquare(test_shape, pdf_shape);
  
  delete test_shape;
  delete pdf_shape;

  return Chi2;
}

void AnalyseConfig(int config, TFile* file, int nToy) {

  TH1D* signal = (TH1D*)file->Get(Form("PDF_Background_1_config%i",config));
  TH1D* background = (TH1D*)file->Get(Form("PDF_Background_config%i",config));
  TVectorD* efficiencies = (TVectorD*)file->Get("Efficiencies");
  signal    ->SetDefaultBufferSize(0);
  background->SetDefaultBufferSize(0);
  signal    ->BufferEmpty(0);
  background->BufferEmpty(0);
  
  double efficiency = (*efficiencies)[config];
  double background_rate = background->Integral();
  background->Scale(10);
  signal->Scale(10 * efficiency / signal->Integral());
  signal->Add(background);

  signal->SetLineColor(kRed);
  background->SetLineColor(kBlue);

  TLegend* leg = new TLegend(0.6, 0.8, 0.9, 0.9);
  leg->AddEntry(signal, "Signal+Background");
  leg->AddEntry(background, "Background");
  vector<double> maxes = {background->GetMaximum(),
                          signal    ->GetMaximum()};
  double max_h = (*max_element(maxes.begin(), maxes.end()))*1.4;

  
  TText text_eff (8, 0.9*max_h, Form("Individual SN event efficiency = %.f%%", efficiency*100));
  TText text_back(8, 0.8*max_h, Form("Background rate = %f Hz", background_rate));
  text_eff.SetTextSize(0.04);
  text_back.SetTextSize(0.04);
  signal->SetStats(0);
  signal->SetMaximum(max_h);
  signal->Draw();
  background->Draw("SAME");
  text_eff.Draw();
  text_back.Draw();
  leg->Draw();
  c.Print(PDFFileName.c_str());
  c.Clear();

  TRandom3 rand;

  pair<vector<double>,vector<double>> Chi2Absolute;
  pair<vector<double>,vector<double>> Chi2Normalisation;
  pair<vector<double>,vector<double>> Chi2Shape;
  pair<vector<double>,vector<double>> BinomialNormalisation;
  pair<vector<double>,vector<double>> GeometricalTest;
  pair<vector<double>,vector<double>> KSStat;
  pair<vector<double>,vector<double>> CVMStat;
  pair<vector<double>,vector<double>> AndersonDarlingStat;
  pair<vector<double>,vector<double>> LikelihoodStat;
  CurrentProg=0;
  TCanvas bank_shape, bank_absol;

  bank_shape.Print(Form("Bank_Shape_Config_%i.pdf[",config));
  bank_absol.Print(Form("Bank_Absol_Config_%i.pdf[",config));

  TH1D* Clock = new TH1D("Clock", ";nToy;Time [ms]", nToy, 0, nToy);

  for (int iToy=0; iToy<nToy; ++iToy) {
    PrintProgress(iToy, nToy);
    

    TH1D* background_trial = (TH1D*)background->Clone();
    TH1D* signal_trial     = (TH1D*)signal->Clone();
    
    background_trial->Reset();
    signal_trial    ->Reset();
    
    int nBackground          = rand.Poisson(background_rate * 10);
    int nSignalAndBackground = rand.Poisson(background_rate * 10) + rand.Poisson(efficiency * 10);
    
    // cout << "n back " << nBackground          << "\n";
    // cout << "n sign " << nSignalAndBackground << "\n";
    for (int iBackground=0; iBackground<nBackground; ++iBackground) {
      background_trial->Fill(background->GetRandom());
    }
    
    for (int iSignalAndBackground=0; iSignalAndBackground<nSignalAndBackground; ++iSignalAndBackground) {
      signal_trial->Fill(signal->GetRandom());
    }
    
    Chi2Absolute.first         .push_back(GetChiSquare             (background_trial, background));

    auto startTime=std::chrono::steady_clock::now();
    Chi2Normalisation.first    .push_back(GetChiSquareNormalisation(background_trial, background));
    auto now=std::chrono::steady_clock::now();

    Chi2Shape.first            .push_back(GetChiSquareShape        (background_trial, background));
    BinomialNormalisation.first.push_back(GetBinomialTest          (background_trial, background));
    GeometricalTest.first      .push_back(GetGeometricalTest       (background_trial, background));
    KSStat.first               .push_back(GetKSStat                (background_trial, background));
    CVMStat.first              .push_back(GetCVMStat               (background_trial, background));
    AndersonDarlingStat.first  .push_back(GetAndersonDarlingStat   (background_trial, background));
    LikelihoodStat.first       .push_back(GetLikelihoodStat        (background_trial, background));
    
    Chi2Absolute.second         .push_back(GetChiSquare             (signal_trial,     background));
    Chi2Normalisation.second    .push_back(GetChiSquareNormalisation(signal_trial,     background));
    Chi2Shape.second            .push_back(GetChiSquareShape        (signal_trial,     background));
    BinomialNormalisation.second.push_back(GetBinomialTest          (signal_trial,     background));
    GeometricalTest.second      .push_back(GetGeometricalTest       (signal_trial,     background));
    KSStat.second               .push_back(GetKSStat                (signal_trial,     background));
    CVMStat.second              .push_back(GetCVMStat               (signal_trial,     background));
    AndersonDarlingStat.second  .push_back(GetAndersonDarlingStat   (signal_trial,     background));
    LikelihoodStat.second       .push_back(GetLikelihoodStat        (signal_trial,     background));

    //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(now - startTime).count() << "\n";
    Clock->Fill(iToy, std::chrono::duration_cast<std::chrono::microseconds>(now - startTime).count());
    
    if (iToy<-1) {
      TH1D* background_trial_shape = (TH1D*)background_trial->Clone();
      TH1D* signal_trial_shape     = (TH1D*)signal_trial    ->Clone();
      
      background_trial_shape->SetLineColor(kBlue);
      signal_trial_shape    ->SetLineColor(kRed);
      
      bank_absol.cd();
   
      background_trial->SetMaximum(1.2*max(background_trial->GetMaximum(),
                                           signal_trial->GetMaximum()));

      background_trial_shape->Draw();
      signal_trial_shape    ->Draw("SAME");
      bank_absol.Print(Form("Bank_Absol_Config_%i.pdf",config));

      if(background_trial_shape->Integral()>0)background_trial_shape->Scale(1./background_trial_shape->Integral());
      if(signal_trial_shape    ->Integral()>0)signal_trial_shape    ->Scale(1./signal_trial_shape    ->Integral());
      
      bank_shape.cd();
      background_trial_shape->SetMaximum(1.2*max(background_trial_shape->GetMaximum(),
                                                 signal_trial_shape->GetMaximum()));
      background_trial_shape->SetStats(0);
      background_trial_shape->Draw();
      signal_trial_shape    ->Draw("SAME");
      TLatex chi2_1(100,0.9*background_trial_shape->GetMaximum(),Form("#chi^{2}_{signal+background} = %f", Chi2Shape.second.back()));
      TLatex chi2_2(100,0.8*background_trial_shape->GetMaximum(),Form("#chi^{2}_{background} = %f", Chi2Shape.first.back()));
      chi2_1.Draw();
      chi2_2.Draw();
      bank_shape.Print(Form("Bank_Shape_Config_%i.pdf",config));
    }
    
  }
  bank_shape.Print(Form("Bank_Shape_Config_%i.pdf]",config));
  bank_absol.Print(Form("Bank_Absol_Config_%i.pdf]",config));
  TCanvas time_prof_canvas;
  time_prof_canvas.Print("time_prof_canvas.pdf[");
  time_prof_canvas.cd();
  Clock->Draw();
  time_prof_canvas.Print("time_prof_canvas.pdf");
  time_prof_canvas.Print("time_prof_canvas.pdf]");

  vector<pair<vector<double>,vector<double>>> Chi2Plot {Chi2Absolute, Chi2Normalisation, Chi2Shape,
                                                        BinomialNormalisation, GeometricalTest,
                                                        KSStat, CVMStat, AndersonDarlingStat, LikelihoodStat};
  vector<string> Chi2Title  {"Full #chi^{2}", "#chi^{2} (Normalisation only)", "#chi^{2} (Shape only)",
                            "Binomial Normalisation Test", "Bhattacharyya Distance Test",
                            "Kolmogorov Smirnov", "Cramer Von Mises", "Anderson Darling", "Likelihood"};
  vector<string> Chi2XName {"#chi^{2}", "#chi^{2}", "#chi^{2}", "Binomial test", "BDM", "KS", "CVM", "AD", "-2log(L)"};
  vector<string> Chi2Name  {"chi2", "chi2_norm", "chi2_shape", "binom_test", "BDM", "KS", "CVM", "AD", "loglikelihood"};
  c.cd();
  vector<pair<TH1D*,TH1D*>> histos = PlotHistos(Chi2Plot, Chi2Title, Chi2Name, Chi2XName);

  vector<TGraph*> ROC;
  int i=0;
  
  const vector<int> colours=getColors();
  TLegend* legROC = new TLegend(0.6, 0.1, 0.9, 0.5);
  
  for (auto const& it: histos) {
    //ROC.push_back()
    string name = it.first->GetTitle();
    int nbins = it.first->GetNcells();
    double total_integral_back = it.first->Integral();
    double total_integral_signback = it.second->Integral();
    vector<double> xval;
    vector<double> yval;
    if (name != "Bhattacharyya Distance Test" &&
        name != "Kolmogorov Smirnov" &&
        name != "Binomial Normalisation Test") {
      for (int iBin=0; iBin<=it.first->GetNcells(); ++iBin) {
        if (it.first->GetBinContent(iBin) !=0 || it.second->GetBinContent(iBin) != 0) {
          yval.push_back(it.second->Integral(iBin,nbins) / total_integral_signback);
          xval.push_back(263520*it.first->Integral(iBin,nbins) / total_integral_back);
        }
      }
    } else {
      for (int iBin=0; iBin<=it.first->GetNcells(); ++iBin) {
        if (it.first->GetBinContent(iBin) !=0 || it.second->GetBinContent(iBin) != 0) {
          yval.push_back(it.second->Integral(1, iBin) / total_integral_signback);
          xval.push_back(263520*it.first->Integral(1, iBin) / total_integral_back);
        }
      }
    }
    gPad->SetLogx(true);

    ROC.push_back(new TGraph(xval.size(), xval.data(), yval.data()));
    ROC.back()->SetTitle   (name.c_str());
    ROC.back()->SetName    (Chi2Name[i].c_str());

    ROC.back()->GetXaxis()->SetTitle("SN triggers / months");
    ROC.back()->GetYaxis()->SetTitle("LMC efficiency (10 #nu in 10kT)");
    legROC->AddEntry(ROC.back(),ROC.back()->GetTitle());
    ROC.back()->GetHistogram()->SetMinimum(0.);
    ROC.back()->SetLineColor(colours[i]);
    ROC.back()->SetMarkerColor(colours[i]);
    if (i==0) {
      ROC.back()->SetTitle(Form("Config %i", config));
      ROC.back()->Draw("AL");
    } else {
      ROC.back()->Draw("L");
    }
    ++i;
  }
  legROC->Draw();
  c.Print(PDFFileName.c_str());
  gPad->SetLogx(false);

  TFile* fileout = new TFile(ROOTFileName.c_str(), "RECREATE");
  fileout->cd();
  for (auto const& it: histos) {
    it.first->Write();
    it.second->Write();
  }
  for (auto const& it: ROC) {
    it->Write();
  }
  fileout->Close();

}




int Optimiser(int nToy, int nConfig, string InputFileName, string OutputFileName) {
  
  TFile* f = new TFile(InputFileName.c_str(), "READ");
  PDFFileName = OutputFileName+".pdf";
  ROOTFileName = OutputFileName+".root";
  c.Print((PDFFileName+"[").c_str());
  // ProfilerStart("RunSmartTrigger.prof");
  AnalyseConfig(nConfig, f, nToy);
  // ProfilerStop();
  c.Print((PDFFileName+"]").c_str());
  return 0;
}

int main(int argc, char** argv) {

  CLI::App app{"SmartTrigger"};
  string InputFileName = "~/Documents/Sussex-Postdoc/Physics/LikelihoodTrigger.root";
  string OutputFileName = "LikelihoodTriggerOutput";

  int nConfig, nToy;
  app.add_option("--ntoy",   nToy,           "Number of toys to throw")->required();
  app.add_option("--config", nConfig,        "Which config to run on")->required();
  app.add_option("--output", OutputFileName, "Output file name");
  app.add_option("--input",  InputFileName,  "Input file name" );

  CLI11_PARSE(app, argc, argv);
  
  return Optimiser(nToy, nConfig, InputFileName, OutputFileName);
}

//____________________________________________________________________ 
//  
// EOF
//
//____________________________________________________________________ 
//  
