#include "UserCode/B_production_x_sec_13_TeV/interface/BDefinitions.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/BChannel.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/BWarnings.h"

double getWeight(double var, TH1D* h_weight);
void Reweight(TH1D* h_weight, TFile* f_input, int channel, std::string var_str);
TH1D* getHistogram(TFile *f, std::string var_str, int channel);
//TH1D* createHistogram(TNtupleD* nt, std::string var_str, int channel);

int main(int argc, char** argv) {
  BWarnings Warn;

  if (argc != 5 && argc != 7)
    {
      Warn.RunningError("NUMBER_ERROR");
      return 0;
    }

  std::string arg1 = argv[1], arg2 = argv[3];
  if(arg1 != "--channel" || arg2 != "--var")
    {
      Warn.RunningError("NAME_ERROR");
      return 0;
    }

  int channel = 0;
  std::string var_str = nullptr, mc_str = nullptr;
  std::stringstream convert1, convert2;
  
  convert1 << argv[2];
  convert1 >> channel;
  if(channel!=1 && channel!=2 && channel!=4)
    {
      Warn.RunningError("CHANNEL_ERROR");
      return 0;
    }
  
  convert2 << argv[4];
  convert2 >> var_str;
 
  if(argc == 7) 
    {
      std::string arg3 = argv[5];
      if (arg3 != "--input_mc")
	{
	  Warn.RunningError("NAME_ERROR");
	  return 0;
	}
      std::stringstream convert3;
      convert3 << argv[6];
      convert3 >> mc_str;
    }

  /////////OPEN FILES//////////////////////////
  BChannel Chn;
  TFile *f_weight = new TFile( ("weights_" + Chn.channel_to_ntuple_name(channel) + ".root").c_str(),"READ");
  if (f_weight==NULL) std::cout << "ERROR: The file f_weight could not be opened." << std::endl;
  
  TFile *f_mc = nullptr;
  if (mc_str != nullptr) f_mc = new TFile(mc_str.c_str(),"READ");
  else  f_mc = new TFile( ("/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/myloop_new_mc_truth_" +  Chn.channel_to_ntuple_name(channel) + "_with_cuts.root").c_str(),"READ");

  if (f_mc==nullptr) std::cout << "ERROR: The file f_mc could not be opened." << std::endl;

  /////////HISTOGRAMS//////////////////////////
  TH1D *h_weight = getHistogram(f_weight, var_str, channel);
  if (h_weight==nullptr) std::cout << "ERROR: The histogram does not exist (main)!" << std::endl;

  //check to see that the weights being used are the one we really want
  TCanvas test;
  test.cd();
  h_weight->Draw();
  test.SaveAs("test.png");

  Reweight(h_weight,f_mc,channel,var_str);

  /////////DELETE POINTERS/////////////////////
  f_weight->Close();
  f_mc->Close();

  return 0;
}

/********************
Function that returns the weight to be applied on each event.
Arguments: the double represents the variable that is being used to extract the weights, the histogram 'h_var' the place where the same variable is kept, and the histogram 'h_weights' the place where weights are stored
*********************/
double getWeight(double var, TH1D* h){
  int bin = h->FindBin(var);
  return h->GetBinContent(bin);
}

/*******************
Function that returns a histogram contained in a file.
The name of the histograms must obey the following rule: "channel" + channel_number + var_name.
********************/
TH1D* getHistogram(TFile *f, std::string var_str, int channel) {
  if (f==nullptr) std::cout << "ERROR: The file could not be opened." << std::endl;
  f->cd();
  TH1D *h = (TH1D*)f->Get(("channel" + std::to_string(channel) + var_str).c_str());
  if (h==nullptr) std::cout << "ERROR: The histogram was not found." << std::endl;
  return h;
}

/*******************
Function that creates a histogram from a NtupleD
Arguments: 'nt' is the Ntuple and 'var_str' is the name of the branch whose data we want to transform into a histogram
*******************/
/*TH1D* createHistogram(TNtupleD* nt, std::string var_str, int channel) {
  if (nt == NULL) std::cout << "ERROR: The pointer points to nothing! (createHistogram)" << std::endl;
  int nentries = nt->GetEntries();
  double var1, var2;
  nt->SetBranchAddress("lxy",&var1);
  nt->SetBranchAddress("errxy",&var2);
  TH1D* h = new TH1D(var_str.c_str(),var_str.c_str(),BIN_NUMBER,getXRange(var_str,channel).at(0),getXRange(var_str,channel).at(1));
  for (int i=0; i<nentries; ++i) {
    nt->GetEntry(i);
    h->Fill(var1/var2);
  }
  return h;
}
*/
/****************************
The function uses the weights contained in one histogram to reweight histograms contained in a tree, event by event, and store the corrected histograms into a new file.
It is assumed that all corrected histograms have the same number of bins, and that the same happens with the histogram that contains the weights.
It is also assumed that the histograms stored in the input file have the name of the corresponding variable.
Arguments: 'h_weight' is the histogram where the weights to be applied are stored, 'f_input' is the file where the uncorrected histograms are stored, 'channel' is the decay channel considered and 'var_str' is the name of the variable whose weights are being used.
*****************************/
void Reweight(TH1D* h_weight, TFile *f_input, int channel, std::string var_str) {
  
  BChannel Chn;
  std::vector<double> var(VAR_NUMBER);
  std::vector<TH1D*> h_out(VAR_NUMBER); 
  TNtupleD* nt = (TNtupleD*)f_input->Get(Chn.channel_to_ntuple_name(channel).c_str());
  if (nt==NULL) std::cout << "The pointer points to nothing! (Reweight)" << std::endl;
  TFile *f_output = new TFile("reweighted_mc.root","RECREATE");
  f_output->cd();

  /*for (int i=0; i<VAR_NUMBER; ++i) {
    tr->Branch(varName(i+1).c_str(), &var_out.at(i), (varName(i+1)+"/D").c_str() );
  }
  */
  //this extra branch is used to help checking that the weights are being correctly extracted

  int lxy_id = 99, errxy_id = 99, pt_id = 99, mass_id = 99;
  for (int i=0; i<VAR_NUMBER; ++i) {
    if (VAR_LIST.at(i).compare("lerrxy")!=0 && VAR_LIST.at(i).compare("propert")!=0) {
      nt->SetBranchAddress(VAR_LIST.at(i).c_str(),&var.at(i));
      if (VAR_LIST.at(i) == "lxy") lxy_id = i;
      else if (VAR_LIST.at(i) == "errxy") errxy_id = i;
      else if (VAR_LIST.at(i) == "pt") pt_id = i;
      else if (VAR_LIST.at(i) == "mass") mass_id = i;
    } 
  }
  if (lxy_id == 99 || errxy_id == 99 || pt_id == 99 || mass_id == 99) std::cout << "ERROR: The variable did not change its value! (Reweight)" << std::endl;

  BDefinitions Def;
  for (int i=0; i<VAR_NUMBER; ++i) {
    h_out.at(i) = new TH1D((VAR_LIST.at(i)+"").c_str(), (VAR_LIST.at(i)+"").c_str(), BIN_NUMBER, Def.SelectionVarRange(VAR_LIST.at(i).c_str(),channel).first, Def.SelectionVarRange(VAR_LIST.at(i).c_str(),channel).second);
  }

  double weight = 0.;
  int nentries = nt->GetEntries();
  for (int j=0; j<nentries; ++j) {
    nt->GetEntry(j);
    weight = getWeight(var.at(Def.VarID(var_str)),h_weight);
    for (int k=0; k<VAR_NUMBER; ++k) {
      if (VAR_LIST.at(k) == "lerrxy") {
	h_out.at(k)->Fill(var.at(lxy_id)/(var.at(errxy_id)*1.14),weight);
      }
      else if (VAR_LIST.at(k) == "propert") {
	h_out.at(k)->Fill(var.at(mass_id)*var.at(lxy_id)/var.at(pt_id),weight);
      }
      else if (VAR_LIST.at(k) == "errxy") {
	h_out.at(k)->Fill(var.at(errxy_id)*1.14,weight);
      }
      else {
	h_out.at(k)->Fill(var.at(k),weight);
      }
    }               
  }

  f_output->Write();
  f_output->Close();
}

