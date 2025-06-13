#include "TileSpectra.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

ClassImp(TileSpectra);

int TileSpectra::GetCellID(){
  return cellID;
}

bool TileSpectra::Fill(double l, double h){
  hspectraLG.Fill(l);
  hspectraHG.Fill(h);
  hspectraLGHG.Fill(l,h);
  hspectraHGLG.Fill(h,l);
  return true;
}

bool TileSpectra::FillSpectra(double l, double h){
  hspectraLG.Fill(l);
  hspectraHG.Fill(h);
  return true;
}

bool TileSpectra::FillExt(double l, double h, double e, double lheq){
  hspectraLG.Fill(l);
  hspectraHG.Fill(h);
  if (extend ==  1){
    hcombined.Fill(e);
    if (h < 3500)
      hspectraLGHG.Fill(l,e);
    hspectraHGLG.Fill(l,lheq-h);
  } else if (extend ==  2 && ROType == ReadOut::Type::Caen){
    hspectraLGHG.Fill(l,h);
    hcorr.Fill(l,h);
  } else if (extend ==  2 && ROType == ReadOut::Type::Hgcroc){
    hspectraLGHG.Fill(l,h);
  }
  return true;
}

bool TileSpectra::FillWaveform(std::vector<int> samples){
 for (int k = 0; k < (int)samples.size(); k++ ){
   hcorr.Fill(k,samples.at(k));
 }
 return true;
}

bool TileSpectra::FillTrigger(double t){
  if (!bTriggPrim) bTriggPrim =true;
  hTriggPrim.Fill(t);
  return true;
}


bool TileSpectra::FillCorr(double l, double h){
  hspectraLGHG.Fill(l,h);
  hspectraHGLG.Fill(h,l);
  return true;
}


bool TileSpectra::FitNoise(double* out, int year = -1, bool isNoiseTrigg = false){        //[0] LG mean, [2] LG sigma, [4] HG mean, [6] HG sigma errors uneven numbers
  TFitResultPtr result;
  if (ROType == ReadOut::Type::Caen) {
    // estimate LG pedestal per channel
    BackgroundLG=TF1(Form("fped%sLGCellID%d",TileName.Data(),cellID),"gaus",0,400);
    BackgroundLG.SetNpx(400);
    if (year == 2023){
      BackgroundLG.SetParameter(1,50);
      BackgroundLG.SetParLimits(1,40,60);     // might need to make these values settable
      BackgroundLG.SetParameter(2,4);
      BackgroundLG.SetParLimits(2,0,10);     // might need to make these values settable      
      BackgroundLG.SetRange(0,70);
    } else {
      BackgroundLG.SetParameter(1,hspectraLG.GetMean());    
      BackgroundLG.SetParLimits(1,0,hspectraLG.GetMean()+100);     // might need to make these values settable
      BackgroundLG.SetParameter(2,10);
      BackgroundLG.SetParLimits(2,0,100);     // might need to make these values settable      
    }
    double maxLG = 0;
    if (isNoiseTrigg){
      maxLG = GetMaxXInRangeLG(0,300);
      BackgroundLG.SetParameter(1,maxLG);
      BackgroundLG.SetParLimits(1,maxLG-5,maxLG+5);
      if (debug > 1) std::cout << "reset LG: " << maxLG << std::endl;
    }
    
    if (!isNoiseTrigg){
      BackgroundLG.SetParLimits(0,0,hspectraLG.GetEntries());
      BackgroundLG.SetParameter(0,hspectraLG.GetEntries()/5);
      result=hspectraLG.Fit(&BackgroundLG,"QRMEN0S"); // initial fit
      double minLGFit = result->Parameter(1)-2*result->Parameter(2);
      double maxLGFit = result->Parameter(1)+1*result->Parameter(2);
      if (debug > 1) std::cout << "LG: " << minLGFit << "\t" << maxLGFit << "\t" << hspectraLG.GetEntries() << "\t" << hspectraLG.GetMean()<< std::endl;
      result=hspectraLG.Fit(&BackgroundLG,"QRMEN0S","", minLGFit, maxLGFit);  // limit to 2sigma
    } else {
      result=hspectraLG.Fit(&BackgroundLG,"QRMEN0S","", maxLG-20, maxLG+30);  // limit to 2sigma
      if (debug > 1) std::cout <<"LG: " << result->Parameter(0) << "\t"<< result->Parameter(1) << "\t" << result->Parameter(2) << std::endl;
    }
    bpedLG=true;
    calib->PedestalMeanL=result->Parameter(1);//Or maybe we do not want to do it automatically, only if =0?
    calib->PedestalSigL =result->Parameter(2);//Or maybe we do not want to do it automatically, only if =0?
    out[0]=result->Parameter(1);
    out[1]=result->Error(1);
    out[2]=result->Parameter(2);
    out[3]=result->Error(2);
    
    // estimate HG pedestal per channel
    BackgroundHG=TF1(Form("fped%sHGCellID%d",TileName.Data(),cellID),"gaus",0,400);
    BackgroundHG.SetNpx(400);
    if (year == 2023){
      BackgroundHG.SetParameter(1,60);
      BackgroundHG.SetParLimits(1,0,100);     // might need to make these values settable
      BackgroundHG.SetRange(0,200);
    } else {
      BackgroundHG.SetParameter(1,hspectraHG.GetMean());
      BackgroundHG.SetParLimits(1,0,hspectraHG.GetMean()+100);     // might need to make these values settable
    }
    BackgroundHG.SetParameter(2,10);  
    BackgroundHG.SetParLimits(2,0,100);     // might need to make these values settable    
    
    double maxHG = 0;
    if (isNoiseTrigg){
      maxHG = GetMaxXInRangeHG(0,300);
      BackgroundHG.SetParameter(2,20);  
      BackgroundHG.SetParameter(1,maxHG);
      BackgroundHG.SetParLimits(1,maxHG-5,maxHG+5);
      if (debug > 1) std::cout << "reset HG: " << maxHG << std::endl;
    }
    
    if (!isNoiseTrigg){
      BackgroundHG.SetParLimits(0,0,hspectraHG.GetEntries());
      BackgroundHG.SetParameter(0,hspectraHG.GetEntries()/10);
      result=hspectraHG.Fit(&BackgroundHG,"QRMEN0S");      // initial fit
      double minHGFit = result->Parameter(1)-2*result->Parameter(2);
      double maxHGFit = result->Parameter(1)+1*result->Parameter(2);
      if (debug > 1) std::cout <<"HG: " << minHGFit << "\t" << maxHGFit << "\t" << hspectraHG.GetEntries() << "\t" << hspectraHG.GetMean()<< std::endl;
      result=hspectraHG.Fit(&BackgroundHG,"QRMEN0S","",minHGFit, maxHGFit);  // limit to 2sigma range of previous fit
    } else {
      result=hspectraHG.Fit(&BackgroundHG,"QRMEN0S","",maxHG-40, maxHG+60);
      if (debug > 1) std::cout <<"HG: " << result->Parameter(0) << "\t"<< result->Parameter(1) << "\t" << result->Parameter(2) << std::endl;
    }
    bpedHG=true;
    
    calib->PedestalMeanH=result->Parameter(1);//Or maybe we do not want to do it automatically, only if =0?
    calib->PedestalSigH =result->Parameter(2);//Or maybe we do not want to do it automatically, only if =0?
    out[4]=result->Parameter(1);
    out[5]=result->Error(1);
    out[6]=result->Parameter(2);
    out[7]=result->Error(2);
    
    return true;
  } else if (ROType == ReadOut::Type::Hgcroc) {
    /*
    QRMEN0S
    Q: Quiet mode
    R: Use TF1::SetRange
    M: IMPROVE algorithm
    E: Minos error estimation
    N: Don't store graphics and don't draw
    0: Don't draw, but do store? 
    S: Return full result in TFitResultPtr
    QNSWW
    */
    // hspectraHG.Rebin(2);
    BackgroundHG = TF1(Form("fped%sHGCellID%d", TileName.Data(), cellID), "gaus", 0, 400);
    if (debug > 2) {
      std::cout << "Histogram has " << hspectraHG.GetEntries() << " entries" << std::endl;
      std::cout << "Mean is " << hspectraHG.GetMean() << std::endl;
      std::cout << "Standard deviation is " << hspectraHG.GetStdDev() << std::endl;
    }

    // First iteration
    // BackgroundHG.SetParLimits(0, 0, hspectraHG.GetEntries());
    // BackgroundHG.SetParameter(0, hspectraHG.GetEntries() / 5);
    // BackgroundHG.SetParameter(1, hspectraHG.GetMean());
    // BackgroundHG.SetParLimits(1, 0, hspectraHG.GetMean() + 100);
    // BackgroundHG.SetParameter(2, hspectraHG.GetStdDev());
    // BackgroundHG.SetParLimits(2, 0, 100);

    result = hspectraHG.Fit(&BackgroundHG, "QNSWW");
    if ((int)result != 0 || result->IsValid() != true) {
      if (debug > 1) std::cout << "FIT FAILED FOR CELL " << cellID << ", FIRST ITERATION" << std::endl;
      return false;
    }

    // Second iteration
    double minHGFit = result->Parameter(1) - 2 * result->Parameter(2);
    double maxHGFit = result->Parameter(1) + 1 * result->Parameter(2);
    if (debug > 1) std::cout << "HG: " << minHGFit << "\t" << maxHGFit << "\t" << hspectraHG.GetEntries() << "\t" << hspectraHG.GetMean()<< std::endl;
    result = hspectraHG.Fit(&BackgroundHG, "QNSWW", "", minHGFit, maxHGFit);  // limit to 2sigma
    if ((int)result != 0 || result->IsValid() != true){
    if (debug > 1) std::cout << "FIT FAILED FOR CELL " << cellID << ", SECOND ITERATION" << std::endl;
      return false;
    }

    bpedHG=true; // We're (I'm) being lazy and just calling the HGCROC ADC the high gain.  maybe we use LG for TOT info?
    calib->PedestalMeanH=result->Parameter(1);
    calib->PedestalSigH =result->Parameter(2);
    out[4]=result->Parameter(1);
    out[5]=result->Error(1);
    out[6]=result->Parameter(2);
    out[7]=result->Error(2);
    return true;
  }
  
  return false;
}

void TileSpectra::FitFixedNoise(){

  // estimate LG pedestal per channel
  BackgroundLG=TF1(Form("fpedLGCellID%d",cellID),"gaus",-10,10);
  BackgroundLG.SetNpx(400);
  BackgroundLG.SetParLimits(1,-2,2);     
  BackgroundLG.SetParameter(2,calib->PedestalSigL);     
  
  hspectraLG.Fit(&BackgroundLG,"QIRN0"); // initial fit
  bpedLG=BackgroundLG.IsValid();
  
  // estimate HG pedestal per channel
  BackgroundHG=TF1(Form("fpedHGCellID%d",cellID),"gaus",-20,20);
  BackgroundHG.SetNpx(400);
  // BackgroundHG.FixParameter(1,0);     
  BackgroundLG.SetParLimits(1,-2,2);     
  BackgroundHG.SetParameter(2,calib->PedestalSigH);     
  
  hspectraHG.Fit(&BackgroundHG,"QIRN0"); // initial fit
  bpedHG=BackgroundHG.IsValid();

  return;
}

void TileSpectra::InitializeNoiseFitsFromCalib(){

  // estimate LG pedestal per channel
  BackgroundLG=TF1(Form("fpedLGCellID%d",cellID),"gaus",-20,20);
  BackgroundLG.SetNpx(400);
  BackgroundLG.FixParameter(1,0);     
  BackgroundLG.FixParameter(2,calib->PedestalSigL);     
  hspectraLG.Fit(&BackgroundLG,"QIRN0"); // initial fit
  BackgroundLG.FixParameter(1,0);     
  BackgroundLG.FixParameter(2,calib->PedestalSigL);     
  bpedLG=true;
  
  // estimate HG pedestal per channel
  BackgroundHG=TF1(Form("fpedHGCellID%d",cellID),"gaus",-30,30);
  BackgroundHG.SetNpx(400);
  BackgroundHG.FixParameter(1,0);     
  BackgroundHG.FixParameter(2,calib->PedestalSigH);       
  hspectraHG.Fit(&BackgroundHG,"QIRN0"); // initial fit
  BackgroundHG.FixParameter(1,0);     
  BackgroundHG.FixParameter(2,calib->PedestalSigH);       
  bpedHG=true;

  return;
}


bool TileSpectra::FitMipHG(double* out, double* outErr, int verbosity, int year, bool impE = false, double vov = -1000, double avmip = -1000){
  
  // Once again, here are the Landau * Gaussian parameters:
  //   par[0]=Width (scale) parameter of Landau density
  //   par[1]=Most Probable (MP, location) parameter of Landau density
  //   par[2]=Total area (integral -inf to inf, normalization constant)
  //   par[3]=Width (sigma) of convoluted Gaussian function
  //

  if (verbosity > 2) std::cout << "FitHG cell ID: " << cellID << std::endl;
  
  TString funcName = Form("fmip%sHGCellID%d",TileName.Data(),cellID);
  bmipHG           = false;
  
  if (calib->BadChannel != -64 && calib->BadChannel < 1 ){
    if (verbosity > 0) std::cout << "==========> Skipped HG cell " << cellID << " channel dead" << std::endl;
    return false;
  }
  
  double fitrange[2]      = {50, 2000};
  if (impE){
    fitrange[0] = 0.6*avmip;
    fitrange[1] = 3*avmip;
  }
  if (year == 2023 && fitrange[0] < 100)
    fitrange[0] = 100;
  
  
  double intArea    = hspectraHG.Integral(hspectraHG.FindBin(fitrange[0]),hspectraHG.FindBin(fitrange[1]));
  double intNoise   = hspectraHG.Integral(hspectraHG.FindBin(-2*calib->PedestalSigH),hspectraHG.FindBin(+2*calib->PedestalSigH));
  double intAN3s    = hspectraHG.Integral(hspectraHG.FindBin(+3*calib->PedestalSigH),hspectraHG.FindBin(fitrange[1]));
  
  if (intArea/intNoise < 1e-5 && intAN3s > 200){
    if (verbosity > 0) std::cout << "==========> Skipped HG cell " << cellID << " S/B too small!" << std::endl;
    return false;
  }
  double startvalues[4]   = {50, 300, intArea, calib->PedestalSigH};
  double parlimitslo[4]   = {0.5, 50, 1.0, calib->PedestalSigH*0.01};
  double parlimitshi[4]   = {500, 1000, intArea*5, calib->PedestalSigH*40};
  if (year == 2023){
    startvalues[0]  = 200;
    startvalues[1]  = 500;    
    parlimitslo[1]  = 100;
    parlimitshi[0]  = 1000;
    parlimitshi[1]  = 1500;
  }
  
  if (impE && (avmip =! -1000)){
    startvalues[1]  = avmip;    
    parlimitslo[1]  = 0.5*avmip;    
    parlimitshi[1]  = 1.7*avmip;
  }
  if (vov != -1000){
    if (verbosity > 1) std::cout << "adjusting according to V_ov: " << vov<< std::endl;
    if (vov < 2.5){
      parlimitslo[1]  = 20;    
      fitrange[0]     = 15;
    } else if (vov > 5){
      fitrange[0]     = 150;      
    }
  }  
  SignalHG = TF1(funcName.Data(),langaufun,fitrange[0],fitrange[1],4);
  SignalHG.SetNpx(1000);
  SignalHG.SetParameters(startvalues);
  SignalHG.SetParNames("Width","MP","Area","GSigma");

  for (int i=0; i<4; i++) {
    SignalHG.SetParLimits(i, parlimitslo[i], parlimitshi[i]);
  }

  TString fitOption = "";
  if (impE){ 
    fitOption = "QRLMNE0";
    if (verbosity > 2) 
      fitOption = "RLMNE0";
  } else {
    fitOption = "QRLN0";
    if (verbosity > 2) 
      fitOption = "RLN0";
  }
  
  int fitStatus = hspectraHG.Fit(&SignalHG,fitOption);   // fit within specified range, use ParLimits, do not plot
  // Minuit status codes:
  // 4000 - Successful
  //    0 - Successful
  // 4070 - Problems
  //   70 - Problems
  // 
  if (!SignalHG.IsValid())
    return false;
  
  int limitStatus = 0;
  for (int i=0; i<4; i++) {
    if ( TMath::Abs(SignalHG.GetParameter(i) - parlimitslo[i]) < 1e-5 || TMath::Abs(SignalHG.GetParameter(i) - parlimitshi[i]) < 1e-5 ) {
      limitStatus++;
      if (verbosity > 0) std::cout << i << "\t" << SignalHG.GetParameter(i) << "\t : \t"<< parlimitslo[i] << "\t" << parlimitshi[i] << std::endl;
    }
  }
  if (verbosity > 1){
    std::cout << "Fit status HG " << cellID << " \t" << fitStatus << "\t limit reached: " << limitStatus  << std::endl;
  }
  
  if (!(fitStatus == 4000 || fitStatus == 0 || fitStatus == 4070 || fitStatus == 70 )){ // only accept fits which succeeded in general
    if (verbosity > 0) std::cout << "==========> Skipped HG cell " << cellID << " fit failed" << std::endl;
    return false;
  }
  if (limitStatus > 0){                        // don't accept fits which reached the set limits
    if (verbosity > 0) std::cout << "==========> Skipped HG cell " << cellID << " too many limits reached" << std::endl;
    return false;
  }
  bmipHG = true;
  
  if (bmipHG){
    SignalHG.GetParameters(out);    // obtain fit parameters
    for (int i=0; i<4; i++) {
      outErr[i] = SignalHG.GetParError(i);     // obtain fit parameter errors
    }
    outErr[4] = SignalHG.GetChisquare();  // obtain chi^2
    outErr[5] = SignalHG.GetNDF();           // obtain ndf
    
    double SNRPeak, SNRFWHM;
    langaupro(out,SNRPeak,SNRFWHM);

    calib->ScaleH       = SNRPeak;
    calib->ScaleWidthH  = SNRFWHM;
    out[4]    = SNRPeak;
    out[5]    = SNRFWHM;
  }
  return bmipHG;
}


bool TileSpectra::FitMipLG(double* out, double* outErr, int verbosity, int year, bool impE = false, double avmip = 1){
  
  // Once again, here are the Landau * Gaussian parameters:
  //   par[0]=Width (scale) parameter of Landau density
  //   par[1]=Most Probable (MP, location) parameter of Landau density
  //   par[2]=Total area (integral -inf to inf, normalization constant)
  //   par[3]=Width (sigma) of convoluted Gaussian function
  //

  TString funcName = Form("fmip%sLGCellID%d",TileName.Data(),cellID);
  
  double fitrange[2]      = {0, 500};
  if (impE){
    fitrange[0] = 0.5*avmip;
    fitrange[1] = 4*avmip;
  }

  if (calib->BadChannel != -64 && calib->BadChannel < 1 ){
    if (verbosity > 0) std::cout << "==========> Skipped LG cell " << cellID << " channel dead" << std::endl;
    return false;
  }

  double intArea    = hspectraLG.Integral(hspectraLG.FindBin(fitrange[0]),hspectraLG.FindBin(fitrange[1]));
  double intNoise   = hspectraLG.Integral(hspectraLG.FindBin(-2*calib->PedestalSigL),hspectraLG.FindBin(+2*calib->PedestalSigL));
  double intAN3s    = hspectraLG.Integral(hspectraLG.FindBin(+3*calib->PedestalSigL),hspectraLG.FindBin(fitrange[1]));
  
  if (intArea/intNoise < 1e-5 && intAN3s > 200){
    if (verbosity > 0) std::cout << "==========> Skipped LG cell " << cellID << " S/B too small!" << std::endl;
    return false;
  }
  double startvalues[4]   = {calib->PedestalSigL, 20, intArea, calib->PedestalSigL};
  double parlimitslo[4]   = {0.5, 0, 1.0, calib->PedestalSigL*0.1};
  double parlimitshi[4]   = {calib->PedestalSigL*10, 600, intArea*5, calib->PedestalSigL*10};
  
  SignalLG = TF1(funcName.Data(),langaufun,fitrange[0],fitrange[1],4);
  SignalLG.SetNpx(1000);
  SignalLG.SetParameters(startvalues);
  SignalLG.SetParNames("Width","MP","Area","GSigma");

  for (int i=0; i<4; i++) {
    SignalLG.SetParLimits(i, parlimitslo[i], parlimitshi[i]);
  }

  TString fitOption = "";
  if (impE){ 
    fitOption = "QRLMNE0";
    if (verbosity > 1) 
      fitOption = "RLMNE0";
  } else {
    fitOption = "QRLN0";
    if (verbosity > 1) 
      fitOption = "RLN0";
  }
  
  int fitStatus = hspectraLG.Fit(&SignalLG,fitOption);   // fit within specified range, use ParLimits, do not plot
  // Minuit status codes:
  // 4000 - Successful
  //    0 - Successful
  // 4070 - Problems
  //   70 - Problems
  // 
  if (!SignalLG.IsValid())
    return false;
  
  int limitStatus = 0;
  for (int i=0; i<4; i++) {
    if ( TMath::Abs(SignalLG.GetParameter(i) - parlimitslo[i]) < 1e-5 || TMath::Abs(SignalLG.GetParameter(i) - parlimitshi[i]) < 1e-5 ) {
      limitStatus++;
      if (verbosity > 0) std::cout << i << "\t" << SignalLG.GetParameter(i) << "\t : \t"<< parlimitslo[i] << "\t" << parlimitshi[i] << std::endl;
    }
  }
  if (verbosity > 1){
    std::cout << "Fit status LG " << cellID << " \t" << fitStatus << "\t limit reached: " << limitStatus  << std::endl;
  }
  
  if (!(fitStatus == 4000 || fitStatus == 0 || fitStatus == 4070 || fitStatus == 70)){ // only accept fits which succeeded or problems in general
    if (verbosity > 0) std::cout << "==========> Skipped LG cell " << cellID << " fit failed" << std::endl;
    return false;
  }
  if (limitStatus > 0){                        // don't accept fits which reached the set limits
    if (verbosity > 0) std::cout << "==========> Skipped LG cell " << cellID << " too many limits reached" << std::endl;
    return false;
  }
  bmipLG = true;
  
  if (bmipLG){
    SignalLG.GetParameters(out);    // obtain fit parameters
    for (int i=0; i<4; i++) {
      outErr[i] = SignalLG.GetParError(i);     // obtain fit parameter errors
    }
    outErr[4] = SignalLG.GetChisquare();  // obtain chi^2
    outErr[5] = SignalLG.GetNDF();           // obtain ndf
    
    double SNRPeak, SNRFWHM;
    langaupro(out,SNRPeak,SNRFWHM);

    calib->ScaleL       = SNRPeak;
    calib->ScaleWidthL  = SNRFWHM;
    out[4]    = SNRPeak;
    out[5]    = SNRFWHM;
  }
  return bmipLG;
}


bool TileSpectra::FitCorr(int verbosity){
  if (verbosity > 2) std::cout << "FitCorr cell ID: " << cellID << std::endl;
  TString funcName = Form("fcorr%sLGHGCellID%d",TileName.Data(),cellID);
  
  Double_t fitRangeLG[2]  = {20., 250.};
  Double_t fitRangeHG[2]  = {350., 3500.};
  int fitStatus   = 0; 
  int limitStatus = 0;
  
  
  LGHGcorr =  TF1(funcName.Data(),"pol1", fitRangeLG[0], fitRangeLG[1]);
  LGHGcorr.SetParameter(0,0.);
  LGHGcorr.SetParameter(1,10.);
  LGHGcorr.SetParLimits(1,0,100.);
  
  fitStatus = hspectraLGHG.Fit(&LGHGcorr,"QRMNE0"); 
  
  if (!(LGHGcorr.IsValid())){
    if (verbosity > 0) std::cout << "Skipped LGHG cell " << cellID << " fit failed" << std::endl;
    bcorrLGHG = false;
  } else {
    if ( LGHGcorr.GetParameter(1) == 0. || LGHGcorr.GetParameter(1) == 100. ) 
      limitStatus++;
    if (!(fitStatus == 4000 || fitStatus == 0)){ // only accept fits which succeeded in general
      if (verbosity > 0) std::cout << "Skipped LGHG cell " << cellID << " fit failed" << std::endl;
      bcorrLGHG = false;
    } else if (limitStatus > 0){                        // don't accept fits which reached the set limits
      if (verbosity > 0) std::cout << "Skipped LGHG cell " << cellID << " too many limits reached" << std::endl;
      bcorrLGHG = false;
    } else {
      bcorrLGHG= true;
    }  
  }
  if (bcorrLGHG){
    calib->LGHGCorr     = LGHGcorr.GetParameter(1);
    calib->LGHGCorrOff  = LGHGcorr.GetParameter(0);
  }
  funcName = Form("fcorr%sHGLGCellID%d",TileName.Data(),cellID);
  HGLGcorr =  TF1(funcName.Data(),"pol1",fitRangeHG[0],fitRangeHG[1]);
  HGLGcorr.SetParameter(0,0.);
  HGLGcorr.SetParameter(1,0.1);
  HGLGcorr.SetParLimits(1,0.,1.);

  fitStatus = hspectraHGLG.Fit(&HGLGcorr,"QRMNE0"); 
  limitStatus = 0;

  if (!(HGLGcorr.IsValid())){
    if (verbosity > 0) std::cout << "Skipped HGLG cell " << cellID << " fit failed" << std::endl;
    bcorrHGLG = false;
  } else {
    if ( HGLGcorr.GetParameter(1) == 0. || HGLGcorr.GetParameter(1) == 1. ) 
      limitStatus++;
    
    if (!(fitStatus == 4000 || fitStatus == 0)){ // only accept fits which succeeded in general
      if (verbosity > 0) std::cout << "Skipped HGLG cell " << cellID << " fit failed" << std::endl;
      bcorrHGLG = false;
    } else if (limitStatus > 0){                        // don't accept fits which reached the set limits
      if (verbosity > 0) std::cout << "Skipped HGLG cell " << cellID << " too many limits reached" << std::endl;
      bcorrHGLG = false;
    } else {
      bcorrHGLG= true;
    }
  }
  if (bcorrHGLG){
    calib->HGLGCorr     = HGLGcorr.GetParameter(1);
    calib->HGLGCorrOff  = HGLGcorr.GetParameter(0);
  }
  return true;
}

bool TileSpectra::FitLGHGCorr(int verbosity, bool resetCalib){
  if (verbosity > 2) std::cout << "FitCorr cell ID: " << cellID << std::endl;
  TString funcName = Form("fcorr%sLGHGCellID%d",TileName.Data(),cellID);
  
  Double_t fitRangeLG[2]  = {calib->PedestalSigL*5, 250.};
  int fitStatus   = 0; 
  int limitStatus = 0;
  
  
  LGHGcorr =  TF1(funcName.Data(),"pol1", fitRangeLG[0], fitRangeLG[1]);
  LGHGcorr.SetParameter(0,0.);
  LGHGcorr.SetParameter(1,10.);
  LGHGcorr.SetParLimits(1,0,1000.);
  
  fitStatus = hspectraLGHG.Fit(&LGHGcorr,"QRMNE0"); 
  
  if (!(LGHGcorr.IsValid())){
    if (verbosity > 0) std::cout << "Skipped LGHG cell " << cellID << " fit failed" << std::endl;
    bcorrLGHG = false;
  } else {
    if ( LGHGcorr.GetParameter(1) == 0. || LGHGcorr.GetParameter(1) == 1000. ) 
      limitStatus++;
    if (!(fitStatus == 4000 || fitStatus == 0)){ // only accept fits which succeeded in general
      if (verbosity > 0) std::cout << "Skipped LGHG cell " << cellID << " fit failed" << std::endl;
      bcorrLGHG = false;
    } else if (limitStatus > 0){                        // don't accept fits which reached the set limits
      if (verbosity > 0) std::cout << "Skipped LGHG cell " << cellID << " too many limits reached" << std::endl;
      bcorrLGHG = false;
    } else {
      bcorrLGHG= true;
    }  
  }
  if (bcorrLGHG && resetCalib){
    calib->LGHGCorr     = LGHGcorr.GetParameter(1);
    calib->LGHGCorrOff  = LGHGcorr.GetParameter(0);
  }
  
  return true;
}

bool TileSpectra::FitPedConstWage(int verbosity){
  if (verbosity > 2) std::cout << "FitCorr cell ID: " << cellID << std::endl;
  TString funcName = Form("fcorr%sPedWaveCellID%d",TileName.Data(),cellID);
  
  Double_t fitRange[2]  = {0, hcorr.GetXaxis()->GetBinCenter(hcorr.GetNbinsX())};
  int fitStatus   = 0; 
  int limitStatus = 0;
  
  // bcorrHGLG = true;
  HGLGcorr =  TF1(funcName.Data(),"pol0", fitRange[0], fitRange[1]);
  HGLGcorr.SetParameter(0,80.);
  HGLGcorr.SetParLimits(0,40.,120.);
  
  hcorr.RebinY(4);
  fitStatus = hcorr.Fit(&HGLGcorr,"QRMNE0"); 
  
  if (!(HGLGcorr.IsValid())){
    if (verbosity > 0) std::cout << "Skipped ped wave cell " << cellID << " fit failed" << std::endl;
    bcorrHGLG = false;
  } else {
    if ( HGLGcorr.GetParameter(0) == 40. || HGLGcorr.GetParameter(0) == 120. ) 
      limitStatus++;
    if (!(fitStatus == 4000 || fitStatus == 0)){ // only accept fits which succeeded in general
      if (verbosity > 0) std::cout << "Skipped ped wave cell " << cellID << " fit failed" << std::endl;
      bcorrHGLG = false;
    } else if (limitStatus > 0){                        // don't accept fits which reached the set limits
      if (verbosity > 0) std::cout << "Skipped ped wave cell " << cellID << " too many limits reached" << std::endl;
      bcorrHGLG = false;
    } else {
      bcorrHGLG= true;
    }
  }
  
  if (bcorrHGLG){
    // std::cout << HGLGcorr.GetParameter(0) << std::endl;
    calib->PedestalMeanL    = HGLGcorr.GetParameter(0);
    calib->PedestalSigL     = HGLGcorr.GetParError(0);
  }
  
  return bcorrHGLG;
}


bool TileSpectra::FitNoiseWithBG(double* out){
  return true;
}

TH1D* TileSpectra::GetHG(){
  return &hspectraHG;
}

TH1D* TileSpectra::GetLG(){
  return &hspectraLG;
}

TH1D* TileSpectra::GetTriggPrim(){
  return &hTriggPrim;
}

TH1D* TileSpectra::GetComb(){
  return &hcombined;
}

TProfile* TileSpectra::GetLGHGcorr(){
  return &hspectraLGHG;
}
TProfile* TileSpectra::GetHGLGcorr(){
  return &hspectraHGLG;
}

TH2D* TileSpectra::GetCorr(){
  return &hcorr;
}

TF1* TileSpectra::GetBackModel(int lh){
  if(lh==0 && bpedLG){
    return &BackgroundLG;
  }
  else if (lh==1 && bpedHG){
    return &BackgroundHG;
  }
  else return nullptr;
}

TF1* TileSpectra::GetSignalModel(int lh){
  if(lh==0 && bmipLG){
    return &SignalLG;
  }
  else if (lh==1 && bmipHG){
    return &SignalHG;
  }
  else return nullptr;
}

TileCalib* TileSpectra::GetCalib(){
  return calib;
}

TF1* TileSpectra::GetCorrModel( int lh ){
  if(lh==0 && bcorrLGHG){
    return &LGHGcorr;
  }
  else if (lh==1 && bcorrHGLG){
    return &HGLGcorr;
  }
  else return nullptr;
}

void TileSpectra::Write( bool wFits = true){
  hspectraHG.Write(hspectraHG.GetName(), kOverwrite);
  hspectraLG.Write(hspectraLG.GetName(), kOverwrite);
  hspectraLGHG.Write(hspectraLGHG.GetName(), kOverwrite);
  hspectraHGLG.Write(hspectraHGLG.GetName(), kOverwrite);
  if (bTriggPrim) hTriggPrim.Write(hTriggPrim.GetName(), kOverwrite);
  if ( wFits ){
    if(bpedHG)BackgroundHG.Write(BackgroundHG.GetName(), kOverwrite);
    if(bmipHG)SignalHG.Write(SignalHG.GetName(), kOverwrite);
    if(bpedLG)BackgroundLG.Write(BackgroundLG.GetName(), kOverwrite);
    if(bmipLG)SignalLG.Write(SignalLG.GetName(), kOverwrite);
    if(bcorrLGHG)LGHGcorr.Write(LGHGcorr.GetName(), kOverwrite);
    if(bcorrHGLG)HGLGcorr.Write(HGLGcorr.GetName(), kOverwrite);
  }
}

void TileSpectra::WriteExt( bool wFits = true){
  hspectraHG.Write(hspectraHG.GetName(), kOverwrite);
  hspectraLG.Write(hspectraLG.GetName(), kOverwrite);
  hspectraLGHG.Write(hspectraLGHG.GetName(), kOverwrite);  
  if (extend == 1){
    hcombined.Write(hcombined.GetName(), kOverwrite);
    hspectraHGLG.Write(hspectraHGLG.GetName(), kOverwrite);  
  } else if (extend == 2){
    hcorr.Write(hcorr.GetName(), kOverwrite);  
  }
  if ( wFits ){
    if(bpedHG)BackgroundHG.Write(BackgroundHG.GetName(), kOverwrite);
    if(bmipHG)SignalHG.Write(SignalHG.GetName(), kOverwrite);
    if(bpedLG)BackgroundLG.Write(BackgroundLG.GetName(), kOverwrite);
    if(bmipLG)SignalLG.Write(SignalLG.GetName(), kOverwrite);
    if(bcorrLGHG)LGHGcorr.Write(LGHGcorr.GetName(), kOverwrite);
  }
}

double TileSpectra::langaufun(double *x, double *par) {

  //Fit parameters:
  //par[0]=Width (scale) parameter of Landau density
  //par[1]=Most Probable (MP, location) parameter of Landau density
  //par[2]=Total area (integral -inf to inf, normalization constant)
  //par[3]=Width (sigma) of convoluted Gaussian function
  //
  //In the Landau distribution (represented by the CERNLIB approximation),
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.

  // Numeric constants
  static double invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  static double mpshift  = -0.22278298;       // Landau maximum location

  // Control constants
  static double np = 100.0;      // number of convolution steps
  static double sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

  // Variables
  double xx;
  double mpc;
  double fland;
  double sum = 0.0;
  double xlow,xupp;
  double step;
  double i;


  // MP shift correction
  mpc = par[1] - mpshift * par[0];

  // Range of convolution integral
  xlow = x[0] - sc * par[3];
  xupp = x[0] + sc * par[3];

  step = (xupp-xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for(i=1.0; i<=np/2; i++) {
    xx = xlow + (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0],xx,par[3]);

    xx = xupp - (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0],xx,par[3]);
  }

  return (par[2] * step * sum * invsq2pi / par[3]);
}


int TileSpectra::langaupro(double *params, double &maxx, double &FWHM) {

  // Searches for the location (x value) at the maximum of the
  // Landau-Gaussian convolute and its full width at half-maximum.
  //
  // The search is probably not very efficient, but it's a first try.
  double p,x,fy,fxr,fxl;
  double step;
  double l,lold;
  int i = 0;
  int MAXCALLS = 10000;

  // Search for maximum
  p = params[1] - 0.1 * params[0];
  step = 0.05 * params[0];
  lold = -2.0;
  l    = -1.0;

  while ( (l != lold) && (i < MAXCALLS) ) {
    i++;
    lold = l;
    x = p + step;
    l = langaufun(&x,params);
    if (l < lold)
      step = -step/10;
    p += step;
  }

  if (i == MAXCALLS)
    return (-1);
  maxx = x;
  fy = l/2;

  // Search for right x location of fy
  p = maxx + params[0];
  step = params[0];
  lold = -2.0;
  l    = -1e300;
  i    = 0;
    
  while ( (l != lold) && (i < MAXCALLS) ) {
    i++;
    lold = l;
    x = p + step;
    l = TMath::Abs(langaufun(&x,params) - fy);
    if (l > lold)
      step = -step/10;
    p += step;
  }

  if (i == MAXCALLS)
    return (-2);
  fxr = x;

  // Search for left x location of fy
  p = maxx - 0.5 * params[0];
  step = -params[0];
  lold = -2.0;
  l    = -1e300;
  i    = 0;

  while ( (l != lold) && (i < MAXCALLS) ) {
    i++;
    lold = l;
    x = p + step;
    l = TMath::Abs(langaufun(&x,params) - fy);
    if (l > lold)
      step = -step/10;
    p += step;
  }

  if (i == MAXCALLS)
    return (-3);


  fxl = x;
  FWHM = fxr - fxl;
  return (0);
}

//__________________________________________________________________________________________________________
// find bin with largest content in given range and return X
//__________________________________________________________________________________________________________
double TileSpectra::GetMaxXInRangeHG(double minX = -10000, double maxX = -10000) {
  Double_t largestContent     = 0;
  Int_t minBin = 1;
  Int_t maxBin = hspectraHG.GetNbinsX()+1;
  if (minX != -10000) minBin = hspectraHG.GetXaxis()->FindBin(minX);
  if (maxX != -10000) maxBin = hspectraHG.GetXaxis()->FindBin(maxX)+0.0001;
  Int_t largestBin = minBin;
  for (Int_t i= minBin; i < maxBin; i++){
    if (largestContent < hspectraHG.GetBinContent(i)){
      largestContent = hspectraHG.GetBinContent(i);
      largestBin = i;
    }
  }
  return hspectraHG.GetBinCenter(largestBin);
}



//__________________________________________________________________________________________________________
// find bin with largest content in given range and return X
//__________________________________________________________________________________________________________
double TileSpectra::GetMaxXInRangeLG(double minX = -10000, double maxX = -10000) {
  Double_t largestContent     = 0;
  Int_t minBin = 1;
  Int_t maxBin = hspectraLG.GetNbinsX()+1;
  if (minX != -10000) minBin = hspectraLG.GetXaxis()->FindBin(minX);
  if (maxX != -10000) maxBin = hspectraLG.GetXaxis()->FindBin(maxX)+0.0001;
  Int_t largestBin = minBin;
  for (Int_t i= minBin; i < maxBin; i++){
    if (largestContent < hspectraLG.GetBinContent(i)){
      largestContent = hspectraLG.GetBinContent(i);
      largestBin = i;
    }
  }
  return hspectraLG.GetBinCenter(largestBin);
}

//__________________________________________________________________________________________________________
// find bad channels
//__________________________________________________________________________________________________________
short TileSpectra::DetermineBadChannel(){
  short bc = -64;
 
  FitFixedNoise();
  
  // double sigL   = calib->PedestalSigL;
  // double sigH   = calib->PedestalSigH;

  double sigLN   = BackgroundLG.GetParameter(2);
  double sigHN   = BackgroundHG.GetParameter(2);
  // std::cout << "HG: " << sigH << "\t" << sigHN <<  "\t LG: " << sigL << "\t" << sigLN  << std::endl;
  
//   double bgH3    = hspectraHG.Integral(hspectraHG.FindBin(-sigH*3-0.01),hspectraHG.FindBin(sigH*3+0.01));
//   double bgL3    = hspectraLG.Integral(hspectraLG.FindBin(-sigL*3-0.01),hspectraLG.FindBin(sigL*3+0.01));
//   double intH3   = hspectraHG.Integral(hspectraHG.FindBin(sigH*3+0.01),hspectraHG.GetNbinsX()-1);
//   double intL3   = hspectraLG.Integral(hspectraLG.FindBin(sigL*3+0.01),hspectraLG.GetNbinsX()-1);
//   double intH5   = hspectraHG.Integral(hspectraHG.FindBin(sigH*5+0.01),hspectraHG.GetNbinsX()-1);
//   double intL5   = hspectraLG.Integral(hspectraLG.FindBin(sigL*5+0.01),hspectraLG.GetNbinsX()-1);
//   
//   if (intH5 > 100 && intL5 > 100){
//     bc = 3;
//   } else if (intH5 > 100 && intL3 > 100){
//     bc = 2;
//     std::cout << bc << "  \t HG: " << intH5  << "\t" << intH5/bgH3 << "\t" << " LG: "<< intL3  << "\t" << intL3/bgL3 << std::endl;
//   } else if (intH3 > 100 && intL3 > 100){
//     bc = 1;
//     std::cout << bc << "  \t HG: " << intH3  << "\t" << intH3/bgH3 << "\t" << " LG: "<< intL3  << "\t"  << intL3/bgL3 << std::endl;
//   } else {
//     bc = 0;
//     std::cout << bc << "  \t HG: " << intH3  << "\t" << intH3/bgH3 << "\t" << " LG: "<< intL3  << "\t"  << intL3/bgL3 << std::endl;
//   }
  BackgroundHG.SetRange(-100,400);
  // double nH3  = BackgroundHG.Integral(-3*sigHN,5*sigHN);
  // double nH5  = BackgroundHG.Integral(-5*sigHN,7*sigHN);
  double nH3  = hspectraHG.Integral(hspectraHG.FindBin(-3*sigHN),hspectraHG.FindBin(3*sigHN));
  double nH5  = hspectraHG.Integral(hspectraHG.FindBin(-5*sigHN),hspectraHG.FindBin(5*sigHN));
  double fH   = hspectraHG.Integral(1,hspectraHG.GetNbinsX()-1);
  BackgroundLG.SetRange(-100,400);
  double nL3  = BackgroundLG.Integral(-3*sigLN,3*sigLN);
  double nL5  = BackgroundLG.Integral(-5*sigLN,5*sigLN);
  double fL   = hspectraLG.Integral(1,hspectraLG.GetNbinsX()-1);

  double rH3  = (fH > 0) ? (fH-nH3)/fH : 0;
  double rH5  = (fH > 0) ? (fH-nH5)/fH : 0;
  double rL3  = (fL > 0) ? (fL-nL3)/fL : 0;
  double rL5  = (fL > 0) ? (fL-nL5)/fL : 0;

  if (rH5 > 1e-6 && rL5 > 1e-6){
    bc = 3;
  } else if (rH5 > 1e-6 && rL3 > 0.005){
    bc = 2;
  } else if (rH3 > 0.005 && rL3 > 0.005){
    bc = 1;
  } else {
    bc = 0;
  }
  calib->BadChannel = bc;
  
  return bc;
}

void TileSpectra::SetBadChannelInCalib(short s ){
  calib->BadChannel = s;
}
