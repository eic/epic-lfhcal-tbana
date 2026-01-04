#include "Setup.h"
#include "RootSetupWrapper.h"
#include <iostream>
#include <fstream>
#include "TObjArray.h"
#include "TObjString.h"
#include "utility"

Setup* Setup::instancePtr = nullptr;

ClassImp(Setup);

bool Setup::Initialize(TString file, int debug){

  std::cout << "entered setup initialize" << std::endl;
  if(isInit){
    std::cout<<"Already initialized, bailing out without action"<<std::endl;
    return true;
  }
  std::fstream input;
  input.open(file.Data(),std::ios::in);
  if(!input.is_open()){
    std::cout<<"Could not open "<<file<<std::endl;
    std::cout<<"Leaving uninitialized"<<std::endl;
    return false;
  }
  nMaxLayer   = -1;
  nMaxRow     = -1;
  nMaxColumn  = -1;
  nMaxModule  = -1;
  nMaxROUnit  = -1;
  maxCellID   = -1;
  int AROunit,AROchannel,Alayer,AROlayer,Arow,Acolumn,Amod;
  TString Anassembly;
  int Akey;
  for( TString tempLine; tempLine.ReadLine(input, kTRUE); ) {
        // check if line should be considered
    if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
        continue;
    }
    TObjArray *tempArr  = tempLine.Tokenize("\t");
    if(tempArr->GetEntries()<1){
        if (debug > 1) std::cout << "nothing to be done" << std::endl;
        delete tempArr;
        continue;
    } else if (tempArr->GetEntries() == 1 ){
        // Separate the string according to space
        tempArr       = tempLine.Tokenize(" ");
        if(tempArr->GetEntries()<1){
            if (debug > 1) std::cout << "nothing to be done" << std::endl;
            delete tempArr;
            continue;
        } else if (tempArr->GetEntries() == 1  ) {
            if (debug > 1) std::cout << ((TString)((TObjString*)tempArr->At(0))->GetString()).Data() << " no correct format detected" << std::endl;
            delete tempArr;
            continue;
        }
    }
    if (tempArr->GetEntries()<10){
      if (debug > 1) std::cout << "line not conform with mapping format, skipping" << std::endl;
      delete tempArr;
      continue;
    }
    
    AROunit     = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
    AROchannel  = ((TString)((TObjString*)tempArr->At(1))->GetString()).Atoi();
    Alayer      = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atoi();
    Anassembly  = ((TString)((TObjString*)tempArr->At(3))->GetString());
    AROlayer    = ((TString)((TObjString*)tempArr->At(4))->GetString()).Atoi();
    Arow        = ((TString)((TObjString*)tempArr->At(5))->GetString()).Atoi();
    Acolumn     = ((TString)((TObjString*)tempArr->At(6))->GetString()).Atoi();
    Amod        = ((TString)((TObjString*)tempArr->At(7))->GetString()).Atoi();
    float AmodX       = ((TString)((TObjString*)tempArr->At(8))->GetString()).Atof();
    float AmodY       = ((TString)((TObjString*)tempArr->At(9))->GetString()).Atof(); 
    

    // Try to set map for mod pos
    ModPos[Amod]=std::make_pair(AmodX,AmodY);
    //std::cerr<< "modnr: "<< Amod <<std::endl;

    Akey=(Amod<<9)+(Arow<<8)+(Acolumn<<6)+(Alayer);
    assemblyID[Akey] = Anassembly;
    ROunit    [Akey] = AROunit;
    ROchannel [Akey] = AROchannel;
    Board     [Akey] = AROlayer;
    CellIDfromRO[std::make_pair(AROunit,AROchannel)]=Akey;
    if (nMaxLayer < Alayer)   nMaxLayer   = Alayer;
    if (nMaxRow < Arow)       nMaxRow     = Arow;
    if (nMaxColumn < Acolumn) nMaxColumn  = Acolumn;
    if (nMaxModule < Amod)    nMaxModule  = Amod;
    if (nMaxROUnit < AROunit) nMaxROUnit  = AROunit;
    if (maxCellID < Akey)     maxCellID   = Akey;
    if (debug > 10)std::cout <<AROunit<< "\t" << AROchannel << "\t"<< Alayer << "\t"<< Anassembly << "\t"<< AROlayer << "\t"<< Arow << "\t"<< Acolumn << "\t"<< Amod << std::endl;
    if (debug > 1)std::cout << "registered cell: " << DecodeCellID(Akey).Data() << std::endl;
  }
  input.close();
  isInit=true;
  return isInit;
}

bool Setup::Initialize(RootSetupWrapper& rsw){
  std::cout<<rsw.isInit<<"\t"<<rsw.assemblyID.size()<<std::endl;
  isInit          =rsw.isInit;
  assemblyID      =rsw.assemblyID;
  ROunit          =rsw.ROunit;
  ROchannel       =rsw.ROchannel;
  Board           =rsw.Board;
  CellIDfromRO    =rsw.CellIDfromRO;
  nMaxLayer       =rsw.nMaxLayer;
  nMaxRow         =rsw.nMaxRow;
  nMaxColumn      =rsw.nMaxColumn;
  nMaxModule      =rsw.nMaxModule;
  nMaxROUnit      =rsw.nMaxROUnit;
  maxCellID       =rsw.maxCellID;
  ModPos          =rsw.ModPos;
  return isInit;
}

bool Setup::IsInit() const{
  return isInit;
}

TString Setup::GetAssemblyID(int cellID) const{
  std::map<int, TString>::const_iterator it=assemblyID.find(cellID);
  if(it!=assemblyID.end()) return it->second;
  else return "";
}

TString Setup::GetAssemblyID(int row, int col, int lay, int mod=0) const{
  return GetAssemblyID((mod<<9)+(row<<8)+(col<<6)+lay);
}

int Setup::GetCellID(int roboard, int roch) const{
  std::map<std::pair<int,int>,int>::const_iterator it=CellIDfromRO.find(std::make_pair(roboard,roch));
  if(it!=CellIDfromRO.cend()) return it->second;
  else return -1;
}

int Setup::GetCellID(int row, int col, int lay, int mod=0)const {
  return (mod<<9)+(row<<8)+(col<<6)+lay;
}

int Setup::GetColumn(int cellID) const{
  return (cellID&(3<<6))>>6;
}

int Setup::GetLayer(int cellID) const{
  return (cellID&((1<<6)-1));
}

int Setup::GetModule(int cellID) const{
  return cellID>>9;
}

int Setup::GetROchannel(int cellID) const{
  std::map<int, int>::const_iterator it=ROchannel.find(cellID);
  if(it!=ROchannel.end()) return it->second;
  else return -999;
}

int Setup::GetROchannel(int row, int col, int lay, int mod=0) const{
  return GetROchannel((mod<<9)+(row<<8)+(col<<6)+lay);
}

int Setup::GetROunit(int cellID) const{
  std::map<int,int>::const_iterator it=ROunit.find(cellID);
  if(it!=ROunit.end()) return it->second;
  else return -999;
}

int Setup::GetROunit(int row, int col, int lay, int mod=0) const{
  return GetROunit((mod<<9)+(row<<8)+(col<<6)+lay);
}

int Setup::GetRow(int cellID) const{
  return (cellID&(1<<8))>>8;
}

double Setup::GetModuleX(int mod)const {
  auto it = ModPos.find(mod);
  if (it != ModPos.end()) {
//      std::cerr<< "My value: " << it->second.first << std::endl;
      return static_cast<double>(it->second.first);
  }
  else {
    std::cerr << "Warning: Module " << mod << " not found in ModPos\n";
    return -999.0;
  }
}

double Setup::GetModuleY(int mod)const {
    auto it = ModPos.find(mod);
  if (it != ModPos.end()) {
      return static_cast<double>(it->second.second);
  }
  else {
    std::cerr << "Warning: Module " << mod << " not found in ModPos\n";
    return -999.0;
  }
}

int Setup::GetTotalNbChannels(void) const {
  return (int) ROunit.size();
}

double Setup::GetX(int cellID) const{
  int col=GetColumn(cellID);
  return -7.5/*cm*/+col*cellW/*cm*/ + GetModuleX(GetModule(cellID));
}

double Setup::GetY(int cellID) const{
  int row=GetRow(cellID);
  return -2.5/*cm*/+row*cellH/*cm*/ + GetModuleY(GetModule(cellID));
}

double Setup::GetZ(int cellID) const{
  int lay=GetLayer(cellID);
  return cellD/2 + lay*cellD/*cm*/;
}

TString Setup::DecodeCellID(int cellID) const{
  TString out = Form("cell ID: %d ==> RO unit %d RO channel %d  module %d  layer %d  column %d row %d", cellID, GetROunit(cellID), GetROchannel(cellID), GetModule(cellID), GetLayer(cellID), GetColumn(cellID), GetRow(cellID));
  return out;
}


int Setup::GetNMaxLayer() const{
  return nMaxLayer;
}

int Setup::GetNMaxRow() const{
  return nMaxRow;
}

int Setup::GetNMaxColumn() const{
  return nMaxColumn;
}

int Setup::GetNMaxModule() const{
  return nMaxModule;
}

int Setup::GetNMaxROUnit() const{
  return nMaxROUnit;
}

int Setup::GetNMaxKCUs() const{
  return (int)(nMaxROUnit/2+1);
}

int Setup::GetMaxCellID() const{
  return maxCellID;
}

int Setup::GetChannelInLayer(int cellID) const{
  int row     = GetRow(cellID);
  int column  = GetColumn(cellID);
  int absChL  = row*(nMaxColumn+1)+column;
  return absChL;
}

float Setup::GetMinX() const{
  float min = 1e6;
  std::map<int, TString>::const_iterator it;
  for(it=assemblyID.begin(); it!=assemblyID.end(); ++it){
    int cellID = it->first;
    if( GetX(cellID) < min ) min = GetX(cellID);
  }
  return min-(cellW/2);/*to get to the center of the tile*/
}

float Setup::GetMaxX() const{
  float max = -1e6;
  std::map<int, TString>::const_iterator it;
  for(it=assemblyID.begin(); it!=assemblyID.end(); ++it){
    int cellID = it->first;
    if( GetX(cellID) > max ) max = GetX(cellID);
  }
  return max+(cellW/2);/*to get to the center of the tile*/
}

float Setup::GetMinY() const{
  float min = 1e6;
  std::map<int, TString>::const_iterator it;
  for(it=assemblyID.begin(); it!=assemblyID.end(); ++it){
    int cellID = it->first;
    if( GetY(cellID) < min ) min = GetY(cellID);
  }
  return min-(cellH/2);/*to get to the center of the tile*/
}

float Setup::GetMaxY() const{
  float max = -1e6;
  std::map<int, TString>::const_iterator it;
  for(it=assemblyID.begin(); it!=assemblyID.end(); ++it){
    int cellID = it->first;
    if( GetY(cellID) > max ) max = GetY(cellID);
  }
  return max+(cellH/2);/*to get to the center of the tile*/
}

float Setup::GetMinZ() const{
  float min = 1e6;
  std::map<int, TString>::const_iterator it;
  for(it=assemblyID.begin(); it!=assemblyID.end(); ++it){
    int cellID = it->first;
    if( GetZ(cellID) < min ) min = GetZ(cellID);
  }
  return min-(cellD/2);/*to get to the center of the tile*/
}

float Setup::GetMaxZ() const{
  float max = -1e6;
  std::map<int, TString>::const_iterator it;
  for(it=assemblyID.begin(); it!=assemblyID.end(); ++it){
    int cellID = it->first;
    if( GetZ(cellID) > max ) max = GetZ(cellID);
  }
  return max+(cellD/2);/*to get to the center of the tile*/
}

bool Setup::IsLayerOn(int layer, int mod) const{
  bool isOn = false;
  for (int r = 0; r< GetNMaxRow(); r++){
      for (int c = 0; c < GetNMaxColumn(); c++){
          int cellID = GetCellID(r, c, layer, mod);
          std::map<int, TString>::const_iterator it=assemblyID.find(cellID);
          if (it != assemblyID.end()){
            isOn = true;
            break;
          }
      }
  }
  return isOn;
}


float Setup::GetCellWidth() const{
  return cellW;
}

float Setup::GetCellHeight() const{
  return cellH;
}

float Setup::GetCellDepth() const{
  return cellD;
}
