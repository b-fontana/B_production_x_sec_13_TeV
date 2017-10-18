#include <iostream>
#include <TChain.h>
#include <sstream>
#include <TTree.h>
#include <TFile.h>
#include <TMath.h>
#include <TLorentzVector.h>
#include <TVector3.h>
#include "UserCode/B_production_x_sec_13_TeV/interface/format.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/myloop.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/channel.h"

//mc_reco_signal_study --input file
int main(int argc, char** argv)
{
  TString input_file = "";

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--input")
	{
          convert << argv[++i];
          convert >> input_file;
        }
    }

    TChain *root = new TChain("demo/root");

    root->Add(input_file);
    
    int n_entries = root->GetEntries();
    //int percent = n_entries/100;

    printf("Going to process %d entries.\n",n_entries);
    
    //-----------------------------------------------------------------
    // setting memory addresses to the branches

    GenInfoBranches *GenInfo = new GenInfoBranches;    
    EvtInfoBranches *EvtInfo = new EvtInfoBranches;
    VtxInfoBranches *VtxInfo = new VtxInfoBranches;
    MuonInfoBranches *MuonInfo = new MuonInfoBranches;
    TrackInfoBranches *TrackInfo = new TrackInfoBranches;
    BInfoBranches *BInfo = new BInfoBranches;
    
    GenInfo->setbranchadd(root);
    EvtInfo->setbranchadd(root);
    VtxInfo->setbranchadd(root);
    MuonInfo->setbranchadd(root);
    TrackInfo->setbranchadd(root);
    BInfo->setbranchadd(root);
    
    for (int evt=0; evt<n_entries; evt++)
      {
	//if (evt%percent==0 || evt==n_entries-1) printf("processing %d/%d (%.2f%%).\n",evt,n_entries-1,(double)evt/(double)(n_entries-1)*100.);
        
        root->GetEntry(evt);
	
	int ujidx  = -1;
	int tk1idx = -1;
	int tk2idx = -1;
	int mu1idx = -1;
	int mu2idx = -1;

	std::cout << "==================== " << std::endl;
	std::cout << "Event number : " << evt << std::endl;
	std::cout << "B counter : " << BInfo->size << std::endl;
	std::cout << "Mu counter : " << MuonInfo->size << std::endl;
	std::cout << "Tk counter : " << TrackInfo->size << std::endl;
	std::cout << std::endl;
	
	/*
	//track info
	for (int tkidx = 0; tkidx < TrackInfo->size; tkidx++)
	  {
	    if(GenInfo->pdgId[TrackInfo->geninfo_index[tkidx]] != 0)
	      std::cout << "tk("<< tkidx <<")("<< GenInfo->pdgId[TrackInfo->geninfo_index[tkidx]] <<")" << std::endl;
	  }
	*/
	
	// Start of BInfo loop
	for (int bidx = 0; bidx < BInfo->size; bidx++)
	  {
	    int b_type = BInfo->type[bidx];
	    
	    //the indices to run over the Binfo. These are used to identify the signal when running on MC.
	    ujidx = BInfo->rfuj_index[bidx];
	    tk1idx = BInfo->rftk1_index[bidx];
	    tk2idx = BInfo->rftk2_index[bidx];
	    mu1idx = BInfo->uj_rfmu1_index[ujidx];
	    mu2idx = BInfo->uj_rfmu2_index[ujidx];
	
	    //channel 1
	    if (b_type != 1 && b_type != 2) continue; // skip any non K+ or pi+
	    
	    if (abs(GenInfo->pdgId[MuonInfo->geninfo_index[mu1idx]]) != 13) continue;
	    if (abs(GenInfo->pdgId[MuonInfo->geninfo_index[mu2idx]]) != 13) continue;
	    if (GenInfo->mo1[MuonInfo->geninfo_index[mu1idx]] != GenInfo->mo1[MuonInfo->geninfo_index[mu2idx]]) continue;
	    if (abs(GenInfo->pdgId[GenInfo->mo1[MuonInfo->geninfo_index[mu1idx]]]) != 443) continue;
	    if (abs(GenInfo->pdgId[TrackInfo->geninfo_index[tk1idx]]) != 321) continue; //skip any tk that was not generated as a K+-
	    if (GenInfo->mo1[GenInfo->mo1[MuonInfo->geninfo_index[mu1idx]]] != GenInfo->mo1[TrackInfo->geninfo_index[tk1idx]]) continue;
	    if (abs(GenInfo->pdgId[GenInfo->mo1[GenInfo->mo1[MuonInfo->geninfo_index[mu1idx]]]]) != 521) continue;
	    
	    std::cout << "Reco decay chain: B("<< bidx <<")("<< GenInfo->pdgId[GenInfo->mo1[GenInfo->mo1[MuonInfo->geninfo_index[mu1idx]]]] <<")-> uj("<< ujidx <<")("<< GenInfo->pdgId[GenInfo->mo1[MuonInfo->geninfo_index[mu1idx]]] <<")-> mu1("<< mu1idx <<")("<< GenInfo->pdgId[MuonInfo->geninfo_index[mu1idx]] <<") + mu2("<< mu2idx <<")("<< GenInfo->pdgId[MuonInfo->geninfo_index[mu2idx]] <<") + tk1("<< tk1idx <<")("<< GenInfo->pdgId[TrackInfo->geninfo_index[tk1idx]] <<") + tk2("<< tk2idx <<")("<< GenInfo->pdgId[TrackInfo->geninfo_index[tk2idx]] <<")" <<std::endl;

	  } // end of BInfo loop
      } // end of evt loop
    
    delete GenInfo;
    delete EvtInfo;
    delete VtxInfo;
    delete MuonInfo;
    delete TrackInfo;
    delete BInfo;
}
