#ifndef PLOTTHELPER_1MLAYER_H
#define PLOTTHELPER_1MLAYER_H

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // dedicated class for all 1M layer plotting functions
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  //__________________________________________________________________________________________________________
  // Plot Noise with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotNoiseWithFits1MLayer (TCanvas* canvas2Panel, Double_t topRCornerX,  Double_t topRCornerY, Double_t relSizeP, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int option, 
                                  Double_t xPMin, Double_t xPMax, Double_t scaleYMax, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    
    Setup* setupT = Setup::GetInstance();
    
    int skipped = 0;
    int tempCellID = setupT->GetCellID(0,0, layer, mod);
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
                                  Double_t xPMin, Double_t xPMax, Double_t scaleYMax, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    Setup* setupT = Setup::GetInstance();
    
    int tempCellID = setupT->GetCellID(0,0, layer, mod);
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
                                  Double_t xPMin, Double_t xPMax, Double_t maxY, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileSpectra>::iterator ithSpectra;    
    int skipped = 0;
    ReadOut::Type rotype = ReadOut::Type::Undef;
    
    
    canvas2Panel->cd();
    int tempCellID = setupT->GetCellID(0,0, layer, mod);
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
    temp2D->Draw("col");

    DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xPMin, 0, xPMax, maxY);
    temp2D->Draw("axis,same");
    
    if (tempProfile){
      SetMarkerDefaultsProfile(tempProfile, 24, 0.3, kRed+2, kRed+2);   
      tempProfile->Draw("pe, same");
    } 
        
    TString label           = Form("layer %d", layer);
    TLatex *labelChannel    = new TLatex(topRCornerX,topRCornerY-1.2*relSizeP,label);
    SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,11);

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
        legend = GetAndSetLegend2( topRCornerX, topRCornerY-4*0.85*relSizeP-0.4*relSizeP, topRCornerX+6*relSizeP, topRCornerY-0.6*relSizeP,0.85*textSizePixel, 1, label, 43,0.1);
        legend->AddEntry(fit, "linear fit, trigg.", "l");
        legend->AddEntry((TObject*)0, Form("#scale[0.8]{b = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");
        legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(1), fit->GetParError(1) ) , " ");
      } else {
        legend = GetAndSetLegend2( topRCornerX, topRCornerY-3*0.85*relSizeP-0.4*relSizeP, topRCornerX+6*relSizeP, topRCornerY-0.6*relSizeP,0.85*textSizePixel, 1, label, 43,0.1);
        legend->AddEntry(fit, "const fit", "l");
        legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");   
      }
      legend->Draw();
    } else {
      labelChannel->Draw();  
    }
  
    if (option == 1){
      for (int i = 0; i < currRunInfo.samples; i++){
          DrawLines(i*25e3, i*25e3, 0,maxY*0.2, 1, kGray+2, 7, 1);
      } 
    }
  
    DrawLatex(topRCornerX, topRCornerY-offset*0.85*relSizeP-1.4*relSizeP, GetStringFromRunInfo(currRunInfo, 2), false, 0.85*relSizeP, 42);
    DrawLatex(topRCornerX, topRCornerY-offset*0.85*relSizeP-2.2*relSizeP, GetStringFromRunInfo(currRunInfo, 3), false, 0.85*relSizeP, 42);
    canvas2Panel->SaveAs(nameOutput.Data());
  }
//   
//   //__________________________________________________________________________________________________________
//   // Plot Run overlay for all 1 tiles for all runs available
//   //__________________________________________________________________________________________________________
  inline void PlotRunOverlay1MLayer (TCanvas* canvas2Panel, Double_t topRCornerX, Double_t topLCornerX,  Double_t topRCornerY, Double_t relSizeP, Int_t textSizePixel, 
                              std::map<int,TileTrend> trending, int nruns, int optionTrend, 
                              Double_t xPMin, Double_t xPMax, int layer, int mod,  TString nameOutput, TString nameOutputSummary, RunInfo currRunInfo, Int_t detailedPlot = 1, bool plotMean = false){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    
    Double_t maxY         = 0.;
    Double_t minY         = 9999.;
    bool isSameVoltage    = true;
    double commanVoltage  = 0;
    int tempCellID = setupT->GetCellID(0,0, layer, mod);
    ithTrend=trending.find(tempCellID);
    if(ithTrend==trending.end()){
      skipped++;
      std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t layer " << layer << "\t module " << mod << std::endl;
      return;
    } 
    if (optionTrend == 0){      // HG
      if(maxY<ithTrend->second.GetMaxHGSpec()) maxY=ithTrend->second.GetMaxHGSpec();
      if(minY>ithTrend->second.GetMinHGSpec()) minY=ithTrend->second.GetMinHGSpec();
    } else if (optionTrend == 1){   //LG
      if(maxY<ithTrend->second.GetMaxLGSpec()) maxY=ithTrend->second.GetMaxLGSpec();
      if(minY>ithTrend->second.GetMinLGSpec()) minY=ithTrend->second.GetMinLGSpec();
    } 
    
    for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
      if (rc == 0){
        commanVoltage = ithTrend->second.GetVoltage(rc);
      } else {
        if (commanVoltage != ithTrend->second.GetVoltage(rc))  isSameVoltage = false;
      }
    }
    
    if (maxY == 0 && minY == 9999.){
      std::cout <<"Something went wrong! No ranges set for layer " <<  layer << " \t trend plotting option: " << optionTrend << "\t ABORTING!" << std::endl;
      return;
    }
    maxY = 3*maxY;
    TH1D* histos[30];

    Int_t columns     = 4;
    double lineBottom  = (7+0.1);
    double scaleTextSize = 0.75;
    if (nruns < 5) lineBottom = (1+0.1);
    else if (nruns < 9) lineBottom = (2+0.1);
    else if (nruns < 13) lineBottom = (3+0.1);
    else if (nruns < 17) lineBottom = (4+0.1);
    else if (nruns < 21) lineBottom = (5+0.1);
    else if (nruns < 25) lineBottom = (6+0.1);
    
    if (plotMean) {
      columns     = 2;
      lineBottom  = (7+0.1);
      if (nruns < 3) lineBottom = (1+0.1);
      else if (nruns < 5) lineBottom = (2+0.1);
      else if (nruns < 7) lineBottom = (3+0.1);
      else if (nruns < 9) lineBottom = (4+0.1);
      else if (nruns < 11) lineBottom = (5+0.1);
      else if (nruns < 13) lineBottom = (6+0.1);
      scaleTextSize = 0.65; 
    }

    double startLegY  = topRCornerY-lineBottom*relSizeP;
    double endLegY    = topRCornerY-relSizeP;
    
    TLegend* legend = GetAndSetLegend2(  0.6, startLegY, topRCornerX, endLegY,
                                        scaleTextSize*textSizePixel, columns, "",43,0.25);


    canvas2Panel->cd();
    canvas2Panel->SetLogy(1);
    
    for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
      int tmpRunNr = ithTrend->second.GetRunNr(rc);
      histos[rc] = nullptr;
      if (tmpRunNr != -1) {
        if (optionTrend == 0){      // HG
          histos[rc] = ithTrend->second.GetHGTriggRun(ithTrend->second.GetRunNr(rc));
        } else if (optionTrend == 1){      // LG
          histos[rc] = ithTrend->second.GetLGTriggRun(ithTrend->second.GetRunNr(rc));
        }
      }
      if (histos[rc]){
        SetStyleHistoTH1ForGraphs( histos[rc], histos[rc]->GetXaxis()->GetTitle(), histos[rc]->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.95, 1.3,  510, 510, 43, 63);  
        SetLineDefaults(histos[rc], GetColorLayer(rc), 2, GetLineStyleLayer(rc));   
        if(rc == 0){
          histos[rc]->GetXaxis()->SetRangeUser(xPMin,xPMax);
          histos[rc]->GetYaxis()->SetRangeUser(minY,maxY);
          histos[rc]->Draw("hist");
        } else {
          histos[rc]->Draw("same,hist");
        }
        if (plotMean){
          DrawLines(histos[rc]->GetMean(), histos[rc]->GetMean(),minY, 0.2*maxY, 4,GetColorLayer(rc) ,GetLineStyleLayer(rc));
          std::cout << tmpRunNr << "\t" <<  histos[rc]->GetMean() << std::endl;
          legend->AddEntry(histos[rc],Form("%d (%.1f)",tmpRunNr,histos[rc]->GetMean() ),"l");
        } else {
          legend->AddEntry(histos[rc],Form("%d",tmpRunNr),"l");
        }
      }
    }
    if (histos[0]) histos[0]->Draw("axis,same");                
    
    legend->Draw();
    TString label           = Form("layer %d", layer);
    TString label2          = Form("Common V_{op} = %2.1f V", commanVoltage);
    TString beamline = GetStringFromRunInfo(currRunInfo, 9).Data();
    TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", beamline.Data());
    // labeling inside the panels & legend drawing 
    DrawLatex(topRCornerX, topRCornerY-(lineBottom+1.3)*relSizeP, label, true, 0.75*textSizePixel, 43);
    if (isSameVoltage ){
      DrawLatex(topRCornerX, topRCornerY-(lineBottom+0.5)*relSizeP, label2, true, 0.75*textSizePixel, 43);
    }    
    if (beamline.Contains("ORNL")) lab1 = beamline;
    TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
    TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
    DrawLatex(topLCornerX, topRCornerY-relSizeP-1*0.75*relSizeP, lab1, false, 0.85*textSizePixel, 43);
    DrawLatex(topLCornerX, topRCornerY-relSizeP-2*0.75*relSizeP, lab2, false, 0.85*textSizePixel, 43);
    DrawLatex(topLCornerX, topRCornerY-relSizeP-3*0.75*relSizeP, lab3, false, 0.85*textSizePixel, 43);
    if(detailedPlot) canvas2Panel->SaveAs(nameOutput.Data());
    if (layer == 0) canvas2Panel->Print(Form("%s.pdf[",nameOutputSummary.Data()));
    canvas2Panel->Print(Form("%s.pdf",nameOutputSummary.Data()));
    if (layer == setupT->GetNMaxLayer()) canvas2Panel->Print(Form("%s.pdf]",nameOutputSummary.Data()));
  }

  //__________________________________________________________________________________________________________
  // Plot Run overlay for all 1 tiles for all runs available
  //__________________________________________________________________________________________________________
  inline void PlotRunOverlayProfile1MLayer (TCanvas* canvas2Panel, Double_t topRCornerX,  Double_t topLCornerX,  Double_t topRCornerY, Double_t relSizeP, Int_t textSizePixel, 
                                      std::map<int,TileTrend> trending, int nruns,
                                      Double_t xPMin, Double_t xPMax, Double_t yPMin, Double_t yPMax,  int layer, int mod,  TString nameOutput, TString nameOutputSummary, RunInfo currRunInfo, Int_t detailedPlot = 1, bool scaleInt = false ){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    
    bool isSameVoltage    = true;
    double commanVoltage  = 0;
    
    int tempCellID = setupT->GetCellID(0,0, layer, mod);
    ithTrend=trending.find(tempCellID);
    if(ithTrend==trending.end()){
      std::cout << "WARNING: skipping cell ID: " << tempCellID  << "\t layer " << layer << "\t module " << mod << std::endl;
      return;
    } 

    for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
      if (rc == 0){
        commanVoltage = ithTrend->second.GetVoltage(rc);
      } else {
        if (commanVoltage != ithTrend->second.GetVoltage(rc))  isSameVoltage = false;
      }
    }
    TProfile* profs[30];

    double lineBottom  = (7);
    if (nruns < 5) lineBottom = (1);
    else if (nruns < 9) lineBottom = (2);
    else if (nruns < 13) lineBottom = (3);
    else if (nruns < 17) lineBottom = (4);
    else if (nruns < 21) lineBottom = (5);
    else if (nruns < 25) lineBottom = (6);
    
    canvas2Panel->cd();
    canvas2Panel->SetLogy(0);
    ithTrend=trending.find(tempCellID);

    TString label           = Form("layer %d", layer);
    TString label2          = Form("Common V_{op} = %2.1f V", commanVoltage);
    
    Double_t yPosStart = topRCornerY;
    double startLegY  = yPosStart -lineBottom*relSizeP;
    double endLegY    = yPosStart;
    TLegend* legend = GetAndSetLegend2(  topRCornerX, startLegY, 0.55, endLegY,
                                0.75*textSizePixel, 4, "",43,0.25);    
    TH1D* dummyhist;
    for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
      int tmpRunNr = ithTrend->second.GetRunNr(rc);
      profs[rc] = nullptr;
      if (tmpRunNr != -1) {
        profs[rc] = ithTrend->second.GetLGHGTriggRun(ithTrend->second.GetRunNr(rc));
        if (scaleInt) profs[rc]->Scale(1/profs[rc]->Integral());
        std::cout << profs[rc] << std::endl;
      }
      if (profs[rc]){
        if (rc == 0){
          TString yTitle = profs[rc]->GetYaxis()->GetTitle();
          if (scaleInt) yTitle = Form("%s/ integral", yTitle.Data());
          dummyhist = new TH1D("dummyhist", "", profs[rc]->GetNbinsX(), profs[rc]->GetXaxis()->GetXmin(), profs[rc]->GetXaxis()->GetXmax());
          SetStyleHistoTH1ForGraphs( dummyhist, profs[rc]->GetXaxis()->GetTitle(), yTitle, 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);
          dummyhist->GetXaxis()->SetRangeUser(xPMin,xPMax);
          dummyhist->GetYaxis()->SetRangeUser(yPMin,yPMax);
          dummyhist->Draw("axis");
        }

        SetLineDefaults(profs[rc], GetColorLayer(rc), 2, GetLineStyleLayer(rc));   
        profs[rc]->SetMarkerStyle(24);
        if (rc < 10)
          profs[rc]->SetMarkerStyle(24);
        else 
          profs[rc]->SetMarkerStyle(25);
        profs[rc]->Draw("same,pe");
        legend->AddEntry(profs[rc],Form("%d",tmpRunNr),"p");
      }
    }
    if (dummyhist) dummyhist->Draw("axis,same");                
    
    // labeling inside the panels & legend drawing 
    if (isSameVoltage){
      DrawLatex(topRCornerX, yPosStart-(lineBottom+0.5)*relSizeP, label2, false, 0.75*textSizePixel, 43);
    }
    
    legend->Draw();
    TString beamline = GetStringFromRunInfo(currRunInfo, 9).Data();
    TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", beamline.Data());
    if (beamline.Contains("ORNL"))
      lab1 = beamline;
    TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
    TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
    DrawLatex(topLCornerX, yPosStart-1*0.75*relSizeP, lab1, true, 0.75*textSizePixel, 43);
    DrawLatex(topLCornerX, yPosStart-2*0.75*relSizeP, lab2, true, 0.75*textSizePixel, 43);
    DrawLatex(topLCornerX, yPosStart-3*0.75*relSizeP, lab3, true, 0.75*textSizePixel, 43);
    DrawLatex(topLCornerX, yPosStart-4*0.75*relSizeP, label, true, 0.75*textSizePixel, 43);
  
    if(detailedPlot) canvas2Panel->SaveAs(nameOutput.Data());
    if (layer == 0) canvas2Panel->Print(Form("%s.pdf[",nameOutputSummary.Data()));
    canvas2Panel->Print(Form("%s.pdf",nameOutputSummary.Data()));
    if (layer == setupT->GetNMaxLayer()) canvas2Panel->Print(Form("%s.pdf]",nameOutputSummary.Data()));
  }
  
#endif

