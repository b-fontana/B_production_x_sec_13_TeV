#ifndef BLOOKS_H
#define BLOOKS_H

#include <sstream>
#include <vector>
#include <string>
#include "TLatex.h"
#include "TLegend.h"
#include "TH1.h"

class BLooks {
 public:
  std::pair<TLatex*,TLatex*> StandardLatexEntries();
  TLatex* LatexEntry(std::pair<double,double>, std::string);  
  TLegend* LegendEntries(std::vector<double>, std::vector<TH1*>, std::vector<std::string>);
};

std::pair<TLatex*,TLatex*> BLooks::StandardLatexEntries()
{
  std::pair<TLatex*,TLatex*> pair;
  TLatex* tex1 = new TLatex(0.7,0.93,"2.71 fb^{-1} (13 TeV)");
  tex1->SetNDC(kTRUE);
  tex1->SetLineWidth(2);
  TLatex* tex2 = new TLatex(0.1,0.93,"CMS Preliminary");
  tex2->SetNDC(kTRUE);
  tex2->SetTextFont(42);
  tex2->SetLineWidth(2);
  return std::make_pair(tex1,tex2);
}

TLatex* BLooks::LatexEntry(std::pair<double,double> pos, std::string s)
{
  TLatex* tex = new TLatex(pos.first, pos.second, s.c_str());
  tex->SetNDC(kTRUE);
  tex->SetLineWidth(2);
  return tex;
}

TLegend* BLooks::LegendEntries(std::vector<double> pos, std::vector<TH1*> h, std::vector<std::string> s)
{
  if (pos.size()!=4 || h.size() != s.size()) std::cout << "ERROR [BLooks::LegendEntry()]" << std::endl;
  TLegend* leg = new TLegend(pos.at(0), pos.at(1), pos.at(2), pos.at(3));
  int n = h.size();
  for(int i=0; i<n; ++i)
    {
      leg->AddEntry(h.at(i), s.at(i).c_str(), "l");
    }
  leg->SetBorderSize(0);
  leg->SetTextSize(0.05);
  return leg;
}





#endif
