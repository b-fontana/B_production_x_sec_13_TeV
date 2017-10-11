#include "UserCode/B_production_x_sec_13_TeV/interface/BChannel.h"
#include <string>

std::string BChannel::channel_to_ntuple_name(int channel)
{
  std::string str = "";
  switch(channel){
  default:
  case 1:
    str="ntkp";
    break;
  case 2:
    str="ntkstar";
    break;
  case 3:
   str="ntks";
   break;
  case 4:
    str="ntphi";
    break;
  case 5:
   str="ntmix";
   break;
  case 6:
    str="ntlambda";
    break;
 }
  return str;
}