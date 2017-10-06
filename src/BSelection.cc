#include "UserCode/B_production_x_sec_13_TeV/interface/BSelection.h"
#include <string>
#include <iostream>

getCuts_struct BSelection::getCuts(std::string var) {

  getCuts_struct getCuts_struct_obj;
  if (var == "pt") {
    getCuts_struct_obj.lower_cut = 0.;
    getCuts_struct_obj.upper_cut = 200.;
  }
  else if (var == "y") {
    getCuts_struct_obj.lower_cut = -4.;
    getCuts_struct_obj.upper_cut = 4.;
  }
  else if (var == "mu1pt") {
    getCuts_struct_obj.lower_cut = 4.;
    getCuts_struct_obj.upper_cut = 200.;
  }
  else if (var == "mu2pt") {
    getCuts_struct_obj.lower_cut = 4.;
    getCuts_struct_obj.upper_cut = 200.;
  }
  else if (var == "mu1eta") {
    getCuts_struct_obj.lower_cut = -2.4;
    getCuts_struct_obj.upper_cut = 2.4;
  }
  else if (var == "mu2eta") {
    getCuts_struct_obj.lower_cut = -2.4;
    getCuts_struct_obj.upper_cut = 2.4;
  }
  else if (var == "tk1pt") {
    getCuts_struct_obj.lower_cut = 0.7;
    getCuts_struct_obj.upper_cut = 200.;
  }
  else if (var == "tk2pt") {
    getCuts_struct_obj.lower_cut = 0.7;
    getCuts_struct_obj.upper_cut = 200;
  }
  else if (var == "tk1eta") {
    getCuts_struct_obj.lower_cut = -2.5;
    getCuts_struct_obj.upper_cut = 2.5;
  }
  else if (var == "tk2eta") {
    getCuts_struct_obj.lower_cut = -2.5;
    getCuts_struct_obj.upper_cut = 2.5;
  }
  else if (var == "lxy") {
    getCuts_struct_obj.lower_cut = 0.;
    getCuts_struct_obj.upper_cut = 100.;
  }
  else if (var == "errxy") {
    getCuts_struct_obj.lower_cut = 0;
    getCuts_struct_obj.upper_cut = 100.;
  }
  else if (var == "vtxprob") {
    getCuts_struct_obj.lower_cut = 0.2;
    getCuts_struct_obj.upper_cut = 1.;
  }
  else if (var == "cosalpha2d") {
    getCuts_struct_obj.lower_cut = 0.996;
    getCuts_struct_obj.upper_cut = 1.;
  }
  else if (var == "lerrxy") {
    getCuts_struct_obj.lower_cut = 3.;
    getCuts_struct_obj.upper_cut = 200.;
  }
  else if (var == "propert") {
    getCuts_struct_obj.lower_cut = 0.;
    getCuts_struct_obj.upper_cut = 100.;
  }
  else if (var == "mass") {
    getCuts_struct_obj.lower_cut = 0.;
    getCuts_struct_obj.upper_cut = 100.;
  }
  else {
    getCuts_struct_obj.lower_cut = 0.;
    getCuts_struct_obj.upper_cut = 0.;
  }

  return getCuts_struct_obj;
}
