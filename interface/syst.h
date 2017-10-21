#include <TString.h>

std::vector<std::string> ntkp_syst_list    = {"signal_pdf_syst", "combinatorial_background_pdf_syst", "mass_window_syst", "b_fraction_syst", "mc_stat_syst","tracking_syst"};//, "recoeff_reweight_syst"};
std::vector<std::string> ntkstar_syst_list = {"signal_pdf_syst", "combinatorial_background_pdf_syst", "mass_window_syst", "b_fraction_syst", "mc_stat_syst"};//, "recoeff_reweight_syst"};
std::vector<std::string> ntphi_syst_list   = {"signal_pdf_syst", "combinatorial_background_pdf_syst", "mass_window_syst", "b_fraction_syst", "mc_stat_syst"};//, "recoeff_reweight_syst"};

void setup_syst_list(int channel, std::vector<std::string>* syst_vector)
{
  switch (channel)
    {
    default:
    case 1:
      for(int i=0; i<(int)ntkp_syst_list.size(); i++)
	{
	  syst_vector->push_back(ntkp_syst_list[i]);
	}
      break;
    case 2:
      for(int i=0; i<(int)ntkstar_syst_list.size(); i++)
	{
	  syst_vector->push_back(ntkstar_syst_list[i]);
	}
      break;
    case 4:
      for(int i=0; i<(int)ntphi_syst_list.size(); i++)
	{
	  syst_vector->push_back(ntphi_syst_list[i]);
	}
      break;
    }
}
