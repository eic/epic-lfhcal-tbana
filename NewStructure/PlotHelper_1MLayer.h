#ifndef PLOTHELPER_1MLAYER_H
#define PLOTHELPER_1MLAYER_H

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // dedicated class for all 1M layer plotting functions
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //__________________________________________________________________________________________________________
  // Plot Noise with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotNoiseWithFits1MLayer (TCanvas* canvas2Panel, Double_t topRCornerX,  Double_t topRCornerY, Double_t relSizeP, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int option, 
                                  Double_t xPMin, Double_t xPMax, Double_t scaleYMax, int layer, int mod,  TString nameOutput, RunInfo currRunInfo, int extCellId = -1){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    
    Setup* setupT = Setup::GetInstance();
    
    int skipped = 0;
    int tempCellID = -1;
    if (layer != -1 && mod != -1 )
      tempCellID = setupT->GetCellID(0,0, layer, mod);
    if (extCellId != -1)
      tempCellID = extCellId;
    if (tempCellID < 0)
      return;
   
    ithSpectra=spectra.find(tempCellID);
    if(ithSpectra==spectra.end()){
      std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t layer " << layer << "\t module " << mod << std::endl;
      skipped++;
      return;
    } 
    TH1D* tempHist = nullptr;
    if (option == 0){
        tempHist = ithSpectra->second.GetHG();
    } else if (option ==1){
        tempHist = ithSpectra->second.GetLG();
    } else if (option ==2){
        tempHist = ithSpectra->second.GetTOA();
    } else if (option ==3){
        tempHist = ithSpectra->second.GetTOT();
    }
    if (maxY < FindLargestBin1DHist(tempHist, xPMin , xPMax)) maxY = FindLargestBin1DHist(tempHist, xPMin , xPMax);

    canvas2Panel->cd();
    canvas2Panel->SetLogy();
    SetStyleHistoTH1ForGraphs( tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.1, 510, 510, 43, 63);  
    SetMarkerDefaults(tempHist, 20, 1, kBlue+1, kBlue+1, kFALSE);   
    tempHist->GetXaxis()->SetRangeUser(xPMin,xPMax);
    tempHist->GetYaxis()->SetRangeUser(0.7,scaleYMax*maxY);
      
    tempHist->Draw("pe");
    short bctemp = ithSpectra->second.GetCalib()->BadChannel;
    if (bctemp != -64 && bctemp < 3){
      Color_t boxCol = kGray;
      if (bctemp == 1)
        boxCol = kGray+1;
      else if (bctemp == 0)
        boxCol = kGray+2;
      TBox* badChannelArea =  CreateBox(boxCol, xPMin, 0.7, xPMax,scaleYMax*maxY, 1001 );
      badChannelArea->SetFillColorAlpha(boxCol, 0.35);
      badChannelArea->Draw();
      tempHist->Draw("same,axis");
      tempHist->Draw("same,pe");
    }
      
    TString label           = Form("layer %d", layer);
    if (extCellId != -1)
      label = Form("a:%d, ch:%d",setupT->GetROunit(extCellId), setupT->GetROchannel(extCellId) );

    TLatex *labelChannel    = new TLatex(topRCornerX-0.04,topRCornerY-1.2*relSizeP,label);
    SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);

      
    TF1* fit = nullptr;
    if (option == 0){
      fit = ithSpectra->second.GetBackModel(1);
    } else  if (option ==1){
      fit = ithSpectra->second.GetBackModel(0);  
    }
    if (fit){
      SetStyleFit(fit , xPMin, xPMax, 7, 7, kBlack);
      fit->Draw("same");
      TLegend* legend = GetAndSetLegend2( topRCornerX-8*relSizeP, topRCornerY-4*0.85*relSizeP-0.4*relSizeP, topRCornerX-0.04, topRCornerY-0.6*relSizeP,0.85*textSizePixel, 1, label, 43,0.2);
      legend->AddEntry(fit, "Gauss noise fit", "l");
      legend->AddEntry((TObject*)0, Form("#mu = %2.2f #pm %2.2f",fit->GetParameter(1), fit->GetParError(1) ) , " ");
      legend->AddEntry((TObject*)0, Form("#sigma = %2.2f #pm %2.2f",fit->GetParameter(2), fit->GetParError(2) ) , " ");
      legend->Draw();
        
    } else {
      labelChannel->Draw();  
    }
  
    if (xPMin < -5) DrawLines(0, 0,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
    DrawLatex(topRCornerX-0.04, topRCornerY-4*0.85*relSizeP-1.4*relSizeP, GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeP, 42);
    DrawLatex(topRCornerX-0.04, topRCornerY-4*0.85*relSizeP-2.2*relSizeP, GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeP, 42);
    canvas2Panel->SaveAs(nameOutput.Data());
  }
  //__________________________________________________________________________________________________________
  // Plot Spectra with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotSpectra1MLayer (TCanvas* canvas,
                           Double_t topRCornerX,  Double_t topRCornerY, Double_t relSizeP, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int option, 
                                  Double_t xPMin, Double_t xPMax, Double_t scaleYMax, int layer, int mod,  TString nameOutput, RunInfo currRunInfo, Int_t extCellId = -1){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    Setup* setupT = Setup::GetInstance();
    
    int tempCellID = -1;
    if (layer != -1 && mod != -1 )
      tempCellID = setupT->GetCellID(0,0, layer, mod);
    if (extCellId != -1)
      tempCellID = extCellId;
    if (tempCellID < 0)
      return;

    
    ithSpectra=spectra.find(tempCellID);
    if(ithSpectra==spectra.end()){
      std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t layer " << layer << "\t module " << mod << std::endl;
      return;
    } 
    TH1D* tempHist = nullptr;
    double noiseWidth = 0;
    if (option == 0){
      tempHist = ithSpectra->second.GetHG();
      noiseWidth = ithSpectra->second.GetCalib()->PedestalSigH;
    } else if (option == 1){
      tempHist = ithSpectra->second.GetLG();
      noiseWidth = ithSpectra->second.GetCalib()->PedestalSigL;
    } else if (option == 2){
      tempHist = ithSpectra->second.GetComb();
    } else if (option == 3){
      tempHist = ithSpectra->second.GetTOA();
    } else if (option == 4){
      tempHist = ithSpectra->second.GetTOT();
    }
    if (maxY < FindLargestBin1DHist(tempHist, xPMin , xPMax)) maxY = FindLargestBin1DHist(tempHist, xPMin , xPMax);
    
    canvas->cd();
    canvas->SetLogy();
    SetStyleHistoTH1ForGraphs( tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.1, 510, 510, 43, 63);  
    SetMarkerDefaults(tempHist, 20, 1, kBlue+1, kBlue+1, kFALSE);   
    tempHist->GetXaxis()->SetRangeUser(xPMin,xPMax);
    tempHist->GetYaxis()->SetRangeUser(0.7,scaleYMax*maxY);
    
    tempHist->Draw("pe");
    short bctemp = ithSpectra->second.GetCalib()->BadChannel;
    if (bctemp != -64 && bctemp < 3){
      Color_t boxCol = kGray;
      if (bctemp == 1)
        boxCol = kGray+1;
      else if (bctemp == 0)
        boxCol = kGray+2;
      TBox* badChannelArea =  CreateBox(boxCol, xPMin, 0.7, xPMax,scaleYMax*maxY, 1001 );
      badChannelArea->Draw();
      tempHist->Draw("same,axis");
      tempHist->Draw("same,pe");
    }
            
    TString label = Form("layer %d", layer);
    if (extCellId != -1)
      label = Form("a:%d, ch:%d",setupT->GetROunit(extCellId), setupT->GetROchannel(extCellId) );

    TLatex *labelChannel    = new TLatex(topRCornerX-0.045,topRCornerY-1.2*relSizeP,label);
    SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);
    labelChannel->Draw();  
  
    if (option < 2){
      DrawLines(noiseWidth*3, noiseWidth*3,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
      DrawLines(noiseWidth*5, noiseWidth*5,0.7, scaleYMax*maxY, 2, kGray+1, 6);  
    } else if (option == 3){
      DrawLines(0.3, 0.3, 0.7, scaleYMax*maxY, 2, kGray+1, 10);  
    }
    DrawLatex(topRCornerX-0.045, topRCornerY-4*0.85*relSizeP-1.4*relSizeP, GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeP, 42);
    DrawLatex(topRCornerX-0.045, topRCornerY-4*0.85*relSizeP-2.2*relSizeP, GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeP, 42);
    
    canvas->SaveAs(nameOutput.Data());
  }
  //__________________________________________________________________________________________________________
  // Plot Corr with Fits for Full layer 2D
  //__________________________________________________________________________________________________________
  inline void PlotCorr2D1MLayer (TCanvas* canvas2Panel, Double_t topRCornerX,  Double_t topRCornerY, Double_t relSizeP, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int option,
                                  Double_t xPMin, Double_t xPMax, Double_t maxY, int layer, int mod,  TString nameOutput, RunInfo currRunInfo, Int_t extCellId = -1 ){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileSpectra>::iterator ithSpectra;    
    int skipped = 0;
    ReadOut::Type rotype = ReadOut::Type::Undef;
    
    
    canvas2Panel->cd();
    int tempCellID = -1;
    if (layer != -1 && mod != -1 )
      tempCellID = setupT->GetCellID(0,0, layer, mod);
    if (extCellId != -1)
      tempCellID = extCellId;
    if (tempCellID < 0)
      return;

    
    canvas2Panel->SetLogy(0);
    canvas2Panel->SetLogz(1);
      
    Int_t offset = 1;
    ithSpectra=spectra.find(tempCellID);
    if(ithSpectra==spectra.end()){
      skipped++;
      std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t layer " << layer << "\t module " << mod << std::endl;
      return;
    } else {
      rotype = ithSpectra->second.GetROType();
    }

    TProfile* tempProfile = nullptr;
    TH2D* temp2D          = nullptr;        
    // LG-HG correlation CAEN
    if (option == 0){
      tempProfile     = ithSpectra->second.GetLGHGcorr();
      temp2D          = ithSpectra->second.GetCorr();
    // HGCROC waveform
    } else if (option == 1){
      tempProfile     = ithSpectra->second.GetWave1D();
      temp2D          = ithSpectra->second.GetCorr();          
    // HGCROC TOA-ADC correlation
    } else if (option == 2){
      tempProfile     = ithSpectra->second.GetTOAADC();
      temp2D          = ithSpectra->second.GetCorrTOAADC();                    
    } else if (option == 3){
      temp2D          = ithSpectra->second.GetCorrTOASample();
    }
    
    SetStyleHistoTH2ForGraphs( temp2D, temp2D->GetXaxis()->GetTitle(), temp2D->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
    
    std::cout << "min: " << xPMin << "\t" << xPMax << std::endl; 
    temp2D->GetYaxis()->SetRangeUser(-10,maxY);
    temp2D->GetXaxis()->SetRangeUser(xPMin,xPMax);
    temp2D->Draw("colz");

    DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xPMin, 0, xPMax, maxY);
    temp2D->Draw("axis,same");
    
    if (tempProfile){
      SetMarkerDefaultsProfile(tempProfile, 24, 0.3, kRed+2, kRed+2);   
      tempProfile->Draw("pe, same");
    } 
        
    TString labelCh           = Form("layer %d", layer);
    if (extCellId != -1 && (currRunInfo.detector).CompareTo("FoCal") == 0)
      labelCh = Form("a:%d, ch:%d",setupT->GetROunit(extCellId), setupT->GetROchannel(extCellId) );
    else 
      labelCh = Form("m:%d, l:%d, c:%d, r:%d",setupT->GetModule(extCellId), setupT->GetLayer(extCellId), setupT->GetColumn(extCellId), setupT->GetRow(extCellId));
    
    TF1* fit            = ithSpectra->second.GetCorrModel(0);
    if (rotype == ReadOut::Type::Hgcroc)
      fit            = ithSpectra->second.GetCorrModel(2);
    if (fit){
      offset=4;
      Double_t rangeFit[2] = {0,0};
      fit->GetRange(rangeFit[0], rangeFit[1]);
      SetStyleFit(fit , rangeFit[0], rangeFit[1], 7, 3, kRed+3);
      fit->Draw("same");
      TLegend* legend = nullptr;
      if (rotype == ReadOut::Type::Caen){
        legend = GetAndSetLegend2( topRCornerX, topRCornerY-4*0.85*relSizeP-0.4*relSizeP, topRCornerX+6*relSizeP, topRCornerY-0.6*relSizeP,0.85*textSizePixel, 1, labelCh, 43,0.1);
        legend->AddEntry(fit, "linear fit, trigg.", "l");
        legend->AddEntry((TObject*)0, Form("#scale[0.8]{b = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");
        legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(1), fit->GetParError(1) ) , " ");
      } else {
        legend = GetAndSetLegend2( topRCornerX, topRCornerY-3*0.85*relSizeP-0.4*relSizeP, topRCornerX+6*relSizeP, topRCornerY-0.6*relSizeP,0.85*textSizePixel, 1, labelCh, 43,0.1);
        legend->AddEntry(fit, "const fit", "l");
        legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");   
      }
      legend->Draw();
    } else {
      DrawLatex(0.86, topRCornerY-offset*0.85*relSizeP-0.1*relSizeP, labelCh, true, 0.85*relSizeP, 42);
    }
  
    if (option == 1){
      for (int i = 0; i < currRunInfo.samples; i++){
          DrawLines(i*25e3, i*25e3, 0,maxY*0.2, 1, kGray+2, 7, 1);
      } 
    }
  
    
    TString beamline = GetStringFromRunInfo(currRunInfo, 9).Data();
    TString lab1 = Form("#it{#bf{%s TB:}} %s", (currRunInfo.detector).Data(), beamline.Data());
    // labeling inside the panels & legend drawing 
    if (beamline.Contains("ORNL")) lab1 = beamline;
    
    DrawLatex(topRCornerX, topRCornerY-offset*0.85*relSizeP-0.1*relSizeP, lab1, false, 0.85*relSizeP, 42);
    DrawLatex(topRCornerX, topRCornerY-offset*0.85*relSizeP-0.9*relSizeP, GetStringFromRunInfo(currRunInfo, 2), false, 0.85*relSizeP, 42);
    DrawLatex(topRCornerX, topRCornerY-offset*0.85*relSizeP-1.7*relSizeP, GetStringFromRunInfo(currRunInfo, 3), false, 0.85*relSizeP, 42);
    canvas2Panel->SaveAs(nameOutput.Data());
  }

  
  //__________________________________________________________________________________________________________
  // Plot Run overlay for all 1 tiles for all runs available
  //__________________________________________________________________________________________________________
  inline void PlotRunOverlay1MLayer (TCanvas* canvas2Panel, Double_t topRCornerX, Double_t topLCornerX,  Double_t topRCornerY, Double_t relSizeP, Int_t textSizePixel, 
                              std::map<int,TileTrend> trending, int nruns, int optionTrend, 
                              Double_t xPMin, Double_t xPMax, int layer, int mod,  TString nameOutput, TString nameOutputSummary, RunInfo commonRunInfo, Int_t detailedPlot = 1, bool plotMean = false, int labelOpt = 0, int extCellId = -1){
    
    // loading instance of setip
    Setup* setupT = Setup::GetInstance();
    // checking whether we are plotting a specific cell
    int tempCellID = -1;
    if (layer != -1 && mod != -1 )
      tempCellID = setupT->GetCellID(0,0, layer, mod);
    if (extCellId != -1)
      tempCellID = extCellId;
    if (tempCellID < 0)
      return;
    
    Double_t maxY         = 0.;
    Double_t minY         = 9999.;
    
    // testing whether cell is contained in tile trend map
    std::map<int, TileTrend>::iterator ithTrend;    
    ithTrend=trending.find(tempCellID);
    if(ithTrend==trending.end()){
      std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t layer " << layer << "\t module " << mod << std::endl;
      return;
    }
    // setting min and max for spectra plot
    if (optionTrend == 0){      // HG
      if(maxY<ithTrend->second.GetMaxHGSpec()) maxY=ithTrend->second.GetMaxHGSpec();
      if(minY>ithTrend->second.GetMinHGSpec()) minY=ithTrend->second.GetMinHGSpec();
    } else if (optionTrend == 1){   //LG
      if(maxY<ithTrend->second.GetMaxLGSpec()) maxY=ithTrend->second.GetMaxLGSpec();
      if(minY>ithTrend->second.GetMinLGSpec()) minY=ithTrend->second.GetMinLGSpec();
    }

        // evaluate whether its the same quantities
    bool isSameRun      = false;
    int altStyle        = 0;
    if (commonRunInfo.runNr != -10000){
      isSameRun         = true;
      altStyle          = 1;
    }
    if (commonRunInfo.pdg != -10000)
      altStyle          = 1;
    bool isSameVoltage  = false;
    double commonVoltage  = 0;
    if (commonRunInfo.vop != -10000.){
      isSameVoltage     = true;
      commonVoltage     = commonRunInfo.vop;
    }
        
    if (maxY == 0 && minY == 9999.){
      std::cout <<"Something went wrong! No ranges set for layer " <<  layer << " \t trend plotting option: " << optionTrend << "\t ABORTING!" << std::endl;
      return;
    }
    maxY = 3*maxY;
  
    // defining global settings
    double labelScaleL = 0.75;
    double labelScaleT = 0.75;
    double sizeLabelsPix = labelScaleT*textSizePixel;
    double width        = 0.6;
    Int_t columns       = 4;
    Int_t nSameSettings = GetNSameSettings(commonRunInfo, labelOpt);
    TString header      = GetHeaderLegendCommonRunObject(commonRunInfo, nSameSettings, width, columns, labelScaleL);
    if (plotMean){
      columns         = 2;
      labelScaleL   = 0.65; 
    }
    double lineBottom   = (int)(nruns/columns)+1;
    if (header.CompareTo("") != 0 ) lineBottom++;
    
    double lineDist     = labelScaleT*relSizeP;
    double lineDistL    = labelScaleL*relSizeP;

    // calculating the legend position
    Double_t yPosStart  = topRCornerY -lineDist;
    double startLegY    = topRCornerY -(lineBottom+0.25)*lineDistL;
    double endLegY      = topRCornerY -0.25*lineDistL;
    // std::cout << lineBottom << "\t" << yPosStart << "\t"<< startLegY << "\t"<< endLegY << std::endl;

    // entering canvas
    canvas2Panel->cd();
    canvas2Panel->SetLogy(1);

    TLegend* legend = GetAndSetLegend2(  width, startLegY, topRCornerX, endLegY,
                                        labelScaleL*textSizePixel, columns, header,43,0.25);
    
    TH1D* histos[30];
    for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
      int tmpRunNr = ithTrend->second.GetRunNr(rc);
      histos[rc] = nullptr;
      if (tmpRunNr != -1 && !isSameRun) {
        if (optionTrend == 0){      // HG
          histos[rc] = ithTrend->second.GetHGTriggRun(ithTrend->second.GetRunNr(rc));
        } else if (optionTrend == 1){      // LG
          histos[rc] = ithTrend->second.GetLGTriggRun(ithTrend->second.GetRunNr(rc));
        }
      } else if (isSameRun && tmpRunNr != -1){
        if (optionTrend == 0){      // HG
          histos[rc] = ithTrend->second.GetHGTriggRun(rc);
        } else if (optionTrend == 1){      // LG
          histos[rc] = ithTrend->second.GetLGTriggRun(rc);
        }              
      }
      
      TString drawOpt = "f";
      if (histos[rc]){
        SetStyleHistoTH1ForGraphs( histos[rc], histos[rc]->GetXaxis()->GetTitle(), histos[rc]->GetYaxis()->GetTitle(), sizeLabelsPix, 0.9*textSizePixel, sizeLabelsPix, 0.9*textSizePixel,0.9, 1.3,  510, 510, 43, 63);  
        if (nameOutput.Contains("TOT") || nameOutput.Contains("Tot") ||  nameOutput.Contains("ToT"))
          SetHistDefaultsWFill(histos[rc], GetColorLayer(rc, altStyle), 3, GetLineStyleLayer(rc, altStyle), GetFillStyleLayer(rc));   
        else {
          SetLineDefaults(histos[rc], GetColorLayer(rc, altStyle), 3, GetLineStyleLayer(rc, altStyle));   
          drawOpt ="l";
        }
        if(rc == 0){
          histos[rc]->GetXaxis()->SetRangeUser(xPMin,xPMax);
          histos[rc]->GetYaxis()->SetRangeUser(minY,maxY);
          histos[rc]->Draw("hist");
        } else {
          histos[rc]->Draw("same,hist");
        }
        TString labelLegend = ithTrend->second.GetLabel(rc);
        if (plotMean){
          DrawLines(histos[rc]->GetMean(), histos[rc]->GetMean(),minY, 0.2*maxY, 4,GetColorLayer(rc, altStyle) ,GetLineStyleLayer(rc, altStyle));
          legend->AddEntry(histos[rc],Form("%s (%.1f)",labelLegend.Data(),histos[rc]->GetMean() ),drawOpt.Data());
        } else {
          legend->AddEntry(histos[rc],labelLegend.Data(),drawOpt.Data());
        }
      }
    }
    if (histos[0]) histos[0]->Draw("axis,same");                
    
    // labeling right top corner & legend drawing
    legend->Draw();    
    TString label2          = GetLabelVoltageTemp(commonRunInfo);
    TString label3          = "";
    TString label4          = "";
    if (commonRunInfo.species.Contains("injection") || labelOpt > 0 ){
      label3          = GetLabelHGCROCSettingsCF(commonRunInfo);
      label4          = GetLabelHGCROCSettingsRFCC(commonRunInfo);
      if (commonRunInfo.injDAC > -10000. &&  commonRunInfo.injDAC > 0. )
        label2 = label2+Form(",inj=%.1f fC", commonRunInfo.injDAC);
    }
    if ( nSameSettings > 0 ){
      DrawLatex(topRCornerX, startLegY-lineDist, label2, true, sizeLabelsPix, 43);
      DrawLatex(topRCornerX, startLegY-2*lineDist, label3, true, sizeLabelsPix, 43);
      DrawLatex(topRCornerX, startLegY-3*lineDist, label4, true, sizeLabelsPix, 43);
    }
    
    // labeling left top corner
    TString beamline = GetStringFromRunInfo(commonRunInfo, 9).Data();
    TString lab1 = Form("#it{#bf{%s TB:}} %s", (commonRunInfo.detector).Data(), beamline.Data());
    if (beamline.Contains("ORNL")) lab1 = beamline;
    TString lab2 = GetStringFromRunInfo(commonRunInfo, 8);
    TString lab3 = GetStringFromRunInfo(commonRunInfo, 10);
    TString cellLabel           = Form("layer %d", layer);
    if (extCellId != -1 || (commonRunInfo.detector).Contains("FoCal"))
      cellLabel = Form("a:%d, ch:%d",setupT->GetROunit(tempCellID), setupT->GetROchannel(tempCellID) );    
    DrawLatex(topLCornerX, yPosStart, lab1, false, sizeLabelsPix, 43); // beam line info
    DrawLatex(topLCornerX, yPosStart-1*lineDist, lab2, false, sizeLabelsPix, 43); // readout type
    DrawLatex(topLCornerX, yPosStart-2*lineDist, lab3, false, sizeLabelsPix, 43); // TB dates
    DrawLatex(topLCornerX, yPosStart-3*lineDist, cellLabel, false, sizeLabelsPix, 43); // tile label
    
    canvas2Panel->SaveAs(nameOutput.Data());
  }

  //__________________________________________________________________________________________________________
  // Plot Run overlay for all 1 tiles for all runs available
  //__________________________________________________________________________________________________________
  inline void PlotRunOverlayProfile1MLayer (TCanvas* canvas2Panel, Double_t topRCornerX,  Double_t topLCornerX,  Double_t topRCornerY, Double_t relSizeP, Int_t textSizePixel, 
                                      std::map<int,TileTrend> trending, int nruns, int option,
                                      Double_t xPMin, Double_t xPMax, Double_t yPMin, Double_t yPMax,  int layer, int mod,  TString nameOutput, TString nameOutputSummary,
                                      RunInfo commonRunInfo, Int_t detailedPlot = 1, bool scaleInt = false, int labelOpt = 0, int extCellId = -1 ){
                                  
    // loading instance of setip
    Setup* setupT = Setup::GetInstance();
    // checking whether we are plotting a specific cell
    int tempCellID = -1;
    if (layer != -1 && mod != -1 )
      tempCellID = setupT->GetCellID(0,0, layer, mod);
    if (extCellId != -1)
      tempCellID = extCellId;
    if (tempCellID < 0)
      return;

    // checking whether trend is filled
    std::map<int, TileTrend>::iterator ithTrend;    
    ithTrend=trending.find(tempCellID);
    if(ithTrend==trending.end()){
      std::cout << "WARNING: skipping cell ID: " << tempCellID  << "\t layer " << layer << "\t module " << mod << std::endl;
      return;
    } 
    
        // evaluate whether its the same quantities
    bool isSameRun      = false;
    int altStyle        = 0;
    if (commonRunInfo.runNr != -10000){
      isSameRun         = true;
      altStyle          = 1;
    }
    if (commonRunInfo.pdg != -10000)
      altStyle          = 1;
    bool isSameVoltage  = false;
    double commonVoltage  = 0;
    if (commonRunInfo.vop != -10000.){
      isSameVoltage     = true;
      commonVoltage     = commonRunInfo.vop;
    }
    
    // defining global settings
    double width        = 0.5;
    int columns         = 4;
    double labelScaleL  = 0.75;
    double labelScaleT  = 0.75;
    double sizeLabelsPix    = labelScaleT*textSizePixel;
    Int_t nSameSettings = GetNSameSettings(commonRunInfo, labelOpt);
    TString header      = GetHeaderLegendCommonRunObject(commonRunInfo, nSameSettings, width, columns, labelScaleL);
    double lineBottom   = (int)(nruns/columns)+1;
    if (header.CompareTo("") != 0 ) lineBottom++;
    
    double lineDist     = labelScaleT*relSizeP;
    double lineDistL    = labelScaleL*relSizeP;
    
    // entering canvas
    canvas2Panel->cd();
    canvas2Panel->SetLogy(0);
    ithTrend=trending.find(tempCellID);

    // calculating the legend position
    Double_t yPosStart  = topRCornerY -lineDist;
    double startLegY    = topRCornerY -(lineBottom+0.25)*lineDistL;
    double endLegY      = topRCornerY -0.25*lineDistL;
    // std::cout << lineBottom << "\t" << yPosStart << "\t"<< startLegY << "\t"<< endLegY << std::endl;
    
    // defining the legend
    TLegend* legend = GetAndSetLegend2(  topRCornerX, startLegY, 0.6, endLegY,
                                labelScaleL*textSizePixel, columns, header,43,0.25);    
    
    TProfile* profs[30];  // maximum 30 profiles to be loaded
    TH1D* dummyhist = nullptr;
    for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
      int tmpRunNr = ithTrend->second.GetRunNr(rc);
      profs[rc] = nullptr;
      if (tmpRunNr != -1) {
        if (option == 0)
          profs[rc] = ithTrend->second.GetLGHGTriggRun(ithTrend->second.GetRunNr(rc));
        else if (option == 1)
          profs[rc] = ithTrend->second.GetWave1DRun(ithTrend->second.GetRunNr(rc));
        else if (option == 2)
          profs[rc] = ithTrend->second.GetTOARun(ithTrend->second.GetRunNr(rc));
        else if (option == 3)  
          profs[rc] = ithTrend->second.GetTOTRun(ithTrend->second.GetRunNr(rc));
        if (scaleInt&& profs[rc] != nullptr) profs[rc]->Scale(1/profs[rc]->Integral());
        // std::cout << profs[rc] << std::endl;
      }
      if (profs[rc]){
        if (rc == 0){
          TString yTitle = profs[rc]->GetYaxis()->GetTitle();
          if (scaleInt) yTitle = Form("%s/ integral", yTitle.Data());
          dummyhist = new TH1D("dummyhist", "", profs[rc]->GetNbinsX(), profs[rc]->GetXaxis()->GetXmin(), profs[rc]->GetXaxis()->GetXmax());
          SetStyleHistoTH1ForGraphs( dummyhist, profs[rc]->GetXaxis()->GetTitle(), yTitle, labelScaleT*textSizePixel, textSizePixel, labelScaleT*textSizePixel, textSizePixel,0.85, 1.3, 510, 510, 43, 63);
          dummyhist->GetXaxis()->SetRangeUser(xPMin,xPMax);
          dummyhist->GetYaxis()->SetRangeUser(yPMin,yPMax);
          dummyhist->Draw("axis");
        }

        SetLineDefaults(profs[rc], GetColorLayer(rc, altStyle), 2, GetLineStyleLayer(rc, altStyle));   
        profs[rc]->SetMarkerSize(0.7);
        profs[rc]->SetMarkerStyle(GetMarkerLayer(rc,true));
        profs[rc]->Draw("same,pe");
        
        TString labelLegend = ithTrend->second.GetLabel(rc);
        legend->AddEntry(profs[rc],labelLegend.Data(),"p");
      }
    }
    if (dummyhist) dummyhist->Draw("axis,same");                
    
    // labeling right top corner & legend draw
    legend->Draw();
    TString label2          = GetLabelVoltageTemp(commonRunInfo);
    TString label3          = "";
    TString label4          = "";
    if (commonRunInfo.species.Contains("injection") || labelOpt > 0){
      label3          = GetLabelHGCROCSettingsCF(commonRunInfo);
      label4          = GetLabelHGCROCSettingsRFCC(commonRunInfo);
      if (commonRunInfo.injDAC > -10000. &&  commonRunInfo.injDAC > 0. )
        label2 = label2+Form(",inj=%.1f fC", commonRunInfo.injDAC);
    }
    if ( nSameSettings > 0 ){
      DrawLatex(topRCornerX, startLegY-lineDist, label2, true, sizeLabelsPix, 43);
      DrawLatex(topRCornerX, startLegY-2*lineDist, label3, true, sizeLabelsPix, 43);
      DrawLatex(topRCornerX, startLegY-3*lineDist, label4, true, sizeLabelsPix, 43);
    }
    
    // labeling left top corner
    TString cellLabel           = Form("layer %d", layer);
    if (extCellId != -1 || (commonRunInfo.detector).Contains("FoCal"))
      cellLabel = Form("a:%d, ch:%d",setupT->GetROunit(tempCellID), setupT->GetROchannel(tempCellID) );
    
    TString beamline = GetStringFromRunInfo(commonRunInfo, 9).Data();
    TString lab1 = Form("#it{#bf{%s TB:}} %s", (commonRunInfo.detector).Data(), beamline.Data());
    if (beamline.Contains("ORNL"))
      lab1 = beamline;
    TString lab2 = GetStringFromRunInfo(commonRunInfo, 8);
    TString lab3 = GetStringFromRunInfo(commonRunInfo, 10);
    DrawLatex(topLCornerX, yPosStart, lab1, false, sizeLabelsPix, 43);  // beam line info
    DrawLatex(topLCornerX, yPosStart-1*lineDist, lab2, false, sizeLabelsPix, 43);  // readout type
    DrawLatex(topLCornerX, yPosStart-2*lineDist, lab3, false, sizeLabelsPix, 43);  // TB dates
    DrawLatex(topLCornerX, yPosStart-3*lineDist, cellLabel, false, sizeLabelsPix, 43); // tile label
  
    canvas2Panel->SaveAs(nameOutput.Data());
  }
  
#endif

