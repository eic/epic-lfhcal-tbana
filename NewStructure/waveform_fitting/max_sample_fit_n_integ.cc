#include "waveform_fit_base.h"
#include "max_sample_fit_n_integ.h"

#include <iostream>

max_sample_fit_n_integ::max_sample_fit_n_integ() : waveform_fit_base{} {
    max_sample_value  = 0;
    pedestal_value    = 0;
    max_sample        = 0;
    integ_nSample     = 0;
    
    saturated         = false;
    stale             = true;
}

max_sample_fit_n_integ::max_sample_fit_n_integ( int n) : waveform_fit_base{} {
    max_sample_value  = 0;
    pedestal_value    = 0;
    max_sample        = 0;
    integ_nSample     = 0;
    
    
    saturated         = false;
    stale             = true;
    nSample           = n;
    if (!( nSample == 3 || nSample == 5)) {
      std::cerr << "Waveform has not been set" << std::endl;
      return;
    }
}

max_sample_fit_n_integ::~max_sample_fit_n_integ() {
}

void max_sample_fit_n_integ::fit() {
    if (waveform.empty()) {
        std::cerr << "Waveform has not been set" << std::endl;
        return;
    }
    pedestal_value      = waveform[0];
    max_sample_value    = waveform[0];
    max_sample          = 0;
    for (size_t i = 0; i < waveform.size(); ++i) {
        if (waveform[i] > max_sample_value) {
            max_sample_value  = waveform[i];
            max_sample        = i;
        }
        if (waveform[i] > 1000) {  // Check for saturation. TODO: Make this a parameter
            saturated = true;
        }
    }
    if (nSample == 3){
      integ_nSample = max_sample_value - pedestal_value;
      if (max_sample > 0 )
        integ_nSample = integ_nSample+ (waveform[max_sample-1] - pedestal_value);
      if (max_sample < waveform.size()-1)
        integ_nSample = integ_nSample+ (waveform[max_sample+1] - pedestal_value);
    } else if (nSample == 5){
      integ_nSample = max_sample_value - pedestal_value;
      if (max_sample > 1 )
        integ_nSample = integ_nSample+ (waveform[max_sample-2] - pedestal_value);
      if (max_sample > 0 )
        integ_nSample = integ_nSample+ (waveform[max_sample-1] - pedestal_value);
      if (max_sample < waveform.size()-1)
        integ_nSample = integ_nSample+ (waveform[max_sample+1] - pedestal_value);
      if (max_sample < waveform.size()-2)
        integ_nSample = integ_nSample+ (waveform[max_sample+2] - pedestal_value);
    }
    E         = integ_nSample;  // The energy is the maximum sample value minus the pedestal
    fit_ndf   = -1;
    fit_chi2  = -1;  // Not applicable for max sample fit
    stale     = false;
}

void max_sample_fit_n_integ::fit_with_average_ped(double ped) {
    if (waveform.empty()) {
        std::cerr << "Waveform has not been set" << std::endl;
        return;
    }
    pedestal_value      = ped;
    max_sample_value    = ped;
    max_sample          = 0;
    for (size_t i = 0; i < waveform.size(); ++i) {
        if (waveform[i] > max_sample_value) {
            max_sample_value  = waveform[i];
            max_sample        = i;
        }
        if (waveform[i] > 1000) {  // Check for saturation. TODO: Make this a parameter
            saturated = true;
        }
    }
    if (nSample == 3){
      integ_nSample = max_sample_value - pedestal_value;
      if (max_sample > 0 )
        integ_nSample = integ_nSample+ (waveform[max_sample-1] - pedestal_value);
      if (max_sample < waveform.size()-1)
        integ_nSample = integ_nSample+ (waveform[max_sample+1] - pedestal_value);
    } else if (nSample == 5){
      integ_nSample = max_sample_value - pedestal_value;
      if (max_sample > 1 )
        integ_nSample = integ_nSample+ (waveform[max_sample-2] - pedestal_value);
      if (max_sample > 0 )
        integ_nSample = integ_nSample+ (waveform[max_sample-1] - pedestal_value);
      if (max_sample < waveform.size()-1)
        integ_nSample = integ_nSample+ (waveform[max_sample+1] - pedestal_value);
      if (max_sample < waveform.size()-2)
        integ_nSample = integ_nSample+ (waveform[max_sample+2] - pedestal_value);
    } 

    
    E         = integ_nSample;  // The energy is the maximum sample value minus the average pedestal
    fit_ndf   = -1;
    fit_chi2  = -1;  // Not applicable for max sample fit
    stale     = false;
}


int max_sample_fit_n_integ::get_pedestal() {
    if (stale) {
        return -1;
    }
    return pedestal_value;
}
