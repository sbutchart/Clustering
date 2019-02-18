







void CheckPhotonMCC11 () {

  TFile f("/pnfs/dune/scratch/dunepro/mcc11/snb_nov2018/hist/mcc11_pd_tdr/mcc11_pd_tdr_dra_hist-03f10efc-ed38-4147-9a32-7ee67d1fe3bb.root", "READ");

  std::map<std::string,TH1D*> map_sign;
  std::map<std::string,TH1D*> map_back;
  std::vector<std::string> vec_tree;
  vec_tree.push_back("snanaDEF45cm0100Hz5snrNonRefl/SNSimTree");
  vec_tree.push_back("snanaEFF15cm0100Hz5snrNonRefl/SNSimTree");
  vec_tree.push_back("snanaEFF30cm0100Hz5snrNonRefl/SNSimTree");
  vec_tree.push_back("snanaEFF60cm0100Hz5snrNonRefl/SNSimTree");
  vec_tree.push_back("snanaNSE45cm0010Hz5snrNonRefl/SNSimTree");
  vec_tree.push_back("snanaNSE45cm0300Hz5snrNonRefl/SNSimTree");
  vec_tree.push_back("snanaNSE45cm1000Hz5snrNonRefl/SNSimTree");
  vec_tree.push_back("snanaREF45cm0100Hz5snrOptRefl/SNSimTree");
  vec_tree.push_back("snanaREF45cm0100Hz5snrPesRefl/SNSimTree");
  vec_tree.push_back("snanaSNR45cm0100Hz3snrNonRefl/SNSimTree");
  vec_tree.push_back("snanaSNR45cm0100Hz4snrNonRefl/SNSimTree");
  vec_tree.push_back("snanaSNR45cm0100Hz7snrNonRefl/SNSimTree");

  for (auto const& it:vec_tree) {
    map_sign[it] = new TH1D((it+"sign").c_str(), (it+";nPE;nHits").c_str(),40, 0, 10);
    map_back[it] = new TH1D((it+"back").c_str(), (it+";nPE;nHits").c_str(),40, 0, 10);
    TTree* t = (TTree*)f.Get(it.c_str());
    std::vector<double> *PDS_OpHit_PE          =NULL;
    std::vector<int> *   PDS_OpHit_True_GenType=NULL;
    
    t->SetBranchAddress("PDS_OpHit_PE",           &PDS_OpHit_PE          );
    t->SetBranchAddress("PDS_OpHit_True_GenType", &PDS_OpHit_True_GenType);
    std::cout << "doing " << it << std::endl;
    for(int i=0; i<t->GetEntries()/10; ++i){
      t->GetEntry(i);
      for (size_t hit=0; hit<PDS_OpHit_PE->size(); ++hit) {
        if (PDS_OpHit_True_GenType->at(hit) == 1) map_sign[it]->Fill(PDS_OpHit_PE->at(hit));
        else                                      map_back[it]->Fill(PDS_OpHit_PE->at(hit));
      }
    }
  }

  TCanvas c;
  c.Print("PE.pdf[");
  for (auto const& it: vec_tree) {
    map_sign[it]->Scale(map_back[it]->GetBinContent(map_back[it]->GetMaximumBin())/ map_sign[it]->GetBinContent(map_sign[it]->GetMaximumBin()));
    double max = map_sign[it]->GetMaximum();
    if (map_back[it]->GetMaximum() > max) {
      map_sign[it]->SetMaximum(map_back[it]->GetMaximum()*1.2);
    }
    map_sign[it]->SetLineColor(kRed);
    map_back[it]->SetLineColor(kBlack);
    map_sign[it]->Draw("");
    map_back[it]->Draw("SAME");
    c.Print("PE.pdf");
  }
  c.Print("PE.pdf]");


  
}
