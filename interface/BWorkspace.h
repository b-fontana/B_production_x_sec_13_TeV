#ifndef BWORKSPACE_H
#define BWORKSPACE_H

#include <string>
#include <memory>
#include <RooWorkspace.h>
#include <RooDataSet.h>

class var_info {
 public:
  std::string name;
  double min;
  double max;
  var_info(std::string name_, int min_, int max_): name(name_), min(min_), max(max_) {}; //ctor
};

class new_var_info {
 public:
  std::string name;
  std::string formula;
  new_var_info(std::string name_, std::string formula_): name(name_), formula(formula_) {}; //ctor
};

class splot_info {
 public:
  std::string name;
  std::string title;
  std::string data_name;
  std::string model_name;
  std::vector<std::string> yields;
  std::vector<std::string> fix_vars;
  std::string new_data_name;
  splot_info(std::string name_, std::string title_, std::string data_name_, std::string model_name_, 
	     std::vector<std::string> yields_, std::vector<std::string> fix_vars_, std::string new_data_name_): 
             name(name_), title(title_), data_name(data_name_), model_name(model_name_), 
	       yields(yields_), fix_vars(fix_vars_), new_data_name(new_data_name_) {}; //ctor
};

class BWorkspace {
 private:
  std::shared_ptr<RooWorkspace> ws;

 public:
  BWorkspace(std::string); //ctor
  BWorkspace(BWorkspace& obj); //copy ctor
  ~BWorkspace(); //dtor

  void setVars(std::vector<var_info>);
  void factory(std::string);
  void generateData(std::string, std::string, std::vector<std::string>, int);
  void importData(std::string file_name, std::string tree_name, std::string data_name, 
		  std::vector<std::string> vars, std::vector<new_var_info> new_vars);
  void splot(splot_info*);
  RooPlot* FitAndPlot(std::string,std::string,std::string);
  RooRealVar* var(std::string);
  RooAbsPdf* pdf(std::string);
  RooDataSet* data(std::string);
  std::shared_ptr<RooWorkspace> returnWorkspace();
};

#endif