#include <algorithm>
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

double pdf_syst(RooWorkspace& ws, int channel, double pt_min, double pt_max, double y_min, double y_max, double nominal_yield, TString syst);
double mass_window_syst(RooWorkspace& ws, int channel, double pt_min, double pt_max, double y_min, double y_max, double nominal_yield, TString input_file);
double reweighting_syst(int channel, double pt_min, double pt_max, double y_min, double y_max, double nominal_quantity);

//input example: calculate_bin_syst --channel 0 --ratio none --syst signal_pdf_syst --ptmin 30 --ptmax 35 --ymin 0.00 --ymax 2.25
int main(int argc, char** argv)
{
  int channel = 0;
  TString ratio = "";
  TString syst = "";
  double pt_min = -1;
  double pt_max = -1;
  double y_min = -1;
  double y_max = -1;

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--channel")
	{
	  convert << argv[++i];
	  convert >> channel;
	}
      if(argument == "--ratio")
	{
	  convert << argv[++i];
	  convert >> ratio;
	}      
      if(argument == "--syst")
	{
	  convert << argv[++i];
	  convert >> syst;
	}
      if(argument == "--ptmin")
	{
	  convert << argv[++i];
	  convert >> pt_min;
	}
      if(argument == "--ptmax")
	{
	  convert << argv[++i];
	  convert >> pt_max;
	}
      if(argument == "--ymin")
	{
	  convert << argv[++i];
	  convert >> y_min;
	}
      if(argument == "--ymax")
	{
	  convert << argv[++i];
	  convert >> y_max;
	}
    }

  if(pt_min == -1 || pt_max == -1 ||y_min == -1 ||y_max == -1)
    {
      std::cout << "Error: The bin was not well defined. Please enter pt and y bin." << std::endl;
      return 0;
    }
  
  //to create the directories to save the files
  std::vector<std::string> dir_list;  
  
  dir_list.push_back(static_cast<const char*>(TString::Format(VERSION) + "/systematics_root/" + "combined_ratios/"));
  dir_list.push_back(static_cast<const char*>(TString::Format(VERSION) + "/systematics_root/" + channel_to_ntuple_name(channel)));
  dir_list.push_back(static_cast<const char*>(TString::Format(VERSION) + "/mass_fits/syst/" + channel_to_ntuple_name(channel)));
  dir_list.push_back(static_cast<const char*>(TString::Format(VERSION) + "/mass_fits/syst/" + channel_to_ntuple_name(channel) + "/workspace/"));
  dir_list.push_back(static_cast<const char*>(TString::Format(VERSION) + "/efficiencies_root/" + channel_to_ntuple_name(channel)));
  create_dir(dir_list);

  ///////////////////////////////////////////////////
  //calculate the syst error for a bin of pt and y.//
  ///////////////////////////////////////////////////
  std::cout << "processing subsample: " << pt_min << " < " << "pt" << " < " << pt_max << " and " << y_min << " < " << "|y|" << " < " << y_max << std::endl;
  
  //////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
  TString bins_str = "_pt_from_" + TString::Format("%d_to_%d", (int)pt_min, (int)pt_max) + "_y_from_" + TString::Format("%.2f_to_%.2f", y_min, y_max);
  
  TString command = "";
  TString opt = " --ptmin " + TString::Format("%d", (int)pt_min) + " --ptmax " + TString::Format("%d", (int)pt_max) + " --ymin " + TString::Format("%.2f", y_min) + " --ymax " + TString::Format("%.2f", y_max); 

  TString dir = "";
  TString val_name = "";

  TString syst_dir = TString::Format(VERSION) + "/systematics_root/";
  
  //absolute value of syst, i.e. from 0 to 1
  double absolute_syst_val = -1; //set to -1 as default, should be replaced below by a specific syst or the combined syst
  
  //list to calculate the combined_syst
  std::vector<std::string> syst_list;
  std::vector<double> syst_list_val;

  //cicle to calculate combined_syst per channel
  if(ratio == "")
    {
      std::cout << "calculate syst per channel" << std::endl;

      if(syst == "combined_syst")
	{
	  std::cout << "calculate combined syst per channel" << std::endl;
	  
	  setup_syst_list(channel, &syst_list);
	  
	  //print syst list with names
	  std::cout << "=== Systematics list name ===" << std::endl;
      
	  for(int k=0; k<(int)syst_list.size(); k++)
	    {
	      std::cout << "Syst nr " << k << " : " << syst_list[k] << std::endl;
	    }
      
	  double sqrt_err = 0;
      
	  for(int k=0; k<(int)syst_list.size(); k++)
	    {
	      TString f_name = syst_dir + channel_to_ntuple_name(channel) + "/" + syst_list[k] + "_" + channel_to_ntuple_name(channel) + bins_str + ".root";
	      TFile* f_syst = new TFile(f_name);
	  
	      if(f_syst->IsZombie())
		{
		  std::cout << "The file with " << syst_list[k] << " was not found." << std::endl;
		  std::cout << "No problem, it will be calculated" << std::endl;
	      
		  command = "calculate_bin_syst --syst " + syst_list[k];
	      
		  command += " --channel " + TString::Format("%d", channel) + opt;

		  std::cout << "Executing command:" << std::endl;
		  std::cout << command << std::endl;
	      
		  gSystem->Exec(command);

		  f_syst = new TFile(f_name);
		}

	      TVectorD *in_err_hi = (TVectorD*)f_syst->Get("err_hi");
	      delete f_syst;

	      syst_list_val.push_back(in_err_hi[0][0]);
	  
	      sqrt_err += pow(in_err_hi[0][0],2);
	    }
      
	  absolute_syst_val = sqrt(sqrt_err);

	  //print syst list with val
	  std::cout << "=== Systematics list val ===" << std::endl;

	  for(int k=0; k<(int)syst_list_val.size(); k++)
	    {
	      std::cout << "Syst nr " << k << ": " << syst_list[k] << " : " << syst_list_val[k] << std::endl;
	    }
	}
  
      ////////////////////////////////////////////////////////////////////////////////
      //calculate each syst, starting with error propagation ones. And then nominal vs alternative.
      else
	{
	  std::cout << "calculate each syst per channel" << std::endl;
	  
	  if(syst == "mc_stat_syst")// || syst == "b_fraction_syst" || syst == "tracking_syst")
	    {
	      std::cout << "calculating " << syst << " ,This is not a comparison with nominal value, just error propagation." << std::endl;
	  
	      if(syst == "mc_stat_syst")
		{
		  TString in_eff_file = TString::Format(VERSION) + "/efficiencies_root/" + channel_to_ntuple_name(channel) + "/totaleff_" + channel_to_ntuple_name(channel) + bins_str + ".root";
	      
		  std::cout << "reading totaleff from : " << std::endl;
		  std::cout << in_eff_file << std::endl;
	      
		  TFile* f = new TFile(in_eff_file);
	      
		  if(f->IsZombie())
		    {
		      std::cout << "The nominal val file " << in_eff_file << " was not found." << std::endl;
		      std::cout << "No problem, it will be calculated" << std::endl;
		  
		      command = "calculate_bin_eff --eff totaleff ";
	      	  
		      command += " --channel " + TString::Format("%d", channel) + opt;
		  
		      std::cout << "Executing command:" << std::endl;
		      std::cout << command << std::endl;
		  
		      gSystem->Exec(command);
	  	  
		      f = new TFile(in_eff_file);
		    }
	      
		  TVectorD *in_eff_val = (TVectorD*)f->Get("val");
		  TVectorD *in_eff_err_lo = (TVectorD*)f->Get("err_lo");
		  TVectorD *in_eff_err_hi = (TVectorD*)f->Get("err_hi");
		  delete f;
	      
		  RooRealVar totaleff_val("totaleff_val", "totaleff_val", in_eff_val[0][0]);
		  totaleff_val.setAsymError(-in_eff_err_lo[0][0],in_eff_err_hi[0][0]);
	      
		  absolute_syst_val = std::max(fabs(totaleff_val.getAsymErrorHi()),fabs(totaleff_val.getAsymErrorLo())) /totaleff_val.getVal();
		}
	      else
		if(syst == "b_fraction_syst") //not in use now, included in global uncertainty
		  {
		    //RooRealVar* branch = branching_fraction(measure, channel);
		    //absolute_syst_val= branch->getError() / branch->getVal();
		  }
		else 
		  if(syst == "tracking_syst")
		    {
		      //absolute_syst_val=0.028; //2.8% tracking efficiency from the tracking POG. not in use now, incuded in global uncertainty
		    }
	    }
	  else
	    {
	      //////////////////////////////////////////////////
	      //read nominal value to compare with alternative//
	      //////////////////////////////////////////////////
	  
	      if(syst.Contains("eff"))
		{
		  dir = "/efficiencies_root/";
	      
		  if(syst.Contains("preeff"))
		    val_name = "preeff";
		  else
		    if(syst.Contains("recoeff"))
		      val_name = "recoeff";
		    else
		      {
			std::cout << "Error: " <<  syst << " systematic not well defined" << std::endl;
			return 0;
		      }
		}
	      else
		{
		  dir = "/signal_yield_root/";
		  val_name = "yield";
		}
      
	      TString in_file_name = TString::Format(VERSION) + dir + channel_to_ntuple_name(channel) + "/" + val_name + "_" + channel_to_ntuple_name(channel) + bins_str + ".root";;
      
	      std::cout << "reading nominal val from : " << std::endl;
	      std::cout << in_file_name << std::endl;
      
	      TFile* fin = new TFile(in_file_name);
      
	      if(fin->IsZombie())
		{
		  std::cout << "The nominal val file " << in_file_name << " was not found." << std::endl;
		  std::cout << "No problem, it will be calculated" << std::endl;
	  
		  if(syst.Contains("eff"))
		    command = "calculate_bin_eff --eff " + val_name;
		  else
		    command = "calculate_bin_yield";
	  
		  command += " --channel " + TString::Format("%d", channel) + opt;
	  
		  std::cout << "Executing command:" << std::endl;
		  std::cout << command << std::endl;

		  gSystem->Exec(command);
	  	  
		  fin = new TFile(in_file_name);
		}
      
	      TVectorD *in_val = (TVectorD*)fin->Get("val");
	      TVectorD *in_err_lo = (TVectorD*)fin->Get("err_lo");
	      TVectorD *in_err_hi = (TVectorD*)fin->Get("err_hi");
	      delete fin;
      
	      RooRealVar nominal_val("nominal_val", "nominal_val", in_val[0][0]);
	      nominal_val.setAsymError(-in_err_lo[0][0],in_err_hi[0][0]);
      
	      //debug:
	      std::cout << "nominal_val: " << nominal_val.getVal() << " err_lo: " << nominal_val.getAsymErrorLo() << " err_hi: " << nominal_val.getAsymErrorHi() << std::endl;
      
	      //calculate systematic absolute value
	      double alternative_val = 0;
      
	      ///////////////read dataset//////////////////////
	      if(syst.Contains("eff") == false)
		{
		  TString data_selection_input_file = "";
		  if(RERECO) data_selection_input_file = "/lstore/cms/balves/Jobs/Full_Dataset_2015_Rereco/myloop_new_data_" + channel_to_ntuple_name(channel) + "_with_cuts_hadd.root";
		  else data_selection_input_file = TString::Format(BASE_DIR_2016) + "Full_Dataset_2016/NewSelection/myloop_new_data_" + channel_to_ntuple_name(channel) + "_with_cuts.root";
		  //old 2015 prompt-reco: TString data_selection_input_file = TString::Format(BASE_DIR) + "/new_inputs/myloop_new_data_" + channel_to_ntuple_name(channel) + "_with_cuts.root";
		  RooWorkspace* ws = new RooWorkspace("ws","Bmass");
	  
		  //set up mass, pt and y variables inside ws  
		  set_up_workspace_variables(*ws,channel);
	      
		  //read data from the selected data file, and import it as a dataset into the workspace.
		  read_data(*ws, data_selection_input_file,channel);
	      
		  if(syst == "mass_window_syst")
		    alternative_val = mass_window_syst(*ws, channel, pt_min, pt_max, y_min, y_max, nominal_val.getVal(), data_selection_input_file);
		  else
		    if(syst.Contains("pdf_syst"))
		      alternative_val = pdf_syst(*ws, channel, pt_min, pt_max, y_min, y_max, nominal_val.getVal(), syst);
		}
	      else
		{
		  if(syst == "recoeff_reweight_syst")
		    {
		      std::cout << "calculating alternative eff" << std::endl;
		      alternative_val = reweighting_syst(channel, pt_min, pt_max, y_min, y_max, nominal_val.getVal()); 
		    }
		}
	  
	      //keep the signal of the systematic for the reweighting
	      if(syst == "recoeff_reweight_syst") 
		absolute_syst_val = (nominal_val.getVal() - alternative_val)/nominal_val.getVal(); 
	      else absolute_syst_val = fabs((nominal_val.getVal() - alternative_val)/nominal_val.getVal()); 
	    }
	}
    }
  else //calculate ratio systs
    {
      if(syst == "combined_syst")
	{
	  std::cout << "calculate ratio combined_syst" << std::endl;
	  
	  setup_syst_list(ratio, &syst_list);
	  
	  double combined[2];
	  double sqrt_err = 0;

	  //print syst list with names
	  std::cout << "=== Systematics list name ===" << std::endl;
	  std::cout << "Syst : combined ch 0" << std::endl;
	  std::cout << "Syst : combined ch 1" << std::endl;
	  
	  for(int k=0; k<(int)syst_list.size(); k++)
	    std::cout << "Syst nr " << k+2 << " : " << syst_list[k] << std::endl;
	  
	  for(int ch=0; ch<2; ch++)
	    {
	      if(ratio == "fsfu")
		channel = 3*ch+1;
	      else
		if(ratio == "fsfd")
		  channel = 2*(ch+1);
		else
		  if(ratio == "fdfu")
		    channel= ch+1;
		  
	      TString f_name = syst_dir + channel_to_ntuple_name(channel) + "/combined_syst_" + channel_to_ntuple_name(channel) + bins_str + ".root";
	      TFile* f_syst = new TFile(f_name);
		  
	      if(f_syst->IsZombie())
		{
		  std::cout << "The file with combined_syst was not found." << std::endl;
		  std::cout << "No problem, it will be calculated" << std::endl;
		      
		  command = "calculate_bin_syst --syst combined_syst";
		      
		  command += " --channel " + TString::Format("%d", channel) + opt;
		      
		  std::cout << "Executing command:" << std::endl;
		  std::cout << command << std::endl;
		      
		  gSystem->Exec(command);
		      
		  f_syst = new TFile(f_name);
		}
		  
	      TVectorD *in_err_hi = (TVectorD*)f_syst->Get("err_hi");
	      delete f_syst;
	      
	      combined[ch] = in_err_hi[0][0];

	      sqrt_err += pow(in_err_hi[0][0],2);
	    }
	    
	  for(int k=0; k<(int)syst_list.size(); k++)
	    {
	      TString f_name = TString::Format(VERSION) + "/systematics_root/" + ratio + "_" + syst_list[k] + bins_str + ".root";
	      TFile* f_syst = new TFile(f_name);
	  
	      if(f_syst->IsZombie())
		{
		  std::cout << "The file with " << syst_list[k] << " was not found." << std::endl;
		  std::cout << "No problem, it will be calculated" << std::endl;
	      
		  command = "calculate_bin_syst --syst " + syst_list[k];
	      
		  command += " --ratio " + ratio + opt;

		  std::cout << "Executing command:" << std::endl;
		  std::cout << command << std::endl;
	      
		  gSystem->Exec(command);

		  f_syst = new TFile(f_name);
		}

	      TVectorD *in_err_hi = (TVectorD*)f_syst->Get("err_hi");
	      delete f_syst;

	      syst_list_val.push_back(in_err_hi[0][0]);

	      sqrt_err += pow(in_err_hi[0][0],2);  //Corrigido por Bruno Alves - 8 de Abril de 2018
	    }

	  absolute_syst_val = sqrt(sqrt_err);

	  //print syst list with val
	  std::cout << "=== Systematics list val ===" << std::endl;
	  std::cout << "Syst : combined ch 0 " << " : " << combined[0] << std::endl;
	  std::cout << "Syst : combined ch 1 " << " : " << combined[1] << std::endl;
	  
	  for(int k=0; k<(int)syst_list_val.size(); k++)
	    std::cout << "Syst nr " << k+2 << ": " << syst_list[k] << " : " << syst_list_val[k] << std::endl;
	}
      else
	{
	  if(syst == "ratio_reweight_syst")
	    {
	      double ratio_nominal_val = 0;
	      double ratio_alternative_val = 0;
            
	      for(int ch=0; ch<2; ch++)
		{
		  if(ratio == "fsfu")
		    channel = 3*ch+1;
		  else
		    if(ratio == "fsfd")
		      channel = 2*(ch+1);
		    else
		      if(ratio == "fdfu")
			channel= ch+1;
		  
		  TString ntuple_name = channel_to_ntuple_name(channel);
	  
		  TString in_file_name = TString::Format(VERSION) + "/efficiencies_root/" + ntuple_name + "/recoeff_" + ntuple_name + bins_str + ".root";
	  
		  std::cout << "reading nominal val from : " << std::endl;
		  std::cout << in_file_name << std::endl;
	  
		  TFile* fin = new TFile(in_file_name);
	  
		  if(fin->IsZombie())
		    {
		      std::cout << "The nominal val file " << in_file_name << " was not found." << std::endl;
		      std::cout << "No problem, it will be calculated" << std::endl;
	      
		      command = "calculate_bin_eff --eff recoeff";
	      
		      command += " --channel " + TString::Format("%d", channel) + opt;
	      
		      std::cout << "Executing command:" << std::endl;
		      std::cout << command << std::endl;
	      
		      gSystem->Exec(command);
	      
		      fin = new TFile(in_file_name);
		    }
	  
		  TVectorD *in_val = (TVectorD*)fin->Get("val");
		  TVectorD *in_err_lo = (TVectorD*)fin->Get("err_lo");
		  TVectorD *in_err_hi = (TVectorD*)fin->Get("err_hi");
		  delete fin;
	  
		  RooRealVar nominal_val("nominal_val", "nominal_val", in_val[0][0]);
		  nominal_val.setAsymError(-in_err_lo[0][0],in_err_hi[0][0]);
		  	  	  
		  //debug:
		  std::cout << "nominal_val: " << nominal_val.getVal() << " err_lo: " << nominal_val.getAsymErrorLo() << " err_hi: " << nominal_val.getAsymErrorHi() << std::endl;
	  
		  if(ch==0)
		    ratio_nominal_val = nominal_val.getVal();
		  else
		    ratio_nominal_val = ratio_nominal_val / nominal_val.getVal();
	  
		  double alternative_val = 0;
	  
		  alternative_val = reweighting_syst(channel, pt_min, pt_max, y_min, y_max, nominal_val.getVal());
	  
		  if(ch==0)
		    ratio_alternative_val = alternative_val;
		  else
		    ratio_alternative_val = ratio_alternative_val / alternative_val;
		}
      
	      //debug:
	      std::cout << "ratio_nominal_val: " << ratio_nominal_val << std::endl;
	      std::cout << "ratio_alternative_val: " << ratio_alternative_val << std::endl;

	      absolute_syst_val = (ratio_alternative_val - ratio_nominal_val) / ratio_nominal_val;
	    }
	  else
	    {
	      std::cout << "ERROR: syst not defined for ratios" << std::endl;
	      return 0;
	    }
	}
    }
      
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////

  //debug:
  std::cout << syst << " absolute_syst_val: " << absolute_syst_val << std::endl;

  //write to file with syst name

  TString syst_file_name = "";
    
  if(syst == "combined_syst" && ratio != "") {
    syst_file_name = syst_dir + "combined_ratios/" + ratio + bins_str + ".root";
    std::cout << "FILE TO BE OPEN " << syst_file_name << std::endl;
  }
  else if(syst.Contains("ratio") == false) {
    syst_file_name = syst_dir + channel_to_ntuple_name(channel) + "/" + syst + "_" + channel_to_ntuple_name(channel) + bins_str + ".root";
    std::cout << syst_file_name << std::endl;
  }
  else
    syst_file_name = TString::Format(VERSION) + "/systematics_root/" + ratio + "_" + syst + bins_str + ".root";

  TFile* syst_file = new TFile(syst_file_name,"recreate");

  TVectorD val(1);
  TVectorD err_lo(1);
  TVectorD err_hi(1);

  //val[0] = quantity_res; //instead of 1.
  val[0] = 1.00;
  err_lo[0] = absolute_syst_val;
  err_hi[0] = absolute_syst_val;
  
  val.Write("val");
  err_lo.Write("err_lo");
  err_hi.Write("err_hi");

  syst_file->Close();
  delete syst_file; 
}//end 

double pdf_syst(RooWorkspace& ws, int channel, double pt_min, double pt_max, double y_min, double y_max, double nominal_yield, TString syst)
{
  //calculates the signal_yield using different pdfs for signal or backgrounds (seperatly), and returns the maximum difference from the nominal yield.

  RooRealVar* fit_res;
  
  std::vector<std::string> signal = {"3gauss"}; //,"crystal", "1gauss"};
  std::vector<std::string> combinatorial = {"bern"}; //, "2exp", "power"};
  std::vector<std::string> jpsipi = {"no_jpsipi"};
  std::vector<std::string> jpsiX = {"jpsiX_gauss"};

  std::vector<std::string> pdf;
  TString pdf_name = "";
  
  std::cout << syst << std::endl;
  
  //copy the names of the pdfs into the pdf vector.
  if(syst.Contains("signal"))
    {
      pdf_name = "signal";
      pdf.reserve((int)signal.size()); 
      
      for(int i=0; i<(int)signal.size(); i++)
	pdf.push_back(signal[i]);
    }
  else
    if(syst.Contains("combinatorial"))
      {
	pdf_name = "background";
	pdf.reserve((int)combinatorial.size()); 
	
	for(int i=0; i<(int)combinatorial.size(); i++)
	  pdf.push_back(combinatorial[i]);
      }
    else
      if(syst.Contains("jpsipi"))
	{
	  pdf_name = "background";
	  pdf.reserve((int)jpsipi.size()); 
	  
	  for(int i=0; i<(int)jpsipi.size(); i++)
	    pdf.push_back(jpsipi[i]);
	}
      else
	if(syst.Contains("jpsiX"))
	  {
	    pdf_name = "background";
	    pdf.reserve((int)jpsiX.size()); 
	    
	    for(int i=0; i<(int)jpsiX.size(); i++)
	      pdf.push_back(jpsiX[i]);
	  }
  
  //to save the various yield results
  std::vector<double> yield_syst;
  yield_syst.reserve((int)pdf.size());
  
  double mass_min = 0.0;
  double mass_max = 0.0;
  
  //calculate systematics
  for(int i=0; i<(int)pdf.size(); i++)
    {
      if(syst.Contains("jpsiX") && pdf[i] == "jpsiX_gauss")
	{
	  //mass_min = 5.15;
	  //mass_max = 5.4; //(ws.var("mass"))->getMax();
	}

      std::cout << std::endl;
      std::cout << "SYST!" << std::endl;
      std::cout << std::endl;
      fit_res = bin_mass_fit(ws, channel, pt_min, pt_max, y_min, y_max, pdf[i], pdf_name.Data(), mass_min, mass_max);
      yield_syst.push_back((double)fit_res->getVal());
    }
  
  //print table at the end
  for(int i=0; i<(int)yield_syst.size(); i++)
    std::cout << pdf_name << "_syst[" << i << "]: " << pdf[i] << " : " << yield_syst[i] << std::endl;
    
  int i_max = 0;
  double max_diff = 0;
  
  for(int i=0; i<(int)yield_syst.size(); i++)
    {
      if(fabs(yield_syst[i] - nominal_yield) > max_diff)
	{
	  max_diff = fabs(yield_syst[i] - nominal_yield);
	  i_max = i;
	}
    }
  
  return yield_syst[i_max];
}

double mass_window_syst(RooWorkspace& ws, int channel, double pt_min, double pt_max, double y_min, double y_max, double nominal_yield, TString input_file)
{
  std::cout << "calculate mass_window_syst" << std::endl;

  RooRealVar mass = *(ws.var("mass"));
  RooRealVar* fit_res;

  //10% of the size of the mass window
  double mass_window_variation = 0.1 * fabs(mass.getMax() - mass.getMin());

  std::vector<double> mass_min;
  std::vector<double> mass_max;

  mass_min.push_back(mass.getMin());
  mass_min.push_back(mass.getMin() - mass_window_variation);
  mass_max.push_back(mass.getMax());
  mass_max.push_back(mass.getMax() + mass_window_variation);

  std::vector<double> range_syst;

  range_syst.reserve((int)mass_min.size());

  //Mass Range Systematics
  for(int i=0; i<(int)mass_min.size(); i++)
    {
      RooWorkspace* ws1 = new RooWorkspace("ws1","Bmass");
      set_up_workspace_variables(*ws1,channel,mass_min[i],mass_max[1-i]);
      read_data(*ws1, input_file, channel);
    
      fit_res = bin_mass_fit(*ws1, channel, pt_min, pt_max, y_min, y_max, "", "", mass_min[i], mass_max[1-i]);
      range_syst.push_back((double)fit_res->getVal());
    }
  
  for(int i=0; i<(int)range_syst.size(); i++)
    std::cout << "mass_window_syst[" << i << "]: " << range_syst[i] << std::endl;

  int i_max = 0;
  double max_diff = 0;
  
  for(int i=0; i<(int)range_syst.size(); i++)
    {
      if(fabs(range_syst[i] - nominal_yield) > max_diff)
	{
	  max_diff = fabs(range_syst[i] - nominal_yield);
	  i_max = i;
	}
    }
  
  return range_syst[i_max];
}

double reweighting_syst(int channel, double pt_min, double pt_max, double y_min, double y_max, double nominal_quantity) {
  std::vector<double> range_syst;
  std::vector<TString> reweight_var_names;
  reweight_var_names.push_back("lerrxy");
  reweight_var_names.push_back("mu1pt");
  reweight_var_names.push_back("mu1eta");
  reweight_var_names.push_back("tk1pt");
  reweight_var_names.push_back("tk1eta");
  //if(channel!=1) reweight_var_names.push_back("tktkmass");

  RooRealVar* eff_corrected;
  //int reweight_variables_number = static_cast<int>(reweight_var_names.size());  

  eff_corrected = reco_efficiency(channel, pt_min, pt_max, y_min, y_max, true, reweight_var_names);
  range_syst.push_back(static_cast<double>(eff_corrected->getVal()));
  std::cout << "Corrected efficiency  " << ": " << eff_corrected->getVal() << std::endl;
  
  int i_max = 0;
  double max_diff = 0.00;
  
  for(int i=0; i<static_cast<int>(range_syst.size()); i++) {
    if(fabs(range_syst[i] - nominal_quantity) > max_diff) {
      max_diff = fabs(range_syst[i] - nominal_quantity);
      i_max = i;
    }
  }
  
  std::cout << "alternative efficiency value : " << range_syst[i_max] << std::endl;

  return range_syst[i_max];
}
