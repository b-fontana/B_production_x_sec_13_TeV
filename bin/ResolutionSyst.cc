#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/BWarnings.h"

std::pair<double,double> mass_cuts(int channel);
std::pair<double,double> histo_limits(int channel);
void create_dataset(RooWorkspace& w, TString filename, int channel);
void set_up_vars(RooWorkspace& w, int channel);
TH1D* sideband_sub(RooWorkspace& w, RooWorkspace& w_mc, int channel, double m1, double m2);
std::vector<double> getBorder(int channel, int option);
double getSigma(RooWorkspace& w, int channel);
void HistoSave(const std::tuple<TString, TString, TString> names, TH1D& h1, TH1D& h2);

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

  TString PATH = "/lstore/cms/balves/Jobs/Full_Dataset_2015_Rereco/";
  TString file_data = PATH + "myloop_new_data_" + channel_to_ntuple_name(channel) + "_with_cuts_no_tk_win_cut.root";
  TString file_mc = "notktkmasscut_myloop_new_mc_truth_" + channel_to_ntuple_name(channel) + "_with_cuts_no_tk_win_cut.root";

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

  int nbins = h->GetNbinsX();
  std::cout << h->GetEffectiveEntries() << std::endl;
  int nentries = 0, nentries_withcuts = 0;
  for(int i=1; i<=nbins; ++i) {
    nentries += h->GetBinContent(i);
    if (h->GetBinCenter(i)>=cuts.first && h->GetBinCenter(i)<=cuts.second) nentries_withcuts += h->GetBinContent(i);
  }
  
  std::cout << "Data:" << std::endl;
  std::cout << "Nominal tktkmass cuts efficiency: " << static_cast<double>(nentries_withcuts)/static_cast<double>(nentries) << " (Numerator: " << nentries_withcuts << ", Denominator: " << nentries << ")" << std::endl;

  HistoSave(std::make_tuple("ResolutionSystHistos_"+channel_to_ntuple_name(channel)+".root","h_data","h_mc"), *h, *h_mc);
  return 0;
}

void HistoSave(const std::tuple<TString, TString, TString> names, TH1D &h1, TH1D &h2) {
  TFile f(std::get<0>(names), "RECREATE");
  h1.Write(std::get<1>(names));
  h2.Write(std::get<2>(names));
  f.Close();
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
    min = 0.65;
    max = 1.32;
    break;
  case 4:
    min = 0.986;
    max = 1.09;
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
    //if (h1->GetBinContent(i) < 0.) h1->SetBinContent(i,0.);
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
