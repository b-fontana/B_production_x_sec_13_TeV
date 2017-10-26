#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"
#include "TH2.h"

using namespace RooFit;

//Running example: fitBias --channel 4 --ptmin 9 --ptmax 13
int main(int argc, char** argv)
{
  int channel = 1;
  double pt_min = 9., pt_max = 13.;
  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;
      if(argument == "--channel")
        {
          convert << argv[++i];
          convert >> channel;
        }
      if(argument == "--ptmin")
        {
          convert << argv[++i];
          convert >> pt_min;
        }
      if(argument == "--ptmax")
        {
          convert << argv[++i];
          convert >> pt_max;
        }
    }

  std::vector<TString> params_names;
  params_names.push_back("m_sigma1");
  params_names.push_back("m_sigma2");
  params_names.push_back("m_fraction");
  params_names.push_back("m_exp");
  if(channel == 1) {
    params_names.push_back("m_nonprompt_scale");
    params_names.push_back("m_nonprompt_shift");
    params_names.push_back("m_jpsipi_fraction2");
    params_names.push_back("m_jpsipi_fraction3");
  }
  else if(channel == 2) {
    params_names.push_back("sigma_swapped1");
    params_names.push_back("sigma_swapped2");
    params_names.push_back("sigma_swapped3");
    params_names.push_back("alpha1");
    params_names.push_back("alpha2");
    params_names.push_back("alpha3");
    params_names.push_back("n1_parameter");
    params_names.push_back("n2_parameter");
    params_names.push_back("n3_parameter");
    params_names.push_back("r1");
    params_names.push_back("r2");
  }
  else if(channel == 4) {
  }
  else std::cout << "We are not currently calculating Pulls for this channel." << std::endl;

  int params_size = params_names.size();

  //here I will use the ptmin and ptmax according to the bin I am reading
  TFile file_open("XXXXXXX.root", "OPEN");

  RooWorkspace *w_open = static_cast<RooWorkspace*>(file_open.Get("w"));
  RooRealVar mass_open = *(w_open->var("mass"));
  std::vector<RooRealVar> params;
  for(int i=0; i<params_size; ++i) {
    params.push_back(*(w_open->var(params_names.at(i))));
  }
  RooAbsPdf *model_open = w_open->pdf("model");
  
  RooMCStudy* mcstudy = new RooMCStudy(*model_open,mass_open,Binned(kTRUE),Silence(),Extended(),FitOptions(Save(kTRUE),PrintEvalErrors(0)));

  // Generate and fit 1000 samples of Poisson(nExpected) events
  mcstudy->generateAndFit(1000);

  // Make plots of the distributions of the pull of each parameter
  std::vector<RooPlot*> frames;
  for (int i=0; i<params_size; ++i) {
    frames.push_back(mcstudy->plotPull(params.at(i),Bins(40),FitGauss(kTRUE)));
  }

  // Draw all plots on a canvas
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  std::vector<TCanvas*> canvases;
  for(int i=0; i<params_size; ++i) {
    canvases.push_back(new TCanvas("canvas_"+params_names.at(i), "canvas_"+params_names.at(i), 900, 900));
    canvases.at(i)->cd();
    gPad->SetLeftMargin(0.15);
    frames.at(i)->GetYaxis()->SetTitleOffset(1.4);
    frames.at(i)->Draw();
    canvases.at(i)->SaveAs("pull_"+params_names.at(i)+".png");
  }
}
