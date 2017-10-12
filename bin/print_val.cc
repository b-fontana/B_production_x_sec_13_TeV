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

//input example: print_val --file input_file.root
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
  
  /////////////////////////////////////////////////////////////////
  /////////////////////////////read file///////////////////////////
  /////////////////////////////////////////////////////////////////

  //debug
  std::cout << "read :" << input_file << std::endl;

  double out_val;
  double out_err_lo;
  double out_err_hi;

  //open input file
  TFile* fin = new TFile(input_file);
  
  fin->ls();

  TVectorD *in_val = (TVectorD*)fin->Get("val");
  TVectorD *in_err_lo = (TVectorD*)fin->Get("err_lo");
  TVectorD *in_err_hi = (TVectorD*)fin->Get("err_hi");
  
  out_val = in_val[0][0];
  out_err_lo = in_err_lo[0][0];
  out_err_hi = in_err_hi[0][0];
      
  //print the val and the err
  std::cout << "val : " << out_val << " err_lo : " << out_err_lo << " err_hi : " << out_err_hi << std::endl;
  
  delete fin;
}//end
