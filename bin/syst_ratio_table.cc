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

//input example: syst_ratio_table --ratio fsfu --bins pt
int main(int argc, char** argv)
{
  TString measure = "ratio";
  std::string bins = "pt";
  TString ratio = "";

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--ratio")
	{
	  convert << argv[++i];
	  convert >> ratio;
	}
      if(argument == "--bins")
	{
	  convert << argv[++i];
	  convert >> bins;
	}
    }
  
  if(ratio == "")
    {
      std::cout << "ERROR: No --ratio input was provided." << std::endl;
      return 0;
    }
  
  //create dir to save the tables
  std::vector<std::string> dir_list;
  dir_list.push_back(static_cast<const char*>(TString::Format(VERSION) + "/tables/"));
  create_dir(dir_list);

  ///////////////////////////////////////////////////////////////////////////////////////
  
  //set up the vectors
  TString var1_name = "";
  TString var2_name = "";
  int n_var1_bins=1;
  int n_var2_bins=1;
  double* var1_bins = NULL;
  double* var2_bins = NULL;
  
  int precision = 1;
  
  setup_bins(measure, 0, bins, &var1_name, &n_var1_bins, &var2_name, &n_var2_bins, &var1_bins, &var2_bins);
  
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
  
  //set up the syst_list for the ratio. if --ratio is fsfu or fdfu, use the list form channel 1. Else use the list from channel 2 (or 4), they are the same.
  int channel_setup = 0;

  if(ratio.Contains("fu"))
    channel_setup = 1;
  else
    channel_setup = 2;
  
  std::vector<std::string> syst_list;
  setup_syst_list(channel_setup, &syst_list);
  syst_list.push_back("ratio_reweight_syst");
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
	  double val_array[2][n_var2_bins][n_var1_bins];
	  double val_err_lo[2][n_var2_bins][n_var1_bins];
	  double val_err_hi[2][n_var2_bins][n_var1_bins];
      
	  for(int ch=0; ch<2; ch++)
	    {
	      int channel = 1;

	      if(ratio == "fsfu")
		channel = 3*ch+1; //fs_fu: if ch=0 -> channel=1, if ch=1 -> channel=4
	      else
		if(ratio == "fsfd")
		  channel = 2*(ch+1); //fs_fd: if ch=0 -> channel=2, if ch=1 -> channel=4
		else
		  if(ratio == "fdfu")
		    channel= ch+1; //fd_fu: if ch=0 -> channel=1, if ch=1 -> channel=2
		  else
		    {
		      printf("ERROR: The ratio you asked for is not deffined. Only fsfu, fsfd, fdfu are deffined. Please check in the code.");
		      return 0;
		    }
	    
	      if(channel != 1 && (syst_list[k] == "jpsiX_pdf_syst" || syst_list[k] == "jpsipi_pdf_syst")) continue; //read jpsipi and jpsiX only for channel 1
	      if(syst_list[k] == "ratio_reweight_syst" && ch >0) continue; //only read the ratio_reweight_syst one time.

	      if(syst_list[k] != "ratio_reweight_syst") //all the regular syst
		read_vector(channel, syst_list[k], var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, val_array[ch][0], "", val_err_lo[ch][0], val_err_hi[ch][0]);
	      else //special case of ratio_reweight_syst
		read_vector(channel, "ratio_reweight_syst", var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, val_array[ch][0], ratio, val_err_lo[ch][0], val_err_hi[ch][0]);
	    }//end of ch cicle
	  
	  ///////////////////////////////////////////
	  
	  for(int i=0; i<n_var1_bins; i++)
	    {
	      double syst_val = -1; // de fault value to debug

	      if((ratio.Contains("fu") && (syst_list[k] == "jpsiX_pdf_syst" || syst_list[k] == "jpsipi_pdf_syst")) || syst_list[k] == "ratio_reweight_syst")
		syst_val = val_err_hi[0][j][i];
	      else
		syst_val = sqrt(pow(val_err_hi[0][j][i],2)+pow(val_err_hi[1][j][i],2));

	      aux.push_back(TString::Format("%.*f", precision, 100*syst_val).Data());
	    }
	  numbers.push_back(aux);
	  aux.clear();
	}//end of syst_list cicle
  
      
      TString file_name = "";
      TString dir = TString::Format(VERSION) + "/tables/";
      TString bins_str ="";
            
      if(var1_name == "pt")
	bins_str = TString::Format("%.2f_to_%.2f", var2_bins[j], var2_bins[j+1]);
      else
	bins_str = TString::Format("%d_to_%d", (int)var2_bins[j], (int)var2_bins[j+1]);

      file_name = dir + ratio + "_syst_summary_" + var1_name + "_bins_" + var2_name + "_from_" + bins_str;
      
      if(bins == "full")
	file_name = dir + ratio + "_syst_summary_full_bins";
      
      TString bins_cap ="";
      
      if(var1_name == "pt")
	bins_cap = TString::Format("$%.2f$ to $%.2f$", var2_bins[j], var2_bins[j+1]);
      else
	bins_cap = TString::Format("$%d$ to $%d$", (int)var2_bins[j], (int)var2_bins[j+1]);

      TString caption = ratio + " systematics, " + var2_name + " from " + bins_cap;
      
      latex_table(file_name.Data(), col_name.size(), n_var1_bins + 1, col_name, labels, numbers, caption.Data());
    }//end of var2 cicle
  
}//end
