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

#endif