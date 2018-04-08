#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"

//Example: fitbias --channel 1 --bin pt
int main(int argc, char** argv) {
  int channel = 1;
  double *pt_bins = total_pt_bin_edges, *y_bins = total_y_bin_edges;
  int n_pt_bins = 0, n_y_bins = 0;
  std::string bin = "";
  TString line;

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;
      if(argument == "--channel")
        {
          convert << argv[++i];
          convert >> channel;
        }
      if(argument == "--bin")
	{
	  convert << argv[++i];
	  convert >> bin;
	}
    }

  switch(channel) {
  case 1:
    if(bin != "y") {
      pt_bins = ntkp_pt_bins;
      n_pt_bins = (sizeof(ntkp_pt_bins) / sizeof(double)) - 1 ;
    }
    else n_pt_bins = 1;

    if(bin != "pt") {
      y_bins = ntkp_y_bins;
      n_y_bins = (sizeof(ntkp_y_bins) / sizeof(double)) - 1 ;
    }
    else n_y_bins = 1;
    break;

  case 2:
    if(bin != "y") {
      pt_bins = ntkstar_pt_bins;
      n_pt_bins = (sizeof(ntkstar_pt_bins) / sizeof(double)) - 1 ;
    }
    else n_pt_bins = 1;

    if(bin != "pt") {
      y_bins = ntkstar_y_bins;
      n_y_bins = (sizeof(ntkstar_y_bins) / sizeof(double)) - 1 ;
    }
    else n_y_bins = 1;
    break;

  case 4:
    if(bin != "y") {
      pt_bins = ntphi_pt_bins;
      n_pt_bins = (sizeof(ntphi_pt_bins) / sizeof(double)) - 1 ;
    }
    else n_pt_bins = 1;

    if(bin != "pt") {
      y_bins = ntphi_y_bins;
      n_y_bins = (sizeof(ntphi_y_bins) / sizeof(double)) - 1 ;
    }
    else n_y_bins = 1;
    break;
  }

  std::cout << n_y_bins << ", " << n_pt_bins << std::endl;

  for (int i_y=0; i_y<n_y_bins; ++i_y) {
    for (int i_pt=0; i_pt<n_pt_bins; ++i_pt) {
      TString f_str = TString::Format(VERSION) + "/mass_fits/" + channel_to_ntuple_name(channel) + "/workspace/" + channel_to_ntuple_name(channel) + "_mass_fit_pt_from_" + TString::Format("%d_to_%d", (int)pt_bins[i_pt], (int)pt_bins[i_pt+1]) + "_y_from_" + TString::Format("%.2f_to_%.2f", y_bins[i_y], y_bins[i_y+1]) + ".root";
      gSystem->Exec("calculate_bin_fitbias --channel " + TString::Format("%d", (int)channel) + " --ptmin " + TString::Format("%d", (int)pt_bins[i_pt]) + " --ptmax " + TString::Format("%d", (int)pt_bins[i_pt+1]) + " --ymin " + TString::Format("%.2f", y_bins[i_y]) + " --ymax " + TString::Format("%.2f", y_bins[i_y+1]));
    }
  }

  return 0;
}
