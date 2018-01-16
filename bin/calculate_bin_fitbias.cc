#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"
#include "TRandom.h"

using namespace RooFit;

//Running example: calculate_bin_fitbias --channel 4 --ptmin 9 --ptmax 13 --ymin 0.00 --ymax 0.50
int main(int argc, char** argv)
{
  int channel = 1;
  double pt_min = 9., pt_max = 13.;
  double y_min = 0.00, y_max = 0.50;

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
     if(argument == "--ymin")
        {
          convert << argv[++i];
          convert >> y_min;
        }
     if(argument == "--ymax")
        {
          convert << argv[++i];
          convert >> y_max;
        }
    }

  std::vector<TString> params_names;
  params_names.push_back("n_signal");
  /*params_names.push_back("n_combinatorial");
  params_names.push_back("m_sigma2");
  params_names.push_back("m_fraction");
  params_names.push_back("m_exp");
  params_names.push_back("m_sigma1");
  */
  int params_size = params_names.size();

  //here I will use the ptmin and ptmax according to the bin I am reading
  TString file_string = TString::Format(VERSION) + "/mass_fits/" + channel_to_ntuple_name(channel) + "/workspace/" +\
 channel_to_ntuple_name(channel) + "_mass_fit_pt_from_" + TString::Format("%d_to_%d", (int)pt_min, (int)pt_max) + "_y_from_" + TString::Format("%.2f_to_%.2f", y_min, y_max) + ".root";
  TFile file_open(file_string, "OPEN");

  RooWorkspace *w_open = static_cast<RooWorkspace*>(file_open.Get("ws_bin"));
  RooRealVar mass_open = *(w_open->var("mass"));
  std::vector<RooRealVar> params;
  for(int i=0; i<params_size; ++i) {
    params.push_back(*(w_open->var(params_names.at(i))));
  }
  RooAbsPdf *model_open = w_open->pdf("model");
  RooDataSet *data_open = static_cast<RooDataSet*>(w_open->data("data"));

  RooMCStudy* mcstudy = new RooMCStudy(*model_open,mass_open,Binned(kTRUE),Silence(),Extended(),FitOptions(Save(kTRUE),PrintEvalErrors(0)));

  mcstudy->generateAndFit(5000/*,gRandom->Poisson(data_open->numEntries())*/);

  std::cout << data_open->numEntries() << std::endl;

  // Make plots of the distributions of the pull of each parameter
  std::vector<RooPlot*> frames, framesParam;
  for (int i=0; i<params_size; ++i) {
    frames.push_back(mcstudy->plotPull(params.at(i),FrameBins(70),FitGauss(kTRUE),FrameRange(-4.5,4.5)));
    frames[i]->SetTitle(" ");
    framesParam.push_back(mcstudy->plotParam(params.at(i),FrameBins(50)));
    framesParam[i]->SetTitle(" ");
  }

  // Draw all plots on a canvas
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  TCanvas* c = new TCanvas("pulls","pulls",900,800);
  gPad->SetLeftMargin(0.15);
  for(int i=0; i<params_size; ++i) {
    c->cd();
    frames.at(i)->GetYaxis()->SetTitleOffset(1.4);
    frames.at(i)->Draw();
  }
  
  TString ptFormat = TString::Format("%d_to_%d", (int)pt_min, (int)pt_max);
  TString yFormat = TString::Format("%.2f_to_%.2f", y_min, y_max);
  yFormat[1] = '_';
  yFormat[9] = '_';
  c->SaveAs("pulls_poisson_" + channel_to_ntuple_name(channel) + "_pt_from_" + ptFormat + "_y_from_" + yFormat + ".png");

  TCanvas* c_params = new TCanvas("c_params", "c_params",900,800);
  for (int i=0; i<params_size; ++i) {
    c_params->cd();
    framesParam.at(i)->GetYaxis()->SetTitleOffset(1.4);
    framesParam.at(i)->Draw();
  }
  c_params->SaveAs("pulls_params_poisson_" + channel_to_ntuple_name(channel) + "_pt_from_" + ptFormat + "_y_from_" + yFormat + ".png");

}
