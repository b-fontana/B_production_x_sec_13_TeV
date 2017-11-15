#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/BWarnings.h"
using namespace RooFit;

/*This macro calculates the mass resolution of B+, B0 an Bs channels and the double track resolution of the B0 and Bs channels.
  The data signal is obtained with the sideband subtraction method. 
  The macro stores the sideband subtracted data histogram so that it does not need to perform the method each time is called.
  Running example: Res --channel 4 --cuts 1 
*/

double histoResolution(TH1D* h);
void create_dataset(RooWorkspace& w, std::string filename, int channel);
void set_up_vars(RooWorkspace& w, int channel);
double getMass(int channel);
double getSigma(RooWorkspace& w, int channel);
std::vector<double> getBorder(RooWorkspace& w, int channel, int option);
TH1D* sideband_sub(RooWorkspace& w, RooWorkspace& w_mc, int channel, std::string extension, std::string varName, double m1, double m2);
std::pair<double,double> DoubleTrackRes(RooWorkspace& ws, RooWorkspace& ws_mc, int channel);
std::pair<double,double> MesonRes(RooWorkspace& ws, RooWorkspace& ws_mc, int channel);

int main(int argc, char **argv) {

  int channel = 0;
  int input_data_flag = 0, input_mc_flag = 0;
  std::string input_data, input_mc;
  bool cuts = 1;

  for(int i=1; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;
     
      if(argument == "--channel")
        {
          convert << argv[++i]; convert >> channel;
        }
      else if(argument == "--input_data")
        {
          convert << argv[++i]; convert >> input_data;
	  input_data_flag = 1;
        }
      else if(argument == "--input_mc")
	{
	  convert << argv[++i]; convert >> input_mc;
	  input_mc_flag = 1;
	}
      else if(argument == "--cuts")
	{
	  convert << argv[++i]; convert >> cuts;
	}
      else std::cout << "ERROR: The argument provided was not recognised." << std::endl;
    }

  if(channel==0)
    {
      std::cout << "No channel was provided as input. Please use --channel. Example: Res --channel 4 --cuts 1" << std::endl;
      return 0;
    }

  if (input_data_flag!=1) {
    if(cuts)
      input_data = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/new_inputs/myloop_new_data_" + channel_to_ntuple_name(channel) 
	+ "_with_cuts.root";
    else
      input_data = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/no_cuts/myloop_new_data_" + channel_to_ntuple_name(channel) 
	+ "_no_cuts.root"; 
  }
  if (input_mc_flag!=1) {
    if(cuts)
      input_mc = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/new_inputs/myloop_new_mc_truth_" +  channel_to_ntuple_name(channel) 
	+ "_with_cuts.root";
    else
      input_mc = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/no_cuts/myloop_new_mc_truth_" + channel_to_ntuple_name(channel)
	+ "_no_cuts.root";
  }
  std::cout << std::endl;

  ////////////////////////////////////////////////////
  /////Fill Data and MC Workspaces////////////////////
  ////////////////////////////////////////////////////
  RooWorkspace *ws = new RooWorkspace("ws","ws");
  RooWorkspace *ws_mc = new RooWorkspace("ws_mc","ws_mc");
  set_up_vars(*ws, channel);
  set_up_vars(*ws_mc, channel);
  create_dataset(*ws, input_data, channel); 
  create_dataset(*ws_mc, input_mc, channel);

  std::pair<double,double> tktkmass_resolutions, mass_resolutions;
  /////////////////////////////////////////////////////////
  ///////Double Track Resolution///////////////////////////
  /////////////////////////////////////////////////////////
  if(channel != 1) tktkmass_resolutions = DoubleTrackRes(*ws, *ws_mc, channel);

  /////////////////////////////////////////////////////////
  ///////B Meson Resolution////////////////////////////////
  /////////////////////////////////////////////////////////
  mass_resolutions = MesonRes(*ws, *ws_mc, channel);

  /////////////////////////////////////////////////////////
  //////Print resolutions//////////////////////////////////
  /////////////////////////////////////////////////////////
  std::cout << "Resolutions: " << std::endl;
  if (channel != 1) std::cout << "Double Track Resolutions: " << tktkmass_resolutions.first << " (Data) and " << 
		      tktkmass_resolutions.second << " (MC);" << std::endl;
  std::cout << "Meson Resolutions: " << mass_resolutions.first << " (Data) and " << mass_resolutions.second << " (MC)." << std::endl;

  return 0;
}

std::pair<double,double> DoubleTrackRes(RooWorkspace& w, RooWorkspace& w_mc, int channel) {
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(0.55);
  double min = 0., max = 0., mass_fix = 0., fit_lo_data = 0., fit_hi_data =0., fit_lo_mc = 0., fit_hi_mc = 0., frame_lo = 0., frame_hi = 0.;
   if (channel == 2) {
     min = 0.75; max = 1.05; mass_fix = KSTAR_MASS; fit_lo_data = 0.87; fit_hi_data = 0.92; fit_lo_mc = 0.87; fit_hi_mc = 0.92;
     frame_lo = 0.82; frame_hi = 0.98;
   }
   else if (channel ==4) {
     min = 0.998; max = 1.041; mass_fix = PHI_MASS; fit_lo_data = 1.016; fit_hi_data = 1.023; fit_lo_mc = 1.0165; fit_hi_mc = 1.022;
     frame_lo = 1.013; frame_hi = 1.026;
   }
   else std::cout << "The selected channel is not a valid option." << std::endl;

   RooRealVar tktkmass_data = *(w.var("tktkmass"));
   RooRealVar tktkmass_mc = *(w_mc.var("tktkmass"));

   RooRealVar mean("mean", "mean", mass_fix);
   //RooRealVar r("r", "r", 0.700, 0.010, 0.990);
   RooRealVar sigma1("sigma", "sigma", 0.02, 0.001, 0.100);
   //   RooRealVar sigma2("sigma2", "sigma2", 0.006, 0.001, 0.100);
   // RooRealVar alpha1("alpha1","alpha1", 0.5, -6., 6.);
   // RooRealVar alpha2("alpha2","alpha2", -0.5, -6., 6.);
   // RooRealVar n1("n1", "n1", 3.00);
   // RooRealVar n2("n2", "n2", 3.00);

   RooGaussian g11("g11", "cb11", tktkmass_data, mean, sigma1/*, alpha1, n1*/); 
   //RooGaussian g12("g12", "cb12", tktkmass_data, mean, sigma2/*, alpha2, n2*/);
   RooGaussian g21("g21", "cb21", tktkmass_mc, mean, sigma1/*, alpha1, n1*/);
   //RooGaussian g22("g22", "cb22", tktkmass_mc, mean, sigma2/*, alpha2, n2*/);
  
   //RooAddPdf g1("g1", "g1", g11, g12, r);
   //RooAddPdf g2("g2", "g2", g21, g22, r);

   TFile *f = new TFile("ss_histograms.root","OPEN");
   TH1D* h;
   h = (TH1D*)f->Get("tktkmass_ss_data_"+channel_to_ntuple_name(channel));
   if(h==nullptr) {
     std::cout << "The sideband subtracted histogram was not available. It will be calculated now." << std::endl;
     h = sideband_sub(w, w_mc, channel, "_ss_data", "tktkmass", min, max);
   }

   RooDataHist dh("dh", "dh", tktkmass_data, Import(*h));
   RooPlot* frame_data = tktkmass_data.frame(Title(" "), Range(frame_lo,frame_hi));
   dh.plotOn(frame_data);
   tktkmass_data.setRange("centre_tktkmass_data", fit_lo_data, fit_hi_data);
   g11.fitTo(dh, Range("centre_tktkmass_data"));
   g11.paramOn(frame_data, Layout(0.65,0.995,0.90));
   frame_data->getAttText()->SetTextSize(0.035);
   //g11.plotOn(frame_data);
   g11.plotOn(frame_data, LineColor(2), LineWidth(3.5), LineStyle(1));
   //g11.plotOn(frame_data,Components("g11"),LineColor(2),LineWidth(1),LineStyle(2));
   //g11.plotOn(frame_data,Components("g12"),LineColor(2),LineWidth(1),LineStyle(2));
   double resolution_data = TMath::Sqrt(TMath::Power(sigma1.getVal(),2)/* *r.getVal() + TMath::Power(sigma2.getVal(),2)*(1-r.getVal())*/);

   RooDataSet *d = static_cast<RooDataSet*>(w_mc.data("data"));
   RooPlot* frame_mc = tktkmass_mc.frame(Title(" "), Range(frame_lo,frame_hi));
   d->plotOn(frame_mc);
   tktkmass_mc.setRange("centre_tktkmass_mc", fit_lo_mc, fit_hi_mc);
   g21.fitTo(*d, Range("centre_tktkmass_mc"));
   g21.paramOn(frame_mc, Layout(0.65,0.995,0.9));
   frame_mc->getAttText()->SetTextSize(0.035);
   //g21.plotOn(frame_mc);
   g21.plotOn(frame_mc, LineColor(4), LineWidth(3.5), LineStyle(1));
   //g21.plotOn(frame_mc,Components("g21"),LineColor(4),LineWidth(1),LineStyle(2));
   //g21.plotOn(frame_mc,Components("g22"),LineColor(4),LineWidth(1),LineStyle(2));
   double resolution_mc = TMath::Sqrt(TMath::Power(sigma1.getVal(),2)/* *r.getVal() + TMath::Power(sigma2.getVal(),2)*(1-r.getVal())*/);
   
   TString xlabel = "";
   switch(channel) {
   case 2: 
     xlabel = "K^{*} mass (GeV)";
     break;
   case 4:
     xlabel = "#phi mass (GeV)";
     break;
   }
   frame_mc->GetXaxis()->SetTitle(xlabel);
   frame_data->GetXaxis()->SetTitle(xlabel);
   TCanvas c1("c1", "c1", 1200, 900); c1.cd(); frame_data->GetYaxis()->SetTitleOffset(1.4); frame_data->Draw(); 
   TCanvas c2("c2", "c2", 1200, 900); c2.cd(); frame_mc->GetYaxis()->SetTitleOffset(1.4); frame_mc->Draw();
   c1.SaveAs("tktkmass_fit_" + channel_to_ntuple_name(channel) + "_data.png");
   c2.SaveAs("tktkmass_fit_" + channel_to_ntuple_name(channel) + "_mc.png");

   std::pair<double,double> resolutions(resolution_data,resolution_mc);
   return resolutions;
}

std::pair<double,double> MesonRes(RooWorkspace& w, RooWorkspace& w_mc, int channel) {
  gStyle->SetTitleAlign(33);
  gStyle->SetTitleX(0.7);
  double min = 0., max = 0., mass_fix = 0., fit_lo_data = 0., fit_hi_data =0., fit_lo_mc = 0., fit_hi_mc = 0., frame_lo = 0., frame_hi = 0.;
  if (channel == 1) {
    min = 5.2; max = 5.37; mass_fix = BP_MASS; fit_lo_data = 5.256; fit_hi_data = 5.303; fit_lo_mc = 5.263; fit_hi_mc = 5.295;
    frame_lo = 5.21; frame_hi = 5.35;
  }
  else if (channel == 2) {
    min = 5.24; max = 5.32; mass_fix = B0_MASS; fit_lo_data = 5.262; fit_hi_data = 5.295; fit_lo_mc = 5.265; fit_hi_mc = 5.293;
    frame_lo = 5.24; frame_hi = 5.32;
  }
  else if (channel ==4) {
    min = 5.32; max = 5.41; mass_fix = BS_MASS; fit_lo_data = 5.356; fit_hi_data = 5.377; fit_lo_mc = 5.359; fit_hi_mc = 5.375;
    frame_lo = 5.345; frame_hi = 5.385;
  }
  else std::cout << "The selected channel is not a valid option." << std::endl;

  RooRealVar mass_data = *(w.var("mass"));
  RooRealVar mass_mc = *(w_mc.var("mass"));

  RooRealVar mean("mean", "mean", mass_fix);
  //RooRealVar r("r", "r", 0.171, 0.010, 0.990);
  RooRealVar sigma1("sigma", "sigma", 0.014, 0.0001, 0.05);
  //RooRealVar sigma2("sigma2", "sigma2", 0.002, 0.0001, 0.05);
  //RooRealVar alpha1("alpha1","alpha1", 0.5, -6., 6.);
  //RooRealVar alpha2("alpha2","alpha2", -0.5, -6., 6.);
  //RooRealVar n1("n1", "n1", 3.00);
  //RooRealVar n2("n2", "n2", 3.00);
  
  RooGaussian g11("g11", "g11", mass_data, mean, sigma1/*, alpha1, n1*/); 
  //RooGaussian g12("g12", "g12", mass_data, mean, sigma2/*, alpha2, n2*/);
  RooGaussian g21("g21", "g21", mass_mc, mean, sigma1/*, alpha1, n1*/);
  //RooGaussian g22("g22", "g22", mass_mc, mean, sigma2/*, alpha2, n2*/);
  
  //RooAddPdf g1("g1", "g1", g11, g12, r);
  //RooAddPdf g2("g2", "g2", g21, g22, r);
  
  TFile *f = new TFile("ss_histograms.root","OPEN");
  TH1D* h;
  h = (TH1D*)f->Get("mass_ss_data_"+channel_to_ntuple_name(channel));
  if(h==nullptr) {
    std::cout << "The sideband subtracted histogram was not available. It will be calculated now." << std::endl;
    h = sideband_sub(w, w_mc, channel, "_ss_data", "mass", min, max);
  }

  RooDataHist dh("dh", "dh", mass_data, Import(*h));
  RooPlot* frame_data = mass_data.frame(Title(" "), Range(frame_lo,frame_hi));
  dh.plotOn(frame_data);
  mass_data.setRange("centre_mass_data", fit_lo_data, fit_hi_data);
  g11.fitTo(dh, Range("centre_mass_data"));
  g11.paramOn(frame_data, Layout(0.65,0.995,0.9));
  frame_data->getAttText()->SetTextSize(0.035);
  //g11.plotOn(frame_data);
  g11.plotOn(frame_data, LineColor(2), LineWidth(3.5), LineStyle(1));
  //g11.plotOn(frame_data,Components("g11"),LineColor(2),LineWidth(1),LineStyle(2));
  //g11.plotOn(frame_data,Components("g12"),LineColor(2),LineWidth(1),LineStyle(2));
  double resolution_data = TMath::Sqrt(TMath::Power(sigma1.getVal(),2)/* *r.getVal() + TMath::Power(sigma2.getVal(),2)*(1-r.getVal())*/);

  RooDataSet *d = static_cast<RooDataSet*>(w_mc.data("data"));
  RooPlot* frame_mc = mass_mc.frame(Title(" "), Range(frame_lo,frame_hi));
  d->plotOn(frame_mc);
  mass_mc.setRange("centre_mass_mc", fit_lo_mc, fit_hi_mc);
  g21.fitTo(*d, Range("centre_mass_mc"));
  g21.paramOn(frame_mc, Layout(0.65,0.995,0.9));
  frame_mc->getAttText()->SetTextSize(0.035);
  //g21.plotOn(frame_mc);
  g21.plotOn(frame_mc, LineColor(4), LineWidth(3.5), LineStyle(1));
  //g21.plotOn(frame_mc,Components("g21"),LineColor(4),LineWidth(1),LineStyle(2));
  //g21.plotOn(frame_mc,Components("g22"),LineColor(4),LineWidth(1),LineStyle(2));
  double resolution_mc = TMath::Sqrt(TMath::Power(sigma1.getVal(),2)/* + TMath::Power(sigma2.getVal(),2)*/);

  TString xlabel = "";
  switch(channel) {
  case 1:
    xlabel = "B^{+} mass (GeV)";
    break;
  case 2: 
    xlabel = "B^{0} mass (GeV)";
    break;
  case 4:
    xlabel = "B_{s} mass (GeV)";
    break;
  }
  frame_mc->GetXaxis()->SetTitle(xlabel);   
  frame_data->GetXaxis()->SetTitle(xlabel);   
  TCanvas c1("c1", "c1", 1200, 900); c1.cd(); frame_data->GetYaxis()->SetTitleOffset(1.4); frame_data->Draw(); 
  TCanvas c2("c2", "c2", 1200, 900); c2.cd(); frame_mc->GetYaxis()->SetTitleOffset(1.4); frame_mc->Draw();
  c1.SaveAs("mass_fit_" + channel_to_ntuple_name(channel) + "_data.png");
  c2.SaveAs("mass_fit_" + channel_to_ntuple_name(channel) + "_mc.png");

  std::pair<double,double> resolutions(resolution_data,resolution_mc);
  return resolutions;
}

void create_dataset(RooWorkspace& w, std::string filename, int channel)
{
  BWarnings Warn;
  TFile* f = new TFile(filename.c_str());
  Warn.CheckPointer(f);

  TNtupleD* nt =  (TNtupleD*)f->Get(channel_to_ntuple_name(channel));
  RooArgSet arg_list(*(w.var("mass")) , *(w.var("tktkmass")));

  RooDataSet* data = new RooDataSet("data","data",nt,arg_list);
  w.import(*data);

  f->Close();
}

void set_up_vars(RooWorkspace& w, int channel)
{
  double mass_min, mass_max;
  double tktkmass_min, tktkmass_max;

  switch(channel) {
  case 1:
    tktkmass_min = KAON_MASS-0.6;
    tktkmass_max = KAON_MASS+0.6;
    break;
  case 2:
    tktkmass_min = KSTAR_MASS-0.6;
    tktkmass_max = KSTAR_MASS+0.6;
    break;
  case 4:
    tktkmass_min = PHI_MASS-0.4;
    tktkmass_max = PHI_MASS+0.4;
    break;
  default:
    tktkmass_min = 0;
    tktkmass_max = 0;
    std::cout << "ERROR: The specified channel was not defined!" << std::endl;
    break;
  }

  std::vector<double> vector;
  vector = getBorder(w,channel,1);
  mass_min = vector.at(0);
  mass_max = vector.at(1);

  RooRealVar mass("mass","mass",mass_min,mass_max);
  RooRealVar tktkmass("tktkmass","tktkmass",tktkmass_min,tktkmass_max);
 
  w.import(mass);
  w.import(tktkmass);
}


std::vector<double> getBorder(RooWorkspace& w, int channel, int option) {
  double mass_min = 0., mass_max = 0.;
  std::vector<double> v;

  if (option != 1 && option != 2) 
    std::cout << "ERROR: The specified option is not available." << std::endl;
  
  switch (channel) {
  case 1:
    if (option==1) {
      mass_min = 4.15; 
      mass_max = 6.55;
    }
    else if (option==2) {
      mass_min = getMass(channel)-3*getSigma(w,channel); 
      mass_max = getMass(channel)+3*getSigma(w,channel);
    }
    break;
  case 2:
    if (option==1) {
      mass_min = 4.15; 
      mass_max = 6.55;
    }
    else if (option==2) {
      mass_min = getMass(channel)-3*getSigma(w,channel); 
      mass_max = getMass(channel)+3*getSigma(w,channel);
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
  case 4:
    if (option==1) {
      mass_min = 4.15; 
      mass_max = 6.55;
    }
    else if (option==2) {
      mass_min = getMass(channel)-3*getSigma(w,channel); 
      mass_max = getMass(channel)+3*getSigma(w,channel);
    }
    break;
  case 5:
    if (option==1) {
      mass_min = 5.0; 
      mass_max = 6.;
    }
    else if (option==2) {
      mass_min = 5.0; 
      mass_max = 6.;
    }
    break;
  case 6:
    if (option==1) {
      mass_min = 5.0; 
      mass_max = 6.;
    }
    else if (option==2) {
      mass_min = 5.0; 
      mass_max = 6.;
    }    break;
  default:
    std::cout << "ERROR: The channel provided was not recognised." << std::endl;
    break;
  }

  v.push_back(mass_min);
  v.push_back(mass_max);
  return v;
}

double getMass(int channel) {
  double mass = 0.;
  switch(channel) {
  case 1:
    mass=BP_MASS;
    break;
  case 2:
    mass=B0_MASS;
    break;
  case 3:
    mass=5.279;
    break;
  case 4:
    mass=BS_MASS;
    break;
  case 5:
    mass=5.;
    break;
  case 6:
    mass=5.;
    break;
  default:
    std::cout << "ERROR: The channel provided was not recognised." << std::endl;
  }
  return mass;
} 

double getSigma(RooWorkspace& w, int channel) {
  RooDataSet *data =(RooDataSet*) w.data("data");
  RooRealVar mass = *(w.var("mass"));
  RooRealVar mean("mean","mean", getMass(channel));
  mean.setConstant(kTRUE);
  RooRealVar r("r","r", 0.5, 0., 1.);
  RooRealVar sigma1("x_sigma1","x_sigma1", 0.01, 0.001, 0.3);
  RooRealVar sigma2("x_sigma2","x_sigma2", 0.03, 0.001, 0.3);
  RooGaussian gauss1("gauss1","gauss1", mass, mean, sigma1);
  RooGaussian gauss2("gauss2","gauss2", mass, mean, sigma2);
  RooAddPdf pdf("pdf","pdf", RooArgList(gauss1,gauss2), r);

  RooPlot* frame = mass.frame(Title("Monte Carlo Signal Fit"));
  data->plotOn(frame);
  pdf.fitTo(*data);
  pdf.paramOn(frame, Layout(0.6,0.96,0.95));
  pdf.plotOn(frame, LineColor(7), LineWidth(4), LineStyle(1));
  pdf.plotOn(frame,Components("gauss1"),LineColor(8),LineWidth(4),LineStyle(2));
  pdf.plotOn(frame,Components("gauss2"),LineColor(5),LineWidth(4),LineStyle(2));

  sigma1.setConstant(kTRUE);
  sigma2.setConstant(kTRUE);
  r.setConstant(kTRUE);

  TCanvas c;
  c.SetLogy();
  c.cd();
  frame->Draw();
  c.SaveAs("fit_mc.png");

  return TMath::Sqrt(r.getValV()*TMath::Power(sigma1.getValV(),2)+(1-r.getValV())*TMath::Power(sigma2.getValV(),2));
}

TH1D* sideband_sub(RooWorkspace& w, RooWorkspace& w_mc, int channel, std::string extension, std::string varName, double m1, double m2) {
  
  RooRealVar* var = w.var(varName.c_str());
  RooRealVar mass = *(w.var("mass")); //this is redundant but useful
  RooDataSet *data =(RooDataSet*) w.data("data");
  RooDataSet *reduceddata_side, *reduceddata_side_aux, *reduceddata_central;
  //Make selection for the different bands using mass as the selection variable
  std::vector<double> v2;
  v2 = getBorder(w_mc,channel,2);
  double left = v2.at(0);
  double right = v2.at(1);
  double min=4.9, max=5.8, gap = 0.01;

  reduceddata_side = (RooDataSet*) data->reduce(Form("mass>%lf", right+gap));
  //the B+ channel background can only be estimated using the right side of the mass spectrum 
  if (channel!=1) {
    reduceddata_side_aux = (RooDataSet*) data->reduce(Form("mass<%lf",left-gap));
    reduceddata_side->append(*reduceddata_side_aux);
  }
  reduceddata_central = (RooDataSet*) data->reduce(Form("mass>%lf",left));
  reduceddata_central = (RooDataSet*) reduceddata_central->reduce(Form("mass<%lf",right));

  RooRealVar lambda("lambda", "lambda",-0., -10., 0.);
  RooExponential fit_side("fit_side", "fit_side_exp", mass, lambda);

  mass.setRange("all", min, max);
  mass.setRange("right",right+gap,max);
  mass.setRange("left",min,left-gap);
  mass.setRange("peak",left,right);  

  if (channel!=1) {
    fit_side.fitTo(*reduceddata_side,Range("left,right"));
  }
  else {
    fit_side.fitTo(*reduceddata_side,Range("right"));
  }
  lambda.setConstant(kTRUE);

  RooPlot* massframe = mass.frame(Title("Exponential Fit - Sideband Subtraction"));
  reduceddata_side->plotOn(massframe,Range("all"));
  fit_side.plotOn(massframe/*, Range("all")*/);
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
  double chis = massframe->chiSquare();

  TLatex* tex12 = new TLatex(0.15, 0.85, Form("#lambda_{exp} = %.3lf #pm %.3lf",lambda_str,lambda_err));
  tex12->SetNDC(kTRUE);
  tex12->SetTextFont(42);
  tex12->SetTextSize(0.04);
  tex12->Draw();   
  TLatex* tex13 = new TLatex(0.15, 0.8, Form("#chi/DOF = %.3lf",chis));
  tex13->SetNDC(kTRUE);
  tex13->SetTextFont(42);
  tex13->SetTextSize(0.04);
  tex13->Draw();
  d.SetLogy();
  d.SaveAs("background_fit.png");   

  std::cout << std::endl << "chisquare: " << massframe->chiSquare() << std::endl;

  //Integrar a distribuição de background
  RooAbsReal* int_fit_side_left = nullptr; fit_side.createIntegral(mass, "left");
  RooAbsReal* int_fit_side_right = nullptr; fit_side.createIntegral(mass, "right");
  RooAbsReal* int_fit_peak = nullptr;

  if (channel!=1) {
    int_fit_side_left = fit_side.createIntegral(mass, "left");
  }
  int_fit_side_right = fit_side.createIntegral(mass, "right");
  int_fit_peak = fit_side.createIntegral(mass, "peak");

  if (int_fit_side_left!=NULL) std::cout << std::endl << "Integral on the side: " << int_fit_side_left->getVal()+int_fit_sid\
e_right->getVal() << std::endl;
  else std::cout << std::endl << "Integral on the side: " << int_fit_side_right->getVal() << std::endl;
  std::cout<< std::endl << "Integral at the centre: " << int_fit_peak->getVal() << std::endl;

  double factor;
  if (channel==1) {
    factor = int_fit_peak->getVal()/int_fit_side_right->getVal();
  }
  else {
    factor = (int_fit_peak->getVal())/(int_fit_side_left->getVal()+int_fit_side_right->getVal());
  }

  std::cout << std::endl << "Factor: " << factor << std::endl;  

  TH1D* aux1 = NULL;
  TH1D* aux2 = NULL;
  std::string aux_str = "_aux";

  RooDataSet* reduceddata_side2 = (RooDataSet*)reduceddata_side->reduce(Form("mass<%lf",max));
  if (channel!=1) {
    reduceddata_side2 = (RooDataSet*)reduceddata_side2->reduce(Form("mass>%lf",min));
  }

  aux1 = (TH1D*)reduceddata_central->createHistogram((varName+extension).c_str(),*(var),Binning(120,m1,m2));
  aux2 = (TH1D*)reduceddata_side2->createHistogram((varName+extension+aux_str).c_str(),*(var),Binning(120,m1,m2));
  aux1->Add(aux2,-factor);

  TFile h_file("ss_histograms.root","UPDATE");
  h_file.cd();
  aux1->SetName((varName+extension+"_").c_str()+channel_to_ntuple_name(channel));
  aux1->Write();
  h_file.Close();

  return aux1;
}
