#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/BWarnings.h"

std::pair<double,double> mass_cuts(int channel);
std::pair<double,double> histo_limits(int channel);
void create_dataset(RooWorkspace& w, TString filename, int channel);
void set_up_vars(RooWorkspace& w, int channel);
TH1D* sideband_sub(RooWorkspace& w, RooWorkspace& w_mc, int channel, double m1, double m2);
std::vector<double> getBorder(int channel, int option);
double getSigma(RooWorkspace& w, int channel);
double HistoFit(int channel, bool mc, TH1& h);

//Example: ResSyst --channel 4
int main(int argc, char **argv) {
  
  int channel = 0;

  for(int i=1; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--channel")
        {
          convert << argv[++i]; convert >> channel;
        }
      else std::cout << "ERROR: The argument provided was not recognised." << std::endl;
    }

  if(channel==0)
    {
      std::cout << "No channel was provided as input. Please use --channel. Example: Res --channel 4 --cuts 1" << std::endl;
      return 0;
    }

  TString file_data = "notktkmasscut_finalselection_myloop_new_data_" + channel_to_ntuple_name(channel) + "_with_cuts.root";
  TString file_mc = "notktkmasscut_myloop_new_mc_truth_" + channel_to_ntuple_name(channel) + 
    "_with_cuts_no_tk_win_cut.root";

  std::pair<double,double> cuts = mass_cuts(channel);
  std::pair<double,double> limits = histo_limits(channel);

  //////////////////////////////////////////////////
  //////////////MC tktkmass cuts efficiency/////////
  //////////////////////////////////////////////////
  RooWorkspace *ws_mc = new RooWorkspace("ws_mc","ws_mc");
  set_up_vars(*ws_mc, channel);
  create_dataset(*ws_mc, file_mc, channel);

  RooDataSet *d_mc = static_cast<RooDataSet*>(ws_mc->data("data"));
  RooDataSet *d_mc_reduced = static_cast<RooDataSet*>( d_mc->reduce(Form("tktkmass>%lf && tktkmass<%lf", cuts.first, cuts.second)) );

  std::cout << "MC:" << std::endl;
  std::cout << "Nominal tktkmass cuts efficiency: " << static_cast<double>(d_mc_reduced->numEntries())/static_cast<double>(d_mc->numEntries()) << 
    " (Numerator: " << d_mc_reduced->numEntries() << ", Denominator: " << d_mc->numEntries() << ")" << std::endl;
  
  TH1D* h_mc = static_cast<TH1D*>( d_mc->createHistogram("MC Tktkmass Distribution", *(ws_mc->var("tktkmass")),Binning(200,limits.first,limits.second)) );
  TCanvas *c_h_mc = new TCanvas("c_h_mc", "c_h_mc", 900, 800);
  gStyle->SetOptStat(0);
  //gStyle->SetOptFit(1111);
  h_mc->SetTitle(" ");
  TString xlabel = "";
  switch(channel) {
  case 2:
    xlabel = "K* mass (GeV)";
    break;
  case 4:
    xlabel = "#phi mass (GeV)";
    break;
  }
  h_mc->GetXaxis()->SetTitle(xlabel);
  h_mc->GetYaxis()->SetTitleOffset(1.55);
  double WidthB0_mc = 0.;
  WidthB0_mc = HistoFit(channel, 1, *h_mc);
  h_mc->Draw();
  std::cout << h_mc->GetRMS() << std::endl;
  TLatex *ltx_mc = nullptr;
  std::string t_mc = "Width: #sigma = " + std::to_string(WidthB0_mc) + " GeV";
  ltx_mc = new TLatex(0.55,0.8,t_mc.c_str());
  ltx_mc->SetNDC(kTRUE);
  ltx_mc->SetTextFont(42);
  ltx_mc->SetTextSize(0.035);
  ltx_mc->SetLineWidth(2);
  ltx_mc->Draw("same");
  std::string t2_mc = "RMS: " + std::to_string(h_mc->GetRMS()) + " GeV";
  TLatex *ltx2_mc = new TLatex(0.55,0.72,t2_mc.c_str());
  ltx2_mc->SetNDC(kTRUE);
  ltx2_mc->SetTextFont(42);
  ltx2_mc->SetTextSize(0.035);
  ltx2_mc->SetLineWidth(2);
  ltx2_mc->Draw("same");

  double maximum = h_mc->GetMaximum();
  TLine *line1_mc = new TLine(cuts.first,0,cuts.first,maximum);
  line1_mc->SetLineStyle(7);
  line1_mc->SetLineColor(2);
  TLine *line2_mc = new TLine(cuts.second,0,cuts.second,maximum);
  line2_mc->SetLineStyle(7);
  line2_mc->SetLineColor(2);
  line1_mc->Draw();
  line2_mc->Draw();
  c_h_mc->SaveAs("c_h_mc_"+channel_to_ntuple_name(channel)+".png"); 
  
  //////////////////////////////////////////////////
  //////////SS Data tktkmass cuts efficiency////////
  //////////////////////////////////////////////////
  RooWorkspace *ws_data = new RooWorkspace("ws_data","ws_data");
  set_up_vars(*ws_data, channel);
  create_dataset(*ws_data, file_data, channel);

  TFile *f = new TFile("ss_histograms_ResolutionSyst.root","OPEN");
  TH1D* h;
  h = (TH1D*)f->Get("tktkmass_ss_data_"+channel_to_ntuple_name(channel));
  if(h==nullptr) {
    std::cout << "The sideband subtracted histogram was not available. It will be calculated now." << std::endl;
    h = sideband_sub(*ws_data, *ws_mc, channel, limits.first, limits.second);
  }

  TCanvas *c_h = new TCanvas("c_h", "c_h", 900, 800);
  c_h->cd();
  gStyle->SetOptStat(0);
  h->SetTitle(" ");
  switch(channel) {
  case 2:
    xlabel = "K* mass (GeV)";
    break;
  case 4:
    xlabel = "#phi mass (GeV)";
    break;
  }
  h->GetXaxis()->SetTitle(xlabel);
  h->GetYaxis()->SetTitleOffset(1.55);
  double WidthB0 = 0.;
  WidthB0 = HistoFit(channel, 0, *h);
  h->Draw();
  TLatex *ltx = nullptr;
  std::string t = "Width: #sigma = " + std::to_string(WidthB0) + " GeV";
  ltx = new TLatex(0.55,0.8,t.c_str());
  ltx->SetNDC(kTRUE);
  ltx->SetTextFont(42);
  ltx->SetTextSize(0.035);
  ltx->SetLineWidth(2);
  ltx->Draw("same");
  std::string t2 = "RMS: " + std::to_string(h->GetRMS()) + " GeV";
  TLatex *ltx2 = new TLatex(0.58,0.72,t2.c_str());
  ltx2->SetNDC(kTRUE);
  ltx2->SetTextFont(42);
  ltx2->SetTextSize(0.035);
  ltx2->SetLineWidth(2);
  ltx2->Draw("same");

  maximum = h->GetMaximum();
  TLine *line1_data = new TLine(cuts.first,0,cuts.first,maximum);
  line1_data->SetLineStyle(7);
  line1_data->SetLineColor(2);
  TLine *line2_data = new TLine(cuts.second,0,cuts.second,maximum);
  line2_data->SetLineStyle(7);
  line2_data->SetLineColor(2);
  line1_data->Draw();
  line2_data->Draw();
  c_h->SaveAs("c_h_"+channel_to_ntuple_name(channel)+".png");
  delete c_h;

  int nbins = h->GetNbinsX();
  std::cout << h->GetEffectiveEntries() << std::endl;
  int nentries = 0, nentries_withcuts = 0;
  for(int i=1; i<=nbins; ++i) {
    nentries += h->GetBinContent(i);
    if (h->GetBinCenter(i)>=cuts.first && h->GetBinCenter(i)<=cuts.second) nentries_withcuts += h->GetBinContent(i);
  }
  
  std::cout << "Data:" << std::endl;
  std::cout << "Nominal tktkmass cuts efficiency: " << static_cast<double>(nentries_withcuts)/static_cast<double>(nentries) << 
    " (Numerator: " << nentries_withcuts << ", Denominator: " << nentries << ")" << std::endl;
}

double HistoFit(int channel, bool mc, TH1& h) {
  double MASS = 0;
  TF1* sig = nullptr;
  /*  TF1* backg = nullptr;
  TF1* total = nullptr;
  */
  double left_sig = 0., right_sig = 0.;
  //double left_backg = 0., right_backg = 0.;
  if(channel == 2)
    {
      MASS = KSTAR_MASS;
      left_sig = 0.75; right_sig = 1.05;
      //left_backg = 0.96; right_backg = 1.3;
      sig = new TF1("sig","([0]*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))) + ([3]*exp(-(x-[4])*(x-[4])/(2*[5]*[5]))) + ([6]*exp(-(x-[7])*(x-[7])/(2*[8]*[8])))", left_sig, right_sig);
      //     backg = new TF1("backg","([0] + [1]*x + [2]*x*x)", left_backg, right_backg);
    }
  else if (channel == 4)
    {
      left_sig = 0.99; right_sig = 1.04;
      //left_backg = 1.03; right_backg = 1.09;
      MASS = PHI_MASS;
      sig = new TF1("sig","([0]*exp(-(x-[1])*(x-[1])/(2*[2]*[2]))) + ([3]*exp(-(x-[4])*(x-[4])/(2*[5]*[5]))) + ([6]*exp(-(x-[7])*(x-[7])/(2*[8]*[8])))", left_sig, right_sig);
      //backg = new TF1("backg","([0] + [1]*x + [2]*x*x)", left_backg, right_backg);
    }
  //triple gaussian
  sig->SetLineColor(3);
  sig->SetLineWidth(3);
  sig->SetParNames("SignalConst1","SignalMean1","SignalSigma1","SignalConst2","SignalMean2","SignalSigma2","SignalConst3","SignalMean3","SignalSigma3");
  if(channel==2) 
    {
      if(mc) {
	sig->SetParameter(0,2500.);
	sig->SetParLimits(0,0.,9000);
      }
      else  {
	sig->SetParameter(0,1000.);
	sig->SetParLimits(0,0.,7000);
      }
      sig->SetParameter(1,MASS);
      sig->SetParLimits(1,MASS-0.05,MASS+0.05);
      sig->SetParameter(2,0.001);
      sig->SetParLimits(2,0.00001,0.6);
      sig->SetParameter(3,1000.);
      if(mc) sig->SetParLimits(3,0.,8000);
      else  sig->SetParLimits(3,0.,7000);
      sig->SetParameter(4,MASS);
      sig->SetParLimits(4,MASS-0.05,MASS+0.05);
      sig->SetParameter(5,0.003);
      sig->SetParLimits(5,0.0001,0.35);
      sig->SetParameter(6,2000.);
      sig->SetParLimits(6,0.,10000);
      sig->SetParameter(7,MASS);
      sig->SetParLimits(7,MASS-0.05,MASS+0.05);
      sig->SetParameter(8,0.01);
      sig->SetParLimits(8,0.001,0.5);
      h.Fit("sig","ER", "", left_sig, right_sig);
    }
  else if(channel==4) 
    {
      if(mc) {
	sig->SetParameter(0,2500.);
	sig->SetParLimits(0,0.,45000);
      }
      else  {
	sig->SetParameter(0,1000.);
	sig->SetParLimits(0,0.,7000);
      }
      sig->SetParameter(1,MASS);
      sig->SetParLimits(1,MASS-0.03,MASS+0.03);
      sig->SetParameter(2,0.001);
      sig->SetParLimits(2,0.00001,0.35);
      if(mc) {
	sig->SetParameter(3,10000.);
	sig->SetParLimits(3,0.,45000);
      }
      else  {
	sig->SetParameter(3,1000.);
	sig->SetParLimits(3,0.,7000);
      }
      sig->SetParameter(4,MASS);
      sig->SetParLimits(4,MASS-0.03,MASS+0.03);
      sig->SetParameter(5,0.003);
      sig->SetParLimits(5,0.0001,0.35);
      if(mc) {
	sig->SetParameter(6,10000.);
	sig->SetParLimits(6,0.,30000);
      }
      else {
	sig->SetParameter(6,2000.);
	sig->SetParLimits(6,0.,10000);
      }
      sig->SetParameter(7,MASS);
      sig->SetParLimits(7,MASS-0.03,MASS+0.03);
      sig->SetParameter(8,0.01);
      sig->SetParLimits(8,0.001,0.5);
      h.Fit("sig","ER", "", left_sig, right_sig);
    }
  /*if(channel == 2 && !mc)
    {
      backg->SetLineColor(3);
      backg->SetLineWidth(1);
      backg->SetParNames("BackgConst","Slope","Quadratic");
      backg->SetParameter(0,150);
      backg->SetParLimits(0,0,1000);
      backg->SetParameter(1,-0.2);
      backg->SetParLimits(1,-20.,0.);
      backg->SetParameter(2,-5.);
      backg->SetParLimits(2,-20.,20.);
      //h.Fit("backg","ER+", "", left_backg, right_backg);
 
      total = new TF1("total", "sig+backg", 0.65, 1.3);
      total->SetLineColor(4);
      total->SetLineWidth(4);
      h.Fit("total","ER+", "", 0.65, 1.3);
    }
  */

  double r1 = sig->GetParameter(0)/(sig->GetParameter(0)+sig->GetParameter(3)+sig->GetParameter(6));
  double r2 = sig->GetParameter(3)/(sig->GetParameter(0)+sig->GetParameter(3)+sig->GetParameter(6));
  std::cout << r1 << ", " << r2 << std::endl;
  return TMath::Sqrt(TMath::Power(sig->GetParameter(2),2)*r1 + TMath::Power(sig->GetParameter(5),2)*r2 + TMath::Power(sig->GetParameter(8),2)*(1-r1-r2));
}

void create_dataset(RooWorkspace& w, TString filename, int channel)
{
  BWarnings Warn;
  TFile* f = new TFile(filename);
  Warn.CheckPointer(f);

  TNtupleD* nt =  (TNtupleD*)f->Get(channel_to_ntuple_name(channel));
  RooArgSet arg_list( *(w.var("mass")), *(w.var("tktkmass")) );

  RooDataSet* data = new RooDataSet("data","data",nt,arg_list);
  w.import(*data);

  f->Close();
}

void set_up_vars(RooWorkspace& w, int channel)
{
  double tktkmass_min, tktkmass_max;

  switch(channel) {
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

  RooRealVar mass("mass","mass",4.15,6.55);
  RooRealVar tktkmass("tktkmass","tktkmass",tktkmass_min,tktkmass_max);

  w.import(mass);
  w.import(tktkmass);
}

std::pair<double,double> mass_cuts(int channel){
  double min=0, max=0;
  switch(channel) {
  case 2:
    min = KSTAR_MASS - 0.050;
    max = KSTAR_MASS + 0.050;
    break;
  case 4:
    min = PHI_MASS - 0.010;
    max = PHI_MASS + 0.010;
    break;
  default:
    std::cout << "ERROR" << std::endl;
    break;
  }
  std::pair<double,double> pair(min,max);
  return pair;
}

std::pair<double,double> histo_limits(int channel){
  double min=0, max=0;
  switch(channel) {
  case 2:
    min = 0.62;
    max = 1.32;
    break;
  case 4:
    min = 0.986;
    max = 1.15;
    break;
  default:
    std::cout << "ERROR" << std::endl;
    break;
  }
  std::pair<double,double> pair(min,max);
  return pair;
}

TH1D* sideband_sub(RooWorkspace& w, RooWorkspace& w_mc, int channel, double m1, double m2) {

  RooRealVar mass = *(w.var("mass")); //this is redundant but useful
  RooDataSet *data =(RooDataSet*) w.data("data");
  RooDataSet *reduceddata_side, *reduceddata_side_aux, *reduceddata_central;
  //Make selection for the different bands using mass as the selection variable
  std::vector<double> v2, v3, v4;
  v2 = getBorder(channel,2);
  v3 = getBorder(channel,3);
  v4 = getBorder(channel,4);
  double left = v2.at(0);
  double right = v2.at(1);
  std::cout << left << " - " << right << std::endl;
  double min1=v3.at(0), min2=v3.at(1);
  double max1=v4.at(0), max2=v4.at(1);
  //consider only the right band
 
  std::cout << max1 << std::endl;

  reduceddata_side = static_cast<RooDataSet*>(data->reduce(Form("mass>%lf",max1)));
 
  //the B+ channel background can only be estimated using the right side of the mass spectrum
  reduceddata_side_aux = (RooDataSet*) data->reduce(Form("mass<%lf", min2));
  reduceddata_side->append(*reduceddata_side_aux);
  reduceddata_central = (RooDataSet*) data->reduce(Form("mass>%lf",left));
  reduceddata_central = (RooDataSet*) reduceddata_central->reduce(Form("mass<%lf",right));

  RooRealVar lambda("lambda", "lambda",-2., -5., 0.);
  RooExponential fit_side("fit_side", "fit_side_exp", mass, lambda);

  mass.setRange("all", min1, max2);
  mass.setRange("right",max1,max2);
  mass.setRange("left",min1,min2);
  mass.setRange("peak",left,right);  

  fit_side.fitTo(*reduceddata_side,Range("left,right"));
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
  int_fit_side_left = fit_side.createIntegral(mass, "left");
  int_fit_side_right = fit_side.createIntegral(mass, "right");
  int_fit_peak = fit_side.createIntegral(mass, "peak");

  if (int_fit_side_left!=NULL) std::cout << std::endl << "Integral on the side: " << int_fit_side_left->getVal()+int_fit_side_right->getVal() << std::endl;
  else std::cout << std::endl << "Integral on the side: " << int_fit_side_right->getVal() << std::endl;
  std::cout<< std::endl << "Integral at the centre: " << int_fit_peak->getVal() << std::endl;

  double factor = (int_fit_peak->getVal())/(int_fit_side_left->getVal()+int_fit_side_right->getVal());
  std::cout << std::endl << "Factor: " << factor << std::endl;

  //Criar um vector de histogramas que contém o sinal extraído dos dados através do método da subtracção das zonas laterais
  //Os histograms podem depois ser desenhados usando a função histoPlot()
  TH1D *h1, *h2;

  RooDataSet* reduceddata_side2 = (RooDataSet*)reduceddata_side->reduce(Form("mass<%lf",max2));
  reduceddata_side2 = (RooDataSet*)reduceddata_side2->reduce(Form("mass>%lf",min1));

  h1 = (TH1D*)reduceddata_central->createHistogram("tktkmass_ss_data",*(w.var("tktkmass")),Binning(200,m1,m2));
  h2 = (TH1D*)reduceddata_side2->createHistogram("",*(w.var("tktkmass")),Binning(200,m1,m2));
  h1->Add(h2,-factor);
  h1->SetName("tktkmass_ss_data_"+channel_to_ntuple_name(channel));

  TCanvas c_h_beforecut;
  h1->Draw();
  c_h_beforecut.SaveAs("c_h_beforecut.png");

  int nbins = h1->GetNbinsX();
  for (int i=1; i<=nbins; ++i) {
    if (h1->GetBinContent(i) < 0.) h1->SetBinContent(i,0.);
    if (channel == 4 && h1->GetBinCenter(i) > 1.09) h1->SetBinContent(i,0.);
    /* if (channel == 2 && h1->GetBinCenter(i) > 1.2) {
      h1->SetBinContent(i,0.);
      }*/
    
  }			     

  TFile f("ss_histograms_ResolutionSyst.root","RECREATE");
  h1->Draw();
  h1->Write();

  return h1;
}

std::vector<double> getBorder(int channel, int option) {
  double mass_min = 0., mass_max = 0.;
  std::vector<double> v;
  
  switch (channel) {
  case 2: //nkstar
    if(option == 1) {
      mass_min = 4.15;
      mass_max = 6.55;
    }
    else if (option==2) {
      mass_min = 5.222; 
      mass_max = 5.322;
    }
   else if (option==3) {
     mass_min = 4.85;
     mass_max = 5.15;
   }
   else if (option==4) {
     mass_min = 5.6;
     mass_max = 5.9;
   }
    break;
  case 4: //nphi
    if(option == 1) {
      mass_min = 4.15;
      mass_max = 6.55;
    }
    else if (option==2) {
      mass_min = 5.322; 
      mass_max = 5.4;
    }
    else if (option==3) {
      mass_min = 4.95;
      mass_max = 5.25;
    }
    else if (option==4) {
      mass_min = 5.45;
      mass_max = 5.75;
    }
    break;
  }

  v.push_back(mass_min);
  v.push_back(mass_max);
  return v;
}
