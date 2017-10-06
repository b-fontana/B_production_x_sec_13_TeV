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

#endif
