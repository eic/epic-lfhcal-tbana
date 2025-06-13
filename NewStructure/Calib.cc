#include "Calib.h"
#include <iostream>
#include <fstream>
#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include <utility>

ClassImp(Calib);

//*****************************************************************************************
// CALIBRATION Getters by cell ID
//*****************************************************************************************
double Calib::GetPedestalMeanH(int cellID) const{
  //std::map<int, double>::const_iterator it= PedestalMeanH.find(cellID);
  //if(it!=PedestalMeanH.end()) return it->second;
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.PedestalMeanH;
  else return 9999;
}

double Calib::GetPedestalMeanL(int cellID) const{
  //std::map<int, double>::const_iterator it= PedestalMeanL.find(cellID);
  //if(it!=PedestalMeanL.end()) return it->second;
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.PedestalMeanL;
  else return 9999;
}

double Calib::GetPedestalSigH(int cellID) const{
  //std::map<int, double>::const_iterator it= PedestalSigH.find(cellID);
  //if(it!=PedestalSigH.end()) return it->second;
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.PedestalSigH;
  else return 9999;
}
double Calib::GetPedestalSigL(int cellID) const{
  //std::map<int, double>::const_iterator it= PedestalSigL.find(cellID);
  //if(it!=PedestalSigL.end()) return it->second;
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.PedestalSigL;
  else return 9999;
}

double Calib::GetScaleHigh(int cellID)const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    if (it->second.ScaleH != -1000  && (BCcalc && it->second.BadChannel > 1)){
      return it->second.ScaleH;
    } else {
      return GetAverageScaleHigh();
    }
  } else return -1.;
}

double Calib::GetScaleWidthHigh(int cellID)const {
  //std::map<int, double>::const_iterator it=ScaleH.find(cellID);
  //if(it!=ScaleH.end()) return it->second;
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.ScaleWidthH;
  else return -1.;
}

double Calib::GetCalcScaleLow(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    if (it->second.ScaleH != -1000 && it->second.LGHGCorr != -64  && (BCcalc && it->second.BadChannel > 1)){
      return it->second.ScaleH/it->second.LGHGCorr;
    } else if ( it->second.LGHGCorr != -64  && (BCcalc && it->second.BadChannel > 1)){
      return GetAverageScaleHigh()/it->second.LGHGCorr;
    } else if (it->second.ScaleH != -1000 && (BCcalc && it->second.BadChannel > 1)){
      return it->second.ScaleH/GetAverageLGHGCorr();
    } else {
      return GetAverageScaleHigh()/GetAverageLGHGCorr();
    }
  }
  else return -1.;
}

double Calib::GetScaleLow(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.ScaleL;
  else return -1.;
}

double Calib::GetScaleWidthLow(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.ScaleWidthL;
  else return -1.;
}

double Calib::GetLGHGCorr(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.LGHGCorr;
  else return -1.;
}

double Calib::GetLGHGCorrOff(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.LGHGCorrOff;
  else return -1.;
}

double Calib::GetHGLGCorr(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.HGLGCorr;
  else return -1.;
}

double Calib::GetHGLGCorrOff(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.HGLGCorrOff;
  else return -1.;
}

short Calib::GetBadChannel(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.BadChannel;
  else return -1.;
}

//*****************************************************************************************
// CALIBRATION Getters by cell row, col, layer and module
//*****************************************************************************************
double Calib::GetPedestalMeanH(int row, int col, int lay, int mod=0) const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetPedestalMeanH(key);
}

double Calib::GetPedestalMeanL(int row, int col, int lay, int mod=0) const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetPedestalMeanL(key);
}

double Calib::GetPedestalSigH(int row, int col, int lay, int mod=0) const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetPedestalSigH(key);
}

double Calib::GetPedestalSigL(int row, int col, int lay, int mod=0) const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetPedestalSigL(key);
}

double Calib::GetScaleHigh(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col,lay,mod);
  return GetScaleHigh(key);
}

double Calib::GetScaleWidthHigh(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col,lay,mod);
  return GetScaleWidthHigh(key);
}

double Calib::GetScaleLow(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetScaleLow(key);
}

double Calib::GetCalcScaleLow(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetCalcScaleLow(key);
}

double Calib::GetScaleWidthLow(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetScaleWidthLow(key);
}

double Calib::GetLGHGCorr(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetLGHGCorr(key);
}

double Calib::GetLGHGCorrOff(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetLGHGCorrOff(key);
}

double Calib::GetHGLGCorr(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetHGLGCorr(key);
}

double Calib::GetHGLGCorrOff(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetHGLGCorrOff(key);
}

short Calib::GetBadChannel(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetBadChannel(key);
}

//*****************************************************************************************
// CALIBRATION Average calculators
//*****************************************************************************************
double Calib::GetAveragePedestalMeanHigh()const{
  double avSc   = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.PedestalMeanH == -1000)
      notCalib++;
    else 
      avSc += it->second.PedestalMeanH;
  }
  return avSc/(CaloCalib.size()-notCalib);
}

double Calib::GetAveragePedestalSigHigh()const{
  double avSc = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    avSc += it->second.PedestalSigH;
  }
  return avSc/CaloCalib.size();
}

double Calib::GetAveragePedestalMeanLow()const{
  double avSc   = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.PedestalMeanL == -1000)
      notCalib++;
    else
      avSc += it->second.PedestalMeanL;
  }
  return avSc/(CaloCalib.size()-notCalib);
}

double Calib::GetAveragePedestalSigLow()const{
  double avSc = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    avSc += it->second.PedestalSigL;
  }
  return avSc/CaloCalib.size();
}


double Calib::GetAverageScaleHigh( )const{
  double avSc   = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.ScaleH == -1000 || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.ScaleH;
    }
  }
  return avSc/(CaloCalib.size()-notCalib);
}


double Calib::GetAverageScaleHigh(int &active )const{
  double avSc   = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.ScaleH == -1000 || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.ScaleH;
    }
  }
  active=(CaloCalib.size()-notCalib);
  return avSc/(CaloCalib.size()-notCalib);
}

double Calib::GetAverageScaleWidthHigh()const{
  double avSc = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.ScaleWidthH == -1000 || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.ScaleWidthH;
    }
  }
  return avSc/(CaloCalib.size()-notCalib);
}

double Calib::GetAverageScaleLow()const{
  double avSc = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.ScaleL == -1000 || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.ScaleL;
    }
  }
  return avSc/(CaloCalib.size()-notCalib);
}
double Calib::GetAverageScaleWidthLow()const{
  double avSc = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.ScaleWidthL == -1000 || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.ScaleWidthL;
    }
  }
  return avSc/(CaloCalib.size()-notCalib);
}

double Calib::GetAverageLGHGCorr()const{
  double avSc = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.LGHGCorr == -64. || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.LGHGCorr;
    }
  }
  return avSc/(CaloCalib.size()-notCalib);
}

double Calib::GetAverageLGHGCorrOff()const{
  double avSc = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.LGHGCorrOff == -1000. || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.LGHGCorrOff;
    }
  }
  return avSc/(CaloCalib.size()-notCalib);
}

double Calib::GetAverageHGLGCorr()const{
  double avSc = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.HGLGCorr == -64. || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.HGLGCorr;
    }
  }
  return avSc/(CaloCalib.size()-notCalib);
}

double Calib::GetAverageHGLGCorrOff()const{
  double avSc = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.HGLGCorrOff == -1000. || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.HGLGCorrOff;
    }
  }
  return avSc/(CaloCalib.size()-notCalib);
}


int Calib::GetNumberOfChannelsWithBCflag( short bcflag )const{
  int nCh = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (bcflag == it->second.BadChannel)
      nCh++;
  }
  return nCh;
}


//*****************************************************************************************
// Getters for full calib objects
//*****************************************************************************************
TileCalib* Calib::GetTileCalib(int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    return &(it->second);
  }
  else {
    TileCalib acal;
    CaloCalib[cellID]=acal;
    return &(CaloCalib[cellID]);
  }
}

TileCalib* Calib::GetTileCalib(int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetTileCalib(key);
}

//*****************************************************************************************
// CALIBRATION Setters by cell ID
//*****************************************************************************************
void Calib::SetPedestalMeanH(double ped, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.PedestalMeanH=ped;
    CaloCalib[cellID]=acal;
  }
  else it->second.PedestalMeanH=ped;
}

void Calib::SetPedestalMeanL(double ped, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.PedestalMeanL=ped;
    CaloCalib[cellID]=acal;
  }
  else it->second.PedestalMeanL=ped;
}

void Calib::SetPedestalSigH(double ped, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.PedestalSigH=ped;
    CaloCalib[cellID]=acal;
  }
  else it->second.PedestalSigH=ped;
}

void Calib::SetPedestalSigL(double ped, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.PedestalSigL=ped;
    CaloCalib[cellID]=acal;
  }
  else it->second.PedestalSigL=ped;
}

void Calib::SetScaleHigh(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.ScaleH=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.ScaleH=s;
}

void Calib::SetScaleWidthHigh(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.ScaleWidthH=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.ScaleWidthH=s;
}

void Calib::SetScaleLow(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.ScaleL=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.ScaleL=s;
}

void Calib::SetScaleWidthLow(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.ScaleWidthL=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.ScaleWidthL=s;
}

void Calib::SetLGHGCorr(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.LGHGCorr=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.LGHGCorr=s;
}

void Calib::SetLGHGCorrOff(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.LGHGCorrOff=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.LGHGCorrOff=s;
}

void Calib::SetHGLGCorr(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.HGLGCorr=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.HGLGCorr=s;
}

void Calib::SetHGLGCorrOff(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.HGLGCorrOff=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.HGLGCorrOff=s;
}

void Calib::SetBadChannel(short s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.BadChannel=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.BadChannel=s;
}


//*****************************************************************************************
// CALIBRATION Setters by cell row, col, layer and module
//*****************************************************************************************
void Calib::SetPedestalMeanH(double ped, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetPedestalMeanH(ped,key);
}
void Calib::SetPedestalMeanL(double ped, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetPedestalMeanL(ped,key);
}
void Calib::SetPedestalSigH(double ped, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetPedestalSigH(ped,key);
}
void Calib::SetPedestalSigL(double ped, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetPedestalSigL(ped,key);
}

void Calib::SetScaleHigh(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetScaleHigh(s,key);
}

void Calib::SetScaleWidthHigh(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetScaleWidthHigh(s,key);
}

void Calib::SetScaleLow(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetScaleLow(s,key);
}

void Calib::SetScaleWidthLow(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetScaleWidthLow(s,key);
}

void Calib::SetLGHGCorr(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetLGHGCorr(s,key);
}

void Calib::SetLGHGCorrOff(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetLGHGCorrOff(s,key);
}

void Calib::SetHGLGCorr(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetHGLGCorr(s,key);
}

void Calib::SetHGLGCorrOff(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetHGLGCorrOff(s,key);
}

void Calib::SetBadChannel(short s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetBadChannel(s,key);
}


//*****************************************************************************************
// CALIBRATION Getters for global properties
//*****************************************************************************************
int Calib::GetRunNumber(void){
  return RunNumber;
}

int Calib::GetRunNumberPed(void){
  return RunNumberPed;
}

int Calib::GetRunNumberMip(void){
  return RunNumberMip;
}

const TTimeStamp* Calib::GetBeginRunTime(void) const{
  return &BeginRunTime;
}

const TTimeStamp* Calib::GetBeginRunTimePed(void) const{
  return &BeginRunTimePed;
}

const TTimeStamp* Calib::GetBeginRunTimeMip(void) const{
  return &BeginRunTimeMip;
}

double Calib::GetVop(void){
  return Vop;
}

double Calib::GetVov(void){
  return Vov;
}

bool Calib::GetBCCalib(void){
  return BCcalc;
}

//*****************************************************************************************
// CALIBRATION Setters for global properties
//*****************************************************************************************
void Calib::SetRunNumber(int r){
  RunNumber=r;
}

void Calib::SetRunNumberPed(int r){
  RunNumberPed=r;
}

void Calib::SetRunNumberMip(int r){
  RunNumberMip=r;
}

void Calib::SetBeginRunTime(TTimeStamp t){
  BeginRunTime=t;
}

void Calib::SetBeginRunTimePed(TTimeStamp t){
  BeginRunTimePed=t;
}

void Calib::SetBeginRunTimeMip(TTimeStamp t){
  BeginRunTimeMip=t;
}

void Calib::SetVop(double v){
  Vop=v;
}

void Calib::SetVov(double v){
  Vov=v;
}

void Calib::SetBCCalib(bool b){
  BCcalc=b;
}


void Calib::PrintGlobalInfo(){
  std::cout << "********************************************************************************************************" << std::endl;
  std::cout << "Calib info:\n \t RunNr: " << GetRunNumber() << "\t start time:" << GetBeginRunTime() 
            << "\n\t RunNr ped: " << GetRunNumberPed() << "\t start time:" << GetBeginRunTimePed() 
            << "\n\t RunNr mip: " << GetRunNumberMip() << "\t start time:" << GetBeginRunTimeMip() 
            << "\n\t Vop: " << GetVop() << "\t Vov: "<< GetVov() << "\t BC calib set: " << GetBCCalib() << std::endl;  
  std::cout << "********************************************************************************************************" << std::endl;
}


void Calib::PrintCalibToFile(TString filename){
  std::fstream fFileCalibOut;
  std::cout << "********************************************************************************************************" << std::endl;
  std::cout << "Printing calib info to: " << filename.Data() << std::endl;
  std::cout << "********************************************************************************************************" << std::endl;
  fFileCalibOut.open(filename.Data(), std::ios::out);
  fFileCalibOut << "#****************************************************************************************************************************************************************************************************************" << std::endl;
  fFileCalibOut << "#Calib info:\n \t RunNr: " << GetRunNumber() << "\t start time:" << GetBeginRunTime() << "\t RunNrPed: " << GetRunNumberPed() << "\t start time:" << GetBeginRunTimePed()<< "\t RunNrMip: " << GetRunNumberMip() << "\t start time:" << GetBeginRunTimeMip()<< "\t Vop: " << GetVop() << "\t Vov: "<< GetVov() << "\t BC calib set: " << GetBCCalib() << std::endl;  
  fFileCalibOut << "#****************************************************************************************************************************************************************************************************************" << std::endl;
  Setup* setup = Setup::GetInstance();

  TString head = Form("#cellID\tlayer\trow\tcolumn\tmodule\tped mean H\tped sig H\tped mean L\tped sig L\tmip Scale H\tmip Width H\tmip Scale L\tmip Width L\tLG-HG\tHG-LG\tBC");  
  fFileCalibOut << head.Data() << std::endl;
  fFileCalibOut << "#****************************************************************************************************************************************************************************************************************" << std::endl;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
      TString outSt = "";
      outSt = Form("%d\t%d\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d",  
                   it->first, setup->GetLayer(it->first), setup->GetRow(it->first), setup->GetColumn(it->first), setup->GetModule(it->first), 
                   it->second.PedestalMeanH, it->second.PedestalSigH,
                   it->second.PedestalMeanL, it->second.PedestalSigL, 
                   it->second.ScaleH, it->second.ScaleWidthH,
                   it->second.ScaleL, it->second.ScaleWidthL,
                   it->second.LGHGCorr, it->second.LGHGCorrOff, it->second.HGLGCorr, it->second.HGLGCorrOff, 
                   it->second.BadChannel);
    fFileCalibOut << outSt.Data() << std::endl;
  }
  fFileCalibOut.close();
}

void Calib::ReadCalibFromTextFile(TString filename, int debug){
  
  std::fstream fFileCalibIn;
  std::cout << "********************************************************************************************************" << std::endl;
  std::cout << "Reading calib info from: " << filename.Data() << std::endl;
  std::cout << "********************************************************************************************************" << std::endl;
  fFileCalibIn.open(filename.Data(), std::ios::in);
  if (!fFileCalibIn) {
      std::cout << "ERROR: file " << filename.Data() << " not found!" << std::endl;
      return;
  }
  int nMod = 0;
  for( TString tempLine; tempLine.ReadLine(fFileCalibIn, kTRUE); ) {
    // check if line should be considered
    if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
        continue;
    }
    if (debug > 0) std::cout << tempLine.Data() << std::endl;

    // Separate the string according to tabulators
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
    if (tempArr->GetEntries() == 9){
     std::cout << tempLine.Data() << std::endl;
     TString part = ((TObjString*)tempArr->At(0))->GetString();
     TObjArray *tempArr2  = part.Tokenize(" ");
     int runNr = ((TString)((TObjString*)tempArr2->At(1))->GetString()).Atoi();
     if (GetRunNumber() != runNr){
       if (debug > 0) std::cout << "Resetting run number: " << runNr << std::endl;
       SetRunNumber(runNr);
       nMod++;
     }
     delete tempArr2;
     
     part = ((TObjString*)tempArr->At(2))->GetString();
     TObjArray *tempArr6  = part.Tokenize(" ");
     int runNrPed = ((TString)((TObjString*)tempArr6->At(1))->GetString()).Atoi();
     if (GetRunNumberPed() != runNrPed){
       if (debug > 0) std::cout << "Resetting run number pedestal: " << runNrPed << std::endl;
       SetRunNumberPed(runNrPed);
       nMod++;
     }
     delete tempArr6;

     part = ((TObjString*)tempArr->At(4))->GetString();
     TObjArray *tempArr5  = part.Tokenize(" ");
     int runNrMip = ((TString)((TObjString*)tempArr5->At(1))->GetString()).Atoi();
     if (GetRunNumberMip() != runNrMip){
       if (debug > 0) std::cout << "Resetting run number mip: " << runNrMip << std::endl;
       SetRunNumberMip(runNrMip);
       nMod++;
     }
     delete tempArr5;
     
     part = ((TObjString*)tempArr->At(6))->GetString();
     TObjArray *tempArr3  = part.Tokenize(" ");
     double vop = ((TString)((TObjString*)tempArr3->At(1))->GetString()).Atof();
     if (TMath::Abs(GetVop() - vop) > 1e-2){
       if (debug > 0) std::cout << "Resetting Vop: " << vop << std::endl;
       SetVop(vop);     
       nMod++;
     }
     delete tempArr3;

     part = ((TObjString*)tempArr->At(7))->GetString();
     TObjArray *tempArr4  = part.Tokenize(" ");
     double vov = ((TString)((TObjString*)tempArr4->At(1))->GetString()).Atof();
     if (TMath::Abs(GetVov() - vov) > 1e-2){
       if (debug > 0) std::cout << "Resetting Vov: " << vov << std::endl;
       SetVop(vov);     
       nMod++;
     }
     delete tempArr4;
     
     continue;
    } else if (tempArr->GetEntries() != 18){
      std::cout << "Temp array has " << tempArr->GetEntries() << " entries"<< std::endl;
      std::cout << tempLine.Data() << std::endl;
      std::cout << "line has wrong format, should be" << std::endl;
      TString head = Form("#cellID\tlayer\trow\tcolumn\tmodule\tped mean H\tped sig H\tped mean L\tped sig L\tmip Scale H\tmip Width H\tmip Scale L\tmip Width L\tLG-HG\tHG-LG\tBC");  
      std::cout << head.Data() << std::endl;
      delete tempArr;
      continue;
    }
    int cellID      = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
    int layer       = ((TString)((TObjString*)tempArr->At(1))->GetString()).Atoi();
    int row         = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atoi();
    int column      = ((TString)((TObjString*)tempArr->At(3))->GetString()).Atoi();
    int moduleNr    = ((TString)((TObjString*)tempArr->At(4))->GetString()).Atoi();
    double pedMH    = ((TString)((TObjString*)tempArr->At(5))->GetString()).Atof();
    double pedSH    = ((TString)((TObjString*)tempArr->At(6))->GetString()).Atof();                       
    double pedML    = ((TString)((TObjString*)tempArr->At(7))->GetString()).Atof();
    double pedSL    = ((TString)((TObjString*)tempArr->At(8))->GetString()).Atof();                       
    double ScaleH   = ((TString)((TObjString*)tempArr->At(9))->GetString()).Atof();                       
    double ScaleHW  = ((TString)((TObjString*)tempArr->At(10))->GetString()).Atof();                       
    double ScaleL   = ((TString)((TObjString*)tempArr->At(11))->GetString()).Atof();                       
    double ScaleLW  = ((TString)((TObjString*)tempArr->At(12))->GetString()).Atof();                       
    double LGHG     = ((TString)((TObjString*)tempArr->At(13))->GetString()).Atof();                       
    double LGHGOff  = ((TString)((TObjString*)tempArr->At(14))->GetString()).Atof();                       
    double HGLG     = ((TString)((TObjString*)tempArr->At(15))->GetString()).Atof();                       
    double HGLGOff  = ((TString)((TObjString*)tempArr->At(16))->GetString()).Atof();                       
    short bc        = ((TString)((TObjString*)tempArr->At(17))->GetString()).Atoi();
      
    if (debug > 0) std::cout << "checking need for reset for CellID:  " << cellID << "\t" << layer << "\t" << row << "\t" << column << "\t" << moduleNr << std::endl;
    TileCalib* tileCal = GetTileCalib(cellID);
    if ( TMath::Abs(pedMH - tileCal->PedestalMeanH) > 1e-4){
      if (debug > 1) std::cout << "resetting ped mean HG" << tileCal->PedestalMeanH << "\t" << pedMH << std::endl;
      tileCal->PedestalMeanH = pedMH;
      nMod++;
    }
    if (TMath::Abs(pedML - tileCal->PedestalMeanL)  > 1e-4){
      if (debug > 1) std::cout << "resetting ped mean LG" << tileCal->PedestalMeanL << "\t" << pedML << std::endl;
      tileCal->PedestalMeanL = pedML;
      nMod++;
    }
    if (TMath::Abs(pedSH - tileCal->PedestalSigH)  > 1e-4){
      if (debug > 1) std::cout << "resetting ped sig HG" << tileCal->PedestalSigH << "\t" << pedSH << std::endl;
      tileCal->PedestalSigH = pedSH;
      nMod++;
    }
    if (TMath::Abs(pedSL - tileCal->PedestalSigL)  > 1e-4){
      if (debug > 1) std::cout << "resetting ped sig LG" << tileCal->PedestalMeanH << "\t" << pedSL << std::endl;
      tileCal->PedestalSigL = pedSL;
      nMod++;
    }
    if (TMath::Abs(ScaleH - tileCal->ScaleH )  > 1e-4){
      if (debug > 1) std::cout << "resetting scale HG" << tileCal->ScaleH << "\t" << ScaleH << std::endl;
      tileCal->ScaleH = ScaleH;
      nMod++;
    }
    if (TMath::Abs(ScaleHW - tileCal->ScaleWidthH)  > 1e-4){
      if (debug > 1) std::cout << "resetting scale width HG" << tileCal->ScaleWidthH << "\t" << ScaleHW << std::endl;
      tileCal->ScaleWidthH = ScaleHW;
      nMod++;
    }
    if (TMath::Abs(ScaleL - tileCal->ScaleL)  > 1e-4){
      if (debug > 1) std::cout << "resetting scale L" << tileCal->ScaleL << "\t" << ScaleL << std::endl;
      tileCal->ScaleL = ScaleL;
      nMod++;
    }
    if (TMath::Abs(ScaleLW - tileCal->ScaleWidthL)  > 1e-4){
      if (debug > 1) std::cout << "resetting scale width LG" << tileCal->ScaleWidthL << "\t" << ScaleLW << std::endl;
      tileCal->ScaleWidthL = ScaleLW;
      nMod++;
    }
    if (TMath::Abs(LGHG - tileCal->LGHGCorr)  > 1e-4){
      if (debug > 1) std::cout << "resetting LG-HG corr" << tileCal->LGHGCorr << "\t" << LGHG << std::endl;
      tileCal->LGHGCorr = LGHG;
      nMod++;
    }
    if (TMath::Abs(LGHGOff - tileCal->LGHGCorrOff)  > 1e-4){
      if (debug > 1) std::cout << "resetting LG-HG corr offset" << tileCal->LGHGCorrOff << "\t" << LGHGOff << std::endl;
      tileCal->LGHGCorrOff = LGHGOff;
      nMod++;
    }
    if (TMath::Abs(HGLG - tileCal->HGLGCorr)  > 1e-5){
      if (debug > 1) std::cout << "resetting HG-LG corr" << tileCal->HGLGCorr << "\t" << HGLG << std::endl;
      tileCal->HGLGCorr = HGLG;
      nMod++;
    }
    if (TMath::Abs(HGLGOff - tileCal->HGLGCorrOff)  > 1e-5){
      if (debug > 1) std::cout << "resetting HG-LG corr offset" << tileCal->HGLGCorrOff << "\t" << HGLGOff << std::endl;
      tileCal->HGLGCorrOff = HGLGOff;
      nMod++;
    }
    if (bc != tileCal->BadChannel){
      if (debug > 1) std::cout << "resetting bad channel" << tileCal->BadChannel << "\t" << bc << std::endl;
      tileCal->BadChannel = bc;
      nMod++;
    }
    delete tempArr;
  }
  
  std::map<int, TileCalib>::const_iterator it;
  bool allBC =true;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.BadChannel == -64) allBC = false;
  }
  if (GetBCCalib() == false && allBC == true){
    nMod++;
    SetBCCalib(true);
  }
  if (GetBCCalib() == true && allBC == false){
    std::cout << "At least one channel is missing the correct bad chanel value" << std::endl; 
  }
  std::cout << "********************************************************************************************************" << std::endl;
  std::cout << "had to perform " << nMod << " modifications to calib loaded from root file." << std::endl;
  std::cout << "********************************************************************************************************" << std::endl;
  std::cout << "done reading input calib" << std::endl;
  std::cout << "********************************************************************************************************" << std::endl;
  

}
