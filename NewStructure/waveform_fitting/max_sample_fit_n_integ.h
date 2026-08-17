// Gets the magnitude simply from n samples surrounging the ADC_max subtracting equivalently the number of pedestals

#pragma once

#include "waveform_fit_base.h"

#include <vector>

class max_sample_fit_n_integ : public waveform_fit_base {
public:
    max_sample_fit_n_integ();
    max_sample_fit_n_integ( int );
    ~max_sample_fit_n_integ();

    void fit()                              override;
    void fit_with_average_ped(double ped)   override;
    int get_pedestal()                      override;
    
    
private:
    int max_sample;           // maximum sample nr
    double integ_nSample;     // integral of n sample
    double max_sample_value;  // The maximum sample value in the waveform
    double pedestal_value;    // The pedestal value, which is the average of the first 10 samples
    int nSample               = 3; // neighboring samples to be integrated, this should be uneven!! i.e. 3 or 5
};
