#include "HGCROC.h"
#include <cassert>
#include <iostream>

ClassImp(Hgcroc)
std::vector<int> Hgcroc::GetADCWaveform(void) const{
  return adc_waveform;
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

double Hgcroc::GetTOT(void) const{
  return TOT;
}

double Hgcroc::GetTOA(void) const{
  return TOA;
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

void Hgcroc::SetTOT(double tot){
  TOT=tot;
}

void Hgcroc::SetTOA(double toa){
  TOA=toa;
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
  std::cout << " integ: "<<GetIntegratedADC() <<"\t"<< GetTOT() << "\t" << GetTOA() << "\t nTOA = " << GetFirstTOASample() << "\t nTh = " << GetFirstSampleAboveTh() << "\t nMax = " << GetMaxSampleADC()  << std::endl;

}
