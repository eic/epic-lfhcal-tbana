#include "waveform_fit_base.h"
#include "max_sample_fit.h"

#include <iostream>

max_sample_fit::max_sample_fit() : waveform_fit_base{} {
    max_sample_value = 0;
    pedestal_value = 0;

    saturated = false;
    stale = true;
}

max_sample_fit::~max_sample_fit() {
}

void max_sample_fit::fit() {
    if (waveform.empty()) {
        std::cerr << "Waveform has not been set" << std::endl;
        return;
    }
    pedestal_value      = waveform[0];
    max_sample_value    = waveform[0];
    for (size_t i = 0; i < waveform.size(); ++i) {
        if (waveform[i] > max_sample_value) {
            max_sample_value = waveform[i];
        }
        if (waveform[i] > 1000) {  // Check for saturation. TODO: Make this a parameter
            saturated = true;
        }
    }
    E = max_sample_value - pedestal_value;  // The energy is the maximum sample value minus the pedestal
    fit_ndf = -1;
    fit_chi2 = -1;  // Not applicable for max sample fit
    stale = false;
}

void max_sample_fit::fit_with_average_ped(double ped) {
    if (waveform.empty()) {
        std::cerr << "Waveform has not been set" << std::endl;
        return;
    }
    pedestal_value      = ped;
    max_sample_value    = ped;
    for (size_t i = 0; i < waveform.size(); ++i) {
        if (waveform[i] > max_sample_value) {
            max_sample_value = waveform[i];
        }
        if (waveform[i] > 1000) {  // Check for saturation. TODO: Make this a parameter
            saturated = true;
        }
    }
    E = max_sample_value - pedestal_value;  // The energy is the maximum sample value minus the average pedestal
    fit_ndf = -1;
    fit_chi2 = -1;  // Not applicable for max sample fit
    stale = false;
}


int max_sample_fit::get_pedestal() {
    if (stale) {
        return -1;
    }
    return pedestal_value;
}
