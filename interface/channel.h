#include <TString.h>

TString channel_to_ntuple_name(int channel)
{
  //returns a TString with the ntuple name corresponding to the channel. It can be used to find the data on each channel saved in a file. or to write the name of a directory                                                                                                                               
  TString ntuple_name = "";
  
  switch(channel){
  default:
  case 1:
    ntuple_name="ntkp";
    break;
  case 2:
    ntuple_name="ntkstar";
    break;
  case 3:
   ntuple_name="ntks";
   break;
  case 4:
    ntuple_name="ntphi";
    break;
  case 5:
   ntuple_name="ntmix";
   break;
  case 6:
    ntuple_name="ntlambda";
    break;
 }
  return ntuple_name;
}

TString channel_to_xaxis_title(int channel)
{
  TString xaxis_title = "";
  
  switch (channel) {
  default:
  case 1:
    xaxis_title = "M_{J/#psi K^{#pm}} [GeV]";
    break;
  case 2:
    xaxis_title = "M_{J/#psi K^{#pm}#pi^{#mp}} [GeV]";
    break;
  case 3:
    xaxis_title = "M_{J/#psi K^{0}_{S}} [GeV]";
    break;
  case 4:
    xaxis_title = "M_{J/#psi K^{#pm}K^{#mp}} [GeV]";
    break;
  case 5:
    xaxis_title = "M_{J/#psi #pi^{#pm}#pi^{#mp}} [GeV]";
    break;
  case 6:
    xaxis_title = "M_{J/#psi #Lambda} [GeV]";
    break;
  }
  return xaxis_title;
}

int channel_to_nbins(int channel)
{
  int nbins;

  switch (channel) {
  default:
  case 1:
    nbins = 50;
    break;
  case 2:
    nbins = 50;
    break;
  case 3:
    nbins = 50;
    break;
  case 4:
    nbins = 50;
    break;
  case 5:
    nbins = 80;
    break;
  case 6:
    nbins = 50;
    break;
  }
  return nbins;
}

bool LowStat(int channel, double ptmin, double ymin) 
{
  bool b = false;
  switch(channel) {
    case 1: if(ptmin>85) b = true; break;
    case 2: if(ptmin>65 || ymin>1.7) b = true; break;
    case 4: if(ptmin>55 || ymin>1.7) b = true; break;}
  return b;
}

std::tuple<double,double,double> get_fit_params(int channel, std::pair<double,double> pt_pair, 
						std::pair<double,double> y_pair, std::string s)
{
  std::cout << "==============ENTERED THE FUNCTION!====================" << s << std::endl;
  std::cout << y_pair.first << "-" << y_pair.second << "-" << pt_pair.first << "-" << pt_pair.second << std::endl;
  std::tuple<double,double,double> tuple;
  switch(channel) {
  case 1: 
    if(s=="m_exp") tuple = std::make_tuple(-0.50,-100.,0.); 
    else if(s=="m_sig2scale") tuple = std::make_tuple(0.41,0.00,8.00);
    else if(s=="m_sig2scale_3gauss") {
      if(y_pair.first == 0.00 && y_pair.second == 2.25) 
	{
	  if(pt_pair.first == 42 && pt_pair.second == 50)
	    {
	      tuple = std::make_tuple(0.80,0.00,4.00);
	      std::cout << "" << std::endl;
	      std::cout << "PRINT!" << std::endl;
	      std::cout << "" << std::endl;
	    }
	  else
	    tuple = std::make_tuple(0.41,0.00,8.00);
	}
      else tuple = std::make_tuple(0.41,0.00,8.00);
    }
    else tuple = std::make_tuple(0.,0.,0.); 
    break;
    ///////////////////////////////////////////////////////////////////////////
  case 2: 
    if(s=="m_exp") 
      {
	tuple = std::make_tuple(-2.91,-5.00,0.00); 
      }
    else if(s=="m_sig2scale") 
      {
	if(pt_pair.first == 10 && pt_pair.second == 90)
	  {
	    if(y_pair.first == 1.75 && y_pair.second == 2.00)
	      tuple = std::make_tuple(0.201,0.000,8.000);
	    else 
	      tuple = std::make_tuple(0.000,0.000,10.000);
	  }
	else if(y_pair.first == 0.00 && y_pair.second == 2.25)
	  {
	    if(pt_pair.first == 10 && pt_pair.second == 16)
	      tuple = std::make_tuple(0.201,0.000,8.000);
	    else
	      tuple = std::make_tuple(0.000,0.000,10.000);
	  }
      }
    else if(s=="m_sig2scale_3gauss") 
      {
	if(y_pair.first == 0.00 && y_pair.second == 2.25)
	  {
	    if(pt_pair.first == 10 && pt_pair.second == 16) {
	      std::cout << std::endl;
	      std::cout << std::endl;
	      std::cout << "TEST!!!!!" << std::endl;
	      std::cout << std::endl;
	      std::cout << std::endl;
	      tuple = std::make_tuple(0.501,0.150,3.00);
	    }
	    else
	      tuple = std::make_tuple(0.300,0.150,3.00);
	  }
	tuple = std::make_tuple(0.200,0.010,2.000);
      }
    else tuple = std::make_tuple(0.,0.,0.); 
    break;
    ///////////////////////////////////////////////////////////////////////////
  case 4: 
    if(s=="m_exp") tuple = std::make_tuple(-0.45,-10.,0.); 
    else if(s=="m_sig2scale") tuple = std::make_tuple(1.41,1.00,8.00);
    else if(s=="m_sig2scale_3gauss") tuple = std::make_tuple(1.41,1.00,8.00);
    else tuple = std::make_tuple(0.,0.,0.); 
    break;
  }
  return tuple;
}
