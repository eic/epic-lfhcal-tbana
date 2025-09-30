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
  int GetNsample(void) const ;
  double GetTOT(void) const;
  double GetTOA(void) const;
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

  void SetNsample(int);
  void SetTOT(double);
  void SetTOA(double);
  void SetPedestal(int);

  double GetIntegratedADC() {return integrated_adc;};  // Value from just ADC
  double GetIntegratedTOT() {return integrated_tot;};  // Value from just TOT
  double GetIntegratedValue() {return integrated_value;};  // Combined ADC and TOT

  void SetIntegratedADC(double val) { integrated_adc = val; }
  void SetIntegratedTOT(double val) { integrated_tot = val; }
  void SetIntegratedValue(double val) { integrated_value = val; }

  
 protected:
  int Nsample;
  std::vector<int> adc_waveform;
  std::vector<int> toa_waveform;
  std::vector<int> tot_waveform;
  int TOA;
  int TOT;
  int pedestal;

  // "finalized" values
  double integrated_adc = 0.;
  double integrated_tot = 0.;
  double integrated_value = 0.;
  
 private:

  ClassDef(Hgcroc,1)
};


#endif
