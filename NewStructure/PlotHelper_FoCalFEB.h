#ifndef PLOTHELPER_FOCALFEB_H
#define PLOTHELPER_FOCALFEB_H

  //*****************************************************************
  // FOCal FEB geometry: 4 rows x 8 columns
  //===========================================================
  //    col 0     col 1     col 2     col 3     col 4     col 5     col 6     col 7
  // row 0  [ 0 ] [ 1 ] [ 2 ] [ 3 ] [ 4 ] [ 5 ] [ 6 ] [ 7 ]
  // row 1  [ 8 ] [ 9 ] [10 ] [11 ] [12 ] [13 ] [14 ] [15 ]
  // row 2  [16 ] [17 ] [18 ] [19 ] [20 ] [21 ] [22 ] [23 ]
  // row 3  [24 ] [25 ] [26 ] [27 ] [28 ] [29 ] [30 ] [31 ]
  //*****************************************************************

  /**
   * Plot trigger primitives for a single FOCal FEB in a 4x8 geometry.
   *
   * This helper mirrors the other layout-specific PlotHelper_* functions and is
   * designed for small, FEB-level displays where each pad corresponds to one
   * FOCal channel. The plotting routine determines the per-channel histograms,
   * applies the trigger window, and saves the final canvas when the board has
   * enough drawable content.
   */
  inline void PlotTriggerPrimFoCalFEB (TCanvas* canvasFEB, TPad** pads,
                                      Double_t* topRCornerX,  Double_t* topRCornerY,
                                      Double_t* relSizeFEB, Int_t textSizePixel,
                                      std::map<int,TileSpectra> spectra,
                                      double avMip, double facLow, double facHigh,
                                      Double_t xPMin, Double_t xPMax, Double_t scaleYMax,
                                      int mod, TString nameOutput, RunInfo currRunInfo){

    Setup* setupT = Setup::GetInstance();
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    int nRow = 4;
    int nCol = 8;
    int skipped = 0;
    bool found = false;
    TH1D* dummyHist = nullptr;

    for (int r = 0; r < nRow; ++r){
      for (int c = 0; c < nCol; ++c){
        int row = 3 - r;
        int tempCellID = setupT->GetCellID(row, c, 0, mod);
        ithSpectra = spectra.find(tempCellID);
        if (ithSpectra == spectra.end()) continue;
        TH1D* tempHist = ithSpectra->second.GetTriggPrim();
        if (tempHist && maxY < FindLargestBin1DHist(tempHist, xPMin, xPMax))
          maxY = FindLargestBin1DHist(tempHist, xPMin, xPMax);
        if (!found && tempHist){
          dummyHist = new TH1D("dummyTempFoCalFEB", "", tempHist->GetNbinsX(),
                              tempHist->GetBinCenter(1)-tempHist->GetBinWidth(1),
                              tempHist->GetBinCenter(tempHist->GetNbinsX())+tempHist->GetBinWidth(tempHist->GetNbinsX()));
          SetStyleHistoTH1ForGraphs(dummyHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(),
                                    0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,
                                    0.9, 1.1, 510, 510, 43, 63);
          dummyHist->SetBinContent(dummyHist->FindBin(0), 0.5);
          dummyHist->SetDirectory(0);
          found = true;
        }
      }
    }

    for (int r = 0; r < nRow; ++r){
      for (int c = 0; c < nCol; ++c){
        canvasFEB->cd();
        int row = 3 - r;
        int tempCellID = setupT->GetCellID(row, c, 0, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->cd();
        pads[p]->SetLogy();
        ithSpectra = spectra.find(tempCellID);

        TString label = Form("row %d col %d", row, c);
        TLatex *labelChannel = new TLatex(topRCornerX[p]-0.045, topRCornerY[p]-1.2*relSizeFEB[p], label);
        SetStyleTLatex(labelChannel, 0.85*textSizePixel, 4, 1, 43, kTRUE, 31);

        if (ithSpectra == spectra.end()){
          skipped++;
          pads[p]->Clear();
          pads[p]->Draw();
          if (dummyHist){
            TH1D* dummyHist2 = (TH1D*)dummyHist->Clone("dummyTemp2FoCalFEB");
            dummyHist2->GetYaxis()->SetRangeUser(0.7, scaleYMax*maxY);
            dummyHist2->GetXaxis()->SetRangeUser(xPMin, xPMax);
            SetStyleHistoTH1ForGraphs(dummyHist2, dummyHist2->GetXaxis()->GetTitle(), dummyHist2->GetYaxis()->GetTitle(),
                                      0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,
                                      0.9, 1.1, 510, 510, 43, 63);
            dummyHist2->Draw("");
          }
          labelChannel->Draw();
          if (p == 31){
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-2.*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeFEB[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-3.*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeFEB[p], 42);
            DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4.*relSizeFEB[p], "Trigger primitives", true, 0.85*relSizeFEB[p], 42);
          }
          continue;
        }

        TH1D* tempHist = ithSpectra->second.GetTriggPrim();
        SetStyleHistoTH1ForGraphs(tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(),
                                  0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,
                                  0.9, 1.1, 510, 510, 43, 63);
        SetMarkerDefaults(tempHist, 20, 1, kBlue+1, kBlue+1, kFALSE);
        tempHist->GetXaxis()->SetRangeUser(xPMin, xPMax);
        tempHist->GetYaxis()->SetRangeUser(0.7, scaleYMax*maxY);

        tempHist->Draw("pe");
        DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel, xPMin, 0, xPMax, maxY);
        tempHist->Draw("same,axis");
        tempHist->Draw("same,pe");

        labelChannel->Draw();

        TBox* triggArea = CreateBox(kBlue-8, avMip*facLow, 0.7, avMip*facHigh, scaleYMax*maxY, 1001);
        triggArea->Draw();
        DrawLines(avMip*facLow, avMip*facLow, 0.7, scaleYMax*maxY, 1, 1, 7);
        DrawLines(avMip*facHigh, avMip*facHigh, 0.7, scaleYMax*maxY, 1, 1, 7);
        tempHist->Draw("same,axis");
        tempHist->Draw("same,pe");

        if (p == 31){
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-2.*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeFEB[p], 42);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-3.*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeFEB[p], 42);
          DrawLatex(topRCornerX[p]-0.045, topRCornerY[p]-4.*relSizeFEB[p], "Trigger primitives", true, 0.85*relSizeFEB[p], 42);
        }
      }
    }

    if (skipped < 8)
      canvasFEB->SaveAs(nameOutput.Data());
  }

  /**
   * Plot noise spectra with fits for a single FOCal FEB in a 4x8 arrangement.
   */
  inline void PlotNoiseWithFitsFoCalFEB (TCanvas* canvasFEB, TPad** pads,
                                        Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSizeFEB,
                                        Int_t textSizePixel, std::map<int,TileSpectra> spectra, int option,
                                        Double_t xPMin, Double_t xPMax, Double_t scaleYMax,
                                        int mod, TString nameOutput, RunInfo currRunInfo){

    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    Setup* setupT = Setup::GetInstance();

    int nRow = 4;
    int nCol = 8;
    int skipped = 0;
    for (int r = 0; r < nRow; ++r){
      for (int c = 0; c < nCol; ++c){
        int tempCellID = setupT->GetCellID(r, c, 0, mod);
        ithSpectra = spectra.find(tempCellID);
        if (ithSpectra == spectra.end()){
          skipped++;
          continue;
        }
        TH1D* tempHist = nullptr;
        if (option == 0) tempHist = ithSpectra->second.GetHG();
        else if (option == 1) tempHist = ithSpectra->second.GetLG();
        else if (option == 2) tempHist = ithSpectra->second.GetTOA();
        else if (option == 3) tempHist = ithSpectra->second.GetTOT();
        if (tempHist && maxY < FindLargestBin1DHist(tempHist, xPMin, xPMax))
          maxY = FindLargestBin1DHist(tempHist, xPMin, xPMax);
      }
    }

    for (int r = 0; r < nRow; ++r){
      for (int c = 0; c < nCol; ++c){
        canvasFEB->cd();
        int tempCellID = setupT->GetCellID(r, c, 0, mod);  // for plotting
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->cd();
        pads[p]->SetLogy();
        ithSpectra = spectra.find(tempCellID);
        if (ithSpectra == spectra.end()){
          pads[p]->Clear();
          pads[p]->Draw();
          if (p == 31){
            DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSizeFEB[p]-1.4*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeFEB[p], 42);
            DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSizeFEB[p]-2.2*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeFEB[p], 42);
          }
          continue;
        }

        TH1D* tempHist = nullptr;
        if (option == 0) tempHist = ithSpectra->second.GetHG();
        else if (option == 1) tempHist = ithSpectra->second.GetLG();
        else if (option == 2) tempHist = ithSpectra->second.GetTOA();
        else if (option == 3) tempHist = ithSpectra->second.GetTOT();

        SetStyleHistoTH1ForGraphs(tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(),
                                  0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,
                                  0.9, 1.1, 510, 510, 43, 63);
        SetMarkerDefaults(tempHist, 20, 1, kBlue+1, kBlue+1, kFALSE);
        tempHist->GetXaxis()->SetRangeUser(xPMin, xPMax);
        tempHist->GetYaxis()->SetRangeUser(0.7, scaleYMax*maxY);

        tempHist->Draw("pe");
        short bctemp = ithSpectra->second.GetCalib()->BadChannel;
        if (bctemp != -64 && bctemp < 3){
          Color_t boxCol = kGray;
          if (bctemp == 1) boxCol = kGray+1;
          else if (bctemp == 0) boxCol = kGray+2;
          TBox* badChannelArea = CreateBox(boxCol, xPMin, 0.7, xPMax, scaleYMax*maxY, 1001);
          badChannelArea->SetFillColorAlpha(boxCol, 0.35);
          badChannelArea->Draw();
          tempHist->Draw("same,axis");
          tempHist->Draw("same,pe");
        }

        TString label = Form("row %d col %d", r, c);
        TLatex *labelChannel = new TLatex(topRCornerX[p]-0.04, topRCornerY[p]-1.2*relSizeFEB[p], label);
        SetStyleTLatex(labelChannel, 0.85*textSizePixel, 4, 1, 43, kTRUE, 31);

        TF1* fit = nullptr;
        if (option == 0) fit = ithSpectra->second.GetBackModel(1);
        else if (option == 1) fit = ithSpectra->second.GetBackModel(0);
        if (fit){
          SetStyleFit(fit, xPMin, xPMax, 7, 7, kBlack);
          fit->Draw("same");
          TLegend* legend = GetAndSetLegend2(topRCornerX[p]-8*relSizeFEB[p], topRCornerY[p]-4*0.85*relSizeFEB[p]-0.4*relSizeFEB[p],
                                            topRCornerX[p]-0.04, topRCornerY[p]-0.6*relSizeFEB[p], 0.85*textSizePixel, 1, label, 43, 0.2);
          legend->AddEntry(fit, "Gauss noise fit", "l");
          legend->AddEntry((TObject*)0, Form("#mu = %2.2f #pm %2.2f", fit->GetParameter(1), fit->GetParError(1)), " ");
          legend->AddEntry((TObject*)0, Form("#sigma = %2.2f #pm %2.2f", fit->GetParameter(2), fit->GetParError(2)), " ");
          legend->Draw();
        } else {
          labelChannel->Draw();
        }

        if (xPMin < -5) DrawLines(0, 0, 0.7, scaleYMax*maxY, 2, kGray+1, 10);
        if (p == 31){
          DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSizeFEB[p]-1.4*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeFEB[p], 42);
          DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-4*0.85*relSizeFEB[p]-2.2*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeFEB[p], 42);
        }
      }
    }

    if (skipped < 32)
      canvasFEB->SaveAs(nameOutput.Data());
  }

  /**
   * Plot one selected spectrum for every FOCal FEB channel.
   */
  inline void PlotSpectraFoCalFEB (TCanvas* canvasFEB, TPad** pads,
                                  Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSizeFEB,
                                  Int_t textSizePixel, std::map<int,TileSpectra> spectra, int option,
                                  Double_t xPMin, Double_t xPMax, Double_t scaleYMax,
                                  int mod, TString nameOutput, RunInfo currRunInfo){

    Setup* setupT = Setup::GetInstance();
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    int nRow = 4;
    int nCol = 8;
    int skipped = 0;

    for (int r = 0; r < nRow; ++r){
      for (int c = 0; c < nCol; ++c){
        int tempCellID = setupT->GetCellID(r, c, 0, mod);
        ithSpectra = spectra.find(tempCellID);
        if (ithSpectra == spectra.end()) continue;
        TH1D* tempHist = nullptr;
        if      (option == 0) tempHist = ithSpectra->second.GetHG();
        else if (option == 1) tempHist = ithSpectra->second.GetLG();
        else if (option == 2) tempHist = ithSpectra->second.GetComb();
        else if (option == 3) tempHist = ithSpectra->second.GetTOA();
        else if (option == 4) tempHist = ithSpectra->second.GetTOT();
        if (tempHist && maxY < FindLargestBin1DHist(tempHist, xPMin, xPMax))
          maxY = FindLargestBin1DHist(tempHist, xPMin, xPMax);
      }
    }

    for (int r = 0; r < nRow; ++r){
      for (int c = 0; c < nCol; ++c){
        canvasFEB->cd();
        int tempCellID = setupT->GetCellID(r, c, 0, mod);
        int p = setupT->GetChannelInLayer(tempCellID);
        pads[p]->Draw();
        pads[p]->cd();
        pads[p]->SetLogy();
        ithSpectra = spectra.find(tempCellID);
        if (ithSpectra == spectra.end()){
          skipped++;
          pads[p]->Clear();
          pads[p]->Draw();
          continue;
        }

        TH1D* tempHist = nullptr;
        if      (option == 0) tempHist = ithSpectra->second.GetHG();
        else if (option == 1) tempHist = ithSpectra->second.GetLG();
        else if (option == 2) tempHist = ithSpectra->second.GetComb();
        else if (option == 3) tempHist = ithSpectra->second.GetTOA();
        else if (option == 4) tempHist = ithSpectra->second.GetTOT();

        SetStyleHistoTH1ForGraphs(tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(),
                                  0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,
                                  0.9, 1.1, 510, 510, 43, 63);
        SetMarkerDefaults(tempHist, 20, 1, kBlue+1, kBlue+1, kFALSE);
        tempHist->GetXaxis()->SetRangeUser(xPMin, xPMax);
        tempHist->GetYaxis()->SetRangeUser(0.7, scaleYMax*maxY);

        tempHist->Draw("pe");
        short bctemp = ithSpectra->second.GetCalib()->BadChannel;
        if (bctemp != -64 && bctemp < 3){
          Color_t boxCol = kGray;
          if (bctemp == 1) boxCol = kGray+1;
          else if (bctemp == 0) boxCol = kGray+2;
          TBox* badChannelArea = CreateBox(boxCol, xPMin, 0.7, xPMax, scaleYMax*maxY, 1001);
          badChannelArea->SetFillColorAlpha(boxCol, 0.35);
          badChannelArea->Draw();
          tempHist->Draw("same,axis");
          tempHist->Draw("same,pe");
        }

        TString label = Form("row %d col %d", r, c);
        TLatex *labelChannel = new TLatex(topRCornerX[p]-0.04, topRCornerY[p]-1.2*relSizeFEB[p], label);
        SetStyleTLatex(labelChannel, 0.85*textSizePixel, 4, 1, 43, kTRUE, 31);
        labelChannel->Draw();

        if (p == 31){
          DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-2.4*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeFEB[p], 42);
          DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-3.6*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeFEB[p], 42);
        }
      }
    }

    if (skipped < 32)
    canvasFEB->SaveAs(nameOutput.Data());
}

/**
 * Plot noise spectra with trigger-selected overlays for a FOCal FEB.
 *
 * This is the FOCal 4x8 equivalent of the MediumTB advanced-noise plotting
 * helper. It reuses the existing FOCal FEB geometry conventions and draws the
 * standard inclusive + trigger-selected noise overlays in each pad.
 */
inline void PlotNoiseAdvWithFitsFoCalFEB (TCanvas* canvasFEB, TPad** pads,
                                         Double_t* topRCornerX, Double_t* topRCornerY, Double_t* relSizeFEB,
                                         Int_t textSizePixel, std::map<int,TileSpectra> spectra,
                                         std::map<int,TileSpectra> spectraTrigg, bool opt,
                                         Double_t xPMin, Double_t xPMax, Double_t scaleYMax,
                                         int mod, TString nameOutput, RunInfo currRunInfo){
  Setup* setupT = Setup::GetInstance();
  Double_t maxY = 0;
  std::map<int, TileSpectra>::iterator ithSpectra;
  std::map<int, TileSpectra>::iterator ithSpectraTrigg;
  int nRow = 4;
  int nCol = 8;
  int skipped = 0;

  for (int r = 0; r < nRow; ++r){
    for (int c = 0; c < nCol; ++c){
      int row = 3 - r;
      int tempCellID = setupT->GetCellID(row, c, 0, mod);
      ithSpectra = spectra.find(tempCellID);
      if (ithSpectra == spectra.end()) continue;
      TH1D* tempHist = (opt ? ithSpectra->second.GetHG() : ithSpectra->second.GetLG());
      if (tempHist && maxY < FindLargestBin1DHist(tempHist, xPMin, xPMax))
        maxY = FindLargestBin1DHist(tempHist, xPMin, xPMax);
    }
  }

  for (int r = 0; r < nRow; ++r){
    for (int c = 0; c < nCol; ++c){
      canvasFEB->cd();
      int row = 3 - r;
      int tempCellID = setupT->GetCellID(row, c, 0, mod);
      int p = setupT->GetChannelInLayer(tempCellID);
      pads[p]->Draw();
      pads[p]->cd();
      pads[p]->SetLogy();

      ithSpectra = spectra.find(tempCellID);
      if (ithSpectra == spectra.end()){
        skipped++;
        pads[p]->Clear();
        pads[p]->Draw();
        continue;
      }

      ithSpectraTrigg = spectraTrigg.find(tempCellID);
      if (ithSpectraTrigg == spectraTrigg.end()){
        skipped++;
        pads[p]->Clear();
        pads[p]->Draw();
        continue;
      }

      TH1D* tempHist = (opt ? ithSpectra->second.GetHG() : ithSpectra->second.GetLG());
      TH1D* tempHistT = (opt ? ithSpectraTrigg->second.GetHG() : ithSpectraTrigg->second.GetLG());
      if (!tempHist) continue;

      SetStyleHistoTH1ForGraphs(tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(),
                                0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,
                                0.9, 1.1, 510, 510, 43, 63);
      SetMarkerDefaults(tempHist, 20, 1, kBlue+1, kBlue+1, kFALSE);
      tempHist->GetXaxis()->SetRangeUser(xPMin, xPMax);
      tempHist->GetYaxis()->SetRangeUser(0.7, scaleYMax*maxY);
      tempHist->Draw("pe");

      if (tempHistT){
        SetMarkerDefaults(tempHistT, 24, 1, kRed+1, kRed+1, kFALSE);
        tempHistT->Draw("same,pe");
      }

      TString label = Form("row %d col %d", row, c);
      TLatex* labelChannel = new TLatex(topRCornerX[p]-0.04, topRCornerY[p]-1.2*relSizeFEB[p], label);
      SetStyleTLatex(labelChannel, 0.85*textSizePixel, 4, 1, 43, kTRUE, 31);
      labelChannel->Draw();

      DrawLines(0, 0, 0.7, scaleYMax*maxY, 2, kGray+1, 10);
      if (p == 31){
        DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-2.4*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeFEB[p], 42);
        DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-3.6*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeFEB[p], 42);
      }
    }
  }

  if (skipped < 32)
    canvasFEB->SaveAs(nameOutput.Data());
}

/**
 * Overlay three spectra per FOCal FEB channel.
 */
inline void Plot3SpectraOverlayFoCalFEB (TCanvas* canvasFEB, TPad** pads,
                                        Double_t* topRCornerX, Double_t* topRCornerY, Double_t* relSizeFEB,
                                        Int_t textSizePixel, std::map<int,TileSpectra> spectra,
                                        std::map<int,TileSpectra> spectraTrigg,
                                        std::map<int,TileSpectra> spectraNoise,
                                        bool opt, Double_t xPMin, Double_t xPMax, Double_t scaleYMax,
                                        int mod, TString nameOutput, RunInfo currRunInfo){
  Setup* setupT = Setup::GetInstance();
  Double_t maxY = 0;
  std::map<int, TileSpectra>::iterator ithSpectra;
  int nRow = 4;
  int nCol = 8;
  int skipped = 0;

  for (int r = 0; r < nRow; ++r){
    for (int c = 0; c < nCol; ++c){
      int row = 3 - r;
      int tempCellID = setupT->GetCellID(row, c, 0, mod);
      ithSpectra = spectra.find(tempCellID);
      if (ithSpectra == spectra.end()) continue;
      TH1D* tempHist = (opt ? ithSpectra->second.GetHG() : ithSpectra->second.GetLG());
      if (tempHist && maxY < FindLargestBin1DHist(tempHist, xPMin, xPMax))
        maxY = FindLargestBin1DHist(tempHist, xPMin, xPMax);
    }
  }

  for (int r = 0; r < nRow; ++r){
    for (int c = 0; c < nCol; ++c){
      canvasFEB->cd();
      int row = 3 - r;
      int tempCellID = setupT->GetCellID(row, c, 0, mod);
      int p = setupT->GetChannelInLayer(tempCellID);
      pads[p]->Draw();
      pads[p]->cd();
      pads[p]->SetLogy();

      ithSpectra = spectra.find(tempCellID);
      if (ithSpectra == spectra.end()){
        skipped++;
        pads[p]->Clear();
        pads[p]->Draw();
        continue;
      }

      TH1D* tempHist = (opt ? ithSpectra->second.GetHG() : ithSpectra->second.GetLG());
      if (!tempHist) continue;

      SetStyleHistoTH1ForGraphs(tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(),
                                0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,
                                0.9, 1.1, 510, 510, 43, 63);
      SetMarkerDefaults(tempHist, 24, 1, kGray+1, kGray+1, kFALSE);
      tempHist->GetXaxis()->SetRangeUser(xPMin, xPMax);
      tempHist->GetYaxis()->SetRangeUser(0.7, scaleYMax*maxY);
      tempHist->Draw("pe");

      std::map<int, TileSpectra>::iterator ithTrigg = spectraTrigg.find(tempCellID);
      if (ithTrigg != spectraTrigg.end()){
        TH1D* histTrig = (opt ? ithTrigg->second.GetHG() : ithTrigg->second.GetLG());
        if (histTrig){
          SetMarkerDefaults(histTrig, 20, 1, kRed+1, kRed+1, kFALSE);
          histTrig->Draw("same,pe");
        }
      }

      std::map<int, TileSpectra>::iterator ithNoise = spectraNoise.find(tempCellID);
      if (ithNoise != spectraNoise.end()){
        TH1D* histNoise = (opt ? ithNoise->second.GetHG() : ithNoise->second.GetLG());
        if (histNoise){
          SetMarkerDefaults(histNoise, 24, 1, kBlue+1, kBlue+1, kFALSE);
          histNoise->Draw("same,pe");
        }
      }

      TString label = Form("row %d col %d", row, c);
      TLatex* labelChannel = new TLatex(topRCornerX[p]-0.04, topRCornerY[p]-1.2*relSizeFEB[p], label);
      SetStyleTLatex(labelChannel, 0.85*textSizePixel, 4, 1, 43, kTRUE, 31);
      labelChannel->Draw();

      DrawLines(0, 0, 0.7, scaleYMax*maxY, 2, kGray+1, 10);
      if (p == 31){
        DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-2.4*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeFEB[p], 42);
        DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-3.6*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeFEB[p], 42);
      }
    }
  }

  if (skipped < 32)
    canvasFEB->SaveAs(nameOutput.Data());
}

/**
 * Correlation plot with fit for a FOCal FEB channel.
 */
inline void PlotCorrWithFitsFoCalFEB (TCanvas* canvasFEB, TPad** pads, Double_t* topRCornerX, Double_t* topRCornerY,
                                     Double_t* relSizeFEB, Int_t textSizePixel, std::map<int,TileSpectra> spectra,
                                     int option, Double_t xPMin, Double_t xPMax, Double_t maxY,
                                     int mod, TString nameOutput, RunInfo currRunInfo){
  Setup* setupT = Setup::GetInstance();
  std::map<int, TileSpectra>::iterator ithSpectra;
  int nRow = 4;
  int nCol = 8;
  int skipped = 0;

  for (int r = 0; r < nRow; ++r){
    for (int c = 0; c < nCol; ++c){
      canvasFEB->cd();
      int tempCellID = setupT->GetCellID(r, c, 0, mod);
      int p = setupT->GetChannelInLayer(tempCellID);
      pads[p]->Draw();
      pads[p]->SetLogy(0);
      pads[p]->cd();

      ithSpectra = spectra.find(tempCellID);
      if (ithSpectra == spectra.end()){
        skipped++;
        pads[p]->Clear();
        pads[p]->Draw();
        continue;
      }

      TProfile* tempProfile = nullptr;
      if (option == 1 || option == 2)
        tempProfile = ithSpectra->second.GetHGLGcorr();
      else
        tempProfile = ithSpectra->second.GetLGHGcorr();
      if (!tempProfile) continue;

      TH1D* dummyhist = new TH1D("dummyhistFoCalFEB", "", tempProfile->GetNbinsX(), tempProfile->GetXaxis()->GetXmin(), tempProfile->GetXaxis()->GetXmax());
      SetStyleHistoTH1ForGraphs(dummyhist, tempProfile->GetXaxis()->GetTitle(), tempProfile->GetYaxis()->GetTitle(),
                                0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,
                                0.9, 1.5, 510, 510, 43, 63);
      Int_t maxX = 3900;
      if (option == 0 || option == 2) maxX = 340;
      dummyhist->GetYaxis()->SetRangeUser(0, maxY);
      dummyhist->GetXaxis()->SetRangeUser(0, maxX);
      dummyhist->Draw("axis");
      DrawCorrectBadChannelBox(ithSpectra->second.GetCalib()->BadChannel, xPMin, 0, xPMax, maxY);
      tempProfile->Draw("pe,same");

      TString label = Form("row %d col %d", r, c);
      TLatex* labelChannel = new TLatex(topRCornerX[p]+0.045, topRCornerY[p]-1.2*relSizeFEB[p], label);
      SetStyleTLatex(labelChannel, 0.85*textSizePixel, 4, 1, 43, kTRUE, 11);

      TF1* fit = nullptr;
      if (option == 1) fit = ithSpectra->second.GetCorrModel(1);
      else if (option == 0) fit = ithSpectra->second.GetCorrModel(0);
      if (fit){
        Double_t rangeFit[2] = {0, 0};
        fit->GetRange(rangeFit[0], rangeFit[1]);
        SetStyleFit(fit, rangeFit[0], rangeFit[1], 7, 3, kRed+3);
        fit->Draw("same");
      } else {
        labelChannel->Draw();
      }

      if (p == 31){
        DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-2.4*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 2), false, 0.85*relSizeFEB[p], 42);
        DrawLatex(topRCornerX[p]+0.045, topRCornerY[p]-3.6*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 3), false, 0.85*relSizeFEB[p], 42);
      }
    }
  }

  if (skipped < 32)
    canvasFEB->SaveAs(nameOutput.Data());
}

/**
 * 2D correlation plot for a FOCal FEB channel.
 */
inline void PlotCorr2DFoCalFEB (TCanvas* canvasFEB, TPad** pads, Double_t* topRCornerX, Double_t* topRCornerY,
                               Double_t* relSizeFEB, Int_t textSizePixel, std::map<int,TileSpectra> spectra,
                               int option, Double_t xPMin, Double_t xPMax, Double_t minY, Double_t maxY,
                               int mod, TString nameOutput, RunInfo currRunInfo, bool noCalib = 0, int triggCh = -1){
  Setup* setupT = Setup::GetInstance();
  std::map<int, TileSpectra>::iterator ithSpectra;
  int nRow = 4;
  int nCol = 8;
  int skipped = 0;

  for (int r = 0; r < nRow; ++r){
    for (int c = 0; c < nCol; ++c){
      canvasFEB->cd();
      int tempCellID = setupT->GetCellID(r, c, 0, mod);
      int p = setupT->GetChannelInLayer(tempCellID);
      pads[p]->Draw();
      pads[p]->SetLogy(0);
      pads[p]->SetLogz(1);
      pads[p]->cd();

      ithSpectra = spectra.find(tempCellID);
      if (ithSpectra == spectra.end()){
        skipped++;
        pads[p]->Clear();
        pads[p]->Draw();
        continue;
      }

      TH2D* temp2D = nullptr;
      TProfile* tempProfile = nullptr;
      if (option == 0){
        tempProfile = ithSpectra->second.GetLGHGcorr();
        temp2D = ithSpectra->second.GetCorr();
      } else if (option == 1){
        temp2D = ithSpectra->second.GetCorr();
      } else if (option == 2){
        tempProfile = ithSpectra->second.GetTOAADC();
        temp2D = ithSpectra->second.GetCorrTOAADC();
      }

      if (temp2D){
        SetStyleHistoTH2ForGraphs(temp2D, temp2D->GetXaxis()->GetTitle(), temp2D->GetYaxis()->GetTitle(),
                                  0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,
                                  0.9, 1.5, 510, 510, 43, 63);
        temp2D->GetYaxis()->SetRangeUser(minY, maxY);
        temp2D->GetXaxis()->SetRangeUser(xPMin, xPMax);
        temp2D->Draw("col");
      } else if (tempProfile){
        TH1D* dummyhist = new TH1D("dummyhistFoCalFEB2D", "", tempProfile->GetNbinsX(), tempProfile->GetXaxis()->GetXmin(), tempProfile->GetXaxis()->GetXmax());
        SetStyleHistoTH1ForGraphs(dummyhist, tempProfile->GetXaxis()->GetTitle(), tempProfile->GetYaxis()->GetTitle(),
                                  0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,
                                  0.9, 1.5, 510, 510, 43, 63);
        dummyhist->GetXaxis()->SetRangeUser(xPMin, xPMax);
        dummyhist->GetYaxis()->SetRangeUser(minY, maxY);
        dummyhist->Draw("axis");
      }

      if (tempProfile)
        tempProfile->Draw("pe,same");

      TString label = Form("row %d col %d", r, c);
      TLatex* labelChannel = new TLatex(topRCornerX[p]-0.04, topRCornerY[p]-1.2*relSizeFEB[p], label);
      SetStyleTLatex(labelChannel, 0.85*textSizePixel, 4, 1, 43, kTRUE, 31);
      labelChannel->Draw();

      if (p == 31){
        DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-2.4*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeFEB[p], 42);
        DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-3.6*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeFEB[p], 42);
      }
    }
  }

  if (skipped < 32)
    canvasFEB->SaveAs(nameOutput.Data());
}

/**
 * Trending plot equivalent for the FOCal FEB.
 */
inline void PlotTrendingFoCalFEB (TCanvas* canvasFEB, TPad** pads, Double_t* topRCornerX, Double_t* topRCornerY,
                                 Double_t* relSizeFEB, Int_t textSizePixel, std::map<int,TileTrend> trending,
                                 int optionTrend, Double_t xPMin, Double_t xPMax, Double_t minY, Double_t maxY,
                                 bool isSameVoltage, double commonVoltage, int mod, TString nameOutput,
                                 TString nameOutputSummary, RunInfo currRunInfo, Int_t detailedPlot = 1){
  Setup* setupT = Setup::GetInstance();
  std::map<int, TileTrend>::iterator ithTrend;
  int nRow = 4;
  int nCol = 8;
  int skipped = 0;

  TH1D* dummyhist = new TH1D("dummyhistFoCalFEBTrend", "", 100, xPMin, xPMax);
  SetStyleHistoTH1ForGraphs(dummyhist, "run", "value", 0.85*textSizePixel, textSizePixel,
                           0.85*textSizePixel, textSizePixel, 0.9, 1.5, 510, 510, 43, 63);
  dummyhist->GetYaxis()->SetRangeUser(minY, maxY);

  for (int r = 0; r < nRow; ++r){
    for (int c = 0; c < nCol; ++c){
      canvasFEB->cd();
      int row = 3 - r;
      int tempCellID = setupT->GetCellID(row, c, 0, mod);
      int p = setupT->GetChannelInLayer(tempCellID);
      pads[p]->Draw();
      pads[p]->cd();
      if (optionTrend == 6)
        pads[p]->SetLogy(1);
      else
        pads[p]->SetLogy(0);

      ithTrend = trending.find(tempCellID);
      if (ithTrend == trending.end()){
        skipped++;
        pads[p]->Clear();
        pads[p]->Draw();
        dummyhist->Draw("axis");
        continue;
      }

      TGraphErrors* tempGraph = ithTrend->second.GetTrendingBasedOnOption(optionTrend);
      if (!tempGraph) continue;
      SetMarkerDefaultsTGraphErr(tempGraph, 20, 1, kBlue+1, kBlue+1);
      dummyhist->Draw("axis");
      tempGraph->Draw("pe,same");

      TString label = Form("row %d col %d", row, c);
      TLatex* labelChannel = new TLatex(topRCornerX[p]-0.04, topRCornerY[p]-1.2*relSizeFEB[p], label);
      SetStyleTLatex(labelChannel, 0.85*textSizePixel, 4, 1, 43, kTRUE, 31);
      labelChannel->Draw();

      if (p == 31){
        DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-2.4*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 2), true, 0.85*relSizeFEB[p], 42);
        DrawLatex(topRCornerX[p]-0.04, topRCornerY[p]-3.6*relSizeFEB[p], GetStringFromRunInfo(currRunInfo, 3), true, 0.85*relSizeFEB[p], 42);
      }
    }
  }

  if (skipped < 32)
    canvasFEB->SaveAs(nameOutput.Data());
}

/**
 * Run-overlay plot equivalent for the FOCal FEB.
 */
inline void PlotRunOverlayFoCalFEB (TCanvas* canvasFEB, TPad** pads, Double_t* topRCornerX, Double_t* topRCornerY,
                                   Double_t* relSizeFEB, Int_t textSizePixel, std::map<int,TileTrend> trending,
                                   int nruns, int optionTrend, Double_t xPMin, Double_t xPMax, int mod,
                                   TString nameOutput, TString nameOutputSummary, RunInfo commonRunInfo,
                                   Int_t detailedPlot = 1){
  Setup* setupT = Setup::GetInstance();
  std::map<int, TileTrend>::iterator ithTrend;
  int nRow = 4;
  int nCol = 8;
  int skipped = 0;

  for (int r = 0; r < nRow; ++r){
    for (int c = 0; c < nCol; ++c){
      canvasFEB->cd();
      int row = 3 - r;
      int tempCellID = setupT->GetCellID(row, c, 0, mod);
      int p = setupT->GetChannelInLayer(tempCellID);
      pads[p]->Draw();
      pads[p]->cd();
      pads[p]->SetLogy(1);

      ithTrend = trending.find(tempCellID);
      if (ithTrend == trending.end()){
        skipped++;
        pads[p]->Clear();
        pads[p]->Draw();
        continue;
      }

      TGraphErrors* tempGraph = ithTrend->second.GetTrendingBasedOnOption(optionTrend);
      if (!tempGraph) continue;
      SetMarkerDefaultsTGraphErr(tempGraph, 20, 1, kBlue+1, kBlue+1);
      tempGraph->GetXaxis()->SetRangeUser(xPMin, xPMax);
      tempGraph->Draw("pe, same");

      TString label = Form("row %d col %d", row, c);
      TLatex* labelChannel = new TLatex(topRCornerX[p]-0.04, topRCornerY[p]-1.2*relSizeFEB[p], label);
      SetStyleTLatex(labelChannel, 0.85*textSizePixel, 4, 1, 43, kTRUE, 31);
      labelChannel->Draw();
    }
  }

  if (skipped < 32)
    canvasFEB->SaveAs(nameOutput.Data());
}

/**
 * Profile overlay plot equivalent for the FOCal FEB.
 */
inline void PlotRunOverlayProfileFoCalFEB (TCanvas* canvasFEB, TPad** pads, Double_t* topRCornerX, Double_t* topRCornerY,
                                          Double_t* relSizeFEB, Int_t textSizePixel, std::map<int,TileTrend> trending,
                                          int nruns, int option, Double_t xPMin, Double_t xPMax,
                                          Double_t yPMin, Double_t yPMax, int mod, TString nameOutput,
                                          TString nameOutputSummary, RunInfo commonRunInfo,
                                          Int_t detailedPlot = 1, bool scaleInt = false){
  Setup* setupT = Setup::GetInstance();
  std::map<int, TileTrend>::iterator ithTrend;
  int nRow = 4;
  int nCol = 8;
  int skipped = 0;

  for (int r = 0; r < nRow; ++r){
    for (int c = 0; c < nCol; ++c){
      canvasFEB->cd();
      int row = 3 - r;
      int tempCellID = setupT->GetCellID(row, c, 0, mod);
      int p = setupT->GetChannelInLayer(tempCellID);
      pads[p]->Draw();
      pads[p]->cd();
      pads[p]->SetLogy(0);

      ithTrend = trending.find(tempCellID);
      if (ithTrend == trending.end()){
        skipped++;
        pads[p]->Clear();
        pads[p]->Draw();
        continue;
      }

      TProfile* prof = nullptr;
      if (option == 0) prof = ithTrend->second.GetLGHGTriggRun(ithTrend->second.GetRunNr(0));
      else if (option == 1) prof = ithTrend->second.GetWave1DRun(ithTrend->second.GetRunNr(0));
      if (prof){
        if (scaleInt) prof->Scale(1/prof->Integral());
        prof->GetXaxis()->SetRangeUser(xPMin, xPMax);
        prof->GetYaxis()->SetRangeUser(yPMin, yPMax);
        prof->Draw("pe, same");
      }

      TString label = Form("row %d col %d", row, c);
      TLatex* labelChannel = new TLatex(topRCornerX[p]-0.04, topRCornerY[p]-1.2*relSizeFEB[p], label);
      SetStyleTLatex(labelChannel, 0.85*textSizePixel, 4, 1, 43, kTRUE, 31);
      labelChannel->Draw();
    }
  }

  if (skipped < 32)
    canvasFEB->SaveAs(nameOutput.Data());
}

/**
 * MIP-like spectrum plot with fit, matching the MediumTB wrapper family.
 */
inline void PlotMipWithFitsFoCalFEB (TCanvas* canvasFEB, TPad** pads, Double_t* topRCornerX, Double_t* topRCornerY,
                                     Double_t* relSizeFEB, Int_t textSizePixel, std::map<int,TileSpectra> spectra,
                                     std::map<int,TileSpectra> spectraTrigg, int opt, Double_t xPMin, Double_t xPMax,
                                     Double_t scaleYMax, int mod, TString nameOutput, RunInfo currRunInfo){
  PlotNoiseAdvWithFitsFoCalFEB(canvasFEB, pads, topRCornerX, topRCornerY, relSizeFEB, textSizePixel,
                              spectra, spectraTrigg, opt == 1, xPMin, xPMax, scaleYMax, mod, nameOutput, currRunInfo);
}

#endif
