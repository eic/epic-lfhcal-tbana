/**
 * @file HGCROC.h
 * @brief Declaration of the HGCROC-specific Tile implementation.
 *
 * The Hgcroc class extends the generic Tile base class for HGCROC readout,
 * storing waveform samples, pedestal information and derived timing quantities.
 */

#ifndef HGCROC_H
#define HGCROC_H

#include <vector>
#include "Tile.h"

/**
 * @class Hgcroc
 * @brief Tile specialization for HGCROC readout channel data.
 *
 * This class stores ADC/TOT/TOA waveforms, pedestal information and derived
 * integrated quantities needed for trigger and calibration studies.
 */
class Hgcroc: public Tile {

 public:
 /**
  * @brief Default constructor.
  * Sets the Tile readout type to ReadOut::Type::Hgcroc.
  */
 Hgcroc():Tile(ReadOut::Type::Hgcroc){}

 /**
  * @brief Destructor.
  */
 ~Hgcroc(){}

 /** @brief Return the ADC waveform sample vector. */
 std::vector<int> GetADCWaveform(void) const;

 /** @brief Return the TOA waveform sample vector. */
 std::vector<int> GetTOAWaveform(void) const;

 /** @brief Return the TOT waveform sample vector. */
 std::vector<int> GetTOTWaveform(void) const;

 /** @brief Return the first sample index with a TOA above zero. */
 int GetFirstTOASample (void);

 /**
  * @brief Return the corrected first TOA sample, optionally shifted by an offset.
  * @param offset Offset applied to the TOA calibration.
  * @return Index of the first valid TOA sample.
  */
 int GetCorrectedFirstTOASample(double);

 /** @brief Return the first ADC sample above threshold. */
 int GetFirstSampleAboveTh(void);

 /** @brief Return the index of the maximum ADC sample. */
 int GetMaxSampleADC (void);

 /** @brief Return the number of waveform samples. */
 int GetNsample(void) const ;

 /** @brief Return the raw TOT value from the first firing TOT sample. */
 double GetRawTOT(void) const;

 /** @brief Return the maximum TOT value in the waveform. */
 double GetMaxTOT(void) const;

 /**
  * @brief Return the mean ADC after subtracting a pedestal estimate.
  * @param pedestal Pedestal value used for subtraction.
  * @return Mean pedestal-subtracted ADC.
  */
 double GetMeanADC(double pedestal) const;

 /** @brief Return the corrected TOT value. */
 double GetCorrectedTOT(void) const;

 /** @brief Return the raw TOA value from the first non-zero sample. */
 double GetRawTOA(void) const;

 /** @brief Return the corrected TOA value. */
 double GetCorrectedTOA(void) const;
   
 /** @brief Return the pedestal value associated with this channel. */
 int GetPedestal(void) const;

 /** @brief Set the complete ADC waveform vector. */
 void SetADCWaveform(std::vector<int>);

 /** @brief Append a single ADC value to the waveform vector. */
 void AppendWaveformADC(int);

 /** @brief Reset a waveform ADC sample at a given index. */
 void ResetADCWaveformPoint(int, int);
   
 /** @brief Set the complete TOA waveform vector. */
 void SetTOAWaveform(std::vector<int>);

 /** @brief Append a single TOA value to the waveform vector. */
 void AppendWaveformTOA(int);

 /** @brief Reset a TOA waveform sample at a given index. */
 void ResetTOAWaveformPoint(int, int);
   
 /** @brief Set the complete TOT waveform vector. */
 void SetTOTWaveform(std::vector<int>);

 /** @brief Append a single TOT value to the waveform vector. */
 void AppendWaveformTOT(int);

 /** @brief Reset a TOT waveform sample at a given index. */
 void ResetTOTWaveformPoint(int, int);
 
 /** @brief Check whether any ADC sample exceeds the hardware saturation threshold. */
 bool IsSaturatedADC(void) const;

 /** @brief Check whether any TOT sample exceeds the hardware saturation threshold. */
 bool IsSaturatedTOT(void) const;

 /**
  * @brief Find the first ADC sample below a pedestal-based threshold.
  * @param pedSig Pedestal sigma value used for thresholding.
  * @return Index of the first sample below threshold or -1 if none is found.
  */
 int IsBelowPed(double) const;

 /** @brief Set the number of waveform samples. */
 void SetNsample(int);

 /** @brief Set the corrected TOT value. */
 void SetCorrectedTOT(double);

 /**
  * @brief Correct the TOA using the supplied offset.
  * @param offset Timing offset for correction.
  * @return Number of samples associated with the corrected TOA.
  */
 int SetCorrectedTOA(int);

 /** @brief Set the pedestal value for this channel. */
 void SetPedestal(int);

 /** @brief Return a linearized raw TOA quantity. */
 int GetLinearizedRawTOA();
   
 /** @brief Return the integrated ADC value stored in the finalized waveform summary. */
 double GetIntegratedADC() {return integrated_adc;};

 /** @brief Return the integrated TOT value stored in the finalized waveform summary. */
 double GetIntegratedTOT() {return integrated_tot;};

 /** @brief Return the combined integrated value stored in the finalized waveform summary. */
 double GetIntegratedValue() {return integrated_value;};

 /** @brief Return the approximate TOA time resolution in nanoseconds. */
 double GetTOATimeResolution(void) {return (double)(25./1024);};

 /** @brief Store the integrated ADC sum. */
 void SetIntegratedADC(double val) { integrated_adc = val; }

 /** @brief Store the integrated TOT sum. */
 void SetIntegratedTOT(double val) { integrated_tot = val; }

 /** @brief Store the combined integrated value. */
 void SetIntegratedValue(double val) { integrated_value = val; }

 /**
  * @brief Print debug information about the waveform contents and derived values.
  * @param pedMeanH Mean high-gain pedestal.
  * @param pedMeanL Mean low-gain pedestal.
  * @param pedSig Pedestal sigma.
  */
 void PrintWaveFormDebugInfo( double, double, double);
 protected:
 int Nsample; ///< Number of waveform samples
 std::vector<int> adc_waveform; ///< ADC waveform samples
 std::vector<int> toa_waveform; ///< TOA waveform samples
 std::vector<int> tot_waveform; ///< TOT waveform samples
 int TOA     = -10e5;              ///< Corrected TOA value
 int TOT     = -10e5;              ///< Corrected TOT value
 int pedestal = 0;                 ///< Channel pedestal

 // "finalized" values
 double integrated_adc = 0.; ///< Integrated ADC value
 double integrated_tot = 0.; ///< Integrated TOT value
 double integrated_value = 0.; ///< Combined integrated value
   
 private:

 ClassDef(Hgcroc,2)
};


#endif
