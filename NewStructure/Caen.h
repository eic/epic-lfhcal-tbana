#ifndef CAEN_H
#define CAEN_H

#include "Tile.h"

class Caen: public Tile{

 public:
  Caen():Tile(ReadOut::Type::Caen){}
  ~Caen(){}
  int GetADCHigh()const {return HG;};
  int GetADCLow()const {return LG;};
  void SetADCHigh(int);
  void SetADCLow(int);
  bool IsSaturatedADCHigh();
  bool IsSaturatedADCLow();
 private:
  double HG;
  double LG;

  ClassDef(Caen,1)
};


#endif
