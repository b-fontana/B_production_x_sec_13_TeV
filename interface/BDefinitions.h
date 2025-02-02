#ifndef BDEFINITIONS_H
#define BDEFINITIONS_H

#include <sstream>
#include <vector>
#include <string>

std::vector<std::string> VAR_LIST = {"pt","y","mu1pt","mu2pt","mu1eta","mu2eta","tk1pt","tk2pt","tk1eta","tk2eta","lxy","errxy","vtxprob","cosalpha2d","lerrxy","propert","tktkmass","mass"};
const int VAR_NUMBER = VAR_LIST.size();
const int BIN_NUMBER = 75;

class BDefinitions {
 public:
  BDefinitions() {};
  int VarID(std::string);
  std::pair<double,double> SelectionVarRange(std::string var, int channel);
};

int BDefinitions::VarID(std::string s)
{
  int id = -1;
  for (int i=0; i<VAR_NUMBER; ++i)
      if(s.compare(VAR_LIST.at(i))==0) id=i;
  return id;
}

std::pair<double,double> BDefinitions::SelectionVarRange(std::string var, int channel)
{
  std::pair<double,double> pair; //pair which contains <min,max> of the range of the variables
  if (var.compare("pt")==0) pair = std::make_pair(7.,90.); 
  else if (var.compare("y")==0) pair = std::make_pair(-2.9,2.9); 
  else if (var.compare("mu1pt")==0) pair = std::make_pair(3.,35.); 
  else if (var.compare("mu2pt")==0) pair = std::make_pair(3.,35.); 
  else if (var.compare("mu1eta")==0) pair = std::make_pair(-2.9,2.9);
  else if (var.compare("mu2eta")==0) pair = std::make_pair(-2.9,2.9); 
  else if (var.compare("tk1pt")==0) pair = std::make_pair(0.5,13.);
  else if (var.compare("tk2pt")==0) pair = std::make_pair(0.5,13.); 
  else if (var.compare("tk1eta")==0) pair = std::make_pair(-2.9,2.9);
  else if (var.compare("tk2eta")==0) pair = std::make_pair(-2.9,2.9); 
  else if (var.compare("lxy")==0) pair = std::make_pair(0.,0.6); 
  else if (var.compare("errxy")==0) pair = std::make_pair(0.,0.025);
  else if (var.compare("vtxprob")==0) pair = std::make_pair(0.05,1.05);
  else if (var.compare("cosalpha2d")==0) pair = std::make_pair(0.999996,1.);
  else if (var.compare("lerrxy")==0) pair = std::make_pair(0.,110.);
  else if (var.compare("propert")==0) pair = std::make_pair(0.,0.2);
  else if (var.compare("tktkmass")==0)
    {
    switch(channel) {
    case 1: pair = std::make_pair(0.,1.); break;
    case 2: pair = std::make_pair(0.65,1.2); break;
    case 3: pair = std::make_pair(0.,1.); break;
    case 4: pair = std::make_pair(0.99,1.06); break;
    case 5: pair = std::make_pair(0.,1.); break;
    case 6: pair = std::make_pair(0.,1.); break;
    }
  }
  else if (var.compare("mass")==0)
    {
    switch(channel) {
    case 1: pair = std::make_pair(5.15,5.38); break;
    case 2: pair = std::make_pair(5.12,5.45); break;
    case 3: pair = std::make_pair(5.15,5.38); break;
    case 4: pair = std::make_pair(5.23,5.52); break;
    case 5: pair = std::make_pair(5.15,5.38); break;
    case 6: pair = std::make_pair(5.15,5.38); break;
    }
  }
  return pair;
}


#endif

