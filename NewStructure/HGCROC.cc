#include "HGCROC.h"
#include <cassert>
#include <iostream>

ClassImp(Hgcroc)
std::vector<int> Hgcroc::GetADCWaveform(void) const{
  return adc_waveform;
}

bool Hgcroc::IsSaturatedADC() const{
  for (int k = 0; k < (int)adc_waveform.size(); k++ ){
    if (adc_waveform.at(k) > 1022)
      return true;
  }
  return false;
}

int Hgcroc::IsBelowPed(double pedSig) const{
  for (int k = 0; k < (int)adc_waveform.size(); k++ ){
    if ( adc_waveform.at(k) < (pedestal - pedSig))
      return k;
  }
  return -1;
}


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

std::vector<int> Hgcroc::GetTOAWaveform(void) const{
  return toa_waveform;
}

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


std::vector<int> Hgcroc::GetTOTWaveform(void) const{
  return tot_waveform;
}

int Hgcroc::GetNsample(void) const{
  return Nsample;
}

double Hgcroc::GetCorrectedTOT(void) const{
  return TOT;
}

// Gets first TOT fired for the waveform 
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

// Gets first TOA fired for the waveform 
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

double Hgcroc::GetCorrectedTOA(void) const{
  return TOA;
}

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


int Hgcroc::GetPedestal(void) const{
  return pedestal;
}

void Hgcroc::SetADCWaveform(std::vector<int> v){
  adc_waveform=v;
}

void Hgcroc::AppendWaveformADC(int a){
  adc_waveform.push_back(a);
}

void Hgcroc::ResetADCWaveformPoint(int s, int a){
  assert(0<=s && s<(int)adc_waveform.size());
  adc_waveform.at(s)=a;
}

void Hgcroc::SetTOAWaveform(std::vector<int> v){
  toa_waveform=v;
}

void Hgcroc::AppendWaveformTOA(int a){
  toa_waveform.push_back(a);
}

void Hgcroc::ResetTOAWaveformPoint(int s, int a){
  assert(0<=s && s<(int)toa_waveform.size());
  toa_waveform.at(s)=a;
}

void Hgcroc::SetTOTWaveform(std::vector<int> v){
  tot_waveform=v;
}

void Hgcroc::AppendWaveformTOT(int a){
  tot_waveform.push_back(a);
}

void Hgcroc::ResetTOTWaveformPoint(int s, int a){
  assert(0<=s && s<(int)tot_waveform.size());
  tot_waveform.at(s)=a;
}

void Hgcroc::SetNsample(int n){
  Nsample=n;
}

void Hgcroc::SetCorrectedTOT(double tot){
  TOT=tot;
}

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

int Hgcroc::GetLinearizedRawTOA(){
  int rawTOA    = (int)GetRawTOA();
  if (rawTOA == 0)
    return -10e5;
  int nSampTOA  = (int)GetFirstTOASample();
  
  return (-1)*nSampTOA*1024-rawTOA;
}

void Hgcroc::SetPedestal(int ped){
  pedestal=ped;
}

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
