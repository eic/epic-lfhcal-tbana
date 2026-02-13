#include "TileTrend.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

ClassImp(TileTrend);

//************************************************************************
// Fill functions for the trending objects
//************************************************************************
//===============================================================================
bool TileTrend::Fill(double x, const TileCalib& tc, int runNr, double volt){
  gTrendLGped   .AddPoint     (x,tc.PedestalMeanL);
  gTrendLGped   .SetPointError(gTrendLGped.GetN()-1,0.,tc.PedestalSigL);
  if(tc.PedestalMeanL<MinLGped && tc.PedestalMeanL > -100) MinLGped=tc.PedestalMeanL;
  if(tc.PedestalMeanL>MaxLGped) MaxLGped=tc.PedestalMeanL;
  
  gTrendHGped   .AddPoint     (x,tc.PedestalMeanH);
  gTrendHGped   .SetPointError(gTrendHGped.GetN()-1,0.,tc.PedestalSigH);
  if(tc.PedestalMeanH<MinHGped && tc.PedestalMeanH > -100) MinHGped=tc.PedestalMeanH;
  if(tc.PedestalMeanH>MaxHGped) MaxHGped=tc.PedestalMeanH;

  gTrendLGpedwidth   .AddPoint     (x,tc.PedestalSigL);
  gTrendLGpedwidth   .SetPointError(gTrendLGpedwidth.GetN()-1,0.,0.);
  if(tc.PedestalSigL<MinLGpedwidth && tc.PedestalSigL > -100) MinLGpedwidth=tc.PedestalSigL;
  if(tc.PedestalSigL>MaxLGpedwidth) MaxLGpedwidth=tc.PedestalSigL;
  
  gTrendHGpedwidth   .AddPoint     (x,tc.PedestalSigH);
  gTrendHGpedwidth   .SetPointError(gTrendHGpedwidth.GetN()-1,0.,0.);
  if(tc.PedestalSigH<MinHGpedwidth && tc.PedestalSigH > -100) MinHGpedwidth=tc.PedestalSigH;
  if(tc.PedestalSigH>MaxHGpedwidth) MaxHGpedwidth=tc.PedestalSigH;
  
  gTrendLGscale .AddPoint     (x,tc.ScaleL       );
  gTrendLGscale .SetPointError(gTrendLGscale.GetN()-1,0.,tc.ScaleWidthL);
  if(tc.ScaleL<MinLGscale && tc.ScaleL > 0) MinLGscale=tc.ScaleL;
  if(tc.ScaleL>MaxLGscale) MaxLGscale=tc.ScaleL;
  
  gTrendHGscale .AddPoint     (x,tc.ScaleH       );
  gTrendHGscale .SetPointError(gTrendHGscale.GetN()-1,0.,tc.ScaleWidthH);
  if(tc.ScaleH<MinHGscale  && tc.ScaleH > 0) MinHGscale=tc.ScaleH;
  if(tc.ScaleH>MaxHGscale) MaxHGscale=tc.ScaleH;
  
  gTrendHGLGcorr.AddPoint     (x,tc.HGLGCorr     );
  gTrendHGLGcorr.SetPointError(gTrendHGLGcorr.GetN()-1,0.,0.);
  if(tc.HGLGCorr<MinHGLGcorr && tc.HGLGCorr > 0) MinHGLGcorr=tc.HGLGCorr;
  if(tc.HGLGCorr>MaxHGLGcorr && tc.HGLGCorr > 0) MaxHGLGcorr=tc.HGLGCorr;

  gTrendHGLGOffset.AddPoint     (x,tc.HGLGCorrOff     );
  gTrendHGLGOffset.SetPointError(gTrendHGLGOffset.GetN()-1,0.,0.);
  if(tc.HGLGCorrOff<MinHGLGOff && tc.HGLGCorrOff != -1000.) MinHGLGOff=tc.HGLGCorrOff;
  if(tc.HGLGCorrOff>MaxHGLGOff && tc.HGLGCorrOff != -1000.) MaxHGLGOff=tc.HGLGCorrOff;
  
  gTrendLGHGcorr.AddPoint     (x,tc.LGHGCorr     );
  gTrendLGHGcorr.SetPointError(gTrendLGHGcorr.GetN()-1,0.,0.);
  if(tc.LGHGCorr<MinLGHGcorr && tc.LGHGCorr > 0) MinLGHGcorr=tc.LGHGCorr;
  if(tc.LGHGCorr>MaxLGHGcorr && tc.LGHGCorr > 0) MaxLGHGcorr=tc.LGHGCorr;

  gTrendLGHGOffset.AddPoint     (x,tc.LGHGCorrOff     );
  gTrendLGHGOffset.SetPointError(gTrendLGHGOffset.GetN()-1,0.,0.);
  if(tc.LGHGCorrOff<MinLGHGOff && tc.LGHGCorrOff != -1000.) MinLGHGOff=tc.LGHGCorrOff;
  if(tc.LGHGCorrOff>MaxLGHGOff && tc.LGHGCorrOff != -1000.) MaxLGHGOff=tc.LGHGCorrOff;
  
  voltages.push_back(volt);
  runNrs.push_back(runNr);
  return true;
}

//===============================================================================
bool TileTrend::FillExtended(double x, int triggers, int runNr, TH1D* histHG, TH1D* histLG, TProfile* profLGHG ){
  
  if (extended == 1 || extended == 2 ){
    gTrendTrigger.AddPoint     (x,triggers     );
    gTrendTrigger.SetPointError(gTrendTrigger.GetN()-1,0.,0.);
    if(triggers<MinTrigg) MinTrigg  = triggers;
    if(triggers>MaxTrigg) MaxTrigg  = triggers;
    
    if (histHG){
      TH1D temp = *histHG;
      temp.SetName(Form("%s_Run%i",histHG->GetName(),runNr));
      temp.SetDirectory(0);
      temp.Scale(1./triggers);
      temp.GetYaxis()->SetTitle("Counts/ local mip trigger");
      temp.Rebin(2);
      if (MinHGSpec > 1./triggers) MinHGSpec = (double)1./triggers;
      if (MaxHGSpec < temp.GetMaximum()) MaxHGSpec = temp.GetMaximum();
      HGTriggRuns[runNr] = temp;
    }
    if (histLG){
      TH1D temp2 = *histLG;
      temp2.SetName(Form("%s_Run%i",histLG->GetName(),runNr));
      temp2.SetDirectory(0);
      temp2.Scale(1./triggers);
      temp2.GetYaxis()->SetTitle("Counts/ local mip trigger");
      if (MinLGSpec > 1./triggers) MinLGSpec = (double)1./triggers;
      if (MaxLGSpec < temp2.GetMaximum()) MaxLGSpec = temp2.GetMaximum();
      LGTriggRuns[runNr] = temp2;
    }
  }
  if (profLGHG){
    TProfile temp3 = *profLGHG;
    temp3.SetName(Form("%s_Run%i",profLGHG->GetName(),runNr));
    temp3.SetDirectory(0);
    // temp3.Scale(1./triggers);
    LGHGTriggRuns[runNr] = temp3;
  }
  
  if (extended == 4){
    if (histHG){
      TH1D temp = *histHG;
      temp.SetName(Form("%s_Run%i",histHG->GetName(),runNr));
      temp.SetDirectory(0);
      double scaler = 1./temp.GetEntries();
      temp.Scale(scaler);
      temp.GetYaxis()->SetTitle("Counts/ trigger");
      temp.Rebin(2);
      if (MinHGSpec > scaler) MinHGSpec = (double)scaler;
      if (MaxHGSpec < temp.GetMaximum()) MaxHGSpec = temp.GetMaximum();
      HGTriggRuns[runNr] = temp;
    }    
  }
  
  return true;
}

//===============================================================================
void TileTrend::FillMPV(double x, double hgmpv, double ehgmpv, double lgmpv, double elgmpv){
  gTrendHGLMPV.AddPoint     (x,hgmpv     );
  gTrendHGLMPV.SetPointError(gTrendHGLMPV.GetN()-1,0.,ehgmpv);
  if(hgmpv<MinHGMPV) MinHGMPV  = hgmpv;
  if(hgmpv>MaxHGMPV) MaxHGMPV  = hgmpv;
    
  gTrendLGLMPV.AddPoint     (x,lgmpv     );
  gTrendLGLMPV.SetPointError(gTrendLGLMPV.GetN()-1,0.,elgmpv);
  if(lgmpv<MinLGMPV) MinLGMPV  = lgmpv;
  if(lgmpv>MaxLGMPV) MaxLGMPV  = lgmpv;
}

//===============================================================================
void TileTrend::FillLSigma(double x, double hglsig, double ehglsig, double lglsig, double elglsig){
  gTrendHGLSigma.AddPoint     (x,hglsig     );
  gTrendHGLSigma.SetPointError(gTrendHGLSigma.GetN()-1,0.,ehglsig);
  if(hglsig<MinHGLSigma ) MinHGLSigma  = hglsig;
  if(hglsig>MaxHGLSigma ) MaxHGLSigma  = hglsig;
    
  gTrendLGLSigma.AddPoint     (x,lglsig     );
  gTrendLGLSigma.SetPointError(gTrendLGLSigma.GetN()-1,0.,elglsig);
  if(lglsig<MinLGLSigma ) MinLGLSigma  = lglsig;
  if(lglsig>MaxLGLSigma ) MaxLGLSigma  = lglsig;
}

//===============================================================================
void TileTrend::FillGSigma(double x, double hggsig, double ehggsig, double lggsig, double elggsig){
  gTrendHGGSigma.AddPoint     (x,hggsig     );
  gTrendHGGSigma.SetPointError(gTrendHGGSigma.GetN()-1,0.,ehggsig);
  if(hggsig<MinHGGSigma) MinHGGSigma  = hggsig;
  if(hggsig>MaxHGGSigma) MaxHGGSigma  = hggsig;
    
  gTrendLGGSigma.AddPoint     (x,lggsig     );
  gTrendLGGSigma.SetPointError(gTrendLGGSigma.GetN()-1,0.,elggsig);
  if(lggsig<MinLGGSigma) MinLGGSigma  = lggsig;
  if(lggsig>MaxLGGSigma) MaxLGGSigma  = lggsig;
}

//===============================================================================
void TileTrend::FillSB(double x, double sbsig, double sbnoise){
  gTrendSBNoise.AddPoint     (x,sbnoise     );
  gTrendSBNoise.SetPointError(gTrendSBNoise.GetN()-1,0.,0.);
  if(sbnoise<MinSBNoise) MinSBNoise  = sbnoise;
  if(sbnoise>MaxSBNoise) MaxSBNoise  = sbnoise;

  gTrendSBSignal.AddPoint     (x,sbsig     );
  gTrendSBSignal.SetPointError(gTrendSBSignal.GetN()-1,0.,0.);
  if(sbsig<MinSBSignal) MinSBSignal  = sbsig;
  if(sbsig>MaxSBSignal) MaxSBSignal  = sbsig;  
}

//===============================================================================
void TileTrend::FillCorrOffset(double x, double lghgoff, double lghgoff_e, double hglgoff,double hglgoff_e ){
  gTrendLGHGOffset.AddPoint     (x,lghgoff     );
  gTrendLGHGOffset.SetPointError(gTrendLGHGOffset.GetN()-1,0.,lghgoff_e);
  if(lghgoff<MinLGHGOff && lghgoff != -10000.) MinLGHGOff  = lghgoff;
  if(lghgoff>MaxLGHGOff && lghgoff != -10000.) MaxLGHGOff  = lghgoff;

  gTrendHGLGOffset.AddPoint     (x,hglgoff     );
  gTrendHGLGOffset.SetPointError(gTrendHGLGOffset.GetN()-1,0.,hglgoff_e);
  if(hglgoff<MinHGLGOff && hglgoff != -10000.) MinHGLGOff  = hglgoff;
  if(hglgoff>MaxHGLGOff && hglgoff != -10000.) MaxHGLGOff  = hglgoff;  
}


//************************************************************************
// Getter functions for individual run histograms
//************************************************************************
//===============================================================================
TH1D* TileTrend::GetHGTriggRun(int run){
  std::map<int, TH1D>::iterator currRun;
  currRun=HGTriggRuns.find(run);
  if(currRun!=HGTriggRuns.end()){
    return &currRun->second;
  } else {
    return nullptr;
  }
}
//===============================================================================
TH1D* TileTrend::GetLGTriggRun(int run){
  std::map<int, TH1D>::iterator currRun;
  currRun=LGTriggRuns.find(run);
  if(currRun!=LGTriggRuns.end()){
    return &currRun->second;
  } else {
    return nullptr;
  }
}

//===============================================================================
TProfile* TileTrend::GetLGHGTriggRun(int run){
  std::map<int, TProfile>::iterator currRun;
  currRun=LGHGTriggRuns.find(run);
  if(currRun!=LGHGTriggRuns.end()){
    return &currRun->second;
  } else {
    return nullptr;
  }
}

//************************************************************************
// Drawing functions for graphs
//************************************************************************
//===============================================================================
bool TileTrend::DrawLGped(TString opt){
  gTrendLGped.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawHGped(TString opt){
  gTrendHGped.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawLGscale(TString opt){
  gTrendLGscale.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawHGscale(TString opt){
  gTrendHGscale.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawHGLGcorr(TString opt){
  gTrendHGLGcorr.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawLGHGcorr(TString opt){
  gTrendLGHGcorr.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawTrigger(TString opt){
  gTrendTrigger.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawSBNoise(TString opt){
  gTrendSBNoise.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawSBSignal(TString opt){
  gTrendSBSignal.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawHGLMPV(TString opt){
  gTrendHGLMPV.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawLGLMPV(TString opt){
  gTrendLGLMPV.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawHGLSigma(TString opt){
  gTrendHGLSigma.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawLGLSigma(TString opt){
  gTrendLGLSigma.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawHGGSigma(TString opt){
  gTrendHGGSigma.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawLGGSigma(TString opt){
  gTrendLGGSigma.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawHGLGOffset(TString opt){
  gTrendHGLGOffset.Draw(opt.Data());
  return true;
}
//===============================================================================
bool TileTrend::DrawLGHGOffset(TString opt){
  gTrendLGHGOffset.Draw(opt.Data());
  return true;
}

//************************************************************************
// Set Drawing options
//************************************************************************
//===============================================================================
bool TileTrend::SetLineColor(uint col){
  gTrendLGped    .SetLineColor(col);
  gTrendHGped    .SetLineColor(col);
  gTrendLGpedwidth.SetLineColor(col);
  gTrendHGpedwidth.SetLineColor(col);
  gTrendLGscale  .SetLineColor(col);
  gTrendHGscale  .SetLineColor(col);
  gTrendHGLGcorr .SetLineColor(col);
  gTrendLGHGcorr .SetLineColor(col);
  gTrendHGLGOffset .SetLineColor(col);
  gTrendLGHGOffset .SetLineColor(col);
  if (extended == 1 || extended == 2 ){
    gTrendTrigger .SetLineColor(col);
    gTrendSBNoise .SetLineColor(col);
    gTrendSBSignal.SetLineColor(col);
  }
  if (extended == 1){
    gTrendHGLMPV  .SetLineColor(col);
    gTrendLGLMPV  .SetLineColor(col);
    gTrendHGLSigma.SetLineColor(col);
    gTrendLGLSigma.SetLineColor(col);
    gTrendHGGSigma.SetLineColor(col);
    gTrendLGGSigma.SetLineColor(col);
  }
  return true;
}
//===============================================================================
bool TileTrend::SetMarkerColor(uint col){
  gTrendLGped    .SetMarkerColor(col);
  gTrendHGped    .SetMarkerColor(col);
  gTrendLGpedwidth.SetMarkerColor(col);
  gTrendHGpedwidth.SetMarkerColor(col);
  gTrendLGscale  .SetMarkerColor(col);
  gTrendHGscale  .SetMarkerColor(col);
  gTrendHGLGcorr .SetMarkerColor(col);
  gTrendLGHGcorr .SetMarkerColor(col);
  gTrendHGLGOffset .SetMarkerColor(col);
  gTrendLGHGOffset .SetMarkerColor(col);
  if (extended == 1 || extended == 2 ){
    gTrendTrigger .SetMarkerColor(col);
    gTrendSBNoise .SetMarkerColor(col);
    gTrendSBSignal.SetMarkerColor(col);
  }
  if (extended == 1){
    gTrendHGLMPV  .SetMarkerColor(col);
    gTrendLGLMPV  .SetMarkerColor(col);
    gTrendHGLSigma.SetMarkerColor(col);
    gTrendLGLSigma.SetMarkerColor(col);
    gTrendHGGSigma.SetMarkerColor(col);
    gTrendLGGSigma.SetMarkerColor(col);
  }
  return true;
}
//===============================================================================
bool TileTrend::SetMarkerStyle(uint col){
  gTrendLGped    .SetMarkerStyle(col);
  gTrendHGped    .SetMarkerStyle(col);
  gTrendLGpedwidth.SetMarkerStyle(col);
  gTrendHGpedwidth.SetMarkerStyle(col);
  gTrendLGscale  .SetMarkerStyle(col);
  gTrendHGscale  .SetMarkerStyle(col);
  gTrendHGLGcorr .SetMarkerStyle(col);
  gTrendLGHGcorr .SetMarkerStyle(col);
  gTrendHGLGOffset .SetMarkerStyle(col);
  gTrendLGHGOffset .SetMarkerStyle(col);
  if (extended == 1 || extended == 2 ){
    gTrendTrigger .SetMarkerStyle(col);
    gTrendSBNoise .SetMarkerStyle(col);
    gTrendSBSignal.SetMarkerStyle(col);
  }
  if (extended == 1){
    gTrendHGLMPV  .SetMarkerStyle(col);
    gTrendLGLMPV  .SetMarkerStyle(col);
    gTrendHGLSigma.SetMarkerStyle(col);
    gTrendLGLSigma.SetMarkerStyle(col);
    gTrendHGGSigma.SetMarkerStyle(col);
    gTrendLGGSigma.SetMarkerStyle(col);
  }
  return true;
}
//===============================================================================
bool TileTrend::SetXAxisTitle(TString title){
  gTrendLGped    .GetXaxis()->SetTitle(title.Data());
  gTrendHGped    .GetXaxis()->SetTitle(title.Data());
  gTrendLGpedwidth.GetXaxis()->SetTitle(title.Data());
  gTrendHGpedwidth.GetXaxis()->SetTitle(title.Data());
  gTrendLGscale  .GetXaxis()->SetTitle(title.Data());
  gTrendHGscale  .GetXaxis()->SetTitle(title.Data());
  gTrendHGLGcorr .GetXaxis()->SetTitle(title.Data());
  gTrendLGHGcorr .GetXaxis()->SetTitle(title.Data());
  gTrendHGLGOffset.GetXaxis()->SetTitle(title.Data());
  gTrendLGHGOffset.GetXaxis()->SetTitle(title.Data());
  if (extended == 1 || extended == 2 ){
    gTrendTrigger .GetXaxis()->SetTitle(title.Data());
    gTrendSBNoise .GetXaxis()->SetTitle(title.Data());
    gTrendSBSignal.GetXaxis()->SetTitle(title.Data());
  }
  if (extended == 1){
    gTrendHGLMPV  .GetXaxis()->SetTitle(title.Data());
    gTrendLGLMPV  .GetXaxis()->SetTitle(title.Data());
    gTrendHGLSigma.GetXaxis()->SetTitle(title.Data());
    gTrendLGLSigma.GetXaxis()->SetTitle(title.Data());
    gTrendHGGSigma.GetXaxis()->SetTitle(title.Data());
    gTrendLGGSigma.GetXaxis()->SetTitle(title.Data());
  }
  return true;
}

//************************************************************************
// Sort
//************************************************************************
void TileTrend::Sort(){
  gTrendLGped    .Sort();
  gTrendHGped    .Sort();
  gTrendLGpedwidth.Sort();
  gTrendHGpedwidth.Sort();
  gTrendLGscale  .Sort();
  gTrendHGscale  .Sort();
  gTrendHGLGcorr .Sort();
  gTrendLGHGcorr .Sort();
  gTrendHGLGOffset .Sort();
  gTrendLGHGOffset .Sort();
  if (extended == 1 || extended == 2 ){
    gTrendTrigger .Sort();
    gTrendSBNoise .Sort();
    gTrendSBSignal.Sort();
  }
  if (extended == 1){
    gTrendHGLMPV  .Sort();
    gTrendLGLMPV  .Sort();
    gTrendHGLSigma.Sort();
    gTrendLGLSigma.Sort();
    gTrendHGGSigma.Sort();
    gTrendLGGSigma.Sort();
  }
  return;  
}

//************************************************************************
// Write 
//************************************************************************
bool TileTrend::Write(TFile* f){
  f->cd();
  gTrendLGped    .Write();
  gTrendHGped    .Write();
  gTrendLGpedwidth.Write();
  gTrendHGpedwidth.Write();
  gTrendLGscale  .Write();
  gTrendHGscale  .Write();
  gTrendHGLGcorr .Write();
  gTrendLGHGcorr .Write();
  gTrendHGLGOffset .Write();
  gTrendLGHGOffset .Write();
  if (extended == 1 || extended == 2 ){
    gTrendTrigger .Write();
    gTrendSBNoise .Write();
    gTrendSBSignal.Write();
  }
  if (extended == 1){
    gTrendHGLMPV  .Write();
    gTrendLGLMPV  .Write();
    gTrendHGLSigma.Write();
    gTrendLGLSigma.Write();
    gTrendHGGSigma.Write();
    gTrendLGGSigma.Write();
  }
  return true;
}
