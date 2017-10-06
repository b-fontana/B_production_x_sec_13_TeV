#ifndef BDEFINITIONS_H
#define BDEFINITIONS_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <TF1.h>
#include <TCanvas.h>
#include <TStyle.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TPaveText.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TNtupleD.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <TLegend.h>
#include <TSystem.h>
#include <RooWorkspace.h>
#include <RooRealVar.h>
#include <RooConstVar.h>
#include <RooDataSet.h>
#include <RooGaussian.h>
#include <RooChebychev.h>
#include <RooBernstein.h>
#include <RooExponential.h>
#include <RooCBShape.h>
#include <RooAddPdf.h>
#include <RooPlot.h>
#include "TMath.h"
#include "RooExtendPdf.h"
#include "TFitResult.h"
#include "RooPolynomial.h"
#include "TBranch.h"
using namespace RooFit;
#include "RooStats/SPlot.h"
#include "RooProdPdf.h"
#include "RooAddition.h"
#include "RooProduct.h"
#include "RooAbsPdf.h"
#include "RooFit.h"
#include "RooFitResult.h"

extern std::vector<std::string> VAR_LIST;

extern const int VAR_NUMBER;
extern const int BIN_NUMBER;

extern const double KSTAR_MASS;
extern const double PHI_MASS;
extern const double JPSI_MASS;
extern const double BP_MASS;
extern const double B0_MASS;
extern const double BS_MASS;

class BDefinitions {
 public:
  BDefinitions() {};
  int VarID(std::string);
  std::pair<double,double> SelectionVarRange(std::string var, int channel);
};

#endif
