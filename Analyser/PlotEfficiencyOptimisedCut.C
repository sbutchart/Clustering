




void PlotEfficiencyOptimisedCut() {
  bool eff = false;
  std::vector<std::string> file_name;
  std::map<std::string,double> efficiency_data;
  std::map<std::string,double> backgroundrate_data;
  std::vector<std::string> label;

  // file_name.push_back("../build/snanaEFF15cm0100Hz5snrNonRefl_NHitCut_With1PEPreCut.pdf.root");
  // file_name.push_back("../build/snanaEFF30cm0100Hz5snrNonRefl_NHitCut_With1PEPreCut.pdf.root");
  // file_name.push_back("../build/snanaDEF45cm0100Hz5snrNonRefl_NHitCut_With1PEPreCut.pdf.root");
  // file_name.push_back("../build/snanaEFF60cm0100Hz5snrNonRefl_NHitCut_With1PEPreCut.pdf.root");
  // file_name.push_back("../build/snanaEFF15cm0100Hz5snrNonRefl_NHitCut.pdf.root");
  // file_name.push_back("../build/snanaEFF30cm0100Hz5snrNonRefl_NHitCut.pdf.root");
  // file_name.push_back("../build/snanaDEF45cm0100Hz5snrNonRefl_NHitCut.pdf.root");
  // file_name.push_back("../build/snanaEFF60cm0100Hz5snrNonRefl_NHitCut.pdf.root");
  
  if (eff) {
    file_name.push_back("../build/snanaEFF15cm0100Hz5snrNonRefl_PECut.pdf.root");
    file_name.push_back("../build/snanaEFF30cm0100Hz5snrNonRefl_PECut.pdf.root");
    file_name.push_back("../build/snanaDEF45cm0100Hz5snrNonRefl_PECut.pdf.root");
    file_name.push_back("../build/snanaEFF60cm0100Hz5snrNonRefl_PECut.pdf.root");
    label.push_back("15cm^{2}");
    label.push_back("30cm^{2}");
    label.push_back("45cm^{2}");
    label.push_back("60cm^{2}");
  } else {
    file_name.push_back("../build/snanaDEF45cm0100Hz5snrNonRefl_PECut.pdf.root");
    file_name.push_back("../build/snanaREF45cm0100Hz5snrPesRefl_PECut.pdf.root");
    file_name.push_back("../build/snanaREF45cm0100Hz5snrOptRefl_PECut.pdf.root");
    label.push_back("No Ref");
    label.push_back("Pes Ref");
    label.push_back("Opt Ref");
  }
  
  efficiency_data["../build/snanaEFF15cm0100Hz5snrNonRefl_PECut.pdf.root"] = 0.146596;
  efficiency_data["../build/snanaEFF30cm0100Hz5snrNonRefl_PECut.pdf.root"] = 0.0817949;
  efficiency_data["../build/snanaDEF45cm0100Hz5snrNonRefl_PECut.pdf.root"] = 0.138592;
  efficiency_data["../build/snanaEFF60cm0100Hz5snrNonRefl_PECut.pdf.root"] = 0.273841;
  efficiency_data["../build/snanaREF45cm0100Hz5snrPesRefl_PECut.pdf.root"] = 0.144544;
  efficiency_data["../build/snanaREF45cm0100Hz5snrOptRefl_PECut.pdf.root"] = 0.127872;

  backgroundrate_data["../build/snanaEFF15cm0100Hz5snrNonRefl_PECut.pdf.root"] = 1.39354;
  backgroundrate_data["../build/snanaEFF30cm0100Hz5snrNonRefl_PECut.pdf.root"] = 5.96978;
  backgroundrate_data["../build/snanaDEF45cm0100Hz5snrNonRefl_PECut.pdf.root"] = 7.14981;
  backgroundrate_data["../build/snanaEFF60cm0100Hz5snrNonRefl_PECut.pdf.root"] = 7.7674;
  backgroundrate_data["../build/snanaREF45cm0100Hz5snrPesRefl_PECut.pdf.root"] = 0.785879;
  backgroundrate_data["../build/snanaREF45cm0100Hz5snrOptRefl_PECut.pdf.root"] = 0.332976;

  TH1D* EfficiencyPlot = new TH1D("EfficiencyLMC", "Optimal efficiency point;;Efficiency for 10 events",
                                  file_name.size(), 0, file_name.size());
  TH1D* SecondBestEfficiencyPlot = new TH1D("SecondBestEfficiencyLMC", "Second optimal efficiency point;;Efficiency for 10 events",
                                            file_name.size(), 0, file_name.size());
  TH1D* ThirdBestEfficiencyPlot = new TH1D("ThirdBestEfficiencyLMC", "Third optimal efficiency point;;Efficiency for 10 events",
                                           file_name.size(), 0, file_name.size());
  TH1D* ClusteringEfficiencyPlot = new TH1D("ClusteringEfficiency", "Clustering efficiency at optimal point;;Clustering efficiency",
                                            file_name.size(), 0, file_name.size());
  TH1D* ClusteringBackgroundRatePlot = new TH1D("ClusteringBackgroundRatePlot", "Background rate at 50% clustering efficiency;;Background rate [Hz]",
                                                file_name.size(), 0, file_name.size());
  EfficiencyPlot->SetMinimum(0.);
  EfficiencyPlot->SetMaximum(1.2);
  EfficiencyPlot->SetStats(0);    
  SecondBestEfficiencyPlot->SetMinimum(0.); 
  SecondBestEfficiencyPlot->SetMaximum(1.2);
  SecondBestEfficiencyPlot->SetStats(0);    
  ThirdBestEfficiencyPlot->SetMinimum(0.); 
  ThirdBestEfficiencyPlot->SetMaximum(1.2);
  ThirdBestEfficiencyPlot->SetStats(0);     
  
  ClusteringEfficiencyPlot->SetMinimum(0.);
  ClusteringEfficiencyPlot->SetMaximum(1.2);
  ClusteringEfficiencyPlot->SetStats(0);
  ClusteringBackgroundRatePlot->SetMinimum(0.);
  ClusteringBackgroundRatePlot->SetStats(0);
  TCanvas c;
  c.Print("EfficiencyPlot.pdf[");
  gPad->SetGridx();
  gPad->SetGridy();
  gROOT->ForceStyle();
  for (size_t it=0; it<file_name.size(); ++it) {
    TFile* file = new TFile(file_name[it].c_str(), "READ");
    if (file->IsOpen()) {
      std::cout << "Trying file " << file_name[it] << std::endl;
      TH1D* efficiencyLMC = (TH1D*)file->Get("h_nPE_LMCEff");
      if (!efficiencyLMC) {
        TH1D* efficiencyLMC = (TH1D*)file->Get("h_nHits_LMCEff");
      }
      TH1D* derivative = (TH1D*)efficiencyLMC->Clone();
      for (int bin=1; bin<efficiencyLMC->GetXaxis()->GetNbins(); ++bin) {
        double bincontent          = efficiencyLMC->GetBinContent(bin);
        double bincontent_previous = efficiencyLMC->GetBinContent(bin-1);
        if (bincontent_previous > 0.01 && bincontent > 0.01) {
          derivative->SetBinContent(bin,bincontent-bincontent_previous);
        } else {
          derivative->SetBinContent(bin,0);
        }
      }
      std::vector<int> bins;
      for (int bin=efficiencyLMC->GetXaxis()->GetNbins(); bin>1; --bin) {
        double derivative_at_bin          = derivative->GetBinContent(bin);
        double derivative_at_previous_bin = derivative->GetBinContent(bin-1);
        if (derivative_at_bin>0 && derivative_at_previous_bin<0) {
          bins.push_back(bin);
        }
      }
      for (size_t i=0; i<3; ++i) {
        std::cout << i << std::endl;
        std::cout << "bin=" << bins[i] << "\n";
        std::cout << efficiencyLMC->GetBinContent(bins[i]-1) << "\n";
        std::cout << efficiencyLMC->GetBinContent(bins[i]) << "\n";
        std::cout << efficiencyLMC->GetBinContent(bins[i]+1) << "\n";
      }

      
      std::cout << "Cut for " << label[it] << " is " << efficiencyLMC->GetBinCenter(efficiencyLMC->GetMaximumBin()) << std::endl;
      double MaxEff = efficiencyLMC->GetBinContent(efficiencyLMC->GetMaximumBin());
      std::cout << "Corresponding to an LMC eff of " << MaxEff << std::endl;
      EfficiencyPlot->SetBinContent(it+1, MaxEff);
      EfficiencyPlot->GetXaxis()->SetBinLabel(it+1,label[it].c_str());
      SecondBestEfficiencyPlot->SetBinContent(it+1, efficiencyLMC->GetBinContent(bins[1]));
      SecondBestEfficiencyPlot->GetXaxis()->SetBinLabel(it+1,label[it].c_str());
      ThirdBestEfficiencyPlot->SetBinContent(it+1, efficiencyLMC->GetBinContent(bins[2]));
      ThirdBestEfficiencyPlot->GetXaxis()->SetBinLabel(it+1,label[it].c_str());
      ClusteringEfficiencyPlot->SetBinContent(it+1,efficiency_data.at(file_name[it]));
      ClusteringEfficiencyPlot->GetXaxis()->SetBinLabel(it+1,label[it].c_str());
      ClusteringBackgroundRatePlot->SetBinContent(it+1,backgroundrate_data.at(file_name[it]));
      ClusteringBackgroundRatePlot->GetXaxis()->SetBinLabel(it+1,label[it].c_str());
      efficiencyLMC->GetXaxis()->SetTitle("nPE cut");
      efficiencyLMC->SetTitle(label[it].c_str());
      efficiencyLMC->SetMaximum(1.);
      efficiencyLMC->Draw();
      c.Print("EfficiencyPlot.pdf");
      
    }
  }

 
  EfficiencyPlot->SetLineWidth(2);
  EfficiencyPlot->Draw();
  c.SaveAs("EfficiencyPlot.pdf");
  SecondBestEfficiencyPlot->SetLineWidth(2);
  SecondBestEfficiencyPlot->Draw();
  c.SaveAs("EfficiencyPlot.pdf");
  ThirdBestEfficiencyPlot->SetLineWidth(2);
  ThirdBestEfficiencyPlot->Draw();
  c.SaveAs("EfficiencyPlot.pdf");
  ClusteringEfficiencyPlot->SetLineWidth(2);
  ClusteringEfficiencyPlot->Draw();
  c.SaveAs("EfficiencyPlot.pdf");
  ClusteringBackgroundRatePlot->SetLineWidth(2);
  ClusteringBackgroundRatePlot->Draw();
  c.SaveAs("EfficiencyPlot.pdf");
  c.SaveAs("EfficiencyPlot.pdf]");  
}
