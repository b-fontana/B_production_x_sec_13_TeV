#include <TChain.h>
#include <iostream>
#include <sstream>
#include <TTree.h>
#include <TH1D.h>
#include <TNtupleD.h>
#include <TFile.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include "UserCode/B_production_x_sec_13_TeV/interface/format.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/myloop.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/channel.h"

//mc_gen_signal_study --input file
int main(int argc, char** argv)
{
  TString input_file = "";
  int entries = 0;
  
  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--input")
        {
          convert << argv[++i];
          convert >> input_file;
        }
      if(argument == "--n")
        {
          convert << argv[++i];
          convert >> entries;
        }
    }
    
  if(entries < 0)
    entries = 0;

  TChain *root = new TChain("demo/root");
  
  root->Add(input_file);
  
  int n_entries = root->GetEntries();
  printf("Going to process %d entries.\n",n_entries);
  
  //-----------------------------------------------------------------
  // setting memory addresses to the branches
  // create new output trees
  
  GenInfoBranches *GenInfo = new GenInfoBranches;
  VtxInfoBranches *VtxInfo = new VtxInfoBranches;
  
  GenInfo->setbranchadd(root);
  VtxInfo->setbranchadd(root);
  
  int idx_b    = -1;
  int idx_jpsi = -1;
  int idx_tktk = -1;
  int idx_tk1  = -1;
  int idx_tk2  = -1;
  int idx_mu1  = -1;
  int idx_mu2  = -1;
  
  if(entries == 0)
    entries = n_entries;
  
  for (int evt=0; evt<entries; evt++) 
    {
      if (evt%1000==0 || evt==n_entries-1) printf("processing %d/%d (%.2f%%).\n",evt,n_entries-1,(double)evt/(double)(n_entries-1)*100.);
      
      root->GetEntry(evt);

      // Look for indices of the whole decay tree
      for (int idx = 0; idx < GenInfo->size; idx++) 
	{

	  idx_b    = idx;
	  idx_jpsi = GenInfo->da1[idx_b];
	  idx_tktk = GenInfo->da2[idx_b];
	  idx_tk1  = GenInfo->da1[idx_tktk];
	  idx_tk2  = GenInfo->da2[idx_tktk];
	  idx_mu1  = GenInfo->da1[idx_jpsi];
	  idx_mu2  = GenInfo->da2[idx_jpsi];
	  	  	  
	  if(fabs(GenInfo->pdgId[idx_b]) != 511 && fabs(GenInfo->pdgId[idx_b]) != 521 && fabs(GenInfo->pdgId[idx_b]) != 531 && fabs(GenInfo->pdgId[idx_b]) != 541) continue;

	  if(fabs(GenInfo->pdgId[idx_jpsi]) != 443) continue;
	  if(fabs(GenInfo->pdgId[idx_tk1]) != 211 && fabs(GenInfo->pdgId[idx_tk1]) != 321 ) continue;
	  if(fabs(GenInfo->pdgId[idx_tk2]) != 211 && fabs(GenInfo->pdgId[idx_tk2]) != 321 ) continue;


	  std::cout << "B("<< GenInfo->pdgId[idx_b] <<") -> J/Y("<< GenInfo->pdgId[idx_jpsi] <<") + tktk("<< GenInfo->pdgId[idx_tktk] <<") -> mu1("<< GenInfo->pdgId[idx_mu1] <<") + mu2("<< GenInfo->pdgId[idx_mu2] <<") + tk1("<< GenInfo->pdgId[idx_tk1] <<") + tk2("<< GenInfo->pdgId[idx_tk2] <<")" <<std::endl;


	  //std::cout << "da1 or JPSI pdgID : " << GenInfo->pdgId[idx_jpsi] << std::endl;
	  //std::cout << "da1->da1 or mu1  pdgID : " << GenInfo->pdgId[idx_mu1] << std::endl;
	  //std::cout << "da1->da2 or mu2  pdgID : " << GenInfo->pdgId[idx_mu2] << std::endl;

	  //std::cout << "da2 or tktk  pdgID : " << GenInfo->pdgId[idx_tktk] << std::endl;
	  //std::cout << "da2->da1 or tk1  pdgID : " << GenInfo->pdgId[idx_tk1] << std::endl;
	  //std::cout << "da2->da2 or tk2  pdgID : " << GenInfo->pdgId[idx_tk2] << std::endl;
	  
	} //end of GenInfo loop
    } // end of evt loop
  
  delete GenInfo;
}
