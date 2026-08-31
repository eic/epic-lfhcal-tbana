#include "TileTrend.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

ClassImp(TileTrend);

//************************************************************************
// Fill functions for the trending objects
//************************************************************************
//===============================================================================
bool TileTrend::Fill(double x, const TileCalib& tc, int runNr, double volt, int pdg,  double hgmaxerr, double lgmaxerr, double tempE, double temper){
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
  if (lgmaxerr > -10000.)
    gTrendLGscale .SetPointError(gTrendLGscale.GetN()-1,0.,lgmaxerr);
  else 
    gTrendLGscale .SetPointError(gTrendLGscale.GetN()-1,0.,tc.ScaleWidthL);
  if(tc.ScaleL<MinLGscale && tc.ScaleL > 0) MinLGscale=tc.ScaleL;
  if(tc.ScaleL>MaxLGscale) MaxLGscale=tc.ScaleL;
  
  gTrendHGscale .AddPoint     (x,tc.ScaleH       );
  if (hgmaxerr > -10000.)
    gTrendHGscale .SetPointError(gTrendHGscale.GetN()-1,0.,hgmaxerr);
  else 
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
  pdgs.push_back(pdg);
  energy.push_back(tempE);
  temp.push_back(temper);
  return true;
}

//===============================================================================
bool TileTrend::FillExtended(double x, int triggers, int runNr, TH1D* histHG, TH1D* histLG, TProfile* profLGHG, TProfile* wave ){
  
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
    // std::cout << "filling profile " <<  profLGHG->GetName() << std::endl;
    //std::cout << "setting LG-HG profile " << profLGHG->GetName() << std::endl;
    TProfile temp3 = *profLGHG;
    temp3.SetName(Form("%s_Run%i",profLGHG->GetName(),runNr));
    temp3.SetDirectory(0);
    // temp3.Scale(1./triggers);
    LGHGTriggRuns[runNr] = temp3;
  }
  
  if (wave){
      TProfile temp = *wave;
      // std::cout << "filling wave " <<  wave->GetName() << std::endl;
      temp.SetName(Form("%s_Run%i",wave->GetName(),runNr));
      temp.SetDirectory(0);
      if (MaxInjADC < temp.GetMaximum()) MaxInjADC = temp.GetMaximum();
      Wave1DProf[runNr] = temp;
    }
  
  if (extended == 4){
    if (histHG){
      TH1D temp = *histHG;
      temp.SetName(Form("%s_Run%i",histHG->GetName(),runNr));
      temp.SetDirectory(0);
      double scaler = 1./temp.GetEntries();
      temp.Scale(scaler);
      temp.GetYaxis()->SetTitle("Counts/ trigger");
      TString name = histHG->GetName();
      if (name.Contains("TOT"))
        temp.Rebin(8);
      else 
        temp.Rebin(2);
      if (MinHGSpec > scaler) MinHGSpec = (double)scaler;
      if (MaxHGSpec < temp.GetMaximum()) MaxHGSpec = temp.GetMaximum();
      HGTriggRuns[runNr] = temp;
    }
    if (histLG){
      TH1D temp2 = *histLG;
      temp2.SetName(Form("%s_Run%i",histLG->GetName(),runNr));
      temp2.SetDirectory(0);
      temp2.Scale(1./triggers);
      temp2.GetYaxis()->SetTitle("Counts/ trigger");
      if (MinLGSpec > 1./triggers) MinLGSpec = (double)1./triggers;
      if (MaxLGSpec < temp2.GetMaximum()) MaxLGSpec = temp2.GetMaximum();
      LGTriggRuns[runNr] = temp2;
    }
    if (wave){
      TProfile temp = *wave;
      // std::cout << "filling wave " <<  wave->GetName() << std::endl;
      temp.SetName(Form("%s_Run%i",wave->GetName(),runNr));
      temp.SetDirectory(0);
      if (MaxInjADC < temp.GetMaximum()) MaxInjADC = temp.GetMaximum();
      Wave1DProf[runNr] = temp;
    }
  }
  
  return true;
}

//===============================================================================
bool TileTrend::FillInjection(
                                double x, double ped, int runNr, 
                                TProfile* wave, TProfile* toa, TProfile* tot, 
                                double val_rf, double val_cf, double val_cfcomp, double val_cc, double val_inj
                              ){
  
  if (extended != 3 ){
    std::cout << "This option Filling option isn't implemented for the current extended option" << std::endl;
    return false;
  }

  gTrendHGped.AddPoint     (x,ped     );
  gTrendHGped.SetPointError     (gTrendHGped.GetN()-1,0.,0.);

  if(ped<MinHGped) MinHGped  = ped;
  if(ped>MaxHGped) MaxHGped  = ped;
    
  if (wave){
    TProfile temp = *wave;
    temp.SetName(Form("%s_Run%i",wave->GetName(),runNr));
    temp.SetDirectory(0);
    MinHGSpec = 0;
    if (MaxInjADC < temp.GetMaximum()) MaxInjADC = temp.GetMaximum();
    Wave1DProf[runNr] = temp;
  }
  
  if (toa){
    TProfile temp = *toa;
    temp.SetName(Form("%s_Run%i",toa->GetName(),runNr));
    temp.SetDirectory(0);
    TOAProf[runNr] = temp;
  }
  if (tot){
    TProfile temp = *tot;
    temp.SetName(Form("%s_Run%i",tot->GetName(),runNr));
    if (MaxInjTOT < temp.GetMaximum()) MaxInjTOT = temp.GetMaximum();
    temp.SetDirectory(0);
    TOTProf[runNr] = temp;
  }
  
  rf.push_back(val_rf);
  cf.push_back(val_cf);
  cfcomp.push_back(val_cfcomp);
  cc.push_back(val_cc);
  inj.push_back(val_inj);
  
  return true;
}

//===============================================================================
void TileTrend::FillHGCROCSetting (double val_rf, double val_cf, double val_cfcomp, double val_cc, double val_inj){
  rf.push_back(val_rf);
  cf.push_back(val_cf);
  cfcomp.push_back(val_cfcomp);
  cc.push_back(val_cc);
  inj.push_back(val_inj);
}

//===============================================================================
bool  TileTrend::FillInjectionDACVal  ( double x, double ped, double adc, double toa, double tot, 
                                        int adcSatN, int totSatN, int nTOA, int nSampToA){
  if (extended != 4 ){
    std::cout << "This option Filling option isn't implemented for the current extended option" << std::endl;
    return false;
  }

  gTrendHGped.AddPoint     (x,ped     );
  gTrendHGped.SetPointError     (gTrendHGped.GetN()-1,0.,0.);
  if(ped<MinHGped) MinHGped  = ped;
  if(ped>MaxHGped) MaxHGped  = ped;
    
  gTrendADCMax.AddPoint     (x,adc     );
  gTrendADCMax.SetPointError     (gTrendADCMax.GetN()-1,0.,0.);
  if(adc<MinADCmax) MinADCmax  = adc;
  if(adc>MaxADCmax) MaxADCmax  = adc;
    
  gTrendTOA.AddPoint     (x,toa     );
  gTrendTOA.SetPointError     (gTrendTOA.GetN()-1,0.,0.);
  if(toa<MinTOA) MinTOA  = toa;
  if(toa>MaxTOA) MaxTOA  = toa;

  gTrendTOT.AddPoint     (x,tot     );
  gTrendTOT.SetPointError     (gTrendTOT.GetN()-1,0.,0.);
  if(tot<MinTOT) MinTOT  = tot;
  if(tot>MaxTOT) MaxTOT  = tot;
 
  if (MaxTOT > 4095 ) MaxTOT = 4095;
  
  
  gTrendADCSaturated.AddPoint     (x,adcSatN     );
  gTrendADCSaturated.SetPointError     (gTrendADCSaturated.GetN()-1,0.,0.);
  if(adcSatN<MinADCsat) MinADCsat  = adcSatN;
  if(adcSatN>MaxADCsat) MaxADCsat  = adcSatN;
  
  
  gTrendTOTSaturated.AddPoint     (x,totSatN     );
  gTrendTOTSaturated.SetPointError     (gTrendTOTSaturated.GetN()-1,0.,0.);
  if(totSatN<MinTOTsat) MinTOTsat  = totSatN;
  if(totSatN>MaxTOTsat) MaxTOTsat  = totSatN;
  
  gTrendNSampTOA.AddPoint     (x,nSampToA     );
  gTrendNSampTOA.SetPointError     (gTrendNSampTOA.GetN()-1,0.,0.);
  if(nSampToA<MinNSampTOA) MinNSampTOA  = nSampToA;
  if(nSampToA>MaxNSampTOA) MaxNSampTOA  = nSampToA;
  
  gTrendNTOA.AddPoint     (x,nTOA     );
  gTrendNTOA.SetPointError     (gTrendNTOA.GetN()-1,0.,0.);
  if(nTOA<MinNTOA) MinNTOA  = nTOA;
  if(nTOA>MaxNTOA) MaxNTOA  = nTOA;
  
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

//===============================================================================
TProfile* TileTrend::GetWave1DRun(int run){
  std::map<int, TProfile>::iterator currRun;
  currRun=Wave1DProf.find(run);
  if(currRun!=Wave1DProf.end()){
    return &currRun->second;
  } else {
    return nullptr;
  }
}

//===============================================================================
TProfile* TileTrend::GetTOARun(int run){
  std::map<int, TProfile>::iterator currRun;
  currRun=TOAProf.find(run);
  if(currRun!=TOAProf.end()){
    return &currRun->second;
  } else {
    return nullptr;
  }
}

//===============================================================================
TProfile* TileTrend::GetTOTRun(int run){
  std::map<int, TProfile>::iterator currRun;
  currRun=TOTProf.find(run);
  if(currRun!=TOTProf.end()){
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

//===============================================================================
bool TileTrend::DrawADCmax(TString opt){
  gTrendADCMax.Draw(opt.Data());
  return true;
}

//===============================================================================
bool TileTrend::DrawADCsat(TString opt){
  gTrendADCSaturated.Draw(opt.Data());
  return true;
}

//===============================================================================
bool TileTrend::DrawTOT(TString opt){
  gTrendTOT.Draw(opt.Data());
  return true;
}

//===============================================================================
bool TileTrend::DrawTOTsat(TString opt){
  gTrendTOTSaturated.Draw(opt.Data());
  return true;
}

//===============================================================================
bool TileTrend::DrawTOA(TString opt){
  gTrendTOA.Draw(opt.Data());
  return true;
}

//===============================================================================
bool TileTrend::DrawNSampTOA(TString opt){
  gTrendNSampTOA.Draw(opt.Data());
  return true;
}

//===============================================================================
bool TileTrend::DrawNTOA(TString opt){
  gTrendNTOA.Draw(opt.Data());
  return true;
}

//************************************************************************
// Set Drawing options
//************************************************************************
//===============================================================================
bool TileTrend::SetLineColor(uint col){
  if (extended < 3){
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
  } else if (extended == 3){
    gTrendHGped     .SetLineColor(col);
    gTrendHGpedwidth.SetLineColor(col);
  } else if (extended == 4){
    gTrendHGped       .SetLineColor(col);
    gTrendHGpedwidth  .SetLineColor(col);
    gTrendADCSaturated.SetLineColor(col);
    gTrendTOA         .SetLineColor(col);
    gTrendNSampTOA    .SetLineColor(col);
    gTrendNTOA        .SetLineColor(col);
    gTrendTOT         .SetLineColor(col);
    gTrendTOTSaturated.SetLineColor(col);
  }
  return true;
}
//===============================================================================
bool TileTrend::SetMarkerColor(uint col){
  if (extended < 3){
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
  } else if (extended == 3){
    gTrendHGped     .SetMarkerColor(col);
    gTrendHGpedwidth.SetMarkerColor(col);
  } else if (extended == 4){
    gTrendHGped       .SetMarkerColor(col);
    gTrendHGpedwidth  .SetMarkerColor(col);
    gTrendADCSaturated.SetMarkerColor(col);
    gTrendTOA         .SetMarkerColor(col);
    gTrendNSampTOA    .SetMarkerColor(col);
    gTrendNTOA        .SetMarkerColor(col);
    gTrendTOT         .SetMarkerColor(col);
    gTrendTOTSaturated.SetMarkerColor(col);
  }
  return true;
}
//===============================================================================
bool TileTrend::SetMarkerStyle(uint col){
  if (extended < 3){
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
  } else if (extended == 3){
    gTrendHGped     .SetMarkerStyle(col);
    gTrendHGpedwidth.SetMarkerStyle(col);
  } else if (extended == 4){
    gTrendHGped       .SetMarkerStyle(col);
    gTrendHGpedwidth  .SetMarkerStyle(col);
    gTrendADCSaturated.SetMarkerStyle(col);
    gTrendTOA         .SetMarkerStyle(col);
    gTrendNSampTOA    .SetMarkerStyle(col);
    gTrendNTOA        .SetMarkerStyle(col);
    gTrendTOT         .SetMarkerStyle(col);
    gTrendTOTSaturated.SetMarkerStyle(col);
  }  
  return true;
}
//===============================================================================
bool TileTrend::SetXAxisTitle(TString title){
  if (extended < 3){
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
  } else if (extended == 3){
    gTrendHGped    .GetXaxis()->SetTitle(title.Data());
    gTrendHGpedwidth.GetXaxis()->SetTitle(title.Data());    
  } else if (extended == 4){
    gTrendHGped       .GetXaxis()->SetTitle(title.Data());
    gTrendHGpedwidth  .GetXaxis()->SetTitle(title.Data());    
    gTrendADCSaturated.GetXaxis()->SetTitle(title.Data());    
    gTrendTOA         .GetXaxis()->SetTitle(title.Data());    
    gTrendNSampTOA    .GetXaxis()->SetTitle(title.Data());    
    gTrendNTOA        .GetXaxis()->SetTitle(title.Data());    
    gTrendTOT         .GetXaxis()->SetTitle(title.Data());    
    gTrendTOTSaturated.GetXaxis()->SetTitle(title.Data());    
  }
  return true;
}

//************************************************************************
// Sort
//************************************************************************
void TileTrend::Sort(){
  if (extended < 3){
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
  } else if (extended == 3){
    gTrendHGped    .Sort();
    gTrendHGpedwidth.Sort();
  } else if (extended == 4){
    gTrendHGped       .Sort();
    gTrendADCMax      .Sort();
    gTrendADCSaturated.Sort();
    gTrendTOA         .Sort();
    gTrendNSampTOA    .Sort();
    gTrendNTOA        .Sort();
    gTrendTOT         .Sort();
    gTrendTOTSaturated.Sort();
  } 
  return;  
}

//************************************************************************
// Write 
//************************************************************************
bool TileTrend::Write(TFile* f){
  f->cd();
  TDirectoryFile* dirIndCells = (TDirectoryFile*)f->Get("IndividualCells");
  if (!dirIndCells)
    f->mkdir("IndividualCells");
  f->cd("IndividualCells");
  if (extended < 3){
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
  }
  if (extended == 3){
    gTrendHGped    .Write();
    gTrendHGpedwidth.Write();
  }
  if (extended == 4){
    gTrendHGped       .Write();
    gTrendHGpedwidth  .Write();
    gTrendADCMax      .Write();
    gTrendADCSaturated.Write();
    gTrendTOA         .Write();
    gTrendNSampTOA    .Write();
    gTrendNTOA        .Write();
    gTrendTOT         .Write();
    gTrendTOTSaturated.Write();
  } 
  
  return true;
}

//************************************************************************
// Write 
//************************************************************************
bool TileTrend::Write(){
  if (extended < 3){
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
  }
  if (extended == 3){
    gTrendHGped    .Write();
    gTrendHGpedwidth.Write();
  }
  if (extended == 4){
    gTrendHGped       .Write();
    gTrendHGpedwidth  .Write();
    gTrendADCMax      .Write();
    gTrendADCSaturated.Write();
    gTrendTOA         .Write();
    gTrendNSampTOA    .Write();
    gTrendNTOA        .Write();
    gTrendTOT         .Write();
    gTrendTOTSaturated.Write();
  } 
  
  return true;
}

void TileTrend::SetLabelPerRun(TString label){
  labels.push_back(label);
}

//*************************************************************************
// Labeling legend entries
//*************************************************************************
TString TileTrend::GetLabelLegend( RunInfo commonRunInfo, int runIndex, int nSameSettings){
  
  TString labelLegend = "";
  if (commonRunInfo.species.Contains("injection")){
    if (nSameSettings == 6){
      if (commonRunInfo.vop < -9999) labelLegend = Form("%.1f",(double)GetVoltage(runIndex));
      if (commonRunInfo.rf < -9999) labelLegend = Form("%.1f",ReturnRFValue(GetRF(runIndex)));
      if (commonRunInfo.cf < -9999) labelLegend = Form("%.0f",ReturnCFValue(GetCF(runIndex)));
      if (commonRunInfo.cfcomp < -9999) labelLegend = Form("%.0f",ReturnCFCompValue(GetCFComp(runIndex)));
      if (commonRunInfo.cc < -9999)  labelLegend = Form("%.3f",ReturnCCValue(GetCC(runIndex)));
      if (commonRunInfo.injDAC < -9999)  labelLegend = Form("%.0f",GetInj(runIndex));
      if (commonRunInfo.energy < -9999)  labelLegend = Form("%.0f",GetEnergy(runIndex));
    } else if (nSameSettings == 5){
      if (commonRunInfo.vop < -9999) labelLegend = Form("%.1f ",(double)GetVoltage(runIndex));
      if (commonRunInfo.rf < -9999) labelLegend = labelLegend+Form("%.1f ",ReturnRFValue(GetRF(runIndex)));
      if (commonRunInfo.cf < -9999) labelLegend = labelLegend+Form("%.0f ",ReturnCFValue(GetCF(runIndex)));
      if (commonRunInfo.cfcomp < -9999) labelLegend = labelLegend+Form("%.0f ",ReturnCFCompValue(GetCFComp(runIndex)));
      if (commonRunInfo.cc < -9999)  labelLegend = labelLegend+Form("%.3f ",ReturnCCValue(GetCC(runIndex)));
      if (commonRunInfo.injDAC < -9999)  labelLegend = labelLegend+Form("%.0f",GetInj(runIndex));
      if (commonRunInfo.energy < -9999)  labelLegend = labelLegend+Form("%.0f",GetEnergy(runIndex));
    }
  } else if (commonRunInfo.species.Contains("laser")){
    if (commonRunInfo.energy < -9999)  labelLegend = Form("%.0f",GetEnergy(runIndex));
    if (commonRunInfo.temp < -9999)  labelLegend = Form("%.0f",GetTemp(runIndex));
  // labeling for waveform compare from beam data 5 same settings
  } else if (nSameSettings == 5){
    if (commonRunInfo.vop < -9999) labelLegend = Form("%.1f",(double)GetVoltage(runIndex));
    if (commonRunInfo.rf < -9999) labelLegend = Form("%.1f",ReturnRFValue(GetRF(runIndex)));
    if (commonRunInfo.cf < -9999) labelLegend = Form("%.0f",ReturnCFValue(GetCF(runIndex)));
    if (commonRunInfo.cfcomp < -9999) labelLegend = Form("%.0f",ReturnCFCompValue(GetCFComp(runIndex)));
    if (commonRunInfo.cc < -9999)  labelLegend = Form("%.3f",ReturnCCValue(GetCC(runIndex)));
    if (commonRunInfo.energy < -9999)  labelLegend = Form("%.0f",GetEnergy(runIndex));
  // labeling for waveform compare from beam data 4 same settings    
  } else if (nSameSettings == 4){
    if (commonRunInfo.vop < -9999) labelLegend = Form("%.1f ",(double)GetVoltage(runIndex));
    if (commonRunInfo.rf < -9999) labelLegend = labelLegend+Form("%.1f ",ReturnRFValue(GetRF(runIndex)));
    if (commonRunInfo.cf < -9999) labelLegend = labelLegend+Form("%.0f ",ReturnCFValue(GetCF(runIndex)));
    if (commonRunInfo.cfcomp < -9999) labelLegend = labelLegend+Form("%.0f ",ReturnCFCompValue(GetCFComp(runIndex)));
    if (commonRunInfo.cc < -9999)  labelLegend = labelLegend+Form("%.3f ",ReturnCCValue(GetCC(runIndex)));
    if (commonRunInfo.energy < -9999)  labelLegend = labelLegend+Form("%.0f",GetEnergy(runIndex));
  }
    
    
  if (labelLegend.CompareTo("") == 0)
    labelLegend = Form("%i",GetRunNr(runIndex) );
    
  return   labelLegend;
}


//*************************************************************************
// Printing min & max ranges for all properties
//*************************************************************************
void TileTrend::PrintMinMaxRanges(){
  std::cout << "=================================================" << std::endl;
  std::cout << "===== Printing min and max ranges ===============" << std::endl; 
  std::cout << "=================================================" << std::endl;
  std::cout << " +++++++ Cell ID: " << CellID << "+++++++++++++++" << std::endl;
  std::cout << "HG Ped :\t"       << MinHGped       << "\t" << MaxHGped       << std::endl;
  std::cout << "HG Ped width :\t" << MinHGpedwidth  << "\t" << MaxHGpedwidth  << std::endl;
  std::cout << "HG Scale :\t"     << MinHGscale     << "\t" << MaxHGscale     << std::endl;
  std::cout << "LG-HG Corr :\t"   << MinLGHGcorr    << "\t" << MaxLGHGcorr    << std::endl;
  std::cout << "LG-HG Off :\t"    << MinLGHGOff     << "\t" << MaxLGHGOff     << std::endl;
  std::cout << "Trigg :\t"        << MinTrigg       << "\t" << MaxTrigg       << std::endl;
  std::cout << "HG Spec :\t"      << MinHGSpec      << "\t" << MaxHGSpec      << std::endl;
  std::cout << "HG MPV :\t"       << MinHGMPV       << "\t" << MaxHGMPV       << std::endl;
  std::cout << "HG Land Sigma :\t" << MinHGLSigma   << "\t" << MaxHGLSigma    << std::endl;
  std::cout << "HG Gaus Sigma :\t" << MinHGGSigma   << "\t" << MaxHGGSigma    << std::endl;
  std::cout << "-------------------------------------------------" << std::endl;
  std::cout << "LG Ped :\t"       << MinLGped       << "\t" << MaxLGped       << std::endl;
  std::cout << "LG Ped width :\t" << MinLGpedwidth  << "\t" << MaxLGpedwidth  << std::endl;
  std::cout << "LG Scale :\t"     << MinLGscale     << "\t" << MaxLGscale     << std::endl;
  std::cout << "HG-LG Corr :\t"   << MinHGLGcorr    << "\t" << MaxHGLGcorr    << std::endl;
  std::cout << "HG-LG Off :\t"    << MinHGLGOff     << "\t" << MaxHGLGOff     << std::endl;
  std::cout << "LG Spec :\t"      << MinLGSpec      << "\t" << MaxLGSpec      << std::endl;
  std::cout << "LG MPV :\t"       << MinLGMPV       << "\t" << MaxLGMPV       << std::endl;
  std::cout << "LG Land Sigma :\t" << MinLGLSigma   << "\t" << MaxLGLSigma    << std::endl;
  std::cout << "LG Gaus Sigma :\t" << MinLGGSigma   << "\t" << MaxLGGSigma    << std::endl;
  std::cout << "-------------------------------------------------" << std::endl;
  std::cout << "S/B Noise :\t"    << MinSBNoise     << "\t" << MaxSBNoise     << std::endl;
  std::cout << "S/B Signal :\t"   << MinSBSignal    << "\t" << MaxSBSignal    << std::endl;
  std::cout << "inj Dac :\t"      << MaxInjADC                                << std::endl;
  std::cout << "inj TOT :\t"      << MaxInjTOT                                << std::endl;
  std::cout << "ADC max :\t"      << MinADCmax      << "\t"<< MaxADCmax       << std::endl;
  std::cout << "ADC sat :\t"      << MinADCsat      << "\t"<< MaxADCsat       << std::endl;
  std::cout << "TOT :\t"          << MinTOT         << "\t"<< MaxTOT          << std::endl;
  std::cout << "TOT sat :\t"      << MinTOTsat      << "\t"<< MaxTOTsat       << std::endl;
  std::cout << "TOA :\t"          << MinTOA         << "\t"<< MaxTOA          << std::endl;
  std::cout << "n TOA :\t"        << MinNSampTOA    << "\t"<< MaxNSampTOA     << std::endl;
  std::cout << "nSample TOA :\t"  << MinNSampTOA    << "\t"<< MaxNSampTOA     << std::endl;
  std::cout << "n TOA :\t"        << MinNTOA        << "\t"<< MaxNTOA         << std::endl;
  std::cout << "=================================================" << std::endl;
}

TGraphErrors* TileTrend::GetTrendingBasedOnOption(int option){
  if (option == 0)       return GetHGped();
  else if (option == 1)  return GetLGped();
  else if (option == 2)  return GetHGScale();            
  else if (option == 3)  return GetLGScale();
  else if (option == 4)  return GetLGHGcorr();
  else if (option == 5)  return GetHGLGcorr();
  else if (option == 6)  return GetTrigger();
  else if (option == 7)  return GetSBSignal();
  else if (option == 8)  return GetSBNoise();
  else if (option == 9)  return GetHGLMPV();
  else if (option == 10) return GetLGLMPV();
  else if (option == 11) return GetHGLSigma();
  else if (option == 12) return GetLGLSigma();
  else if (option == 13) return GetHGGSigma();
  else if (option == 14) return GetLGGSigma();
  else if (option == 15) return GetHGpedwidth();
  else if (option == 16) return GetLGpedwidth();
  else if (option == 17) return GetLGHGOff();
  else if (option == 18) return GetHGLGOff();
    // special mode for injections
  else if (option == 30) return GetADCmax();
  else if (option == 31) return GetADCsat();
  else if (option == 32) return GetTOT();
  else if (option == 33) return GetTOTsat();
  else if (option == 34) return GetTOA();
  else if (option == 35) return GetNSampTOA();
  else if (option == 36) return GetNTOA();
  else return nullptr;
  return nullptr;
}

void TileTrend::GetMinMaxBasedOnOptionAndCompare(int option, Double_t &min, Double_t &max){
  if (option == 0){
    if(min>GetMinHGped()) min=GetMinHGped();
    if(max<GetMaxHGped()) max=GetMaxHGped();
  } else if (option == 1){
    if(min>GetMinLGped()) min=GetMinLGped();
    if(max<GetMaxLGped()) max=GetMaxLGped();
  } else if (option == 2){
    if(min>GetMinHGscale()) min=GetMinHGscale();
    if(max<GetMaxHGscale()) max=GetMaxHGscale();
  } else if (option == 3){
    if(min>GetMinLGscale()) min=GetMinLGscale();
    if(max<GetMaxLGscale()) max=GetMaxLGscale();
  } else if (option == 4){
    if(min>GetMinLGHGcorr()) min=GetMinLGHGcorr();
    if(max<GetMaxLGHGcorr()) max=GetMaxLGHGcorr();
  } else if (option == 5){
    if(min>GetMinHGLGcorr()) min=GetMinHGLGcorr();
    if(max<GetMaxHGLGcorr()) max=GetMaxHGLGcorr();          
  } else if (option == 6){
    if(min>GetMinTrigg()) min=GetMinTrigg();
    if(max<GetMaxTrigg()) max=GetMaxTrigg();          
  } else if (option == 7){
    if(min>GetMinSBSignal()) min=GetMinSBSignal();
    if(max<GetMaxSBSignal()) max=GetMaxSBSignal();          
  } else if (option == 8){
    if(min>GetMinSBNoise()) min=GetMinSBNoise();
    if(max<GetMaxSBNoise()) max=GetMaxSBNoise();          
  } else if (option == 9){
    if(min>GetMinHGMPV()) min=GetMinHGMPV();
    if(max<GetMaxHGMPV()) max=GetMaxHGMPV();          
  } else if (option == 10){
    if(min>GetMinLGMPV()) min=GetMinLGMPV();
    if(max<GetMaxLGMPV()) max=GetMaxLGMPV();          
  } else if (option == 11){
    if(min>GetMinHGLSigma()) min=GetMinHGLSigma();
    if(max<GetMaxHGLSigma()) max=GetMaxHGLSigma();          
  } else if (option == 12){
    if(min>GetMinLGLSigma()) min=GetMinLGLSigma();
    if(max<GetMaxLGLSigma()) max=GetMaxLGLSigma();          
  } else if (option == 13){
    if(min>GetMinHGGSigma()) min=GetMinHGGSigma();
    if(max<GetMaxHGGSigma()) max=GetMaxHGGSigma();          
  } else if (option == 14){
    if(min>GetMinLGGSigma()) min=GetMinLGGSigma();
    if(max<GetMaxLGGSigma()) max=GetMaxLGGSigma();          
  } else if (option == 15){
    if(min>GetMinHGpedwidth()) min=GetMinHGpedwidth();
    if(max<GetMaxHGpedwidth()) max=GetMaxHGpedwidth();          
  } else if (option == 16){
    if(min>GetMinLGpedwidth()) min=GetMinLGpedwidth();
    if(max<GetMaxLGpedwidth()) max=GetMaxLGpedwidth();          
  } else if (option == 17){
    if(min>GetMinLGHGOffset()) min=GetMinLGHGOffset();
    if(max<GetMaxLGHGOffset()) max=GetMaxLGHGOffset();          
  } else if (option == 18){
    if(min>GetMinHGLGOffset()) min=GetMinHGLGOffset();
    if(max<GetMaxHGLGOffset()) max=GetMaxHGLGOffset();          
  } else if (option == 19){
    if(min>GetMinHGped()) min=GetMinHGped();
    if(max<GetMaxHGped()) max=GetMaxHGped();
    if(min>GetMinLGped()) min=GetMinLGped();
    if(max<GetMaxLGped()) max=GetMaxLGped();
  } else if (option == 20){
    if(min>GetMinHGpedwidth()) min=GetMinHGpedwidth();
    if(max<GetMaxHGpedwidth()) max=GetMaxHGpedwidth();          
    if(min>GetMinLGpedwidth()) min=GetMinLGpedwidth();
    if(max<GetMaxLGpedwidth()) max=GetMaxLGpedwidth();          
  } else if (option == 30){
    if(min>GetMinADCmax()) min=GetMinADCmax();
    if(max<GetMaxADCmax()) max=GetMaxADCmax();          
  } else if (option == 31){
    if(min>GetMinADCsat()) min=GetMinADCsat();
    if(max<GetMaxADCsat()) max=GetMaxADCsat();          
  } else if (option == 32){
    if(min>GetMinTOT()) min=GetMinTOT();
    if(max<GetMaxTOT()) max=GetMaxTOT();          
  } else if (option == 33){
    if(min>GetMinTOTsat()) min=GetMinTOTsat();
    if(max<GetMaxTOTsat()) max=GetMaxTOTsat();          
  } else if (option == 34){
    if(min>GetMinTOA()) min=GetMinTOA();
    if(max<GetMaxTOA()) max=GetMaxTOA();          
  } else if (option == 35){ 
    if(min>GetMinNSampTOA()) min=GetMinNSampTOA();
    if(max<GetMaxNSampTOA()) max=GetMaxNSampTOA();          
  } else if (option == 36){
    if(min>GetMinNTOA()) min=GetMinNTOA();
    if(max<GetMaxNTOA()) max=GetMaxNTOA();          
  }
  return;
}
