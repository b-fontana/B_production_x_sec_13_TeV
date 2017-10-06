#ifndef BSELECTION_H
#define BSELECTION_H

#include<string>

struct getCuts_struct {
  double lower_cut;
  double upper_cut;
};

class BSelection {
 public:
  getCuts_struct getCuts(std::string);
};

#endif
