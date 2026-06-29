#ifndef PLOTHELPER_ASICLFHCAL_H
#define PLOTHELPER_ASICLFHCAL_H

  //*****************************************************************
  // ASIC geom sorted by LFHCal layer geom in addtion
  //===========================================================
    
  //__________________________________________________________________________________________________________
  // Plot Corr with Fits for Full Asic 2D
  //__________________________________________________________________________________________________________
  inline void PlotCorr2DAsicLFHCal (TCanvas* canvas, TPad** pads, 
                              Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                              std::map<int,TileSpectra> spectra, int option,
                              Double_t xMin, Double_t xMax, Double_t minY, Double_t maxY, int asic, TString nameOutput, RunInfo currRunInfo, bool noCalib = 0, int triggCh = -1 ){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileSpectra>::iterator ithSpectra;    
    
    int nChA  = setupT->GetAbsNMaxROChannel()+1;
    int skipped = 0;
    ReadOut::Type rotype = ReadOut::Type::Undef;
    
    bool isTriggCh = false;
    canvas->cd();

    if (triggCh != -1)
      isTriggCh = true;
    
    for (int ch = 0; ch < nChA; ch++){
      int tempCellID = setupT->GetCellID(asic, ch);
      if (tempCellID == -1 ) {
        skipped++;
        continue;    
      }
      int chInLayer  = setupT->GetChannelInLayer(tempCellID); 
      int layer      = setupT->GetLayer(tempCellID); 
      int row        = setupT->GetRow(tempCellID); 
      int col        = setupT->GetColumn(tempCellID); 
      int mod        = setupT->GetModule(tempCellID); 
        
      int cp         = layer%8*8+chInLayer;
      // std::cout << "cell ID: " << tempCellID << "\t row " << row << "\t column " << col << "\t layer " << layer << "\t module " << mod << "\t asic " << asic << "\tro ch asic " << ch << "\t " << cp<< std::endl;    
      canvas->cd();
      pads[cp]->Draw();
      // pads[cp]->Clear();
      pads[cp]->SetLogy(0);
      pads[cp]->SetLogz(1);
      pads[cp]->cd();
      ithSpectra=spectra.find(tempCellID);
      if(ithSpectra==spectra.end()){
        skipped++;
        std::cout << "WARNING: PlotCorr2DAsicLFHCal skipping cell ID: " << tempCellID << "\t row " << row << "\t column " << col << "\t layer " << layer << "\t module " << mod << "\t asic " << asic << "\tro ch asic " << ch << std::endl;
        pads[cp]->Clear();
        pads[cp]->Draw();
        if (cp ==63 ){
          DrawLatex(topRCornerX[cp]-0.04, topRCornerY[cp]-1*0.85*relSize8P[cp]-2.4*relSize8P[cp], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[cp], 42);
          DrawLatex(topRCornerX[cp]-0.04, topRCornerY[cp]-1*0.85*relSize8P[cp]-3.2*relSize8P[cp], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[cp], 42);
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
        tempProfile     = ithSpectra->second.GetTOAADC();
        temp2D          = ithSpectra->second.GetCorrTOAADC();                    
      } else if (option == 3){
        temp2D          = ithSpectra->second.GetCorrTOASample();
      } else if (option == 4){
        temp2D          = ithSpectra->second.GetCorrADCTOT();
        tempProfile     = ithSpectra->second.GetADCTOT();
      } else if (option == 5){
        tempProfile     = ithSpectra->second.GetTOTProfile();
      } else if (option == 6){
        tempProfile     = ithSpectra->second.GetTOAProfile();
      } else if (option == 11){
        tempProfile     = ithSpectra->second.GetWave1D();
        temp2D          = ithSpectra->second.GetCorr();          
      }
      TH1D* dummyhist = nullptr;
      if (!temp2D && (option < 4)) continue;
      if (temp2D){
        SetStyleHistoTH2ForGraphs( temp2D, temp2D->GetXaxis()->GetTitle(), temp2D->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
        temp2D->GetYaxis()->SetRangeUser(minY,maxY);
        temp2D->GetXaxis()->SetRangeUser(xMin,xMax);
        temp2D->Draw("col");

        if( !noCalib ){
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xMin, minY, xMax, maxY);
          temp2D->Draw("axis,same");
        }
        if (isTriggCh && triggCh == ch){
          std::cout << "entered highlighting" << std::endl;
          DrawHighlightTrigg(xMin, minY, xMax, maxY);
          temp2D->Draw("axis,same");
        }
      } else {
        if (!tempProfile) continue;
        gStyle->SetOptDate(0);   //show day and time
        gStyle->SetOptStat(0);  //show statistic
        
        dummyhist = new TH1D(Form("dummyhist_%d_%d_%d",option, asic,ch ), "", tempProfile->GetNbinsX(), tempProfile->GetXaxis()->GetXmin(), tempProfile->GetXaxis()->GetXmax());
        dummyhist->SetDirectory(0);
        SetStyleHistoTH1ForGraphs( dummyhist, tempProfile->GetXaxis()->GetTitle(), tempProfile->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
        dummyhist->GetXaxis()->SetRangeUser(xMin,xMax);
        dummyhist->GetYaxis()->SetRangeUser(minY,maxY);
        dummyhist->Draw("axis");
        if( !noCalib ){
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xMin, minY, xMax, maxY);
          dummyhist->Draw("axis,same");
        }                  
        // std::cout << "here" << std::endl;
        if (isTriggCh && triggCh == ch){
          std::cout << "entered highlighting" << std::endl;
          DrawHighlightTrigg(xMin, minY, xMax, maxY);
          dummyhist->Draw("axis,same");
        }
      }      
      if (tempProfile ){
        if (option == 2)
          SetMarkerDefaultsProfile(tempProfile, 24, 0.7, kBlue+1, kBlue+1);           
        else 
          SetMarkerDefaultsProfile(tempProfile, 24, 0.7, kRed+2, kRed+2);           
        tempProfile->Draw("pe, same");
      }
      TString label           = Form("r:%d c:%d, ro-ch:%d", row, col, ch);
      TString labelAsic       = "";
      if (cp%8 == 7)
        labelAsic = Form("layer:%d", layer);
      if (cp == 63){
        labelAsic = Form("layer:%d module:%d, asic:%d", layer, mod, asic);
      }
      if(option == 1) label = Form("%s, entries=%d",label.Data(),(Int_t)(temp2D->GetEntries()/11) ) ;
      TLatex *labelChannel    = new TLatex(topRCornerX[cp]-0.04,topRCornerY[cp]-1.2*relSize8P[cp],label);
      SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);
      TLatex *labelLayer;
      if (cp%8 == 7){
        labelLayer    = new TLatex(topRCornerX[cp]-0.04,topRCornerY[cp]-2.2*relSize8P[cp],labelAsic);
        SetStyleTLatex( labelLayer, 0.85*textSizePixel,4,1,43,kTRUE,31);
      }
      TF1* fit            = ithSpectra->second.GetCorrModel(0);
      if (rotype == ReadOut::Type::Hgcroc && option != 4)
        fit            = ithSpectra->second.GetCorrModel(2);
      int nlinesTot = 1;
      if(option == 2 || option == 3){ 
        if (ithSpectra->second.GetCalib()->HGLGCorrOff != -1000.){
          DrawLines(ithSpectra->second.GetCalib()->HGLGCorrOff, ithSpectra->second.GetCalib()->HGLGCorrOff,0, 0.7*maxY, 1, 1, 7);
        }
      }
      if (fit){
        Double_t rangeFit[2] = {0,0};
        fit->GetRange(rangeFit[0], rangeFit[1]);
        SetStyleFit(fit , rangeFit[0], rangeFit[1], 7, 3, kRed+3);
        fit->Draw("same");
        TLegend* legend = nullptr;
        if (rotype == ReadOut::Type::Caen){
          legend = GetAndSetLegend2( topRCornerX[cp]-0.4, topRCornerY[cp]-4*0.85*relSize8P[cp]-0.4*relSize8P[cp], topRCornerX[cp]+0.045, topRCornerY[cp]-0.6*relSize8P[cp],0.85*textSizePixel, 1, label+" "+labelAsic, 43,0.1);
          legend->AddEntry(fit, "linear fit, trigg.", "l");
          legend->AddEntry((TObject*)0, Form("#scale[0.8]{b = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");
          legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(1), fit->GetParError(1) ) , " ");
          nlinesTot = 4;
        } else {
          legend = GetAndSetLegend2( topRCornerX[cp]-0.4, topRCornerY[cp]-4*0.85*relSize8P[cp]-1.4*relSize8P[cp], topRCornerX[cp]+0.045, topRCornerY[cp]-0.6*relSize8P[cp],0.85*textSizePixel, 1, label+" "+labelAsic, 43,0.1);
          legend->AddEntry(fit, "const fit", "l");
          legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.1f #pm %2.1f}",fit->GetParameter(0), fit->GetParError(0) ) , " "); 
          nlinesTot = 3;
        }
        legend->Draw();
      } else {
        labelChannel->Draw();  
        if (cp%8 == 7)
          labelLayer->Draw();  
      }
      if (cp ==63 ){
        DrawLatex(topRCornerX[cp]-0.04, topRCornerY[cp]-nlinesTot*0.85*relSize8P[cp]-2.4*relSize8P[cp], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[cp], 42);
        DrawLatex(topRCornerX[cp]-0.04, topRCornerY[cp]-nlinesTot*0.85*relSize8P[cp]-3.2*relSize8P[cp], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[cp], 42);
      }
    }
    if (skipped < 64)
      canvas->SaveAs(nameOutput.Data());
  }
  
  //__________________________________________________________________________________________________________
  // Plot Corr with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotTrendingAsicLFHCal (TCanvas* canvas, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                              std::map<int,TileTrend> trending, int optionTrend, 
                              Double_t xMin, Double_t xMax, Double_t minY, Double_t maxY, bool isSameVoltage, double commanVoltage, 
                              int asic, TString nameOutput, TString nameOutputSummary, RunInfo currRunInfo, Int_t  detailedPlot = 1){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int skipped = 0;

    int nChA  = setupT->GetAbsNMaxROChannel()+1;
    std::cout << "Max channel nr. readout: "<<  nChA << std::endl;
    ReadOut::Type rotype = ReadOut::Type::Undef;
    
    bool isTriggCh = false;
    canvas->cd();
    
    // prep for log scale
    if (optionTrend == 6){ 
      if (minY ==0 ) minY = 1;
      else minY = minY/5.;
      maxY= maxY*5.;
    } else if (optionTrend == 17 || optionTrend == 18 ){ 
      minY = 1.1*minY;
      maxY = 2*maxY;      
    } else {
      minY = 0.9*minY;
      maxY = 1.1*maxY;      
    }
    
    for (int ch = 0; ch < nChA; ch++){
      int tempCellID = setupT->GetCellID(asic, ch);
      if (tempCellID == -1 ){
        skipped++;
        continue;    
      }
      int chInLayer  = setupT->GetChannelInLayer(tempCellID); 
      int layer      = setupT->GetLayer(tempCellID); 
      int row        = setupT->GetRow(tempCellID); 
      int col        = setupT->GetColumn(tempCellID); 
      int mod        = setupT->GetModule(tempCellID); 
        
      int cp         = layer%8*8+chInLayer;
      
      TString label           = Form("r:%d c:%d, ro-ch:%d", row, col, ch);
      std::cout << "cell ID:\t"<< tempCellID <<"\t panel nr:\t"<<  cp<< "\t"<< label.Data() << std::endl;
      TString labelAsic       = "";
      if (cp%8 == 7)
        labelAsic = Form("layer:%d", layer);
      if (cp == 63){
        labelAsic = Form("layer:%d module:%d, asic:%d", layer, mod, asic);
      }
      TString label2          = Form("Common V_{op} = %2.1f V", commanVoltage);
      
      canvas->cd();
      pads[cp]->Draw();
      
      if (optionTrend == 6){ 
        pads[cp]->SetLogy(1);
      } else {
        pads[cp]->SetLogy(0);          
      }
      
      canvas->cd();
      pads[cp]->cd();
      ithTrend=trending.find(tempCellID);
      if(ithTrend==trending.end()){
        skipped++;
        std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << row << "\t column " << col << "\t layer " << layer << "\t module " << mod<< std::endl;
        pads[cp]->Clear();
        pads[cp]->Draw();
        if (cp ==63 ){
          TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
          TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
          TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
          DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp]-1*0.85*relSize8P[cp], lab1, true, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp]-2*0.85*relSize8P[cp], lab2, true, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp]-3*0.85*relSize8P[cp], lab3, true, 0.85*textSizePixel, 43);
        }
        continue;
      } 
      TGraphErrors* tempGraph = nullptr;
      if (optionTrend == 0)       tempGraph = ithTrend->second.GetHGped();
      else if (optionTrend == 1)  tempGraph = ithTrend->second.GetLGped();
      else if (optionTrend == 2)  tempGraph = ithTrend->second.GetHGScale();            
      else if (optionTrend == 3)  tempGraph = ithTrend->second.GetLGScale();
      else if (optionTrend == 4)  tempGraph = ithTrend->second.GetLGHGcorr();
      else if (optionTrend == 5)  tempGraph = ithTrend->second.GetHGLGcorr();
      else if (optionTrend == 6)  tempGraph = ithTrend->second.GetTrigger();
      else if (optionTrend == 7)  tempGraph = ithTrend->second.GetSBSignal();
      else if (optionTrend == 8)  tempGraph = ithTrend->second.GetSBNoise();
      else if (optionTrend == 9)  tempGraph = ithTrend->second.GetHGLMPV();
      else if (optionTrend == 10) tempGraph = ithTrend->second.GetLGLMPV();
      else if (optionTrend == 11) tempGraph = ithTrend->second.GetHGLSigma();
      else if (optionTrend == 12) tempGraph = ithTrend->second.GetLGLSigma();
      else if (optionTrend == 13) tempGraph = ithTrend->second.GetHGGSigma();
      else if (optionTrend == 14) tempGraph = ithTrend->second.GetLGGSigma();
      else if (optionTrend == 15) tempGraph = ithTrend->second.GetHGpedwidth();
      else if (optionTrend == 16) tempGraph = ithTrend->second.GetLGpedwidth();
      else if (optionTrend == 17) tempGraph = ithTrend->second.GetLGHGOff();
      else if (optionTrend == 18) tempGraph = ithTrend->second.GetHGLGOff();
      // special mode for injections
      else if (optionTrend == 30) tempGraph = ithTrend->second.GetADCmax();
      else if (optionTrend == 31) tempGraph = ithTrend->second.GetADCsat();
      else if (optionTrend == 32) tempGraph = ithTrend->second.GetTOT();
      else if (optionTrend == 33) tempGraph = ithTrend->second.GetTOTsat();
      else if (optionTrend == 34) tempGraph = ithTrend->second.GetTOA();
      else if (optionTrend == 35) tempGraph = ithTrend->second.GetNSampTOA();
      else if (optionTrend == 36) tempGraph = ithTrend->second.GetNTOA();

      if (!tempGraph) continue;
      TH1D* dummyhist = new TH1D(Form("dummyhist_%d_%d_%d",optionTrend,asic,ch), "", 100, xMin, xMax);
      dummyhist->SetDirectory(0);
      SetStyleHistoTH1ForGraphs( dummyhist, tempGraph->GetXaxis()->GetTitle(), tempGraph->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
      // if (optionTrend == 6)std::cout << "\t" << tempGraph->GetXaxis()->GetTitle() << "\t" << tempGraph->GetYaxis()->GetTitle() << std::endl;
      SetMarkerDefaultsTGraphErr(tempGraph, 20, 1, kBlue+1, kBlue+1);   
      dummyhist->GetYaxis()->SetRangeUser(minY,maxY);
      dummyhist->Draw("axis");
      tempGraph->Draw("pe, same");
              
      DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp], label, true, 0.85*textSizePixel, 43);
      if (isSameVoltage && cp == 7){
        DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp]-1*0.85*relSize8P[cp], label2, true, 0.85*textSizePixel, 43);
      }

      TLatex *labelChannel    = new TLatex(topRCornerX[cp]-0.04,topRCornerY[cp]-1.2*relSize8P[cp],label);
      SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);
      TLatex *labelLayer;
      if (cp%8 == 7){
        labelLayer    = new TLatex(topRCornerX[cp]-0.04,topRCornerY[cp]-2.2*relSize8P[cp],labelAsic);
        SetStyleTLatex( labelLayer, 0.85*textSizePixel,4,1,43,kTRUE,31);
      }
      labelChannel->Draw();  
      if (cp%8 == 7)
        labelLayer->Draw();  

      if (cp == 63 ){
        TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
        TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
        TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
        DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp]-1*0.85*relSize8P[cp], lab1, true, 0.85*textSizePixel, 43);
        DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp]-2*0.85*relSize8P[cp], lab2, true, 0.85*textSizePixel, 43);
        DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp]-3*0.85*relSize8P[cp], lab3, true, 0.85*textSizePixel, 43);
      }
    }

    if (skipped < 64){
      if(detailedPlot){
        std::cout << "Saving as: " << nameOutput.Data()<< std::endl;
        canvas->SaveAs(nameOutput.Data());
      }
      std::cout << "Appending to: " << nameOutputSummary.Data()<< std::endl;
      if (asic == 0) canvas->Print(Form("%s.pdf[",nameOutputSummary.Data()));
      canvas->Print(Form("%s.pdf",nameOutputSummary.Data()));
      if (asic == setupT->GetNMaxROUnit()) canvas->Print(Form("%s.pdf]",nameOutputSummary.Data()));
    }
  }
  
  //__________________________________________________________________________________________________________
  // Plot Run overlay for all 16 tiles for all runs available
  //__________________________________________________________________________________________________________
  inline void PlotRunOverlayProfileAsicLFHCal ( TCanvas* canvas, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                                std::map<int,TileTrend> trending, int nruns, int option,
                                                Double_t xMin, Double_t xMax, Double_t yPMin, Double_t yPMax,  int asic,
                                                TString nameOutput, TString nameOutputSummary, 
                                                RunInfo currRunInfo, Int_t detailedPlot = 1, bool scaleInt = false ){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nChA  = setupT->GetAbsNMaxROChannel()+1;
    int skipped = 0;
    
    Int_t nSameSettings = 0;
    if (currRunInfo.rf > -1) nSameSettings++;
    if (currRunInfo.cf > -1) nSameSettings++;
    if (currRunInfo.cfcomp > -1) nSameSettings++;
    if (currRunInfo.cc > -1) nSameSettings++;
    if (currRunInfo.vop > -1) nSameSettings++;
    
    TProfile* profs[30];

    double lineBottom  = (1.4+5);
    if (nruns < 6) lineBottom = (1.4);
    else if (nruns < 11) lineBottom = (1.4+1);
    else if (nruns < 16) lineBottom = (1.4+2);
    else if (nruns < 21) lineBottom = (1.4+3);
    else if (nruns < 26) lineBottom = (1.4+4);
    if (nSameSettings == 4) lineBottom++;
    TLegend* legend = nullptr;
    
    for (int ch = 0; ch < nChA; ch++){
      int tempCellID = setupT->GetCellID(asic, ch);
      if (tempCellID == -1 ){
        skipped++;
        continue;    
      }
      int chInLayer  = setupT->GetChannelInLayer(tempCellID); 
      int layer      = setupT->GetLayer(tempCellID); 
      int row        = setupT->GetRow(tempCellID); 
      int col        = setupT->GetColumn(tempCellID); 
      int mod        = setupT->GetModule(tempCellID); 
        
      int cp         = layer%8*8+chInLayer;
    
      TString label           = Form("r:%d c:%d, ro-ch:%d", row, col, ch);
      TString labelAsic       = "";
      if (cp%8 == 7)
        labelAsic = Form("layer:%d", layer);
      if (cp == 63){
        labelAsic = Form("layer:%d module:%d, asic:%d", layer, mod, asic);
      }
      TString label2          = Form("V_{op}=%2.1fV", currRunInfo.vop);
      TString label3          = GetLabelHGCROCSettings(currRunInfo);

      canvas->cd();
      pads[cp]->Draw();
    
      pads[cp]->cd();
      ithTrend=trending.find(tempCellID);
      if(ithTrend==trending.end()){
        skipped++;
        std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << row << "\t column " << col << "\t layer " << layer << "\t module " << mod<< std::endl;
        pads[cp]->Clear();
        pads[cp]->Draw();
        if (cp ==55 ){
          TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
          TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
          TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
          DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp]-1*0.85*relSize8P[cp], lab1, false, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp]-2*0.85*relSize8P[cp], lab2, false, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[cp]-0.045, topRCornerY[cp]-1.2*relSize8P[cp]-3*0.85*relSize8P[cp], lab3, false, 0.85*textSizePixel, 43);
        }
        if ( nSameSettings > 0  && cp == 56){
          DrawLatex(0.04, topRCornerY[cp]-1.2*relSize8P[cp]*relSize8P[cp], label2, false, 0.85*textSizePixel, 43);
          DrawLatex(0.04, topRCornerY[cp]-1.2*relSize8P[cp]*relSize8P[cp]-0.85*relSize8P[cp], label3, false, 0.85*textSizePixel, 43);
        }

        continue;
      }    


      if (cp == 63 ){
        double startLegY  = topRCornerY[cp]-lineBottom*relSize8P[cp];
        double endLegY    = topRCornerY[cp]-1.4*relSize8P[cp];
        TString header    = "";
        double width      = 0.5;
        if (nSameSettings == 4){
            width = 0.9;
            std::cout <<  currRunInfo.rf << "\t" << currRunInfo.cf << "\t" << currRunInfo.cfcomp << "\t" << currRunInfo.cc << "\t" << currRunInfo.vop << std::endl;
            if (currRunInfo.rf < 0)
              header = "RF (k#Omega)";
            if (currRunInfo.cf < 0) header = "CF (fF)";
            if (currRunInfo.cfcomp < 0) header = "CF_{comp} (fF)";
            if (currRunInfo.cc < 0)  header = "CC";
            if (currRunInfo.vop < 0)  header = "V_{op} (V)";
        }
        legend = GetAndSetLegend2(  0.045, startLegY, width, endLegY,
                                    0.85*textSizePixel, 5, "",43,0.25);
      }
          
      TH1D* dummyhist;
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
          if (scaleInt && profs[rc] != nullptr) profs[rc]->Scale(1/profs[rc]->Integral());
        }
        if (profs[rc]){
          if (rc == 0){
            TString yTitle = profs[rc]->GetYaxis()->GetTitle();
            if (scaleInt && profs[rc] != nullptr) yTitle = Form("%s/ integral", yTitle.Data());
            dummyhist = new TH1D(Form("dummyhist_%d_%d_%d",option, asic,ch), "", profs[rc]->GetNbinsX(), profs[rc]->GetXaxis()->GetXmin(), profs[rc]->GetXaxis()->GetXmax());
            dummyhist->SetDirectory(0);
            SetStyleHistoTH1ForGraphs( dummyhist, profs[rc]->GetXaxis()->GetTitle(), profs[rc]->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
            dummyhist->GetXaxis()->SetRangeUser(xMin,xMax);
            dummyhist->GetYaxis()->SetRangeUser(yPMin,yPMax);
            dummyhist->Draw("axis");
          }

          SetLineDefaults(profs[rc], GetColorLayer(rc), 2, GetLineStyleLayer(rc));   
          profs[rc]->SetMarkerStyle(GetMarkerLayer(rc));
          profs[rc]->Draw("same,pe");
          if(cp == 63){
            TString labelLegend = Form("%d",tmpRunNr);
            if (nSameSettings == 4){
              if (currRunInfo.vop < 0) labelLegend = Form("%.1f",(double)ithTrend->second.GetVoltage(rc));
              if (currRunInfo.rf < 0) labelLegend = Form("%.1f",ReturnRFValue(ithTrend->second.GetRF(rc)));
              if (currRunInfo.cf < 0) labelLegend = Form("%.0f",ReturnCFValue(ithTrend->second.GetCF(rc)));
              if (currRunInfo.cfcomp < 0) labelLegend = Form("%.0f",ReturnCFCompValue(ithTrend->second.GetCFComp(rc)));
              if (currRunInfo.cc < 0)  labelLegend = Form("%.0f",ReturnCCValue(ithTrend->second.GetCC(rc)));
            }
            legend->AddEntry(profs[rc],labelLegend.Data(),"p");
          }
        }
      }
      if (dummyhist) dummyhist->Draw("axis,same");                
          
      // labeling inside the panels & legend drawing 
      DrawLatex(topRCornerX[cp]+0.045, topRCornerY[cp]-1.2*relSize8P[cp], label, false, 0.85*textSizePixel, 43);
      if ( nSameSettings > 0  && cp == 56){
        DrawLatex(0.04, topRCornerY[cp]-1.2*relSize8P[cp]*relSize8P[cp], label2, false, 0.85*textSizePixel, 43);
        DrawLatex(0.04, topRCornerY[cp]-1.2*relSize8P[cp]*relSize8P[cp]-0.85*relSize8P[cp], label3, false, 0.85*textSizePixel, 43);
      }
      TLatex *labelChannel    = new TLatex(topRCornerX[cp]-0.04,topRCornerY[cp]-1.2*relSize8P[cp],label);
      SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);
      TLatex *labelLayer;
      if (cp%8 == 7){
        labelLayer    = new TLatex(topRCornerX[cp]-0.04,topRCornerY[cp]-2.2*relSize8P[cp],labelAsic);
        SetStyleTLatex( labelLayer, 0.85*textSizePixel,4,1,43,kTRUE,31);
      }
      labelChannel->Draw();  
      if (cp%8 == 7)
        labelLayer->Draw();  
          
      if (cp == 63) legend->Draw();
      if (cp == 55){
        TString lab1 = Form("#it{#bf{%s TB:}} %s", (currRunInfo.detector).Data(), GetStringFromRunInfo(currRunInfo, 9).Data());
        TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
        TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
        DrawLatex(topRCornerX[cp]+0.045, topRCornerY[cp]-1.2*relSize8P[cp]-1*0.85*relSize8P[cp], lab1, false, 0.85*textSizePixel, 43);
        DrawLatex(topRCornerX[cp]+0.045, topRCornerY[cp]-1.2*relSize8P[cp]-2*0.85*relSize8P[cp], lab2, false, 0.85*textSizePixel, 43);
        DrawLatex(topRCornerX[cp]+0.045, topRCornerY[cp]-1.2*relSize8P[cp]-3*0.85*relSize8P[cp], lab3, false, 0.85*textSizePixel, 43);
      }
    }
    if (skipped < 64){
      if(detailedPlot) canvas->SaveAs(nameOutput.Data());
      if (asic == 0) canvas->Print(Form("%s.pdf[",nameOutputSummary.Data()));
      canvas->Print(Form("%s.pdf",nameOutputSummary.Data()));
      if (asic == setupT->GetNMaxROUnit()) canvas->Print(Form("%s.pdf]",nameOutputSummary.Data()));
    }
  }
  
    
#endif

