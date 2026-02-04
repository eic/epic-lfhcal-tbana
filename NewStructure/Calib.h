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
  Calib() {}
  virtual ~Calib() {}
    
  double GetPedestalMeanH (int /**/) const;
  double GetPedestalMeanL (int /**/) const;
  double GetPedestalMeanH (int /**/, int /**/, int /**/, int /**/) const;
  double GetPedestalMeanL (int /**/, int /**/, int /**/, int /**/) const;
  double GetPedestalSigH (int /**/) const;
  double GetPedestalSigL (int /**/) const;
  double GetPedestalSigH (int /**/, int /**/, int /**/, int /**/) const;
  double GetPedestalSigL (int /**/, int /**/, int /**/, int /**/) const;
  double GetScaleHigh(int /**/) const;
  double GetScaleHigh(int /**/, int /**/, int /**/, int /**/) const;
  double GetScaleWidthHigh(int /**/) const;
  double GetScaleWidthHigh(int /**/, int /**/, int /**/, int /**/) const;
  double GetCalcScaleLow (int /**/) const;
  double GetCalcScaleLow (int /**/, int /**/, int /**/, int /**/) const;
  double GetScaleLow (int /**/) const;
  double GetScaleLow (int /**/, int /**/, int /**/, int /**/) const;
  double GetScaleWidthLow (int /**/) const;
  double GetScaleWidthLow (int /**/, int /**/, int /**/, int /**/) const;
  double GetLGHGCorr (int /**/) const;
  double GetLGHGCorr (int /**/, int /**/, int /**/, int /**/) const;
  double GetLGHGCorrOff (int /**/) const;
  double GetLGHGCorrOff (int /**/, int /**/, int /**/, int /**/) const;
  double GetHGLGCorr (int /**/) const;
  double GetHGLGCorr (int /**/, int /**/, int /**/, int /**/) const;
  double GetHGLGCorrOff (int /**/) const;
  double GetHGLGCorrOff (int /**/, int /**/, int /**/, int /**/) const;
  double GetToAOff (int /**/) const;
  double GetToAOff (int /**/, int /**/, int /**/, int /**/) const;
  
  double GetAveragePedestalMeanHigh() const;
  double GetAveragePedestalSigHigh() const;
  double GetAveragePedestalMeanLow() const;
  double GetAveragePedestalSigLow() const;
  double GetAverageScaleHigh() const;
  double GetAverageScaleHigh(int &) const;
  double GetAverageScaleWidthHigh() const;
  double GetAverageScaleLow() const;
  double GetAverageScaleWidthLow() const;
  double GetAverageHGLGCorr() const;
  double GetAverageHGLGCorrOff() const;
  double GetAverageLGHGCorr() const;
  double GetAverageLGHGCorrOff() const;
  
  int GetNumberOfChannelsWithBCflag (short ) const;
  short GetBadChannel(int /**/) const;
  short GetBadChannel(int /**/, int /**/, int /**/, int /**/) const;
  
  TileCalib* GetTileCalib(int /**/);
  TileCalib* GetTileCalib(int /**/, int /**/, int /**/, int /**/);

  void   SetPedestalMeanH (double, int);
  void   SetPedestalMeanL (double, int);
  void   SetPedestalMeanH (double, int, int, int, int);
  void   SetPedestalMeanL (double, int, int, int, int);
  void   SetPedestalSigH (double, int);
  void   SetPedestalSigL (double, int);
  void   SetPedestalSigH (double, int, int, int, int);
  void   SetPedestalSigL (double, int, int, int, int);
  void   SetScaleHigh(double, int);
  void   SetScaleHigh(double, int, int, int, int);
  void   SetScaleWidthHigh(double, int);
  void   SetScaleWidthHigh(double, int, int, int, int);
  void   SetScaleLow (double, int);
  void   SetScaleLow (double, int, int, int, int);
  void   SetScaleWidthLow (double, int);
  void   SetScaleWidthLow (double, int, int, int, int);
  void   SetLGHGCorr (double, int);
  void   SetLGHGCorr (double, int, int, int, int);
  void   SetLGHGCorrOff (double, int);
  void   SetLGHGCorrOff (double, int, int, int, int);
  void   SetHGLGCorr (double, int);
  void   SetHGLGCorr (double, int, int, int, int);
  void   SetHGLGCorrOff (double, int);
  void   SetHGLGCorrOff (double, int, int, int, int);
  void   SetBadChannel (short, int);
  void   SetBadChannel (short, int, int, int, int);
  void   SetToAOff (double, int);
  void   SetToAOff (double, int, int, int, int);
  
  int GetRunNumber(void);
  int GetRunNumberPed(void);
  int GetRunNumberMip(void);
  const TTimeStamp* GetBeginRunTime(void) const;
  const TTimeStamp* GetBeginRunTimePed(void) const;
  const TTimeStamp* GetBeginRunTimeMip(void) const;
  double GetVov(void);
  double GetVop(void);
  bool GetBCCalib(void);            // is bad channel map calculated

  void SetRunNumber(int);           // 
  void SetRunNumberPed(int);           // 
  void SetRunNumberMip(int);           // 
  void SetBeginRunTime(TTimeStamp); // 
  void SetBeginRunTimePed(TTimeStamp); // 
  void SetBeginRunTimeMip(TTimeStamp); // 
  void SetVop(double);              // 
  void SetVov(double);              // 
  void SetBCCalib(bool);            // Bad channel map calculated

  void PrintGlobalInfo();
  void PrintCalibToFile( TString );
  void ReadCalibFromTextFile( TString, int);
  void ReadExternalBadChannelMap(TString, int);
  void ReadExternalToAOffsets(TString, int);
  
  bool IsLayerEnabled(int, int) const;
  
  inline std::map<int, TileCalib>::const_iterator begin() {return CaloCalib.cbegin();};
  inline std::map<int, TileCalib>::const_iterator end()   {return CaloCalib.cend();};
  
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
