#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"
#include <TH2D.h>

using namespace RooFit;
TH2D* SidebandSub(int channel, TString file_name, TString tree_name, TString var1_str, TString var2_str, int nbinsx, double nbinsy);
std::vector<double> getBorder(int channel, int option);

//Example: 2D --channel 4 --sample_kind data --particle B
int main(int argc, char** argv)
{
  int channel = 1;
  TString sample_kind, particle;

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;
      if(argument == "--channel")
        {
          convert << argv[++i];
          convert >> channel;
        }
      if(argument == "--sample_kind")
	{
	  convert << argv[++i];
	  convert >> sample_kind;
	}
      if(argument == "--particle")
        {
          convert << argv[++i];
          convert >> particle;
        }
    }

  int nbinsx = 90, nbinsy = 90;
  TH2D *h1 = new TH2D("h1", "h1", nbinsx, -3.95, 3.95, nbinsy, 0, 60);
  TH2D *h2 = new TH2D("h2", "h2", nbinsx, -3.95, 3.95, nbinsy, 0, 60);
  TH2D *h_data = nullptr;

  TH1D *h_eff_gen = new TH1D("h_eff_gen", "h_eff_gen", 100, 0., 3.4);
  TH1D *h_eff_gen2 = new TH1D("h_eff_gen2", "h_eff_gen2", 100, 0., 3.4); //with bmuon filter
  TH1D *h_eff_reco = new TH1D("h_eff_reco", "h_eff_reco", 100, 0., 3.4);
  TH1D *h_eff_reco_nocuts = new TH1D("h_eff_reco_nocuts", "h_eff_reco_nocuts", 100, 0., 3.4);

  TString file1_string = "";
  TString file2_string = "";
  TString file3_string = "";
  TString file4_string = "";
  if(sample_kind == "mc") {
    file1_string = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/new_inputs/myloop_gen_" + channel_to_ntuple_name(channel) + "_bfilter.root";
    file2_string = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/new_inputs/myloop_gen_" + channel_to_ntuple_name(channel) +"_bmuonfilter.root";
    file3_string = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/final_selection/reduced_myloop_new_mc_truth_" + channel_to_ntuple_name(channel) + "_with_cuts.root";
    file4_string = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/no_cuts/myloop_new_mc_truth_" + channel_to_ntuple_name(channel) + "_no_cuts.root";
  }
  else if(sample_kind == "data") {
    file1_string = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/final_selection/myloop_new_data_" + channel_to_ntuple_name(channel) + "_with_cuts.root";
    if (particle == "B") {
      h_data = SidebandSub(channel,file1_string,channel_to_ntuple_name(channel),"y","pt",nbinsx,nbinsy);
      h_data->GetYaxis()->SetTitle("p_{T} (GeV)");
    }
    else if (particle == "muon") {
      h_data = SidebandSub(channel,file1_string,channel_to_ntuple_name(channel),"mu1eta","mu1pt",nbinsx,nbinsy);
      h_data->GetYaxis()->SetTitle("mu1pt (GeV)");
    }
    else std::cout << "Please define the kind of particle hwose map you would like to see." << std::endl;
    h_data->SetStats(kFALSE);
    h_data->SetTitle("");
    h_data->GetYaxis()->SetTitleOffset(1.1);
    h_data->GetZaxis()->SetTitle("");
    TCanvas c1("c1","c1",1500,1100);
    TCanvas c2("c2","c2",1500,1100);
    c1.cd();
    TPad pad1("pad1", "pad1", 0, 0, 1, 1);
    pad1.Draw();
    pad1.cd();
    h_data->DrawCopy("colz1");
    pad1.SetRightMargin(5.);
    c1.SaveAs("2D_"+channel_to_ntuple_name(channel)+"_"+sample_kind+"_"+particle+".png");
    c2.cd();
    if (particle == "B") h_data->GetYaxis()->SetRangeUser(7.0,14);
    else if (particle == "muon") h_data->GetYaxis()->SetRangeUser(2.9,6);
    h_data->Draw("colz1");
    c2.SetRightMargin(10.);
    c2.SaveAs("2D_Zoom_"+channel_to_ntuple_name(channel)+"_"+sample_kind+"_"+particle+".png");
  }
  else std::cout << "ERROR! The data kind specified is not available!" << std::endl;

  if(sample_kind == "mc") 
    {
      TFile* f1 = new TFile(file1_string, "OPEN");
      TFile* f2 = new TFile(file2_string, "OPEN");
      TFile* f3 = new TFile(file3_string, "OPEN");
      TFile* f4 = new TFile(file4_string, "OPEN");
      TTree* t1 = static_cast<TTree*>(f1->Get(channel_to_ntuple_name(channel)+"_gen"));
      TTree* t2 = static_cast<TTree*>(f2->Get(channel_to_ntuple_name(channel)+"_gen"));
      TTree* t3 = static_cast<TTree*>(f3->Get(channel_to_ntuple_name(channel)));
      TTree* t4 = static_cast<TTree*>(f4->Get(channel_to_ntuple_name(channel)));
  
      double var1_1, var2_1, var1_2, var2_2, var1_3, var2_3, var1_4, var2_4;
      TString var1_str = "y", var2_str = "pt";
      t1->SetBranchAddress(var1_str,&var1_1);
      t1->SetBranchAddress(var2_str,&var2_1);

      //////2D Plots///////////////////////////////////////////////////////
      int nentries1 = t1->GetEntries();
      for(int entry=0; entry<nentries1; ++entry) {
	t1->GetEntry(entry);
	h1->Fill(var1_1,var2_1);
	h_eff_gen->Fill(var1_1);
      }

      t2->SetBranchAddress(var1_str,&var1_2);
      t2->SetBranchAddress(var2_str,&var2_2);
      int nentries2 = t2->GetEntries();
      for(int entry=0; entry<nentries2; ++entry) {
	t2->GetEntry(entry);
	h2->Fill(var1_2,var2_2);
	h_eff_gen2->Fill(var1_2);
      }
      ///////////////////////////////////////////////////////////////////////
      /////1D Plots (I know the macro name is wrong!)////////////////////////
      ///////////////////////////////////////////////////////////////////////

      t3->SetBranchAddress(var1_str,&var1_3);
      t3->SetBranchAddress(var2_str,&var2_3);
      int nentries3 = t3->GetEntries();
      for(int entry=0; entry<nentries3; ++entry) {
	t3->GetEntry(entry);
	h_eff_reco->Fill(var1_3);
      }

      t4->SetBranchAddress(var1_str,&var1_4);
      t4->SetBranchAddress(var2_str,&var2_4);
      int nentries4 = t4->GetEntries();
      for(int entry=0; entry<nentries4; ++entry) {
	t4->GetEntry(entry);
	h_eff_reco_nocuts->Fill(var1_4);
      }

      gStyle->SetOptStat(0);
      TCanvas *c = new TCanvas("c","c",1800,1100);
      c->Divide(1,2);
      c->cd(1);
      h1->SetTitle(" ");
      h1->GetXaxis()->SetTitleOffset(0.75);
      h1->GetYaxis()->SetTitleOffset(1.05);
      h1->GetXaxis()->SetTitle(var1_str);
      h1->GetYaxis()->SetTitle(var2_str+"(GeV)");
      h1->Draw("colz");
      c->cd(2);
      h2->SetTitle(" ");
      h2->GetXaxis()->SetTitleOffset(0.75);
      h2->GetYaxis()->SetTitleOffset(1.05);
      h2->GetXaxis()->SetTitle(var1_str);
      h2->GetYaxis()->SetTitle(var2_str+"(GeV)");
      h2->Draw("colz");
      /*c->cd(3);
      TH2D* h3 = static_cast<TH2D*>(h1->Clone("h3"));
      h3->Divide(h2);
      h3->Draw("colz");
      */
      c->SaveAs("2DPlot_"+channel_to_ntuple_name(channel)+"_"+sample_kind+"_"+particle+".png");

      TCanvas *c_eff = new TCanvas("c_eff","c_eff",1800,1800);
      c_eff->Divide(1,4);
      c_eff->cd(1);
      h_eff_gen->SetTitle("");
      h_eff_gen->GetXaxis()->SetTitleOffset(0.8);
      h_eff_gen->GetXaxis()->SetTitle(var1_str);
      h_eff_gen->Draw();
      TLatex* text_eff_gen = new TLatex(0.60, 0.84, TString::Format("2015 MC GEN (BFilter)"));
      text_eff_gen->SetNDC(kTRUE);
      text_eff_gen->SetTextSize(0.04);
      text_eff_gen->SetLineWidth(2);
      text_eff_gen->Draw("same");
      c_eff->cd(2);
      h_eff_gen2->SetTitle("");
      h_eff_gen2->GetXaxis()->SetTitleOffset(0.8);
      h_eff_gen2->GetXaxis()->SetTitle(var1_str);
      h_eff_gen2->Draw();
      TLatex* text_eff_gen2 = new TLatex(0.60, 0.84, TString::Format("2015 MC GEN (BMuonFilter)"));
      text_eff_gen2->SetNDC(kTRUE);
      text_eff_gen2->SetTextSize(0.04);
      text_eff_gen2->SetLineWidth(2);
      text_eff_gen2->Draw("same");
      c_eff->cd(4);
      h_eff_reco->SetTitle("");
      h_eff_reco->GetXaxis()->SetTitleOffset(0.8);
      h_eff_reco->GetXaxis()->SetTitle(var1_str);
      h_eff_reco->Draw();
      TLatex* text_eff_reco = new TLatex(0.60, 0.84, TString::Format("2015 MC RECO (with selection cuts)"));
      text_eff_reco->SetNDC(kTRUE);
      text_eff_reco->SetTextSize(0.04);
      text_eff_reco->SetLineWidth(2);
      text_eff_reco->Draw("same");
      c_eff->cd(3);
      h_eff_reco_nocuts->SetTitle("");
      h_eff_reco_nocuts->GetXaxis()->SetTitleOffset(0.8);
      h_eff_reco_nocuts->GetXaxis()->SetTitle(var1_str);
      h_eff_reco_nocuts->Draw();
      TLatex* text_eff_reco_nocuts = new TLatex(0.60, 0.84, TString::Format("2015 MC RECO (without selection cuts)"));
      text_eff_reco_nocuts->SetNDC(kTRUE);
      text_eff_reco_nocuts->SetTextSize(0.04);
      text_eff_reco_nocuts->SetLineWidth(2);
      text_eff_reco_nocuts->Draw("same");
      c_eff->SaveAs("Eff_GenvsReco_" + channel_to_ntuple_name(channel) + ".png");
    }

  return 0;
}

TH2D* SidebandSub(int channel, TString file_name, TString tree_name, TString var1_str, TString var2_str, int nbinsx, double nbinsy) {

  TFile *f = new TFile(file_name,"READ");
  if(f == nullptr) std::cout << "ERROR! (f)" << std::endl;
  TTree *t = (TTree*)(f->Get(tree_name));    
  if(t == nullptr) std::cout << "ERROR! (t)" << std::endl;

  std::vector<double> v1, v2, v3, v4;
  v1 = getBorder(channel,1);
  RooRealVar mass("mass","mass",v1[0],v1[1]);
  RooRealVar var1(var1_str,var1_str,-5,5); //eta of B's or muons
  RooRealVar var2(var2_str,var2_str,0,300); //pT of B's or muons
  RooArgSet argset(mass,var1,var2);
  RooDataSet *data = new RooDataSet("data","data",t,argset);
  if(data == nullptr) std::cout << "ERROR! (data)" << std::endl;

  RooDataSet *reduceddata_side, *reduceddata_side_aux, *reduceddata_central;
  v2 = getBorder(channel,2);
  v3 = getBorder(channel,3);
  v4 = getBorder(channel,4);
  double left = v2.at(0);
  double right = v2.at(1);
  std::cout << left << " - " << right << std::endl;
  double min1=v3.at(0), min2=v3.at(1);
  double max1=v4.at(0), max2=v4.at(1);
  bool BP = channel==1;

  reduceddata_side = static_cast<RooDataSet*>(data->reduce(Form("mass>%lf",max1)));
  //the B+ channel background can only be estimated using the right side of the mass spectrum
  if (BP==false) {
    reduceddata_side_aux = (RooDataSet*) data->reduce(Form("mass<%lf", min2));
    reduceddata_side->append(*reduceddata_side_aux);
  }
  reduceddata_central = (RooDataSet*) data->reduce(Form("mass>%lf",left));
  reduceddata_central = (RooDataSet*) reduceddata_central->reduce(Form("mass<%lf",right));

  RooRealVar lambda("lambda", "lambda",-2., -5., 0.);
  RooExponential fit_side("fit_side", "fit_side_exp", mass, lambda);

  mass.setRange("all", min1, max2);
  mass.setRange("right",max1,max2);
  mass.setRange("left",min1,min2);
  mass.setRange("peak",left,right);  
  if (BP == false)
    fit_side.fitTo(*reduceddata_side,Range("left,right"));
  else
    fit_side.fitTo(*reduceddata_side,Range("right"));
  lambda.setConstant(kTRUE);

  RooPlot* massframe = mass.frame(Title("Exponential Fit - Sideband Subtraction"));
  reduceddata_side->plotOn(massframe,Range("all"));
  fit_side.plotOn(massframe);
  massframe->GetYaxis()->SetTitleOffset(1.2);

  //Fit ao background nas zonas laterais do espectro de massa necessário para estimar a mesma quantidade na zona central
  TCanvas d;
  d.cd();
  massframe->Draw(); 
  TLatex* tex11 = new TLatex(0.68,0.8,"2.71 fb^{-1} (13 TeV)");
  tex11->SetNDC(kTRUE);
  tex11->SetLineWidth(2);
  tex11->SetTextSize(0.04);
  tex11->Draw();
  tex11 = new TLatex(0.68,0.85,"CMS Preliminary");
  tex11->SetNDC(kTRUE);
  tex11->SetTextFont(42);
  tex11->SetTextSize(0.04);
  tex11->SetLineWidth(2);
  tex11->Draw();

  double lambda_str = lambda.getVal();
  double lambda_err = lambda.getError();
  
  TLatex* tex12 = new TLatex(0.15, 0.50, Form("#lambda_{exp} = %.3lf #pm %.3lf",lambda_str,lambda_err));
  tex12->SetNDC(kTRUE);
  tex12->SetTextFont(42);
  tex12->SetTextSize(0.04);
  tex12->Draw();     
  d.SetLogy();
  TString canvas = channel_to_ntuple_name(channel) + "_background_fit.png";
  d.SaveAs(canvas);

  RooAbsReal* int_fit_side_left = NULL;
  RooAbsReal* int_fit_side_right = NULL;
  RooAbsReal* int_fit_peak = NULL;
  if (BP == false) {
    int_fit_side_left = fit_side.createIntegral(mass, "left");
  }
  int_fit_side_right = fit_side.createIntegral(mass, "right");
  int_fit_peak = fit_side.createIntegral(mass, "peak");

  if (int_fit_side_left!=NULL) std::cout << std::endl << "Integral on the side: " << int_fit_side_left->getVal()+int_fit_side_right->getVal() << std::endl;
  else std::cout << std::endl << "Integral on the side: " << int_fit_side_right->getVal() << std::endl;
  std::cout<< std::endl << "Integral at the centre: " << int_fit_peak->getVal() << std::endl;

  double factor;
  if (BP == false) {
    factor = (int_fit_peak->getVal())/(int_fit_side_left->getVal()+int_fit_side_right->getVal());
  }
  else factor = (int_fit_peak->getVal())/int_fit_side_right->getVal();
 
  std::cout << std::endl << "Factor: " << factor << std::endl;

  TH2D *h1, *h2;
  RooDataSet* reduceddata_side2 = (RooDataSet*)reduceddata_side->reduce(Form("mass<%lf",max2));
  if (BP == false) {
    reduceddata_side2 = (RooDataSet*)reduceddata_side2->reduce(Form("mass>%lf",min1));
  }

  h1 = static_cast<TH2D*>(reduceddata_central->createHistogram("H1",var1, Binning(nbinsx,-2.55,2.55), YVar(var2, Binning(nbinsy,0.,45))));
  h2 = static_cast<TH2D*>(reduceddata_side2->createHistogram("H2",var1, Binning(nbinsx,-2.55,2.55), YVar(var2, Binning(nbinsy,0.,45))));
  h1->Add(h2,-factor);

  return h1;
}

std::vector<double> getBorder(int channel, int option) {
  double mass_min = 0., mass_max = 0.;
  std::vector<double> v;

  if (option != 1 && option != 2 && option != 3 && option != 4) 
    std::cout << "ERROR: The specified option is not available." << std::endl;
  
  switch (channel) {
  case 1: //ntkp
    if (option==1) {
      mass_min = 4.15; 
      mass_max = 6.55;
    }
    else if (option==2) {
      mass_min = 5.2;
      mass_max = 5.35;
    }
    else if (option==3) {
      mass_min = 0.;
      mass_max = 0.;
    }
    else if (option==4) {
      mass_min = 5.5;
      mass_max = 5.9;
    }
    break;
  case 2: //nkstar
    if (option==1) {
      mass_min = 4.15; 
      mass_max = 6.55;
    }
    else if (option==2) {
      mass_min = 5.2;
      mass_max = 5.35;
    }
   else if (option==3) {
     mass_min = 4.8;
     mass_max = 5.;
    }
    else if (option==4) {
      mass_min = 5.5;
      mass_max = 5.75;
    }
    break;
  case 3:
    if (option==1) {
      mass_min = 5.0; 
      mass_max = 6.;
    }
    else if (option==2) {
      mass_min = 5.0; 
      mass_max = 6.;
    }
    break;
  case 4: //nphi
    if (option==1) {
      mass_min = 4.15; 
      mass_max = 6.55;
    }
    else if (option==2) {
      mass_min = 5.3;
      mass_max = 5.45;
    }
   else if (option==3) {
     mass_min = 4.8;
     mass_max = 5.15;
    }
    else if (option==4) {
      mass_min = 5.55;
      mass_max = 5.85;
    }
    break;
  }

  v.push_back(mass_min);
  v.push_back(mass_max);
  return v;
}
