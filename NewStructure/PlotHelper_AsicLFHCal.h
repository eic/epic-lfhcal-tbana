#ifndef PLOTTHELPER_ASICLFHCAL_H
#define PLOTTHELPER_ASICLFHCAL_H

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
      if (tempCellID == -1 ) continue;    
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
      }
      if (!temp2D && option != 4) continue;
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
        TH1D* dummyhist = new TH1D("dummyhist", "", tempProfile->GetNbinsX(), tempProfile->GetXaxis()->GetXmin(), tempProfile->GetXaxis()->GetXmax());
        SetStyleHistoTH1ForGraphs( dummyhist, tempProfile->GetXaxis()->GetTitle(), tempProfile->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
        dummyhist->GetXaxis()->SetRangeUser(xMin,xMax);
        dummyhist->GetYaxis()->SetRangeUser(minY,maxY);
        dummyhist->Draw("axis");
        if( !noCalib ){
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xMin, minY, xMax, maxY);
          dummyhist->Draw("axis,same");
        }                              
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
    
#endif

