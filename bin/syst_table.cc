#include "UserCode/B_production_x_sec_13_TeV/interface/syst.h"

//-----------------------------------------------------------------
// Definition of channel #
// channel = 1: B+ -> J/psi K+
// channel = 2: B0 -> J/psi K*
// channel = 3: B0 -> J/psi Ks
// channel = 4: Bs -> J/psi phi
// channel = 5: Jpsi + pipi
// channel = 6: Lambda_b -> Jpsi + Lambda
//-----------------------------------------------------------------

//input example: syst_table --measure x_sec --channel 1 --bins pt
int main(int argc, char** argv)
{
  TString measure = "";
  int channel = 0;
  std::string bins = "pt";

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--measure")
	{
	  convert << argv[++i];
	  convert >> measure;
	}

      if(argument == "--channel")
	{
	  convert << argv[++i];
	  convert >> channel;
	}
      if(argument == "--bins")
	{
	  convert << argv[++i];
	  convert >> bins;
	}
    }
  
  if(measure == "")
    {
      std::cout << "ERROR: No --measure or --vector input was provided." << std::endl;
      return 0;
    }
  
  //create dir to save the tables
  std::vector<std::string> dir_list;
  dir_list.push_back(static_cast<const char*>(TString::Format(VERSION) + "/tables/"));
  create_dir(dir_list);

  ///////////////////////////////////////////////////////////////////////////////////////
  
  TString b_title= "";
  switch(channel)
    {
    case 1:
      b_title = "B+";
      break;
    case 2:
      b_title = "B0";
      break;
    case 4:
      b_title = "Bs";
      break;
    default:
      break;
    }
  
  //set up the vectors
  TString var1_name = "";
  TString var2_name = "";
  int n_var1_bins=1;
  int n_var2_bins=1;
  double* var1_bins = NULL;
  double* var2_bins = NULL;
  
  int precision = 1;
  
  setup_bins(measure, channel, bins, &var1_name, &n_var1_bins, &var2_name, &n_var2_bins, &var1_bins, &var2_bins);
  
  //set up lables
  std::vector<std::string> labels;
  std::string lab;
  
  for(int i=0; i<n_var1_bins; i++)
    {
      if(var1_name == "pt")
	lab = std::to_string((int)var1_bins[i]) + " to " + std::to_string((int)var1_bins[i+1]);
      else
	lab = TString::Format("%.2f to %.2f", (double)var1_bins[i], (double)var1_bins[i+1]).Data();
      
      labels.push_back(lab);
    }
  
  //set up columns names
  std::vector<std::string> col_name;

  std::string var1_str = "";

  if(var1_name == "pt")
    var1_str = "pT [GeV]";
  else
    if(var1_name == "y")
      var1_str = "|y|";
  
  col_name.push_back(var1_str);
  
  //set up syst list
  std::vector<std::string> syst_list;
  setup_syst_list(channel, &syst_list);
  syst_list.push_back("combined_syst");
  

  for(int k=0; k< (int)syst_list.size(); k++)
    {
      col_name.push_back(syst_fancy_name(syst_list[k]).append("[\\%]"));
    }
  
  //read the arrays
  for(int j=0; j<n_var2_bins; j++)
    {
      std::vector<std::vector<std::string> > numbers;
      std::vector<std::string> aux;
      
      for(int k=0; k< (int)syst_list.size(); k++)
	{ 
	  double val_array[n_var2_bins][n_var1_bins];
	  double val_err_lo[n_var2_bins][n_var1_bins];
	  double val_err_hi[n_var2_bins][n_var1_bins];
      
	  //read syst
	  read_vector(channel, syst_list[k], var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, val_array[0], val_err_lo[0], val_err_hi[0]);
	  
	  for(int i=0; i<n_var1_bins; i++)
	    {
	      aux.push_back(TString::Format("%.*f", precision, 100*val_err_hi[j][i]).Data());
	    }
	  numbers.push_back(aux);
	  aux.clear();
	}
  
      
      TString file_name = "";
      TString dir = TString::Format(VERSION) + "/tables/";
      TString bins_str ="";
      TString ntuple_name = channel_to_ntuple_name(channel);
      
      if(var1_name == "pt")
	bins_str = TString::Format("%.2f_to_%.2f", var2_bins[j], var2_bins[j+1]);
      else
	bins_str = TString::Format("%d_to_%d", (int)var2_bins[j], (int)var2_bins[j+1]);

      file_name = dir + measure + "_syst_summary_" + ntuple_name + "_" + var1_name + "_bins_" + var2_name + "_from_" + bins_str;
      
      if(bins == "full")
	file_name = dir + measure + "_syst_summary_" + ntuple_name + "_full_bins";
      
      TString bins_cap ="";
      
      if(var1_name == "pt")
	bins_cap = TString::Format("$%.2f$ to $%.2f$", var2_bins[j], var2_bins[j+1]);
      else
	bins_cap = TString::Format("$%d$ to $%d$", (int)var2_bins[j], (int)var2_bins[j+1]);

      TString caption = b_title + " systematics, " + var2_name + " from " + bins_cap;
      
      latex_table(file_name.Data(), col_name.size(), n_var1_bins + 1, col_name, labels, numbers, caption.Data(), true);
    }//end of var2 cicle
  
}//end
