#include "CalibSummary.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

ClassImp(CalibSummary);

bool CalibSummary::Fill(const TileCalib& tc){
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

bool CalibSummary::Fill(TileCalib* tc){
  hLGped        .Fill(tc->PedestalMeanL);
  hLGpedwidth   .Fill(tc->PedestalSigL);
  hHGped        .Fill(tc->PedestalMeanH);
  hHGpedwidth   .Fill(tc->PedestalSigH);
  hLGscale      .Fill(tc->ScaleL);
  if (tc->LGHGCorr != 0. ) hLGscaleCalc  .Fill(tc->ScaleH/tc->LGHGCorr);
  else hLGscaleCalc  .Fill(-1000);
  hLGscalewidth .Fill(tc->ScaleWidthL);
  hHGscale      .Fill(tc->ScaleH);
  hHGscalewidth .Fill(tc->ScaleWidthH);
  hHGLGcorr     .Fill(tc->HGLGCorr);
  hHGLGOffcorr  .Fill(tc->HGLGCorrOff);
  hLGHGcorr     .Fill(tc->LGHGCorr);
  hLGHGOffcorr  .Fill(tc->LGHGCorrOff);
  return true;
}

bool CalibSummary::FillRefRunProps(const TileCalib& tc, const TileCalib& tcRef ){
  if (tc.PedestalMeanH == -1000.)
    hHGpedDiffRef   .Fill(-1000);
  else if (tcRef.PedestalMeanH == -1000.)
    hHGpedDiffRef   .Fill(1000);
  else 
    hHGpedDiffRef   .Fill(tc.PedestalMeanH-tcRef.PedestalMeanH);
  
  if (tc.PedestalSigH == -1000.)
    hHGpedwidthDiffRef   .Fill(-1000);
  else if (tcRef.PedestalSigH == -1000.)
    hHGpedwidthDiffRef   .Fill(1000);
  else 
    hHGpedwidthDiffRef   .Fill(tc.PedestalSigH-tcRef.PedestalSigH);
   
  if (tc.PedestalMeanL == -1000.)
    hLGpedDiffRef   .Fill(-1000);
  else if (tcRef.PedestalMeanL == -1000.)
    hLGpedDiffRef   .Fill(1000);
  else 
    hLGpedDiffRef   .Fill(tc.PedestalMeanL-tcRef.PedestalMeanL);
  
  if (tc.ScaleH == -1000.)
    hHGscaleDiffRef   .Fill(-1000);
  else if (tcRef.ScaleH == -1000.)
    hHGscaleDiffRef   .Fill(1000);
  else 
    hHGscaleDiffRef .Fill(tc.ScaleH-tcRef.ScaleH);
  
  hHGscaleCorrRef.Fill(tc.ScaleH, tcRef.ScaleH);
  pHGscaleCorrRef.Fill(tc.ScaleH, tcRef.ScaleH);

  
  if (tc.ScaleL == -1000.)
    hLGscaleDiffRef   .Fill(-1000);
  else if (tcRef.ScaleL == -1000.)
    hLGscaleDiffRef   .Fill(1000);
  else 
    hLGscaleDiffRef .Fill(tc.ScaleL-tcRef.ScaleL);
  
  if (tc.LGHGCorr == 0. || tc.LGHGCorr == -1000.|| tc.ScaleH == -1000. )
    hLGscaleCalcDiffRef .Fill(-1000);
  else if (tcRef.LGHGCorr == 0. || tcRef.LGHGCorr == -1000. || tcRef.ScaleH == -1000. )
    hLGscaleCalcDiffRef .Fill(1000);
  else 
    hLGscaleCalcDiffRef .Fill((tc.ScaleH/tc.LGHGCorr)-(tcRef.ScaleH/tcRef.LGHGCorr));
  
  if (tc.LGHGCorr == 0. || tc.LGHGCorr == -1000. )
    hLGHGcorrDiffRef .Fill(-1000);
  else if (tcRef.LGHGCorr == 0. || tcRef.LGHGCorr == -1000. )
    hLGHGcorrDiffRef .Fill(1000);
  else   
    hLGHGcorrDiffRef  .Fill(tc.LGHGCorr-tcRef.LGHGCorr);
  
  return true;
}


bool CalibSummary::FillRefRunProps(TileCalib* tc, TileCalib* tcRef ){
  if (tc->PedestalMeanH == -1000.)
    hHGpedDiffRef   .Fill(-1000);
  else if (tcRef->PedestalMeanH == -1000.)
    hHGpedDiffRef   .Fill(1000);
  else 
    hHGpedDiffRef   .Fill(tc->PedestalMeanH-tcRef->PedestalMeanH);
 
 if (tc->PedestalSigH == -1000.)
    hHGpedwidthDiffRef   .Fill(-1000);
  else if (tcRef->PedestalSigH == -1000.)
    hHGpedwidthDiffRef   .Fill(1000);
  else 
    hHGpedwidthDiffRef   .Fill(tc->PedestalSigH-tcRef->PedestalSigH);
   
  if (tc->PedestalMeanL == -1000.)
    hLGpedDiffRef   .Fill(-1000);
  else if (tcRef->PedestalMeanL == -1000.)
    hLGpedDiffRef   .Fill(1000);
  else 
    hLGpedDiffRef   .Fill(tc->PedestalMeanL-tcRef->PedestalMeanL);
  
  if (tc->ScaleH == -1000.)
    hHGscaleDiffRef   .Fill(-1000);
  else if (tcRef->ScaleH == -1000.)
    hHGscaleDiffRef   .Fill(1000);
  else 
    hHGscaleDiffRef .Fill(tc->ScaleH-tcRef->ScaleH);
  
  hHGscaleCorrRef.Fill(tc->ScaleH, tcRef->ScaleH);
  pHGscaleCorrRef.Fill(tc->ScaleH, tcRef->ScaleH);
  
  if (tc->ScaleL == -1000.)
    hLGscaleDiffRef   .Fill(-1000);
  else if (tcRef->ScaleL == -1000.)
    hLGscaleDiffRef   .Fill(1000);
  else 
    hLGscaleDiffRef .Fill(tc->ScaleL-tcRef->ScaleL);
  
  if (tc->LGHGCorr == 0. || tc->LGHGCorr == -1000.|| tc->ScaleH == -1000. )
    hLGscaleCalcDiffRef .Fill(-1000);
  else if (tcRef->LGHGCorr == 0. || tcRef->LGHGCorr == -1000. || tcRef->ScaleH == -1000. )
    hLGscaleCalcDiffRef .Fill(1000);
  else 
    hLGscaleCalcDiffRef .Fill((tc->ScaleH/tc->LGHGCorr)-(tcRef->ScaleH/tcRef->LGHGCorr));
  
  if (tc->LGHGCorr == 0. || tc->LGHGCorr == -1000. )
    hLGHGcorrDiffRef .Fill(-1000);
  else if (tcRef->LGHGCorr == 0. || tcRef->LGHGCorr == -1000. )
    hLGHGcorrDiffRef .Fill(1000);
  else   
    hLGHGcorrDiffRef  .Fill(tc->LGHGCorr-tcRef->LGHGCorr);
  
  return true;
}

bool CalibSummary::Write(TFile* f){
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
  
  if (RunNr != RunNrRef){
    hHGpedDiffRef       .Write();
    hHGpedwidthDiffRef  .Write();
    hLGpedDiffRef       .Write();
    hHGscaleDiffRef     .Write();
    hLGscaleDiffRef     .Write();
    hLGscaleCalcDiffRef .Write();
    hLGHGcorrDiffRef    .Write();
    hHGscaleCorrRef     .Write();
    pHGscaleCorrRef     .Write();
  }
  return true;
}



//****************************************************************
// analyse the full calibrations overview object
// return status:
//      0 - neither ped nor mip scale are filled
//      1 - ped is filled
//      2 - ped & mip scale are filled
//****************************************************************
int CalibSummary::Analyse(int debug){
  int calibStatus = 0;
  if (hLGped.GetMean() > 0. || hHGped.GetMean() > 0.)  
    calibStatus = 1;
  if (hLGscale.GetMean() > 0. || hHGscale.GetMean() > 0.)  
    calibStatus = 2;
  
  if (debug > 0){
    std::cout << "***********************************************************************************************************************" << std::endl;
    std::cout << "Run Nr.: "<< RunNr << "\t total entries: "<< hLGped.GetEntries() << "\t calib status: " << calibStatus<< std::endl;
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
  }
  return calibStatus;
} // end CalibSummary::Analyse()

//*************************************************************************
// Set Run Property information
//*************************************************************************
void CalibSummary::SetRunProperties( RunInfo currRunInfo){
  rf      = ReturnRFValue(currRunInfo.rf);
  cf      = ReturnCFValue(currRunInfo.cf);
  cfcomp  = ReturnCFCompValue(currRunInfo.cfcomp);
  cc      = ReturnCCValue(currRunInfo.cc);
  injDAC  = currRunInfo.injDAC;
  energy  = currRunInfo.energy;
  temp    = currRunInfo.temp;
}


//*************************************************************************
// Labeling legend entries
//*************************************************************************
TString CalibSummary::GetLabelLegend( RunInfo commonRunInfo, int nSameSettings){
  
  TString labelLegend = "";
  if (commonRunInfo.species.Contains("injection")){
    if (nSameSettings == 6){
      if (commonRunInfo.vop < -9999)    labelLegend = Form("%.1f",(double)Voltage);
      if (commonRunInfo.rf < -9999)     labelLegend = Form("%.1f",rf);
      if (commonRunInfo.cf < -9999)     labelLegend = Form("%.0f",cf);
      if (commonRunInfo.cfcomp < -9999) labelLegend = Form("%.0f",cfcomp);
      if (commonRunInfo.cc < -9999)     labelLegend = Form("%.3f",cc);
      if (commonRunInfo.injDAC < -9999)  labelLegend = Form("%.0f",injDAC);
      if (commonRunInfo.energy < -9999)  labelLegend = Form("%.0f",energy);
    } else if (nSameSettings == 5){
      if (commonRunInfo.vop < -9999) labelLegend = Form("%.1f ",(double)Voltage);
      if (commonRunInfo.rf < -9999) labelLegend = labelLegend+Form("%.1f ",rf);
      if (commonRunInfo.cf < -9999) labelLegend = labelLegend+Form("%.0f ",cf);
      if (commonRunInfo.cfcomp < -9999) labelLegend = labelLegend+Form("%.0f ",cfcomp);
      if (commonRunInfo.cc < -9999)  labelLegend = labelLegend+Form("%.3f ",cc);
      if (commonRunInfo.injDAC < -9999)  labelLegend = labelLegend+Form("%.0f",injDAC);
      if (commonRunInfo.energy < -9999)  labelLegend = labelLegend+Form("%.0f",energy);
    }
  } else if (commonRunInfo.species.Contains("laser")){
    if (commonRunInfo.energy < -9999)  labelLegend = Form("%.0f",energy);
    if (commonRunInfo.temp < -9999)  labelLegend = Form("%.0f",temp);
  // labeling for waveform compare from beam data 5 same settings
  } else if (nSameSettings == 5){
    if (commonRunInfo.vop < -9999) labelLegend = Form("%.1f",(double)Voltage);
    if (commonRunInfo.rf < -9999) labelLegend = Form("%.1f",rf);
    if (commonRunInfo.cf < -9999) labelLegend = Form("%.0f",cf);
    if (commonRunInfo.cfcomp < -9999) labelLegend = Form("%.0f",cfcomp);
    if (commonRunInfo.cc < -9999)  labelLegend = Form("%.3f",cc);
    if (commonRunInfo.energy < -9999)  labelLegend = Form("%.0f",energy);
  // labeling for waveform compare from beam data 4 same settings    
  } else if (nSameSettings == 4){
    if (commonRunInfo.vop < -9999) labelLegend = Form("%.1f ",(double)Voltage);
    if (commonRunInfo.rf < -9999) labelLegend = labelLegend+Form("%.1f ",rf);
    if (commonRunInfo.cf < -9999) labelLegend = labelLegend+Form("%.0f ",cf);
    if (commonRunInfo.cfcomp < -9999) labelLegend = labelLegend+Form("%.0f ",cfcomp);
    if (commonRunInfo.cc < -9999)  labelLegend = labelLegend+Form("%.3f ",cc);
    if (commonRunInfo.energy < -9999)  labelLegend = labelLegend+Form("%.0f",energy);
  }
    
  if (labelLegend.CompareTo("") == 0)
    labelLegend = Form("%i",RunNr );
    
  return   labelLegend;
}
