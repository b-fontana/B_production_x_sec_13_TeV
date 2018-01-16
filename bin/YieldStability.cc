#include <stdio.h>
#include <iostream>
#include "TGraphErrors.h"
#include "TCanvas.h"
#include "TText.h"
#include "TAxis.h"
#include "TLatex.h"
#include "TMath.h"

int main() {

  std::vector<std::string> labels = {"D1","D2","D3","D4","D5"};
  unsigned int n = labels.size();
  TCanvas *c1 = new TCanvas("c1","c1",10,10,1600,850);
  c1->Divide(2,2);
  
  //////B+////////////////////////////////////////////
  c1->cd(1); 

  std::vector<double> x1  = {1.,2.,3.,4.,5.};
  std::vector<double> N1 = {28373.000,37883.000,31043.000,15517.000,25832.000};
  std::vector<double> eN1 = {233.000,283.000,264.000,183.000,254.000};
  //std::vector<double> L1 = {0.200,0.542,0.935,0.321,0.585};
  std::vector<double> L1 = {0.325,0.727,0.625,0.332,0.574};
  std::vector<double> y1;
  for (unsigned int i=0; i<n; ++i) {
    y1.push_back(N1.at(i)/L1.at(i));
  }
  std::vector<double> ex1 = {0.,0.,0.,0.,0.};
  std::vector<double> ey1;
  for (unsigned int i=0; i<n; ++i) {
    ey1.push_back(y1.at(i) * TMath::Sqrt(TMath::Power(eN1.at(i)/N1.at(i),2) + TMath::Power(0.023,2)));
    std::cout << ey1.at(i) << std::endl;
  }
  
  TGraphErrors *gr1 = new TGraphErrors(n,&x1[0],&y1[0],&ex1[0],&ey1[0]);
  gr1->SetTitle("B^{+}");
  gr1->SetMarkerColor(30);
  gr1->SetMarkerStyle(21);
  gr1->GetXaxis()->SetLabelSize(0.);
  gr1->SetMinimum(20000);
  gr1->Draw("ALP");

  TLatex ytitle1;
  ytitle1.SetTextSize(0.05);
  ytitle1.DrawLatex(-0.05,85000.,"N/L");

  TText *t = new TText();
  t->SetTextAlign(32);
  t->SetTextSize(0.035);
  t->SetTextFont(72);
  for (unsigned int i=0; i<labels.size(); i++) {
    t->DrawText(x1.at(i), 30000, labels.at(i).c_str());
  }
  

  //////B0////////////////////////////////////////////
  c1->cd(2); 

  std::vector<double> x2  = {1.,2.,3.,4.,5.};
  std::vector<double> N2 = {11895.000,15482.000,12564.000,6051.000,9953.000};
  std::vector<double> eN2 = {142.000,163.000,150.000,105.000,132.000};
  //std::vector<double> L2 = {0.200,0.542,0.935,0.321,0.585};
  std::vector<double> L2 = {0.325,0.727,0.625,0.332,0.574};
  std::vector<double> y2;
  for (unsigned int i=0; i<n; ++i) {
    y2.push_back(N2.at(i)/L2.at(i));
  }
  std::vector<double> ex2 = {0.,0.,0.,0.,0.};
  std::vector<double> ey2;
  for (unsigned int i=0; i<n; ++i) {
    ey2.push_back(y2.at(i) * TMath::Sqrt(TMath::Power(eN2.at(i)/N2.at(i),2) + TMath::Power(0.023,2)));
    std::cout << ey2.at(i) << std::endl;
  }
  
  TGraphErrors *gr2 = new TGraphErrors(n,&x2[0],&y2[0],&ex2[0],&ey2[0]);
  gr2->SetTitle("B_{0}");
  gr2->SetMarkerColor(30);
  gr2->SetMarkerStyle(21);
  gr2->GetXaxis()->SetLabelSize(0.);
  gr2->SetMinimum(10000);
  gr2->Draw("ALP");

  TLatex ytitle2;
  ytitle2.SetTextSize(0.05);
  ytitle2.DrawLatex(-0.05,37000.,"N/L");

  for (unsigned int i=0; i<labels.size(); i++) {
    t->DrawText(x2.at(i), 13000, labels.at(i).c_str());
  }

  //////Bs////////////////////////////////////////////
  c1->cd(4); 

  std::vector<double> x4  = {1.,2.,3.,4.,5.};
  std::vector<double> N4 = {2103.000,2743.000,2065.000,1067.000,1741.000};
  std::vector<double> eN4 = {56.000,65.000,57.000,41.000,50.000};
  //std::vector<double> L4 = {0.200,0.542,0.935,0.321,0.585};
  std::vector<double> L4 = {0.325,0.727,0.625,0.332,0.574};
  std::vector<double> y4;
  for (unsigned int i=0; i<n; ++i) {
    y4.push_back(N4.at(i)/L4.at(i));
  }
  std::vector<double> ex4 = {0.,0.,0.,0.,0.};
  std::vector<double> ey4;
  for (unsigned int i=0; i<n; ++i) {
    ey4.push_back(y4.at(i) * TMath::Sqrt(TMath::Power(eN4.at(i)/N4.at(i),2) + TMath::Power(0.023,2)));
    std::cout << ey4.at(i) << std::endl;
  }
  
  TGraphErrors *gr4 = new TGraphErrors(n,&x4[0],&y4[0],&ex4[0],&ey4[0]);
  gr4->SetTitle("B_{s}");
  gr4->SetMarkerColor(30);
  gr4->SetMarkerStyle(21);
  gr4->GetXaxis()->SetLabelSize(0.);
  gr4->SetMinimum(1400);
  gr4->Draw("ALP");

  TLatex ytitle4;
  ytitle4.SetTextSize(0.045);
  ytitle4.DrawLatex(-0.05,6500.,"N/L");

  for (unsigned int i=0; i<labels.size(); i++) {
    t->DrawText(x4.at(i), 2100, labels.at(i).c_str());
  }

  c1->Update();
  c1->SaveAs("YieldStability.png");

  //////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////////////////////////
  TCanvas *c2 = new TCanvas("c2","c2",10,10,1050,850);
  c2->Divide(2,2); 

  /////////// B0/B+ //////////////////////////////////////////////
  c2->cd(1);
  std::vector<double> ratio1;
  for(unsigned int i=0; i<n; ++i) {
    ratio1.push_back(N2.at(i)/N1.at(i));
  }
  std::vector<double> eratio1;
  for (unsigned int i=0; i<n; ++i) {
    eratio1.push_back(ratio1.at(i) * TMath::Sqrt(TMath::Power(eN1.at(i)/N1.at(i),2) + TMath::Power(eN2.at(i)/N2.at(i),2)));
  }
  TGraphErrors *gr_ratio1 = new TGraphErrors(n,&x1[0],&ratio1[0],&ex1[0],&eratio1[0]);
  gr_ratio1->SetTitle("B^{0}/B^{+}");
  gr_ratio1->SetMarkerColor(38);
  gr_ratio1->SetMarkerStyle(21);
  gr_ratio1->GetXaxis()->SetLabelSize(0.);
  gr_ratio1->SetMinimum(0.495);
  gr_ratio1->Draw("ALP");

  TLatex ytitle_ratio1;
  ytitle_ratio1.SetTextSize(0.045);
  ytitle_ratio1.DrawLatex(-0.8,0.539,"#frac{N_{B^{0}}}{N_{B^{+}}}");
  
  for (unsigned int i=0; i<labels.size(); i++) {
    t->DrawText(x1.at(i), 0.505, labels.at(i).c_str());
  }

  /////////// B0/Bs //////////////////////////////////////////////
  c2->cd(2);
  std::vector<double> ratio2;
  for(unsigned int i=0; i<n; ++i) {
    ratio2.push_back(N2.at(i)/N4.at(i));
  }
  std::vector<double> eratio2;
  for (unsigned int i=0; i<n; ++i) {
    eratio2.push_back(ratio2.at(i) * TMath::Sqrt(TMath::Power(eN4.at(i)/N4.at(i),2) + TMath::Power(eN2.at(i)/N2.at(i),2)));
  }
  TGraphErrors *gr_ratio2 = new TGraphErrors(n,&x2[0],&ratio2[0],&ex2[0],&eratio2[0]);
  gr_ratio2->SetTitle("B^{0}/B_{s}");
  gr_ratio2->SetMarkerColor(38);
  gr_ratio2->SetMarkerStyle(21);
  gr_ratio2->GetXaxis()->SetLabelSize(0.);
  gr_ratio2->SetMinimum(5.40);
  gr_ratio2->Draw("ALP");

  TLatex ytitle_ratio2;
  ytitle_ratio2.SetTextSize(0.045);
  ytitle_ratio2.DrawLatex(-0.8,7.35,"#frac{N_{B^{0}}}{N_{B_{s}}}");
  
  for (unsigned int i=0; i<labels.size(); i++) {
    t->DrawText(x2.at(i), 6.75, labels.at(i).c_str());
  }


  /////////// Bs/B+ //////////////////////////////////////////////
  c2->cd(4);
  std::vector<double> ratio4;
  for(unsigned int i=0; i<n; ++i) {
    ratio4.push_back(N4.at(i)/N1.at(i));
  }
  std::vector<double> eratio4;
  for (unsigned int i=0; i<n; ++i) {
    eratio4.push_back(ratio1.at(i) * TMath::Sqrt(TMath::Power(eN1.at(i)/N1.at(i),2) + TMath::Power(eN4.at(i)/N4.at(i),2)));
  }
  TGraphErrors *gr_ratio4 = new TGraphErrors(n,&x4[0],&ratio4[0],&ex4[0],&eratio4[0]);
  gr_ratio4->SetTitle("B_{s}/B^{+}");
  gr_ratio4->SetMarkerColor(38);
  gr_ratio4->SetMarkerStyle(21);
  gr_ratio4->GetXaxis()->SetLabelSize(0.);
  gr_ratio4->SetMaximum(0.09);
  gr_ratio4->SetMinimum(0.055);
  gr_ratio4->Draw("ALP");

  TLatex ytitle_ratio4;
  ytitle_ratio4.SetTextSize(0.05);
  ytitle_ratio4.DrawLatex(-0.85,0.0885,"#frac{N_{B_{s}}}{N_{B^{+}}}");
  
  for (unsigned int i=0; i<labels.size(); i++) {
    t->DrawText(x4.at(i), 0.06, labels.at(i).c_str());
  }

  c2->Update();
  c2->SaveAs("YieldStability_ratios.png");

  double maxdev1 = ( ratio1.at(4) - (ratio1.at(0)+ratio1.at(1)+ratio1.at(2)+ratio1.at(3)+ratio1.at(4))/5 ) / ( (ratio1.at(0)+ratio1.at(1)+ratio1.at(2)+ratio1.at(3)+ratio1.at(4))/5 );
  double maxdev1_bruno = ( ratio1.at(4) - (ratio1.at(0)+ratio1.at(1)+ratio1.at(2)+ratio1.at(3)+ratio1.at(4))/5 ) / (2*(eratio1.at(0)+eratio1.at(1)+eratio1.at(2)+eratio1.at(3)+eratio1.at(4))/5 );

  double maxdev2 = ( ratio2.at(1) - (ratio2.at(0)+ratio2.at(1)+ratio2.at(2)+ratio2.at(3)+ratio2.at(4))/5 ) / ( (ratio2.at(0)+ratio2.at(1)+ratio2.at(2)+ratio2.at(3)+ratio2.at(4))/5 );
  double maxdev2_bruno = ( ratio2.at(1) - (ratio2.at(0)+ratio2.at(1)+ratio2.at(2)+ratio2.at(3)+ratio2.at(4))/5 ) / (2*(eratio2.at(0)+eratio2.at(1)+eratio2.at(2)+eratio2.at(3)+eratio2.at(4))/5 );

  double maxdev4 = ( ratio4.at(1) - (ratio4.at(0)+ratio4.at(1)+ratio4.at(2)+ratio4.at(3)+ratio4.at(4))/5 ) / ( (ratio4.at(0)+ratio4.at(1)+ratio4.at(2)+ratio4.at(3)+ratio4.at(4))/5 );
  double maxdev4_bruno = ( ratio4.at(1) - (ratio4.at(0)+ratio4.at(1)+ratio4.at(2)+ratio4.at(3)+ratio4.at(4))/5 ) / (2*(eratio4.at(0)+eratio4.at(1)+eratio4.at(2)+eratio4.at(3)+eratio4.at(4))/5 );

  std::cout << maxdev1 << "; " << maxdev1_bruno << std::endl;
  std::cout << maxdev2 << "; " << maxdev2_bruno << std::endl;
  std::cout << maxdev4 << "; " << maxdev4_bruno << std::endl;

 return 0;
}
