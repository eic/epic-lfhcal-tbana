#ifndef PLOTTHELPER_8MLAYER_H
#define PLOTTHELPER_8MLAYER_H

  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // dedicated class for all 8M layer plotting functions
  // +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

  //__________________________________________________________________________________________________________
  // Plot Trigger Primitive with Fits for Full layer
  //__________________________________________________________________________________________________________
  void PlotTriggerPrimWithFits8MLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, 
                                         Double_t* relSize8P, Int_t textSizePixel, 
                                         std::map<int,TileSpectra> spectra, Setup* setupT, 
                                         double avMip, double facLow, double facHigh,
                                         Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                                         int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    std::map<int, TileSpectra>::iterator ithSpectraTrigg;
    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          continue;
        } 
        TH1D* tempHist = ithSpectra->second.GetTriggPrim();
        if (maxY < FindLargestBin1DHist(tempHist, xPMin , xPMax)) maxY = FindLargestBin1DHist(tempHist, xPMin , xPMax);
      }  
    }

    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        canvas8Panel->cd();
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->cd();
        pads[p]->SetLogy();
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          skipped++;
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          pads[p]->Clear();
          pads[p]->Draw();
          if (p ==7 ){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-2.*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-3.*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4.*relSize8P[p], "Trigger primitives", true, 0.85*relSize8P[p], 42);
          }
          continue;
        } 
        TH1D* tempHist = ithSpectra->second.GetTriggPrim();
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

        TString label           = Form("row %d col %d", r, c);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
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
        
        if (p ==7 ){
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-2.*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-3.*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4.*relSize8P[p], "Trigger primitives", true, 0.85*relSize8P[p], 42);
        }
      }
    }
    if (skipped < 6)
      canvas8Panel->SaveAs(nameOutput.Data());
  }
    
  //__________________________________________________________________________________________________________
  // Plot Noise with Fits for Full layer
  //__________________________________________________________________________________________________________
  void PlotNoiseWithFits8MLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, Setup* setupT, bool isHG, 
                                  Double_t xPMin, Double_t xPMax, Double_t scaleYMax, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          skipped++;
          continue;
        } 
        TH1D* tempHist = nullptr;
        if (isHG){
          tempHist = ithSpectra->second.GetHG();
        } else {
          tempHist = ithSpectra->second.GetLG();
        }
        if (maxY < FindLargestBin1DHist(tempHist, xPMin , xPMax)) maxY = FindLargestBin1DHist(tempHist, xPMin , xPMax);
      }  
    }
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        canvas8Panel->cd();
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->cd();
        pads[p]->SetLogy();
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          pads[p]->Clear();
          pads[p]->Draw();
          if (p ==7 ){
            DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          }
          continue;
        } 
        TH1D* tempHist = nullptr;
        if (isHG){
            tempHist = ithSpectra->second.GetHG();
        } else {
            tempHist = ithSpectra->second.GetLG();
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
        
        TString label           = Form("row %d col %d", r, c);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
        }
        TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.04,topRCornerY[p]-1.2*relSize8P[p],label);
        SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);

        
        TF1* fit = nullptr;
        if (isHG){
          fit = ithSpectra->second.GetBackModel(1);
        } else {
          fit = ithSpectra->second.GetBackModel(0);  
        }
        if (fit){
          SetStyleFit(fit , xPMin, xPMax, 7, 7, kBlack);
          fit->Draw("same");
          TLegend* legend = GetAndSetLegend2( topRCornerX[p]-8*relSize8P[p], topRCornerY[p]-4*0.85*relSize8P[p]-0.4*relSize8P[p], topRCornerX[p]-0.04, topRCornerY[p]-0.6*relSize8P[p],0.85*textSizePixel, 1, label, 43,0.2);
          legend->AddEntry(fit, "Gauss noise fit", "l");
          legend->AddEntry((TObject*)0, Form("#mu = %2.2f #pm %2.2f",fit->GetParameter(1), fit->GetParError(1) ) , " ");
          legend->AddEntry((TObject*)0, Form("#sigma = %2.2f #pm %2.2f",fit->GetParameter(2), fit->GetParError(2) ) , " ");
          legend->Draw();
            
        } else {
          labelChannel->Draw();  
        }
      
        if (xPMin < -5) DrawLines(0, 0,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
      
        if (p ==7 ){
          DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
          DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
        }
      }
    }
    if (skipped < 6)
      canvas8Panel->SaveAs(nameOutput.Data());
  }
  
  
  //__________________________________________________________________________________________________________
  // Plot Noise extracted from collision data
  //__________________________________________________________________________________________________________
  void PlotNoiseAdvWithFits8MLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                      std::map<int,TileSpectra> spectra, std::map<int,TileSpectra> spectraTrigg, bool isHG, 
                                      Double_t xPMin, Double_t xPMax, Double_t scaleYMax, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    std::map<int, TileSpectra>::iterator ithSpectraTrigg;
    
    Setup* setupT = Setup::GetInstance();
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          continue;
        } 
        TH1D* tempHist = nullptr;
        if (isHG){
          tempHist = ithSpectra->second.GetHG();
        } else {
          tempHist = ithSpectra->second.GetLG();
        }
        if (maxY < FindLargestBin1DHist(tempHist, xPMin , xPMax)) maxY = FindLargestBin1DHist(tempHist, xPMin , xPMax);
      }  
    }
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        canvas8Panel->cd();
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->cd();
        pads[p]->SetLogy();
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          skipped++;
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          pads[p]->Clear();
          pads[p]->Draw();
          if (p ==7 ){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          }
          continue;
        } 
        ithSpectraTrigg=spectraTrigg.find(tempCellID);
        TH1D* tempHist = nullptr;
        if (isHG){
            tempHist = ithSpectra->second.GetHG();
        } else {
            tempHist = ithSpectra->second.GetLG();
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
        
        TH1D* tempHistT = nullptr;
        if (isHG){
            tempHistT = ithSpectraTrigg->second.GetHG();
        } else {
            tempHistT = ithSpectraTrigg->second.GetLG();
        }
        SetMarkerDefaults(tempHistT, 24, 1, kRed+1, kRed+1, kFALSE);   
        tempHistT->Draw("same,pe");
        
        TString label           = Form("row %d col %d", r, c);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
        }
        TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.045,topRCornerY[p]-1.2*relSize8P[p],label);
        SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);

        
        TF1* fit            = nullptr;
        bool isTrigFit      = false;
        if (isHG){
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
        if (p ==7 ){
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
        }
      
      }
    }
    if (skipped < 6)
      canvas8Panel->SaveAs(nameOutput.Data());
  }  
  
  //__________________________________________________________________________________________________________
  // Plot Mip with Fits for Full layer
  //__________________________________________________________________________________________________________
  void PlotMipWithFits8MLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, std::map<int,TileSpectra> spectraTrigg, Setup* setupT, bool isHG, 
                                  Double_t xPMin, Double_t xPMax, Double_t scaleYMax, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    std::map<int, TileSpectra>::iterator ithSpectraTrigg;
    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          continue;
        } 
        TH1D* tempHist = nullptr;
        if (isHG){
          tempHist = ithSpectra->second.GetHG();
        } else {
          tempHist = ithSpectra->second.GetLG();
        }
        if (maxY < FindLargestBin1DHist(tempHist, xPMin , xPMax)) maxY = FindLargestBin1DHist(tempHist, xPMin , xPMax);
      }  
    }
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        canvas8Panel->cd();
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->cd();
        pads[p]->SetLogy();
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          skipped++;
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          pads[p]->Clear();
          pads[p]->Draw();
          if (p ==7 ){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          }          
          continue;
        } 
        ithSpectraTrigg=spectraTrigg.find(tempCellID);
        TH1D* tempHist = nullptr;
        double noiseWidth = 0;
        if (isHG){
            tempHist = ithSpectra->second.GetHG();
            noiseWidth = ithSpectra->second.GetCalib()->PedestalSigH;
        } else {
            tempHist = ithSpectra->second.GetLG();
            noiseWidth = ithSpectra->second.GetCalib()->PedestalSigL;
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
        
        TH1D* tempHistT = nullptr;
        
        if (isHG){
            tempHistT = ithSpectraTrigg->second.GetHG();
        } else {
            tempHistT = ithSpectraTrigg->second.GetLG();
        }
        SetMarkerDefaults(tempHistT, 24, 1, kRed+1, kRed+1, kFALSE);   
        tempHistT->Draw("same,pe");
        
        TString label           = Form("row %d col %d", r, c);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
        }
        TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.045,topRCornerY[p]-1.2*relSize8P[p],label);
        SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);

        
        TF1* fit            = nullptr;
        bool isTrigFit      = false;
        double maxFit       = 0;
        if (isHG){
          fit = ithSpectraTrigg->second.GetSignalModel(1);
          if (!fit){
              fit = ithSpectra->second.GetSignalModel(1);
              if (fit){
                maxFit = ithSpectra->second.GetCalib()->ScaleH;
              }
          } else {
              isTrigFit = true;
              maxFit = ithSpectraTrigg->second.GetCalib()->ScaleH;
          }
        } else {
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
          TLegend* legend = GetAndSetLegend2( topRCornerX[p]-10*relSize8P[p], topRCornerY[p]-4*0.85*relSize8P[p]-0.4*relSize8P[p], topRCornerX[p]-0.04, topRCornerY[p]-0.6*relSize8P[p],0.85*textSizePixel, 1, label, 43,0.1);
          if (isTrigFit)
            legend->AddEntry(fit, "Landau-Gauss fit, trigg.", "l");
          else 
            legend->AddEntry(fit, "Landau-Gauss fit", "l");  
          legend->AddEntry((TObject*)0, Form("#scale[0.8]{L MPV = %2.2f #pm %2.2f}",fit->GetParameter(1), fit->GetParError(1) ) , " ");
          legend->AddEntry((TObject*)0, Form("#scale[0.8]{Max = %2.2f}", maxFit ) , " ");
          legend->Draw();
          DrawLines(maxFit, maxFit,0.7, scaleYMax*maxY/10, 5, kRed+3, 7);  
        } else {
          labelChannel->Draw();  
        }
      
        DrawLines(noiseWidth*3, noiseWidth*3,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
        DrawLines(noiseWidth*5, noiseWidth*5,0.7, scaleYMax*maxY, 2, kGray+1, 6);  
      
        if (p ==7 ){
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
        }
      }
    }
    if (skipped < 6)
      canvas8Panel->SaveAs(nameOutput.Data());
  }

  //__________________________________________________________________________________________________________
  // Plot Spectra with Fits for Full layer
  //__________________________________________________________________________________________________________
  void PlotSpectra8MLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int isHG, 
                                  Double_t xPMin, Double_t xPMax, Double_t scaleYMax, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    Setup* setupT = Setup::GetInstance();
    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          continue;
        } 
        TH1D* tempHist = nullptr;
        if (isHG == 0){
          tempHist = ithSpectra->second.GetHG();
        } else if (isHG == 1){
          tempHist = ithSpectra->second.GetLG();
        } else {
          tempHist = ithSpectra->second.GetComb();
        }
        if (maxY < FindLargestBin1DHist(tempHist, xPMin , xPMax)) maxY = FindLargestBin1DHist(tempHist, xPMin , xPMax);
      }  
    }
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        canvas8Panel->cd();
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->cd();
        pads[p]->SetLogy();
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          skipped++;
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          pads[p]->Clear();
          pads[p]->Draw();
          if (p ==7 ){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
          }          
          continue;
        } 
        TH1D* tempHist = nullptr;
        double noiseWidth = 0;
        if (isHG == 0){
            tempHist = ithSpectra->second.GetHG();
            noiseWidth = ithSpectra->second.GetCalib()->PedestalSigH;
        } else if (isHG == 1){
            tempHist = ithSpectra->second.GetLG();
            noiseWidth = ithSpectra->second.GetCalib()->PedestalSigL;
        } else {
            tempHist = ithSpectra->second.GetComb();
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
                
        TString label           = Form("row %d col %d", r, c);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
        }
        TLatex *labelChannel    = new TLatex(topRCornerX[p]-0.045,topRCornerY[p]-1.2*relSize8P[p],label);
        SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,31);
        labelChannel->Draw();  
      
        if (isHG < 2){
          DrawLines(noiseWidth*3, noiseWidth*3,0.7, scaleYMax*maxY, 2, kGray+1, 10);  
          DrawLines(noiseWidth*5, noiseWidth*5,0.7, scaleYMax*maxY, 2, kGray+1, 6);  
        } else {
          DrawLines(0.3, 0.3, 0.7, scaleYMax*maxY, 2, kGray+1, 10);  
        }
        if (p ==7 ){
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSize8P[p], 42);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSize8P[p], 42);
        }
      }
    }
    if (skipped < 6)
      canvas8Panel->SaveAs(nameOutput.Data());
  }
  
  //__________________________________________________________________________________________________________
  // Plot Corr with Fits for Full layer
  //__________________________________________________________________________________________________________
  void PlotCorrWithFits8MLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, int option, 
                                  Double_t xPMin, Double_t xPMax, Double_t maxY, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileSpectra>::iterator ithSpectra;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        canvas8Panel->cd();
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->SetLogy(0);
        pads[p]->cd();
        
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          skipped++;
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          pads[p]->Clear();
          pads[p]->Draw();
          if (p ==7 ){
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
          dummyhist->GetXaxis()->SetRangeUser(xPMin,maxX);
        } else {
          dummyhist->GetYaxis()->SetRangeUser(0,maxY);
          dummyhist->GetXaxis()->SetRangeUser(0,maxX);
        }
        
        
        dummyhist->Draw("axis");

        short bctemp = ithSpectra->second.GetCalib()->BadChannel;
        if (bctemp != -64 && bctemp < 3){
          Color_t boxCol = kGray;
          if (bctemp == 1)
            boxCol = kGray+1;
          else if (bctemp == 0)
            boxCol = kGray+2;
          TBox* badChannelArea =  CreateBox(boxCol, 0, 0, maxX,maxY, 1001 );
          badChannelArea->Draw();
          dummyhist->Draw("axis,same");
        }

        tempProfile->Draw("pe, same");
                
        TString label           = Form("row %d col %d", r, c);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
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
         DrawLines(xPMin,maxX,0, 0, 2, kGray+1, 10);   
        }
        if (p ==7 ){
          DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), false, 0.85*relSize8P[p], 42);
          DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), false, 0.85*relSize8P[p], 42);
        }
      }
    }
    if (skipped < 6)
      canvas8Panel->SaveAs(nameOutput.Data());
  }

  //__________________________________________________________________________________________________________
  // Plot Corr with Fits for Full layer 2D
  //__________________________________________________________________________________________________________
  void PlotCorr2D8MLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, 
                                  Double_t xPMin, Double_t xPMax, Double_t maxY, int layer, int mod,  TString nameOutput, RunInfo currRunInfo){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileSpectra>::iterator ithSpectra;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    ReadOut::Type rotype = ReadOut::Type::Undef;
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        canvas8Panel->cd();
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->SetLogy(0);
        pads[p]->SetLogz(1);
        pads[p]->cd();
        
        ithSpectra=spectra.find(tempCellID);
        if(ithSpectra==spectra.end()){
          skipped++;
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          pads[p]->Clear();
          pads[p]->Draw();
          if (p ==7 ){
            DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), false, 0.85*relSize8P[p], 42);
            DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), false, 0.85*relSize8P[p], 42);
          }
          continue;
        } else {
          rotype = ithSpectra->second.GetROType();
        }
        TProfile* tempProfile = ithSpectra->second.GetLGHGcorr();
        TH2D* temp2D          = ithSpectra->second.GetCorr();
        if (!tempProfile) continue;
        SetStyleHistoTH2ForGraphs( temp2D, tempProfile->GetXaxis()->GetTitle(), tempProfile->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
        SetMarkerDefaultsProfile(tempProfile, 24, 0.7, kRed+2, kRed+2);   
        
        temp2D->GetYaxis()->SetRangeUser(0,maxY);
        temp2D->GetXaxis()->SetRangeUser(0,xPMax);
        temp2D->Draw("colz");

        short bctemp = ithSpectra->second.GetCalib()->BadChannel;
        if (bctemp != -64 && bctemp < 3){
          Color_t boxCol = kGray;
          if (bctemp == 1)
            boxCol = kGray+1;
          else if (bctemp == 0)
            boxCol = kGray+2;
          TBox* badChannelArea =  CreateBox(boxCol, 0, 0, xPMax,maxY, 1001 );
          badChannelArea->Draw();
          temp2D->Draw("axis,same");
        }

        TString xTit = temp2D->GetXaxis()->GetTitle();
        if (xTit.Contains("ample") != 0){
          tempProfile->Draw("pe, same");
        } else {
          tempProfile->Draw("pe, same");
        }
          
        TString label           = Form("row %d col %d", r, c);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
        }
        TLatex *labelChannel    = new TLatex(topRCornerX[p]+0.045,topRCornerY[p]-1.2*relSize8P[p],label);
        SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,11);

        TF1* fit            = ithSpectra->second.GetCorrModel(0);
        if (rotype == ReadOut::Type::Hgcroc)
          fit            = ithSpectra->second.GetCorrModel(1);
        if (fit){
          Double_t rangeFit[2] = {0,0};
          fit->GetRange(rangeFit[0], rangeFit[1]);
          SetStyleFit(fit , rangeFit[0], rangeFit[1], 7, 3, kRed+3);
          fit->Draw("same");
          TLegend* legend = nullptr;
          if (rotype == ReadOut::Type::Caen){
            legend = GetAndSetLegend2( topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-0.4*relSize8P[p], topRCornerX[p]+6*relSize8P[p], topRCornerY[p]-0.6*relSize8P[p],0.85*textSizePixel, 1, label, 43,0.1);
            legend->AddEntry(fit, "linear fit, trigg.", "l");
            legend->AddEntry((TObject*)0, Form("#scale[0.8]{b = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");
            legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(1), fit->GetParError(1) ) , " ");
          } else {
            legend = GetAndSetLegend2( topRCornerX[p]+0.045, topRCornerY[p]-3*0.85*relSize8P[p]-0.4*relSize8P[p], topRCornerX[p]+6*relSize8P[p], topRCornerY[p]-0.6*relSize8P[p],0.85*textSizePixel, 1, label, 43,0.1);
            legend->AddEntry(fit, "const fit", "l");
            legend->AddEntry((TObject*)0, Form("#scale[0.8]{a = %2.3f #pm %2.4f}",fit->GetParameter(0), fit->GetParError(0) ) , " ");   
          }
          legend->Draw();
        } else {
          labelChannel->Draw();  
        }
      
        if (p ==7 ){
          DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-1.4*relSize8P[p], GetStringFromRunInfo(currRunInfo, 2), false, 0.85*relSize8P[p], 42);
          DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-4*0.85*relSize8P[p]-2.2*relSize8P[p], GetStringFromRunInfo(currRunInfo, 3), false, 0.85*relSize8P[p], 42);
        }
      }
    }
    if (skipped < 6)
      canvas8Panel->SaveAs(nameOutput.Data());
  }
  
  //__________________________________________________________________________________________________________
  // Plot Corr with Fits for Full layer
  //__________________________________________________________________________________________________________
  void PlotTrending8MLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                              std::map<int,TileTrend> trending, int optionTrend, 
                              Double_t xPMin, Double_t xPMax, int layer, int mod,  TString nameOutput, TString nameOutputSummary, RunInfo currRunInfo, Int_t  detailedPlot = 1){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    bool isSameVoltage    = true;
    double commanVoltage  = 0;
    
    Double_t minY = 9999;
    Double_t maxY = 0.;
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        ithTrend=trending.find(tempCellID);
        if (optionTrend == 0){
          if(minY>ithTrend->second.GetMinHGped()) minY=ithTrend->second.GetMinHGped();
          if(maxY<ithTrend->second.GetMaxHGped()) maxY=ithTrend->second.GetMaxHGped();
        } else if (optionTrend == 1){
          if(minY>ithTrend->second.GetMinLGped()) minY=ithTrend->second.GetMinLGped();
          if(maxY<ithTrend->second.GetMaxLGped()) maxY=ithTrend->second.GetMaxLGped();
        } else if (optionTrend == 2){
          if(minY>ithTrend->second.GetMinHGscale()) minY=ithTrend->second.GetMinHGscale();
          if(maxY<ithTrend->second.GetMaxHGscale()) maxY=ithTrend->second.GetMaxHGscale();
        } else if (optionTrend == 3){
          if(minY>ithTrend->second.GetMinLGscale()) minY=ithTrend->second.GetMinLGscale();
          if(maxY<ithTrend->second.GetMaxLGscale()) maxY=ithTrend->second.GetMaxLGscale();
        } else if (optionTrend == 4){
          if(minY>ithTrend->second.GetMinLGHGcorr()) minY=ithTrend->second.GetMinLGHGcorr();
          if(maxY<ithTrend->second.GetMaxLGHGcorr()) maxY=ithTrend->second.GetMaxLGHGcorr();
        } else if (optionTrend == 5){
          if(minY>ithTrend->second.GetMinHGLGcorr()) minY=ithTrend->second.GetMinHGLGcorr();
          if(maxY<ithTrend->second.GetMaxHGLGcorr()) maxY=ithTrend->second.GetMaxHGLGcorr();          
        } else if (optionTrend == 6){
          if(minY>ithTrend->second.GetMinTrigg()) minY=ithTrend->second.GetMinTrigg();
          if(maxY<ithTrend->second.GetMaxTrigg()) maxY=ithTrend->second.GetMaxTrigg();          
        } else if (optionTrend == 7){
          if(minY>ithTrend->second.GetMinSBSignal()) minY=ithTrend->second.GetMinSBSignal();
          if(maxY<ithTrend->second.GetMaxSBSignal()) maxY=ithTrend->second.GetMaxSBSignal();          
        } else if (optionTrend == 8){
          if(minY>ithTrend->second.GetMinSBNoise()) minY=ithTrend->second.GetMinSBNoise();
          if(maxY<ithTrend->second.GetMaxSBNoise()) maxY=ithTrend->second.GetMaxSBNoise();          
        } else if (optionTrend == 9){
          if(minY>ithTrend->second.GetMinHGMPV()) minY=ithTrend->second.GetMinHGMPV();
          if(maxY<ithTrend->second.GetMaxHGMPV()) maxY=ithTrend->second.GetMaxHGMPV();          
        } else if (optionTrend == 10){
          if(minY>ithTrend->second.GetMinLGMPV()) minY=ithTrend->second.GetMinLGMPV();
          if(maxY<ithTrend->second.GetMaxLGMPV()) maxY=ithTrend->second.GetMaxLGMPV();          
        } else if (optionTrend == 11){
          if(minY>ithTrend->second.GetMinHGLSigma()) minY=ithTrend->second.GetMinHGLSigma();
          if(maxY<ithTrend->second.GetMaxHGLSigma()) maxY=ithTrend->second.GetMaxHGLSigma();          
        } else if (optionTrend == 12){
          if(minY>ithTrend->second.GetMinLGLSigma()) minY=ithTrend->second.GetMinLGLSigma();
          if(maxY<ithTrend->second.GetMaxLGLSigma()) maxY=ithTrend->second.GetMaxLGLSigma();          
        } else if (optionTrend == 13){
          if(minY>ithTrend->second.GetMinHGGSigma()) minY=ithTrend->second.GetMinHGGSigma();
          if(maxY<ithTrend->second.GetMaxHGGSigma()) maxY=ithTrend->second.GetMaxHGGSigma();          
        } else if (optionTrend == 14){
          if(minY>ithTrend->second.GetMinLGGSigma()) minY=ithTrend->second.GetMinLGGSigma();
          if(maxY<ithTrend->second.GetMaxLGGSigma()) maxY=ithTrend->second.GetMaxLGGSigma();          
        } else if (optionTrend == 15){
          if(minY>ithTrend->second.GetMinHGpedwidth()) minY=ithTrend->second.GetMinHGpedwidth();
          if(maxY<ithTrend->second.GetMaxHGpedwidth()) maxY=ithTrend->second.GetMaxHGpedwidth();          
        } else if (optionTrend == 16){
          if(minY>ithTrend->second.GetMinLGpedwidth()) minY=ithTrend->second.GetMinLGpedwidth();
          if(maxY<ithTrend->second.GetMaxLGpedwidth()) maxY=ithTrend->second.GetMaxLGpedwidth();          
        } else if (optionTrend == 17){
          if(minY>ithTrend->second.GetMinLGHGOffset()) minY=ithTrend->second.GetMinLGHGOffset();
          if(maxY<ithTrend->second.GetMaxLGHGOffset()) maxY=ithTrend->second.GetMaxLGHGOffset();          
        } else if (optionTrend == 18){
          if(minY>ithTrend->second.GetMinHGLGOffset()) minY=ithTrend->second.GetMinHGLGOffset();
          if(maxY<ithTrend->second.GetMaxHGLGOffset()) maxY=ithTrend->second.GetMaxHGLGOffset();          
        }
        for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
          if (r == 0 && c == 0){
            if (rc == 0){
              commanVoltage = ithTrend->second.GetVoltage(rc);
            } else {
              if (commanVoltage != ithTrend->second.GetVoltage(rc))  isSameVoltage = false;
            }
          }
        }
      }
    }
    if (minY == 9999 && maxY == 0.){
      std::cout <<"Something went wrong! No ranges set for layer " <<  layer << " \t trend plotting option: " << optionTrend << "\t ABORTING!" << std::endl;
      return;
    }
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
        
        canvas8Panel->cd();
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        int p = setupT->GetChannelInLayer(tempCellID);

        TString label           = Form("row %d col %d", r, c);
        TString label2          = Form("Common V_{op} = %2.1f V", commanVoltage);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
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
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          pads[p]->Clear();
          pads[p]->Draw();
          if (p ==4 ){
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
        TH1D* dummyhist = new TH1D("dummyhist", "", 100, xPMin, xPMax);
        SetStyleHistoTH1ForGraphs( dummyhist, tempGraph->GetXaxis()->GetTitle(), tempGraph->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.5, 510, 510, 43, 63);  
        // if (optionTrend == 6)std::cout << "\t" << tempGraph->GetXaxis()->GetTitle() << "\t" << tempGraph->GetYaxis()->GetTitle() << std::endl;
        SetMarkerDefaultsTGraphErr(tempGraph, 20, 1, kBlue+1, kBlue+1);   
        dummyhist->GetYaxis()->SetRangeUser(minY,maxY);
        dummyhist->Draw("axis");
        tempGraph->Draw("pe, same");
                
        DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p], label, true, 0.85*textSizePixel, 43);
        if (isSameVoltage && p == 7){
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], label2, true, 0.85*textSizePixel, 43);
        }
        if (p ==4 ){
          TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
          TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
          TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], lab1, true, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-2*0.85*relSize8P[p], lab2, true, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-3*0.85*relSize8P[p], lab3, true, 0.85*textSizePixel, 43);
        }
      }
    }
    if (skipped < 8){
      if(detailedPlot) canvas8Panel->SaveAs(nameOutput.Data());
      if (layer == 0) canvas8Panel->Print(Form("%s.pdf[",nameOutputSummary.Data()));
      canvas8Panel->Print(Form("%s.pdf",nameOutputSummary.Data()));
      if (layer == setupT->GetNMaxLayer()) canvas8Panel->Print(Form("%s.pdf]",nameOutputSummary.Data()));
    }
  }
  
  //__________________________________________________________________________________________________________
  // Plot Run overlay for all 8 tiles for all runs available
  //__________________________________________________________________________________________________________
  void PlotRunOverlay8MLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                              std::map<int,TileTrend> trending, int nruns, int optionTrend, 
                              Double_t xPMin, Double_t xPMax, int layer, int mod,  TString nameOutput, TString nameOutputSummary, RunInfo currRunInfo, Int_t detailedPlot = 1){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    
    Double_t maxY         = 0.;
    Double_t minY         = 9999.;
    bool isSameVoltage    = true;
    double commanVoltage  = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
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
              commanVoltage = ithTrend->second.GetVoltage(rc);
            } else {
              if (commanVoltage != ithTrend->second.GetVoltage(rc))  isSameVoltage = false;
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
        canvas8Panel->cd();
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->cd();
        pads[p]->SetLogy(1);
        ithTrend=trending.find(tempCellID);

        TString label           = Form("row %d col %d", r, c);
        TString label2          = Form("Common V_{op} = %2.1f V", commanVoltage);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
        }
        if(ithTrend==trending.end()){
          skipped++;
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          pads[p]->Clear();
          pads[p]->Draw();
          if (p ==4 ){
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

        if (p ==7 ){
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
              histos[rc]->GetXaxis()->SetRangeUser(xPMin,xPMax);
              histos[rc]->GetYaxis()->SetRangeUser(minY,maxY);
              histos[rc]->Draw("hist");
            } else {
              histos[rc]->Draw("same,hist");
            }
            if(p == 7) legend->AddEntry(histos[rc],Form("%d",tmpRunNr),"l");
          }
        }
        if (histos[0]) histos[0]->Draw("axis,same");                
        
        // labeling inside the panels & legend drawing 
        DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p], label, true, 0.85*textSizePixel, 43);
        if (isSameVoltage && p == 7){
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-(lineBottom+0.5)*relSize8P[p], label2, true, 0.85*textSizePixel, 43);
        }
        
        if (p == 7) legend->Draw();
        if (p ==4 ){
          TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
          TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
          TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], lab1, true, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-2*0.85*relSize8P[p], lab2, true, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSize8P[p]-3*0.85*relSize8P[p], lab3, true, 0.85*textSizePixel, 43);
        }
      }
    }
    if (skipped < 8){
      if(detailedPlot) canvas8Panel->SaveAs(nameOutput.Data());
      if (layer == 0) canvas8Panel->Print(Form("%s.pdf[",nameOutputSummary.Data()));
      canvas8Panel->Print(Form("%s.pdf",nameOutputSummary.Data()));
      if (layer == setupT->GetNMaxLayer()) canvas8Panel->Print(Form("%s.pdf]",nameOutputSummary.Data()));
    }
  }
  
  //__________________________________________________________________________________________________________
  // Plot Run overlay for all 8 tiles for all runs available
  //__________________________________________________________________________________________________________
  void PlotRunOverlayProfile8MLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                      std::map<int,TileTrend> trending, int nruns,
                                      Double_t xPMin, Double_t xPMax, Double_t yPMin, Double_t yPMax,  int layer, int mod,  TString nameOutput, TString nameOutputSummary, RunInfo currRunInfo, Int_t detailedPlot = 1){
                                  
    Setup* setupT = Setup::GetInstance();
    
    std::map<int, TileTrend>::iterator ithTrend;    
    int nRow = setupT->GetNMaxRow()+1;
    int nCol = setupT->GetNMaxColumn()+1;
    int skipped = 0;
    
    bool isSameVoltage    = true;
    double commanVoltage  = 0;
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        ithTrend=trending.find(tempCellID);
        for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
          if (r == 0 && c == 0){
            if (rc == 0){
              commanVoltage = ithTrend->second.GetVoltage(rc);
            } else {
              if (commanVoltage != ithTrend->second.GetVoltage(rc))  isSameVoltage = false;
            }
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
    
    for (int r = 0; r < nRow; r++){
      for (int c = 0; c < nCol; c++){
        canvas8Panel->cd();
        int tempCellID = setupT->GetCellID(r,c, layer, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->cd();
        pads[p]->SetLogy(0);
        ithTrend=trending.find(tempCellID);

        TString label           = Form("row %d col %d", r, c);
        TString label2          = Form("Common V_{op} = %2.1f V", commanVoltage);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
        }
        if(ithTrend==trending.end()){
          skipped++;
          std::cout << "WARNING: skipping cell ID: " << tempCellID << "\t row " << r << "\t column " << c << "\t layer " << layer << "\t module " << mod << std::endl;
          pads[p]->Clear();
          pads[p]->Draw();
          if (p ==4 ){
            TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
            TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
            TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
            DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], lab1, false, 0.85*textSizePixel, 43);
            DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-1.2*relSize8P[p]-2*0.85*relSize8P[p], lab2, false, 0.85*textSizePixel, 43);
            DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-1.2*relSize8P[p]-3*0.85*relSize8P[p], lab3, false, 0.85*textSizePixel, 43);
          }
          
          DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-1.2*relSize8P[p], label, false, 0.85*textSizePixel, 43);
          continue;
        } 

        if (p ==7 ){
          double startLegY  = topRCornerY[p]-lineBottom*relSize8P[p];
          double endLegY    = topRCornerY[p]-1.4*relSize8P[p];
          legend = GetAndSetLegend2(  topRCornerX[p]+0.045/2, startLegY, 0.7, endLegY,
                                      0.85*textSizePixel, 5, "",43,0.25);
        }
        
        TH1D* dummyhist;
        for (int rc = 0; rc < ithTrend->second.GetNRuns() && rc < 30; rc++ ){
          int tmpRunNr = ithTrend->second.GetRunNr(rc);
          profs[rc] = nullptr;
          if (tmpRunNr != -1) {
            profs[rc] = ithTrend->second.GetLGHGTriggRun(ithTrend->second.GetRunNr(rc));
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
            if(p == 7) legend->AddEntry(profs[rc],Form("%d",tmpRunNr),"p");
          }
        }
        if (dummyhist) dummyhist->Draw("axis,same");                
        
        // labeling inside the panels & legend drawing 
        DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-1.2*relSize8P[p], label, false, 0.85*textSizePixel, 43);
        if (isSameVoltage && p == 7){
          DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-(lineBottom+0.5)*relSize8P[p], label2, false, 0.85*textSizePixel, 43);
        }
        
        if (p == 7) legend->Draw();
        if (p ==4 ){
          TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
          TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
          TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
          DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-1.2*relSize8P[p]-1*0.85*relSize8P[p], lab1, false, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-1.2*relSize8P[p]-2*0.85*relSize8P[p], lab2, false, 0.85*textSizePixel, 43);
          DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-1.2*relSize8P[p]-3*0.85*relSize8P[p], lab3, false, 0.85*textSizePixel, 43);
        }
      }
    }
    if (skipped < 8){
      if(detailedPlot) canvas8Panel->SaveAs(nameOutput.Data());
      if (layer == 0) canvas8Panel->Print(Form("%s.pdf[",nameOutputSummary.Data()));
      canvas8Panel->Print(Form("%s.pdf",nameOutputSummary.Data()));
      if (layer == setupT->GetNMaxLayer()) canvas8Panel->Print(Form("%s.pdf]",nameOutputSummary.Data()));
    }
  }
  
#endif

