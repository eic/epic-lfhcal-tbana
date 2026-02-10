#ifndef HGCROC_H
#define HGCROC_H

#include <vector>
#include "Tile.h"

class Hgcroc: public Tile {

 public:
  Hgcroc():Tile(ReadOut::Type::Hgcroc){}
  ~Hgcroc(){}
  std::vector<int> GetADCWaveform(void) const;
  std::vector<int> GetTOAWaveform(void) const;
  std::vector<int> GetTOTWaveform(void) const;
  int GetFirstTOASample (void);
  int GetCorrectedFirstTOASample(double);
  int GetFirstSampleAboveTh(void);
  int GetMaxSampleADC (void);
  int GetNsample(void) const ;
  double GetRawTOT(void) const;
  double GetCorrectedTOT(void) const;
  double GetRawTOA(void) const;
  double GetCorrectedTOA(void) const;
  
  int GetPedestal(void) const;

  void SetADCWaveform(std::vector<int>);
  void AppendWaveformADC(int);
  void ResetADCWaveformPoint(int, int);
  
  void SetTOAWaveform(std::vector<int>);
  void AppendWaveformTOA(int);
  void ResetTOAWaveformPoint(int, int);
  
  void SetTOTWaveform(std::vector<int>);
  void AppendWaveformTOT(int);
  void ResetTOTWaveformPoint(int, int);

  bool IsSaturatedADC(void) const;
  int IsBelowPed(double) const;
  void SetNsample(int);
  void SetCorrectedTOT(double);
  int SetCorrectedTOA(int);
  void SetPedestal(int);

  int GetLinearizedRawTOA();
  
  double GetIntegratedADC() {return integrated_adc;};  // Value from just ADC
  double GetIntegratedTOT() {return integrated_tot;};  // Value from just TOT
  double GetIntegratedValue() {return integrated_value;};  // Combined ADC and TOT
  double GetTOATimeResolution(void) {return (double)(25./1024);}; // return approximate time resolution in ns

  void SetIntegratedADC(double val) { integrated_adc = val; }
  void SetIntegratedTOT(double val) { integrated_tot = val; }
  void SetIntegratedValue(double val) { integrated_value = val; }

  void PrintWaveFormDebugInfo( double, double, double);
 protected:
  int Nsample;
  std::vector<int> adc_waveform;
  std::vector<int> toa_waveform;
  std::vector<int> tot_waveform;
  int TOA     = -10e5;              // linearized and corrected TOA (has to be set externally)
  int TOT     = -10e5;
  int pedestal = 0;

  // "finalized" values
  double integrated_adc = 0.;
  double integrated_tot = 0.;
  double integrated_value = 0.;
  
 private:

  ClassDef(Hgcroc,2)
};


#endif
