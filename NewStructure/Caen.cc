#include "Caen.h"
ClassImp(Caen)
void Caen::SetADCHigh(int h){
  HG=h;
}

void Caen::SetADCLow(int l){
  LG=l;
}

bool Caen::IsSaturatedADCLow(){
  if (LG > 4094) return true;
  else return false;
}

bool Caen::IsSaturatedADCHigh(){
  if (HG > 4094) return true;
  else return false;
}
