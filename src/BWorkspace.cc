#include "UserCode/B_production_x_sec_13_TeV/interface/BWorkspace.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/BWarnings.h"
#include "UserCode/B_production_x_sec_13_TeV/interface/BDefinitions.h"
#include <string>

BWorkspace::BWorkspace(std::string s = "WS")
{
  std::cout << "BWorkspace Constructor activated." << std::endl; 
  ws = std::make_shared<RooWorkspace>(s.c_str());
}

BWorkspace::BWorkspace(BWorkspace& obj) 
{
  std::cout << "Copy Ctor activated" << std::endl;
  ws = obj.ws;
}

BWorkspace::~BWorkspace()
{
  std::cout << "BWorkspace Destructor activated." << std::endl; 
  if(ws==nullptr) std::cout << "The BWorkspace pointer is NULL!" << std::endl;
}

void BWorkspace::setVars(std::vector<var_info> info)
{
  int size = info.size();
  std::vector<RooRealVar*> var_vector(size, nullptr);
  for (int i=0; i<size; ++i) 
    {
      var_vector.at(i) = new RooRealVar( (info.at(i).name).c_str(), (info.at(i).name).c_str(), info.at(i).min, info.at(i).max );
      ws->import(*var_vector.at(i));
    }
  for (int i=0; i<size; ++i) delete var_vector.at(i);
}

void BWorkspace::factory(std::string s)
{
  ws->factory(s.c_str());
}

void BWorkspace::generateData(std::string model_name, std::string dataset_name, std::vector<std::string> var_names, int nevents)
{
  int nvars = var_names.size();
  RooArgSet argset;
  RooAbsPdf* model = ws->pdf(model_name.c_str());
  for (int i=0; i<nvars; ++i) {
    argset.add( *(ws->var(var_names.at(i).c_str())) );
  }
  RooDataSet* dataset = model->generate(argset,nevents);

  TCanvas* teste = new TCanvas("TESTE","TESTE", 400, 600);
  teste->cd();
  RooPlot* teste_frame = ws->var("time")->frame() ; 
  dataset->plotOn(teste_frame) ; 
  teste_frame->SetTitle("proper time: total sample");
  teste_frame->Draw() ;
  teste->SaveAs("TESTE.png");


  ws->import(*dataset, Rename(dataset_name.c_str()) ); 
}

RooRealVar* BWorkspace::var(std::string s)
{
  return ws->var(s.c_str());
}
	
RooAbsPdf* BWorkspace::pdf(std::string s)
{
  return ws->pdf(s.c_str());
}

RooDataSet* BWorkspace::data(std::string s)
{
  return (RooDataSet*)ws->data(s.c_str());
}

void BWorkspace::splot(splot_info* sp)
{
  int size1 = sp->fix_vars.size();
  //sPlot technique requires model parameters (other than the yields) to be fixed
  for (int i=0; i<size1; ++i) {
    (ws->var( (sp->fix_vars.at(i)).c_str() )) -> setConstant();
  }
  int size2 = sp->yields.size();
  RooArgList yields_list;
  for (int i=0; i<size2; ++i) {
    yields_list.add( *(ws->var((sp->yields.at(i)).c_str())) );
  }
  RooDataSet* data = (RooDataSet*)ws->data(sp->data_name.c_str());
  RooMsgService::instance().setSilentMode(false);
  RooStats::SPlot(sp->name.c_str(), sp->title.c_str(), *data, ws->pdf(sp->model_name.c_str()), yields_list);
  ws->import(*data, Rename(sp->new_data_name.c_str()));
}

RooPlot* BWorkspace::FitAndPlot(std::string fit_var_name, std::string pdf_name, std::string data_name)
{
  RooPlot *frame = ws->var(fit_var_name.c_str())->frame( Title((fit_var_name + " Frame").c_str()) );
  ws->data(data_name.c_str())->plotOn(frame);
  ws->pdf(pdf_name.c_str())->fitTo(*(ws->data(data_name.c_str())));
  ws->pdf(pdf_name.c_str())->paramOn(frame);
  ws->pdf(pdf_name.c_str())->plotOn(frame);
  return frame;
}

void BWorkspace::importData(std::string file_name, std::string tree_name, std::string data_name, std::vector<std::string> vars, std::vector<new_var_info> new_vars)
{
  BWarnings warn;
  TFile* file_bworkspace = new TFile(file_name.c_str(), "READ");
  warn.CheckPointer(file_bworkspace);
  TTree* tree_bworkspace = (TTree*)file_bworkspace->Get(tree_name.c_str());
  warn.CheckPointer(tree_bworkspace);
  RooArgSet argset;
  int size1 = vars.size();
  int size2 = new_vars.size();
  for(int i=0; i<size1; ++i) {
    argset.add( *(ws->var(vars.at(i).c_str())) );
  }
  RooDataSet* data = new RooDataSet(data_name.c_str(),data_name.c_str(),tree_bworkspace,argset);
  std::vector<RooFormulaVar> formulas;
  for (int i=0; i<size2; ++i) {
    RooFormulaVar zzzz(new_vars.at(i).name.c_str(),new_vars.at(i).name.c_str(),new_vars.at(i).formula.c_str(),argset);
    formulas.push_back(zzzz);
  }
  for(int i=0; i<size2; ++i) {data->addColumn(formulas.at(i));}
  ws->import(*data);
}

std::shared_ptr<RooWorkspace> BWorkspace::returnWorkspace() 
{
  return ws;
}
