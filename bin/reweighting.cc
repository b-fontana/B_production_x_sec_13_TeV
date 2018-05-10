#include "/home/t3cms/balves/work/CMSSW_9_2_12/src/UserCode/B_production_x_sec_13_TeV/interface/BDefinitions.h"
#include "/home/t3cms/balves/work/CMSSW_9_2_12/src/UserCode/B_production_x_sec_13_TeV/interface/functions.h"
#include "/home/t3cms/balves/work/CMSSW_9_2_12/src/UserCode/B_production_x_sec_13_TeV/interface/BWarnings.h"

double getWeight(double var, TH1D* h_weight);
void Reweight(std::vector<TH1D*> h_weights, TFile* f_input, int channel, std::vector<std::string> var_str);
TH1D* getHistogram(TFile *f, std::string var_str, int channel, bool cuts);

//example: reweight --channel 4 --var1 lerrxy --var2 mu1pt --input_mc mc.root
int main(int argc, char** argv) {
  BWarnings Warn;

  if (argc != 5 && argc != 7 && argc != 9 && argc != 11 && argc != 13 && argc != 15)
    {
      Warn.RunningError("NUMBER_ERROR");
      return 0;
    }

  std::string arg1 = argv[1], arg2 = argv[3];
  if(arg1 != "--channel" || arg2 != "--var1")
    {
      Warn.RunningError("NAME_ERROR");
      return 0;
    }

  int channel = 0;
  std::string var1_str, var2_str, var3_str, var4_str, var5_str; 
  std::string mc_str;
  std::stringstream convert1, convert2;
  bool cuts = 1;

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;
      if(argument == "--channel"){convert << argv[++i];convert >> channel;}
      else if(argument == "--var1"){convert << argv[++i];convert >> var1_str;}
      else if(argument == "--var2"){convert << argv[++i];convert >> var2_str;}
      else if(argument == "--var3"){convert << argv[++i];convert >> var3_str;}
      else if(argument == "--var4"){convert << argv[++i];convert >> var4_str;}
      else if(argument == "--var5"){convert << argv[++i];convert >> var5_str;}
      else if(argument == "--input_mc"){convert << argv[++i];convert >> mc_str;}
      else if(argument == "--cuts"){convert << argv[++i];convert >> cuts;}
    }
  if(channel!=1 && channel!=2 && channel!=4){Warn.RunningError("CHANNEL_ERROR");return 0;}

  TFile *f_weight = new TFile("weights_2015rereco_" + channel_to_ntuple_name(channel) + ".root","READ");
  if (f_weight==nullptr) std::cout << "ERROR: The file f_weight could not be opened." << std::endl;
  
  TFile *f_mc = nullptr;
  if (mc_str.size() != 0) f_mc = new TFile(mc_str.c_str(),"READ");
  else  f_mc = new TFile("/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/final_selection/myloop_new_mc_truth_"+channel_to_ntuple_name(channel)+"_with_cuts.root", "READ");

  if (f_mc==nullptr) std::cout << "ERROR: The file f_mc could not be opened." << std::endl;

  std::vector<std::string> var_str = {var1_str, var2_str, var3_str, var4_str, var5_str};
  std::vector<TH1D*> h_weights;
  for(unsigned int j=0; j<var_str.size(); ++j)
    h_weights.push_back( getHistogram(f_weight, var_str[j], channel, cuts) );

  for(unsigned int i=0; i<h_weights.size(); ++i)
    if (h_weights[i]==nullptr) std::cout << "ERROR: The histogram does not exist (main)!" << std::endl;

  Reweight(h_weights,f_mc,channel,var_str);

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
  //return 1.;
}

/*******************
Function that returns a histogram contained in a file.
The name of the histograms must obey the following rule: "channel" + channel_number + var_name.
********************/
TH1D* getHistogram(TFile *f, std::string var_str, int channel, bool cuts) {
  if (f==nullptr) std::cout << "ERROR: The file could not be opened." << std::endl;
  f->cd();
  TH1D *h = nullptr;
  if(cuts) h = (TH1D*)f->Get( (var_str + "_with_cuts").c_str() );
  else h = (TH1D*)f->Get( (var_str + "_no_cuts").c_str() );

  if (h==nullptr) std::cout << "ERROR: The histogram was not found." << std::endl;
  return h;
}

void Reweight(std::vector<TH1D*> h_weights, TFile *f_input, int channel, std::vector<std::string> var_str) {  
  std::vector<double> var(VAR_NUMBER);
  std::vector<TH1D*> h_out(VAR_NUMBER); 
  TNtupleD* nt = (TNtupleD*)f_input->Get(channel_to_ntuple_name(channel));
  if (nt==nullptr) std::cout << "The pointer points to nothing! (Reweight)" << std::endl;
  TFile *f_output = new TFile("reweighted_mc_" + channel_to_ntuple_name(channel) + "_" + var_str[0].c_str() + ".root" ,"RECREATE");
  f_output->cd();

  int lxy_id = 99, errxy_id = 99, pt_id = 99, mass_id = 99;
  for (int i=0; i<VAR_NUMBER; ++i) {
    if (VAR_LIST.at(i).compare("lerrxy")!=0 && VAR_LIST.at(i).compare("propert")!=0) {
      nt->SetBranchAddress(VAR_LIST.at(i).c_str(),&var.at(i));
      std::cout << VAR_LIST.at(i).c_str() << std::endl;
      if (VAR_LIST.at(i) == "lxy") lxy_id = i;
      else if (VAR_LIST.at(i) == "errxy") errxy_id = i;
      else if (VAR_LIST.at(i) == "pt") pt_id = i;
      else if (VAR_LIST.at(i) == "mass") mass_id = i;
    } 
  }
  if (lxy_id == 99 || errxy_id == 99 || pt_id == 99 || mass_id == 99) std::cout << "ERROR: The variable did not change its value! (Reweight)" << std::endl;

  BDefinitions Def;
  for (int i=0; i<VAR_NUMBER; ++i) {
    /*if (i==11) //change name of histogram to "errxy_shifted"; the weights already reflect the shift 
      h_out.at(i) = new TH1D((VAR_LIST.at(i)+"_shifted").c_str(), (VAR_LIST.at(i)+"_shifted").c_str(), 100, Def.SelectionVarRange(VAR_LIST.at(i).c_str(),channel).first, Def.SelectionVarRange(VAR_LIST.at(i).c_str(),channel).second);
      else {*/
      h_out.at(i) = new TH1D((VAR_LIST.at(i)+"").c_str(), (VAR_LIST.at(i)+"").c_str(), 100, Def.SelectionVarRange(VAR_LIST.at(i).c_str(),channel).first, Def.SelectionVarRange(VAR_LIST.at(i).c_str(),channel).second);
      //}
  }

  double weight = 1.;
  int nentries = nt->GetEntries();
  for (int j=0; j<nentries; ++j) {
    if (j%100000==0) std::cout << "Event: " << j << std::endl;
    nt->GetEntry(j);
    weight = 1; //initialize the weight for each event
    //calculate the weights and multiply all of them, depending on how many reweighting variables are chosen
    for(unsigned int x=0; x<var_str.size(); ++x)
      {
	if(var_str[x].compare("lerrxy")==0)
	  weight *= getWeight( var.at(lxy_id)/(/*1.14**/var.at(errxy_id)), h_weights[x]);
	else if(var_str[x].compare("propert")==0)
	  weight *= getWeight( var.at(Def.VarID("mass"))*var.at(Def.VarID("lxy"))/var.at(Def.VarID("pt")), h_weights[x]);
	else if(var_str[x].compare("errxy")==0)
	  weight *= getWeight( /*1.14**/var.at(Def.VarID("errxy")), h_weights[x]);
	else
	  weight *= getWeight(var.at(Def.VarID(var_str[x])),h_weights[x]);
      }

    for (int k=0; k<VAR_NUMBER; ++k) {
      if (VAR_LIST.at(k) == "lerrxy") {
	h_out.at(k)->Fill(var.at(lxy_id)/(/*1.14**/var.at(errxy_id)),weight);
      }
      else if (VAR_LIST.at(k) == "propert") {
	h_out.at(k)->Fill(var.at(mass_id)*var.at(lxy_id)/var.at(pt_id),weight);
      }
      else if (VAR_LIST.at(k) == "errxy") {
	h_out.at(k)->Fill(/*1.14**/var.at(errxy_id),weight);
      }
      else {
	h_out.at(k)->Fill(var.at(k),weight);
      }
    }               
  }

  f_output->Write();
  f_output->Close();
}
