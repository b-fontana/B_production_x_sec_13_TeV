#ifndef BWARNING_H
#define BWARNING_H

#include <iostream>

class BWarnings {
 public:
  void RunningError(std::string); 
  void TreeManagementError(std::string);
  template <typename T> 
    void CheckPointer(T*);
};

template <typename T> 
void BWarnings::CheckPointer(T* ptr) {
  if (ptr == NULL) std::cout << "The used pointer has 'NULL' value [BWarnings::CheckPointer()]." << std::endl;
}

/////////
//methods
/////////
void BWarnings::RunningError(std::string s) {
  if (s == "NUMBER_ERROR") 
    {
      std::cout << "The code was not properly run. The number of expected arguments does not match the number of given arguments. [BWarnings::RunningError()]" << std::endl;
    }
  else if (s == "NAME_ERROR")
    {
      std::cout << "The code was not properly run. The name of one of the given input arguments has a spelling error or is not contemplated as a valid option. [BWarnings::RunningError()]" << std::endl;
    }
  else if (s == "CHANNEL_ERROR")
    {
      std::cout << "The required channel is not currently ready to be used. [BWarnings::RunningError()]" << std::endl;
    }
  else std::cout << "None of the valid options was activated. [BWarnings::RunningError()]" << std::endl;
}

void BWarnings::TreeManagementError(std::string s) {
  if (s == "SET_ENTRIES")
    {
      std::cout << "There was a problem when you tried to set the number of entries of the tree. [BWarnings::TreeManagementError()]" << std::endl;
    }
}

#endif
