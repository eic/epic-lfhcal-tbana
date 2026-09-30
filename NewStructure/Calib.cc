#include "Calib.h"
#include <iostream>
#include <fstream>
#include "TMath.h"
#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include <utility>

ClassImp(Calib);

//*****************************************************************************************
// CALIBRATION Getters by cell ID
//*****************************************************************************************
/**
 * Return the high-gain pedestal mean for a cell.
 * @param cellID Unique cell identifier.
 * @return Pedestal mean in HG ADC counts or 9999 if unavailable.
 */
double Calib::GetPedestalMeanH(int cellID) const{
  //std::map<int, double>::const_iterator it= PedestalMeanH.find(cellID);
  //if(it!=PedestalMeanH.end()) return it->second;
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.PedestalMeanH;
  else return 9999;
}

/**
 * Return the low-gain pedestal mean for a cell.
 * @param cellID Unique cell identifier.
 * @return Pedestal mean in LG ADC counts or 9999 if unavailable.
 */
double Calib::GetPedestalMeanL(int cellID) const{
  //std::map<int, double>::const_iterator it= PedestalMeanL.find(cellID);
  //if(it!=PedestalMeanL.end()) return it->second;
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.PedestalMeanL;
  else return 9999;
}

/**
 * Return the high-gain pedestal width for a cell.
 * @param cellID Unique cell identifier.
 * @return Pedestal width in HG ADC counts or 9999 if unavailable.
 */
double Calib::GetPedestalSigH(int cellID) const{
  //std::map<int, double>::const_iterator it= PedestalSigH.find(cellID);
  //if(it!=PedestalSigH.end()) return it->second;
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.PedestalSigH;
  else return 9999;
}

/**
 * Return the low-gain pedestal width for a cell.
 * @param cellID Unique cell identifier.
 * @return Pedestal width in LG ADC counts or 9999 if unavailable.
 */
double Calib::GetPedestalSigL(int cellID) const{
  //std::map<int, double>::const_iterator it= PedestalSigL.find(cellID);
  //if(it!=PedestalSigL.end()) return it->second;
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.PedestalSigL;
  else return 9999;
}

/**
 * Return the HG MIP scale for a cell, falling back to the average if needed.
 * @param cellID Unique cell identifier.
 * @return HG scale value or the average scale when the cell is not calibrated.
 */
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

/**
 * Return the per-layer HG MIP scale for a cell, falling back to the average normalized value.
 * @param cellID Unique cell identifier.
 * @return HG scale divided by the number of layers in the segment.
 */
double Calib::GetScaleHighPerSingleLayer(int cellID)const {
  Setup* setup = Setup::GetInstance();
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    if (it->second.ScaleH != -1000  && (BCcalc && it->second.BadChannel > 1)){
      return it->second.ScaleH/setup->GetLayersInSegment(cellID);
    } else {
      return GetAverageScaleHighPerSingleLayer();
    }
  } else return -1.;
}

/**
 * Return the HG scale width for a cell.
 * @param cellID Unique cell identifier.
 * @return Width of the HG MIP distribution or -1 if unavailable.
 */
double Calib::GetScaleWidthHigh(int cellID)const {
  //std::map<int, double>::const_iterator it=ScaleH.find(cellID);
  //if(it!=ScaleH.end()) return it->second;
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.ScaleWidthH;
  else return -1.;
}

/**
 * Return the calculated LG MIP scale for a cell based on the HG scale and LG/HG correlation.
 * @param cellID Unique cell identifier.
 * @return Calculated LG scale value.
 */
double Calib::GetCalcScaleLow(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    if (it->second.ScaleH != -1000 && it->second.LGHGCorr != -64  && (BCcalc && it->second.BadChannel > 1)){
      return (it->second.ScaleH)/it->second.LGHGCorr;
    } else if ( it->second.LGHGCorr != -64  && (BCcalc && it->second.BadChannel > 1)){
      return (GetAverageScaleHigh())/it->second.LGHGCorr;
    } else if (it->second.ScaleH != -1000 && (BCcalc && it->second.BadChannel > 1)){
      return (it->second.ScaleH)/GetAverageLGHGCorr();
    } else {
      return (GetAverageScaleHigh())/GetAverageLGHGCorr();
    }
  }
  else return -1.;
}

/**
 * Return the alternate calculated LG MIP scale for a cell, including the intercept term.
 * @param cellID Unique cell identifier.
 * @return Alternate calculated LG scale value.
 */
double Calib::GetCalcScaleLowAlter(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    if (it->second.ScaleH != -1000 && it->second.LGHGCorr != -64  && (BCcalc && it->second.BadChannel > 1)){
      return (it->second.ScaleH - it->second.LGHGCorrOff)/it->second.LGHGCorr;
    } else if ( it->second.LGHGCorr != -64  && (BCcalc && it->second.BadChannel > 1)){
      return (GetAverageScaleHigh() - it->second.LGHGCorrOff)/it->second.LGHGCorr;
    } else if (it->second.ScaleH != -1000 && (BCcalc && it->second.BadChannel > 1)){
      return (it->second.ScaleH - GetAverageLGHGCorrOff())/GetAverageLGHGCorr();
    } else {
      return (GetAverageScaleHigh()- GetAverageLGHGCorrOff())/GetAverageLGHGCorr();
    }
  }
  else return -1.;
}

/**
 * Return the low-gain MIP scale for a cell.
 * @param cellID Unique cell identifier.
 * @return LG scale value or -1 if unavailable.
 */
double Calib::GetScaleLow(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.ScaleL;
  else return -1.;
}

/**
 * Return the per-layer LG MIP scale for a cell, falling back to the average normalized value.
 * @param cellID Unique cell identifier.
 * @return LG scale divided by the number of layers in the segment.
 */
double Calib::GetScaleLowPerSingleLayer(int cellID)const {
  Setup* setup = Setup::GetInstance();
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    if (it->second.ScaleL != -1000  && (BCcalc && it->second.BadChannel > 1)){
      return it->second.ScaleL/setup->GetLayersInSegment(cellID);
    } else {
      return GetAverageScaleLowPerSingleLayer();
    }
  } else return -1.;
}

/**
 * Return the low-gain scale width for a cell.
 * @param cellID Unique cell identifier.
 * @return Width of the LG MIP distribution or -1 if unavailable.
 */
double Calib::GetScaleWidthLow(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.ScaleWidthL;
  else return -1.;
}

/**
 * Return the LG/HG correlation slope for a cell.
 * @param cellID Unique cell identifier.
 * @return LG/HG slope or -1 if unavailable.
 */
double Calib::GetLGHGCorr(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.LGHGCorr;
  else return -1.;
}

/**
 * Return the LG/HG correlation offset for a cell.
 * @param cellID Unique cell identifier.
 * @return LG/HG offset or -1 if unavailable.
 */
double Calib::GetLGHGCorrOff(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.LGHGCorrOff;
  else return -1.;
}

/**
 * Return the HG/LG correlation slope for a cell.
 * @param cellID Unique cell identifier.
 * @return HG/LG slope or -1 if unavailable.
 */
double Calib::GetHGLGCorr(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.HGLGCorr;
  else return -1.;
}

/**
 * Return the HG/LG correlation offset for a cell.
 * @param cellID Unique cell identifier.
 * @return HG/LG offset or -1 if unavailable.
 */
double Calib::GetHGLGCorrOff(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.HGLGCorrOff;
  else return -1.;
}

/**
 * Return the bad-channel flag for a cell.
 * @param cellID Unique cell identifier.
 * @return Bad-channel flag value.
 */
short Calib::GetBadChannel(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.BadChannel;
  else return -1.;
}

/**
 * Return the ToA offset for a cell.
 * @param cellID Unique cell identifier.
 * @return ToA offset value or -1 if unavailable.
 */
double Calib::GetToAOff(int cellID) const {
  std::map<int, TileCalib>::const_iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()) return it->second.HGLGCorrOff;
  else return -1.;
}


//*****************************************************************************************
// CALIBRATION Getters by cell row, col, layer and module
//*****************************************************************************************
/**
 * Return the high-gain pedestal mean for a cell identified by its geometry coordinates.
 */
double Calib::GetPedestalMeanH(int row, int col, int lay, int mod=0) const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetPedestalMeanH(key);
}

/**
 * Return the low-gain pedestal mean for a cell identified by its geometry coordinates.
 */
double Calib::GetPedestalMeanL(int row, int col, int lay, int mod=0) const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetPedestalMeanL(key);
}

/**
 * Return the high-gain pedestal width for a cell identified by its geometry coordinates.
 */
double Calib::GetPedestalSigH(int row, int col, int lay, int mod=0) const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetPedestalSigH(key);
}

/**
 * Return the low-gain pedestal width for a cell identified by its geometry coordinates.
 */
double Calib::GetPedestalSigL(int row, int col, int lay, int mod=0) const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetPedestalSigL(key);
}

/**
 * Return the HG MIP scale for a cell identified by its geometry coordinates.
 */
double Calib::GetScaleHigh(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col,lay,mod);
  return GetScaleHigh(key);
}

/**
 * Return the per-layer HG MIP scale for a cell identified by its geometry coordinates.
 */
double Calib::GetScaleHighPerSingleLayer(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col,lay,mod);
  return GetScaleHighPerSingleLayer(key);
}

/**
 * Return the HG scale width for a cell identified by its geometry coordinates.
 */
double Calib::GetScaleWidthHigh(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col,lay,mod);
  return GetScaleWidthHigh(key);
}

/**
 * Return the LG MIP scale for a cell identified by its geometry coordinates.
 */
double Calib::GetScaleLow(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetScaleLow(key);
}

/**
 * Return the per-layer LG MIP scale for a cell identified by its geometry coordinates.
 */
double Calib::GetScaleLowPerSingleLayer(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col,lay,mod);
  return GetScaleLowPerSingleLayer(key);
}

/**
 * Return the calculated LG MIP scale for a cell identified by its geometry coordinates.
 */
double Calib::GetCalcScaleLow(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetCalcScaleLow(key);
}

/**
 * Return the alternate calculated LG MIP scale for a cell identified by its geometry coordinates.
 */
double Calib::GetCalcScaleLowAlter(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetCalcScaleLowAlter(key);
}

/**
 * Return the LG scale width for a cell identified by its geometry coordinates.
 */
double Calib::GetScaleWidthLow(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetScaleWidthLow(key);
}

/**
 * Return the LG/HG correlation slope for a cell identified by its geometry coordinates.
 */
double Calib::GetLGHGCorr(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetLGHGCorr(key);
}

/**
 * Return the LG/HG correlation offset for a cell identified by its geometry coordinates.
 */
double Calib::GetLGHGCorrOff(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetLGHGCorrOff(key);
}

/**
 * Return the HG/LG correlation slope for a cell identified by its geometry coordinates.
 */
double Calib::GetHGLGCorr(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetHGLGCorr(key);
}

/**
 * Return the HG/LG correlation offset for a cell identified by its geometry coordinates.
 */
double Calib::GetHGLGCorrOff(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetHGLGCorrOff(key);
}

/**
 * Return the bad-channel flag for a cell identified by its geometry coordinates.
 */
short Calib::GetBadChannel(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetBadChannel(key);
}

/**
 * Return the ToA offset for a cell identified by its geometry coordinates.
 */
double Calib::GetToAOff(int row, int col, int lay, int mod=0)const{
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetToAOff(key);
}


//*****************************************************************************************
// CALIBRATION Average calculators
//*****************************************************************************************
/**
 * Return the average high-gain pedestal mean for the full calibration set.
 */
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

/**
 * Return the average high-gain pedestal width for the full calibration set.
 */
double Calib::GetAveragePedestalSigHigh()const{
  double avSc = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.PedestalSigH == -1000)
      notCalib++;
    else 
      avSc += it->second.PedestalSigH;
  }
  return avSc/(CaloCalib.size()-notCalib);
}

/**
 * Return the average low-gain pedestal mean for the full calibration set.
 */
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

/**
 * Return the average low-gain pedestal width for the full calibration set.
 */
double Calib::GetAveragePedestalSigLow()const{
  double avSc = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.PedestalSigL == -1000)
      notCalib++;
    else
      avSc += it->second.PedestalSigL;
  }
  return avSc/(CaloCalib.size()-notCalib);
}

/**
 * Return the average HG MIP scale over the calibrated channels.
 */
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
  double avScaleR = -10000;
  if (avSc != 0. && (CaloCalib.size()-notCalib) != 0)
    avScaleR      = avSc/(CaloCalib.size()-notCalib);
  return avScaleR;
}

/**
 * Return the average HG MIP scale and fill the number of active channels.
 */
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
  double avScaleR = -10000;
  if (avSc != 0. && active > 0)
    avScaleR      = avSc/active;
  return avScaleR;
}

/**
 * Return the average HG MIP scale normalized by layer over the calibrated channels.
 */
double Calib::GetAverageScaleHighPerSingleLayer( )const{
  double avSc   = 0;
  int notCalib  = 0;
  Setup* setup = Setup::GetInstance();
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.ScaleH == -1000 || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.ScaleH/setup->GetLayersInSegment(it->first);
    }
  }
  int active = (CaloCalib.size()-notCalib);
  double avScaleR = -10000;
  if (avSc != 0. && active > 0)
    avScaleR      = avSc/active;
  return avScaleR;
}

/**
 * Return the average HG MIP scale normalized by layer and the average tiles-per-layer value.
 */
double Calib::GetAverageScaleHighPerSingleLayer(int &active, double &avTilesPerLayer )const{
  double avSc   = 0;
  int notCalib  = 0;
  avTilesPerLayer = 0.;
  Setup* setup = Setup::GetInstance();
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.ScaleH == -1000 || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.ScaleH/setup->GetLayersInSegment(it->first);
      avTilesPerLayer +=setup->GetLayersInSegment(it->first);
    }
  }
  active=(CaloCalib.size()-notCalib);
  double avScaleR = -10000;
  if (avSc != 0. && active > 0){
    avScaleR      = avSc/active;
    avTilesPerLayer=avTilesPerLayer/active;
  } else {
    avTilesPerLayer= -10000;
  }
  return avScaleR;
}

/**
 * Return the average HG scale width over the calibrated channels.
 */
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
  int active = (CaloCalib.size()-notCalib);
  double avScaleR = -10000;
  if (avSc != 0. && active > 0)
    avScaleR      = avSc/active;
  return avScaleR;
}

/**
 * Return the average LG MIP scale over the calibrated channels.
 */
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
  int active = (CaloCalib.size()-notCalib);
  double avScaleR = -10000;
  if (avSc != 0. && active > 0)
    avScaleR      = avSc/active;
  return avScaleR;
}

/**
 * Compute the average LG MIP scale normalized by the number of layers in the segment.
 */
double Calib::GetAverageScaleLowPerSingleLayer( )const{
  double avSc   = 0;
  int notCalib  = 0;
  Setup* setup = Setup::GetInstance();
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.ScaleL == -1000 || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.ScaleL/setup->GetLayersInSegment(it->first);
    }
  }
  int active = (CaloCalib.size()-notCalib);
  double avScaleR = -10000;
  if (avSc != 0. && active > 0)
    avScaleR      = avSc/active;
  return avScaleR;
}

/**
 * Compute the average LG MIP scale normalized by layer and return the active-channel count.
 */
double Calib::GetAverageScaleLowPerSingleLayer(int &active, double &avTilesPerLayer )const{
  double avSc   = 0;
  int notCalib  = 0;
  avTilesPerLayer = 0.;
  Setup* setup = Setup::GetInstance();
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.ScaleL == -1000 || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.ScaleL/setup->GetLayersInSegment(it->first);
      avTilesPerLayer +=setup->GetLayersInSegment(it->first);
    }
  }
  active=(CaloCalib.size()-notCalib);
  double avScaleR = -10000;
  if (avSc != 0. && active > 0){
    avScaleR      = avSc/active;
    avTilesPerLayer=avTilesPerLayer/active;
  } else {
    avTilesPerLayer= -10000;
  }
  return avScaleR;
}

/**
 * Return the average LG MIP scale and fill the number of active channels.
 */
double Calib::GetAverageScaleLow(int &active )const{
  double avSc   = 0;
  int notCalib  = 0;
  std::map<int, TileCalib>::const_iterator it;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.ScaleL == -1000 || (BCcalc && it->second.BadChannel < 2) ){
      notCalib++;
    } else {
      avSc += it->second.ScaleL;
    }
  }
  active=(CaloCalib.size()-notCalib);
  double avScaleR = -10000;
  if (avSc != 0. && active > 0)
    avScaleR      = avSc/active;
  return avScaleR;
}

/**
 * Return the average LG scale width over the calibrated channels.
 */
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
  int active = (CaloCalib.size()-notCalib);
  double avScaleR = -10000;
  if (avSc != 0. && active > 0)
    avScaleR      = avSc/active;
  return avScaleR;
}

/**
 * Return the average LG/HG correlation slope over the calibrated channels.
 */
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

/**
 * Return the average LG/HG correlation offset over the calibrated channels.
 */
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

/**
 * Return the average HG/LG correlation slope over the calibrated channels.
 */
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

/**
 * Return the average HG/LG correlation offset over the calibrated channels.
 */
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

/**
 * Count the number of channels with a specific bad-channel flag.
 * @param bcflag Flag to count.
 * @return Number of matching channels.
 */
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
/**
 * Access the calibration record for a cell, creating it if necessary.
 * @param cellID Unique cell identifier.
 * @return Pointer to the calibration record for this cell.
 */
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

/**
 * Access the calibration record for a cell identified by its geometry coordinates.
 */
TileCalib* Calib::GetTileCalib(int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row, col, lay, mod);
  return GetTileCalib(key);
}

//*****************************************************************************************
// CALIBRATION Setters by cell ID
//*****************************************************************************************
/**
 * Store the high-gain pedestal mean for a cell.
 */
void Calib::SetPedestalMeanH(double ped, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.PedestalMeanH=ped;
    CaloCalib[cellID]=acal;
  }
  else it->second.PedestalMeanH=ped;
}

/**
 * Store the low-gain pedestal mean for a cell.
 */
void Calib::SetPedestalMeanL(double ped, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.PedestalMeanL=ped;
    CaloCalib[cellID]=acal;
  }
  else it->second.PedestalMeanL=ped;
}

/**
 * Store the high-gain pedestal width for a cell.
 */
void Calib::SetPedestalSigH(double ped, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.PedestalSigH=ped;
    CaloCalib[cellID]=acal;
  }
  else it->second.PedestalSigH=ped;
}

/**
 * Store the low-gain pedestal width for a cell.
 */
void Calib::SetPedestalSigL(double ped, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.PedestalSigL=ped;
    CaloCalib[cellID]=acal;
  }
  else it->second.PedestalSigL=ped;
}

/**
 * Store the HG MIP scale for a cell.
 */
void Calib::SetScaleHigh(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.ScaleH=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.ScaleH=s;
}

/**
 * Store the HG scale width for a cell.
 */
void Calib::SetScaleWidthHigh(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.ScaleWidthH=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.ScaleWidthH=s;
}

/**
 * Store the LG MIP scale for a cell.
 */
void Calib::SetScaleLow(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.ScaleL=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.ScaleL=s;
}

/**
 * Store the LG scale width for a cell.
 */
void Calib::SetScaleWidthLow(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.ScaleWidthL=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.ScaleWidthL=s;
}

/**
 * Store the LG/HG correlation slope for a cell.
 */
void Calib::SetLGHGCorr(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.LGHGCorr=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.LGHGCorr=s;
}

/**
 * Store the LG/HG correlation offset for a cell.
 */
void Calib::SetLGHGCorrOff(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.LGHGCorrOff=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.LGHGCorrOff=s;
}

/**
 * Store the HG/LG correlation slope for a cell.
 */
void Calib::SetHGLGCorr(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.HGLGCorr=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.HGLGCorr=s;
}

/**
 * Store the HG/LG correlation offset for a cell.
 */
void Calib::SetHGLGCorrOff(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.HGLGCorrOff=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.HGLGCorrOff=s;
}

/**
 * Store the bad-channel flag for a cell.
 */
void Calib::SetBadChannel(short s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.BadChannel=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.BadChannel=s;
}

/**
 * Store the ToA offset for a cell.
 */
void Calib::SetToAOff(double s, int cellID){
  std::map<int, TileCalib>::iterator it= CaloCalib.find(cellID);
  if(it!=CaloCalib.end()){
    TileCalib acal;
    acal.HGLGCorrOff=s;
    CaloCalib[cellID]=acal;
  }
  else it->second.HGLGCorrOff=s;
}




//*****************************************************************************************
// CALIBRATION Setters by cell row, col, layer and module
//*****************************************************************************************
/**
* Store the high-gain pedestal mean for a cell identified by its geometry coordinates.
*/
void Calib::SetPedestalMeanH(double ped, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetPedestalMeanH(ped,key);
}
/**
* Store the low-gain pedestal mean for a cell identified by its geometry coordinates.
*/
void Calib::SetPedestalMeanL(double ped, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetPedestalMeanL(ped,key);
}
/**
* Store the high-gain pedestal width for a cell identified by its geometry coordinates.
*/
void Calib::SetPedestalSigH(double ped, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetPedestalSigH(ped,key);
}
/**
* Store the low-gain pedestal width for a cell identified by its geometry coordinates.
*/
void Calib::SetPedestalSigL(double ped, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetPedestalSigL(ped,key);
}
 
/**
* Store the HG MIP scale for a cell identified by its geometry coordinates.
*/
void Calib::SetScaleHigh(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetScaleHigh(s,key);
}
 
/**
* Store the HG scale width for a cell identified by its geometry coordinates.
*/
void Calib::SetScaleWidthHigh(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetScaleWidthHigh(s,key);
}
 
/**
* Store the LG MIP scale for a cell identified by its geometry coordinates.
*/
void Calib::SetScaleLow(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetScaleLow(s,key);
}
 
/**
* Store the LG scale width for a cell identified by its geometry coordinates.
*/
void Calib::SetScaleWidthLow(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetScaleWidthLow(s,key);
}
 
/**
* Store the LG/HG correlation slope for a cell identified by its geometry coordinates.
*/
void Calib::SetLGHGCorr(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetLGHGCorr(s,key);
}
 
/**
* Store the LG/HG correlation offset for a cell identified by its geometry coordinates.
*/
void Calib::SetLGHGCorrOff(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetLGHGCorrOff(s,key);
}
 
/**
* Store the HG/LG correlation slope for a cell identified by its geometry coordinates.
*/
void Calib::SetHGLGCorr(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetHGLGCorr(s,key);
}
 
/**
* Store the HG/LG correlation offset for a cell identified by its geometry coordinates.
*/
void Calib::SetHGLGCorrOff(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetHGLGCorrOff(s,key);
}
 
/**
* Store the bad-channel flag for a cell identified by its geometry coordinates.
*/
void Calib::SetBadChannel(short s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetBadChannel(s,key);
}
 
/**
* Store the ToA offset for a cell identified by its geometry coordinates.
*/
void Calib::SetToAOff(double s, int row, int col, int lay, int mod=0){
  Setup* setup = Setup::GetInstance();
  int key=setup->GetCellID(row,col,lay,mod);
  SetToAOff(s,key);
}


/**
 * Check whether a layer contains at least one valid good channel.
 */
bool Calib::IsLayerEnabled(int layer, int mod) const{
  Setup* setup = Setup::GetInstance();
  bool isEnabled = false;
  if (!BCcalc) return true;
  for (int r = 0; r< setup->GetNMaxRow(); r++){
    for (int c = 0; c < setup->GetNMaxColumn(); c++){
      if (mod == -1){
        for (int m = 0; m < setup->GetNMaxModule(); m++){
          int bc = GetBadChannel(r, c, layer, m);
          if (bc == 3 ){
            isEnabled = true;
            break;
          }
        }
      } else {
        int bc = GetBadChannel(r, c, layer, mod);
        if (bc == 3){
          isEnabled = true;
          break;
        }
      }
    }
  }
  return isEnabled;
}

//*****************************************************************************************
// CALIBRATION Getters for global properties
//*****************************************************************************************
/**
 * Return the configured run number.
 */
int Calib::GetRunNumber(void){
  return RunNumber;
}

/**
 * Return the pedestal calibration run number.
 */
int Calib::GetRunNumberPed(void){
  return RunNumberPed;
}

/**
 * Return the MIP calibration run number.
 */
int Calib::GetRunNumberMip(void){
  return RunNumberMip;
}

/**
 * Return the begin-run timestamp for the main calibration.
 */
const TTimeStamp* Calib::GetBeginRunTime(void) const{
  return &BeginRunTime;
}

/**
 * Return the begin-run timestamp for the pedestal calibration.
 */
const TTimeStamp* Calib::GetBeginRunTimePed(void) const{
  return &BeginRunTimePed;
}

/**
 * Return the begin-run timestamp for the MIP calibration.
 */
const TTimeStamp* Calib::GetBeginRunTimeMip(void) const{
  return &BeginRunTimeMip;
}

/**
 * Return the Vop setting.
 */
double Calib::GetVop(void){
  return Vop;
}

/**
 * Return the Vov setting.
 */
double Calib::GetVov(void){
  return Vov;
}

/**
 * Return whether the bad-channel calibration has been computed.
 */
bool Calib::GetBCCalib(void){
  return BCcalc;
}

//*****************************************************************************************
// CALIBRATION Setters for global properties
//*****************************************************************************************
/**
 * Set the main calibration run number.
 */
void Calib::SetRunNumber(int r){
  RunNumber=r;
}

/**
 * Set the pedestal calibration run number.
 */
void Calib::SetRunNumberPed(int r){
  RunNumberPed=r;
}

/**
 * Set the MIP calibration run number.
 */
void Calib::SetRunNumberMip(int r){
  RunNumberMip=r;
}

/**
 * Set the begin-run timestamp for the main calibration.
 */
void Calib::SetBeginRunTime(TTimeStamp t){
  BeginRunTime=t;
}

/**
 * Set the begin-run timestamp for the pedestal calibration.
 */
void Calib::SetBeginRunTimePed(TTimeStamp t){
  BeginRunTimePed=t;
}

/**
 * Set the begin-run timestamp for the MIP calibration.
 */
void Calib::SetBeginRunTimeMip(TTimeStamp t){
  BeginRunTimeMip=t;
}

/**
 * Set the Vop setting.
 */
void Calib::SetVop(double v){
  Vop=v;
}

/**
 * Set the Vov setting.
 */
void Calib::SetVov(double v){
  Vov=v;
}

/**
 * Set whether the bad-channel calibration has been computed.
 */
void Calib::SetBCCalib(bool b){
  BCcalc=b;
}

/**
 * Print a compact summary of the calibration metadata.
 */
void Calib::PrintGlobalInfo(){
  std::cout << "********************************************************************************************************" << std::endl;
  std::cout << "Calib info:\n \t RunNr: " << GetRunNumber() << "\t start time:" << GetBeginRunTime() 
            << "\n\t RunNr ped: " << GetRunNumberPed() << "\t start time:" << GetBeginRunTimePed() 
            << "\n\t RunNr mip: " << GetRunNumberMip() << "\t start time:" << GetBeginRunTimeMip() 
            << "\n\t Vop: " << GetVop() << "\t Vov: "<< GetVov() << "\t BC calib set: " << GetBCCalib() << std::endl;  
  std::cout << "\n\t mean Scale: " <<     GetAverageScaleHigh() << std::endl;      
  std::cout << "********************************************************************************************************" << std::endl;
}

/**
 * Print a detailed summary of the calibration parameters.
 */
void Calib::PrintDetailedGlobalInfo(){
  std::cout << "********************************************************************************************************" << std::endl;
  std::cout << "Calib info:\n \t RunNr: " << GetRunNumber() << "\t start time:" << GetBeginRunTime() 
            << "\n\t RunNr ped: " << GetRunNumberPed() << "\t start time:" << GetBeginRunTimePed() 
            << "\n\t RunNr mip: " << GetRunNumberMip() << "\t start time:" << GetBeginRunTimeMip() 
            << "\n\t Vop: " << GetVop() << "\t Vov: "<< GetVov() << "\t BC calib set: " << GetBCCalib() 
            << "\n\t mean HG Scale: " <<     GetAverageScaleHigh() << "\t width\t" <<  GetAverageScaleWidthHigh() 
            << "\n\t mean LG Scale: " <<     GetAverageScaleLow() << "\t width\t" <<  GetAverageScaleWidthLow() 
            << "\n\t mean LG-HG a: " <<     GetAverageLGHGCorr() << "\t b\t" <<  GetAverageLGHGCorrOff() 
            << "\n\t mean HG-LG a: " <<     GetAverageHGLGCorr() << "\t b\t" <<  GetAverageHGLGCorrOff() << std::endl;      
  std::cout << "********************************************************************************************************" << std::endl;
}


//***********************************************************************************************
//*********************** Print calib file to text file *****************************************
//***********************************************************************************************
/**
 * Write the full calibration table to a text file.
 * @param filename Output file path.
 */
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

//***********************************************************************************************
//*********************** Reparse the calib file from external text file ************************
//***********************************************************************************************
/**
 * Read the calibration constants from a text file.
 * @param filename Input file path.
 * @param debug Verbosity level.
 */
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

//***********************************************************************************************
//***************** Reading of external bad channel map from file *******************************
//***********************************************************************************************
/**
 * Read a bad-channel map from an external calibration file.
 * @param filename Path to the map file.
 * @param debug Verbosity level.
 * file format: tab or space separated list with header being ignored
 * flagged cells need to be given in following format line by line:
 * # moduleNr layerNr rowNr colNr BC-status
 * 
 * BC-status codes:
 *  - 0: off
 *  - 1: bad
 *  - 2: funky
 *  - 3: good
 */
void Calib::ReadExternalBadChannelMap(TString filename, int debug){
   
  std::cout << "Reading in external mapping file" << std::endl;
  Setup* setup = Setup::GetInstance();
   
  std::ifstream bcmapFile;
  bcmapFile.open(filename,std::ios_base::in);
  if (!bcmapFile) {
    std::cout << "ERROR: file " << filename.Data() << " not found!" << std::endl;
    return;
  }

  int nBCs = 0;
  for( TString tempLine; tempLine.ReadLine(bcmapFile, kTRUE); ) {
    // check if line should be considered
    if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
      continue;
    }
    if (debug > 1) std::cout << tempLine.Data() << std::endl;

    // Separate the string according to tabulators
    TObjArray *tempArr  = tempLine.Tokenize(" ");
    if(tempArr->GetEntries()<2){
      if (debug > 1) std::cout << "nothing to be done" << std::endl;
      delete tempArr;
      continue;
    } 
     
    int mod     = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
    int layer   = ((TString)((TObjString*)tempArr->At(1))->GetString()).Atoi();
    int row     = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atoi();
    int col     = ((TString)((TObjString*)tempArr->At(3))->GetString()).Atoi();
    short bc    = short(((TString)((TObjString*)tempArr->At(4))->GetString()).Atoi());
     
    int cellID  = setup->GetCellID( row, col, layer, mod);    
    TileCalib* tileCal = GetTileCalib(cellID);
     
    tileCal->BadChannel = bc;
    nBCs++;
     
    if (debug > 1) std::cout << "cellID " << cellID << "\t BC status: " << bc<< std::endl;
  }
  std::cout << "registered " << nBCs << " bad channels!" << std::endl;

  std::map<int, TileCalib>::const_iterator it;
  bool allBC =true;
  for(it=CaloCalib.begin(); it!=CaloCalib.end(); ++it){
    if (it->second.BadChannel == -64) allBC = false;
  }
  if (GetBCCalib() == false && allBC == true){
    SetBCCalib(true);
  }
  if (GetBCCalib() == true && allBC == false){
    std::cout << "At least one channel is missing the correct bad chanel value" << std::endl; 
  }
  return;
}

//***********************************************************************************************
//***************** Reading of external file to set the toA offsets for each half-asic **********
//***********************************************************************************************
/**
 * Read the ToA offsets from an external file.
 * @param filename Path to the offset file.
 * @param debug Verbosity level.
 */
void Calib::ReadExternalToAOffsets(TString filename, int debug){
  
  std::cout << "Reading in ToA offset file" << std::endl;
  Setup* setup = Setup::GetInstance();
  
  std::ifstream toaOffSetFile;
  toaOffSetFile.open(filename,std::ios_base::in);
  if (!toaOffSetFile) {
    std::cout << "ERROR: file " << filename.Data() << " not found!" << std::endl;
    return;
  }

  int nToAOffsets = 0;
  for( TString tempLine; tempLine.ReadLine(toaOffSetFile, kTRUE); ) {
    // check if line should be considered
    if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
      continue;
    }
    if (debug > 1) std::cout << tempLine.Data() << std::endl;

    // Separate the string according to tabulators
    TObjArray *tempArr  = tempLine.Tokenize("\t");
    // TObjArray *tempArr  = tempLine.Tokenize(" ");
    if(tempArr->GetEntries()<2){
      if (debug > 1) std::cout << "nothing to be done" << std::endl;
      if (debug > 1) std::cout << tempArr->At(0) << std::endl;
      delete tempArr;
      continue;
    } 
    
    int asic    = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
    int half    = ((TString)((TObjString*)tempArr->At(1))->GetString()).Atoi();
    int toaOff  = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atoi();
    
    if (half == 0){
      for (Int_t c = 0; c< 36; c++){
        int cellID  = setup->GetCellID( asic, c); 
        if (cellID != -1){
          TileCalib* tileCal = GetTileCalib(cellID);
          tileCal->HGLGCorrOff = double(toaOff);
          if (debug > 2) std::cout << "cellID " << cellID << "\t ToA offset: " << toaOff<< std::endl;
        }
      }
    } else if  (half == 1){
      for (Int_t c = 36; c< 72; c++){
        int cellID  = setup->GetCellID( asic, c); 
        if (cellID != -1){
          TileCalib* tileCal = GetTileCalib(cellID);
          tileCal->HGLGCorrOff = double(toaOff);
          if (debug > 2) std::cout << "cellID " << cellID << "\t ToA offset: " << toaOff<< std::endl;
        }
      }    
    }
    nToAOffsets++;

  }
  std::cout << "registered " << nToAOffsets << " different offsets!" << std::endl;

  std::map<int, TileCalib>::const_iterator it;
  return;
}

