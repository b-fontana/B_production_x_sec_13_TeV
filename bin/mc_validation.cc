#include <RooHist.h>
#include <TSystem.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <TStyle.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TPaveText.h>
#include <TFile.h>
#include <TTree.h>
#include <TLegend.h>
#include <TString.h>
#include <TCanvas.h>
#include <TNtupleD.h>
#include <TChain.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <RooRealVar.h>
#include <RooProduct.h>
#include <RooConstVar.h>
#include <RooDataSet.h>
#include <RooGaussian.h>
#include <RooBifurGauss.h>
#include <RooChebychev.h>
#include <RooBernstein.h>
#include <RooExponential.h>
#include <RooWorkspace.h>
#include <RooAddPdf.h>
#include <RooGenericPdf.h>
#include <RooCBShape.h>
#include <RooArgusBG.h>
#include <TGraphAsymmErrors.h>
#include <TEfficiency.h>
#include <RooMCStudy.h>
#include <RooPlot.h>
#include <RooPlotable.h>
#include <RooThresholdCategory.h>
#include <Roo1DTable.h>
#include "TMath.h"
#include "TVectorD.h"
#include <TLegend.h>
#include "TF1.h"
#include "TPaveStats.h"

#include "UserCode/B_production_x_sec_13_TeV/interface/BLooks.h"

using namespace RooFit;

#define VAR_NUMBER 17 //the variable 'mass' has to be the last one to be stored
#define BIN_NUMBER 100 

//void newNtuple(std::string filename_open, std::string filename_save, int channel);
void read_data(RooWorkspace& w, std::string filename, int channel);
void set_up_workspace_variables(RooWorkspace& w, int channel);
void histoPlot(std::vector<TH1D*> v1, std::vector<TH1D*> v2, int channel, int flag, TString var_str, int cuts);
int getNBins(int var, int channel);
double getMass(int channel);
double getSigma(RooWorkspace& w, int channel);
double Resolution(RooWorkspace& w, RooWorkspace& w_mc, int channel);
std::vector<TH1D*> histoScale(std::vector<TH1D*> v1, std::vector<TH1D*> v2);
std::vector<TH1D*> sideband_sub(RooWorkspace& w, RooWorkspace& w_mc, int channel, std::string extension);
std::vector<TH1D*> histoBuild(RooWorkspace& w, int channel, std::string extension);
std::vector<double> getBorder(RooWorkspace& w, int channel, int option);
std::vector<double> getXRange(int var, int channel);
std::string channel_to_ntuple_name(int channel);
std::string histoName(int channel/*, int variable*/);
TString canvasName(int channel, int variable, int flag, TString var_str = "");
std::string varName(int variable);
std::string unitName(int variable);

//channels: 1. B+ -> J/psi K+; 2. Bd -> J/psi K*0; 4. Bs -> J/psi phi
//example: valid --channel 1 --cuts 0
int main(int argc, char **argv) {

  int channel = 0;
  int input_data_flag = 0, input_mc_flag = 0;
  std::string input_data, input_mc;
  int cuts = 0;
  TString var_str = "";
  int corrected_mc_flag = 0;
  std::string corrected_mc_str = ""; 

  for(int i=1; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;
     
      if(argument == "--channel")
        {
          convert << argv[++i];
          convert >> channel;
        }
      else if(argument == "--input_data")
        {
          convert << argv[++i];
          convert >> input_data;
	  input_data_flag = 1;
        }
      else if(argument == "--input_mc")
	{
	  convert << argv[++i];
	  convert >> input_mc;
	  input_mc_flag = 1;
	}
      else if(argument == "--cuts")
	{
	  convert << argv[++i];
	  convert >> cuts;
	}
      else if(argument == "--CORRECT_MC") {
	convert << argv[++i];
	convert >> var_str;
	corrected_mc_flag = 1;
	corrected_mc_str = "reweighted_mc_" + channel_to_ntuple_name(channel) + "_" + var_str + ".root";
	std::cout << "Using the already corrected MC..." << std::endl;
      }

      else std::cout << "ERROR: The argument provided was not recognised." << std::endl;
    }

  if(channel==0)
    {
      std::cout << "No channel was provided as input. Please use --channel. Example: mc_validation --channel 4" << std::endl;
      return 0;
    }

  if (input_data_flag!=1) {
    if(cuts)
      input_data = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/new_inputs/myloop_new_data_" + channel_to_ntuple_name(channel) + "_with_cuts.root";
    else 
      input_data = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/no_cuts/myloop_new_data_" + channel_to_ntuple_name(channel) + "_no_cuts.root";
  }
  if (input_mc_flag!=1) {
    if(cuts)
      input_mc = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/new_inputs/myloop_new_mc_truth_" +  channel_to_ntuple_name(channel) + "_with_cuts.root";
    else
      input_mc = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/no_cuts/myloop_new_mc_truth_" + channel_to_ntuple_name(channel) + "_no_cuts.root";
  }
    
  RooWorkspace *ws = new RooWorkspace("ws","ws");
  RooWorkspace *ws_mc = new RooWorkspace("ws_mc","ws_mc");
  set_up_workspace_variables(*ws, channel);
  set_up_workspace_variables(*ws_mc, channel); 
  read_data(*ws, input_data, channel);
  read_data(*ws_mc, input_mc, channel);
  
  std::string extension1 = "_ssdata", extension2 = "_mc";
  std::vector<TH1D*> h1 = sideband_sub(*ws, *ws_mc, channel, extension1);
  std::vector<TH1D*> h2;
  
  if (corrected_mc_flag!=1) {
    h2 = histoBuild(*ws_mc, channel, extension2);
    h2 = histoScale(h1,h2); 
  }
  else {
    TFile *f = new TFile(corrected_mc_str.c_str(),"READ");
    for (int i=0; i<VAR_NUMBER; ++i) {
      TH1D* h_aux = (TH1D*)f->Get(varName(i+1).c_str());
      h2.push_back(h_aux);
    } 
    h2 = histoScale(h1,h2);
  }
  histoPlot(h1, h2, channel, corrected_mc_flag, var_str, cuts);

 return 0;
}

void read_data(RooWorkspace& w, std::string filename, int channel)
{
  TFile *f1 = new TFile(filename.c_str(),"READ");
  if ( (TNtupleD*)f1->Get( (/*"nminus1cuts_" + */channel_to_ntuple_name(channel)).c_str()) == NULL) { 
    std::cout << "ERROR: The specified Ntuple does not exist (f1)!" << std::endl;
  }

  RooRealVar errxy_shifted("errxy_shifted","errxy_shifted",0.,2.5);
  RooRealVar lerrxy("lerrxy","lerrxy",0,300);
  RooRealVar propert("propert","propert",0.,0.5);
  w.import(errxy_shifted);
  w.import(lerrxy);
  w.import(propert);

  TNtupleD* nt1 =  (TNtupleD*)f1->Get( (/*"nminus1cuts_" + */channel_to_ntuple_name(channel)).c_str());
  RooArgSet arg_list1(*(w.var("pt")), *(w.var("y")) , *(w.var("mu1pt")) , *(w.var("mu2pt")) , *(w.var("mu1eta")) , *(w.var("mu2eta")) , *(w.var("tk1pt")) , *(w.var("tk2pt")), *(w.var("tk1eta")));
  arg_list1.add(*(w.var("tk2eta")));
  arg_list1.add(*(w.var("lxy")));
  arg_list1.add(*(w.var("errxy"))); 
  arg_list1.add(*(w.var("vtxprob")));
  arg_list1.add(*(w.var("cosalpha2d")));
  arg_list1.add(*(w.var("mass")));

  RooDataSet* data = new RooDataSet("data","data",nt1,arg_list1);
  TString formula1, formula2;
  if (filename.find("_mc_") != std::string::npos) {
    formula1 = "1.14*errxy";
    formula2 = "lxy/(1.14*errxy)";
  }
  else {
    formula1 = "errxy";
    formula2 = "lxy/errxy";
  }
  RooFormulaVar errxyFunc("errxy_shifted","errxy_shifted",formula1,RooArgList(*(w.var("errxy"))));
  RooFormulaVar lerrxyFunc("lerrxy","lerrxy",formula2, RooArgList( *(w.var("errxy")), *(w.var("lxy")) ));
  RooFormulaVar propertFunc("propert","propert","mass*lxy/pt", RooArgList( *(w.var("mass")),*(w.var("lxy")),*(w.var("pt"))));
  data->addColumn(errxyFunc);
  data->addColumn(lerrxyFunc);
  data->addColumn(propertFunc);  

  w.import(*data);

  f1->Close();
  delete data;
}

std::string channel_to_ntuple_name(int channel)
{
  //returns a string with the ntuple name corresponding to the channel. It can be used to find the data on each channel saved in a file. or to write the name of a directory

  std::string ntuple_name = "";

  switch(channel){
  default:
  case 1:
    ntuple_name="ntkp";
    break;
  case 2:
    ntuple_name="ntkstar";
    break;
  case 3:
    ntuple_name="ntks";
    break;
  case 4:
    ntuple_name="ntphi";
    break;
  case 5:
    ntuple_name="ntmix";
    break;
  case 6:
    ntuple_name="ntlambda";
    break;
  }
  return ntuple_name;
}

void set_up_workspace_variables(RooWorkspace& w, int channel)
{
  double mass_min, mass_max;
  double pt_min, pt_max;
  double y_min, y_max;
  double mu1pt_min, mu1pt_max;
  double mu2pt_min, mu2pt_max;
  double mu1eta_min, mu1eta_max;
  double mu2eta_min, mu2eta_max;
  double tk1pt_min, tk1pt_max;
  double tk2pt_min, tk2pt_max;
  double tk1eta_min, tk1eta_max;
  double tk2eta_min, tk2eta_max;
  double lxy_min, lxy_max;
  double errxy_min, errxy_max;
  double vtxprob_min, vtxprob_max;
  double cosalpha2d_min, cosalpha2d_max;
 
  pt_min=0;
  pt_max=300;
  
  y_min=-3;
  y_max=3;

  mu1pt_min=0;
  mu1pt_max=120;

  mu2pt_min=0;
  mu2pt_max=170;

  mu1eta_min=-3;
  mu1eta_max=3;

  mu2eta_min=-3;
  mu2eta_max=3;

  tk1pt_min=0.;
  tk1pt_max=150.;

  tk2pt_min=0.;
  tk2pt_max=150.;

  tk1eta_min=-3;
  tk1eta_max=3;

  tk2eta_min=-3;
  tk2eta_max=3;

  lxy_min=0.;
  lxy_max=3.;

  errxy_min=0.;
  errxy_max=2.5;

  vtxprob_min=0.;
  vtxprob_max=1.; 

  cosalpha2d_min = 0.9;
  cosalpha2d_max = 1;

  std::vector<double> vector;
  vector = getBorder(w,channel,1);
  mass_min = vector.at(0);
  mass_max = vector.at(1);

  RooRealVar mass("mass","mass",mass_min,mass_max);
  RooRealVar pt("pt","pt",pt_min,pt_max);
  RooRealVar y("y","y",y_min,y_max);
  RooRealVar mu1pt("mu1pt","mu1pt",mu1pt_min,mu1pt_max);
  RooRealVar mu2pt("mu2pt","mu2pt",mu2pt_min,mu2pt_max);
  RooRealVar mu1eta("mu1eta","mu1eta",mu1eta_min,mu1eta_max);
  RooRealVar mu2eta("mu2eta","mu2eta",mu2eta_min,mu2eta_max);
  RooRealVar tk1pt("tk1pt","tk1pt",tk1pt_min,tk1pt_max);
  RooRealVar tk2pt("tk2pt","tk2pt",tk2pt_min,tk2pt_max);
  RooRealVar tk1eta("tk1eta","tk1eta",tk1eta_min,tk1eta_max);
  RooRealVar tk2eta("tk2eta","tk2eta",tk2eta_min,tk2eta_max);
  RooRealVar lxy("lxy","lxy",lxy_min,lxy_max);
  RooRealVar errxy("errxy","errxy",errxy_min,errxy_max);
  RooRealVar vtxprob("vtxprob","vtxprob",vtxprob_min,vtxprob_max);
  RooRealVar cosalpha2d("cosalpha2d","cosalpha2d",cosalpha2d_min,cosalpha2d_max);
 
  w.import(mass);
  w.import(pt);
  w.import(y);
  w.import(mu1pt);
  w.import(mu2pt);
  w.import(mu1eta);
  w.import(mu2eta);
  w.import(tk1pt);
  w.import(tk2pt);
  w.import(tk1eta);
  w.import(tk2eta);
  w.import(lxy);
  w.import(errxy);
  w.import(vtxprob);
  w.import(cosalpha2d);
}

//Scales histograms contained in v2 with respect to those contained in v1
std::vector<TH1D*> histoScale(std::vector<TH1D*> v1, std::vector<TH1D*> v2){
  int variables = VAR_NUMBER;
  for (int i=0; i<variables; ++i) {
    std::cout << v1.at(i)->GetXaxis()->GetXmin() << " - " << v1.at(i)->GetXaxis()->GetXmax()<< std::endl;
    std::cout << v2.at(i)->GetXaxis()->GetXmin() << " - " << v2.at(i)->GetXaxis()->GetXmax()<< std::endl;
    std::cout << "-----------------" << std::endl;
    v2.at(i)->Scale(v1.at(i)->Integral()/v2.at(i)->Integral());
  }
  return v2;
}

double getMass(int channel) {
  double mass = 0.;
  switch(channel) {
  case 1:
    mass=5.279;
    break;
  case 2:
    mass=5.279;
    break;
  case 3:
    mass=5.279;
    break;
  case 4:
    mass=5.366;
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
  RooRealVar *sigma1 = NULL, *sigma2 = NULL;
  RooRealVar *alpha = NULL;
  RooRealVar *n = NULL;
  RooGaussian *gauss1 = NULL, *gauss2 = NULL;
  RooCBShape *crystball = NULL;
  RooAddPdf *pdf = NULL;

  switch(channel) {
  case 1:
    sigma1 = new RooRealVar("#sigma_{1}","sigma1",0.02, 0.001, 0.5);
    sigma2 = new RooRealVar("#sigma_{2}","sigma2",0.02, 0.001, 0.5);
    alpha = new RooRealVar("#alpha","alpha", getMass(channel), 0.01, getMass(channel)+6);
    n = new RooRealVar("n", "n", 1., 0.01, 50.0); 
    gauss1 = new RooGaussian("gauss1","gauss1", mass, mean, *sigma1);
    crystball = new RooCBShape("crystball","crystball", mass, mean, *sigma2, *alpha, *n);
    pdf = new RooAddPdf("pdf","pdf", RooArgList(*gauss1,*crystball), r);
    break;
  case 2:
    sigma1 = new RooRealVar("#sigma_{1}","sigma1",0.02, 0.001, 0.5);
    sigma2 = new RooRealVar("#sigma_{2}","sigma2",0.02, 0.001, 0.5);
    alpha = new RooRealVar("#alpha","alpha", 2.5, 0.1, 5.0);
    n = new RooRealVar("n", "n", 2.5, 0.1, 5.0); 
    gauss1 = new RooGaussian("gauss1","gauss1", mass, mean, *sigma1);
    crystball = new RooCBShape("crystball","crystball", mass, mean, *sigma2, *alpha, *n);
    pdf = new RooAddPdf("pdf","pdf", RooArgList(*gauss1,*crystball), r);
    break;
  case 4:
    sigma1 = new RooRealVar("#sigma_{1}","sigma1",0.02, 0.001, 0.5);
    sigma2 = new RooRealVar("#sigma_{2}","sigma2",0.02, 0.001, 0.5);
    gauss1 = new RooGaussian("gauss1","gauss1", mass, mean, *sigma1);
    gauss2 = new RooGaussian("gauss2","gauss2", mass, mean, *sigma2);
    pdf = new RooAddPdf("pdf","pdf", RooArgList(*gauss1,*gauss2), r);
    break;
  default:
    std::cout << "ERROR: The specified channel does not exist (getSigma)" << std::endl;
    break;
  }

  RooPlot* frame = mass.frame(Title("Monte Carlo Signal Fit"));
  data->plotOn(frame);
  pdf->fitTo(*data);
  pdf->paramOn(frame, Layout(0.6,0.96,0.8));
  pdf->plotOn(frame, LineColor(7), LineWidth(4), LineStyle(1));
  pdf->plotOn(frame,Components("gauss1"),LineColor(8),LineWidth(4),LineStyle(2));
  pdf->plotOn(frame,Components("gauss2"),LineColor(5),LineWidth(4),LineStyle(2));
  pdf->plotOn(frame,Components("crystball"),LineColor(5),LineWidth(4),LineStyle(2));

  sigma1->setConstant(kTRUE);
  sigma2->setConstant(kTRUE);
  r.setConstant(kTRUE);

  TCanvas c;
  c.SetLogy();
  c.cd();
  frame->Draw();
  std::string canvas = channel_to_ntuple_name(channel) + "fit_mc.png";
  c.SaveAs(canvas.c_str());

  double sigma1_d = sigma1->getVal();
  double sigma2_d = sigma2->getVal();
  double r_d = r.getVal();

  delete sigma1;
  delete sigma2;
  delete alpha;
  delete n;
  delete gauss1;
  delete gauss2;
  delete crystball;
  delete pdf;

  return TMath::Sqrt(r_d*TMath::Power(sigma1_d,2)+(1-r_d)*TMath::Power(sigma2_d,2));
}

//options: 1 Data; 2 MC
std::vector<double> getBorder(RooWorkspace& w, int channel, int option) {
  double mass_min = 0., mass_max = 0.;
  std::vector<double> v;

  if (option != 1 && option != 2 && option != 3) 
    std::cout << "ERROR: The specified option is not available." << std::endl;
  
  switch (channel) {
  case 1:
    if (option==1) {
      mass_min = 4.15; 
      mass_max = 6.55;
    }
    else if (option==2) {
      mass_min = 5.2;
      mass_max = 5.35;
      //mass_min = getMass(channel)-2*getSigma(w,channel);  
      //mass_max = getMass(channel)+2*getSigma(w,channel); 
    }
    else if (option==3) {
      mass_min = getMass(channel)-8*getSigma(w,channel);
      mass_max = getMass(channel)-4*getSigma(w,channel);
    }
    else if (option==4) {
      mass_min = 5.5;
      mass_max = 5.9;
      //mass_min = getMass(channel)+4*getSigma(w,channel);
      //mass_max = getMass(channel)+8*getSigma(w,channel);
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
      //mass_min = getMass(channel)-2*getSigma(w,channel); 
      //mass_max = getMass(channel)+2*getSigma(w,channel);
    }
   else if (option==3) {
     mass_min = 4.8;
     mass_max = 5.;
     //mass_min = getMass(channel)-6*getSigma(w,channel);
     // mass_max = getMass(channel)-4*getSigma(w,channel);
    }
    else if (option==4) {
      mass_min = 5.5;
      mass_max = 5.75;
      //mass_min = getMass(channel)+4*getSigma(w,channel);
      //mass_max = getMass(channel)+6*getSigma(w,channel);
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
      //mass_min = getMass(channel)-2*getSigma(w,channel); 
      //mass_max = getMass(channel)+2*getSigma(w,channel);
    }
   else if (option==3) {
     mass_min = 4.8;
     mass_max = 5.15;
     //mass_min = getMass(channel)-6*getSigma(w,channel);
     // mass_max = getMass(channel)-4*getSigma(w,channel);
    }
    else if (option==4) {
      mass_min = 5.55;
      mass_max = 5.85;
      //mass_min = getMass(channel)+4*getSigma(w,channel);
      //mass_max = getMass(channel)+6*getSigma(w,channel);
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
   else if (option==3) {
      mass_min = getMass(channel)-6*getSigma(w,channel);
      mass_max = getMass(channel)-4.5*getSigma(w,channel);
    }
    else if (option==4) {
      mass_min = getMass(channel)+4.5*getSigma(w,channel);
      mass_max = getMass(channel)+6*getSigma(w,channel);
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
    }
   else if (option==3) {
      mass_min = getMass(channel)-6*getSigma(w,channel);
      mass_max = getMass(channel)-4.5*getSigma(w,channel);
    }
    else if (option==4) {
      mass_min = getMass(channel)+4.5*getSigma(w,channel);
      mass_max = getMass(channel)+6*getSigma(w,channel);
    }
    break;
  default:
    std::cout << "ERROR: The channel provided was not recognised." << std::endl;
    break;
  }

  v.push_back(mass_min);
  v.push_back(mass_max);
  return v;
}

std::vector<TH1D*> sideband_sub(RooWorkspace& w, RooWorkspace& w_mc, int channel, std::string extension) {
  std::vector<RooRealVar*> vars;
  int variables = VAR_NUMBER;
  //TFile *f_side = new TFile("sideband_studies.root","UPDATE");

  for (int k=0; k<variables; ++k) {
    vars.push_back( w.var(varName(k+1).c_str()) );
  }
  RooRealVar mass = *(w.var("mass")); //this is redundant but useful
  RooDataSet *data =(RooDataSet*) w.data("data");
  RooDataSet *reduceddata_side, *reduceddata_side_aux, *reduceddata_central;
  //Make selection for the different bands using mass as the selection variable
  std::vector<double> v2, v3, v4;
  v2 = getBorder(w_mc,channel,2);
  v3 = getBorder(w_mc,channel,3);
  v4 = getBorder(w_mc,channel,4);
  double left = v2.at(0);
  double right = v2.at(1);
  std::cout << left << " - " << right << std::endl;
  double min1=v3.at(0), min2=v3.at(1);
  double max1=v4.at(0), max2=v4.at(1);
  //consider only the right band
  bool bool_right = channel!=1/* && channel!=2 && channel!=4*/;

  reduceddata_side = (RooDataSet*) data->reduce(Form("mass>%lf",max1));
  //the B+ channel background can only be estimated using the right side of the mass spectrum
  if (bool_right) {
    reduceddata_side_aux = (RooDataSet*) data->reduce(Form("mass<%lf", min2));
    reduceddata_side->append(*reduceddata_side_aux);
  }
  reduceddata_central = (RooDataSet*) data->reduce(Form("mass>%lf",left));
  reduceddata_central = (RooDataSet*) reduceddata_central->reduce(Form("mass<%lf",right));

  RooRealVar lambda("lambda", "lambda",-2.0, -5, -0.5);
  RooExponential fit_side("fit_side", "fit_side_exp", mass, lambda);

  mass.setRange("all", min1, max2);
  mass.setRange("right",max1,max2);
  mass.setRange("left",min1,min2);
  mass.setRange("peak",left,right);  

  if (bool_right) {
    fit_side.fitTo(*reduceddata_side,Range("left,right"));
  }
  else {
    fit_side.fitTo(*reduceddata_side,Range("right"));
  }
  lambda.setConstant(kTRUE);

  RooPlot* massframe = mass.frame(Title(" "));
  //reduceddata_side->plotOn(massframe,Range("all"));
  data->plotOn(massframe,Range("all"));
  fit_side.plotOn(massframe/*, Range("all")*/);
  massframe->GetYaxis()->SetTitleOffset(1.2);
  /*Bp:  massframe->GetYaxis()->SetRangeUser(800,58000);*/
  /*B0: massframe->GetYaxis()->SetRangeUser(800,26000);*/
  /*Bs: */massframe->GetYaxis()->SetRangeUser(90,8000);
  TString B = "";
  switch(channel) {
  case 1: B = "B^{+}"; break;
  case 2: B = "B^{0}"; break;
  case 4: B = "B_{s}"; break;
  }
  massframe->GetXaxis()->SetTitle(B + " meson mass (GeV)");

  //Fit ao background nas zonas laterais do espectro de massa necessário para estimar a mesma quantidade na zona central
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

  TCanvas d, d_linear;
  d.cd();  
  massframe->Draw();
  TLatex* tex12 = new TLatex(0.65, 0.50, Form("#lambda = %.3lf #pm %.3lf",lambda_str,lambda_err));
  tex12->SetNDC(kTRUE);
  tex12->SetTextFont(42);
  tex12->SetTextSize(0.04);
  tex12->Draw();     
  d.SetLogy();
  std::string canvas = channel_to_ntuple_name(channel) + "_background_fit.png";
  d.SaveAs(canvas.c_str());
  d_linear.cd();
  massframe->Draw();
  tex12->Draw();
  d_linear.SaveAs(("linear_"+canvas).c_str());   

  //Integrar a distribuição de background

  /************************
//Calcular o integral contando explicitamente o número de eventos: o rácio final é parecido aquele que obtenho com createIntegral(), provando portanto que o método está a funcionar correctamente 
*****************************/
  /*
  TH1* h_test = (TH1*)reduceddata_side->createHistogram("h_test", mass);
  h_test->Draw();
  TAxis *axis = h_test->GetXaxis();
  int bmin = axis->FindBin(4.9); 
  int bmax = axis->FindBin(5.8); 
  int bmed = axis->FindBin(5.36);
  std::cout << std::endl << "Rácio   : " << h_test->Integral(bmin,bmed)/h_test->Integral(bmed,bmax) << std::endl;
  //////////////////////////////

  TH1* h1 = (TH1*)data->createHistogram(" ", mass);
  h1->Draw();
  TAxis *axis2 = h1->GetXaxis();
  int bb1 = axis2->FindBin(5.3244);
  int bb2 = axis2->FindBin(5.4076);
  //check: the following ratio has to be larger than 'factor'
  std::cout << std::endl << "Rácio2 :" << h1->Integral(bb1,bb2)/(h_test->Integral(bmin,bmed)+h_test->Integral(bmed,bmax)) << std::endl; 
  */

  RooAbsReal* int_fit_side_left = NULL;
  RooAbsReal* int_fit_side_right = NULL;
  RooAbsReal* int_fit_peak = NULL;
  if (bool_right) {
    int_fit_side_left = fit_side.createIntegral(mass, "left");
  }
  int_fit_side_right = fit_side.createIntegral(mass, "right");
  int_fit_peak = fit_side.createIntegral(mass, "peak");

  if (int_fit_side_left!=NULL) std::cout << std::endl << "Integral on the side: " << int_fit_side_left->getVal()+int_fit_side_right->getVal() << std::endl;
  else std::cout << std::endl << "Integral on the side: " << int_fit_side_right->getVal() << std::endl;
  std::cout<< std::endl << "Integral at the centre: " << int_fit_peak->getVal() << std::endl;

  double factor;
  if (bool_right) {
    factor = (int_fit_peak->getVal())/(int_fit_side_left->getVal()+int_fit_side_right->getVal());
  }
  else {
    factor = int_fit_peak->getVal()/int_fit_side_right->getVal();
  }

  std::cout << std::endl << "Factor: " << factor << std::endl;

  //Criar um vector de histogramas que contém o sinal extraído dos dados através do método da subtracção das zonas laterais
  //Os histograms podem depois ser desenhados usando a função histoPlot()
  std::vector<TH1D*> histos;
  std::vector<TH1D*> histos2;
  TH1D* aux1;
  TH1D* aux2;
  std::string aux_str = "_aux";

  RooDataSet* reduceddata_side2 = (RooDataSet*)reduceddata_side->reduce(Form("mass<%lf",max2));
  if (bool_right) {
    reduceddata_side2 = (RooDataSet*)reduceddata_side2->reduce(Form("mass>%lf",min1));
  }

  for (int i=0; i<variables; ++i) {
    aux1 = (TH1D*) reduceddata_central->createHistogram((varName(i+1)+extension).c_str(),*(vars.at(i)),Binning(BIN_NUMBER,getXRange(i+1,channel).at(0),getXRange(i+1,channel).at(1)));
    aux2 = (TH1D*) reduceddata_side2->createHistogram("",*(vars.at(i)),Binning(BIN_NUMBER,getXRange(i+1,channel).at(0),getXRange(i+1,channel).at(1)));
    
    /*f_side->cd();
    aux2->SetName( (channel_to_ntuple_name(channel)+varName(i+1)+"_6_10").c_str() );
    aux2->Draw();
    aux2->Write();
    */
    aux1->Add(aux2,-factor);

    histos.push_back(aux1); 

    if (i==15) {
      TFile tauss_file("tau_ss.root", "RECREATE");
      tauss_file.cd();
      aux1->SetName("tau_ss_histo");
      aux1->Write();
      tauss_file.Close();
    }

  }
  return histos;
}

std::vector<TH1D*> histoBuild(RooWorkspace& w, int channel, std::string extension) {
  std::vector<TH1D*> histos;
  std::vector<RooRealVar*> vars;
  int variables = VAR_NUMBER;
  
  for (int i=0; i<variables; ++i) {
    std::cout << varName(i+1) << std::endl;
    vars.push_back( w.var(varName(i+1).c_str()) );
    if (vars.at(i)==0) std::cout << "ERROR: The pointer does not exist!" << std::endl;
  }
  
  TH1D* h_aux;
  RooDataSet *data =(RooDataSet*) w.data("data");

  /*TH1D* aux_errxy = new TH1D("aux_errxy", "aux_errxy", BIN_NUMBER, getXRange(12,channel).at(0),getXRange(12,channel).at(1));
  TH1D* aux_lerrxy = new TH1D("aux_lerrxy", "aux_lerrxy", BIN_NUMBER, getXRange(15,channel).at(0),getXRange(15,channel).at(1));
  */
  for (int k=0; k<variables; ++k) {
    std::cout << varName(k+1)+extension << std::endl;
    h_aux = (TH1D*)data->createHistogram((varName(k+1)+extension).c_str(),*(vars.at(k)),Binning(BIN_NUMBER,getXRange(k+1,channel).at(0),getXRange(k+1,channel).at(1)));
    /*if (k==11) { //shift the errxy MC distribution 
      for (int ii=1; ii<=h_aux->GetNbinsX(); ++ii) {
        for (int jj=1; jj<=h_aux->GetBinContent(ii); ++jj)
          aux_errxy->Fill(1.14*h_aux->GetBinCenter(ii));
      }
      h_aux = aux_errxy;
    }
    else if (k==14){ //shift the lerrxy MC distribution
      for (int ii2=1; ii2<=h_aux->GetNbinsX(); ++ii2) {
	std::cout << "Bin: " << h_aux->GetBinLowEdge(ii2) << " - " << h_aux->GetBinLowEdge(ii2) + 2*(h_aux->GetBinCenter(ii2)-h_aux->GetBinLowEdge(ii2)) << "; Previou content: " << h_aux->GetBinCenter(ii2) << "; Current content: " << h_aux->GetBinCenter(ii2)/1.14 << std::endl;
        for (int jj2=1; jj2<=h_aux->GetBinContent(ii2); ++jj2)
          aux_lerrxy->Fill(static_cast<double>(h_aux->GetBinCenter(ii2)/1.14));
      }
      h_aux = aux_lerrxy;
    }
    */
    h_aux->SetTitle((varName(k+1)+extension).c_str());
    histos.push_back(h_aux);
  }

  return histos;
}

void histoPlot(std::vector<TH1D*> v1, std::vector<TH1D*> v2, int channel, int flag, TString var_str, int cuts) {
  int variables = VAR_NUMBER;
  std::vector<TCanvas*> c;
  std::string histo_data = "_histo_data";
  std::string histo_mc = "_histo_mc";
  TH1D* h_aux;
  TLine *l1; 
  //TFile *f_weights = new TFile(("weights_nminus1_" + channel_to_ntuple_name(channel) + ".root").c_str(),"RECREATE");
  TFile *f_weights = nullptr;
  if(!flag) f_weights = new TFile(("weights_" + channel_to_ntuple_name(channel) + ".root").c_str(),"UPDATE");
  for (int j=0; j<variables; ++j) {
    c.push_back(new TCanvas());
  }

  for (int i=0;i<variables;++i) {
    c.at(i)->cd();
    TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    pad1->SetBottomMargin(0.017);
    pad1->Draw();
    pad1->cd();
    v1.at(i)->SetMarkerColor(kBlue);
    v2.at(i)->SetMarkerColor(kRed);
    v1.at(i)->SetLineColor(kBlue);
    v2.at(i)->SetLineColor(kRed);
    v1.at(i)->SetNameTitle((varName(channel)+histo_data).c_str(), "");
    v2.at(i)->SetNameTitle((varName(channel)+histo_mc).c_str(), "");
    v1.at(i)->SetStats(0);
    v2.at(i)->SetStats(0);
    double q;
    if (i==VAR_NUMBER-1) q = v2.at(i)->GetMaximum();
    else  q = v1.at(i)->GetMaximum();
    v1.at(i)->SetMaximum(14*q/10);
    v2.at(i)->SetMaximum(14*q/10);
    v1.at(i)->GetXaxis()->SetRangeUser(getXRange(i+1,channel).at(0), getXRange(i+1,channel).at(1));
    v1.at(i)->GetXaxis()->SetTitleSize(0.06);
    v1.at(i)->GetYaxis()->SetTitleSize(0.045);
    v1.at(i)->GetYaxis()->SetTitleOffset(0.9);
    v1.at(i)->SetLabelSize(0., "x");
    v1.at(i)->SetLabelSize(0.05, "y");
    double bin_width = TMath::Abs( (v1.at(i)->GetBinCenter(1)-v1.at(i)->GetBinCenter(BIN_NUMBER)) / (BIN_NUMBER-1) );
    std::stringstream buf;
    buf.precision(2);
    buf << std::scientific << bin_width;
    v1.at(i)->GetYaxis()->SetTitle( ("Events / (" +  buf.str() + " " + unitName(i+1) + ")").c_str() );
    v1.at(i)->Draw();
    v2.at(i)->Draw("same");

    BLooks Look;
    TLatex* tex1 = Look.StandardLatexEntries().first;
    tex1->Draw();
    TLatex* tex2 = Look.StandardLatexEntries().second;
    tex2->Draw();

    std::vector<double> pos;
    TLegend* leg = nullptr;
    if (i==12 || i==13) pos = {0.15, 0.76, 0.50, 0.88};
    else pos = {0.54, 0.76, 0.89, 0.88};
    std::vector<TH1*> h = {v1.at(i),v2.at(i)};
    std::vector<std::string> s = {"Data (sideband subtracted)", "Monte Carlo"};
    leg = Look.LegendEntries(pos, h, s);
    leg->Draw("same");

    std::pair<double,double> xy = std::make_pair(0.4,.93);
    TLatex* tex3 = Look.LatexEntry(xy, histoName(channel));
    tex3->Draw("same");

    c.at(i)->cd();
    TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    pad2->SetTopMargin(0.1);
    pad2->SetBottomMargin(0.2);
    pad2->Draw();
    if(cuts)
      h_aux = (TH1D*)v1.at(i)->Clone( (varName(i+1) + "_with_cuts").c_str() );
    else
      h_aux = (TH1D*)v1.at(i)->Clone( (varName(i+1) + "_no_cuts").c_str() ); 
    h_aux->SetTitle("");
    h_aux->SetMarkerColor(kGreen);
    h_aux->SetLineColor(kGreen);
    h_aux->SetMaximum(2.);
    h_aux->SetMinimum(0.);
    h_aux->SetStats(0);
    h_aux->GetYaxis()->SetTitle("SS Data / MC");
    h_aux->SetLabelSize(0.13, "xy");
    h_aux->GetXaxis()->SetTitleSize(0.13);
    h_aux->GetYaxis()->SetTitleSize(0.125);
    h_aux->GetYaxis()->SetTitleOffset(0.26);
    h_aux->GetYaxis()->SetTitle("Data / MC");
    if (i==11) h_aux->GetXaxis()->SetTitle("errxy (cm)");
    else h_aux->GetXaxis()->SetTitle((varName(i+1) + " (" + unitName(i+1) + ")").c_str());
    h_aux->GetYaxis()->SetNdivisions(4.5);
    h_aux->Divide(v2.at(i));
    if(!flag) {
      std::cout << "Gravou mais pesos!" << std::endl;
      f_weights->cd();
      h_aux->Write();
    }
    pad2->cd();
    h_aux->Draw();

    l1 = new TLine(getXRange(i+1,channel).at(0),1.,getXRange(i+1,channel).at(1),1.);
    l1->SetLineColor(46);
    l1->SetLineStyle(9);
    l1->Draw("same");


    c.at(i)->SaveAs(canvasName(channel,i+1,flag,var_str));

    delete leg;
    delete tex1;
    delete tex2;
    delete tex3;
    delete pad1;
  }

  for(int k=0; k<1; ++k) {
    delete c.at(k);
  }

  if(!flag) f_weights->Close();
}

std::string varName(int variable) {
  std::string s;
  switch (variable) {
  case 1:
    s = "pt";
    break;
  case 2:
    s = "y";
    break;
  case 3:
    s = "mu1pt";
    break;
  case 4:
    s = "mu2pt";
    break;
  case 5:
    s = "mu1eta";
    break;
  case 6:
    s = "mu2eta";
    break;
  case 7:
    s = "tk1pt";
    break;
  case 8:
    s = "tk2pt";
    break;
  case 9:
    s = "tk1eta";
    break;
  case 10:
    s = "tk2eta";
    break;
  case 11:
    s = "lxy";
    break;
  case 12:
    s = "errxy_shifted"; //one can replace this string by "errxy" to use the non-shifted variable
    break;
  case 13:
    s = "vtxprob";
    break;
  case 14:
    s = "cosalpha2d";
    break;
  case 15:
    s = "lerrxy";
    break;
  case 16:
    s = "propert";
    break;
  case 17:
    s = "mass";
    break;
  default:
    s = "ERROR";
  }

  return s;
}

std::string unitName(int variable) {
  std::string s;
  switch (variable) {
  case 1:
    s = "GeV";
    break;
  case 2:
    s = "";
    break;
  case 3:
    s = "GeV";
    break;
  case 4:
    s = "GeV";
    break;
  case 5:
    s = "";
    break;
  case 6:
    s = "";
    break;
  case 7:
    s = "GeV";
    break;
  case 8:
    s = "GeV";
    break;
  case 9:
    s = "GeV";
    break;
  case 10:
    s = "";
    break;
  case 11:
    s = "cm";
    break;
  case 12:
    s = "cm";
    break;
  case 13:
    s = "";
    break;
  case 14:
    s = "";
    break;
  case 15:
    s = "";
    break;
  case 16:
    s = "cm";
    break;
  case 17:
    s = "GeV";
    break;
  default:
    s = "ERROR";
  }

  return s;
}

std::string histoName(int channel/*, int variable*/) {
  std::string s = "";
  switch (channel) {
  case 1: 
    s = s + "B^{+} #rightarrow J/#psi K^{+}";
    break;
  case 2:
    s = s + "B_{d}^{0} #rightarrow J/#psi K^{*0}";
    break;
  case 3:
    s = s + ".";
    break;
  case 4:
    s = s + "B_{s}^{0} #rightarrow J/#psi #phi";
    break;
  case 5:
    s = s + ".";
    break;
  case 6:
    s = s + ".";
    break;
  default:
    s = s + "ERROR";
    break;
  }

  return s;
}

TString canvasName(int channel, int variable, int flag, TString var_str) {
  TString s;

  switch (channel) {
  case 1: 
    s = "channel1";
    break;
  case 2:
    s = "channel2";
      break;
  case 3:
    s = "channel3";
      break;
  case 4:
    s = "channel4";
      break;
  case 5:
    s = "channel5";
      break;
  case 6:
    s = "channel6";
      break;
  default:
    s = "ERROR";
    break;
  }

  switch (variable) {
  case 1:
    s = s + "pt";
    break;
  case 2:
    s = s + "y";
    break;
  case 3:
    s = s + "mu1pt";
    break;
  case 4:
    s = s + "mu2pt";
    break;
  case 5:
    s = s + "mu1eta";
    break;
  case 6:
    s = s + "mu2eta";
    break;
  case 7:
    s = s + "tk1pt";
    break;
  case 8:
    s = s + "tk2pt";
    break;
  case 9:
    s = s + "tk1eta";
    break;
  case 10:
    s = s + "tk2eta";
    break;
  case 11:
    s = s + "lxy";
    break;
  case 12:
    s = s + "errxy";
    break;
  case 13:
    s = s + "vtxprob";
    break;
  case 14:
    s = s + "cosalpha2d";
    break;
  case 15:
    s = s + "lerrxy";
    break;
  case 16:
    s = s + "propert";
    break;
  case 17:
    s = s + "mass";
    break;
  default:
    s = s + "ERROR";
  }

  if (flag == 1) s = s + "_reweighted_mc_" + var_str;

  s = s + ".png";

  return s;
}

std::vector<double> getXRange(int var, int channel) {
  std::vector<double> v;
  switch(var) {
  case 1: //pt
    v.push_back(0.);
    v.push_back(90.);
    break;
  case 2: //y
    v.push_back(-2.9);
    v.push_back(2.9);
    break;
  case 3: //mu1pt
    v.push_back(0.);
    v.push_back(35.);
    break;
  case 4: //mu2pt
    v.push_back(0.);
    v.push_back(35.);
    break;
  case 5: //mu1eta
    v.push_back(-2.9);
    v.push_back(2.9);
    break;
  case 6: //mu2eta
    v.push_back(-2.9);
    v.push_back(2.9);
    break; 
  case 7: //tk1pt
    v.push_back(0.);
    v.push_back(13.);
    break;
  case 8: //tk2pt
    v.push_back(0.);
    v.push_back(13.);
    break;
  case 9: //tk1eta
    v.push_back(-2.9);
    v.push_back(2.9);
    break;
  case 10: //tk2eta
    v.push_back(-2.9);
    v.push_back(2.9);
    break; 
  case 11: //lxy
    v.push_back(0.);
    v.push_back(0.65);
    break;
  case 12: //errxy
    v.push_back(0.);
    v.push_back(0.025);
    break;
  case 13: //vtxprob
    v.push_back(0.);
    v.push_back(1.2);
    break;
  case 14: //cosalpha2d
    v.push_back(0.999996);
    v.push_back(1.);
    break;
  case 15: //lerrxy
    v.push_back(0.);
    v.push_back(110.);
    break;
  case 16: //propert
    v.push_back(0.);
    v.push_back(0.2);
    break;
  case 17: //mass
    switch(channel) {
    case 1:
      v.push_back(5.15);
      v.push_back(5.38);
      break;
    case 2:
      v.push_back(5.12);
      v.push_back(5.45);
      break;
    case 3:
      v.push_back(5.15);
      v.push_back(5.38);
      break;
    case 4:
      v.push_back(5.23);
      v.push_back(5.52);
      break;
    case 5:
      v.push_back(5.15);
      v.push_back(5.38);
      break;
    case 6:
      v.push_back(5.15);
      v.push_back(5.38);
      break;
    }
    break;
  }

  return v;
}

//returns the sigma of two gaussian distributions
//it is used to compare the resolution of mc and data
/*
double Resolution(RooWorkspace& w, RooWorkspace& w_mc, int channel) {
  RooDataSet* data = (RooDataSet*) w.data("data");
  RooDataSet* mc = (RooDataSet*) w_mc.data("data");
  RooRealVar mass_data = *(w.var("mass"));
  RooRealVar mass_mc = *(w_mc.var("mass"));

  //DATA FIT
  RooRealVar mean_data("mean_data","mean_data", getMass(channel));
  mean_data.setConstant(kTRUE);
  mass_data.setRange("range", 4.9, 5.8);
  //Signal double gaussian
  RooRealVar r1_data("r1_data","r1_data", 0.5, 0., 1.);
  RooRealVar sigma1_data("#sigma_{1,data}","sigma1_data", 0.01, 0.001, 0.3);
  RooRealVar sigma2_data("#sigma_{2,data}","sigma2_data", 0.03, 0.001, 0.3);
  RooGaussian gauss1_data("gauss1_data","gauss1_data", mass_data, mean_data, sigma1_data);
  RooGaussian gauss2_data("gauss2_data","gauss2_data", mass_data, mean_data, sigma2_data);
  RooAddPdf gauss_data("gauss_data","gauss_data", RooArgList(gauss1_data,gauss2_data), r1_data);
  //Background function
  RooRealVar r2_data("r2_data","r2_data", 0.5, 0., 1.);
  RooRealVar lambda_data("lambda_data", "lambda_data",-2.5, -6., -1.);
  RooExponential exp_data("exp_data", "exp_data", mass_data, lambda_data);

  //Total PDF
  RooAddPdf pdf_data("pdf_data","pdf_data", RooArgList(exp_data,gauss_data), r2_data);
  RooPlot* frame_data = mass_data.frame(Title("DATA"));
  //Fit
  data->plotOn(frame_data);
  pdf_data.fitTo(*data,Range("range"));
  pdf_data.paramOn(frame_data, Layout(0.6,0.96,0.8));
  pdf_data.plotOn(frame_data, LineColor(7), LineWidth(4), LineStyle(1));
  //pdf_data.plotOn(frame_data,Components("gauss1_data"),LineColor(8),LineWidth(4),LineStyle(2));
  //pdf_data.plotOn(frame_data,Components("gauss2_data"),LineColor(5),LineWidth(4),LineStyle(2));
  pdf_data.plotOn(frame_data,Components("gauss_data"),LineColor(5),LineWidth(4),LineStyle(2));
  pdf_data.plotOn(frame_data,Components("exp_data"),LineColor(6),LineWidth(4),LineStyle(2));
  sigma1_data.setConstant(kTRUE);
  sigma2_data.setConstant(kTRUE);
  r1_data.setConstant(kTRUE);
  r2_data.setConstant(kTRUE);
  //lambda_data.setConstant(kTRUE);
  std::cout << std::endl << "Data XSquare: " << frame_data->chiSquare() << std::endl;
  //the resolution of the signal peak only depends on r1_data
  double sigma_data = TMath::Sqrt(r1_data.getVal()*TMath::Power(sigma1_data.getVal(),2)+(1-r1_data.getVal())*TMath::Power(sigma2_data.getVal(),2));

  TCanvas c_data;
  c_data.SetLogy();
  c_data.cd();
  frame_data->SetAxisRange(5.read_data15,5.75);
  frame_data->SetMinimum(900);
  frame_data->Draw();
  c_data.SaveAs("c_data.png");

  //MC FIT
  RooRealVar mean_mc("mean_data","mean_data", getMass(channel));
  mean_mc.setConstant(kTRUE);
  RooRealVar r_mc("r_mc","r_mc", 0.5, 0., 1.);
  RooRealVar sigma1_mc("#sigma_{1,mc}","sigma1_mc", 0.01, 0.001, 0.3);
  RooRealVar sigma2_mc("#sigma_{2,mc}","sigma2_mc", 0.03, 0.001, 0.3);
  RooGaussian gauss1_mc("gauss1_mc","gauss1_mc", mass_mc, mean_mc, sigma1_mc);
  RooGaussian gauss2_mc("gauss2_mc","gauss2_mc", mass_mc, mean_mc, sigma2_mc);
  RooAddPdf pdf_mc("pdf_mc","pdf_mc", RooArgList(gauss1_mc,gauss2_mc), r_mc);
  RooPlot* frame_mc = mass_mc.frame(Title("MC"));
  mc->plotOn(frame_mc);
  pdf_mc.fitTo(*mc);
  pdf_mc.paramOn(frame_mc, Layout(0.6,0.96,0.8));
  pdf_mc.plotOn(frame_mc, LineColor(7), LineWidth(4), LineStyle(1));
  pdf_mc.plotOn(frame_mc,Components("gauss1_mc"),LineColor(8),LineWidth(4),LineStyle(2));
  pdf_mc.plotOn(frame_mc,Components("gauss2_mc"),LineColor(5),LineWidth(4),LineStyle(2));
  sigma1_mc.setConstant(kTRUE);
  sigma2_mc.setConstant(kTRUE);
  r_mc.setConstant(kTRUE);
  double sigma_mc = TMath::Sqrt(r_mc.getVal()*TMath::Power(sigma1_mc.getVal(),2)+(1-r_mc.getVal())*TMath::Power(sigma2_mc.getVal(),2));

  TCanvas c_mc;
  c_mc.SetLogy();
  c_mc.cd();
  frame_mc->SetAxisRange(5.1,5.9);
  frame_mc->Draw();
  c_mc.SaveAs("c_mc.png");
  
  return sigma_data/sigma_mc;
} 
*/
