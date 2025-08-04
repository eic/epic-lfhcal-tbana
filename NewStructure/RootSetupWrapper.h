#ifndef ROOTSETUPWRAPPER_H
#define ROOTSETUPWRAPPER_H

#include <cstddef>
#include <map>
#include "TString.h"

class Setup;

class RootSetupWrapper{
  
 public:
  RootSetupWrapper(){};
  RootSetupWrapper(Setup* s);
  virtual ~RootSetupWrapper() {}
  
 private:
  
  bool isInit;
  //key is CellID
  std::map<int, TString> assemblyID;
  std::map<int, int>     ROunit;
  std::map<int, int>     ROchannel;
  std::map<int, int>     Board;
  //key is module number
  std::map<int, std::pair<float,float>> ModPos;
  //Inverse mapping
  std::map< std::pair<int, int>, int> CellIDfromRO;
  int nMaxLayer;
  int nMaxRow;
  int nMaxColumn;
  int nMaxModule;
  int nMaxROUnit;
  int maxCellID;
  
  friend class Setup;
  ClassDef(RootSetupWrapper,1)
};


#endif
