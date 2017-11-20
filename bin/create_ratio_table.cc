#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"

//-----------------------------------------------------------------
// Definition of channel #
// channel = 1: B+ -> J/psi K+
// channel = 2: B0 -> J/psi K*
// channel = 3: B0 -> J/psi Ks
// channel = 4: Bs -> J/psi phi
// channel = 5: Jpsi + pipi
// channel = 6: Lambda_b -> Jpsi + Lambda
//-----------------------------------------------------------------

//input example: create_ratio_table --bins pt --ratio fsfu --vector yield --eff 0 --syst 0
int main(int argc, char** argv)
{
  std::string bins = "pt";
  TString ratio = "";
  TString vector = "";
  int eff = 0;
  int syst = 0;

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--bins")
	{
	  convert << argv[++i];
	  convert >> bins;
	}
      if(argument == "--ratio")
	{
	  convert << argv[++i];
	  convert >> ratio;
	}
      if(argument == "--vector")
	{
	  convert << argv[++i];
	  convert >> vector;
	}
      if(argument == "--eff")
	{
	  convert << argv[++i];
	  convert >> eff;
	}
      if(argument == "--syst")
	{
	  convert << argv[++i];
	  convert >> syst;
	}
    }
  
  if(ratio == "" || vector == "")
    {
      std::cout << "ERROR: No --ratio or --vector input was provided." << std::endl;
      return 0;
    }
  
  //create dir to save the tables
  std::vector<std::string> dir_list;
  dir_list.push_back(static_cast<const char*>(TString::Format(VERSION) + "/tables/"));
  create_dir(dir_list);
  
  //set up the vectors
  TString var1_name = "";
  TString var2_name = "";
  int n_var1_bins=1;
  int n_var2_bins=1;
  double* var1_bins = NULL;
  double* var2_bins = NULL;
 
  TString measure = "ratio";

  //////////////////////////////////////////////////
  setup_bins(measure, 0 , bins, &var1_name, &n_var1_bins, &var2_name, &n_var2_bins, &var1_bins, &var2_bins);

  //initialize arrays for yield, efficiencies, etc 
  double yield[2][n_var2_bins][n_var1_bins];
  double yield_err_lo[2][n_var2_bins][n_var1_bins];
  double yield_err_hi[2][n_var2_bins][n_var1_bins];

  double total_eff[2][n_var2_bins][n_var1_bins];
  double total_eff_err_lo[2][n_var2_bins][n_var1_bins];
  double total_eff_err_hi[2][n_var2_bins][n_var1_bins];

  double ratio_eff[n_var2_bins][n_var1_bins];
  double ratio_eff_err_lo[n_var2_bins][n_var1_bins];
  double ratio_eff_err_hi[n_var2_bins][n_var1_bins];
  
  double ratio_array[n_var2_bins][n_var1_bins];
  double ratio_err_lo[n_var2_bins][n_var1_bins];
  double ratio_err_hi[n_var2_bins][n_var1_bins];

  double combined_syst[2][n_var2_bins][n_var1_bins];
  double combined_syst_lo[2][n_var2_bins][n_var1_bins];
  double combined_syst_hi[2][n_var2_bins][n_var1_bins];

  double ratio_syst_hi[n_var2_bins][n_var1_bins];

  double b_fraction[2];
  
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
      
      RooRealVar* branch = branching_fraction(measure, channel);
      b_fraction[ch] = branch->getVal();
            
      if(vector == "yield")
	read_vector(channel, "yield", var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, yield[ch][0], yield_err_lo[ch][0], yield_err_hi[ch][0]);

      if(eff || vector == "ratioeff")
        read_vector(channel, "totaleff", var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, total_eff[ch][0], total_eff_err_lo[ch][0], total_eff_err_hi[ch][0]);

      if(syst)
        read_vector(channel, "combined_syst", var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, combined_syst[ch][0],combined_syst_lo[ch][0],combined_syst_hi[ch][0]);
            
    }//end of ch cicle
  
  //calculate the ratio

  if(eff || vector == "ratioeff") //calculate ratioeff
    {
      for(int j=0; j<n_var2_bins; j++)
        {
          for(int i=0; i<n_var1_bins; i++)
            {
              ratio_eff[j][i] = total_eff[0][j][i] / total_eff[1][j][i];
              ratio_eff_err_lo[j][i] = ratio_eff[j][i] * sqrt(pow(total_eff_err_lo[0][j][i]/total_eff[0][j][i],2) + pow(total_eff_err_lo[1][j][i]/total_eff[1][j][i],2));
              ratio_eff_err_hi[j][i] = ratio_eff[j][i] * sqrt(pow(total_eff_err_hi[0][j][i]/total_eff[0][j][i],2) + pow(total_eff_err_hi[1][j][i]/total_eff[1][j][i],2));
            }
	}
    }


  //calculate the ratio
  if(vector != "ratioeff")
    {
      for(int j=0; j<n_var2_bins; j++)
	{
	  for(int i=0; i<n_var1_bins; i++)
	    {
	      ratio_array[j][i]  = yield[1][j][i] / yield[0][j][i];
          
	      if(eff)
		{
		  ratio_array[j][i] *= ratio_eff[j][i] * (b_fraction[0]/b_fraction[1]);
		}
	  
	      ratio_err_lo[j][i] = ratio_array[j][i] * sqrt(pow(yield_err_lo[0][j][i]/yield[0][j][i],2) + pow(yield_err_lo[1][j][i]/yield[1][j][i],2));
	      ratio_err_hi[j][i] = ratio_array[j][i] * sqrt(pow(yield_err_hi[0][j][i]/yield[0][j][i],2) + pow(yield_err_hi[1][j][i]/yield[1][j][i],2));

	      if(syst)
		{
		  ratio_syst_hi[j][i]  = ratio_array[j][i] * sqrt( pow(combined_syst_hi[0][j][i],2) + pow(combined_syst_hi[1][j][i],2) );
		}
	    }//var1 cicle
	}//var2 cicle
    }
  else //put the ratioeff in the ratio_array
    {
      for(int j=0; j<n_var2_bins; j++)
	{
	  for(int i=0; i<n_var1_bins; i++)
	    {
	      ratio_array[j][i] = ratio_eff[j][i];
	      ratio_err_lo[j][i] = ratio_eff_err_lo[j][i];
	      ratio_err_hi[j][i] = ratio_eff_err_hi[j][i];
	    }
	}
    }
  ////////////////////////////////////////////////

  TString ratio_cap = ratio;

  if(eff)
    {
      if(ratio == "fsfu")
	ratio_cap = "fs/fu";
      else
	if(ratio == "fs/fd")
	  ratio_cap = "fsfd";
	else
	  if(ratio == "fd/fu")
	    ratio_cap = "fdfu";
    }
  else
    {
      if(ratio == "fsfu")
	ratio_cap = "Bs/Bu";
      else
	if(ratio == "fsfd")
	  ratio_cap = "Bs/Bd";
	else
	  if(ratio == "fdfu")
	    ratio_cap = "Bd/Bu";
    }
  
  if(vector == "ratioeff")
    {
      if(ratio == "fsfu")
	ratio_cap = "$\\epsilon_u/\\epsilon_s$";
      else
	if(ratio == "fsfd")
	  ratio_cap = "$\\epsilon_d/\\epsilon_s$";
	else
	  if(ratio == "fdfu")
	    ratio_cap = "$\\epsilon_u/\\epsilon_d$";
    }

  std::vector<std::string> col_name;

  std::string bins_name = ((std::string)var1_name.Data()).append(" bins");

  col_name.push_back(bins_name);
  col_name.push_back(ratio_cap.Data());
  col_name.push_back("Stat. uncertainty");

  if(syst)
    col_name.push_back("Syst. uncertainty");
  
  int precision = 2;
  
  //specific precision value for each table
  if(vector == "yield")
    precision = 3;
  else
    if(vector == "ratioeff")
      precision = 3;
  
  for(int j=0; j<n_var2_bins; j++)
    {
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

      std::vector<std::vector<std::string> > numbers;
      std::vector<std::string> aux;
      
      for(int i=0; i<n_var1_bins; i++)
	{
	  aux.push_back(TString::Format("%.*f", precision, ratio_array[j][i]).Data());
	}
      numbers.push_back(aux);
      aux.clear();
      
      for(int i=0; i<n_var1_bins; i++)
	{
	  aux.push_back(TString::Format("\\large \\tol{}{%.*f}{%.*f}", precision, ratio_err_lo[j][i], precision, ratio_err_hi[j][i]).Data());
	}
      numbers.push_back(aux);
      aux.clear();

      if(syst && vector != "ratioeff")
	{
	  for(int i=0; i<n_var1_bins; i++)
	    {
	      aux.push_back(TString::Format("$\\pm$ %.*f", precision, ratio_syst_hi[j][i]).Data());
	    }
	  numbers.push_back(aux);
	  aux.clear();
	}
     
      TString file_name = "";
      TString dir = TString::Format(VERSION) + "/tables/";
      TString bins_str ="";
            
      if(var1_name == "pt")
	bins_str = TString::Format("%.2f_to_%.2f", var2_bins[j], var2_bins[j+1]);
      else
	bins_str = TString::Format("%d_to_%d", (int)var2_bins[j], (int)var2_bins[j+1]);

      TString ratio_name = ratio;      

      if(eff == 0 && vector != "ratioeff")
	{
	  if(ratio == "fsfu")
	    ratio_name = "BsBu";
	  else
	    if(ratio == "fsfd")
	      ratio_name = "BsBd";
	    else
	      if(ratio == "fdfu")
		ratio_name = "BdBu";
	}

      TString ratioeff_str = "";
      
      if(vector == "ratioeff")
	ratioeff_str = "ratioeff_";

      TString syst_str = "";
      
      if(syst)
	syst_str = "syst_";

      file_name = dir + ratio_name + "_" + ratioeff_str + syst_str + var1_name + "_bins_" + var2_name + "_from_" + bins_str;
      
      if(bins == "full")
	file_name = dir + ratio_name + "_" + ratioeff_str + syst_str + "full_bins";
      
      TString bins_cap ="";
      
      if(var1_name == "pt")
	bins_cap = TString::Format("$%.2f$ to $%.2f$", var2_bins[j], var2_bins[j+1]);
      else
	bins_cap = TString::Format("$%d$ to $%d$", (int)var2_bins[j], (int)var2_bins[j+1]) + " GeV";

      TString caption = ratio_cap + " " + var2_name + " from " + bins_cap;
      
      latex_table(file_name.Data(), col_name.size(), n_var1_bins + 1, col_name, labels, numbers, caption.Data());
    }//end of var2 cicle
  
}//end
