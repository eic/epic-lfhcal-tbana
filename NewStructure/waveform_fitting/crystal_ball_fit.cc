#include "waveform_fit_base.h"
#include "crystal_ball_fit.h"

#include <map>
#include <vector>
#include <iostream>

#include <TF1.h>
#include <TGraph.h>
#include <TMath.h>

crystal_ball_fit::crystal_ball_fit() : waveform_fit_base{} {
    parameters[0] = 0;  // N
    parameters[1] = 0;  // alpha
    parameters[2] = 0;  // n
    parameters[3] = 0;  // mean
    parameters[4] = 0;  // sigma
    parameters[5] = 0;  // offset

    saturated = false;

    crystal_ball_function = new TF1("crystal_ball_function", this, &crystal_ball_fit::crystal_ball, 0, 10, 6);
    linear_function = new TF1("constant", "[0]", 0, 10);
    crystal_ball_graph = new TGraph();
}

crystal_ball_fit::~crystal_ball_fit() {
    delete crystal_ball_function;
    delete crystal_ball_graph;
}

void crystal_ball_fit::fit() {
    // Fit the waveform with a crystal ball function
    // The crystal ball function is defined as:
    // f(x; alpha, n, mean, sigma) = N * exp(-0.5 * ((x - mean) / sigma)^2) for x < alpha
    //                                N * exp(-0.5 * ((x - mean) / sigma)^2) for x >= alpha
    //                                N * (n / abs(alpha)) * (n / abs(alpha)) * exp(-0.5 * alpha * alpha) * (n / abs(alpha) - abs(alpha) - (x - mean) / sigma)^(-n) for x < alpha
    //                                N * exp(-0.5 * ((x - mean) / sigma)^2) for x >= alpha
    // where N is the normalization factor, alpha is the transition point, n is the power, mean is the mean, and sigma is the standard deviation
    // The parameters are stored in the parameters map with the following keys:
    // 0: N
    // 1: alpha
    // 2: n
    // 3: mean
    // 4: sigma

    // Check if the waveform has been set
    if (waveform.size() == 0) {
        std::cerr << "Waveform has not been set" << std::endl;
        return;
    }
    crystal_ball_function->SetRange(0, waveform.size());

    // Fill the graph
    for (int i = 0; i < waveform.size(); i++) {
        crystal_ball_graph->SetPoint(i, i, waveform[i]);
        if (waveform[i] > 1000) {  // Check for saturation.  TODO: Make this a parameter
            saturated = true;
        }
    }

    // Try a linear fit
    linear_function->SetParameter(0, parameters[5]);
    if (parameters.count(15) && parameters.count(25)) {   // If the parameter has a lower and upper limit
        linear_function->SetParLimits(0, parameters[15], parameters[25]);
    }
    crystal_ball_graph->Fit(linear_function, "Q");
    if (linear_function->GetChisquare() < 500) {
        E = 0;
        fit_ndf = linear_function->GetNDF();
        fit_chi2 = linear_function->GetChisquare();
        stale = false;
        return;
    }
    
    // Set initial parameters
    for (int i = 0; i < 6; i++) {
        crystal_ball_function->SetParameter(i, parameters[i]);
        if (parameters.count(i + 10) && parameters.count(i + 20)) {   // If the parameter has a lower and upper limit
            crystal_ball_function->SetParLimits(i, parameters[i + 10], parameters[i + 20]);
        }
    }
    // Fit the graph
    crystal_ball_graph->Fit(crystal_ball_function, "Q");

    // Get the results
    E = crystal_ball_function->GetParameter(4);
    fit_ndf = crystal_ball_function->GetNDF();
    fit_chi2 = crystal_ball_function->GetChisquare();
    
    stale = false;
}

int crystal_ball_fit::get_pedestal() {
    if (stale) {
        return -1;
    }
    return crystal_ball_function->GetParameter(5);
}

double crystal_ball_fit::crystal_ball(double *inputs, double *parameters) {
// Parameters
    // alpha: Where the gaussian transitions to the power law tail - fix?
    // n: The exponent of the power law tail - fix?
    // x_bar: The mean of the gaussian - free
    // sigma: The width of the gaussian - fix ?
    // N: The normalization of the gaussian - free
    // B baseline - fix?

    double x = inputs[0];

    double alpha = parameters[0];
    double n = parameters[1];
    double x_bar = parameters[2];
    double sigma = parameters[3];
    double N = parameters[4];
    double offset = parameters[5];
    
    double A = pow(n / fabs(alpha), n) * exp(-0.5 * alpha * alpha);
    double B = n / fabs(alpha) - fabs(alpha);
    // std::cout << "A: " << A << std::endl;

    // std::cout << "alpha: " << alpha << " n: " << n << " x_bar: " << x_bar << " sigma: " << sigma << " N: " << N << " B: " << B << " A: " << A << std::endl;

    double ret_val;
    if ((x - x_bar) / sigma < alpha) {
        // std::cout << "path a" << std::endl;
        ret_val = exp(-0.5 * (x - x_bar) * (x - x_bar) / (sigma * sigma));
    } else {
        // std::cout << "path b" << std::endl;
        ret_val = A * pow(B + (x - x_bar) / sigma, -1 * n);
    }
    ret_val = N * ret_val + offset;
    // std::cout << "x: " << x << " y: " << ret_val << std::endl;
    return ret_val;
}