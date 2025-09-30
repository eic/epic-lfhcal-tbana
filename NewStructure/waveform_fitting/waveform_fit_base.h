// Base class for waveform fitting

#pragma once

#include <vector>
#include <map>

class waveform_fit_base {
public:
    waveform_fit_base();
    virtual ~waveform_fit_base() = default;
    
    // Configure the fitting
    virtual void set_waveform(const std::vector<int> &waveform);    // virtual so it can be overwriten, but not pure virtual
    virtual void set_parameter(int parameter, double value);
    virtual double get_parameter(int parameter);
    virtual int get_pedestal() = 0;
    virtual void fit() = 0;                                         // pure virtual so it must be overwritten
    virtual void fit_with_average_ped(double ped) = 0;                        // pure virtual so it must be overwritten
    
    bool is_stale() { return stale; }
    bool is_saturated() { return saturated; }
    double get_E();
    double get_fit_ndf();
    double get_fit_chi2();

protected:
    std::vector<int> waveform;
    std::map<int, double> parameters;
    
    bool stale;
    bool saturated;
    double E;
    double fit_ndf;
    double fit_chi2;
};
