/**
 * @file Caen.h
 * @brief Declaration of the Caen-specific detector tile implementation.
 *
 * The Caen class extends the generic Tile base class with the high-gain and
 * low-gain ADC values used by the CAEN readout chain.
 */

#ifndef CAEN_H
#define CAEN_H

#include "Tile.h"

/**
 * @class Caen
 * @brief Tile specialization for CAEN readout channels.
 *
 * This class stores the two ADC values associated with each CAEN channel and
 * provides convenience accessors for checking signal saturation.
 */
class Caen: public Tile{

 public:
 /**
  * @brief Default constructor.
  * Sets the Tile readout type to ReadOut::Type::Caen.
  */
 Caen():Tile(ReadOut::Type::Caen){}

 /**
  * @brief Destructor.
  */
 ~Caen(){}

 /**
  * @brief Return the high-gain ADC value.
  * @return High-gain ADC count.
  */
 int GetADCHigh()const {return HG;};

 /**
  * @brief Return the low-gain ADC value.
  * @return Low-gain ADC count.
  */
 int GetADCLow()const {return LG;};

 /**
  * @brief Set the high-gain ADC value.
  * @param h New high-gain ADC value.
  */
 void SetADCHigh(int);

 /**
  * @brief Set the low-gain ADC value.
  * @param l New low-gain ADC value.
  */
 void SetADCLow(int);

 /**
  * @brief Check whether the high-gain ADC is saturated.
  * @return true if the high-gain ADC exceeds the CAEN saturation threshold.
  */
 bool IsSaturatedADCHigh();

 /**
  * @brief Check whether the low-gain ADC is saturated.
  * @return true if the low-gain ADC exceeds the CAEN saturation threshold.
  */
 bool IsSaturatedADCLow();
 private:
 double HG; ///< High-gain ADC value
 double LG; ///< Low-gain ADC value

 ClassDef(Caen,1)
};


#endif
