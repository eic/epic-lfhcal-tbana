#ifndef PLOTHELPER_2MODLAYER_H
#define PLOTHELPER_2MODLAYER_H

    //*****************************************************************
    // Test beam geometry (beam coming from viewer)
    //===========================================================
    //||    8 (12)   ||    7 (13)  ||    6 (14)  ||    5 (15)   ||  row 0
    //===========================================================           mod 1
    //||    1 (8)    ||    2 (9)   ||    3 (10)  ||    4 (11)  ||  row 1
    //===========================================================
    //||    8 (4)    ||    7 (5)   ||    6 (6)   ||    5 (7)   ||  row 0
    //===========================================================           mod 0
    //||    1 (0)    ||    2 (1)   ||    3 (2)   ||    4 (3)   ||  row 1
    //    col 0     col 1       col 2     col  3
    // rebuild pad geom in similar way (numbering -1)
    //*****************************************************************
    

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // dedicated class for all 2Mod layer plotting functions
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  //__________________________________________________________________________________________________________
  // Plot Trigger Primitive with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotTriggerPrim2ModLayer (TCanvas* canvas, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, 
                                         Double_t* relSize8P, Int_t textSizePixel, 
                                         std::map<int,TileSpectra> spectra, 
                                         double avMip, double facLow, double facHigh,
                                         Double_t xMin, Double_t xMax, Double_t scaleYMax, 
                                         int layer, TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    Setup* setupT = Setup::GetInstance();
    std::map<int, TileSpectra>::iterator ithSpectra;
    std::map<int, TileSpectra>::iterator ithSpectraTrigg;
    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            continue;
          } 
          TH1D* tempHist = ithSpectra->second.GetTriggPrim();
          if (maxY < FindLargestBin1DHist(tempHist, xMin , xMax)) maxY = FindLargestBin1DHist(tempHist, xMin , xMax);
        }
      }  
    }

    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p = setupT->GetChannelInLayerFull(tempCellID);
          pads[p]->Draw();
          pads[p]->cd();
          pads[p]->SetLogy();
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            skipped++;
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p ==15 ){
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-2.*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-3.*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4.*relSize8P[p], "Trigger primitives", true, 0.85*relSize8P[p], 42);
            }
            continue;
          } 
          TH1D* tempHist = ithSpectra->second.GetTriggPrim();
          SetStyleHistoTH1ForGraphs( tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.1, 510, 510, 43, 63);  
          SetMarkerDefaults(tempHist, 20, 1, kBlue+1, kBlue+1, kFALSE);   
          tempHist->GetXaxis()->SetRangeUser(xMin,xMax);
          tempHist->GetYaxis()->SetRangeUser(0.7,scaleYMax*maxY);
          
          tempHist->Draw("pe");
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xMin, 0, xMax, maxY);
          tempHist->Draw("same,axis");
          tempHist->Draw("same,pe");

          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          if (p == 15){
            label = Form("r:%d c:%d m:%d layer %d", r, c, m, layer);
          }
          TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.045,topRCornerY[p]-1.2*relSize8P[p],label);
          SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);
          labelChannel->Draw();  
                  
          TBox* triggArea =  CreateBox(kBlue-8, avMip*facLow, 0.7, avMip*facHigh,scaleYMax*maxY, 1001 );
          triggArea->Draw();
          DrawLines(avMip*facLow, avMip*facLow,0.7, scaleYMax*maxY, 1, 1, 7);
          DrawLines(avMip*facHigh, avMip*facHigh,0.7, scaleYMax*maxY, 1, 1, 7);
          tempHist->Draw("same,axis");
          tempHist->Draw("same,pe");
          
          if (p == 15 ){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-2.*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-3.*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4.*relSize8P[p], "Trigger primitives", true, 0.85*relSize8P[p], 42);
          }
        }
      }
    }
    if (skipped < 16)
      canvas->SaveAs(nameOutput.Data());
  }
    
  //__________________________________________________________________________________________________________
  // Plot Noise with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotNoiseWithFits2ModLayer (TCanvas* canvas, TPad** pads, 
                                 Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                 std::map<int,TileSpectra> spectra, int option, 
                                 Double_t xMin, Double_t xMax, Double_t scaleYMax, int layer, TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    Setup* setupT = Setup::GetInstance();
    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m<< std::endl;
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
          if (maxY < FindLargestBin1DHist(tempHist, xMin , xMax)) maxY = FindLargestBin1DHist(tempHist, xMin , xMax);
        }
      }  
    }
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p = setupT->GetChannelInLayerFull(tempCellID);
          pads[p]->Draw();
          pads[p]->cd();
          pads[p]->SetLogy();
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p ==15 ){
              DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
              DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
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
          tempHist->GetXaxis()->SetRangeUser(xMin,xMax);
          tempHist->GetYaxis()->SetRangeUser(0.7,scaleYMax*maxY);
          
          tempHist->Draw("pe");
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xMin, 0, xMax, maxY);
          tempHist->Draw("same,axis");
          tempHist->Draw("same,pe");
          
          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          if (p == 15){
            label = Form("r:%d c:%d m:%d layer:%d", r, c, m, layer);
          }
          TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.04,topRCornerY[p]-1.2*relSize8P[p],label);
          SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);

          
          TF1* fit = nullptr;
          if (option == 0){
            fit = ithSpectra->second.GetBackModel(1);
          } else  if (option ==1){
            fit = ithSpectra->second.GetBackModel(0);  
          }
          if (fit){
            SetStyleFit(fit , xMin, xMax, 7, 7, kBlack);
            fit->Draw("same");
            TLegend* legend = GetAndSetLegend2( topRCornerX[p]-0.48, topRCornerY[p]-4*0.85*relSize8P[p]-0.4*relSize8P[p], topRCornerX[p]-0.08, topRCornerY[p]-0.6*relSize8P[p],0.85*textSizePixel, 1, label, 43,0.2);
            legend->AddEntry(fit, "Gauss noise fit", "l");
            legend->AddEntry((TObject*)0, Form("#mu = %2.2f #pm %2.2f",fit->GetParameter(1), fit->GetParError(1) ) , " ");
            legend->AddEntry((TObject*)0, Form("#sigma = %2.2f #pm %2.2f",fit->GetParameter(2), fit->GetParError(2) ) , " ");
            legend->Draw();
              
          } else {
            labelChannel->Draw();  
          }
        
          if (xMin < -5) DrawLines(0, 0,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
        
          if (p ==15 ){
            DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          }
        }
      }
    }
    if (skipped < 15)
      canvas->SaveAs(nameOutput.Data());
  }
  
  
  //__________________________________________________________________________________________________________
  // Plot Noise extracted from collision data
  //__________________________________________________________________________________________________________
  inline void PlotNoiseAdvWithFits2ModLayer (TCanvas* canvas, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                      std::map<int,TileSpectra> spectra, std::map<int,TileSpectra> spectraTrigg, bool opt, 
                                      Double_t xMin, Double_t xMax, Double_t scaleYMax, int layer, TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    std::map<int, TileSpectra>::iterator ithSpectraTrigg;
    
    Setup* setupT = Setup::GetInstance();
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            std::cout << "WARNING: PlotNoiseAdvWithFits2ModLayer skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            continue;
          } 
          TH1D* tempHist = nullptr;
          if (opt == 1){ // HG
            tempHist = ithSpectra->second.GetHG();
          } else {
            tempHist = ithSpectra->second.GetLG();
          }
          if (maxY < FindLargestBin1DHist(tempHist, xMin , xMax)) maxY = FindLargestBin1DHist(tempHist, xMin , xMax);
        }
      }  
    }
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p = setupT->GetChannelInLayerFull(tempCellID);
          pads[p]->Draw();
          pads[p]->cd();
          pads[p]->SetLogy();
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            skipped++;
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p == 15 ){
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
            }
            continue;
          } 
          ithSpectraTrigg=spectraTrigg.find(tempCellID);
          TH1D* tempHist = nullptr;
          if (opt == 1){ // HG
              tempHist = ithSpectra->second.GetHG();
          } else {
              tempHist = ithSpectra->second.GetLG();
          }
          if (!tempHist) continue;
          SetStyleHistoTH1ForGraphs( tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.1, 510, 510, 43, 63);  
          SetMarkerDefaults(tempHist, 20, 1, kBlue+1, kBlue+1, kFALSE);   
          tempHist->GetXaxis()->SetRangeUser(xMin,xMax);
          tempHist->GetYaxis()->SetRangeUser(0.7,scaleYMax*maxY);
          
          tempHist->Draw("pe");
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xMin, 0, xMax, maxY);
          tempHist->Draw("same,axis");
          tempHist->Draw("same,pe");
          
          TH1D* tempHistT = nullptr;
          if (opt == 1){ // HG
              tempHistT = ithSpectraTrigg->second.GetHG();
          } else {
              tempHistT = ithSpectraTrigg->second.GetLG();
          }
          if (tempHistT){
            SetMarkerDefaults(tempHistT, 24, 1, kRed+1, kRed+1, kFALSE);   
            tempHistT->Draw("same,pe");
          }
          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          if (p == 7){
            label = Form("r:%d c:%d m:%d layer %d", r, c, m, layer);
          }
          TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.045,topRCornerY[p]-1.2*relSize8P[p],label);
          SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);

          
          TF1* fit            = nullptr;
          bool isTrigFit      = false;
          if (opt == 1){ // HG
            fit = ithSpectraTrigg->second.GetBackModel(1);
            if (!fit){
                fit = ithSpectra->second.GetBackModel(1);
                
            } else {
                isTrigFit = true;
            }
          } else {
            fit = ithSpectraTrigg->second.GetBackModel(0);
            if (!fit){
                fit = ithSpectra->second.GetBackModel(0);
            } else {
                isTrigFit = true;
            }  
          }
          if (fit){
            if (isTrigFit)
              SetStyleFit(fit , 0, 2000, 7, 3, kRed+3);
            else 
              SetStyleFit(fit , 0, 2000, 7, 7, kBlue+3);  
            fit->Draw("same");
            TLegend* legend = GetAndSetLegend2( topRCornerX[p]-10*relSize8P[p], topRCornerY[p]-4*0.85*relSize8P[p]-0.4*relSize8P[p], topRCornerX[p]-0.04, topRCornerY[p]-0.6*relSize8P[p],0.85*textSizePixel, 1, label, 43,0.1);
            if (isTrigFit)
              legend->AddEntry(fit, "Gauss noise fit, trigg.", "l");
            else 
              legend->AddEntry(fit, "Gauss noise fit", "l");  
            legend->AddEntry((TObject*)0, Form("#mu = %2.2f #pm %2.2f",fit->GetParameter(1), fit->GetParError(1) ) , " ");
            legend->AddEntry((TObject*)0, Form("#sigma = %2.2f #pm %2.2f",fit->GetParameter(2), fit->GetParError(2) ) , " ");
            legend->Draw();
          } else {
            labelChannel->Draw();  
          }
        
          DrawLines(0, 0,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
          if (p == 15 ){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          }
        
        }
      }
    }
    if (skipped < 16)
      canvas->SaveAs(nameOutput.Data());
  }  

  
  //__________________________________________________________________________________________________________
  // Plot Noise extracted from collision data
  //__________________________________________________________________________________________________________
  inline void Plot3SpectraOverlay2ModLayer (TCanvas* canvas, TPad** pads, 
                                            Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                            std::map<int,TileSpectra> spectra,
                                            std::map<int,TileSpectra> spectraTrigg,
                                            std::map<int,TileSpectra> spectraNoise,
                                            bool opt, Double_t xMin, Double_t xMax, Double_t scaleYMax, 
                                            int layer, TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    std::map<int, TileSpectra>::iterator ithSpectraNoise;
    std::map<int, TileSpectra>::iterator ithSpectraTrigg;
    
    Setup* setupT = Setup::GetInstance();
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            std::cout << "WARNING: Plot3SpectraOverlay2ModLayer skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            continue;
          } 
          TH1D* tempHist = nullptr;
          if (opt == 1){ // HG
            tempHist = ithSpectra->second.GetHG();
          } else {
            tempHist = ithSpectra->second.GetLG();
          }
          if (maxY < FindLargestBin1DHist(tempHist, xMin , xMax)) maxY = FindLargestBin1DHist(tempHist, xMin , xMax);
        }
      }  
    }
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p = setupT->GetChannelInLayerFull(tempCellID);
          pads[p]->Draw();
          pads[p]->cd();
          pads[p]->SetLogy();
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            skipped++;
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p == 15 ){
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
            }
            continue;
          } 
          ithSpectraTrigg=spectraTrigg.find(tempCellID);
          ithSpectraNoise=spectraNoise.find(tempCellID);
          TH1D* tempHist = nullptr;
          if (opt == 1){ // HG
              tempHist = ithSpectra->second.GetHG();
          } else {
              tempHist = ithSpectra->second.GetLG();
          }
          if (!tempHist) continue;
          SetStyleHistoTH1ForGraphs( tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.1, 510, 510, 43, 63);  
          SetMarkerDefaults(tempHist, 24, 1, kGray+1, kGray+1, kFALSE);   
          tempHist->GetXaxis()->SetRangeUser(xMin,xMax);
          tempHist->GetYaxis()->SetRangeUser(0.7,scaleYMax*maxY);
          
          tempHist->Draw("pe");
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xMin, 0, xMax, maxY);
          tempHist->Draw("same,axis");
          tempHist->Draw("same,pe");
          
          TH1D* tempHistT = nullptr;
          if (opt == 1){ // HG
              tempHistT = ithSpectraTrigg->second.GetHG();
          } else {
              tempHistT = ithSpectraTrigg->second.GetLG();
          }
          if (tempHistT){
            SetMarkerDefaults(tempHistT, 20, 1, kRed+1, kRed+1, kFALSE);   
            tempHistT->Draw("same,pe");
          }
          TH1D* tempHistN = nullptr;
          if (opt == 1){ // HG
              tempHistN = ithSpectraNoise->second.GetHG();
          } else {
              tempHistN = ithSpectraNoise->second.GetLG();
          }
          if (tempHistN){
            SetMarkerDefaults(tempHistN, 24, 1, kBlue+1, kBlue+1, kFALSE);   
            tempHistN->Draw("same,pe");
          }
          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          if (p == 7){
            label = Form("r:%d c:%d m:%d layer %d", r, c, m, layer);
          }
          TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.045,topRCornerY[p]-1.2*relSize8P[p],label);
          SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);
          labelChannel->Draw();  
        
          DrawLines(0, 0,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
          if (p == 15 ){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          }
        }
      }
    }
    if (skipped < 16)
      canvas->SaveAs(nameOutput.Data());
  }  
  
  //__________________________________________________________________________________________________________
  // Plot Mip with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotMipWithFits2ModLayer (TCanvas* canvas, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, std::map<int,TileSpectra> spectraTrigg, int opt, 
                                  Double_t xMin, Double_t xMax, Double_t scaleYMax, int layer, TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    std::map<int, TileSpectra>::iterator ithSpectraTrigg;
    Setup* setupT = Setup::GetInstance();
    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            continue;
          } 
          TH1D* tempHist = nullptr;
          if (opt == 1){
            tempHist = ithSpectra->second.GetHG();
          } else {
            tempHist = ithSpectra->second.GetLG();
          }
          if (maxY < FindLargestBin1DHist(tempHist, xMin , xMax)) maxY = FindLargestBin1DHist(tempHist, xMin , xMax);
        }
      }  
    }
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p = setupT->GetChannelInLayerFull(tempCellID);
          pads[p]->Draw();
          pads[p]->cd();
          pads[p]->SetLogy();
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            skipped++;
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p ==15 ){
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
            }          
            continue;
          } 
          ithSpectraTrigg=spectraTrigg.find(tempCellID);
          TH1D* tempHist = nullptr;
          double noiseWidth = 0;
          if (opt == 1){
            tempHist = ithSpectra->second.GetHG();
            noiseWidth = ithSpectra->second.GetCalib()->PedestalSigH;
          } else {
            tempHist = ithSpectra->second.GetLG();
            noiseWidth = ithSpectra->second.GetCalib()->PedestalSigL;
          }
          SetStyleHistoTH1ForGraphs( tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.1, 510, 510, 43, 63);  
          SetMarkerDefaults(tempHist, 20, 1, kBlue+1, kBlue+1, kFALSE);   
          tempHist->GetXaxis()->SetRangeUser(xMin,xMax);
          tempHist->GetYaxis()->SetRangeUser(0.7,scaleYMax*maxY);
          
          tempHist->Draw("pe");
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xMin, 0, xMax, maxY);
          tempHist->Draw("same,axis");
          tempHist->Draw("same,pe");
          
          TH1D* tempHistT = nullptr;
          
          if (opt == 1){
            tempHistT = ithSpectraTrigg->second.GetHG();
          } else {
            tempHistT = ithSpectraTrigg->second.GetLG();
          }
          SetMarkerDefaults(tempHistT, 24, 1, kRed+1, kRed+1, kFALSE);   
          tempHistT->Draw("same,pe");
          
          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          if (p == 15){
            label = Form("r:%d c:%d m:%d layer %d", r, c, m, layer);
          }
          TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.045,topRCornerY[p]-1.2*relSize8P[p],label);
          SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);

          
          TF1* fit            = nullptr;
          bool isTrigFit      = false;
          double maxFit       = 0;
          double SNR     			= 0; // signal to noise ratio -EP	
          double maxBin       = 0; 
          if (ithSpectra->second.GetROType() == ReadOut::Type::Caen) { // from Analyses.cc -EP
            maxBin = 3800;
          } else {
            maxBin = 1024;
          }
          if (opt == 1){
            Int_t binNLow = ithSpectra->second.GetHG()->FindBin(-1*noiseWidth); // for SNR -EP
            Int_t binNHigh = ithSpectra->second.GetHG()->FindBin(3*noiseWidth); 
            Int_t binSHigh = ithSpectra->second.GetHG()->FindBin(maxBin); 
            fit = ithSpectraTrigg->second.GetSignalModel(1);
            if (!fit){
                fit = ithSpectra->second.GetSignalModel(1);
                if (fit){
                  maxFit = ithSpectra->second.GetCalib()->ScaleH;
                  double noiseInt = ithSpectra->second.GetHG()->Integral(binNLow, binNHigh); 
                  double sigInt = ithSpectra->second.GetHG()->Integral(binNHigh, binSHigh);
                  SNR = (noiseInt != 0) ? sigInt/noiseInt : 0; // if noise = 0, SNR = 0 -EP
                }
            } else {
                isTrigFit = true;
                maxFit = ithSpectraTrigg->second.GetCalib()->ScaleH;
                double noiseInt = ithSpectraTrigg->second.GetHG()->Integral(binNLow, binNHigh); 
                double sigInt = ithSpectraTrigg->second.GetHG()->Integral(binNHigh, binSHigh);
                SNR = (noiseInt != 0) ? sigInt/noiseInt : 0; // if noise = 0, SNR = 0 -EP
            }
          } // end if (opt == 1) 
          else {
            fit = ithSpectraTrigg->second.GetSignalModel(0);
            if (!fit){
                fit = ithSpectra->second.GetSignalModel(0);
                if (fit){
                  maxFit = ithSpectra->second.GetCalib()->ScaleL;
                }
            } else {
                isTrigFit = true;
                maxFit = ithSpectraTrigg->second.GetCalib()->ScaleL;
            }  
          }
          if (fit){
            if (isTrigFit)
              SetStyleFit(fit , 0, 2000, 7, 3, kRed+3);
            else 
              SetStyleFit(fit , 0, 2000, 7, 7, kBlue+3);  
            fit->Draw("same");
            TLegend* legend = GetAndSetLegend2( topRCornerX[p]-10*relSize8P[p], topRCornerY[p]-6*0.85*relSize8P[p]-0.4*relSize8P[p], topRCornerX[p]-0.04, topRCornerY[p]-0.6*relSize8P[p],0.85*textSizePixel, 1, label, 43,0.1);
            if (opt == 1)
              legend->AddEntry((TObject*)0, Form("Total events: %i", (int)ithSpectra->second.GetHG()->Integral()), "");
            if (isTrigFit)
              legend->AddEntry(fit, "Landau-Gauss fit, trigg.", "l");
            else 
              legend->AddEntry(fit, "Landau-Gauss fit", "l");  
            // estimate uncertainty on max position
            double rel_err_L_MPV = fit->GetParError(1)/fit->GetParameter(1); // relative uncertainty on MPV
            double sigma_Max = rel_err_L_MPV * maxFit;
            legend->AddEntry((TObject*)0, Form("#scale[0.8]{L MPV = %2.2f #pm %2.2f}",fit->GetParameter(1), fit->GetParError(1) ) , " ");
            legend->AddEntry((TObject*)0, Form("#scale[0.8]{Max = %2.2f #pm %2.2f}", maxFit, sigma_Max ) , " ");
            legend->AddEntry((TObject*)0, Form("#scale[0.8]{#chi^{2}/ndf = %2.2f}", fit->GetChisquare()/fit->GetNDF()), " ");
            // add SNR to legend
            if (opt == 1){
              legend->AddEntry((TObject*)0, Form("#scale[0.8]{SNR = %2.2f}", SNR), " ");
            }
            legend->Draw();
            DrawLines(maxFit, maxFit,0.7, scaleYMax*maxY/10, 5, kRed+3, 7);  
          } else {
            labelChannel->Draw();  
          }
        
          DrawLines(noiseWidth*3, noiseWidth*3,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
          DrawLines(noiseWidth*5, noiseWidth*5,0.7, scaleYMax*maxY, 2, kGray+1, 6);  
        
          if (p == 15 ){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-6*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-6*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          }
        }
      }
    }
    if (skipped < 16)
      canvas->SaveAs(nameOutput.Data());
  }

  //__________________________________________________________________________________________________________
  // Plot Spectra with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotSpectra2ModLayer (TCanvas* canvas, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int option, 
                                  Double_t xMin, Double_t xMax, Double_t scaleYMax, int layer, TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    Setup* setupT = Setup::GetInstance();
    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            std::cout << "WARNING: PlotSpectra2ModLayer skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
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
          if (maxY < FindLargestBin1DHist(tempHist, xMin , xMax)) maxY = FindLargestBin1DHist(tempHist, xMin , xMax);
        }
      }  
    }
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p = setupT->GetChannelInLayerFull(tempCellID);
          pads[p]->Draw();
          pads[p]->cd();
          pads[p]->SetLogy();
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            skipped++;
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p == 15 ){
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
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
          tempHist->GetXaxis()->SetRangeUser(xMin,xMax);
          tempHist->GetYaxis()->SetRangeUser(0.7,scaleYMax*maxY);
          
          tempHist->Draw("pe");
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xMin, 0, xMax, maxY);
          tempHist->Draw("same,axis");
          tempHist->Draw("same,pe");
                  
          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          if (p == 15){
            label = Form("r:%d c:%d m:%d layer %d", r, c, m, layer);
          }
          TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.045,topRCornerY[p]-1.2*relSize8P[p],label);
          SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);
          labelChannel->Draw();  
        
          if (option < 2){
            DrawLines(noiseWidth*3, noiseWidth*3,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
            DrawLines(noiseWidth*5, noiseWidth*5,0.7, scaleYMax*maxY, 2, kGray+1, 6);  
          } else if (option == 3){
            DrawLines(0.3, 0.3, 0.7, scaleYMax*maxY, 2, kGray+1, 10);  
          }
          if (p == 15 ){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          }
        }
      }
    }
    if (skipped < 16)
      canvas->SaveAs(nameOutput.Data());
  }
  
  //__________________________________________________________________________________________________________
  // Plot Corr with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotCorrWithFits2ModLayer (TCanvas* canvas, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int option, 
                                  Double_t xMin, Double_t xMax, Double_t maxY, int layer, TString nameOutput, RunInfo currRunInfo){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileSpectra>::iterator ithSpectra;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
    int skipped = 0;
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p = setupT->GetChannelInLayerFull(tempCellID);
          pads[p]->Draw();
          pads[p]->SetLogy(0);
          pads[p]->cd();
          
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            skipped++;
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p == 15 ){
              DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), false, 0.85*relSize8P[p], 42);
              DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), false, 0.85*relSize8P[p], 42);
            }
          continue;
          } 
          TProfile* tempProfile = nullptr;
          if (option == 1 || option == 2){
              tempProfile = ithSpectra->second.GetHGLGcorr();
          } else {
              tempProfile = ithSpectra->second.GetLGHGcorr();
          }
          if (!tempProfile) continue;
          TH1D* dummyhist = new TH1D("dummyhist", "", tempProfile->GetNbinsX(), tempProfile->GetXaxis()->GetXmin(), tempProfile->GetXaxis()->GetXmax());
          SetStyleHistoTH1ForGraphs( dummyhist, tempProfile->GetXaxis()->GetTitle(), tempProfile->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  


          // SetStyleTProfile( tempProfile, tempProfile->GetXaxis()->GetTitle(), tempProfile->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.1, 510, 510, 43, 63);  
          SetMarkerDefaultsProfile(tempProfile, 20, 1, kBlue+1, kBlue+1);   
          Int_t maxX = 3900;        
          if (option == 0 || option == 2 )
            maxX = 340;
          if (option == 2){
            dummyhist->GetYaxis()->SetRangeUser(-maxY,maxY);
            dummyhist->GetXaxis()->SetRangeUser(xMin,maxX);
          } else {
            dummyhist->GetYaxis()->SetRangeUser(0,maxY);
            dummyhist->GetXaxis()->SetRangeUser(0,maxX);
          }
          
          dummyhist->Draw("axis");
          DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel,xMin, 0, xMax, maxY);
          tempProfile->Draw("pe, same");
                  
          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          if (p == 15){
            label = Form("r:%d c:%d m:%d layer %d", r, c, m, layer);
          }
          TLatex *labelChannel    = new TLatex(topRCornerX[p]+0.045,topRCornerY[p]-1.2*relSize8P[p],label);
          SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,11);

          
          TF1* fit            = nullptr;
          if (option == 1 ){
            fit = ithSpectra->second.GetCorrModel(1);
          } else if (option == 0 ){
            fit = ithSpectra->second.GetCorrModel(0);
          }
          if (fit){
            Double_t rangeFit[2] = {0,0};
            fit->GetRange(rangeFit[0], rangeFit[1]);
            SetStyleFit(fit , rangeFit[0], rangeFit[1], 7, 3, kRed+3);
            fit->Draw("same");
            TLegend* legend = GetAndSetLegend2( topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-0.4*relSize8P[p], topRCornerX[p]+6*relSize8P[p], topRCornerY[p]-0.6*relSize8P[p],0.85*textSizePixel, 1, label, 43,0.1);
            legend->AddEntry(fit, "linear fit, trigg.", "l");
            legend->AddEntry((TObject*)0, Form("#scale[0.8]{b = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");
            legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(1), fit->GetParError(1) ) , " ");
            legend->Draw();
          } else {
            labelChannel->Draw();  
          }
        
          if (option == 2){
          DrawLines(xMin,maxX,0, 0, 2, kGray+1, 10);   
          }
          if (p == 15 ){
            DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), false, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), false, 0.85*relSize8P[p], 42);
          }
        }
      }
    }
    if (skipped < 16)
      canvas->SaveAs(nameOutput.Data());
  }

  //__________________________________________________________________________________________________________
  // Plot Corr with Fits for Full layer 2D
  //__________________________________________________________________________________________________________
  inline void PlotCorr2D2ModLayer (TCanvas* canvas, TPad** pads, 
                          Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                          std::map<int,TileSpectra> spectra, int option,
                          Double_t xMin, Double_t xMax, Double_t minY, Double_t maxY, int layer, TString nameOutput, RunInfo currRunInfo, bool noCalib = 0, int triggCh = -1 ){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileSpectra>::iterator ithSpectra;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
    int skipped = 0;
    ReadOut::Type rotype = ReadOut::Type::Undef;
    
    bool isTriggCh = false;
    if (triggCh != -1)
      isTriggCh = true;
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p     = setupT->GetChannelInLayerFull(tempCellID);
          int roCh  = setupT->GetROchannel(tempCellID);
          if (roCh == triggCh){
            std::cout << (setupT->DecodeCellID(tempCellID)).Data() << std::endl;
          }
          pads[p]->Draw();
          pads[p]->SetLogy(0);
          pads[p]->SetLogz(1);
          pads[p]->cd();
          
          ithSpectra=spectra.find(tempCellID);
          if(ithSpectra==spectra.end()){
            skipped++;
            std::cout << "WARNING: PlotCorr2D2ModLayer skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << "ro ch asic " << roCh << std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p ==15 ){
              DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-1*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
              DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-1*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
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
            if (isTriggCh && triggCh == roCh){
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
            if (isTriggCh && triggCh == roCh){
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
            
          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          if (p == 15){
            label = Form("r:%d c:%d m:%d layer:%d", r, c, m, layer);
          }
          if(option == 1) label = Form("%s, entries=%d",label.Data(),(Int_t)(temp2D->GetEntries()/11) ) ;
          TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.04,topRCornerY[p]-1.2*relSize8P[p],label);
          SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);

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
              legend = GetAndSetLegend2( topRCornerX[p]-0.4, topRCornerY[p]-4*0.85*relSize8P[p]-0.4*relSize8P[p], topRCornerX[p]+0.045, topRCornerY[p]-0.6*relSize8P[p],0.85*textSizePixel, 1, label, 43,0.1);
              legend->AddEntry(fit, "linear fit, trigg.", "l");
              legend->AddEntry((TObject*)0, Form("#scale[0.8]{b = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");
              legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(1), fit->GetParError(1) ) , " ");
              nlinesTot = 4;
            } else {
              legend = GetAndSetLegend2( topRCornerX[p]-0.4, topRCornerY[p]-3*0.85*relSize8P[p]-0.4*relSize8P[p], topRCornerX[p]+0.045, topRCornerY[p]-0.6*relSize8P[p],0.85*textSizePixel, 1, label, 43,0.1);
              legend->AddEntry(fit, "const fit", "l");
              legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.1f #pm %2.1f}",fit->GetParameter(0), fit->GetParError(0) ) , " "); 
              nlinesTot = 3;
            }
            legend->Draw();
          } else {
            labelChannel->Draw();  
          }
        
          if (p ==15 ){
            DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-nlinesTot*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-nlinesTot*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          }
        }
      }
    }
    if (skipped < 16)
      canvas->SaveAs(nameOutput.Data());
  }
  
  //__________________________________________________________________________________________________________
  // Plot Corr with Fits for Full layer
  //__________________________________________________________________________________________________________
  inline void PlotTrending2ModLayer (TCanvas* canvas, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                              std::map<int,TileTrend> trending, int optionTrend, 
                              Double_t xMin, Double_t xMax, Double_t minY, Double_t maxY, bool isSameVoltage, double commonVoltage, 
                              int layer, TString nameOutput, TString nameOutputSummary, RunInfo currRunInfo, Int_t  detailedPlot = 1){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
    int skipped = 0;
    
    
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
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p = setupT->GetChannelInLayerFull(tempCellID);

          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          TString label2          = Form("Common V_{op} = %2.1f V", commonVoltage);
          if (p == 15){
            label = Form("r:%d c:%d m:%d layer %d", r, c, m, layer);
          }

          pads[p]->Draw();
          
          if (optionTrend == 6){ 
            pads[p]->SetLogy(1);
          } else {
            pads[p]->SetLogy(0);          
          }

          pads[p]->cd();
          ithTrend=trending.find(tempCellID);
          if(ithTrend==trending.end()){
            skipped++;
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m<< std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p == 12 ){
              TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
              TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
              TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], lab1, true, 0.85*textSizePixel, 43);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-2*0.85*relSize8P[p], lab2, true, 0.85*textSizePixel, 43);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-3*0.85*relSize8P[p], lab3, true, 0.85*textSizePixel, 43);
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
          if (!tempGraph) continue;
          TH1D* dummyhist = new TH1D("dummyhist", "", 100, xMin, xMax);
          SetStyleHistoTH1ForGraphs( dummyhist, tempGraph->GetXaxis()->GetTitle(), tempGraph->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
          // if (optionTrend == 6)std::cout << "\t" << tempGraph->GetXaxis()->GetTitle() << "\t" << tempGraph->GetYaxis()->GetTitle() << std::endl;
          SetMarkerDefaultsTGraphErr(tempGraph, 20, 1, kBlue+1, kBlue+1);   
          dummyhist->GetYaxis()->SetRangeUser(minY,maxY);
          dummyhist->Draw("axis");
          tempGraph->Draw("pe, same");
                  
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p], label, true, 0.85*textSizePixel, 43);
          if (isSameVoltage && p == 15){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], label2, true, 0.85*textSizePixel, 43);
          }
          if (p == 12 ){
            TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
            TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
            TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], lab1, true, 0.85*textSizePixel, 43);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-2*0.85*relSize8P[p], lab2, true, 0.85*textSizePixel, 43);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-3*0.85*relSize8P[p], lab3, true, 0.85*textSizePixel, 43);
          }
        }
      }
    }
    if (skipped < 16){
      if(detailedPlot) canvas->SaveAs(nameOutput.Data());
      if (layer == 0) canvas->Print(Form("%s.pdf[",nameOutputSummary.Data()));
      canvas->Print(Form("%s.pdf",nameOutputSummary.Data()));
      if (layer == setupT->GetNMaxLayer()) canvas->Print(Form("%s.pdf]",nameOutputSummary.Data()));
    }
  } // end PlotTrending2ModLayer()
  
  //__________________________________________________________________________________________________________
  // Plot Run overlay for all 16 tiles for all runs available
  //__________________________________________________________________________________________________________
  inline void PlotRunOverlay2ModLayer (TCanvas* canvas, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                              std::map<int,TileTrend> trending, int nruns, int optionTrend, 
                              Double_t xMin, Double_t xMax, int layer, TString nameOutput, TString nameOutputSummary, RunInfo currRunInfo, Int_t detailedPlot = 1){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
    int skipped = 0;
    
    Double_t maxY         = 0.;
    Double_t minY         = 9999.;
    bool isSameVoltage    = true;
    double commonVoltage  = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          ithTrend=trending.find(tempCellID);
          if (optionTrend == 0){      // HG
            if(maxY<ithTrend->second.GetMaxHGSpec()) maxY=ithTrend->second.GetMaxHGSpec();
            if(minY>ithTrend->second.GetMinHGSpec()) minY=ithTrend->second.GetMinHGSpec();
          } else if (optionTrend == 1){   //LG
            if(maxY<ithTrend->second.GetMaxLGSpec()) maxY=ithTrend->second.GetMaxLGSpec();
            if(minY>ithTrend->second.GetMinLGSpec()) minY=ithTrend->second.GetMinLGSpec();
          } 
          
          for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
            if (r == 0 && c == 0){
              if (rc == 0){
                commonVoltage = ithTrend->second.GetVoltage(rc);
              } else {
                if (commonVoltage != ithTrend->second.GetVoltage(rc))  isSameVoltage = false;
              }
            }
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
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p = setupT->GetChannelInLayerFull(tempCellID);
          pads[p]->Draw();
          pads[p]->cd();
          pads[p]->SetLogy(1);
          ithTrend=trending.find(tempCellID);

          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          TString label2          = Form("Common V_{op} = %2.1f V", commonVoltage);
          if (p == 15){
            label = Form("r:%d c:%d m:%d layer %d", r, c, m, layer);
          }
          if(ithTrend==trending.end()){
            skipped++;
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p == 12 ){
              TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
              TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
              TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], lab1, true, 0.85*textSizePixel, 43);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-2*0.85*relSize8P[p], lab2, true, 0.85*textSizePixel, 43);
              DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-3*0.85*relSize8P[p], lab3, true, 0.85*textSizePixel, 43);
            }
            
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p], label, true, 0.85*textSizePixel, 43);
            continue;
          } 

          if (p == 15 ){
            double startLegY  = topRCornerY[p]-lineBottom*relSize8P[p];
            double endLegY    = topRCornerY[p]-1.4*relSize8P[p];
            legend = GetAndSetLegend2(  0.3, startLegY, topRCornerX[p]-0.045/2, endLegY,
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
                histos[rc]->GetXaxis()->SetRangeUser(xMin,xMax);
                histos[rc]->GetYaxis()->SetRangeUser(minY,maxY);
                histos[rc]->Draw("hist");
              } else {
                histos[rc]->Draw("same,hist");
              }
              if(p == 15) legend->AddEntry(histos[rc],Form("%d",tmpRunNr),"l");
            }
          }
          if (histos[0]) histos[0]->Draw("axis,same");                
          
          // labeling inside the panels & legend drawing 
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p], label, true, 0.85*textSizePixel, 43);
          if (isSameVoltage && p == 15){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-(lineBottom+0.5)*relSize8P[p], label2, true, 0.85*textSizePixel, 43);
          }
          
          if (p == 15) legend->Draw();
          if (p == 12 ){
            TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
            TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
            TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], lab1, true, 0.85*textSizePixel, 43);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-2*0.85*relSize8P[p], lab2, true, 0.85*textSizePixel, 43);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-3*0.85*relSize8P[p], lab3, true, 0.85*textSizePixel, 43);
          }
        }
      }
    }
    if (skipped < 16){
      if(detailedPlot) canvas->SaveAs(nameOutput.Data());
      if (layer == 0) canvas->Print(Form("%s.pdf[",nameOutputSummary.Data()));
      canvas->Print(Form("%s.pdf",nameOutputSummary.Data()));
      if (layer == setupT->GetNMaxLayer()) canvas->Print(Form("%s.pdf]",nameOutputSummary.Data()));
    }
  }


  //__________________________________________________________________________________________________________
  // Plot Run overlay for all 16 tiles for all runs available
  //__________________________________________________________________________________________________________
  inline void PlotRunOverlayProfile2ModLayer (TCanvas* canvas, TPad** pads, Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                      std::map<int,TileTrend> trending, int nruns, int option,
                                      Double_t xMin, Double_t xMax, Double_t yPMin, Double_t yPMax,  int layer, TString nameOutput, TString nameOutputSummary, 
                                      RunInfo currRunInfo, Int_t detailedPlot = 1, bool scaleInt = false){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int nMod = setupT->GetNMaxModule()+1;
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
    TLegend* legend = nullptr;
    
    if (nSameSettings == 4) lineBottom++;
    std::cout << "number of same settings: "  << nSameSettings << std::endl;
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        for (int m = 0; m < nMod; m++){
          canvas->cd();
          int tempCellID = setupT->GetCellID(r,c, layer, m);
          int p = setupT->GetChannelInLayerFull(tempCellID);
          pads[p]->Draw();
          pads[p]->cd();
          pads[p]->SetLogy(0);
          ithTrend=trending.find(tempCellID);

          TString label           = Form("r:%d c:%d m:%d", r, c, m);
          TString label2          = Form("V_{op}=%2.1fV", currRunInfo.vop);
          TString label3          = GetLabelHGCROCSettings(currRunInfo);

          if(ithTrend==trending.end()){
            skipped++;
            std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << m << std::endl;
            pads[p]->Clear();
            pads[p]->Draw();
            if (p == 12 ){
              TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
              TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
              TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
              DrawLatex(topRCornerX[p]-0.8, topRCornerY[p]-1.2*relSize8P[p]-0*0.85*relSize8P[p], lab1, false, 0.85*textSizePixel, 43);
              DrawLatex(topRCornerX[p]-0.8, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], lab2, false, 0.85*textSizePixel, 43);
              DrawLatex(topRCornerX[p]-0.8, topRCornerY[p]-1.2*relSize8P[p]-2*0.85*relSize8P[p], lab3, false, 0.85*textSizePixel, 43);
            }
            
            DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-1.2*relSize8P[p], label, true, 0.85*textSizePixel, 43);
            continue;
          } 

          if (p == 15 ){
            double startLegY  = topRCornerY[p]-(lineBottom-1)*relSize8P[p];
            double endLegY    = topRCornerY[p]-0.04;
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
            legend = GetAndSetLegend2(  0.05, startLegY, width, endLegY,
                                        0.85*textSizePixel, 5, header,43,0.25);
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
                if (scaleInt) yTitle = Form("%s/ integral", yTitle.Data());

                dummyhist = new TH1D("dummyhist", "", profs[rc]->GetNbinsX(), profs[rc]->GetXaxis()->GetXmin(), profs[rc]->GetXaxis()->GetXmax());
                SetStyleHistoTH1ForGraphs( dummyhist, profs[rc]->GetXaxis()->GetTitle(), yTitle, 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
                dummyhist->GetXaxis()->SetRangeUser(xMin,xMax);
                dummyhist->GetYaxis()->SetRangeUser(yPMin,yPMax);
                dummyhist->Draw("axis");
              }

              SetLineDefaults(profs[rc], GetColorLayer(rc), 2, GetLineStyleLayer(rc));   
              profs[rc]->SetMarkerStyle(GetMarkerLayer(rc));
              profs[rc]->Draw("same,pe");
              if(p == 15){
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
          DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-1.2*relSize8P[p], label, true, 0.85*textSizePixel, 43);
          if ( nSameSettings > 0  && p == 13){
            DrawLatex(0.04, topRCornerY[p]-1.2*relSize8P[p], label2, false, 0.85*textSizePixel, 43);
            DrawLatex(0.04, topRCornerY[p]-1.2*relSize8P[p]-0.85*relSize8P[p], label3, false, 0.85*textSizePixel, 43);
            
          }
          
          if (p == 15) legend->Draw();
          if (p == 12 ){
            TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
            TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
            TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
            DrawLatex(topRCornerX[p]-0.8, topRCornerY[p]-1.2*relSize8P[p]-0*0.85*relSize8P[p], lab1, false, 0.85*textSizePixel, 43);
            DrawLatex(topRCornerX[p]-0.8, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], lab2, false, 0.85*textSizePixel, 43);
            DrawLatex(topRCornerX[p]-0.8, topRCornerY[p]-1.2*relSize8P[p]-2*0.85*relSize8P[p], lab3, false, 0.85*textSizePixel, 43);
          }
        }
      }
    }
    if (skipped < 16){
      if(detailedPlot) canvas->SaveAs(nameOutput.Data());
      if (layer == 0) canvas->Print(Form("%s.pdf[",nameOutputSummary.Data()));
      canvas->Print(Form("%s.pdf",nameOutputSummary.Data()));
      if (layer == setupT->GetNMaxLayer()) canvas->Print(Form("%s.pdf]",nameOutputSummary.Data()));
    }
  }
  
#endif

