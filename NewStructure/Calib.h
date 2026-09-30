#ifndef CALIB_H
#define CALIB_H

#include <cstddef>
#include <map>
#include "TTimeStamp.h"
#include "Setup.h"

struct TileCalib{
  double PedestalMeanH  = -1000.;     // pedestal mean HG ADC (CAEN) or first sample ADC (HGCROC)
  double PedestalMeanL  = -1000.;     // pedestal mean LG ADC (CAEN) or average pedestal ADC (HGCROC)
  double PedestalSigH   = -1000.;     // width of pedest HG ADC (CAEN) or width of first sample ADC distribution (HGCROC)
  double PedestalSigL   = -1000.;     // width of pedest LG ADC (CAEN) or width of average pedestal ADC distribution (HGCROC)
  double ScaleH         = -1000.;     // Max Mip in HG ADC (CAEN) or ADC (HGCROC) 
  double ScaleWidthH    = -1000.;     // FWHM of Mip in HG ADC (CAEN) or ADC (HGCROC) 
  double ScaleL         = -1000.;     // Max Mip in LG ADC (CAEN) 
  double ScaleWidthL    = -1000.;     // FWHM Mip in LG ADC (CAEN) 
  double LGHGCorr       = -64;        // slope of correlation between LG (x axis) & HG (y axis) for CAEN data 
  double LGHGCorrOff    = -1000.;     // intercept of correlation between LG (x axis) & HG (y axis) for CAEN data 
  double HGLGCorr       = -64;        // slope of correlation between HG (x axis) & LG (y axis) for CAEN data
  double HGLGCorrOff    = -1000.;     // intercept of correlation between HG (x axis) & LG (y axis) for CAEN data
  short BadChannel      = -64;        // bad channel flag: 0 - off, 1 - bad, 2 - funky, 3 - good
} ;

class Calib{

 public:
  /** Default constructor. */
  Calib() {}
  /** Destructor. */
  virtual ~Calib() {}

  /** Return the high-gain pedestal mean for a cell. */
  double GetPedestalMeanH (int /**/) const;
  /** Return the low-gain pedestal mean for a cell. */
  double GetPedestalMeanL (int /**/) const;
  /** Return the high-gain pedestal mean for a cell identified by row, column, layer, and module. */
  double GetPedestalMeanH (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the low-gain pedestal mean for a cell identified by row, column, layer, and module. */
  double GetPedestalMeanL (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the high-gain pedestal sigma for a cell. */
  double GetPedestalSigH (int /**/) const;
  /** Return the low-gain pedestal sigma for a cell. */
  double GetPedestalSigL (int /**/) const;
  /** Return the high-gain pedestal sigma for a cell identified by row, column, layer, and module. */
  double GetPedestalSigH (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the low-gain pedestal sigma for a cell identified by row, column, layer, and module. */
  double GetPedestalSigL (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the HG MIP scale for a cell. */
  double GetScaleHigh(int /**/) const;
  /** Return the HG MIP scale for a cell identified by row, column, layer, and module. */
  double GetScaleHigh(int /**/, int /**/, int /**/, int /**/) const;
  /** Return the HG MIP scale normalized by layer for a cell. */
  double GetScaleHighPerSingleLayer(int /**/) const;
  /** Return the HG MIP scale normalized by layer for a cell identified by row, column, layer, and module. */
  double GetScaleHighPerSingleLayer(int /**/, int /**/, int /**/, int /**/) const;

  /** Return the high-gain scale width for a cell. */
  double GetScaleWidthHigh(int /**/) const;
  /** Return the high-gain scale width for a cell identified by row, column, layer, and module. */
  double GetScaleWidthHigh(int /**/, int /**/, int /**/, int /**/) const;
  /** Return the calculated LG MIP scale for a cell. */
  double GetCalcScaleLow (int /**/) const;
  /** Return the calculated LG MIP scale for a cell identified by row, column, layer, and module. */
  double GetCalcScaleLow (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the alternate calculated LG MIP scale for a cell. */
  double GetCalcScaleLowAlter (int /**/) const;
  /** Return the alternate calculated LG MIP scale for a cell identified by row, column, layer, and module. */
  double GetCalcScaleLowAlter (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the low-gain MIP scale for a cell. */
  double GetScaleLow (int /**/) const;
  /** Return the low-gain MIP scale for a cell identified by row, column, layer, and module. */
  double GetScaleLow (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the LG MIP scale normalized by layer for a cell. */
  double GetScaleLowPerSingleLayer(int /**/) const;
  /** Return the LG MIP scale normalized by layer for a cell identified by row, column, layer, and module. */
  double GetScaleLowPerSingleLayer(int /**/, int /**/, int /**/, int /**/) const;
  /** Return the low-gain scale width for a cell. */
  double GetScaleWidthLow (int /**/) const;
  /** Return the low-gain scale width for a cell identified by row, column, layer, and module. */
  double GetScaleWidthLow (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the LG/HG correlation slope for a cell. */
  double GetLGHGCorr (int /**/) const;
  /** Return the LG/HG correlation slope for a cell identified by row, column, layer, and module. */
  double GetLGHGCorr (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the LG/HG correlation offset for a cell. */
  double GetLGHGCorrOff (int /**/) const;
  /** Return the LG/HG correlation offset for a cell identified by row, column, layer, and module. */
  double GetLGHGCorrOff (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the HG/LG correlation slope for a cell. */
  double GetHGLGCorr (int /**/) const;
  /** Return the HG/LG correlation slope for a cell identified by row, column, layer, and module. */
  double GetHGLGCorr (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the HG/LG correlation offset for a cell. */
  double GetHGLGCorrOff (int /**/) const;
  /** Return the HG/LG correlation offset for a cell identified by row, column, layer, and module. */
  double GetHGLGCorrOff (int /**/, int /**/, int /**/, int /**/) const;
  /** Return the ToA offset for a cell. */
  double GetToAOff (int /**/) const;
  /** Return the ToA offset for a cell identified by row, column, layer, and module. */
  double GetToAOff (int /**/, int /**/, int /**/, int /**/) const;

  /** Return the average high-gain pedestal mean over all calibrated channels. */
  double GetAveragePedestalMeanHigh() const;
  /** Return the average high-gain pedestal width over all calibrated channels. */
  double GetAveragePedestalSigHigh() const;
  /** Return the average low-gain pedestal mean over all calibrated channels. */
  double GetAveragePedestalMeanLow() const;
  /** Return the average low-gain pedestal width over all calibrated channels. */
  double GetAveragePedestalSigLow() const;
  /** Return the average high-gain MIP scale over all calibrated channels. */
  double GetAverageScaleHigh() const;
  /** Return the mean HG MIP value normalized per layer for the full detector. */
  double GetAverageScaleHighPerSingleLayer()const;
  /** Return the average high-gain MIP scale and the active channel count. */
  double GetAverageScaleHigh(int &) const;
  /** Return the mean HG MIP value and average tiles per layer for the active channels. */
  double GetAverageScaleHighPerSingleLayer(int &, double &)const;
  /** Return the average high-gain scale width over all calibrated channels. */
  double GetAverageScaleWidthHigh() const;
  /** Return the average low-gain MIP scale over all calibrated channels. */
  double GetAverageScaleLow() const;
  /** Return the mean LG MIP value normalized per layer for the full detector. */
  double GetAverageScaleLowPerSingleLayer()const;
  /** Return the average low-gain MIP scale and the active channel count. */
  double GetAverageScaleLow(int &) const;
  /** Return the mean LG MIP value and average tiles per layer for the active channels. */
  double GetAverageScaleLowPerSingleLayer(int &, double &)const;
  /** Return the average low-gain scale width over all calibrated channels. */
  double GetAverageScaleWidthLow() const;
  /** Return the average HG/LG correlation slope over all calibrated channels. */
  double GetAverageHGLGCorr() const;
  /** Return the average HG/LG correlation offset over all calibrated channels. */
  double GetAverageHGLGCorrOff() const;
  /** Return the average LG/HG correlation slope over all calibrated channels. */
  double GetAverageLGHGCorr() const;
  /** Return the average LG/HG correlation offset over all calibrated channels. */
  double GetAverageLGHGCorrOff() const;

  /** Count the number of channels matching a given bad-channel flag. */
  int GetNumberOfChannelsWithBCflag (short ) const;
  /** Return the bad-channel flag for a cell. */
  short GetBadChannel(int /**/) const;
  /** Return the bad-channel flag for a cell identified by row, column, layer, and module. */
  short GetBadChannel(int /**/, int /**/, int /**/, int /**/) const;

  /** Access the calibration record for a cell, creating it if needed. */
  TileCalib* GetTileCalib(int /**/);
  /** Access the calibration record for a cell identified by row, column, layer, and module. */
  TileCalib* GetTileCalib(int /**/, int /**/, int /**/, int /**/);

  /** Set the high-gain pedestal mean for a cell. */
  void   SetPedestalMeanH (double, int);
  /** Set the low-gain pedestal mean for a cell. */
  void   SetPedestalMeanL (double, int);
  /** Set the high-gain pedestal mean for a cell identified by row, column, layer, and module. */
  void   SetPedestalMeanH (double, int, int, int, int);
  /** Set the low-gain pedestal mean for a cell identified by row, column, layer, and module. */
  void   SetPedestalMeanL (double, int, int, int, int);
  /** Set the high-gain pedestal sigma for a cell. */
  void   SetPedestalSigH (double, int);
  /** Set the low-gain pedestal sigma for a cell. */
  void   SetPedestalSigL (double, int);
  /** Set the high-gain pedestal sigma for a cell identified by row, column, layer, and module. */
  void   SetPedestalSigH (double, int, int, int, int);
  /** Set the low-gain pedestal sigma for a cell identified by row, column, layer, and module. */
  void   SetPedestalSigL (double, int, int, int, int);
  /** Set the HG MIP scale for a cell. */
  void   SetScaleHigh(double, int);
  /** Set the HG MIP scale for a cell identified by row, column, layer, and module. */
  void   SetScaleHigh(double, int, int, int, int);
  /** Set the HG scale width for a cell. */
  void   SetScaleWidthHigh(double, int);
  /** Set the HG scale width for a cell identified by row, column, layer, and module. */
  void   SetScaleWidthHigh(double, int, int, int, int);
  /** Set the LG MIP scale for a cell. */
  void   SetScaleLow (double, int);
  /** Set the LG MIP scale for a cell identified by row, column, layer, and module. */
  void   SetScaleLow (double, int, int, int, int);
  /** Set the LG scale width for a cell. */
  void   SetScaleWidthLow (double, int);
  /** Set the LG scale width for a cell identified by row, column, layer, and module. */
  void   SetScaleWidthLow (double, int, int, int, int);
  /** Set the LG/HG correlation slope for a cell. */
  void   SetLGHGCorr (double, int);
  /** Set the LG/HG correlation slope for a cell identified by row, column, layer, and module. */
  void   SetLGHGCorr (double, int, int, int, int);
  /** Set the LG/HG correlation offset for a cell. */
  void   SetLGHGCorrOff (double, int);
  /** Set the LG/HG correlation offset for a cell identified by row, column, layer, and module. */
  void   SetLGHGCorrOff (double, int, int, int, int);
  /** Set the HG/LG correlation slope for a cell. */
  void   SetHGLGCorr (double, int);
  /** Set the HG/LG correlation slope for a cell identified by row, column, layer, and module. */
  void   SetHGLGCorr (double, int, int, int, int);
  /** Set the HG/LG correlation offset for a cell. */
  void   SetHGLGCorrOff (double, int);
  /** Set the HG/LG correlation offset for a cell identified by row, column, layer, and module. */
  void   SetHGLGCorrOff (double, int, int, int, int);
  /** Set the bad-channel flag for a cell. */
  void   SetBadChannel (short, int);
  /** Set the bad-channel flag for a cell identified by row, column, layer, and module. */
  void   SetBadChannel (short, int, int, int, int);
  /** Set the ToA offset for a cell. */
  void   SetToAOff (double, int);
  /** Set the ToA offset for a cell identified by row, column, layer, and module. */
  void   SetToAOff (double, int, int, int, int);

  /** Return the run number. */
  int GetRunNumber(void);
  /** Return the pedestal run number. */
  int GetRunNumberPed(void);
  /** Return the MIP calibration run number. */
  int GetRunNumberMip(void);
  /** Return the begin-run timestamp. */
  const TTimeStamp* GetBeginRunTime(void) const;
  /** Return the pedestal begin-run timestamp. */
  const TTimeStamp* GetBeginRunTimePed(void) const;
  /** Return the MIP begin-run timestamp. */
  const TTimeStamp* GetBeginRunTimeMip(void) const;
  /** Return the Vov setting. */
  double GetVov(void);
  /** Return the Vop setting. */
  double GetVop(void);
  /** Check whether the bad-channel calibration map is available. */
  bool GetBCCalib(void);            // is bad channel map calculated

  /** Set the run number. */
  void SetRunNumber(int);           //
  /** Set the pedestal run number. */
  void SetRunNumberPed(int);           //
  /** Set the MIP calibration run number. */
  void SetRunNumberMip(int);           //
  /** Set the begin-run timestamp. */
  void SetBeginRunTime(TTimeStamp); //
  /** Set the pedestal begin-run timestamp. */
  void SetBeginRunTimePed(TTimeStamp); //
  /** Set the MIP begin-run timestamp. */
  void SetBeginRunTimeMip(TTimeStamp); //
  /** Set the Vop setting. */
  void SetVop(double);              //
  /** Set the Vov setting. */
  void SetVov(double);              //
  /** Set whether the bad-channel calibration map is available. */
  void SetBCCalib(bool);            // Bad channel map calculated

  /** Print a summary of the calibration state. */
  void PrintGlobalInfo();
  /** Print a detailed summary of the calibration state. */
  void PrintDetailedGlobalInfo();
  /** Write the calibration table to a text file. */
  void PrintCalibToFile( TString );
  /** Read calibration parameters from a text file. */
  void ReadCalibFromTextFile( TString, int);
  /** Read the external bad-channel map file. */
  void ReadExternalBadChannelMap(TString, int);
  /** Read the external ToA offset file. */
  void ReadExternalToAOffsets(TString, int);

  /** Return whether a given layer is enabled for analysis. */
  bool IsLayerEnabled(int, int) const;

  /** Return the beginning of the calibration map. */
  inline std::map<int, TileCalib>::const_iterator begin() {return CaloCalib.cbegin();};
  /** Return the end of the calibration map. */
  inline std::map<int, TileCalib>::const_iterator end()   {return CaloCalib.cend();};
  /** Search the calibration map for a cell ID. */
  inline std::map<int, TileCalib>::const_iterator find(int id)  {return CaloCalib.find(id);};
  
 private:
   
  std::map<int, TileCalib> CaloCalib;
  int RunNumber    = -1 ;
  int RunNumberPed = -1;
  int RunNumberMip = -1;
  TTimeStamp BeginRunTime;
  TTimeStamp BeginRunTimePed;
  TTimeStamp BeginRunTimeMip;
  double Vop;
  double Vov;
  bool BCcalc = false;
  ClassDef(Calib,5)
};


#endif
