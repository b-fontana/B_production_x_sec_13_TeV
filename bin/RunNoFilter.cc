#include "UserCode/B_production_x_sec_13_TeV/interface/functions.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/BWarnings.h"
#include <iostream>
#include <fstream>

int main(int argc, char** argv)
{
  int channel = 1;
  bool json = false;
  BWarnings Warn;
  TString sample_kind, particle;

  for(int i=1 ; i<argc ; ++i)
    {
      std::string argument = argv[i];
      std::stringstream convert;
      if(argument == "--channel")
        {
          convert << argv[++i];
          convert >> channel;
        }
      if(argument == "--json")
        {
          convert << argv[++i];
          convert >> json;
        }
    }

  //int RunLimits[6] = {0, 257000, 258100, 259000, 260000, 270000};
  int RunLimits[6] = {0, 257500, 258250, 259000, 260400, 270000};

  /*  TFile* file = new TFile("/lstore/cms/brunogal/input_for_B_production_x_sec_13_TeV/new_inputs/myloop_new_data_" + channel_to_ntuple_name(channel) + "_with_cuts.root","OPEN");
  Warn.CheckPointer(file);
  TTree* tree = static_cast<TTree*>(file->Get(channel_to_ntuple_name(channel)));
  Warn.CheckPointer(tree);
  TFile* f1 = new TFile("/lstore/cms/balves/Jobs/Full_Dataset_2015_Rereco/D/D1/"+channel_to_ntuple_name(channel)+"_2.root", "RECREATE");
  TTree* t1 = tree->CloneTree(0);
  TFile* f2 = new TFile("/lstore/cms/balves/Jobs/Full_Dataset_2015_Rereco/D/D2/"+channel_to_ntuple_name(channel)+"_2.root", "RECREATE");
  TTree* t2 = tree->CloneTree(0);
  TFile* f3 = new TFile("/lstore/cms/balves/Jobs/Full_Dataset_2015_Rereco/D/D3/"+channel_to_ntuple_name(channel)+"_2.root", "RECREATE");
  TTree* t3 = tree->CloneTree(0);
  TFile* f4 = new TFile("/lstore/cms/balves/Jobs/Full_Dataset_2015_Rereco/D/D4/"+channel_to_ntuple_name(channel)+"_2.root", "RECREATE");
  TTree* t4 = tree->CloneTree(0);  
  TFile* f5 = new TFile("/lstore/cms/balves/Jobs/Full_Dataset_2015_Rereco/D/D5/"+channel_to_ntuple_name(channel)+"_2.root", "RECREATE");
  TTree* t5 = tree->CloneTree(0);

  int run;
  tree->SetBranchAddress("run",&run);

  int nentries = tree->GetEntries();
  for(int entry=0; entry<nentries; ++entry) {
    tree->GetEntry(entry);
    if (run > RunLimits[0] && run < RunLimits[1]) { 
      f1->cd();
      t1->Fill();
    }
    else if (run >= RunLimits[1] && run < RunLimits[2]) {
      f2->cd();
      t2->Fill();
    }
    else if (run >= RunLimits[2] && run < RunLimits[3]) {
      f3->cd();
      t3->Fill();
    }
    else if (run >= RunLimits[3] && run < RunLimits[4]) {
      f4->cd();
      t4->Fill();
    }
    else if (run >= RunLimits[4]) {
      f5->cd();
      t5->Fill();
    }
    else std::cout << run << ": this run is not covered in the defined range!" << std::endl;
  }

  f1->Write();
  f2->Write();
  f3->Write();
  f4->Write();
  f5->Write();

  delete file;
  delete f1;
  delete f2;
  delete f3;
  delete f4;  
  delete f5;
  */  
  //////////////////////////////////////
  //////////JSON MANIPULATOR////////////
  //////////////////////////////////////
  if(json) 
    {
      int jsonOutSize = 5;
      std::vector<std::ifstream> jsonIn(7);
      std::vector<std::ofstream> jsonOut(jsonOutSize);
      std::string pathIn = "/home/t3cms/balves/work/CMSSW_8_0_22/src/UserCode/Bfinder/lumis2015rereco/";
      std::string pathOut = "/lstore/cms/balves/Jobs/Full_Dataset_2015_Rereco/D/";

      for (int i=0; i<7; ++i) {
	jsonIn.at(i).open(pathIn + "D" + std::to_string(i+1) + ".json");
	if(!jsonIn[i]) std::cout << "Input file did not open!" << std::endl;
      }
      for (int i=0; i<jsonOutSize; ++i) {
	jsonOut.at(i).open(pathOut + "D" + std::to_string(i+1) + "/D" + std::to_string(i+1) + "_2.json", std::ios_base::trunc);
	if(!jsonOut[i]) std::cout << "Output file did not open!" << std::endl;
      }

      for (int i=0; i<jsonOutSize; ++i) {
	jsonOut[i] << "{";
      }

      std::string Save0="", Save1="", Save2="", Save2_aux1="", Save2_aux2="";
      int JsonRunNo = -1;
      bool end_of_file = false;
      std::vector<bool> was_file_already_opened(jsonOutSize); //initialize all elements to false
      char delimiter = '"';

      for (int i=0; i<7; ++i) {
	end_of_file = false;
	Save0 = "";
	Save1 = "";
	Save2 = "";
	Save2_aux1 = "";
	Save2_aux2 = "";
	while(!jsonIn[i].eof()) 
	  {
	    //goes until the first '"' and stores what lies before the first '"'
	    std::getline(jsonIn[i], Save0, delimiter);
	    //goes until the second '"' and stores what in between the two '"'
	    if (Save0.find("}") != std::string::npos) {
	      break;
	    }
	    else {
	      if(Save0 == ", ") Save0 = "";
	      std::getline(jsonIn[i], Save1, delimiter); 
	      JsonRunNo = std::stoi(Save1);
	    }
	    //std::cout << "Number: " << JsonRunNo << std::endl;
	    bool run_check = false;
	    for (int j=0; j<5; ++j) {
              if (JsonRunNo > RunLimits[j] && JsonRunNo < RunLimits[j+1]) {
		run_check = true;
	      }
	    }
	    if(run_check==false) std::cout << "The run number " << JsonRunNo << "is out of the range!" << std::endl;
	    for (int j=0; j<5; ++j) {
	      if (JsonRunNo > RunLimits[j] && JsonRunNo < RunLimits[j+1] && !end_of_file) { 
		if(Save0 != "{") {
		  jsonOut[j] << Save0; //to avoid more than one '{' to appear
		}
		else {
		  if(was_file_already_opened[j] == true) jsonOut[j] << ""; //are we at the beginning of the file?
		}
		was_file_already_opened[j] = true; 
		jsonOut[j] << "\"";
		jsonOut[j] << Save1;
		jsonOut[j] << "\"";
		std::getline(jsonIn[i], Save2, ']');
		Save2 += "]";
		Save2_aux2 = "]";
		bool new_while = false;
		while(!jsonIn[i].eof() ) {
		  Save2_aux1 = jsonIn[i].get();
		  //check in case the first ']' is found by Save2_aux2
		  if(new_while == true && Save2_aux1 == "]" && Save2_aux2 == "]") {
		    Save2 += "]";
		    break;
		  }
		  //'new_while' checks for first loop
		  if(new_while == false && Save2_aux1 == "]") { 
		    Save2 += "]";
		    break;
		  }
		  else {
		    new_while = true;
		    Save2_aux2 = jsonIn[i].get();
		    Save2 += Save2_aux1 + Save2_aux2;
		    if(Save2_aux1 == "]" && Save2_aux2 == "]") break;
		  }
		}
		jsonOut[j] << Save2  + ", ";
	      }
	    }
	  }
      }


      for (int i=0; i<7; ++i) {
	jsonIn[i].close();
      }
      for (int i=0; i<jsonOutSize; ++i) {
	jsonOut[i] << "}";
	jsonOut[i].close();
      }
    }
  /////////////////////////////////////
  /////////////////////////////////////
  /////////////////////////////////////

  return 0;
}
