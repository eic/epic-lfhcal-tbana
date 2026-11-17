#ifndef TOTSLOPEFIT_H
#define TOTSLOPEFIT_H

// TOT slope fit for ADC-saturated HGCROC cells (ShapeAnalyzer option -T, runShapeAnalyzer mode "tot_fit").
//
// The calibrated energy (Hgcroc::GetE) comes from the ADC only and is capped for saturated cells. Above ADC
// saturation the HGCROC TOT fires, so for saturated hits with TOT the energy is replaced by a linear TOT model
//     E = E_sat + k*(TOT - TOT0)
// per channel. TOT and ADC are never both valid on the same hit (TOT starts at ADC saturation), so k and TOT0
// are fixed by requiring the cell energy spectrum to continue smoothly across the saturation edge:
//   - the unsaturated spectrum just below saturation is fitted with dN/dE = A*exp(-b*E) on [0.4, 0.95]*E_sat and its
//     slope b is continued above E_sat, normalised to the observed number of ADC-saturated hits N_sat:
//     N(>E) = N_sat*exp(-b*(E - E_sat))   (the normalisation of the fit itself typically under-predicts N_sat, see
//     nExpSat in the parameter file, so only its slope is used)
//   - every saturated hit with TOT is assigned the energy at which N(>E) equals the number of saturated hits with
//     a larger TOT (rank matching, monotonic in TOT; saturated hits without TOT rank lowest)
//   - a straight line E = E_sat + k*(TOT - TOT0) is fitted to these (TOT, E) points between the 5% and 95% TOT
//     quantiles (TOT overflow excluded); k is the slope, TOT0 the TOT at which the line reaches E_sat
// The TOT spectrum itself is not assumed to have any shape. E_sat (energy at ADC saturation) =
// median(E/A_peak) of large unsaturated pulses * (1023 - pedestal).
// Channels whose own fit fails use the median k of the good channels with TOT0 = 2% quantile of their own TOT
// (TOT at threshold ~ E_sat). Saturated hits without TOT, and channels without any fit, keep their ADC energy.
// The corrected energy is never below the ADC energy.

#include <map>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include "TH1D.h"
#include "TF1.h"
#include "TFitResultPtr.h"

struct TOTChannelFit{
  int    status   = 0;   // 0: not corrected, 1: own slope fit, 2: fallback (median slope of the good channels)
  int    nUnsat   = 0;   // unsaturated hits
  int    nSat     = 0;   // ADC-saturated hits
  int    nSatTOT  = 0;   // ADC-saturated hits with TOT > 0
  double eSat     = 0;   // energy at ADC saturation
  double b = 0, lnA = 0; // dN/dE = exp(lnA - b*E) (counts per energy unit), fit of the unsaturated hits
  double nExpSat = 0;    // number of hits above E_sat expected from that fit (compare with nSat)
  double k = 0;          // energy per TOT unit
  double tot0 = 0;       // TOT at E_sat
  double eFitLo = 0, eFitHi = 0, tFitLo = 0, tFitHi = 0;   // fit ranges (energy fit, TOT range of the line fit)
};

class TOTSlopeFit{
 public:
  struct SatHit{ float e; float tot; };
  struct ChannelData{
    std::vector<float>  eUnsat;   // energies of unsaturated hits
    std::vector<float>  ratio;    // E/A_peak of large unsaturated pulses (A_peak = max ADC sample - pedestal)
    std::vector<float>  ped;      // pedestal of those pulses (mean of the first two samples)
    std::vector<SatHit> sat;      // ADC-saturated hits (TOT = 0 if the TOT did not fire)
  };

  static constexpr int    kADCMax       = 1023;   // ADC saturation value (Hgcroc::IsSaturatedADC: > 1022)
  static constexpr double kTOTOverflow  = 4088;   // largest raw TOT value (overflow)
  static constexpr double kMinPulseAmp  = 300;    // minimum ADC amplitude of the pulses used for E_sat

  void AddHit(int cellID, double e, const std::vector<int>& adc, double tot, bool saturated){
    ChannelData& d = data[cellID];
    if(saturated){
      d.sat.push_back({(float)e, (float)std::max(0., tot)});
      return;
    }
    d.eUnsat.push_back(e);
    if(adc.size() >= 3 && e > 0){
      const double ped = 0.5*(adc[0]+adc[1]);
      const double amp = *std::max_element(adc.begin(), adc.end()) - ped;
      if(amp > kMinPulseAmp){
        d.ratio.push_back(e/amp);
        d.ped.push_back(ped);
      }
    }
  }

  void Fit(int minHits = 30){
    std::vector<double> goodK;
    for(auto& kv : data){
      const ChannelData& d = kv.second;
      TOTChannelFit& f = fits[kv.first];
      f.nUnsat = d.eUnsat.size();
      f.nSat   = d.sat.size();
      std::vector<double> T;
      for(const SatHit& s : d.sat) if(s.tot > 0) T.push_back(s.tot);
      f.nSatTOT = T.size();
      if(d.ratio.size() >= 10)       f.eSat = Quantile(d.ratio, 0.5)*(kADCMax - Quantile(d.ped, 0.5));
      else if(!d.eUnsat.empty())     f.eSat = Quantile(d.eUnsat, 1.);   // fallback: largest unsaturated energy
      if(f.nSatTOT == 0 || f.eSat <= 0) continue;
      f.tot0 = Quantile(T, 0.02);

      // unsaturated spectrum below the edge
      f.eFitLo = 0.4*f.eSat;
      f.eFitHi = 0.95*f.eSat;
      std::vector<double> E(d.eUnsat.begin(), d.eUnsat.end());
      const bool okE = FitExpo(E, f.eFitLo, f.eFitHi, 25, minHits, f.b, f.lnA);

      if(!okE || f.b <= 0) continue;
      f.nExpSat = std::exp(f.lnA - f.b*f.eSat)/f.b;

      // rank matching: the hit with S saturated hits at larger TOT (S = 0.5, 1.5, ...) gets the energy with N(>E) = S
      std::sort(T.begin(), T.end());
      const double lnNSat = std::log((double)f.nSat);
      const size_t n = T.size(), iLo = (size_t)(0.05*n), iHi = (size_t)(0.95*n);
      f.tFitLo = T[iLo];
      f.tFitHi = T[std::min(iHi, n-1)];
      double sw = 0, sx = 0, sy = 0, sxx = 0, sxy = 0;
      for(size_t i = iLo; i < iHi; i++){
        if(T[i] >= kTOTOverflow) continue;
        const double e = f.eSat + (lnNSat - std::log(n - i - 0.5))/f.b;
        sw += 1; sx += T[i]; sy += e; sxx += T[i]*T[i]; sxy += T[i]*e;
      }
      const double det = sw*sxx - sx*sx;
      if(sw < minHits || det <= 0) continue;
      const double k      = (sw*sxy - sx*sy)/det;
      const double offset = (sy - k*sx)/sw;   // E at TOT = 0
      if(!(k > 1e-3 && k < 20)) continue;
      f.k      = k;
      f.tot0   = (f.eSat - offset)/k;
      f.status = 1;
      goodK.push_back(k);
    }
    // fallback for channels with TOT hits but no usable fit
    if(goodK.empty()) return;
    medianK = Quantile(goodK, 0.5);
    for(auto& kv : fits){
      TOTChannelFit& f = kv.second;
      if(f.status != 0 || f.nSatTOT == 0 || f.eSat <= 0) continue;
      f.k      = medianK;
      f.status = 2;   // tot0 stays at the 2% TOT quantile
    }
  }

  // energy of a hit after the TOT correction (unchanged unless ADC-saturated with TOT and a fit exists)
  double Correct(int cellID, double e, double tot, bool saturated) const {
    if(!saturated || tot <= 0) return e;
    auto it = fits.find(cellID);
    if(it == fits.end() || it->second.status == 0) return e;
    const TOTChannelFit& f = it->second;
    return std::max(e, f.eSat + f.k*(tot - f.tot0));
  }

  const std::map<int, TOTChannelFit>& Fits() const { return fits; }
  const std::map<int, ChannelData>&   Data() const { return data; }
  double MedianSlope() const { return medianK; }

  int NChannels(int status) const {
    int n = 0;
    for(const auto& kv : fits) if(kv.second.status == status && kv.second.nSatTOT > 0) n++;
    return n;
  }

  void WriteParameters(const char* fileName) const {
    FILE* out = fopen(fileName, "w");
    if(!out) return;
    fprintf(out, "# TOT slope fit, E = E_sat + k*(TOT - TOT0); status 1: own fit, 2: median slope, 0: not corrected\n");
    fprintf(out, "# cellID status nUnsat nSat nSatTOT E_sat k TOT0 b nExpSat(fit prediction of nSat)\n");
    for(const auto& kv : fits){
      const TOTChannelFit& f = kv.second;
      if(f.nSat == 0) continue;
      fprintf(out, "%d %d %d %d %d %.3f %.5f %.1f %.5g %.1f\n", kv.first, f.status, f.nUnsat, f.nSat, f.nSatTOT,
              f.eSat, f.k, f.tot0, f.b, f.nExpSat);
    }
    fclose(out);
  }

  template<class V> static double Quantile(const V& in, double p){
    if(in.empty()) return 0;
    std::vector<double> v(in.begin(), in.end());
    std::sort(v.begin(), v.end());
    return v[std::min(v.size()-1, (size_t)(p*v.size()))];
  }

 private:
  // binned likelihood fit of exp(p0 + p1*x) on [lo, hi); slope = -p1, lnDens = ln of the density per unit x at x = 0
  static bool FitExpo(const std::vector<double>& v, double lo, double hi, int nBins, int minHits,
                      double& slope, double& lnDens){
    if(hi <= lo) return false;
    TH1D h("hTOTSlopeFitTmp", "", nBins, lo, hi);
    h.SetDirectory(nullptr);
    for(double x : v) if(x >= lo && x < hi) h.Fill(x);
    if(h.GetEntries() < minHits) return false;
    TF1 f("fTOTSlopeFitTmp", "expo", lo, hi);
    f.SetParameters(std::log(std::max(1., h.GetBinContent(1))) + lo/(hi-lo), -1./(hi-lo));
    const int status = h.Fit(&f, "QLRN0");
    if(status != 0) return false;
    slope  = -f.GetParameter(1);
    lnDens = f.GetParameter(0) - std::log(h.GetBinWidth(1));
    return std::isfinite(slope) && std::isfinite(lnDens);
  }

  std::map<int, ChannelData>   data;
  std::map<int, TOTChannelFit> fits;
  double medianK = 0;
};

#endif
