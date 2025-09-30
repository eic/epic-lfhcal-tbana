// Gets the magnitude simply from ADC_max - ADC_ped

#pragma once

#include "waveform_fit_base.h"

#include <vector>

class max_sample_fit : public waveform_fit_base {
public:
    max_sample_fit();
    ~max_sample_fit();

    void fit()                              override;
    void fit_with_average_ped(double ped)   override;
    int get_pedestal()                      override;

private:
    double max_sample_value;  // The maximum sample value in the waveform
    double pedestal_value;    // The pedestal value, which is the average of the first 10 samples
};
