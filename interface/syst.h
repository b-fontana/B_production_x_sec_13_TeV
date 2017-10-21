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
std::string syst_fancy_name(std::string syst_name)
{
  std::string fancy_name = syst_name;

  if(syst_name == "combined_syst")
    fancy_name = "Overall systematic";
  else
    if(syst_name == "signal_pdf_syst")
      fancy_name = "signal shape";
    else
      if(syst_name == "combinatorial_background_pdf_syst")
	fancy_name = "comb. bkg shape";
      else
	if(syst_name == "mass_window_syst")
	  fancy_name = "mass fit window";
	else
	  if(syst_name == "b_fraction_syst")
	    fancy_name = "Branching ratio";
	  else
	    if(syst_name == "mc_stat_syst")
	      fancy_name = "MC statistics";
	    else
	      if(syst_name == "tracking_syst")
		fancy_name = "Hadron tracking";
  
  return fancy_name; 
}
