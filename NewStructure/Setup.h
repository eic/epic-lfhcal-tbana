#ifndef SETUP_H
#define SETUP_H

#include <cstddef>
#include <map>
#include "TString.h"

class RootSetupWrapper;

namespace DetConf{
  enum Type { Undef, 
              Single8M, 
              Single4M, 
              Single2MH, 
              Single2MV, 
              SingleTile, 
              Dual8M, 
              LargeTB
            };
}


class Setup{
  
 public:
  // deleting copy constructor. 
  Setup(const Setup &)=delete;
  Setup& operator=(const Setup&)=delete;
  
  static Setup *GetInstance(){
    if(instancePtr == NULL) instancePtr = new Setup();
    return instancePtr;
  }
  
  TString GetAssemblyID(int /**/) const;
  TString GetAssemblyID(int /**/, int /**/, int /**/, int /**/) const;
  int     GetCellID    (int /**/, int /**/) const;
  int     GetCellID    (int /**/, int /**/, int /**/, int /**/)const;
  int     GetColumn    (int /**/) const;
  int     GetChannelInLayer(int /**/) const;
  int     GetChannelInLayerFull(int /**/) const;
  int     GetMaxChannelInLayerFull(void) const;
  int     GetNActiveLayers(void) const;
  int     GetNActiveCells(void) const;
  int     GetLayer     (int /**/) const;
  int     GetModule    (int /**/) const;
  double  GetModuleX   (int /**/) const;
  double  GetModuleY   (int /**/) const;
  int     GetROchannel (int /**/) const;
  int     GetROchannel (int /**/, int /**/, int /**/, int /**/) const;
  int     GetROunit    (int /**/) const;
  int     GetROunit    (int /**/, int /**/, int /**/, int /**/) const;
  int     GetRow       (int /**/) const;
  int     GetTotalNbChannels(void) const;
  int     GetNMaxLayer  (void) const;
  int     GetNMaxRow    (void) const;
  int     GetNMaxColumn (void) const;
  int     GetNMaxModule (void) const;
  int     GetNMaxROUnit (void) const;
  int     GetNMaxKCUs   (void) const;
  int     GetMaxCellID (void) const;
  TString DecodeCellID(int /**/) const;
  double  GetX         (int /**/) const;
  double  GetY         (int /**/) const;
  double  GetZ         (int /**/) const;
  bool    Initialize   (TString, int);
  bool    Initialize   (RootSetupWrapper&);
  bool    IsInit       (void) const;
  bool    IsLayerOn     (int /**/, int /**/) const; 
  float     GetMinX       (void) const;
  float     GetMaxX       (void) const;
  float     GetMinY       (void) const;
  float     GetMaxY       (void) const;
  float     GetMinZ       (void) const;
  float     GetMaxZ       (void) const;
  float     GetCellWidth  (void) const;
  float     GetCellHeight (void) const;
  float     GetCellDepth  (void) const;
  DetConf::Type GetDetectorConfig(void) const;
  friend class RootSetupWrapper;
  
 private:
  static Setup* instancePtr;
  Setup(){}
  ~Setup() {}
  
  
  bool isInit=false;
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
  float cellW=5.;/*cm, width*/
  float cellH=5.;/*cm, height*/
  float cellD=2.;/*cm, depth*/
  ClassDef(Setup,2)
};


#endif
