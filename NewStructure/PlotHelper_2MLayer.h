#ifndef PLOTTHELPER_2MLAYER_H
#define PLOTTHELPER_2MLAYER_H
      //***********************************************************************************************************
      //********************************* 2 Panel overview plot  **************************************************
      //***********************************************************************************************************
      //*****************************************************************
        // Test beam geometry (beam coming from viewer)
        //===============================
        //||    1 (0)    ||    2 (1)   ||  row 0
        //===============================
        //    col 0     col 1     
        // rebuild pad geom in similar way (numbering -1)
      //*****************************************************************
       
  //__________________________________________________________________________________________________________
  // Plot Noise with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotNoiseWithFits2MLayer (TCanvas* canvas2Panel, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSizeP, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int option, 
                                  Double_t xPMin, Double_t xPMax, Double_t scaleYMax, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    Setup* setupT = Setup::GetInstance();
    
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    for (int c = 0; c < nCol; c++){
      int tempCellID = setupT->GetCellID(0,c, layer, mod);
      ithSpectra=spectra.find(tempCellID);
      if(ithSpectra==spectra.end()){
        std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << 0 << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
        skipped++;
        continue;
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
    }  
    
    for (int c = 0; c < nCol; c++){
      canvas2Panel->cd();
      int tempCellID = setupT->GetCellID(0,c, layer, mod);
      pads[c]->Draw();
      pads[c]->cd();
      pads[c]->SetLogy();
      ithSpectra=spectra.find(tempCellID);
      if(ithSpectra==spectra.end()){
        std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << 0 << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
        pads[c]->Clear();
        pads[c]->Draw();
        if (c ==1 ){
          DrawLatex(topRCornerX[c]-0.04, topRCornerY[c]-4*0.85*relSizeP[c]-1.4*relSizeP[c], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeP[c], 42);
          DrawLatex(topRCornerX[c]-0.04, topRCornerY[c]-4*0.85*relSizeP[c]-2.2*relSizeP[c], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeP[c], 42);
        }
        continue;
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
      
      TString label           = Form("col %d", c);
      if (c == 1){
        label = Form("col %d layer %d", c, layer);
      }
      TLatex *labelChannel    = new TLatex(topRCornerX[c]-0.04,topRCornerY[c]-1.2*relSizeP[c],label);
      SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);

      
      TF1* fit = nullptr;
      if (option == 0){
        fit = ithSpectra->second.GetBackModel(1);
      } else if (option == 1){
        fit = ithSpectra->second.GetBackModel(0);  
      }
      if (fit){
        SetStyleFit(fit , xPMin, xPMax, 7, 7, kBlack);
        fit->Draw("same");
        TLegend* legend = GetAndSetLegend2( topRCornerX[c]-8*relSizeP[c], topRCornerY[c]-4*0.85*relSizeP[c]-0.4*relSizeP[c], topRCornerX[c]-0.04, topRCornerY[c]-0.6*relSizeP[c],0.85*textSizePixel, 1, label, 43,0.2);
        legend->AddEntry(fit, "Gauss noise fit", "l");
        legend->AddEntry((TObject*)0, Form("#mu = %2.2f #pm %2.2f",fit->GetParameter(1), fit->GetParError(1) ) , " ");
        legend->AddEntry((TObject*)0, Form("#sigma = %2.2f #pm %2.2f",fit->GetParameter(2), fit->GetParError(2) ) , " ");
        legend->Draw();
          
      } else {
        labelChannel->Draw();  
      }
    
      if (xPMin < -5) DrawLines(0, 0,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
    
      if (c ==1 ){
        DrawLatex(topRCornerX[c]-0.04, topRCornerY[c]-4*0.85*relSizeP[c]-1.4*relSizeP[c], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeP[c], 42);
        DrawLatex(topRCornerX[c]-0.04, topRCornerY[c]-4*0.85*relSizeP[c]-2.2*relSizeP[c], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeP[c], 42);
      }
    }
    if (skipped < 2)
      canvas2Panel->SaveAs(nameOutput.Data());
  }

  //__________________________________________________________________________________________________________
  // Plot Spectra with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotSpectra2MLayer (TCanvas* canvas2Panel, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSizeP, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int option, 
                                  Double_t xPMin, Double_t xPMax, Double_t scaleYMax, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    Setup* setupT = Setup::GetInstance();
    
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    for (int c = 0; c < nCol; c++){
      int tempCellID = setupT->GetCellID(0,c, layer, mod);
      ithSpectra=spectra.find(tempCellID);
      if(ithSpectra==spectra.end()){
        std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
        continue;
      } 
      TH1D* tempHist = nullptr;
      if (option == 0){
        tempHist = ithSpectra->second.GetHG();
      } else if (option == 1){
        tempHist = ithSpectra->second.GetLG();
      } else if (option == 2){
        tempHist = ithSpectra->second.GetComb();
      } else if (option == 3){
        tempHist = ithSpectra->second.GetTOA();
      } else if (option == 4){
        tempHist = ithSpectra->second.GetTOT();
      }
      if (maxY < FindLargestBin1DHist(tempHist, xPMin , xPMax)) maxY = FindLargestBin1DHist(tempHist, xPMin , xPMax);
    }  
    
    for (int c = 0; c < nCol; c++){
      canvas2Panel->cd();
      int tempCellID = setupT->GetCellID(0,c, layer, mod);
      pads[c]->Draw();
      pads[c]->cd();
      pads[c]->SetLogy();
      ithSpectra=spectra.find(tempCellID);
      if(ithSpectra==spectra.end()){
        skipped++;
        std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
        pads[c]->Clear();
        pads[c]->Draw();
        if (c ==1 ){
          DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-4*0.85*relSizeP[c]-1.4*relSizeP[c], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeP[c], 42);
          DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-4*0.85*relSizeP[c]-2.2*relSizeP[c], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeP[c], 42);
        }          
        continue;
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
              
      TString label           = Form("col %d", c);
      if (c == 1){
        label = Form("col %d layer %d", c, layer);
      }
      TLatex *labelChannel    = new TLatex(topRCornerX[c]-0.045,topRCornerY[c]-1.2*relSizeP[c],label);
      SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);
      labelChannel->Draw();  
    
      if (option < 2){
        DrawLines(noiseWidth*3, noiseWidth*3,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
        DrawLines(noiseWidth*5, noiseWidth*5,0.7, scaleYMax*maxY, 2, kGray+1, 6);  
      } else if (option == 3){
        DrawLines(0.3, 0.3, 0.7, scaleYMax*maxY, 2, kGray+1, 10);  
      }
      if (c == 1 ){
        DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-4*0.85*relSizeP[c]-1.4*relSizeP[c], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeP[c], 42);
        DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-4*0.85*relSizeP[c]-2.2*relSizeP[c], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeP[c], 42);
      }
    }
    if (skipped < 2)
      canvas2Panel->SaveAs(nameOutput.Data());
  }

  //__________________________________________________________________________________________________________
  // Plot Corr with Fits for Full layer 2D
  //__________________________________________________________________________________________________________
  inline void PlotCorr2D2MLayer (TCanvas* canvas2Panel, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSizeP, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int option, 
                                  Double_t xPMin, Double_t xPMax, Double_t maxY, int layer, int mod,  TString nameOutput, RunInfo currRunInfo, bool noCalib = 0){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileSpectra>::iterator ithSpectra;    
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    ReadOut::Type rotype = ReadOut::Type::Undef;
    
    for (int c = 0; c < nCol; c++){
      canvas2Panel->cd();
      int tempCellID = setupT->GetCellID(0,c, layer, mod);
      pads[c]->Draw();
      pads[c]->SetLogy(0);
      pads[c]->SetLogz(1);
      pads[c]->cd();
      
      Int_t offset = 1;
      
      ithSpectra=spectra.find(tempCellID);
      if(ithSpectra==spectra.end()){
        skipped++;
        std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << 0 << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
        pads[c]->Clear();
        pads[c]->Draw();
        if (c == 1 ){
          DrawLatex(topRCornerX[c]+0.045, topRCornerY[c]-offset*0.85*relSizeP[c]-1.4*relSizeP[c], GetStringFromRunInfo(currRunInfo, 2), false, 0.85*relSizeP[c], 42);
          DrawLatex(topRCornerX[c]+0.045, topRCornerY[c]-offset*0.85*relSizeP[c]-2.2*relSizeP[c], GetStringFromRunInfo(currRunInfo, 3), false, 0.85*relSizeP[c], 42);
        }
        continue;
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
        // tempProfile     = ithSpectra->second.GetWave1D();
        temp2D          = ithSpectra->second.GetCorr();          
      // HGCROC TOA-ADC correlation
      } else if (option == 2){
        // tempProfile     = ithSpectra->second.GetTOAADC();
        temp2D          = ithSpectra->second.GetCorrTOAADC();                    
      } else if (option == 3){
        temp2D          = ithSpectra->second.GetCorrTOASample();
      } else if (option == 4){
        tempProfile     = ithSpectra->second.GetADCTOT();
      } else if (option == 5){
        tempProfile     = ithSpectra->second.GetTOTProfile();
      } else if (option == 6){
        tempProfile     = ithSpectra->second.GetTOAProfile();
      }
        
      if (!temp2D && option < 4) continue;
        
      if (temp2D){  
        SetStyleHistoTH2ForGraphs( temp2D, temp2D->GetXaxis()->GetTitle(), temp2D->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
        SetMarkerDefaultsProfile(tempProfile, 24, 0.7, kRed+2, kRed+2);   
        
        temp2D->GetYaxis()->SetRangeUser(-10,maxY);
        temp2D->GetXaxis()->SetRangeUser(0,xPMax);
        temp2D->Draw("col");
        if( !noCalib ){
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xPMin, 0, xPMax, maxY);
          temp2D->Draw("axis,same");
        }
      } else {
        if (!tempProfile) continue;
        TH1D* dummyhist = new TH1D(Form("dummyhist %d %d",layer,c), "", tempProfile->GetNbinsX(), tempProfile->GetXaxis()->GetXmin(), tempProfile->GetXaxis()->GetXmax());
        SetStyleHistoTH1ForGraphs( dummyhist, tempProfile->GetXaxis()->GetTitle(), tempProfile->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
        dummyhist->SetStats(0);
        dummyhist->GetXaxis()->SetRangeUser(xPMin,xPMax);
        dummyhist->GetYaxis()->SetRangeUser(0,maxY);
        dummyhist->Draw("axis");
        if( !noCalib ){
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xPMin, 0, xPMax, maxY);
          dummyhist->Draw("axis,same");
        }                  
      }
      if (tempProfile ){
        SetMarkerDefaultsProfile(tempProfile, 24, 0.7, kRed+2, kRed+2);           
        tempProfile->Draw("pe, same");
      }
        
      TString label           = Form("col %d", c);
      if (c == 1){
        label = Form("col %d layer %d", c, layer);
      }
      TLatex *labelChannel    = new TLatex(topRCornerX[c]+0.045,topRCornerY[c]-1.2*relSizeP[c],label);
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
          legend = GetAndSetLegend2( topRCornerX[c]+0.045, topRCornerY[c]-4*0.85*relSizeP[c]-0.4*relSizeP[c], topRCornerX[c]+6*relSizeP[c], topRCornerY[c]-0.6*relSizeP[c],0.85*textSizePixel, 1, label, 43,0.1);
          legend->AddEntry(fit, "linear fit, trigg.", "l");
          legend->AddEntry((TObject*)0, Form("#scale[0.8]{b = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");
          legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(1), fit->GetParError(1) ) , " ");
        } else {
          legend = GetAndSetLegend2( topRCornerX[c]+0.045, topRCornerY[c]-3*0.85*relSizeP[c]-0.4*relSizeP[c], topRCornerX[c]+6*relSizeP[c], topRCornerY[c]-0.6*relSizeP[c],0.85*textSizePixel, 1, label, 43,0.1);
          legend->AddEntry(fit, "const fit", "l");
          legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");   
        }
        legend->Draw();
      } else {
        labelChannel->Draw();  
      }
    
      if (c ==1 ){
        DrawLatex(topRCornerX[c]+0.045, topRCornerY[c]-offset*0.85*relSizeP[c]-1.4*relSizeP[c], GetStringFromRunInfo(currRunInfo, 2), false, 0.85*relSizeP[c], 42);
        DrawLatex(topRCornerX[c]+0.045, topRCornerY[c]-offset*0.85*relSizeP[c]-2.2*relSizeP[c], GetStringFromRunInfo(currRunInfo, 3), false, 0.85*relSizeP[c], 42);
      }
    }
    if (skipped < 2)
      canvas2Panel->SaveAs(nameOutput.Data());
  }
//   //__________________________________________________________________________________________________________
//   // Plot Run overlay for all 2 tiles for all runs available
//   //__________________________________________________________________________________________________________
  inline void PlotRunOverlay2MLayer (TCanvas* canvas2Panel, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSizeP, Int_t textSizePixel, 
                              std::map<int,TileTrend> trending, int nruns, int optionTrend, 
                              Double_t xPMin, Double_t xPMax, int layer, int mod,  TString nameOutput, TString nameOutputSummary, RunInfo currRunInfo, Int_t detailedPlot = 1){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    
    Double_t maxY         = 0.;
    Double_t minY         = 9999.;
    bool isSameVoltage    = true;
    double commanVoltage  = 0;
    for (int c = 0; c < nCol; c++){
      int tempCellID = setupT->GetCellID(0,c, layer, mod);
      ithTrend=trending.find(tempCellID);
      if (optionTrend == 0){      // HG
        if(maxY<ithTrend->second.GetMaxHGSpec()) maxY=ithTrend->second.GetMaxHGSpec();
        if(minY>ithTrend->second.GetMinHGSpec()) minY=ithTrend->second.GetMinHGSpec();
      } else if (optionTrend == 1){   //LG
        if(maxY<ithTrend->second.GetMaxLGSpec()) maxY=ithTrend->second.GetMaxLGSpec();
        if(minY>ithTrend->second.GetMinLGSpec()) minY=ithTrend->second.GetMinLGSpec();
      } 
      
      for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
        if (c == 0){
          if (rc == 0){
            commanVoltage = ithTrend->second.GetVoltage(rc);
          } else {
            if (commanVoltage != ithTrend->second.GetVoltage(rc))  isSameVoltage = false;
          }
        }
      }
    }
    if (maxY == 0 && minY == 9999.){
      std::cout <<"Something went wrong! No ranges set for layer " <<  layer << " \t trend plotting option: " << optionTrend << "\t ABORTING!" << std::endl;
      return;
    }
    maxY = 3*maxY;
    TH1D* histos[30];

    double lineBottom  = (1.4+6);
    if (nruns < 6) lineBottom = (1.4+1);
    else if (nruns < 11) lineBottom = (1.4+2);
    else if (nruns < 16) lineBottom = (1.4+3);
    else if (nruns < 21) lineBottom = (1.4+4);
    else if (nruns < 26) lineBottom = (1.4+5);
    TLegend* legend = nullptr;
  
    for (int c = 0; c < nCol; c++){
      canvas2Panel->cd();
      int tempCellID = setupT->GetCellID(0,c, layer, mod);
      pads[c]->Draw();
      pads[c]->cd();
      pads[c]->SetLogy(1);
      ithTrend=trending.find(tempCellID);

      TString label           = Form("col %d", c);
      TString label2          = Form("Common V_{op} = %2.1f V", commanVoltage);
      if ( c == 1 ){
        label = Form("col %d layer %d", c, layer);
      }
      if(ithTrend==trending.end()){
        skipped++;
        std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
        pads[c]->Clear();
        pads[c]->Draw();
        if (c == 0 ){
          TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
          TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
          TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
          DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-1.2*relSizeP[c]-1*0.85*relSizeP[c], lab1, true, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-1.2*relSizeP[c]-2*0.85*relSizeP[c], lab2, true, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-1.2*relSizeP[c]-3*0.85*relSizeP[c], lab3, true, 0.85*textSizePixel, 43);
        }
        
        DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-1.2*relSizeP[c], label, true, 0.85*textSizePixel, 43);
        continue;
      } 

      if ( c == 1 ){
        double startLegY  = topRCornerY[c]-lineBottom*relSizeP[c];
        double endLegY    = topRCornerY[c]-1.4*relSizeP[c];
        legend = GetAndSetLegend2(  0.3, startLegY, topRCornerX[c]-0.045/2, endLegY,
                                    0.85*textSizePixel, 5, "",43,0.25);
      }
      
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
          if(c == 1) legend->AddEntry(histos[rc],Form("%d",tmpRunNr),"l");
        }
      }
      if (histos[0]) histos[0]->Draw("axis,same");                
      
      // labeling inside the panels & legend drawing 
      DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-1.2*relSizeP[c], label, true, 0.85*textSizePixel, 43);
      if (isSameVoltage && c == 1){
        DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-(lineBottom+0.5)*relSizeP[c], label2, true, 0.85*textSizePixel, 43);
      }
      
      if (c == 1) legend->Draw();
      if (c == 0 ){
        TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
        TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
        TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
        DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-1.2*relSizeP[c]-1*0.85*relSizeP[c], lab1, true, 0.85*textSizePixel, 43);
        DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-1.2*relSizeP[c]-2*0.85*relSizeP[c], lab2, true, 0.85*textSizePixel, 43);
        DrawLatex(topRCornerX[c]-0.045, topRCornerY[c]-1.2*relSizeP[c]-3*0.85*relSizeP[c], lab3, true, 0.85*textSizePixel, 43);
      }
    }
    if (skipped < 2){
      if(detailedPlot) canvas2Panel->SaveAs(nameOutput.Data());
      if (layer == 0) canvas2Panel->Print(Form("%s.pdf[",nameOutputSummary.Data()));
      canvas2Panel->Print(Form("%s.pdf",nameOutputSummary.Data()));
      if (layer == setupT->GetNMaxLayer()) canvas2Panel->Print(Form("%s.pdf]",nameOutputSummary.Data()));
    }
  }

  //__________________________________________________________________________________________________________
  // Plot Run overlay for all 8 tiles for all runs available
  //__________________________________________________________________________________________________________
  inline void PlotRunOverlayProfile2MLayer (TCanvas* canvas2Panel, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSizeP, Int_t textSizePixel, 
                                      std::map<int,TileTrend> trending, int nruns,
                                      Double_t xPMin, Double_t xPMax, Double_t yPMin, Double_t yPMax,  int layer, int mod,  TString nameOutput, TString nameOutputSummary, RunInfo currRunInfo, Int_t detailedPlot = 1){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    
    bool isSameVoltage    = true;
    double commanVoltage  = 0;
    for (int c = 0; c < nCol; c++){
      int tempCellID = setupT->GetCellID(0,c, layer, mod);
      ithTrend=trending.find(tempCellID);
      for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
        if (c == 0){
          if (rc == 0){
            commanVoltage = ithTrend->second.GetVoltage(rc);
          } else {
            if (commanVoltage != ithTrend->second.GetVoltage(rc))  isSameVoltage = false;
          }
        }
      }
    }
    TProfile* profs[30];

    double lineBottom  = (1.4+6);
    if (nruns < 6) lineBottom = (1.4+1);
    else if (nruns < 11) lineBottom = (1.4+2);
    else if (nruns < 16) lineBottom = (1.4+3);
    else if (nruns < 21) lineBottom = (1.4+4);
    else if (nruns < 26) lineBottom = (1.4+5);
    TLegend* legend = nullptr;
    
    for (int c = 0; c < nCol; c++){
      canvas2Panel->cd();
      int tempCellID = setupT->GetCellID(0,c, layer, mod);
      pads[c]->Draw();
      pads[c]->cd();
      pads[c]->SetLogy(0);
      ithTrend=trending.find(tempCellID);

      TString label           = Form("col %d", c);
      TString label2          = Form("Common V_{op} = %2.1f V", commanVoltage);
      if (c == 1){
        label = Form(" col %d layer %d", c, layer);
      }
      
      // Double_t yPosStart = topRCornerY[c]-1.2*relSizeP[c];
      Double_t yPosStart = topRCornerY[c];
      if(ithTrend==trending.end()){
        skipped++;
        std::cout << "WARNING: skipping cell ID: " << tempCellID  << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
        pads[c]->Clear();
        pads[c]->Draw();
        if (c == 0 ){
          TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
          TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
          TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
          DrawLatex(topRCornerX[c]+0.045, yPosStart-1*0.85*relSizeP[c], lab1, false, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[c]+0.045, yPosStart-2*0.85*relSizeP[c], lab2, false, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[c]+0.045, yPosStart-3*0.85*relSizeP[c], lab3, false, 0.85*textSizePixel, 43);
        }
        
        DrawLatex(topRCornerX[c]+0.045, yPosStart, label, false, 0.85*textSizePixel, 43);
        continue;
      } 

      if (c == 1 ){
        double startLegY  = yPosStart+1.2*relSizeP[c] -lineBottom*relSizeP[c];
        double endLegY    = yPosStart+1.2*relSizeP[c] -1.4*relSizeP[c];
        legend = GetAndSetLegend2(  topRCornerX[c]+0.045/2, startLegY, 0.7, endLegY,
                                    0.85*textSizePixel, 5, "",43,0.25);
      }
      
      TH1D* dummyhist;
      for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
        int tmpRunNr = ithTrend->second.GetRunNr(rc);
        profs[rc] = nullptr;
        if (tmpRunNr != -1) {
          profs[rc] = ithTrend->second.GetLGHGTriggRun(ithTrend->second.GetRunNr(rc));
          std::cout << profs[rc] << std::endl;
        }
        if (profs[rc]){
          if (rc == 0){
            dummyhist = new TH1D("dummyhist", "", profs[rc]->GetNbinsX(), profs[rc]->GetXaxis()->GetXmin(), profs[rc]->GetXaxis()->GetXmax());
            SetStyleHistoTH1ForGraphs( dummyhist, profs[rc]->GetXaxis()->GetTitle(), profs[rc]->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
            dummyhist->GetXaxis()->SetRangeUser(xPMin,xPMax);
            dummyhist->GetYaxis()->SetRangeUser(yPMin,yPMax);
            dummyhist->Draw("axis");
          }

          SetLineDefaults(profs[rc], GetColorLayer(rc), 2, GetLineStyleLayer(rc));   
          profs[rc]->SetMarkerStyle(24);
          profs[rc]->Draw("same,pe");
          if(c == 1) legend->AddEntry(profs[rc],Form("%d",tmpRunNr),"p");
        }
      }
      if (dummyhist) dummyhist->Draw("axis,same");                
      
      // labeling inside the panels & legend drawing 
      DrawLatex(topRCornerX[c]+0.045, yPosStart, label, false, 0.85*textSizePixel, 43);
      if (isSameVoltage && c == 1){
        DrawLatex(topRCornerX[c]+0.045, yPosStart+1.2*relSizeP[c]-(lineBottom+0.5)*relSizeP[c], label2, false, 0.85*textSizePixel, 43);
      }
      
      if (c == 1 ) legend->Draw();
      if (c == 0 ){
        TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
        TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
        TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
        DrawLatex(topRCornerX[c]+0.045, yPosStart-1*0.85*relSizeP[c], lab1, false, 0.85*textSizePixel, 43);
        DrawLatex(topRCornerX[c]+0.045, yPosStart-2*0.85*relSizeP[c], lab2, false, 0.85*textSizePixel, 43);
        DrawLatex(topRCornerX[c]+0.045, yPosStart-3*0.85*relSizeP[c], lab3, false, 0.85*textSizePixel, 43);
      }
    }
    
    if (skipped < 2){
      if(detailedPlot) canvas2Panel->SaveAs(nameOutput.Data());
      if (layer == 0) canvas2Panel->Print(Form("%s.pdf[",nameOutputSummary.Data()));
      canvas2Panel->Print(Form("%s.pdf",nameOutputSummary.Data()));
      if (layer == setupT->GetNMaxLayer()) canvas2Panel->Print(Form("%s.pdf]",nameOutputSummary.Data()));
    }
  }
  
#endif

