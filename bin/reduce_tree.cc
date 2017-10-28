#include <sstream>
#include <TStyle.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TPaveText.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TNtupleD.h>
#include <TH1D.h>
#include <TLorentzVector.h>
#include <RooWorkspace.h>
#include <RooRealVar.h>
#include <RooConstVar.h>
#include <RooDataSet.h>
#include <RooGaussian.h>
#include <RooChebychev.h>
#include <RooBernstein.h>
#include <RooExponential.h>
#include <RooAddPdf.h>
#include <RooPlot.h>
#include "UserCode/B_production_x_sec_13_TeV/interface/myloop.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/channel.h"
#include "TMath.h"
using namespace RooFit;

//-----------------------------------------------------------------
// Definition of channel #
// channel = 1: B+ -> J/psi K+
// channel = 2: B0 -> J/psi K*
// channel = 3: B0 -> J/psi Ks
// channel = 4: Bs -> J/psi phi
// channel = 5: Jpsi + pipi
// channel = 6: Lambda_b -> Jpsi + Lambda

//input example: reduce_tree --channel 1 --mc 1 --gen 1 --input file
int main(int argc, char** argv)
{
  int channel = 1;
  int mc =1;
  int gen = 1;
  TString input_file = "";

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;

      if(argument == "--channel")
        {
          convert << argv[++i];
          convert >> channel;
        }
      if(argument == "--mc")
	{
	  convert << argv[++i];
          convert >> mc;
	}
      if(argument == "--gen")
	{
	  convert << argv[++i];
          convert >> gen;
	}
      
      if(argument == "--input")
        {
          convert << argv[++i];
          convert >> input_file;
        }
    }
  
  if(input_file == "")
    {
      printf("ERROR: no input file was indicated. Please use --input /some/file \n");
    }
  
  TFile* old_file = new TFile(input_file);
  TString ntuple_name = channel_to_ntuple_name(channel);
  
  if(mc && gen)
    ntuple_name += "_gen";
  
  //if(channel == 2 && mc && !gen) //to get the correctly assigned signal, and not include the swapped component.
  //ntuple_name += "_true";

  TTree* old_tree = (TTree*)old_file->Get(ntuple_name);

  old_tree->SetBranchStatus("*",0);

  //add here the branchs to save in the output file
  old_tree->SetBranchStatus("mass",1);
  old_tree->SetBranchStatus("pt",1);
  old_tree->SetBranchStatus("eta",1);
  old_tree->SetBranchStatus("y",1);
  old_tree->SetBranchStatus("lxy",1);
  old_tree->SetBranchStatus("errxy",1);
  
  old_tree->SetBranchStatus("mu1pt",1);
  old_tree->SetBranchStatus("mu1eta",1);

  old_tree->SetBranchStatus("mu2pt",1);
  old_tree->SetBranchStatus("mu2eta",1);

  /////////////////////////////////////////////////////////////////////////////////////
  
  TString output_file = "reduced_" + input_file;
  TFile *new_file = new TFile(output_file,"recreate");
  
  TTree* new_tree = old_tree->CloneTree(0);
  new_tree->CopyEntries(old_tree);

  new_file->Write();
  new_file->Close();

  delete old_file;
  delete new_file;
}
