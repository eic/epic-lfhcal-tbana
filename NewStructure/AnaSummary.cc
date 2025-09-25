#include "AnaSummary.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

ClassImp(AnaSummary);

bool AnaSummary::Fill(const TileCalib& tc){
  hLGped        .Fill(tc.PedestalMeanL);
  hLGpedwidth   .Fill(tc.PedestalSigL);
  hHGped        .Fill(tc.PedestalMeanH);
  hHGpedwidth   .Fill(tc.PedestalSigH);
  hLGscale      .Fill(tc.ScaleL);
  if (tc.LGHGCorr != 0. ) hLGscaleCalc  .Fill(tc.ScaleH/tc.LGHGCorr);
  else hLGscaleCalc  .Fill(-1000);
  hLGscalewidth .Fill(tc.ScaleWidthL);
  hHGscale      .Fill(tc.ScaleH);
  hHGscalewidth .Fill(tc.ScaleWidthH);
  hHGLGcorr     .Fill(tc.HGLGCorr);
  hHGLGOffcorr  .Fill(tc.HGLGCorrOff);
  hLGHGcorr     .Fill(tc.LGHGCorr);
  hLGHGOffcorr  .Fill(tc.LGHGCorrOff);
  return true;
}

bool AnaSummary::Write(TFile* f){
  f->cd();
  hLGped          .Write();
  hLGpedwidth     .Write();
  hHGped          .Write();
  hHGpedwidth     .Write();
  hLGscale        .Write();
  hLGscaleCalc    .Write();
  hLGscalewidth   .Write();
  hHGscale        .Write();
  hHGscalewidth   .Write();
  hHGLGcorr       .Write();
  hHGLGOffcorr    .Write();
  hLGHGcorr       .Write();
  hLGHGOffcorr    .Write();
  return true;
}

bool AnaSummary::Analyse(){
  std::cout << "***********************************************************************************************************************" << std::endl;
  std::cout << "Run Nr.: "<< RunNr << "\t total entries: "<< hLGped.GetEntries() << std::endl;
  std::cout << Form("\t --> LG pedestal:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hLGped.GetMean(), hLGped.GetRMS(), 
                    hLGped.GetBinContent(0)+hLGped.GetBinContent(hLGped.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> LG pedestal sigma:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hLGpedwidth.GetMean(), hLGpedwidth.GetRMS(), 
                    hLGpedwidth.GetBinContent(0)+hLGpedwidth.GetBinContent(hLGpedwidth.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> HG pedestal:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hHGped.GetMean(), hHGped.GetRMS(), 
                    hHGped.GetBinContent(0)+hHGped.GetBinContent(hHGped.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> HG pedestal sigma:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hHGpedwidth.GetMean(), hHGpedwidth.GetRMS(), 
                    hHGpedwidth.GetBinContent(0)+hHGpedwidth.GetBinContent(hHGpedwidth.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> LG scale:\t\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hLGscale.GetMean(), hLGscale.GetRMS(), 
                    hLGscale.GetBinContent(0)+hLGscale.GetBinContent(hLGscale.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> LG scale calc:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hLGscaleCalc.GetMean(), hLGscaleCalc.GetRMS(), 
                    hLGscaleCalc.GetBinContent(0)+hLGscaleCalc.GetBinContent(hLGscaleCalc.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> LG scale width:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hLGscalewidth.GetMean(), hLGscalewidth.GetRMS(), 
                    hLGscalewidth.GetBinContent(0)+hLGscalewidth.GetBinContent(hLGscalewidth.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> HG scale:\t\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hHGscale.GetMean(), hHGscale.GetRMS(), 
                    hHGscale.GetBinContent(0)+hHGscale.GetBinContent(hHGscale.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> HG scale width:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hHGscalewidth.GetMean(), hHGscalewidth.GetRMS(), 
                    hHGscalewidth.GetBinContent(0)+hHGscalewidth.GetBinContent(hHGscalewidth.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> LG-HG corr:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hLGHGcorr.GetMean(), hLGHGcorr.GetRMS(), 
                    hLGHGcorr.GetBinContent(0)+hLGHGcorr.GetBinContent(hLGHGcorr.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> LG-HG corr offset:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hLGHGOffcorr.GetMean(), hLGHGcorr.GetRMS(), 
                    hLGHGOffcorr.GetBinContent(0)+hLGHGOffcorr.GetBinContent(hLGHGOffcorr.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> HG-LG corr:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hHGLGcorr.GetMean(), hHGLGcorr.GetRMS(), 
                    hHGLGcorr.GetBinContent(0)+hHGLGcorr.GetBinContent(hHGLGcorr.GetNbinsX()+1)  )
            << std::endl;
  std::cout << Form("\t --> HG-LG corr offset:\t mean:\t%3.3f\t\tRMS:\t%3.3f\t\t%.0f\t out of bounds", 
                    hHGLGOffcorr.GetMean(), hHGLGOffcorr.GetRMS(), 
                    hHGLGOffcorr.GetBinContent(0)+hHGLGOffcorr.GetBinContent(hHGLGOffcorr.GetNbinsX()+1)  )
            << std::endl;
  std::cout << "***********************************************************************************************************************" << std::endl;
  return true;
}
bool AnaSummary::SetDeltaTimeHist(TH1D* Hist) {
    if (Hist){
      TH1D temp = *Hist;
      temp.SetName(Form("%s_Run%i",Hist->GetName(),RunNr));
      temp.SetDirectory(0);
      temp.GetYaxis()->SetTitle("Counts");
      hDeltaTime = temp;
      return true;
    }
    else{
        return false;
    }
    return false;
}
