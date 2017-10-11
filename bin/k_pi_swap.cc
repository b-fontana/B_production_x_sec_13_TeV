//this script is used to estimate the ammout of "signal" in the B0->jpsi K*0 channel, in which the k and pi are swapped.
//for this we look at an MC sample where we separate between true signal and swapped signal.
//here we fit each of these two categories separately and extract the relative magnitude and width of the pdf's.
//This is then introduced in the pdf to extract the signal of B0->jpsi K*0, in other sripts in the cross sections studies.

#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"

using namespace RooFit;

//Example: k_pi_swap --output name

int main(int argc, char** argv)
{
  std::string output = "";

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--output")
        {
          convert << argv[++i];
          convert >> output;
        }
    }

  double mass_min = 4.79;
  double mass_max = 5.70;
  double pt_min = 0;
  double pt_max = 300;
  double y_min = -3;
  double y_max = 3;

  RooRealVar mass("mass","mass",mass_min,mass_max);
  RooRealVar pt("pt","pt",pt_min,pt_max);
  RooRealVar y("y","y",y_min,y_max);
  
  TString input_file = TString::Format(BASE_DIR) + "/new_inputs/myloop_new_mc_truth_ntkstar_with_cuts.root";

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
  TH1D* histo_full = (TH1D*)data_full->createHistogram("histo_full", mass, Binning(100, mass.getMin(), mass.getMax() ));
  TH1D* histo_signal = (TH1D*)data_signal->createHistogram("histo_signal", mass, Binning(100, mass.getMin(), mass.getMax() ));
  TH1D* histo_swapped = (TH1D*)data_swapped->createHistogram("histo_swapped", mass, Binning(100, mass.getMin(), mass.getMax() ));
  
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
  RooRealVar n1_signal("n1_signal","n1_signal",200000,0,500000);
  RooRealVar n2_signal("n2_signal","n2_signal",100000,0,500000);
  RooRealVar n3_signal("n3_signal","n3_signal",100000,0,500000);

  RooRealVar sigma1_signal("#sigma_{1,signal}","sigma1_signal", 0.014, 0.001, 0.150);
  RooRealVar sigma2_signal("#sigma_{2,signal}","sigma2_signal", 0.040, 0.001, 0.150);
  RooRealVar sigma3_signal("#sigma_{3,signal}","sigma3_signal", 0.040, 0.001, 0.100);

  RooRealVar alpha1_signal("#alpha_{1,signal}","alpha1_signal", B0_MASS, 0.01000, B0_MASS+6.00000);
  RooRealVar alpha2_signal("#alpha_{2,signal}","alpha2_signal", B0_MASS, 0.01000, B0_MASS+6.00000);
  RooRealVar alpha3_signal("#alpha_{3,signal}","alpha3_signal", -B0_MASS, -4.00000*B0_MASS, -0.01000);
  RooRealVar n1_parameter_signal("n1_parameter_signal", "n1_parameter", 14.8, 0.01, 15.0);
  RooRealVar n2_parameter_signal("n2_parameter", "n2_parameter_signal", 1.906, 0.01, 15.0);  
  RooRealVar n3_parameter_signal("n3_parameter", "n3_parameter_signal", 3.14, 0.01, 15.0);
  RooCBShape crystball1_signal("crystball1_signal","crystball1_signal", mass, mean, sigma1_signal, alpha1_signal, n1_parameter_signal);
  RooCBShape crystball2_signal("crystball2_signal","crystball2_signal", mass, mean, sigma2_signal, alpha2_signal, n2_parameter_signal);
  RooCBShape crystball3_signal("crystball3_signal","crystball3_signal", mass, mean, sigma3_signal, alpha3_signal, n3_parameter_signal);
  RooAddPdf signal("signal","signal", RooArgList(crystball1_signal,crystball2_signal,crystball3_signal), RooArgList(n1_signal,n2_signal,n3_signal));
  
  RooPlot* frame1 = mass.frame(Title("True signal fit"));
  data_signal->plotOn(frame1,Name("theSignal"),Binning(100));
    
  signal.fitTo(*data_signal);
  signal.paramOn(frame1, Layout(0.65,0.99,0.99));
  frame1->getAttText()->SetTextSize(0.027);
  signal.plotOn(frame1, Name("thePdf1"), LineColor(7), LineWidth(1), LineStyle(1));
  signal.plotOn(frame1,Components("crystball1_signal"),LineColor(8),LineWidth(1),LineStyle(2));
  signal.plotOn(frame1,Components("crystball2_signal"),LineColor(5),LineWidth(1),LineStyle(2));  

  TCanvas c1;
  c1.SetLogy();
  c1.cd();
  frame1->Draw();

  double chi_square1 = frame1->chiSquare("thePdf1","theSignal");
  TLatex* tex1 = new TLatex(0.17, 0.97, Form("#chi^{2} = %.3lf", chi_square1));
  tex1->SetNDC(kTRUE);
  tex1->SetTextFont(42);
  tex1->SetTextSize(0.035);  
  tex1->Draw();

  directory = output + "mass_signal_logy.png";
  c1.SaveAs(directory);

  TCanvas c1_new;
  c1_new.cd();
  frame1->Draw();

  double chi_square1_new = frame1->chiSquare("thePdf1","theSignal");
  TLatex* tex1_new = new TLatex(0.17, 0.6, Form("#chi^{2} = %.3lf", chi_square1_new));
  tex1_new->SetNDC(kTRUE);
  tex1_new->SetTextFont(42);
  tex1_new->SetTextSize(0.035);  
  tex1_new->Draw();

  directory = output + "mass_signal.png";
  c1_new.SaveAs(directory);  

  n1_signal.setConstant(kTRUE);
  n2_signal.setConstant(kTRUE);
  n3_signal.setConstant(kTRUE);
  sigma1_signal.setConstant(kTRUE);
  sigma2_signal.setConstant(kTRUE);
  sigma3_signal.setConstant(kTRUE);
  n1_parameter_signal.setConstant(kTRUE);
  n2_parameter_signal.setConstant(kTRUE);
  n3_parameter_signal.setConstant(kTRUE);
  alpha1_signal.setConstant(kTRUE);
  alpha2_signal.setConstant(kTRUE);
  alpha3_signal.setConstant(kTRUE);

  //swapped signal fit
  RooRealVar n1_swapped("n1_swapped","n1_swapped",40000,0,200000);
  RooRealVar n2_swapped("n2_swapped","n2_swapped",40000,0,200000);
  RooRealVar n3_swapped("n3_swapped","n3_swapped",20000,0,100000);
  RooRealVar sigma1_swapped("#sigma_{1,swapped}","sigma1_swapped",0.110, 0.005, 0.200);
  RooRealVar sigma2_swapped("#sigma_{2,swapped}","sigma2_swapped",0.030, 0.005, 0.150);
  RooRealVar sigma3_swapped("#sigma_{3,swapped}","sigma3_swapped",0.030, 0.001, 0.100);
  RooRealVar alpha1("#alpha_{1}","alpha1", B0_MASS, 0.01000, B0_MASS+6.00000);
  RooRealVar alpha2("#alpha_{2}","alpha2", B0_MASS, 0.01000, B0_MASS+6.00000);
  RooRealVar alpha3("#alpha_{3}","alpha3", -B0_MASS, -4.00000*B0_MASS, 0.00000);
  RooRealVar n1_parameter_swapped("n1_parameter_swapped", "n1_parameter_swapped", 7.00, 0.01, 25.11);
  RooRealVar n2_parameter_swapped("n2_parameter_swapped", "n2_parameter_swapped", 12.88, 0.01, 15.00);  
  RooRealVar n3_parameter_swapped("n3_parameter_swapped", "n3_parameter_swapped", 183.00, 50.00, 250.00);

  RooCBShape crystball1_swapped("crystball1_swapped","crystball1_swapped", mass, mean, sigma1_swapped, alpha1, n1_parameter_swapped);
  RooCBShape crystball2_swapped("crystball2_swapped","crystball2_swapped", mass, mean, sigma2_swapped, alpha2, n2_parameter_swapped);
  RooCBShape crystball3_swapped("crystball3_swapped","crystball3_swapped", mass, mean, sigma3_swapped, alpha3, n3_parameter_swapped);

  RooAddPdf swapped("swapped","swapped", RooArgList(crystball1_swapped,crystball2_swapped,crystball3_swapped), RooArgList(n1_swapped,n2_swapped,n3_swapped));

  RooPlot* frame2 = mass.frame(Title("Swapped signal fit"));
  data_swapped->plotOn(frame2,Name("theSwapped"),Binning(100)); 
  swapped.fitTo(*data_swapped);
  swapped.paramOn(frame2, Layout(0.65,0.99,0.99));
  frame2->getAttText()->SetTextSize(0.027);
  swapped.plotOn(frame2, Name("thePdf2"), LineColor(7), LineWidth(1), LineStyle(1));
  swapped.plotOn(frame2,Components("crystball1_swapped"),LineColor(8),LineWidth(1),LineStyle(2));
  swapped.plotOn(frame2,Components("crystball2_swapped"),LineColor(5),LineWidth(1),LineStyle(2));
  swapped.plotOn(frame2,Components("crystball3_swapped"),LineColor(6),LineWidth(1),LineStyle(2));
 
  TCanvas c2;
  c2.SetLogy();
  c2.cd();
  frame2->Draw();

  double chi_square2 = frame2->chiSquare("thePdf2","theSwapped");
  TLatex* tex2 = new TLatex(0.17, 0.6, Form("#chi^{2} = %.3lf", chi_square2));
  tex2->SetNDC(kTRUE);
  tex2->SetTextFont(42);
  tex2->SetTextSize(0.035);
  tex2->Draw();

  directory = output + "mass_swapped_logy.png";
  c2.SaveAs(directory);

  TCanvas c2_new;
  c2_new.cd();
  frame2->Draw();

  double chi_square2_new = frame2->chiSquare("thePdf2","theSwapped");
  TLatex* tex2_new = new TLatex(0.17, 0.6, Form("#chi^{2} = %.3lf", chi_square2_new));
  tex2_new->SetNDC(kTRUE);
  tex2_new->SetTextFont(42);
  tex2_new->SetTextSize(0.035);
  tex2_new->Draw();

  directory = output + "mass_swapped.png";
  c2_new.SaveAs(directory);

  //set signal and swapped fits constant
  n1_swapped.setConstant(kTRUE);
  n2_swapped.setConstant(kTRUE);
  n3_swapped.setConstant(kTRUE);
  sigma1_swapped.setConstant(kTRUE);
  sigma2_swapped.setConstant(kTRUE);
  sigma3_swapped.setConstant(kTRUE);
  alpha1.setConstant(kTRUE);
  alpha2.setConstant(kTRUE);
  alpha3.setConstant(kTRUE);
  n1_parameter_swapped.setConstant(kTRUE);
  n2_parameter_swapped.setConstant(kTRUE);
  n3_parameter_swapped.setConstant(kTRUE);

  //full fit
  RooRealVar r_final("r_final","r_final",0.15, 0.00, 1.00);  
  RooAddPdf full("full","full", RooArgList(swapped,signal), r_final);

  RooPlot* frame3 = mass.frame(Title("Full signal fit"));
  data_full->plotOn(frame3,Name("theFull"),Binning(100));

  full.fitTo(*data_full);
  full.paramOn(frame3/*, Parameters(RooArgSet(r_final))*/,Layout(0.58,0.88,0.8));
  frame3->getAttText()->SetTextSize(0.027);
  full.plotOn(frame3, Name("thePdf3"), LineColor(7), LineWidth(1), LineStyle(1));
  full.plotOn(frame3,Components("swapped"),LineColor(8),LineWidth(1),LineStyle(2));
  full.plotOn(frame3,Components("signal"),LineColor(5),LineWidth(1),LineStyle(2));

  TCanvas c3;
  c3.SetLogy();
  c3.cd();
  frame3->Draw();

  double chi_square3 = frame3->chiSquare("thePdf3","theFull");
  TLatex* tex3 = new TLatex(0.17, 0.8, Form("#chi^{2} = %.3lf", chi_square3));
  tex3->SetNDC(kTRUE);
  tex3->SetTextFont(42);
  tex3->SetTextSize(0.035);
  tex3->Draw();

  directory = output + "mass_full_logy.png";
  c3.SaveAs(directory);

  TCanvas c3_new;
  c3_new.cd();
  frame3->Draw();

  double chi_square3_new = frame3->chiSquare("thePdf3","theFull");
  TLatex* tex3_new = new TLatex(0.17, 0.8, Form("#chi^{2} = %.3lf", chi_square3_new));
  tex3_new->SetNDC(kTRUE);
  tex3_new->SetTextFont(42);
  tex3_new->SetTextSize(0.035);
  tex3_new->Draw();

  directory = output + "mass_full.png";
  c3_new.SaveAs(directory);

  double number_signal = (double)data_signal->sumEntries();
  double number_swap   = (double)data_swapped->sumEntries();
  std::cout << "Counting the entries:" << std::endl;
  std::cout << "Signal events: " << number_signal << std::endl;
  std::cout << "Swapped events: " << number_swap << std::endl;
  std::cout << "Fraction of swapped events: " << number_swap/(number_swap+number_signal) << std::endl;
  std::cout << " " << std::endl;
  
  std::cout << "Numbers from the fits:" << std::endl;

  std::cout << "Signal events: " << n1_signal.getVal() + n2_signal.getVal() + n3_signal.getVal() << std::endl;
  std::cout << "Swapped events: " << n1_swapped.getVal() + n2_swapped.getVal() + n3_swapped.getVal() << std::endl;
  std::cout << "Fraction of swapped events: " << (n1_swapped.getVal()+n2_swapped.getVal()+n3_swapped.getVal())/(n1_signal.getVal()+n2_signal.getVal()+n3_signal.getVal()+n1_swapped.getVal()+n2_swapped.getVal()+n3_swapped.getVal()) << ". Calculated with the fraction: " << r_final.getVal() << "." << std::endl;
  std::cout << " " << std::endl;
  
  std::cout << "O número de eventos no data_full é igual à soma dos outros dois conjuntos de dados: " << data_swapped->sumEntries()+data_signal->sumEntries() << "----" << data_full->sumEntries() << std::endl;
  std::cout << " " << std::endl;

  double res_signal =  TMath::Sqrt(TMath::Power(sigma1_signal.getVal(),2)+TMath::Power(sigma2_signal.getVal(),2)+TMath::Power(sigma3_signal.getVal(),2));
  double res_swapped = TMath::Sqrt(TMath::Power(sigma1_swapped.getVal(),2)+TMath::Power(sigma2_swapped.getVal(),2)+TMath::Power(sigma3_swapped.getVal(),2));
  std::cout << "Resolução:" << std::endl;
  std::cout << "Signal: " << res_signal << std::endl;
  std::cout << "Swapped: " << res_swapped << std::endl;
  std::cout << "Total: " << TMath::Sqrt(TMath::Power(res_signal,2)+TMath::Power(res_swapped,2)) << std::endl;

  std::cout << "Chi-squares: " << std::endl;
  std::cout << chi_square1 << std::endl;
  std::cout << chi_square2 << std::endl;
  std::cout << chi_square3 << std::endl;

  delete data_signal;
  delete data_swapped;
  delete data_full;
  delete fin;
}
