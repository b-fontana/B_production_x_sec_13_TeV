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

//input example: plot_ratio --ratio fsfu --bins pt_y --eff 1 --syst 0 --poly 1
int main(int argc, char** argv)
{
  std::string ratio = "fsfu";
  std::string bins = "pt";
  int poly = 1;
  int eff = 1;
  int syst = 0;

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
      if(argument == "--poly")
	{
	  convert << argv[++i];
	  convert >> poly;
	}
    }
  
  //to create the directories to save the .png files
  std::vector<std::string> dir_list;  
  dir_list.push_back(static_cast<const char*>(TString::Format(VERSION) + "/ratio"));
  create_dir(dir_list);
  
  //set up the vectors
  TString var1_name = "";
  TString var2_name = "";

  int n_var1_bins=1;
  int n_var2_bins=1;

  double* var1_bins = NULL;
  double* var2_bins = NULL;
  
  TString measure = "ratio";
  
  setup_bins(measure, 0, bins, &var1_name, &n_var1_bins, &var2_name, &n_var2_bins, &var1_bins, &var2_bins);
   
  //initialize arrays for yield, efficiencies, etc 
  double var1_bin_centre[n_var1_bins];
  double var1_bin_centre_lo[n_var1_bins];
  double var1_bin_centre_hi[n_var1_bins];
  
  double total_eff[2][n_var2_bins][n_var1_bins];
  double total_eff_err_lo[2][n_var2_bins][n_var1_bins];
  double total_eff_err_hi[2][n_var2_bins][n_var1_bins];
  
  double ratio_eff[n_var2_bins][n_var1_bins];
  double ratio_eff_err_lo[n_var2_bins][n_var1_bins];
  double ratio_eff_err_hi[n_var2_bins][n_var1_bins];

  double yield[2][n_var2_bins][n_var1_bins];
  double yield_err_lo[2][n_var2_bins][n_var1_bins];
  double yield_err_hi[2][n_var2_bins][n_var1_bins];

  double ratio_array[n_var2_bins][n_var1_bins];
  double ratio_err_lo[n_var2_bins][n_var1_bins];
  double ratio_err_hi[n_var2_bins][n_var1_bins];

  double combined_syst[2][n_var2_bins][n_var1_bins];
  double combined_syst_lo[2][n_var2_bins][n_var1_bins];
  double combined_syst_hi[2][n_var2_bins][n_var1_bins];
 
  double ratio_syst_lo[n_var2_bins][n_var1_bins];
  double ratio_syst_hi[n_var2_bins][n_var1_bins];

  double ratio_total_err_lo[n_var2_bins][n_var1_bins];
  double ratio_total_err_hi[n_var2_bins][n_var1_bins];
    
  double b_fraction[2];
  double b_fraction_err[2];

  double ratio_BF_err[n_var2_bins][n_var1_bins];
  
  double global_syst_err = 0.0;
  std::vector<std::string> global_syst_list;
  std::vector<double> global_syst_val;

  setup_global_syst_list(&global_syst_list, measure, ratio);
  setup_global_syst_val(&global_syst_val, measure, ratio);
  
  TString x_axis_name = "";
  if(var1_name =="pt")
    x_axis_name = "p_{T}(B) [GeV]";
  else
    x_axis_name = "|y|(B)";
  
  //calculate bin centre
  for(int i=0; i<n_var1_bins; i++)
    {
      var1_bin_centre[i] = var1_bins[i] + (var1_bins[i+1]-var1_bins[i])/2;
      var1_bin_centre_lo[i] = var1_bin_centre[i] - var1_bins[i];
      var1_bin_centre_hi[i] = var1_bins[i+1] - var1_bin_centre[i];
    }

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
      b_fraction_err[ch] = branch->getError();
      
      //read yield
      read_vector(channel, "yield", var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, yield[ch][0], yield_err_lo[ch][0], yield_err_hi[ch][0]);
	  
      //read efficiency
      if(eff)
	read_vector(channel, "totaleff", var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, total_eff[ch][0], total_eff_err_lo[ch][0], total_eff_err_hi[ch][0]);
      
      //read syst
      if(syst)
	read_vector(channel, "combined_syst", var1_name , var2_name, n_var1_bins, n_var2_bins, var1_bins, var2_bins, combined_syst[ch][0],combined_syst_lo[ch][0],combined_syst_hi[ch][0]);
      else
	for(int j=0; j<n_var2_bins; j++)
	  {
	    for(int i=0; i<n_var1_bins; i++)
	      {
		//syst set to zero
		combined_syst[ch][j][i]=0;
		combined_syst_lo[ch][j][i]=0;
		combined_syst_hi[ch][j][i]=0;
	      }
	  }
    }//end of ch cicle
  
  if(eff)
    {
      //calculate and plot ratioeff
      for(int j=0; j<n_var2_bins; j++)
	{
	  for(int i=0; i<n_var1_bins; i++)
	    {
	      ratio_eff[j][i] = total_eff[0][j][i] / total_eff[1][j][i];
	      ratio_eff_err_lo[j][i] = ratio_eff[j][i] * sqrt(pow(total_eff_err_lo[0][j][i]/total_eff[0][j][i],2) + pow(total_eff_err_lo[1][j][i]/total_eff[1][j][i],2));
	      ratio_eff_err_hi[j][i] = ratio_eff[j][i] * sqrt(pow(total_eff_err_hi[0][j][i]/total_eff[0][j][i],2) + pow(total_eff_err_hi[1][j][i]/total_eff[1][j][i],2));
	    }
	  plot_eff(measure, "ratioeff", 0, n_var1_bins, var2_name, var2_bins[j], var2_bins[j+1], x_axis_name, ratio, var1_bin_centre, var1_bin_centre_lo, var1_bin_centre_hi, ratio_eff[j], ratio_eff_err_lo[j], ratio_eff_err_hi[j]);
	}
    }
  
  //to calculate the ratio
  for(int j=0; j<n_var2_bins; j++)
    {
      for(int i=0; i<n_var1_bins; i++)
	{
	  ratio_array[j][i]  = yield[1][j][i] / yield[0][j][i];
	  ratio_array[j][i]  *= pow(10,j);
          	  
          if(eff)
	    {
	      ratio_array[j][i] *= ratio_eff[j][i] * (b_fraction[0]/b_fraction[1]);
	    }
	    	  
	  ratio_err_lo[j][i] = ratio_array[j][i] * sqrt(pow(yield_err_lo[0][j][i]/yield[0][j][i],2) + pow(yield_err_lo[1][j][i]/yield[1][j][i],2));
	  ratio_err_hi[j][i] = ratio_array[j][i] * sqrt(pow(yield_err_hi[0][j][i]/yield[0][j][i],2) + pow(yield_err_hi[1][j][i]/yield[1][j][i],2));

	  ratio_total_err_lo[j][i] = ratio_err_lo[j][i];
	  ratio_total_err_hi[j][i] = ratio_err_hi[j][i];	      

	  if(syst)
	    {
	      ratio_syst_lo[j][i]  = ratio_array[j][i] * sqrt( pow(combined_syst_lo[0][j][i],2) + pow(combined_syst_lo[1][j][i],2) );
	      ratio_syst_hi[j][i]  = ratio_array[j][i] * sqrt( pow(combined_syst_hi[0][j][i],2) + pow(combined_syst_hi[1][j][i],2) );
	      	      
	      //total error for fitting the ratio in the plot later
	      ratio_total_err_lo[j][i] = sqrt( pow(ratio_err_lo[j][i],2) + pow(ratio_syst_lo[j][i],2) );
	      ratio_total_err_hi[j][i] = sqrt( pow(ratio_err_hi[j][i],2) + pow(ratio_syst_hi[j][i],2) );	      
	    }
	}//var1 cicle
    }//var2 cicle

  //to calculate the BF err for each bin, not ideal for now
  for(int j=0; j<n_var2_bins; j++)
    {
      for(int i=0; i<n_var1_bins; i++)
	{
	  if(syst)
	    ratio_BF_err[j][i] = ratio_array[j][i] * sqrt( pow(b_fraction_err[0]/b_fraction[0],2) + pow(b_fraction_err[1]/b_fraction[1],2) );
	}
    }

  //adding the global uncertainties
  if(syst)
    {
      std::cout << "debug: calculating global systematic uncertainties of " << ratio << std::endl;
      std::cout << "debug: global systematics list:" << std::endl;
      
      for(int k=0 ; k < (int)global_syst_list.size() ; k++)
	{
	  std::cout << "syst " << k+1 << " : " << global_syst_list[k] << " : " << global_syst_val[k] << std::endl;
	  
	  global_syst_err += pow(global_syst_val[k],2);
	}
      
      global_syst_err = sqrt(global_syst_err);

      std::cout << "global syst err : " << global_syst_err << std::endl;
    }

  //plot the ratio
  TCanvas cz;
  
  double leg_x2 = 0.98;
  double leg_x1 = leg_x2 - 0.25;
  double leg_y2 = 0.98;
  double leg_y1 = leg_y2 - (0.05 * n_var2_bins);

  TLegend *leg = new TLegend (leg_x1, leg_y1, leg_x2, leg_y2);
  
  TLatex *tex = new TLatex(0.69,0.91,TString::Format("%.2f fb^{-1} (13 TeV)",LUMINOSITY));
  tex->SetNDC(kTRUE);
  tex->SetLineWidth(2);
  tex->SetTextSize(0.04);
  tex->Draw();
  tex = new TLatex(0.11,0.91,"CMS Preliminary");
  tex->SetNDC(kTRUE);
  tex->SetTextFont(42);
  tex->SetTextSize(0.04);
  tex->SetLineWidth(2);
  tex->Draw();
      
  //find ratio max and min
  double ratio_max = ratio_array[n_var2_bins-1][0] + ratio_syst_hi[n_var2_bins-1][0];
  double ratio_min = ratio_array[0][n_var1_bins-1] - ratio_syst_lo[0][n_var1_bins-1];

  for(int i=0; i<n_var1_bins; i++)
    {
      if((ratio_array[n_var2_bins-1][i] + ratio_syst_hi[n_var2_bins-1][i] ) > ratio_max)
	ratio_max = ratio_array[n_var2_bins-1][i] + ratio_syst_hi[n_var2_bins-1][i];
    }

  for(int i=0; i<n_var1_bins; i++)
    {
      if((ratio_array[0][i] - ratio_syst_lo[0][i]) < ratio_min)
	ratio_min = ratio_array[0][i] - ratio_syst_lo[0][i];
    }

  TString ratio_title = "";
  TString no_eff_ratio = "";
  
  if(ratio == "fsfu")
    {
      no_eff_ratio = "BsBu";
      
      if(eff)
	ratio_title = "#frac{f_s}{f_u}";
      else
	ratio_title = "#frac{N(B^0_s)}{N(B^+)}";
    }
  else
    if(ratio == "fsfd")
      {
	no_eff_ratio = "BsBd";
	
	if(eff)
	  ratio_title = "#frac{f_s}{f_d}";
	else
	  ratio_title = "#frac{N(B^0_s)}{N(B^0)}";
      }
    else
      if(ratio == "fdfu")
	{
	  no_eff_ratio = "BdBu";
	  
	  if(eff)
	    ratio_title = "#frac{f_d}{f_u}";
	  else
	    ratio_title = "#frac{N(B^0)}{N(B^+)}";
	}
  
  //ratio name
  TString ratio_name = "";
  
  if(eff)
    ratio_name = ratio;
  else
    ratio_name = no_eff_ratio;

  for(int j=0; j<n_var2_bins; j++)
    {
      TGraphAsymmErrors* graph = new TGraphAsymmErrors(n_var1_bins, var1_bin_centre, ratio_array[j], var1_bin_centre_lo, var1_bin_centre_hi, ratio_err_lo[j], ratio_err_hi[j]);
      
      //draw this for the first var2 bin, or in case there is only one bin.
      if(j==0) 
	{
	  graph->GetXaxis()->SetTitle(x_axis_name);
	  graph->GetYaxis()->SetTitle(ratio_title);
	  graph->GetYaxis()->SetRangeUser(0.5*ratio_min, 2*ratio_max);

	  //to set the range of the plot, it takes the min and max value of ratio.
	  if(n_var2_bins > 1)
            graph->GetYaxis()->SetRangeUser(0.1*ratio_min, 10*ratio_max);
          else
	    {
	      if(poly)
                {
		  gStyle->SetOptStat();
		  gStyle->SetOptFit();
		  
		  TGraphAsymmErrors* graph2 = new TGraphAsymmErrors(n_var1_bins, var1_bin_centre, ratio_array[j], var1_bin_centre_lo, var1_bin_centre_hi, ratio_total_err_lo[j], ratio_total_err_hi[j]);
 
		  //graph2->GetYaxis()->SetTitle(ratio_title);
		  //graph2->GetYaxis()->SetRangeUser(0.5*ratio_min, 2*ratio_max);
		  //graph2->GetXaxis()->SetTitle(x_axis_name);
		  
		  graph2->Fit("pol0","W","");
                  graph2->GetFunction("pol0")->SetLineColor(1);
                  graph2->Draw("ap");
		  gPad->Update();

		  TPaveStats* tps1 = (TPaveStats*) graph2->FindObject("stats");
		  tps1->SetTextColor(1);
		  tps1->SetLineColor(1);		  
		  tps1->Draw("same");

		  ///////////////////////////////////////////////////////
		  TGraphAsymmErrors* graph3 = (TGraphAsymmErrors*)graph2->Clone("graph3");
		  		  		  
		  graph3->Fit("pol1","W","");
		  graph3->GetFunction("pol1")->SetLineColor(4);
		  graph3->Draw("pX same");
		  gPad->Update();
		  		  
		  double X1 = tps1->GetX1NDC();
		  double Y1 = tps1->GetY1NDC();
		  double X2 = tps1->GetX2NDC();
		  double Y2 = tps1->GetY2NDC();
		  
		  TPaveStats *tps2 = (TPaveStats*) graph3->FindObject("stats");
		  tps2->SetTextColor(4);
		  tps2->SetLineColor(4);
		  tps2->SetX1NDC(X1);
		  tps2->SetX2NDC(X2);
		  tps2->SetY1NDC(Y1-(Y2-Y1));
		  tps2->SetY2NDC(Y1);
		  tps2->Draw("same");
		  //////////////////////////////////////////////////////

		  cz.Update();
		}
	      else
		{
		  graph->Draw("ap same");
		  cz.Update();
		}
	    }
	}
      else //for the rest of the rapidity bins.
	{     
	  graph->Draw("p same");
	}
      
      //print the var2 bin label
      TString label = "";
      
      if(j!=0)
        label += TString::Format("(#times 10^{%d}) ",j);
      
      if(var1_name == "pt")
        label += TString::Format("%.2f",var2_bins[j]) + " < |y| < " + TString::Format("%.2f",var2_bins[j+1]);
      else
        label += TString::Format("%d",(int)var2_bins[j]) + " < pt < " + TString::Format("%d",(int)var2_bins[j+1]);
      
      leg->AddEntry(graph, label, "lp");
      
      //systematic errors
      if(eff && syst && !poly)
        {
	  TGraphAsymmErrors* graph_syst = new TGraphAsymmErrors(n_var1_bins, var1_bin_centre, ratio_array[j], var1_bin_centre_lo, var1_bin_centre_hi, ratio_syst_lo[j], ratio_syst_hi[j]);
	  graph_syst->SetFillColor(j+2);
	  graph_syst->SetFillStyle(3001);
	  graph_syst->Draw("2 same");
	}
    }//end of var2 cicle
  
  leg->Draw("same");

  if(eff && syst)
    {
      TLatex *syst_tex = new TLatex(0.1,0.85,TString::Format("global systematic uncertainty %.2f %%", 100*global_syst_err));
      syst_tex->SetNDC(kTRUE);
      syst_tex->SetTextSize(0.03);
      syst_tex->Draw("same");
    }
  
  cz.Update();
  
  if(n_var2_bins > 1)
    cz.SetLogy();
  
  TString systematic = "";
  TString poly_fit = "";

  if(syst)
    systematic = "_syst";
 
  if(poly)
    poly_fit = "_poly";
  
  cz.SaveAs(TString::Format(VERSION) + "/ratio/" + ratio_name + "_" + bins + "_bins" + systematic + poly_fit + ".png");
  
  /////////////////////////////////////////////////////////////
  //To show the values and the errors at the end, like a table/
  /////////////////////////////////////////////////////////////

  //ratio eff
  //print_table("EFFICIENCY RATIO", n_var1_bins, n_var2_bins, var1_name, var2_name, var1_bins, var2_bins, ratio_eff[0], ratio_eff_err_lo[0], ratio_eff_err_hi[0]);

  std::cout << "=== " << ratio_name << " ===" << std::endl;

  //Fragmentation fraction
  print_table("FRAGMENTATION FRACTION RATIO", n_var1_bins, n_var2_bins, var1_name, var2_name, var1_bins, var2_bins, ratio_array[0], ratio_err_lo[0], ratio_err_hi[0], ratio_syst_lo[0], ratio_syst_hi[0], ratio_BF_err[0]);

}//end
