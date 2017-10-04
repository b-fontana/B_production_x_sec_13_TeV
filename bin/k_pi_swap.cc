//this sript is used to estimate the ammout of "signal" in the B0->jpsi K*0 channel, in which the k and pi are swapped.
//for this we look at an MC sample processed using myloop_new .cc where we separate between true signal and swapped signal.
//here we fit each of these two categories separatly and extract the relative magnitude and width of each gaussian.
//This is then introduced in the pdf to extract the signal of B0->jpsi K*0, in other sripts in the cross sections studies.

#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"

using namespace RooFit;

int main(int argc, char** argv)
{
  std::string output ="";
  int log = 0;

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--output")
        {
          convert << argv[++i];
          convert >> output;
        }
      if(argument == "--log")
        {
          convert << argv[++i];
         convert >> log;
        }
    }

  double mass_window = 0.5;
  double mass_min = B0_MASS - mass_window;
  double mass_max = B0_MASS + mass_window;
  double pt_min = 0;
  double pt_max = 300;
  double y_min = -3;
  double y_max = 3;

  RooRealVar mass("mass","mass",mass_min,mass_max);
  RooRealVar pt("pt","pt",pt_min,pt_max);
  RooRealVar y("y","y",y_min,y_max);
  
  TString input_file = TString::Format(BASE_DIR) + "myloop_new_mc_truth_ntkstar_with_cuts.root";

  TFile *fin = new TFile(input_file);
  TString directory = "";
  
  //set the ntuple name to extract the full signal and put it in RooDataSet data_full
  TString nt = "ntkstar";
  TTree *tr = (TTree*)fin->Get(nt);
  RooDataSet* data_full = new RooDataSet("data_full","data_full", tr, RooArgSet(mass,pt,y) );

  //set the ntuple name to extract the true signal and put it in RooDataset data_signal
  nt = "ntkstar_true";
  tr = (TTree*)fin->Get(nt);
  RooDataSet* data_signal = new RooDataSet("data_signal","data_signal", tr, RooArgSet(mass,pt,y) );

  //set the ntuple name to extract the swapped signal and put it in RooDataset data_swapped
  nt = "ntkstar_swap";
  tr = (TTree*)fin->Get(nt);
  RooDataSet* data_swapped = new RooDataSet("data_swapped","data_swapped", tr, RooArgSet(mass,pt,y) );
  
  //create histograms
  TH1D* histo_full = (TH1D*)data_full->createHistogram("histo_full", mass, Binning(channel_to_nbins(2), mass.getMin(), mass.getMax() ));
  TH1D* histo_signal = (TH1D*)data_signal->createHistogram("histo_signal", mass, Binning(channel_to_nbins(2), mass.getMin(), mass.getMax() ));
  TH1D* histo_swapped = (TH1D*)data_swapped->createHistogram("histo_swapped", mass, Binning(channel_to_nbins(2), mass.getMin(), mass.getMax() ));
  
  for (int i=1; i<=channel_to_nbins(2); i++)
    {
      if (histo_full->GetBinContent(i)==0) histo_full->SetBinError(i,0.);
      if (histo_signal->GetBinContent(i)==0) histo_signal->SetBinError(i,0.);
      if (histo_swapped->GetBinContent(i)==0) histo_swapped->SetBinError(i,0.);
    }
  
  //========make fits of each category===========

  RooRealVar mean("mean","mean", B0_MASS);
  mean.setConstant(kTRUE);
  
  /*
    In the case we need to detail the background:
    RooRealVar m_exp("m_exp","m_exp",-4. ,-6., 0.);
    RooExponential combinatorial_exp("combinatorial_exp","combinatorial_exp", mass, m_exp);
  */

  //true signal fit
  RooRealVar n1_signal("n1_signal","n1_signal",250000,0,500000);
  RooRealVar n2_signal("n2_signal","n2_signal",100000,0,500000);
  RooRealVar sigma1_signal("#sigma_{1,signal}","sigma1_signal", 0.01, 0.001, 0.3);
  RooRealVar sigma2_signal("#sigma_{2,signal}","sigma2_signal", 0.03, 0.001, 0.3);
  RooGaussian gauss1_signal("gauss1_signal","gauss1_signal", mass, mean, sigma1_signal);
  RooGaussian gauss2_signal("gauss2_signal","gauss2_signal", mass, mean, sigma2_signal);
  RooAddPdf signal("signal","signal", RooArgList(gauss1_signal,gauss2_signal), RooArgList(n1_signal,n2_signal));

  /*mypdf=RooAddPdf("mypdf","mypdf",RooArgList(pdf1,pdf2),RooArgList(n1,n2))

    with :
    n1=RooRealVar("n1","n1",170000,0,500000)
    n2=RooRealVar("n2","n2",320000,0,500000)*/ 
  
  RooPlot* frame1 = mass.frame(Title("True signal fit"));
  data_signal->plotOn(frame1,Name("theSignal"),Binning(channel_to_nbins(2)));
    
  signal.fitTo(*data_signal);
  signal.paramOn(frame1, Layout(0.6,0.96,0.8));
  signal.plotOn(frame1, LineColor(7), LineWidth(4), LineStyle(1));
  signal.plotOn(frame1,Components("gauss1_signal"),LineColor(8),LineWidth(4),LineStyle(2));
  signal.plotOn(frame1,Components("gauss2_signal"),LineColor(5),LineWidth(4),LineStyle(2));
    
  TCanvas c1;
  if(log)
  c1.SetLogy();
  c1.cd();
  frame1->Draw();
  directory = output + "mass_signal.png";
  c1.SaveAs(directory);

  n1_signal.setConstant(kTRUE);
  n2_signal.setConstant(kTRUE);
  sigma1_signal.setConstant(kTRUE);
  sigma2_signal.setConstant(kTRUE);

  //swapped signal fit
  RooRealVar n1_swapped("n1_swapped","n1_swapped",250000,0,500000);
  RooRealVar n2_swapped("n2_swapped","n2_swapped",100000,0,500000);
  //RooRealVar nexp_swapped("nexp_swapped","nexp_swapped",170000,0,500000);
  RooRealVar sigma1_swapped("#sigma_{1,swapped}","sigma1_swapped",0.016, 0.001, 0.5);
  RooRealVar sigma2_swapped("#sigma_{2,swapped}","sigma2_swapped",0.015, 0.001, 0.5);
  RooRealVar alpha1("#alpha_{1}","alpha1", B0_MASS, 0.01, B0_MASS+6);
  RooRealVar alpha2("#alpha_{2}","alpha2", B0_MASS, 0.01, B0_MASS+6);
  RooRealVar n1_parameter("n1_parameter", "n1_parameter", 1., 0.01, 50.0);
  RooRealVar n2_parameter("n2_parameter", "n2_parameter", 1., 0.01, 50.0);  
  RooCBShape crystball1_swapped("crystball1_swapped","crystball1_swapped", mass, mean, sigma1_swapped, alpha1, n1_parameter);
  RooCBShape crystball2_swapped("crystball2_swapped","crystball2_swapped", mass, mean, sigma2_swapped, alpha2, n2_parameter);
  //RooRealVar lambda("lambda","lambda",-2.,-7.,0.);
  //RooExponential exp_swapped("exp_swapped","exp_swapped",mass,lambda);
  RooAddPdf swapped("swapped","swapped", RooArgList(crystball1_swapped,crystball2_swapped), RooArgList(n1_swapped,n2_swapped));

  RooPlot* frame2 = mass.frame(Title("Swapped signal fit"));
  data_swapped->plotOn(frame2,Name("theSwapped"),Binning(channel_to_nbins(2)));
  
  swapped.fitTo(*data_swapped);
  swapped.paramOn(frame2, Layout(0.7,0.99,0.99));
  frame2->getAttText()->SetTextSize(0.035);
  swapped.plotOn(frame2, LineColor(7), LineWidth(4), LineStyle(1));
  swapped.plotOn(frame2,Components("crystball1_swapped"),LineColor(8),LineWidth(4),LineStyle(2));
  swapped.plotOn(frame2,Components("crystball2_swapped"),LineColor(5),LineWidth(4),LineStyle(2));
  //swapped.plotOn(frame2,Components("exp_swapped"),LineColor(7),LineWidth(4),LineStyle(2));
 
  TCanvas c2;
  if(log)
    c2.SetLogy();
  c2.cd();
  frame2->Draw();
  directory = output + "mass_swapped.png";
  c2.SaveAs(directory);

  //set signal and swapped fits constant
  n1_swapped.setConstant(kTRUE);
  n2_swapped.setConstant(kTRUE);
  sigma1_swapped.setConstant(kTRUE);
  sigma2_swapped.setConstant(kTRUE);
  alpha1.setConstant(kTRUE);
  alpha2.setConstant(kTRUE);
  n1_parameter.setConstant(kTRUE);
  n2_parameter.setConstant(kTRUE);

  //full fit
  RooRealVar r_final("r_final","r_final",0.15,0.0,1);  
  RooAddPdf full("full","full", RooArgList(swapped,signal), r_final);

  RooPlot* frame3 = mass.frame(Title("Full signal fit"));
  data_full->plotOn(frame3,Name("theFull"),Binning(channel_to_nbins(2)));
  
  full.fitTo(*data_full);
  full.paramOn(frame3/*, Parameters(RooArgSet(r_final))*/,Layout(0.58,0.88,0.8));
  full.plotOn(frame3, LineColor(7), LineWidth(4), LineStyle(1));
  full.plotOn(frame3,Components("swapped"),LineColor(8),LineWidth(4),LineStyle(2));
  full.plotOn(frame3,Components("signal"),LineColor(5),LineWidth(4),LineStyle(2));
  
  TCanvas c3;
  if(log)
    c3.SetLogy();
  c3.cd();
  frame3->Draw();
  directory = output + "mass_full.png";
  c3.SaveAs(directory);
  double number_signal = (double)data_signal->sumEntries();
  double number_swap   = (double)data_swapped->sumEntries();
  std::cout << "Counting the entries" << std::endl;
  std::cout << "Signal events: " << number_signal << std::endl;
  std::cout << "Swapped events: " << number_swap << std::endl;
  std::cout << "Fraction of swapped events: " << number_swap/(number_swap+number_signal) << std::endl;
  std::cout << " " << std::endl;
  
  std::cout << "Numbers from the fits" << std::endl;
  //std::cout << "ratios:" << " r1" << r1.getVal() << " r2" << r2.getVal() << std::endl;
  std::cout << "Signal events: " << n1_signal.getValV() + n2_signal.getValV() << std::endl;
  std::cout << "Swapped events: " << n1_swapped.getValV() + n2_swapped.getValV() << std::endl;
  std::cout << "Fraction of swapped events: " << (n1_swapped.getVal()+n2_swapped.getVal())/(n1_signal.getVal()+n2_signal.getVal()+n1_swapped.getVal()+n2_swapped.getVal()) << std::endl;
  std::cout << " " << std::endl;

  std::cout << "O número de eventos no data_full é igual à soma dos outros dois conjuntos de dados: " << data_swapped->sumEntries()+data_signal->sumEntries() << "----" << data_full->sumEntries() << std::endl;
  std::cout << " " << std::endl;

  double res_signal =  TMath::Sqrt(TMath::Power(sigma1_signal.getValV(),2)+TMath::Power(sigma2_signal.getValV(),2));
  double res_swapped = TMath::Sqrt(TMath::Power(sigma1_swapped.getValV(),2)+TMath::Power(sigma2_swapped.getValV(),2));
  std::cout << "Resolução" << std::endl;
  std::cout << "Signal: " << res_signal << std::endl;
  std::cout << "Swapped: " << res_swapped << std::endl;
  std::cout << "Total: " << TMath::Sqrt(TMath::Power(res_signal,2)+TMath::Power(res_swapped,2)) << std::endl;

  delete data_signal;
  delete data_swapped;
  delete data_full;
  delete fin;
}
