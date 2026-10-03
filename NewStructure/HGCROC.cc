/**
 * @file HGCROC.cc
 * @brief Implementation of the HGCROC readout tile helpers.
 */

#include "HGCROC.h"
#include <cassert>
#include <iostream>

ClassImp(Hgcroc)

/**
 * @brief Return the ADC waveform stored in this HGCROC tile.
 * @return ADC sample vector.
 */
std::vector<int> Hgcroc::GetADCWaveform(void) const{
  return adc_waveform;
}

/**
 * @brief Check whether any ADC waveform sample exceeds the saturation threshold.
 * @return true if an ADC sample is saturated.
 */
bool Hgcroc::IsSaturatedADC() const{
  for (int k = 0; k < (int)adc_waveform.size(); k++ ){
    if (adc_waveform.at(k) > 1022)
      return true;
  }
  return false;
}

/**
 * @brief Check whether any TOT waveform sample exceeds the saturation threshold.
 * @return true if a TOT sample is saturated.
 */
bool Hgcroc::IsSaturatedTOT() const{
  for (int k = 0; k < (int)tot_waveform.size(); k++ ){
    if (tot_waveform.at(k) > 4090)
      return true;
  }
  return false;
}

/**
 * @brief Find the first ADC sample below a pedestal-based threshold.
 * @param pedSig Pedestal sigma used for thresholding.
 * @return Index of the first out-of-threshold sample or -1.
 */
int Hgcroc::IsBelowPed(double pedSig) const{
  for (int k = 0; k < (int)adc_waveform.size(); k++ ){
    if ( adc_waveform.at(k) < (pedestal - pedSig))
      return k;
  }
  return -1;
}

/**
 * @brief Find the waveform sample index with the largest ADC value.
 * @return Index of the maximum ADC sample.
 */
int Hgcroc::GetMaxSampleADC (void){
  Double_t maxADC = -1000;
  int nMaxADC   = 0;
  for (int k = 0; k < (int)adc_waveform.size(); k++ ){
    if (maxADC < adc_waveform.at(k)){
      maxADC  = adc_waveform.at(k);
      nMaxADC = k;
    }
  }
  return nMaxADC;
}

/**
 * @brief Return the index of the first ADC sample above threshold.
 * @return Index of the first suitable waveform sample.
 */
int Hgcroc::GetFirstSampleAboveTh (void){
  int nFirst   = 0;
  for (int k = 0; k < (int)adc_waveform.size(); k++ ){
    if (adc_waveform.at(k) > GetPedestal() +10){
      nFirst = k;
      break;
    }
  }
  return nFirst;
}

/**
 * @brief Return the TOA waveform stored in this tile.
 * @return TOA sample vector.
 */
std::vector<int> Hgcroc::GetTOAWaveform(void) const{
  return toa_waveform;
}

/**
 * @brief Return the first sample index carrying a non-zero TOA value.
 * @return TOA sample index.
 */
int Hgcroc::GetFirstTOASample (void){
  int nSampTOA  = 0;
  for (int k = 0; k < (int)toa_waveform.size(); k++ ){
    if (toa_waveform.at(k) > 0){
      nSampTOA = k;
      break;
    }
  }
  return nSampTOA;
}

/**
 * @brief Return the TOT waveform stored in this tile.
 * @return TOT sample vector.
 */
std::vector<int> Hgcroc::GetTOTWaveform(void) const{
  return tot_waveform;
}

/**
 * @brief Return the number of waveform samples.
 * @return Count of samples.
 */
int Hgcroc::GetNsample(void) const{
  return Nsample;
}

/**
 * @brief Return the corrected TOT value.
 * @return TOT value.
 */
double Hgcroc::GetCorrectedTOT(void) const{
  return TOT;
}

/**
 * @brief Return the first non-zero TOT sample in the waveform.
 * @return Raw TOT value.
 */
double Hgcroc::GetRawTOT(void) const{
  double tot = 0;
  for (int k = 0; k < (int)tot_waveform.size(); k++ ){
    if (tot_waveform.at(k) > 0){
      tot = tot_waveform.at(k);
      break;
    }
  }
  return tot;
}

/**
 * @brief Return the maximum TOT observed in the waveform.
 * @return Maximum TOT value.
 */
double Hgcroc::GetMaxTOT(void) const{
  double tot = 0;
  for (int k = 0; k < (int)tot_waveform.size(); k++ ){
    if (tot_waveform.at(k) > tot){
      tot = tot_waveform.at(k);
    }
  }
  return tot;
}

/**
 * @brief Return the mean ADC after subtracting the supplied pedestal.
 * @param pedestal Pedestal value used for subtraction.
 * @return Mean pedestal-subtracted ADC.
 */
double Hgcroc::GetMeanADC(double pedestal) const{
  if (adc_waveform.size() == 0) return 0;
  double sum = 0;
  for (int k = 0; k < (int)adc_waveform.size(); k++ ){
    sum += (adc_waveform.at(k) - pedestal);
  }
  return sum / (double)adc_waveform.size();
}

/**
 * @brief Return the first non-zero TOA sample in the waveform.
 * @return Raw TOA value.
 */
double Hgcroc::GetRawTOA(void) const{
  double toa = 0;
  for (int k = 0; k < (int)toa_waveform.size(); k++ ){
    if (toa_waveform.at(k) > 0){
      toa = toa_waveform.at(k);
      break;
    }
  }
  return toa;
}

/**
 * @brief Return the corrected TOA value.
 * @return Corrected TOA.
 */
double Hgcroc::GetCorrectedTOA(void) const{
  return TOA;
}

/**
 * @brief Return the corrected first TOA sample, adjusted by an offset if needed.
 * @param toAOffset Offset used to align TOA sample timing.
 * @return Index of the corrected first TOA sample.
 */
int Hgcroc::GetCorrectedFirstTOASample(double toAOffset) {
  int nSampTOA  = GetFirstTOASample();
  int rawTOA    = (int)GetRawTOA();
  // only calculate if the waveform actually had an intrinsic TOA
  if (rawTOA > 1 && toAOffset != -1000.){
    if ((rawTOA-toAOffset) < 0)
      nSampTOA++; 
  }
  return nSampTOA;
}

/**
 * @brief Return the pedestal value stored in the tile.
 * @return Pedestal value.
 */
int Hgcroc::GetPedestal(void) const{
  return pedestal;
}

/**
 * @brief Set the complete ADC waveform.
 * @param v Vector of ADC samples.
 */
void Hgcroc::SetADCWaveform(std::vector<int> v){
  adc_waveform=v;
}

/**
 * @brief Append one ADC sample to the waveform.
 * @param a ADC sample value.
 */
void Hgcroc::AppendWaveformADC(int a){
  adc_waveform.push_back(a);
}

/**
 * @brief Reset a single ADC sample at a given index.
 * @param s Sample index.
 * @param a New ADC value.
 */
void Hgcroc::ResetADCWaveformPoint(int s, int a){
  assert(0<=s && s<(int)adc_waveform.size());
  adc_waveform.at(s)=a;
}

/**
 * @brief Set the complete TOA waveform.
 * @param v Vector of TOA samples.
 */
void Hgcroc::SetTOAWaveform(std::vector<int> v){
  toa_waveform=v;
}

/**
 * @brief Append one TOA sample to the waveform.
 * @param a TOA sample value.
 */
void Hgcroc::AppendWaveformTOA(int a){
  toa_waveform.push_back(a);
}

/**
 * @brief Reset a single TOA sample at a given index.
 * @param s Sample index.
 * @param a New TOA value.
 */
void Hgcroc::ResetTOAWaveformPoint(int s, int a){
  assert(0<=s && s<(int)toa_waveform.size());
  toa_waveform.at(s)=a;
}

/**
 * @brief Set the complete TOT waveform.
 * @param v Vector of TOT samples.
 */
void Hgcroc::SetTOTWaveform(std::vector<int> v){
  tot_waveform=v;
}

/**
 * @brief Append one TOT sample to the waveform.
 * @param a TOT sample value.
 */
void Hgcroc::AppendWaveformTOT(int a){
  tot_waveform.push_back(a);
}

/**
 * @brief Reset a single TOT sample at a given index.
 * @param s Sample index.
 * @param a New TOT value.
 */
void Hgcroc::ResetTOTWaveformPoint(int s, int a){
  assert(0<=s && s<(int)tot_waveform.size());
  tot_waveform.at(s)=a;
}

/**
 * @brief Set the number of waveform samples.
 * @param n Sample count.
 */
void Hgcroc::SetNsample(int n){
  Nsample=n;
}

/**
 * @brief Set the corrected TOT value.
 * @param tot Corrected TOT value.
 */
void Hgcroc::SetCorrectedTOT(double tot){
  TOT=tot;
}

/**
 * @brief Correct the TOA using the supplied offset.
 * @param offset Timing offset for correction.
 * @return Sample index associated with the corrected TOA.
 */
int Hgcroc::SetCorrectedTOA(int offset){
  int rawTOA    = (int)GetRawTOA();
  // only calculate if the waveform actually had an intrinsic TOA
  if (rawTOA > 1){
    int toacorr   = (rawTOA-offset)%1024;
    int nSampTOA  = (int)GetFirstTOASample();
    if (rawTOA-offset < 0)
      nSampTOA++;
    double toacorrf  = double((-1)*nSampTOA*1024-toacorr);
    TOA=toacorrf;
    return nSampTOA;
  // otherwise return default values
  } else {
    TOA = -10e5;
    return 0;
  }
  return 0;
}

/**
 * @brief Return a linearized version of the raw TOA value.
 * @return Linearized raw TOA value.
 */
int Hgcroc::GetLinearizedRawTOA(){
  int rawTOA    = (int)GetRawTOA();
  if (rawTOA == 0)
    return -10e5;
  int nSampTOA  = (int)GetFirstTOASample();
  
  return (-1)*nSampTOA*1024-rawTOA;
}

/**
 * @brief Set the pedestal value for this tile.
 * @param ped Pedestal value.
 */
void Hgcroc::SetPedestal(int ped){
  pedestal=ped;
}

/**
 * @brief Print waveform debug information for calibration and diagnostics.
 * @param pedMeanH Mean high-gain pedestal.
 * @param pedMeanL Mean low-gain pedestal.
 * @param pedSig Pedestal sigma.
 */
void Hgcroc::PrintWaveFormDebugInfo( double pedMeanH, double pedMeanL, double pedSig){
    Setup* setupT = Setup::GetInstance();
    int layer     = setupT->GetLayer(CellID);
    int chInLayer = setupT->GetChannelInLayer(CellID);          
    int roCh      = setupT->GetROchannel(CellID);
    std::cout << "Cell ID:" << CellID<< "\t" << layer <<"\t" << chInLayer << "\t RO channel:\t" << roCh << "\t" << pedMeanH << "\t" << pedMeanL << "\t" << pedSig;
    std::cout << "\n \tADC-wave " ;
    for (int k = 0; k < (int)adc_waveform.size(); k++ ){
      std::cout << adc_waveform.at(k) << "\t" ;
    }
    std::cout << "\n \tTOT-Wave ";
    for (int k = 0; k < (int)tot_waveform.size(); k++ ){
      std::cout << tot_waveform.at(k) << "\t" ;
    }
    std::cout << "\n \tTOA-Wave ";
    for (int k = 0; k < (int)toa_waveform.size(); k++ ){
      std::cout << toa_waveform.at(k) << "\t" ;
    }
  std::cout <<"\n\t\t\t";
  for (int k = 0; k < (int)toa_waveform.size(); k++ )
    std::cout <<"\t";  
  std::cout << " integ: "<<GetIntegratedADC() <<"\t"<< GetRawTOT() << "\t" << GetRawTOA() << "\t nTOA = " << GetFirstTOASample() << "\t nTh = " << GetFirstSampleAboveTh() << "\t nMax = " << GetMaxSampleADC()  << std::endl;

}
