#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"
#include "TH2.h"

using namespace RooFit ;

int main()
{

  /////////////////////////////////////////////////////////////////
  ///////Produce RooWorkspace /////////////////////////////////////
  /////////////////////////////////////////////////////////////////
  RooWorkspace *w = new RooWorkspace("w", "RooWorkspace Title");
  int channel = 4;
  set_up_workspace_variables(*w, channel);
  TString data_selection_input_file = TString::Format(BASE_DIR) + "/new_inputs/myloop_new_data_" + channel_to_ntuple_name(channel) + "_with_cuts.root";
  read_data(*w, data_selection_input_file, channel);
  
  RooRealVar mass = *(w->var("mass"));
  RooRealVar pt = *(w->var("pt"));
  RooRealVar y = *(w->var("y"));
  RooRealVar pt_low("pt_low", "pt_low", 9.);
  RooRealVar pt_high("pt_high", "pt_high", 90.);
  RooRealVar y_low("y_low", "y_low", -2.25);
  RooRealVar y_high("y_high", "y_high", 2.25);
  RooDataSet *data =(RooDataSet*) w->data("data");
  RooFormulaVar cut("cut", "pt>pt_low && pt<pt_high && y>y_low && y<y_high", RooArgList(pt, pt_low, pt_high, y, y_low, y_high));
  data->reduce(cut);
  
  //signal component
  RooRealVar mean("mean","mean", BS_MASS, BS_MASS-0.09, BS_MASS+0.09);
  RooRealVar sigma1("sigma1","sigma1",0.010,0.009,0.200);
  RooRealVar sigma2("sigma2","sigma2",0.005,0.004,0.100);
  RooRealVar fraction("fraction","fraction", 0.4, 0., .85);
  RooGaussian gaussian1("gaussian1","gaussian1",mass,mean,sigma1);
  RooGaussian gaussian2("gaussian2","gaussian2",mass,mean,sigma2);
  RooRealVar sig_frac("sig_frac","sig_frac", 0.20, 0., 0.9);
  RooAddPdf sig("sig","sig",RooArgList(gaussian1,gaussian2), RooArgList(sig_frac));

  //background component
  RooRealVar exponent("exponent","exponent",-1.,-4.,0.);
  RooExponential bkg("bkg","bkg",mass,exponent);

  //full pdf model
  RooRealVar nbkg("nbkg","nbkg",6000,0,50000) ;
  RooRealVar nsig("nsig","nsig",6000,0,50000) ;
  RooAddPdf  model("model","model",RooArgList(bkg,sig),RooArgList(nbkg,nsig));

  //fit and plot
  RooPlot* frame = mass.frame(Title("Test Fit"));
  data->plotOn(frame);
  model.fitTo(*data);
  model.paramOn(frame, Layout(0.6,0.96,0.8));
  model.plotOn(frame, LineColor(7), LineWidth(4), LineStyle(1));
  model.plotOn(frame,Components("gaussian1"),LineColor(8),LineWidth(4),LineStyle(2));
  model.plotOn(frame,Components("gaussian2"),LineColor(5),LineWidth(4),LineStyle(2));
  model.plotOn(frame,Components("bkg"),LineColor(6),LineWidth(4),LineStyle(2));

  TCanvas c; c.cd();
  frame->Draw();
  c.SaveAs("nominal_fit.png");

  TFile file("RooWorkspaceStorage.root", "RECREATE");
  w->import(model);
  w->Write();
  file.Close();
  
  // Instantiate RooMCStudy manager on model with mass as observable and given choice of fit options
  //
  // The Silence() option kills all messages below the PROGRESS level, leaving only a single message
  // per sample executed, and any error message that occur during fitting
  //
  // The Extended() option has two effects: 
  //    1) The extended ML term is included in the likelihood and 
  //    2) A poisson fluctuation is introduced on the number of generated events 
  //
  // The FitOptions() given here are passed to the fitting stage of each toy experiment.
  // If Save() is specified, the fit result of each experiment is saved by the manager  
  //
  // A Binned() option is added in this example to bin the data between generation and fitting
  // to speed up the study at the expemse of some precision


  TFile file_open("RooWorkspaceStorage.root", "OPEN");
  RooWorkspace *w_open = static_cast<RooWorkspace*>(file_open.Get("w"));
  RooRealVar mass_open = *(w_open->var("mass"));
  RooRealVar mean_open = *(w_open->var("mean"));
  RooRealVar sigma1_open = *(w_open->var("sigma1"));
  RooAbsPdf *model_open = w_open->pdf("model");
  
  RooMCStudy* mcstudy = new RooMCStudy(*model_open,mass_open,/*FitModel()*/Binned(kTRUE),Silence(),Extended(),FitOptions(Save(kTRUE),PrintEvalErrors(0)));
  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  std::cout << sigma1_open.getVal() << std::endl;
  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  // Generate and fit 1000 samples of Poisson(nExpected) events
  mcstudy->generateAndFit(1000);

  // Make plots of the distributions of mean, the error on mean and the pull of mean
  RooPlot* frame1 = mcstudy->plotParam(mean_open,Bins(40));
  RooPlot* frame2 = mcstudy->plotPull(sigma1_open,Bins(40),FitGauss(kTRUE));
  RooPlot* frame3 = mcstudy->plotPull(mean_open,Bins(40),FitGauss(kTRUE));

  // Plot distribution of minimized likelihood
  RooPlot* frame4 = mcstudy->plotNLL(Bins(40));

 std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  std::cout << sigma1_open.getVal() << std::endl;
  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;

  // Draw all plots on a canvas
  gStyle->SetPalette(1);
  gStyle->SetOptStat(0);
  TCanvas* c2 = new TCanvas("toyMC_test","toyMC_test",900,900);
  c2->Divide(2,2);
  c2->cd(1) ; gPad->SetLeftMargin(0.15) ; frame1->GetYaxis()->SetTitleOffset(1.4) ; frame1->Draw() ;
  c2->cd(2) ; gPad->SetLeftMargin(0.15) ; frame2->GetYaxis()->SetTitleOffset(1.4) ; frame2->Draw() ;
  c2->cd(3) ; gPad->SetLeftMargin(0.15) ; frame3->GetYaxis()->SetTitleOffset(1.4) ; frame3->Draw() ;
  c2->cd(4) ; gPad->SetLeftMargin(0.15) ; frame4->GetYaxis()->SetTitleOffset(1.4) ; frame4->Draw() ;

  c2->SaveAs("fitBias.png");  
}
