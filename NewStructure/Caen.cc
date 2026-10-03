/**
 * @file Caen.cc
 * @brief Implementation of the CAEN-specific Tile data accessors.
 */

#include "Caen.h"
ClassImp(Caen)

/**
 * @brief Store the high-gain ADC value for this tile.
 * @param h High-gain ADC value.
 */
void Caen::SetADCHigh(int h){
  HG=h;
}

/**
 * @brief Store the low-gain ADC value for this tile.
 * @param l Low-gain ADC value.
 */
void Caen::SetADCLow(int l){
  LG=l;
}

/**
 * @brief Check whether the low-gain ADC is saturated.
 * @return true if the low-gain ADC exceeds the CAEN 12-bit limit.
 */
bool Caen::IsSaturatedADCLow(){
  if (LG > 4094) return true;
  else return false;
}

/**
 * @brief Check whether the high-gain ADC is saturated.
 * @return true if the high-gain ADC exceeds the CAEN 12-bit limit.
 */
bool Caen::IsSaturatedADCHigh(){
  if (HG > 4094) return true;
  else return false;
}
