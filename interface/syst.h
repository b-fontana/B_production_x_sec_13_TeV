#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"

//syst list
std::vector<std::string> ntkp_syst_list    = {"signal_pdf_syst", "combinatorial_background_pdf_syst", "mass_window_syst", "mc_stat_syst", "jpsipi_pdf_syst", "jpsiX_pdf_syst"}; //"recoeff_reweight_syst"
std::vector<std::string> ntkstar_syst_list = {"signal_pdf_syst", "combinatorial_background_pdf_syst", "mass_window_syst", "mc_stat_syst"}; //"recoeff_reweight_syst"
std::vector<std::string> ntphi_syst_list   = {"signal_pdf_syst", "combinatorial_background_pdf_syst", "mass_window_syst", "mc_stat_syst"}; //"recoeff_reweight_syst"

std::vector<std::string> fsfu_syst_list = {"ratio_reweight_syst"};
std::vector<std::string> fsfd_syst_list = {"ratio_reweight_syst"};
std::vector<std::string> fdfu_syst_list = {"ratio_reweight_syst"};

//global_syst_list
std::vector<std::string> fsfu_global_syst_list = {"pull_syst", "b_fraction_syst", "tracking_syst",  "tktk_width_syst"};
std::vector<std::string> fsfd_global_syst_list = {"pull_syst", "b_fraction_syst", "tktk_width_syst"};
std::vector<std::string> fdfu_global_syst_list = {"pull_syst", "b_fraction_syst", "tracking_syst", "tktk_width_syst"};

//global_syst_val
double tracking_syst = 0.028;
double fsfu_tktk_width_syst = 0.051; //set to zero for now.
double fsfd_tktk_width_syst = 0.051;
double fdfu_tktk_width_syst = 0.004;

//note that the uncertainties will refer to the fragmentations times the BFs
RooRealVar* bu_branch = branching_fraction("ratio", 1, true, true);
RooRealVar* bd_branch = branching_fraction("ratio", 2, true, true);
RooRealVar* bs_branch = branching_fraction("ratio", 4, true, true);

RooRealVar* bu_pull = new RooRealVar("bu_pull","bu_pull",0.00518);
RooRealVar* bd_pull = new RooRealVar("bd_pull","bd_pull",0.00309);
RooRealVar* bs_pull = new RooRealVar("bds_pull","bs_pull",0.00959);

double fsfu_b_fraction_syst = sqrt( pow(bs_branch->getError()/bs_branch->getVal(),2) + pow(bu_branch->getError()/bu_branch->getVal(),2) ) ;
double fsfd_b_fraction_syst = sqrt( pow(bs_branch->getError()/bs_branch->getVal(),2) + pow(bd_branch->getError()/bd_branch->getVal(),2) ) ;
//double fsfd_b_fraction_syst = bsbd_pQCD_branch->getError();
double fdfu_b_fraction_syst = sqrt( pow(bd_branch->getError()/bd_branch->getVal(),2) + pow(bu_branch->getError()/bu_branch->getVal(),2) ) ;

double fsfu_pull_syst = sqrt( pow(bs_pull->getVal(),2) + pow(bu_pull->getVal(),2) );
double fsfd_pull_syst = sqrt( pow(bs_pull->getVal(),2) + pow(bd_pull->getVal(),2) );
double fdfu_pull_syst = sqrt( pow(bd_pull->getVal(),2) + pow(bu_pull->getVal(),2) );

void setup_global_syst_val(std::vector<double>* global_syst_val, TString measure, TString ratio = "", int channel = 0)
{
  if(measure == "")
    std::cout << "ERROR: global systematics vector not defined, look in interface/syst.h" << std::endl;
  else
    {
      if(measure == "xsec")
	std::cout << "ERROR: global systematics not yet defined for cross section measurements, look in interface/syst.h" << std::endl;

      if(measure == "ratio")
	{
	  if(ratio == "")
	    std::cout << "ERROR: global systematics vector not defined, look in interface/syst.h" << std::endl;
	  else
	    {
	      if(ratio == "fsfu")
		for(int i=0; i<(int)fsfu_global_syst_list.size(); i++)
		  {
		    if(fsfu_global_syst_list[i] == "b_fraction_syst")
		      global_syst_val->push_back(fsfu_b_fraction_syst);
		    else
		      if(fsfu_global_syst_list[i] == "tracking_syst")
			global_syst_val->push_back(tracking_syst);
		      else
			if(fdfu_global_syst_list[i] == "tktk_width_syst")
			  global_syst_val->push_back(fsfu_tktk_width_syst);
			else
			  if(fsfu_global_syst_list[i] == "pull_syst")
			    global_syst_val->push_back(fsfu_pull_syst);
		  }	      
	      else
		if(ratio == "fsfd")
		  for(int i=0; i<(int)fsfd_global_syst_list.size(); i++)
		    {
		      if(fsfd_global_syst_list[i] == "b_fraction_syst")
			global_syst_val->push_back(fsfd_b_fraction_syst);
		      else
			if(fsfd_global_syst_list[i] == "tktk_width_syst")
			  global_syst_val->push_back(fsfd_tktk_width_syst);
			else
			  if(fsfd_global_syst_list[i] == "pull_syst")
			    global_syst_val->push_back(fsfd_pull_syst);
		    }
		else
		  if(ratio == "fdfu")
		    for(int i=0; i<(int)fdfu_global_syst_list.size(); i++)
		      {
			if(fdfu_global_syst_list[i] == "b_fraction_syst")
			  global_syst_val->push_back(fdfu_b_fraction_syst);
			else
			  if(fdfu_global_syst_list[i] == "tracking_syst")
			    global_syst_val->push_back(tracking_syst);
			  else
			    if(fdfu_global_syst_list[i] == "tktk_width_syst")
			      global_syst_val->push_back(fdfu_tktk_width_syst);
			    else
			      if(fdfu_global_syst_list[i] == "pull_syst")
				global_syst_val->push_back(fdfu_pull_syst);
		      }
	    } 
	}
    }
}

void setup_global_syst_list(std::vector<std::string>* global_syst_names, TString measure, TString ratio = "", int channel = 0)
{
  if(measure == "")
    std::cout << "ERROR: global systematics vector not defined, look in interface/syst.h" << std::endl;
  else
    {
      if(measure == "xsec")
	std::cout << "ERROR: global systematics not yet defined for cross section measurements, look in interface/syst.h" << std::endl;

      if(measure == "ratio")
	{
	  if(ratio == "")
	    std::cout << "ERROR: global systematics vector not defined, look in interface/syst.h" << std::endl;
	  else
	    {
	      if(ratio == "fsfu")
		for(int i=0; i<(int)fsfu_global_syst_list.size(); i++)
		  {
		    global_syst_names->push_back(fsfu_global_syst_list[i]);
		  }
	      else
		if(ratio == "fsfd")
		  for(int i=0; i<(int)fsfd_global_syst_list.size(); i++)
		    {
		      global_syst_names->push_back(fsfd_global_syst_list[i]);
		    }
		else
		  if(ratio == "fdfu")
		    for(int i=0; i<(int)fdfu_global_syst_list.size(); i++)
		      {
			global_syst_names->push_back(fdfu_global_syst_list[i]);
		      }
	    } 
	}
    }
}

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

void setup_syst_list(TString ratio, std::vector<std::string>* syst_vector)
{
  if(ratio== "fsfu")
    for(int i=0; i<(int)fsfu_syst_list.size(); i++)
      {
	syst_vector->push_back(fsfu_syst_list[i]);
      }
  else
    if(ratio== "fsfd")
      for(int i=0; i<(int)fsfd_syst_list.size(); i++)
	{
	  syst_vector->push_back(fsfd_syst_list[i]);
	}
    else
      if(ratio== "fdfu")
	for(int i=0; i<(int)fdfu_syst_list.size(); i++)
	  {
	    syst_vector->push_back(fdfu_syst_list[i]);
	  }
}

std::string syst_fancy_name(std::string syst_name)
{
  std::string fancy_name = syst_name;

  if(syst_name == "combined_syst")
    fancy_name = "Overall syst.";
  else
    if(syst_name == "signal_pdf_syst")
      fancy_name = "signal\\newline shape";
    else
      if(syst_name == "combinatorial_background_pdf_syst")
	fancy_name = "comb.\\newline bkg shape";
      else
	if(syst_name == "mass_window_syst")
	  fancy_name = "mass\\newline fit window";
	else
	  if(syst_name == "b_fraction_syst")
	    fancy_name = "Branching ratio";
	  else
	    if(syst_name == "mc_stat_syst")
	      fancy_name = "MC\\newline statistics";
	    else
	      if(syst_name == "recoeff_reweight_syst")
		fancy_name = "MC reweight";
	      else
		if(syst_name == "ratio_reweight_syst")
		  fancy_name = "MC\\newline reweight";
		else
		  if(syst_name == "tracking_syst")
		    fancy_name = "Hadron\\newline tracking";
		  else
		    if(syst_name == "jpsipi_pdf_syst")
		      fancy_name = "Jpsi pi";
		    else
		      if(syst_name == "jpsiX_pdf_syst")
			fancy_name = "Jpsi X";
		      else
			if(syst_name == "pull_syst")
			  fancy_name = "Pull syst.";
  
  return fancy_name; 
}
