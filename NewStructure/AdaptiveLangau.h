#ifndef LFHCAL_ADAPTIVE_LANGAU_H
#define LFHCAL_ADAPTIVE_LANGAU_H

// Numerical helper only: no minimizer, fit limits, selection, or ROOT objects.
// Extracted from LangauNumerics.h at 3a643646eb301c7112449e8f6d6ab31f12c72943.
// The subdivision rule, breakpoints, tolerances, and +/-5 sigma convention
// are retained. Expensive cross-checks belong in tests, not every production fit.
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <stdexcept>
#include <vector>

namespace lfhcal { namespace adaptive_langau {
using Function = std::function<double(double)>;
struct Parameters { double width, mp, area, sigma; };
struct IntegrationConfig {
  double relative_tolerance = 1e-10;
  double gaussian_span = 5.;
  int max_calls = 500000;
  int max_depth = 18;
  double absolute_tolerance_fraction = .01;
};
struct PeakWidth { double peak, left, right, fwhm, height; };

inline void require(bool ok, const char* message) {
  if (!ok) throw std::runtime_error(message);
}
inline double checked(const Function& f, double x) {
  const double y = f(x);
  require(std::isfinite(y) && y >= 0., "nonfinite or negative Langau evaluation");
  return y;
}

namespace detail {
// Positive nodes/weights of the 16-point Gauss-Legendre rule on [-1,1].
constexpr std::array<double,8> nodes = {
  .095012509837637440185, .28160355077925891323,
  .45801677765722738634, .61787624440264374845,
  .75540440835500303390, .86563120238783174388,
  .94457502307323257608, .98940093499164993260};
constexpr std::array<double,8> weights = {
  .18945061045506849629, .18260341504492358887,
  .16915651939500253819, .14959598881657673208,
  .12462897125553387205, .095158511682492784810,
  .062253523938647892863, .027152459411754094852};
constexpr std::array<double,16> breakpoints = {
  -16.,-8.,-5.5,-4.,-2.,-1.,0.,1.,2.,4.,5.,8.,12.,16.,50.,300.};

inline double gauss16(const Function& f, double a, double b,
                      int& calls, int maximum) {
  calls += 16;
  require(calls <= maximum, "Langau quadrature evaluation budget exhausted");
  const double c=(a+b)/2, d=(b-a)/2;
  double sum=0.;
  for (std::size_t i=0; i<nodes.size(); ++i)
    sum += weights[i]*(checked(f,c-d*nodes[i])+checked(f,c+d*nodes[i]));
  return d*sum;
}
inline double subdivide(const Function& f, double a, double b, double coarse,
                         double atol, double rtol, int depth,
                         int& calls, int maximum) {
  const double mid=(a+b)/2;
  const double l=gauss16(f,a,mid,calls,maximum);
  const double r=gauss16(f,mid,b,calls,maximum);
  const double fine=l+r;
  if (std::abs(fine-coarse) <= atol+rtol*std::abs(fine)) return fine;
  require(depth>0 && a<mid && mid<b, "Langau quadrature did not converge");
  return subdivide(f,a,mid,l,atol/2,rtol,depth-1,calls,maximum)
       + subdivide(f,mid,b,r,atol/2,rtol,depth-1,calls,maximum);
}
} // namespace detail

// u=(t-mpc)/width resolves the narrow Landau on its own scale. A single
// adaptive integral over the unsplit interval could miss its entire core.
class Convolution {
 public:
  Convolution(Parameters p, Function standardLandau, IntegrationConfig cfg = {})
      : p_(p), pdf_(standardLandau), cfg_(cfg) {
    require(std::isfinite(p.width) && p.width>0 && std::isfinite(p.sigma) && p.sigma>0
            && std::isfinite(p.mp) && std::isfinite(p.area) && p.area>0,
            "invalid Langau parameters");
    require(cfg.relative_tolerance>0 && cfg.relative_tolerance<1
            && cfg.gaussian_span>=5 && cfg.gaussian_span<=12
            && cfg.max_calls>0 && cfg.max_depth>0
            && cfg.absolute_tolerance_fraction>0, "invalid Langau integration settings");
  }
  double operator()(double x) const {
    require(std::isfinite(x), "nonfinite Langau coordinate");
    const double mpc=p_.mp+.22278298*p_.width;
    const double lo=(x-cfg_.gaussian_span*p_.sigma-mpc)/p_.width;
    const double hi=(x+cfg_.gaussian_span*p_.sigma-mpc)/p_.width;
    std::vector<double> edges{lo,hi};
    const auto add=[&](double u) { if (u>lo && u<hi) edges.push_back(u); };
    for (double u:detail::breakpoints) add(u);
    const double extent=std::max(std::abs(lo),std::abs(hi));
    require(std::isfinite(extent) && extent<1e12, "Langau parameter ratio exceeds numerical domain");
    for (double u=32.; u<extent; u*=2.) { add(u); add(-u); }
    for (int k=-static_cast<int>(cfg_.gaussian_span);
         k<=static_cast<int>(cfg_.gaussian_span); ++k)
      add((x+k*p_.sigma-mpc)/p_.width);
    std::sort(edges.begin(),edges.end());
    edges.erase(std::unique(edges.begin(),edges.end()),edges.end());
    Function integrand=[&](double u) {
      const double z=(x-mpc-p_.width*u)/p_.sigma;
      return pdf_(u)*std::exp(-.5*z*z);
    };
    int calls=0;
    double sum=0.;
    const double atol=cfg_.relative_tolerance*cfg_.absolute_tolerance_fraction/(edges.size()-1);
    for (std::size_t i=1; i<edges.size(); ++i) {
      const double a=edges[i-1], b=edges[i];
      const double coarse=detail::gauss16(integrand,a,b,calls,cfg_.max_calls);
      sum += detail::subdivide(integrand,a,b,coarse,atol,cfg_.relative_tolerance,
                               cfg_.max_depth,calls,cfg_.max_calls);
    }
    const double value=p_.area*sum/(p_.sigma*std::sqrt(2*std::acos(-1.)));
    require(std::isfinite(value) && value>=0., "invalid Langau integral");
    return value;
  }
 private:
  Parameters p_;
  Function pdf_;
  IntegrationConfig cfg_;
};

// Same bounded peak/half-height procedure as the tested adaptive helper.
// It evaluates the continuous model, not a sampled/FFT graph. This is numerical
// extraction of ScaleH/FWHM, not an additional fit or a goodness-of-fit cut.
inline PeakWidth peakWidth(const Function& f, double centre, double scale) {
  require(std::isfinite(centre) && std::isfinite(scale) && scale>0.,
          "invalid Langau peak-search scale");
  constexpr int scanBins=160;
  double a=centre-8*scale, b=centre+12*scale, lo=0., hi=0.;
  bool bracketed=false;
  for (int expand=0; expand<8; ++expand) {
    std::vector<double> y(scanBins+1);
    int best=0;
    for (int i=0; i<=scanBins; ++i) {
      y[i]=checked(f,a+(b-a)*i/scanBins);
      if (y[i]>y[best]) best=i;
    }
    require(y[best]>0., "zero curve across Langau peak-search window");
    if (best>0 && best<scanBins) {
      int peaks=0;
      for (int i=1; i<scanBins; ++i)
        if (y[i]>y[i-1] && y[i]>=y[i+1] && y[i]>1e-5*y[best]) ++peaks;
      require(peaks==1, "multiple resolved peaks in Langau model");
      lo=a+(b-a)*(best-1)/scanBins;
      hi=a+(b-a)*(best+1)/scanBins;
      bracketed=true;
      break;
    }
    a=centre+2*(a-centre); b=centre+2*(b-centre);
  }
  require(bracketed, "unable to bracket Langau peak");
  const double golden=(std::sqrt(5.)-1)/2, xtol=1e-8*std::max(1.,scale);
  double l=hi-golden*(hi-lo), r=lo+golden*(hi-lo);
  double fl=checked(f,l), fr=checked(f,r);
  for (int k=0; k<160 && hi-lo>xtol; ++k) {
    if (fl>fr) {hi=r; r=l; fr=fl; l=hi-golden*(hi-lo); fl=checked(f,l);}
    else {lo=l; l=r; fl=fr; r=lo+golden*(hi-lo); fr=checked(f,r);}
  }
  require(hi-lo<=xtol, "Langau peak search did not converge");
  const double peak=(lo+hi)/2, height=checked(f,peak);
  require(height>0., "nonpositive Langau peak height");
  const auto crossing=[&](int direction) {
    double inside=peak, outside=peak, distance=scale;
    bool found=false;
    for (int k=0; k<64; ++k) {
      outside=peak+direction*distance;
      if (checked(f,outside)<height/2) {found=true; break;}
      distance*=1.8;
    }
    require(found, "unable to bracket Langau half height");
    for (int k=0; k<200; ++k) {
      const double mid=(inside+outside)/2, residual=checked(f,mid)/height-.5;
      if (std::abs(residual)<1e-10) return mid;
      if (residual>0) inside=mid; else outside=mid;
      if (std::abs(inside-outside)<xtol*.01) return (inside+outside)/2;
    }
    throw std::runtime_error("Langau half-height search did not converge");
  };
  const double left=crossing(-1), right=crossing(1);
  require(left<peak && peak<right && std::abs(checked(f,left)/height-.5)<1e-8
          && std::abs(checked(f,right)/height-.5)<1e-8, "invalid Langau half-height crossings");
  return {peak,left,right,right-left,height};
}
}} // namespace lfhcal::adaptive_langau
#endif
