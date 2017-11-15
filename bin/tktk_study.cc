//This script is used to study the effect of the selection and veto of the tktk_mass in the B0 and Bs channels.
//For the B0 channel, for example, the idea is to read the true signal from MC and aplly the K* mass window cut, and plot it.
//Also plot the KK version of the signal in the Phi mass spectrum, to see what happens to the K* signal in the Phi spectrum.
//The same idea also works starting with Bs signal.

#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"

using namespace RooFit;

//-----------------------------------------------------------------
// Definition of channel #
// channel = 1: B+ -> J/psi K+
// channel = 2: B0 -> J/psi K*
// channel = 3: B0 -> J/psi Ks
// channel = 4: Bs -> J/psi phi
// channel = 5: Jpsi + pipi
// channel = 6: Lambda_b -> Jpsi + Lambda

//input example: tktk_study --channel 2 --input file --output /some/place
int main(int argc, char** argv)
{
  /////////////////////////////////////////
  double phi_window = 0.010;
  double kstar_window = 0.050;
  /////////////////////////////////////////

  int channel = 2;
  TString input_file = "";
  TString output_dir = "";
  
  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--channel")
        {
          convert << argv[++i];
          convert >> channel;
        }
      if(argument == "--input")
        {
          convert << argv[++i];
          convert >> input_file;
        }
      if(argument == "--output")
        {
          convert << argv[++i];
          convert >> output_dir;
        }
    }

  if(input_file == "")
    {
      std::cout << "no input file provided, please use --input" << std::endl;
      return 0;
    }
  
  //////////////////////////////////////////////////////////////////////////    
  TString signal = "";
  TString tree_name = channel_to_ntuple_name(channel);
    
  TChain* tin;
  ReducedBranches br;
  
  TH1D* ditrack_mass_true = new TH1D("empty1","empty1", 100, 0.7, 1.2);
  TH1D* ditrack_veto_true = new TH1D("empty2","empty2", 100, 0.7, 1.2);
  TH1D* ditrack_mass_swap = new TH1D("empty3","empty3", 100, 0.7, 1.2);
  TH1D* ditrack_veto_swap = new TH1D("empty4","empty4", 100, 0.7, 1.2);

  TH1D** ditrack_mass;
  TH1D** ditrack_veto;
  
  ditrack_mass = &ditrack_mass_true;
  ditrack_veto = &ditrack_veto_true;

  TLorentzVector v4_tk1, v4_tk2;
  
  TCanvas* cv1;
  cv1 = new TCanvas("cv1", "First canvas", 1280, 720);
  /////////////////////////////////////////////////////////////////////////

  std::cout << "Filling histograms without mass or veto cuts" << std::endl;
  std::cout << "Reading data from channel " << channel << std::endl;

  for(int i=0 ; i<2 ; i++)
    {
      if(i==0)
	{
	signal = "true";
	
	if(channel == 2)
	  {
	    ditrack_mass = &ditrack_mass_true;
	    ditrack_veto = &ditrack_veto_true;
	  }
	else
	  if(channel == 4)
	  {
	    ditrack_veto = &ditrack_mass_true;
	    ditrack_mass = &ditrack_veto_true;
	  }
	}
      else
	if(i==1)
	  {
	  signal = "swap";
	  
	  if(channel == 2)
	    tree_name += "_swap";
	  
	  if(channel == 2)
	    {
	      ditrack_mass = &ditrack_mass_swap;
	      ditrack_veto = &ditrack_veto_swap;
	    }
	  else
	    if(channel == 4)
	      {
		ditrack_veto = &ditrack_mass_swap;
		ditrack_mass = &ditrack_veto_swap;
	      }
	  }

      std::cout << "Processing " << signal << " component"<< std::endl;
      
      tin = new TChain(tree_name);
      tin->Add(input_file);
      br.setbranchadd(tin);

      if(channel == 2)
	{
	  *ditrack_mass = new TH1D("ditrack_mass_" + signal,"K #pi invariant mass", 100, 0.7, 1.2);  
	  *ditrack_veto = new TH1D("ditrack_veto_" + signal,"KK invariant mass", 100, 0.7, 1.2);
	}
      else
	if(channel == 4)
	  {
	    *ditrack_mass = new TH1D("ditrack_mass_" + signal,"KK invariant mass", 100, 0.7, 1.2);  
	    *ditrack_veto = new TH1D("ditrack_veto_" + signal,"K #pi invariant mass", 100, 0.7, 1.2);
	  }
      
      int n_entries = (int) tin->GetEntries();
      int percent = (int)(0.01*n_entries);
      
      std::cout << "Going to process " << n_entries << " events"<< std::endl;
      
      for (int evt=0; evt<n_entries; evt++) 
	{
	  //show the percentage of processed events
	  if (evt%percent==0 || evt==n_entries-1) printf("processing %d/%d (%.2f%%).\n",evt,n_entries-1,(double)evt/(double)(n_entries-1)*100.);
	  
	  tin->GetEntry(evt);
	  
	  switch(channel)
	    {
	    case 2:
	      (*ditrack_mass)->Fill(br.tktkmass);
	      
	      v4_tk1.SetPtEtaPhiM(br.tk1pt,br.tk1eta,br.tk1phi,KAON_MASS);
	      v4_tk2.SetPtEtaPhiM(br.tk2pt,br.tk2eta,br.tk2phi,KAON_MASS);
	      
	      (*ditrack_veto)->Fill(fabs((v4_tk1+v4_tk2).Mag()));
	      break;
	      
	    case 4:
	      (*ditrack_mass)->Fill(br.tktkmass);
	      
	      if(i==0) //one version of the signal
		{
		  v4_tk1.SetPtEtaPhiM(br.tk1pt,br.tk1eta,br.tk1phi,KAON_MASS);
		  v4_tk2.SetPtEtaPhiM(br.tk2pt,br.tk2eta,br.tk2phi,PION_MASS);
		  
		  (*ditrack_veto)->Fill(fabs((v4_tk1+v4_tk2).Mag()));
		}
	      else
		if(i==1) //another version of the signal
		  {
		    v4_tk1.SetPtEtaPhiM(br.tk1pt,br.tk1eta,br.tk1phi,PION_MASS);
		    v4_tk2.SetPtEtaPhiM(br.tk2pt,br.tk2eta,br.tk2phi,KAON_MASS);
		    
		    (*ditrack_veto)->Fill(fabs((v4_tk1+v4_tk2).Mag()));
		  }
	      break;
	    }
	}//end of event cicle
    }//end of signal cicle


  //////////////////////////////////////////////////////////

  cv1->Divide(2,1);
  cv1->cd(1);
  ditrack_mass_true->SetLineWidth(3);
  ditrack_mass_true->SetLineColor(kOrange);
  ditrack_mass_true->Draw();

  cv1->Update();
  ditrack_mass_swap->SetLineWidth(3);
  ditrack_mass_swap->Draw("same");
  
  TLine *l1 = new TLine(KSTAR_MASS - kstar_window, 0, KSTAR_MASS - kstar_window, ditrack_mass_true->GetMaximum());
  l1->SetLineWidth(2);
  l1->SetLineColor(kRed);
  l1->Draw("");

  TLine *l2 = new TLine(KSTAR_MASS + kstar_window, 0, KSTAR_MASS + kstar_window, ditrack_mass_true->GetMaximum());
  l2->SetLineWidth(2);
  l2->SetLineColor(kRed);
  l2->Draw("");

  /////////////////////////
  
  cv1->cd(2);
  ditrack_veto_true->SetLineColor(kOrange);
  ditrack_veto_true->SetLineWidth(3);
  ditrack_veto_true->Draw();
    
  cv1->Update();
  ditrack_veto_swap->SetLineWidth(3);
  ditrack_veto_swap->Draw("same");

  TLine *l3 = new TLine(PHI_MASS - phi_window, 0, PHI_MASS - phi_window, ditrack_veto_true->GetMaximum());
  l3->SetLineWidth(2);
  l3->SetLineColor(kRed);
  l3->Draw("");

  TLine *l4 = new TLine(PHI_MASS + phi_window, 0, PHI_MASS + phi_window, ditrack_veto_true->GetMaximum());
  l4->SetLineWidth(2);
  l4->SetLineColor(kRed);
  l4->Draw("");
  
  ///////////////////////////////////////////////////////////
  cv1->SaveAs(output_dir + "tktk_mass_and_veto_" + channel_to_ntuple_name(channel) + "_no_window_no_veto.png");

  delete cv1;
  delete tin;
}
