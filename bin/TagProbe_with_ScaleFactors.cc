//matrix information: http://www.boost.org/doc/libs/1_36_0/libs/numeric/ublas/doc/matrix.htm

#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <iostream>
#include <fstream>

using namespace RooFit;
double getWeight(TFile *f, double muon_eta, double muon_pt, std::string mode);
std::pair<int,int> currentBin(int nbins_y, int nbins_pt, double b_y, double b_pt, int channel);
std::pair<int,int> getBinpT(int channel, int bin_number);
std::pair<float,float> getBiny(int channel, int bin_number);
void printSystematics(int channel, std::string bins, TString ModeTString, TString name, std::vector<double>& systematics);

//Example: TPwSF --channel 4 --mode softmuonid --bins pt
int main(int argc, char** argv)
{
  int channel = 1;
  std::string mode = "";
  std::string bins = "";
  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;
      if(argument == "--channel")
        {
          convert << argv[++i];
          convert >> channel;
        }
      if(argument == "--mode")
        {
          convert << argv[++i];
          convert >> mode;
        }
      if(argument == "--bins")
        {
          convert << argv[++i];
          convert >> bins;
        }
    }

  if(mode == "" || bins == "") {
    std::cout << "Please insert the mode as one of the following string, preceded by '--mode': softmuonid, l1l2 or l3." << std::endl;
    std::cout << "--OR--" << std::endl;
    std::cout << "Please insert the bins as one of the following string, preceded by '--bins': pt or y." << std::endl;
    return 0;
  }

  int nbins_pt = 0, nbins_y = 0;
  switch(channel)
    {
    case 1: nbins_pt = sizeof(ntkp_pt_bins)/sizeof(*ntkp_pt_bins)-1;        nbins_y = sizeof(ntkp_y_bins)/sizeof(*ntkp_y_bins)-1;        break;
    case 2: nbins_pt = sizeof(ntkstar_pt_bins)/sizeof(*ntkstar_pt_bins)-1;  nbins_y = sizeof(ntkstar_y_bins)/sizeof(*ntkstar_y_bins)-1;  break;
    case 4: nbins_pt = sizeof(ntphi_pt_bins)/sizeof(*ntphi_pt_bins)-1;      nbins_y = sizeof(ntphi_y_bins)/sizeof(*ntphi_y_bins)-1;      break;
    default: std::cout << "The chosen channel is not supported." << std::endl;
    }

  //declare matrices and initialize their values to zero. The 'zero_matrix' possibility did not work.
  boost::numeric::ublas::matrix<double> weight_product(nbins_y, nbins_pt);
  boost::numeric::ublas::matrix<int> subsample_entries(nbins_y, nbins_pt);
  for(int ii=0; ii<nbins_pt; ++ii) {
    for(int jj=0; jj<nbins_y; ++jj) {
      weight_product(jj,ii) = 0.;
      subsample_entries(jj,ii) = 0;
    }
  }

  //Open the MC analysis file and the file with the Tag and Probe weights
  TString dir = "/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/final_selection/";
  TFile *f = new TFile(dir + "myloop_new_mc_truth_" + channel_to_ntuple_name(channel) + "_with_cuts.root", "OPEN");
  TTree *t = static_cast<TTree*>(f->Get(channel_to_ntuple_name(channel)));
  TString dir_w = "TnP/recipe_740/CMSSW_7_4_0/src/MuonAnalysis/TagAndProbe/test/jpsi/fitTreeAnalyzer/ratio_files/25ns/";
  TString ModeRootFileTString;
  if(mode == "softmuonid") ModeRootFileTString = "MuonID_Soft2012_25ns__pt_abseta_separated__MC_vs_DATA.root";
  else if(mode == "l1l2") ModeRootFileTString = "MuonID_Dimuon10_L1L2_25ns__pt_abseta_separated__MC_vs_DATA.root";
  else if(mode == "l3") ModeRootFileTString = "MuonID_L3_wrt_Dimuon10_L1L2_25ns__pt_abseta_separated__MC_vs_DATA.root";
  TFile *f_w = new TFile(dir_w + ModeRootFileTString, "OPEN");

  //Access the needed variables
  double b_pt, b_y, muon1_pt, muon1_eta, muon2_pt, muon2_eta;
  t->SetBranchAddress("pt",&b_pt);
  t->SetBranchAddress("y",&b_y);
  t->SetBranchAddress("mu1pt",&muon1_pt);
  t->SetBranchAddress("mu1eta",&muon1_eta);
  t->SetBranchAddress("mu2pt",&muon2_pt);
  t->SetBranchAddress("mu2eta",&muon2_eta);

  //Loop trough the events of the MC analysis file to calculate both the number of events in each (pT,y) subsample and to calculate the muon efficiency for each muon pair coming from a certain B meson. Since the systematical error is going to be an average calculate with these two values, the values are incremented and are not separately stored per event
  int mu1_count = 0, mu2_count = 0;
  std::pair<int,int> currBin = std::make_pair(0,0);
  for(int entry=0; entry<t->GetEntries(); ++entry)
    {
      //These two counters are checking how large the fraction of muons which do not have an associated weight is.
      if(muon1_pt>40. || muon1_eta>1.6) mu1_count += 1;
      if(muon2_pt>40. || muon2_eta>1.6) mu2_count += 1;

      t->GetEntry(entry);
      currBin = currentBin(subsample_entries.size1(),subsample_entries.size2(),b_y,b_pt,channel);
      subsample_entries(currBin.first,currBin.second) += 1;
      weight_product(currBin.first,currBin.second) += getWeight(f_w,muon1_eta,muon1_pt,mode)*getWeight(f_w,muon2_eta,muon2_pt,mode); 

    }
  
  std::cout << "Fraction of muon1 with pT larger than 40 GeV and pseudorapidity larger than 1.6: " <<
    static_cast<double>(mu1_count)/static_cast<double>(t->GetEntries()) << std::endl;
  std::cout << "Fraction of muon2 with pT larger than 40 GeV and pseudorapidity larger than 1.6: " <<
    static_cast<double>(mu2_count)/static_cast<double>(t->GetEntries()) << std::endl;

  //double differential: boost::numeric::ublas::matrix<double> systematics(nbins_y, nbins_pt);
  int nbins_syst = 0;
  if(bins == "pt") nbins_syst = nbins_pt;
  else if(bins == "y") nbins_syst = nbins_y;
  std::vector<double> systematics(nbins_syst);  

  //calculate the systematical uncertainty per bin of pT and rapidity
  int subsample_entries_sum = 0;
  double  weight_product_sum = 0;
  if(bins == "pt")
    {
      for(unsigned int i=0; i<weight_product.size2(); ++i) { //cycle over pT
	subsample_entries_sum = 0; 
	weight_product_sum = 0.;
	for (unsigned int j=0; j<weight_product.size1(); ++j) { //cycle over y
	  subsample_entries_sum += subsample_entries(j,i);
	  weight_product_sum += weight_product(j,i);
	}
	systematics.at(i) = (weight_product_sum/static_cast<double>(subsample_entries_sum) - 1);
      }
    }
  else if(bins == "y") 
    {
      for(unsigned int i=0; i<weight_product.size1(); ++i) { //cycle over y
	subsample_entries_sum = 0; 
	weight_product_sum = 0.;
	for (unsigned int j=0; j<weight_product.size2(); ++j) { //cycle over pt
	  subsample_entries_sum += subsample_entries(i,j);
	  weight_product_sum += weight_product(i,j);
	}
	systematics.at(i) = (weight_product_sum/static_cast<double>(subsample_entries_sum) - 1);
      }
    }
  else if(bins == "global")
    {
      subsample_entries_sum = 0; 
      weight_product_sum = 0.;
      for(unsigned int i=0; i<weight_product.size2(); ++i) { //cycle over pT
	for (unsigned int j=0; j<weight_product.size1(); ++j) { //cycle over y
	  subsample_entries_sum += subsample_entries(j,i);
	  weight_product_sum += weight_product(j,i);
	}
      }
      std::cout << "Global " << mode << "error: " << TMath::Abs(weight_product_sum/static_cast<double>(subsample_entries_sum) - 1) << std::endl;
      return 0;
    }

  TString ModeTString;
  if(mode == "softmuonid") ModeTString = "MuonID";
  else if(mode == "l1l2") ModeTString = "L1L2";
  else if(mode == "l3") ModeTString = "L3"; 
  TString BinsTString;
  if(bins == "pt") BinsTString = "PtBins";
  else if(bins == "y") BinsTString = "RapBins";
  printSystematics(channel, bins, ModeTString, "./Table_" + ModeTString + "_Syst_" + BinsTString + "_" + channel_to_ntuple_name(channel) + ".txt", systematics);

  return 0;
}

//return the edges of a pT bin
std::pair<int,int> getBinpT(int channel, int bin_number) 
{
  double *bins_vector_pt = nullptr;
  switch(channel) {
  case 1: bins_vector_pt = &ntkp_pt_bins[0];     break;
  case 2: bins_vector_pt = &ntkstar_pt_bins[0];  break;
  case 4: bins_vector_pt = &ntphi_pt_bins[0];    break;
  default: std::cout << "The chosen channel is not supported." << std::endl;
  }
  std::pair<int,int> pair = std::make_pair(bins_vector_pt[bin_number],bins_vector_pt[bin_number+1]);
  return pair;
}

void printSystematics(int channel, std::string bins, TString mode, TString name, std::vector<double>& systematics) 
{
  std::ofstream Table;
  Table.open(name);
  Table << " ,"; //top left corner of the table

  if(bins == "pt")
    {
      for (unsigned int i_pt=0; i_pt<systematics.size(); ++i_pt) 
	Table << getBinpT(channel,i_pt).first << " - " << getBinpT(channel,i_pt).second << ",";
    }
  else if(bins == "y")
    {
      for (unsigned int i_y=0; i_y<systematics.size(); ++i_y) 
	Table << getBiny(channel,i_y).first << " - " << getBiny(channel,i_y).second << ",";
    }
  
  Table << std::endl;
  for(unsigned int i=0; i<systematics.size(); ++i) {
    Table << TMath::Abs(systematics.at(i))*100 << ",";
  }
  
  Table.close();
}

//return the edges of a y bin
std::pair<float,float> getBiny(int channel, int bin_number) 
{
  double *bins_vector_y = nullptr;
  switch(channel) {
  case 1: bins_vector_y = &ntkp_y_bins[0];     break;
  case 2: bins_vector_y = &ntkstar_y_bins[0];  break;
  case 4: bins_vector_y = &ntphi_y_bins[0];    break;
  default: std::cout << "The chosen channel is not supported." << std::endl;
  }
  std::pair<float,float> pair = std::make_pair(bins_vector_y[bin_number],bins_vector_y[bin_number+1]);
  return pair;
}

double getWeight(TFile *f, double muon_eta, double muon_pt, std::string mode)
{
  TGraphAsymmErrors* h = nullptr;
  if(TMath::Abs(muon_eta)>=0. && TMath::Abs(muon_eta)<0.2) h = static_cast<TGraphAsymmErrors*>(f->Get("ratio_0to0p2"));
  else if(TMath::Abs(muon_eta)>=0.2 && TMath::Abs(muon_eta)<0.3) h = static_cast<TGraphAsymmErrors*>(f->Get("ratio_0p2to0p3"));
  else if(TMath::Abs(muon_eta)>=0.3 && TMath::Abs(muon_eta)<0.9) h = static_cast<TGraphAsymmErrors*>(f->Get("ratio_0p3to0p9"));
  else if(TMath::Abs(muon_eta)>=0.9 && TMath::Abs(muon_eta)<1.2) h = static_cast<TGraphAsymmErrors*>(f->Get("ratio_0p9to1p2"));
  else if(TMath::Abs(muon_eta)>=1.2 && TMath::Abs(muon_eta)<1.6) h = static_cast<TGraphAsymmErrors*>(f->Get("ratio_1p2to1p6"));
  else if(TMath::Abs(muon_eta)>=1.6 && TMath::Abs(muon_eta)<2.1) {
    if(mode == "softmuonid") h = static_cast<TGraphAsymmErrors*>(f->Get("ratio_1p6to2p1"));
  }
  else if(TMath::Abs(muon_eta)>=2.1 && TMath::Abs(muon_eta)<2.4) {
    if(mode == "softmuonid") h = static_cast<TGraphAsymmErrors*>(f->Get("ratio_2p1to2p4"));
  }
  else std::cout << "The muon had a pseudo-rapidity of " << muon_eta << "!" << std::endl;

  if (muon_pt>=2. && muon_pt<=40.) { //values considered inside the weight maps
    if(TMath::Abs(muon_eta)>=1.6 && mode != "softmuonid") return 1.; //not all the maps have weights for these y values
    else return h->Eval(muon_pt); 
  }
  else {/*std::cout << "The muon has a pT of " << muon_pt << "!" << std::endl;*/ return 1.;}  
}

std::pair<int,int> currentBin(int nbins_y, int nbins_pt, double b_y, double b_pt, int channel) 
{
  double *bins_vector_pt = nullptr;
  double *bins_vector_y = nullptr;
  switch(channel) {
  case 1: bins_vector_pt = &ntkp_pt_bins[0];     bins_vector_y = &ntkp_y_bins[0];    break;
  case 2: bins_vector_pt = &ntkstar_pt_bins[0];  bins_vector_y = &ntkstar_y_bins[0]; break;
  case 4: bins_vector_pt = &ntphi_pt_bins[0];    bins_vector_y = &ntphi_y_bins[0];   break;
  default: std::cout << "The chosen channel is not supported." << std::endl;
  }

  b_y = TMath::Abs(b_y);
  int pt_currBin = 0, y_currBin = 0;
  for(int i=0; i<nbins_pt; ++i) 
    {
      if(b_pt >= bins_vector_pt[i] && b_pt < bins_vector_pt[i+1]) pt_currBin = i;
    }
  for(int i=0; i<nbins_y; ++i) 
    {
      if(b_y >= bins_vector_y[i] && b_y < bins_vector_y[i+1]) y_currBin = i;
    }

  std::pair<int,int> pair = std::make_pair(y_currBin,pt_currBin);
  return pair;
}
