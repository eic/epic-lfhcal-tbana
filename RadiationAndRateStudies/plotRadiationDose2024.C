#include "TFile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TEllipse.h"
#include "PlottingHeader.h"

//********************************************************************************************************
//********************************************************************************************************
//********************************** MAIN FUNCTION *******************************************************
//********************************************************************************************************
//********************************************************************************************************
void plotRadiationDose2024() {
    StyleSettingsBasics();
    SetPlotStyle();
    
    //*****************************************************************************************************
    // 1. Open the ROOT file
    //https://wiki.bnl.gov/EPIC/index.php?title=Radiation_Doses 
    //*****************************************************************************************************
    TFile* fileDIS = TFile::Open("3D_map_full_detector_and_IR__1_MEQ_NEUTRON__canyonLake_SiPMOnTile_pipes_vacuum__pythia_crossing_angle_10x275_2_28_2024.root");
    if (!fileDIS || fileDIS->IsZombie()) {
        printf("Error: Could not open file.\n");
        return;
    }
    TFile* filePBeamGas = TFile::Open("3D_map_full_detector_and_IR__1_MEQ_NEUTRON__canyonLake_SiPMOnTile_pipes_vacuum__hadronBeamGas_crossing_angle_2752_29_2024.root");
    if (!filePBeamGas || filePBeamGas->IsZombie()) {
        printf("Error: Could not open file.\n");
        return;
    }

    // Get 3D hists
    TH3F* h3DIS = (TH3F*)fileDIS->Get("Norm3D_MEQ_fluence_neutron");
    if (!h3DIS) {
        printf("Error: Could not find the 2D histogram.\n");
        fileDIS->Close();
        return;
    }    
    h3DIS->Sumw2();
    TH3F* h3PBeamGas = (TH3F*)filePBeamGas->Get("Norm3D_MEQ_fluence_neutron");
    if (!h3PBeamGas) {
        printf("Error: Could not find the 2D histogram.\n");
        filePBeamGas->Close();
        return;
    }
    h3PBeamGas->Sumw2();
    
    
    //*****************************************************************************************************
    // Projections for LFHCal region
    //*****************************************************************************************************
    Double_t rangeZLFHCal[9] = {368, 378, 398, 418, 438, 458, 478, 488, 500 };
    Double_t minY = -30;
    Double_t maxY = 30;
    Double_t minX = -30;
    Double_t maxX = 30;
    TH1F* hLFHCalDIS_X[8];
    TH1F* hLFHCalPBeamGas_X[8];
    TH1F* hLFHCalAll_X[8];    
    TH1F* hLFHCalDIS_Y[8];
    TH1F* hLFHCalPBeamGas_Y[8];
    TH1F* hLFHCalAll_Y[8];    

    TH2F* hLFHCalDIS_XY[8];
    TH2F* hLFHCalPBeamGas_XY[8];
    TH2F* hLFHCalAll_XY[8];   
    
    TH1F* hLFHCalAllFirst5years_X[8];
    TH1F* hLFHCalAllFirst15years_X[8];
    TH1F* hLFHCalAllFirst5years_Y[8];
    TH1F* hLFHCalAllFirst15years_Y[8];

    TString outputDir = "RadPlots2024/";
    gSystem->Exec("mkdir -p "+outputDir);
   
    Double_t binWidthX = h3DIS->GetXaxis()->GetBinWidth(10);
    Double_t binWidthY = h3DIS->GetYaxis()->GetBinWidth(10);
    Double_t binWidthZ = h3DIS->GetZaxis()->GetBinWidth(10);
    std::cout << "X:" << binWidthX << "\tY:" << binWidthY << "\tZ:" << binWidthZ << std::endl;

    Double_t lumi5years   = 40; 
    Double_t lumi15years  = 150; 
    // Projections in various ranges
    Double_t max= 0;
    Int_t binXMin = h3DIS->GetXaxis()->FindBin(minX);
    Int_t binXMax = h3DIS->GetXaxis()->FindBin(maxX);
    Int_t binYMin = h3DIS->GetYaxis()->FindBin(minY);
    Int_t binYMax = h3DIS->GetYaxis()->FindBin(maxY);
    Double_t nBinsPrX = binXMax-binXMin;
    Double_t nBinsPrY = binYMax-binYMin;

    for (Int_t b = 0; b< 8; b++){
      // Projections to X direction
      Int_t binZMin = h3DIS->GetZaxis()->FindBin(rangeZLFHCal[b]);
      Int_t binZMax = h3DIS->GetZaxis()->FindBin(rangeZLFHCal[b+1]);
      
      Double_t nBinsPrZ = binZMax-binZMin;
      
      std::cout << "LFHCal segment: " << b << "\n" 
                << Form("--> X: %d - %d, delta %f", binXMin, binXMax, nBinsPrX) << "\n" 
                << Form("--> Y: %d - %d, delta %f", binYMin, binYMax, nBinsPrY) << "\n" 
                << Form("--> Z: %d - %d, delta %f", binZMin, binZMax, nBinsPrZ) << std::endl;
      hLFHCalDIS_X[b]        = (TH1F*)h3DIS->ProjectionX(Form("hLFHCalDIS_X_%i", b), binYMin, binYMax, binZMin, binZMax, "e");
      hLFHCalDIS_X[b]->Scale(1./(nBinsPrY*nBinsPrZ));
      for (Int_t i = 1; i < hLFHCalDIS_X[b]->GetNbinsX()+1; i++)
        hLFHCalDIS_X[b]->SetBinError(i,0.);
      hLFHCalPBeamGas_X[b]   = (TH1F*)h3PBeamGas->ProjectionX(Form("hLFHCalPBeamGas_X_%i", b), binYMin, binYMax, binZMin, binZMax, "e");
      hLFHCalPBeamGas_X[b]->Scale(1./(nBinsPrY*nBinsPrZ));
      for (Int_t i = 1; i < hLFHCalPBeamGas_X[b]->GetNbinsX()+1; i++)
        hLFHCalPBeamGas_X[b]->SetBinError(i,0.);
      hLFHCalAll_X[b]        = (TH1F*)hLFHCalDIS_X[b]->Clone(Form("hLFHCalAll_X_%i", b));
      hLFHCalAll_X[b]->Sumw2();
      hLFHCalAll_X[b]->Add(hLFHCalPBeamGas_X[b]);
      if (max < hLFHCalAll_X[b]->GetMaximum()) max= hLFHCalAll_X[b]->GetMaximum();
      
      hLFHCalAllFirst5years_X[b]        = (TH1F*)hLFHCalAll_X[b]->Clone(Form("hLFHCalAllFirst5_X_%i", b));
      hLFHCalAllFirst5years_X[b]->Scale(lumi5years);
      hLFHCalAllFirst15years_X[b]       = (TH1F*)hLFHCalAll_X[b]->Clone(Form("hLFHCalAllFirst15_X_%i", b));
      hLFHCalAllFirst15years_X[b]->Scale(lumi15years);

      // Projections to Y direction
      hLFHCalDIS_Y[b]        = (TH1F*)h3DIS->ProjectionY(Form("hLFHCalDIS_Y_%i", b), binXMin, binXMax, binZMin, binZMax, "e");
      hLFHCalDIS_Y[b]->Scale(1./(nBinsPrX*nBinsPrZ));
      for (Int_t i = 1; i < hLFHCalDIS_Y[b]->GetNbinsX()+1; i++)
        hLFHCalDIS_Y[b]->SetBinError(i,0.);
      hLFHCalPBeamGas_Y[b]   = (TH1F*)h3PBeamGas->ProjectionY(Form("hLFHCalPBeamGas_Y_%i", b), binXMin, binXMax, binZMin, binZMax, "e");
      hLFHCalPBeamGas_Y[b]->Scale(1./(nBinsPrX*nBinsPrZ));
      for (Int_t i = 1; i < hLFHCalPBeamGas_Y[b]->GetNbinsX()+1; i++)
        hLFHCalPBeamGas_Y[b]->SetBinError(i,0.);
      hLFHCalAll_Y[b]        = (TH1F*)hLFHCalDIS_Y[b]->Clone(Form("hLFHCalAll_Y_%i", b));
      hLFHCalAll_Y[b]->Sumw2();
      hLFHCalAll_Y[b]->Add(hLFHCalPBeamGas_Y[b]);
      
      hLFHCalAllFirst5years_Y[b]        = (TH1F*)hLFHCalAll_Y[b]->Clone(Form("hLFHCalAllFirst5_Y_%i", b));
      hLFHCalAllFirst5years_Y[b]->Scale(lumi5years);
      hLFHCalAllFirst15years_Y[b]       = (TH1F*)hLFHCalAll_Y[b]->Clone(Form("hLFHCalAllFirst15_Y_%i", b));
      hLFHCalAllFirst15years_Y[b]->Scale(lumi15years);
      
      // Projections to XY direction
      h3DIS->GetZaxis()->SetRange(h3DIS->GetZaxis()->FindBin(rangeZLFHCal[b]), h3DIS->GetZaxis()->FindBin(rangeZLFHCal[b+1]));
      hLFHCalDIS_XY[b]        = (TH2F*)h3DIS->Project3D("yxe");
      hLFHCalDIS_XY[b]->SetName(Form("hLFHCalDIS_XY%i", b));
      // hLFHCalDIS_XY[b]->Sumw2();
      hLFHCalDIS_XY[b]->Scale(1./(nBinsPrZ));

      h3PBeamGas->GetZaxis()->SetRange(h3PBeamGas->GetZaxis()->FindBin(rangeZLFHCal[b]), h3PBeamGas->GetZaxis()->FindBin(rangeZLFHCal[b+1]));
      hLFHCalPBeamGas_XY[b]        = (TH2F*)h3PBeamGas->Project3D("yxe");
      hLFHCalPBeamGas_XY[b]->SetName(Form("hLFHCalPBeamGas_XY%i", b));
      // hLFHCalPBeamGas_XY[b]->Sumw2();
      hLFHCalPBeamGas_XY[b]->Scale(1./(nBinsPrZ));
      
      hLFHCalAll_XY[b]        = (TH2F*)hLFHCalDIS_XY[b]->Clone(Form("hLFHCalAll_XY_%i", b));
      hLFHCalAll_XY[b]->Sumw2();
      hLFHCalAll_XY[b]->Add(hLFHCalPBeamGas_XY[b]);
    }
    
    // Projections to ZX direction
    h3DIS->GetZaxis()->SetRange(h3DIS->GetZaxis()->FindBin(rangeZLFHCal[0]-15), h3DIS->GetZaxis()->FindBin(rangeZLFHCal[7]+15));
    h3DIS->GetYaxis()->SetRange(h3DIS->GetYaxis()->FindBin(-1.5),h3DIS->GetYaxis()->FindBin(1.5));
    TH2F* hLFHCalDIS_ZX        = (TH2F*)h3DIS->Project3D("xze");
    hLFHCalDIS_ZX->SetName("hLFHCalDIS_ZX");
    hLFHCalDIS_ZX->Scale(1./2);
    

    h3PBeamGas->GetZaxis()->SetRange(h3PBeamGas->GetZaxis()->FindBin(rangeZLFHCal[0]-15), h3PBeamGas->GetZaxis()->FindBin(rangeZLFHCal[8]+15));
    h3PBeamGas->GetYaxis()->SetRange(h3PBeamGas->GetYaxis()->FindBin(-1.5),h3PBeamGas->GetYaxis()->FindBin(1.5));
    TH2F* hLFHCalPBeamGas_ZX        = (TH2F*)h3PBeamGas->Project3D("xze");
    hLFHCalPBeamGas_ZX->SetName("hLFHCalPBeamGas_ZX");
    hLFHCalPBeamGas_ZX->Scale(1./2);
    
    TH2F* hLFHCalAll_ZX        = (TH2F*)hLFHCalDIS_ZX->Clone("hLFHCalAll_ZX");
    hLFHCalAll_ZX->Sumw2();
    hLFHCalAll_ZX->Add(hLFHCalPBeamGas_ZX);

    // Unzoom again in Y
    h3DIS->GetYaxis()->SetRange(1,h3DIS->GetNbinsY());
    h3PBeamGas->GetYaxis()->SetRange(1,h3PBeamGas->GetNbinsY());
    
    // Unzoom again in Z
    h3DIS->GetZaxis()->SetRange(1, h3PBeamGas->GetNbinsZ());
    h3PBeamGas->GetZaxis()->SetRange(1, h3PBeamGas->GetNbinsZ());
    
    Double_t textSizeRel = 0.035;
    Double_t min = hLFHCalPBeamGas_X[0]->GetMinimum()/3;
    
    hLFHCalDIS_X[0]->GetYaxis()->SetRangeUser(min, max*2);
    hLFHCalPBeamGas_X[0]->GetYaxis()->SetRangeUser(min, max*2);
    hLFHCalAll_X[0]->GetYaxis()->SetRangeUser(min, max*2);
    hLFHCalAllFirst5years_X[0]->GetYaxis()->SetRangeUser(min*lumi5years, max*2*lumi5years);
    hLFHCalAllFirst15years_X[0]->GetYaxis()->SetRangeUser(min*lumi15years, max*2*lumi15years);

    hLFHCalDIS_Y[0]->GetYaxis()->SetRangeUser(min, max*2);
    hLFHCalPBeamGas_Y[0]->GetYaxis()->SetRangeUser(min, max*2);
    hLFHCalAll_Y[0]->GetYaxis()->SetRangeUser(min, max*2);
    hLFHCalAllFirst5years_Y[0]->GetYaxis()->SetRangeUser(min*lumi5years, max*2*lumi5years);
    hLFHCalAllFirst15years_Y[0]->GetYaxis()->SetRangeUser(min*lumi15years, max*2*lumi15years);
    

    //*****************************************************************************************************
    // Draw projections for LFHCal region
    //*****************************************************************************************************
    TCanvas* c1 = new TCanvas("c1", "Projection Canvas", 800, 600);
    DrawCanvasSettings(c1, 0.07, 0.02, 0.01, 0.07);
    
    TLegend* legend = GetAndSetLegend2( 0.72, 0.8, 0.95, 0.96,textSizeRel*0.85, 2, "", 42,0.2);
    
    Style_t markerStyle[10] = { 20, 21, 24, 25, 27, 28, 29, 33, 34, 43};
    Color_t color[10]       = { kBlack, kViolet+4, kBlue-3, kCyan+1, kGreen+1, 
                                kYellow-4, kOrange, kRed-4, kPink-5, kMagenta+2 };
    TString labelLayer[8]   = {"seg. 1", "seg. 2", "seg. 3", "seg. 4", "seg. 5", "seg. 6", "seg. 7", "electronics" };
     
    //==============================================================================
    // Projections X
    //==============================================================================
    for (Int_t b = 0; b< 8; b++){
      SetMarkerDefaults(hLFHCalDIS_X[b], "X (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalDIS_X[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hLFHCalPBeamGas_X[b], "X (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalPBeamGas_X[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hLFHCalAll_X[b], "X (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalAll_X[b]->GetXaxis()->SetRangeUser(-260,260);

      SetMarkerDefaults(hLFHCalAllFirst5years_X[b], "X (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalAllFirst5years_X[b]->GetXaxis()->SetRangeUser(-260,260);

      SetMarkerDefaults(hLFHCalAllFirst15years_X[b], "X (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalAllFirst15years_X[b]->GetXaxis()->SetRangeUser(-260,260);
      
      legend->AddEntry(hLFHCalDIS_X[b],labelLayer[b].Data(), "p");
    }

    // DIS radiation load alone
    c1->SetLogy();
    c1->cd();
      for (Int_t b = 0; b< 8; b++){
        if (b == 0) hLFHCalDIS_X[b]->Draw("p");
        else hLFHCalDIS_X[b]->Draw("same,p");
      }
      DrawLabelsAndLinesLFHCal1D(min, max*2, textSizeRel, 1,(rangeZLFHCal[0]+rangeZLFHCal[8])/2);
      legend->SetHeader("DIS e-p");
      legend->Draw();
    c1->SaveAs(Form("%sLFHCal_X_DIS.pdf",outputDir.Data()));
    
    // p-Had gas radiation load alone
    c1->cd();
      for (Int_t b = 0; b< 8; b++){
        if (b == 0) hLFHCalPBeamGas_X[b]->Draw("p");
        else hLFHCalPBeamGas_X[b]->Draw("same,p");
      }
      DrawLabelsAndLinesLFHCal1D(min, max*2, textSizeRel, 1,(rangeZLFHCal[0]+rangeZLFHCal[8])/2);
      legend->SetHeader("p beam gas");
      legend->Draw();
    c1->SaveAs(Form("%sLFHCal_X_PBeamGas.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load
    c1->cd();
      for (Int_t b = 0; b< 8; b++){// Projections to XY direction
        if (b == 0) hLFHCalAll_X[b]->Draw("p");
        else hLFHCalAll_X[b]->Draw("same,p");
      }
      DrawLabelsAndLinesLFHCal1D(min, max*2, textSizeRel,  1,(rangeZLFHCal[0]+rangeZLFHCal[8])/2);
      legend->SetHeader("DIS e-p + p beam gas");
      legend->Draw();
    c1->SaveAs(Form("%sLFHCal_X_All.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load 5 years
    c1->cd();
      for (Int_t b = 0; b< 8; b++){
        if (b == 0) hLFHCalAllFirst5years_X[b]->Draw("p");
        else hLFHCalAllFirst5years_X[b]->Draw("same,p");
      }
      DrawLabelsAndLinesLFHCal1D(min*lumi5years, max*lumi5years*2, textSizeRel, lumi5years,(rangeZLFHCal[0]+rangeZLFHCal[8])/2);
      legend->SetHeader("DIS e-p + p beam gas");
      legend->Draw();
    c1->SaveAs(Form("%sLFHCal_X_All_5Years.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load 15 years
    c1->cd();
      for (Int_t b = 0; b< 8; b++){
        if (b == 0) hLFHCalAllFirst15years_X[b]->Draw("p");
        else hLFHCalAllFirst15years_X[b]->Draw("same,p");
      }
      DrawLabelsAndLinesLFHCal1D(min*lumi15years, max*lumi15years*2, textSizeRel, lumi15years,(rangeZLFHCal[0]+rangeZLFHCal[8])/2);
      legend->SetHeader("DIS e-p + p beam gas");
      legend->Draw();
    c1->SaveAs(Form("%sLFHCal_X_All_15Years.pdf",outputDir.Data()));

    //==============================================================================
    // Projections Y
    //==============================================================================
    for (Int_t b = 0; b< 8; b++){
      SetMarkerDefaults(hLFHCalDIS_Y[b], "Y (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalDIS_Y[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hLFHCalPBeamGas_Y[b], "Y (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalPBeamGas_Y[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hLFHCalAll_Y[b], "Y (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalAll_Y[b]->GetXaxis()->SetRangeUser(-260,260);

      SetMarkerDefaults(hLFHCalAllFirst5years_Y[b], "Y (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalAllFirst5years_Y[b]->GetXaxis()->SetRangeUser(-260,260);

      SetMarkerDefaults(hLFHCalAllFirst15years_Y[b], "Y (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalAllFirst15years_Y[b]->GetXaxis()->SetRangeUser(-260,260);
    }

    // DIS radiation load alone
    c1->SetLogy();
    c1->cd();
      for (Int_t b = 0; b< 8; b++){
        if (b == 0) hLFHCalDIS_Y[b]->Draw("p");
        else hLFHCalDIS_Y[b]->Draw("same,p");
      }
      DrawLabelsAndLinesLFHCal1D(min, max, textSizeRel, 1, (rangeZLFHCal[0]+rangeZLFHCal[8])/2, 2);
      legend->SetHeader("DIS e-p");
      legend->Draw();
    c1->SaveAs(Form("%sLFHCal_Y_DIS.pdf",outputDir.Data()));
    
    // p-Had gas radiation load alone
    c1->cd();
      for (Int_t b = 0; b< 8; b++){
        if (b == 0) hLFHCalPBeamGas_Y[b]->Draw("p");
        else hLFHCalPBeamGas_Y[b]->Draw("same,p");
      }
      DrawLabelsAndLinesLFHCal1D(min, max, textSizeRel, 1, (rangeZLFHCal[0]+rangeZLFHCal[8])/2, 2);
      legend->SetHeader("p beam gas");
      legend->Draw();
    c1->SaveAs(Form("%sLFHCal_Y_PBeamGas.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load
    c1->cd();
      for (Int_t b = 0; b< 8; b++){
        if (b == 0) hLFHCalAll_Y[b]->Draw("p");
        else hLFHCalAll_Y[b]->Draw("same,p");
      }
      DrawLabelsAndLinesLFHCal1D(min, max, textSizeRel, 1, (rangeZLFHCal[0]+rangeZLFHCal[8])/2, 2);
      legend->SetHeader("DIS e-p + p beam gas");
      legend->Draw();
    c1->SaveAs(Form("%sLFHCal_Y_All.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load 5 years
    c1->cd();
      for (Int_t b = 0; b< 8; b++){
        if (b == 0) hLFHCalAllFirst5years_Y[b]->Draw("p");
        else hLFHCalAllFirst5years_Y[b]->Draw("same,p");
      }
      DrawLabelsAndLinesLFHCal1D(min*lumi5years, max*lumi5years, textSizeRel, lumi5years, (rangeZLFHCal[0]+rangeZLFHCal[8])/2, 2);
      legend->SetHeader("DIS e-p + p beam gas");
      legend->Draw();
    c1->SaveAs(Form("%sLFHCal_Y_All_5Years.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load 15 years
    c1->cd();
      for (Int_t b = 0; b< 8; b++){
        if (b == 0) hLFHCalAllFirst15years_Y[b]->Draw("p");
        else hLFHCalAllFirst15years_Y[b]->Draw("same,p");
      }
      DrawLabelsAndLinesLFHCal1D(min*lumi15years, max*lumi15years, textSizeRel, lumi15years, (rangeZLFHCal[0]+rangeZLFHCal[8])/2, 2);
      legend->SetHeader("DIS e-p + p beam gas");
      legend->Draw();
    c1->SaveAs(Form("%sLFHCal_Y_All_15Years.pdf",outputDir.Data()));
    

    //==============================================================================
    // Projections X-Y
    //==============================================================================
    TCanvas* c2 = new TCanvas("c2", "Projection Canvas", 700, 600);
    DrawCanvasSettings(c2, 0.07, 0.12, 0.01, 0.07);
    
    for (Int_t b = 0; b< 8; b++){
      // DIS radiation radiation load alone
      c2->cd();
      c2->SetLogz();
        hLFHCalDIS_XY[b]->GetZaxis()->SetRangeUser(min,max);
        SetStyleHistoTH3ForGraphs( hLFHCalDIS_XY[b], "X (cm)", "Y(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
        hLFHCalDIS_XY[b]->Draw("colz");       
        DrawLatex(0.1, 0.88, Form("DIS e-p, %.0f fb^{-1}",1.), false,0.85*textSizeRel, 62,0.05, 1);
        DrawLatex(0.1, 0.92,Form("#it{LFHCal} region, %s",labelLayer[b].Data()), false,textSizeRel, 62,0.05, 1);
      c2->SaveAs(Form("%sLFHCal_XY_DIS_%i.pdf",outputDir.Data(),b));
      
      // p-Had gas radiation load alone
        hLFHCalPBeamGas_XY[b]->GetZaxis()->SetRangeUser(min,max);
        SetStyleHistoTH3ForGraphs( hLFHCalPBeamGas_XY[b], "X (cm)", "Y(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
        hLFHCalPBeamGas_XY[b]->Draw("colz");       
        DrawLatex(0.1, 0.88, Form("p beam gas e-p, %.0f fb^{-1}",1.), false,0.85*textSizeRel, 62,0.05, 1);
        DrawLatex(0.1, 0.92,Form("#it{LFHCal} region, %s",labelLayer[b].Data()), false,textSizeRel, 62,0.05, 1);
      c2->SaveAs(Form("%sLFHCal_XY_PBeamGas_%i.pdf",outputDir.Data(),b));

      // DIS radiation + p-Had gas radiation load
        hLFHCalAll_XY[b]->GetZaxis()->SetRangeUser(min,max);
        SetStyleHistoTH3ForGraphs( hLFHCalAll_XY[b], "X (cm)", "Y(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
        hLFHCalAll_XY[b]->Draw("colz");       
        DrawLatex(0.1, 0.88, Form("DIS e-p + p beam gas e-p, %.0f fb^{-1}",1.), false,0.85*textSizeRel, 62,0.05, 1);
        DrawLatex(0.1, 0.92,Form("#it{LFHCal} region, %s",labelLayer[b].Data()), false,textSizeRel, 62,0.05, 1);
      c2->SaveAs(Form("%sLFHCal_XY_All_%i.pdf",outputDir.Data(),b));
    }
    
    // DIS radiation radiation load alone
    c2->cd();
    c2->SetLogz();
      hLFHCalDIS_ZX->GetZaxis()->SetRangeUser(min,max);
      SetStyleHistoTH3ForGraphs( hLFHCalDIS_ZX, "Z (cm)", "X(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hLFHCalDIS_ZX->Draw("colz");       
      DrawLatex(0.77, 0.90, Form("DIS e-p, %.0f fb^{-1}",1.), true,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.77, 0.94,Form("#it{LFHCal} region, %.1f < y < %.1f",-1.5,1.5), true,textSizeRel, 62,0.05, 1);
      DrawLabelsAndLinesLFHCal2D(rangeZLFHCal,labelLayer, textSizeRel );
    c2->SaveAs(Form("%sLFHCal_ZX_DIS.pdf",outputDir.Data()));  
    
    // p-Had gas radiation load alone
    c2->cd();
    c2->SetLogz();
      hLFHCalPBeamGas_ZX->GetZaxis()->SetRangeUser(min,max);
      SetStyleHistoTH3ForGraphs( hLFHCalPBeamGas_ZX, "Z (cm)", "X(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hLFHCalPBeamGas_ZX->Draw("colz");       
      DrawLatex(0.77, 0.90, Form("p beam gas e-p, %.0f fb^{-1}",1.), true,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.77, 0.94,Form("#it{LFHCal} region, %.1f < y < %.1f",-1.5,1.5), true,textSizeRel, 62,0.05, 1);
      DrawLabelsAndLinesLFHCal2D(rangeZLFHCal,labelLayer, textSizeRel );
    c2->SaveAs(Form("%sLFHCal_ZX_PBeamGas.pdf",outputDir.Data()));  
    
    // DIS radiation + p-Had gas radiation load
    c2->cd();
    c2->SetLogz();
      hLFHCalAll_ZX->GetZaxis()->SetRangeUser(min,max);
      SetStyleHistoTH3ForGraphs( hLFHCalAll_ZX, "Z (cm)", "X(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hLFHCalAll_ZX->Draw("colz");       
      DrawLatex(0.77, 0.90, Form("DIS e-p + p beam gas e-p, %.0f fb^{-1}",1.), true,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.77, 0.94,Form("#it{LFHCal} region, %.1f < y (cm) < %.1f",-1.5,1.5), true,textSizeRel, 62,0.05, 1);
      DrawLabelsAndLinesLFHCal2D(rangeZLFHCal,labelLayer, textSizeRel );
    c2->SaveAs(Form("%sLFHCal_ZX_All.pdf",outputDir.Data()));  
    
    
    //*****************************************************************************************************
    // Projections for LFHCal region
    //*****************************************************************************************************
    
    Double_t rangeRBHCal[5] = {180, 220, 240, 245, 250  };
    TH1F* hBHCalDIS[4];
    TH1F* hBHCalPBeamGas[4];
    TH1F* hBHCalAll[4];
    TH1F* hBHCalAllFirst5years[4];
    TH1F* hBHCalAllFirst15years[4];

    // Projections in various ranges
    Double_t maxBHcal= max/10.;
    for (Int_t b = 0; b< 4; b++){
      Int_t binRMin = h3DIS->GetXaxis()->FindBin(rangeRBHCal[b]);
      Int_t binRMax = h3DIS->GetXaxis()->FindBin(rangeRBHCal[b+1]);      
      Double_t nBinsPrR = binRMax-binRMin;
      
      hBHCalDIS[b]        = (TH1F*)h3DIS->ProjectionZ(Form("hLFHCalDIS_%i", b),binRMin, binRMax, h3DIS->GetYaxis()->FindBin(-1.5), h3DIS->GetYaxis()->FindBin(1.5), "e");
      hBHCalDIS[b]->Scale(1./nBinsPrR);
      for (Int_t i = 1; i < hBHCalDIS[b]->GetNbinsX()+1; i++)
        hBHCalDIS[b]->SetBinError(i,0.);
      hBHCalPBeamGas[b]   = (TH1F*)h3PBeamGas->ProjectionZ(Form("hBHCalPBeamGas_%i", b),binRMin, binRMax, h3DIS->GetYaxis()->FindBin(-1.5), h3DIS->GetYaxis()->FindBin(1.5), "e");
      hBHCalPBeamGas[b]->Scale(1./nBinsPrR);
      for (Int_t i = 1; i < hBHCalPBeamGas[b]->GetNbinsX()+1; i++)
        hBHCalPBeamGas[b]->SetBinError(i,0.);
      hBHCalAll[b]        = (TH1F*)hBHCalDIS[b]->Clone(Form("hBHCalAll_%i", b));
      hBHCalAll[b]->Sumw2();
      hBHCalAll[b]->Add(hBHCalPBeamGas[b]);
      if (maxBHcal < hBHCalAll[b]->GetMaximum()) maxBHcal= hBHCalAll[b]->GetMaximum();
      
      hBHCalAllFirst5years[b]        = (TH1F*)hBHCalAll[b]->Clone(Form("hBHCalAllFirst5_%i", b));
      hBHCalAllFirst5years[b]->Scale(lumi5years);
      hBHCalAllFirst15years[b]       = (TH1F*)hBHCalAll[b]->Clone(Form("hBHCalAllFirst15_%i", b));
      hBHCalAllFirst15years[b]->Scale(lumi15years);
    }
    
    // Projections to XY direction    
    Int_t binsMinZBHCal = h3DIS->GetZaxis()->FindBin(-327.825);
    Int_t binsMaxZBHCal = h3DIS->GetZaxis()->FindBin(327.825);
    Double_t nBinsBHCal = binsMaxZBHCal-binsMinZBHCal;
    
    h3DIS->GetZaxis()->SetRange(h3DIS->GetZaxis()->FindBin(-327.825), h3DIS->GetZaxis()->FindBin(327.825));
    TH2F* hBHCalDIS_XY        = (TH2F*)h3DIS->Project3D("yxe");
    hBHCalDIS_XY->SetName(Form("hBHCalDIS_XY"));
    hBHCalDIS_XY->Scale(1./nBinsBHCal);
    
    h3PBeamGas->GetZaxis()->SetRange(h3PBeamGas->GetZaxis()->FindBin(-327.825), h3PBeamGas->GetZaxis()->FindBin(327.825));
    TH2F* hBHCalPBeamGas_XY        = (TH2F*)h3PBeamGas->Project3D("yxe");
    hBHCalPBeamGas_XY->SetName(Form("hBHCalDIS_XY"));
    hBHCalPBeamGas_XY->Scale(1./nBinsBHCal);
    
    for (Int_t zb = hBHCalDIS_XY->GetXaxis()->FindBin(-rangeRBHCal[0])+1; zb < hBHCalDIS_XY->GetXaxis()->FindBin(rangeRBHCal[0])-1; zb++  ){
      for (Int_t xb = hBHCalDIS_XY->GetYaxis()->FindBin(-rangeRBHCal[0])+1; xb < hBHCalDIS_XY->GetYaxis()->FindBin(rangeRBHCal[0])-1; xb++  ){
        Double_t r = TMath::Sqrt(TMath::Power(hBHCalDIS_XY->GetXaxis()->GetBinCenter(zb),2)+TMath::Power(hBHCalDIS_XY->GetYaxis()->GetBinCenter(xb),2));
        if (r < rangeRBHCal[0]){
          hBHCalDIS_XY->SetBinContent(zb,xb,0);
          hBHCalPBeamGas_XY->SetBinContent(zb,xb,0);
        }
      }
    }
    TH2F* hBHCalAll_XY        = (TH2F*)hBHCalDIS_XY->Clone("hBHCalAll_XY");
    hBHCalAll_XY->Sumw2();
    hBHCalAll_XY->Add(hBHCalPBeamGas_XY);
    

    // Projections to ZX direction
    h3DIS->GetZaxis()->SetRange(h3DIS->GetZaxis()->FindBin(-327.825-40), h3DIS->GetZaxis()->FindBin(327.825+40));
    h3DIS->GetYaxis()->SetRange(h3DIS->GetYaxis()->FindBin(-1.5),h3DIS->GetYaxis()->FindBin(1.5));
    TH2F* hBHCalDIS_ZX        = (TH2F*)h3DIS->Project3D("xze");
    hBHCalDIS_ZX->SetName("hBHCalDIS_ZX");
    hBHCalDIS_ZX->Scale(1./2);
    for (Int_t zb = hBHCalDIS_ZX->GetXaxis()->FindBin(-327.825-40)-1; zb < hBHCalDIS_ZX->GetXaxis()->FindBin(327.825+40)+1; zb++  ){
      for (Int_t xb = hBHCalDIS_ZX->GetYaxis()->FindBin(-rangeRBHCal[0])+1; xb < hBHCalDIS_ZX->GetYaxis()->FindBin(rangeRBHCal[0]); xb++  ){
          hBHCalDIS_ZX->SetBinContent(zb,xb,0);
      }
    }

    h3PBeamGas->GetZaxis()->SetRange(h3PBeamGas->GetZaxis()->FindBin(-327.825-40), h3PBeamGas->GetZaxis()->FindBin(327.825+40));
    h3PBeamGas->GetYaxis()->SetRange(h3PBeamGas->GetYaxis()->FindBin(-1.5),h3PBeamGas->GetYaxis()->FindBin(1.5));
    TH2F* hBHCalPBeamGas_ZX        = (TH2F*)h3PBeamGas->Project3D("xze");
    hBHCalPBeamGas_ZX->SetName("hBHCalPBeamGas_ZX");
    hBHCalPBeamGas_ZX->Scale(1./2);
    for (Int_t zb = hBHCalPBeamGas_ZX->GetXaxis()->FindBin(-327.825-40)-1; zb < hBHCalPBeamGas_ZX->GetXaxis()->FindBin(327.825+40)+1; zb++  ){
      for (Int_t xb = hBHCalPBeamGas_ZX->GetYaxis()->FindBin(-rangeRBHCal[0])+1; xb < hBHCalPBeamGas_ZX->GetYaxis()->FindBin(rangeRBHCal[0]); xb++  ){
          hBHCalPBeamGas_ZX->SetBinContent(zb,xb,0);
      }
    }
    
    TH2F* hBHCalAll_ZX        = (TH2F*)hBHCalDIS_ZX->Clone("hBHCalAll_ZX");
    hBHCalAll_ZX->Sumw2();
    hBHCalAll_ZX->Add(hBHCalPBeamGas_ZX);

    // Unzoom again in Y
    h3DIS->GetYaxis()->SetRange(1,h3DIS->GetNbinsY());
    h3PBeamGas->GetYaxis()->SetRange(1,h3PBeamGas->GetNbinsY());
    
    // Unzoom again in Z
    h3DIS->GetZaxis()->SetRange(1, h3PBeamGas->GetNbinsZ());
    h3PBeamGas->GetZaxis()->SetRange(1, h3PBeamGas->GetNbinsZ());
    
    Double_t minBHCal = hBHCalPBeamGas[4]->GetMinimum();
    hBHCalDIS[0]->GetYaxis()->SetRangeUser(minBHCal, maxBHcal*2);
    hBHCalPBeamGas[0]->GetYaxis()->SetRangeUser(minBHCal, maxBHcal*2);
    hBHCalAll[0]->GetYaxis()->SetRangeUser(minBHCal, maxBHcal*2);
    hBHCalAllFirst5years[0]->GetYaxis()->SetRangeUser(minBHCal*lumi5years, maxBHcal*2*lumi5years);
    hBHCalAllFirst15years[0]->GetYaxis()->SetRangeUser(minBHCal*lumi15years, maxBHcal*2*lumi15years);
    
    TLegend* legendBHCal = GetAndSetLegend2( 0.72, 0.80, 0.95, 0.96,textSizeRel*0.85, 1, "", 42,0.2);
    
    for (Int_t b = 0; b< 4; b++){
      SetMarkerDefaults(hBHCalDIS[b], "Z (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hBHCalDIS[b]->GetXaxis()->SetRangeUser(-350,350);
      SetMarkerDefaults(hBHCalPBeamGas[b], "Z (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hBHCalPBeamGas[b]->GetXaxis()->SetRangeUser(-350,350);
      SetMarkerDefaults(hBHCalAll[b], "Z (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hBHCalAll[b]->GetXaxis()->SetRangeUser(-350,350);

      SetMarkerDefaults(hBHCalAllFirst5years[b], "Z (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hBHCalAllFirst5years[b]->GetXaxis()->SetRangeUser(-350,350);

      SetMarkerDefaults(hBHCalAllFirst15years[b], "Z (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hBHCalAllFirst15years[b]->GetXaxis()->SetRangeUser(-350,350);
      
      legendBHCal->AddEntry(hBHCalDIS[b],Form("%.0f< R (cm) < %.0f", rangeRBHCal[b], rangeRBHCal[b+1]), "p");
    }

    c1->SetLogy();
    c1->cd();
    for (Int_t b = 0; b< 4; b++){
      if (b == 0) hBHCalDIS[b]->Draw("pe");
      else hBHCalDIS[b]->Draw("same,pe");
      
    }
    legendBHCal->SetHeader("DIS e-p");
    legendBHCal->Draw();
    DrawLines(-328, -328, minBHCal,  minBHCal*4, 2, kGray+1, 7);
    DrawLines(328, 328, minBHCal,  minBHCal*4, 2, kGray+1, 7);
    DrawLatex(0.12, 0.92, "#it{BHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    c1->SaveAs(Form("%sBHCal_MEQ_DIS.pdf",outputDir.Data()));
    
    c1->cd();
    for (Int_t b = 0; b< 4; b++){
      if (b == 0) hBHCalPBeamGas[b]->Draw("pe");
      else hBHCalPBeamGas[b]->Draw("same,pe");
    }
    legendBHCal->SetHeader("p beam gas");
    legendBHCal->Draw();
    DrawLines(-328, -328, minBHCal,  minBHCal*4, 2, kGray+1, 7);
    DrawLines(328, 328, minBHCal,  minBHCal*4, 2, kGray+1, 7);
    DrawLatex(0.12, 0.92, "#it{BHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs(Form("%sBHCal_MEQ_PBeamGas.pdf",outputDir.Data()));
    
    
    c1->cd();
    for (Int_t b = 0; b< 4; b++){
      if (b == 0) hBHCalAll[b]->Draw("pe");
      else hBHCalAll[b]->Draw("same,pe");
    }
    legendBHCal->SetHeader("DIS e-p + p beam gas");
    legendBHCal->Draw();
    DrawLatex(0.12, 0.92, "#it{BHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    DrawLines(-328, -328, minBHCal,  minBHCal*4, 2, kGray+1, 7);
    DrawLines(328, 328, minBHCal,  minBHCal*4, 2, kGray+1, 7);
    
    c1->SaveAs(Form("%sBHCal_MEQ_All.pdf",outputDir.Data()));
    
    
    c1->cd();
    for (Int_t b = 0; b< 4; b++){
      if (b == 0) hBHCalAllFirst5years[b]->Draw("pe");
      else hBHCalAllFirst5years[b]->Draw("same,pe");
    }
    legendBHCal->SetHeader("DIS e-p + p beam gas");
    legendBHCal->Draw();
    DrawLines(-328, -328, minBHCal*lumi5years,  minBHCal*4*lumi5years, 2, kGray+1, 7);
    DrawLines(328, 328, minBHCal*lumi5years,  minBHCal*4*lumi5years, 2, kGray+1, 7);
    DrawLatex(0.12, 0.92, Form("#it{BHCal} region, %.0f fb^{-1}", lumi5years), false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs(Form("%sBHCal_MEQ_All_5Years.pdf",outputDir.Data()));
    
    c1->cd();
    for (Int_t b = 0; b< 4; b++){
      if (b == 0) hBHCalAllFirst15years[b]->Draw("pe");
      else hBHCalAllFirst15years[b]->Draw("same,pe");
    }
    legendBHCal->SetHeader("DIS e-p + p beam gas");
    legendBHCal->Draw();    
    DrawLines(-328, -328, minBHCal*lumi15years,  minBHCal*4*lumi15years, 2, kGray+1, 7);
    DrawLines(328, 328, minBHCal*lumi15years,  minBHCal*4*lumi15years, 2, kGray+1, 7);
    DrawLatex(0.12, 0.92, Form("#it{BHCal} region, %.0f fb^{-1}", lumi15years), false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs(Form("%sBHCal_MEQ_All_15Years.pdf",outputDir.Data()));
    
    
        //==============================================================================
    // Projections X-Y
    //==============================================================================    
    // DIS radiation radiation load alone
    c2->cd();
    c2->SetLogz();
      hBHCalDIS_XY->GetZaxis()->SetRangeUser(minBHCal,maxBHcal);
      SetStyleHistoTH3ForGraphs( hBHCalDIS_XY, "X (cm)", "Y(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hBHCalDIS_XY->Draw("colz");       
      DrawLatex(0.1, 0.88, Form("DIS e-p, %.0f fb^{-1}",1.), false,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.1, 0.92,Form("#it{BHCal} region"), false,textSizeRel, 62,0.05, 1);
    c2->SaveAs(Form("%sBHCal_XY_DIS.pdf",outputDir.Data()));
    
    // p-Had gas radiation load alone
      hBHCalPBeamGas_XY->GetZaxis()->SetRangeUser(minBHCal,maxBHcal);
      SetStyleHistoTH3ForGraphs( hBHCalPBeamGas_XY, "X (cm)", "Y(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hBHCalPBeamGas_XY->Draw("colz");       
      DrawLatex(0.1, 0.88, Form("p beam gas e-p, %.0f fb^{-1}",1.), false,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.1, 0.92,Form("#it{BHCal} region"), false,textSizeRel, 62,0.05, 1);
    c2->SaveAs(Form("%sBHCal_XY_PBeamGas.pdf",outputDir.Data()));

    // DIS radiation + p-Had gas radiation load
      hBHCalAll_XY->GetZaxis()->SetRangeUser(minBHCal,maxBHcal);
      SetStyleHistoTH3ForGraphs( hBHCalAll_XY, "X (cm)", "Y(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hBHCalAll_XY->Draw("colz");       
      DrawLatex(0.1, 0.88, Form("DIS e-p + p beam gas e-p, %.0f fb^{-1}",1.), false,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.1, 0.92,Form("#it{BHCal} region"), false,textSizeRel, 62,0.05, 1);
      
      TEllipse* el1 = new TEllipse(0,0,rangeRBHCal[0],rangeRBHCal[0]);
      el1->SetFillStyle(0);
      el1->SetFillColor(0);
      el1->SetLineStyle(7);
      el1->SetLineWidth(2);
      el1->Draw("same");
      TEllipse* el2 = new TEllipse(0,0,rangeRBHCal[4],rangeRBHCal[4]);
      el2->SetFillStyle(0);
      el2->SetFillColor(0);
      el2->SetLineStyle(7);
      el2->SetLineWidth(2);
      el2->Draw("same");
    c2->SaveAs(Form("%sBHCal_XY_All.pdf",outputDir.Data()));
    
    // DIS radiation radiation load alone
    c2->cd();
    c2->SetLogz();
      hBHCalDIS_ZX->GetZaxis()->SetRangeUser(minBHCal,maxBHcal);
      SetStyleHistoTH3ForGraphs( hBHCalDIS_ZX, "Z (cm)", "X(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hBHCalDIS_ZX->Draw("colz");       
      DrawLatex(0.82, 0.76, Form("DIS e-p, %.0f fb^{-1}",1.), true,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.82, 0.80,Form("#it{BHCal} region, %.1f < y < %.1f",-1.5,1.5), true,textSizeRel, 62,0.05, 1);
      DrawLabelsAndLinesBHCal2D(rangeRBHCal, textSizeRel );
    c2->SaveAs(Form("%sBHCal_ZX_DIS.pdf",outputDir.Data()));  
    
    // p-Had gas radiation load alone
    c2->cd();
    c2->SetLogz();
      hBHCalPBeamGas_ZX->GetZaxis()->SetRangeUser(minBHCal,maxBHcal);
      SetStyleHistoTH3ForGraphs( hBHCalPBeamGas_ZX, "Z (cm)", "X(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hBHCalPBeamGas_ZX->Draw("colz");       
      DrawLatex(0.82, 0.76, Form("p beam gas e-p, %.0f fb^{-1}",1.), true,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.82, 0.80,Form("#it{BHCal} region, %.1f < y < %.1f",-1.5,1.5), true,textSizeRel, 62,0.05, 1);
      DrawLabelsAndLinesBHCal2D(rangeRBHCal, textSizeRel );
    c2->SaveAs(Form("%sBHCal_ZX_PBeamGas.pdf",outputDir.Data()));  
    
    // DIS radiation + p-Had gas radiation load
    c2->cd();
    c2->SetLogz();
      hBHCalAll_ZX->GetZaxis()->SetRangeUser(minBHCal,maxBHcal);
      SetStyleHistoTH3ForGraphs( hBHCalAll_ZX, "Z (cm)", "X(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hBHCalAll_ZX->Draw("colz");       
      DrawLatex(0.82, 0.76, Form("DIS e-p + p beam gas e-p, %.0f fb^{-1}",1.), true,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.82, 0.80,Form("#it{BHCal} region, %.1f < y (cm) < %.1f",-1.5,1.5), true,textSizeRel, 62,0.05, 1);
      DrawLabelsAndLinesBHCal2D(rangeRBHCal, textSizeRel );
    c2->SaveAs(Form("%sBHCal_ZX_All.pdf",outputDir.Data()));  


    // *****************************************************************************************************
    // Projections for NHCal region
    // *****************************************************************************************************
    Double_t rangeZNHCal[7] = { 385, 400, 410, 420, 430, 440, 450 };
    TH1F* hNHCalDIS_X[8];
    TH1F* hNHCalPBeamGas_X[8];
    TH1F* hNHCalAll_X[8];
    TH1F* hNHCalDIS_Y[8];
    TH1F* hNHCalPBeamGas_Y[8];
    TH1F* hNHCalAll_Y[8];
    TH1F* hNHCalAllFirst5years_X[8];
    TH1F* hNHCalAllFirst15years_X[8];
    TH1F* hNHCalAllFirst5years_Y[8];
    TH1F* hNHCalAllFirst15years_Y[8];

    TH2F* hNHCalDIS_XY[8];
    TH2F* hNHCalPBeamGas_XY[8];
    TH2F* hNHCalAll_XY[8];   

    // Projections in various ranges
    Double_t maxNHCal= 0;
    for (Int_t b = 0; b< 6; b++){
      Int_t binZMin = h3DIS->GetZaxis()->FindBin(-rangeZNHCal[b+1]);
      Int_t binZMax = h3DIS->GetZaxis()->FindBin(-rangeZNHCal[b]);
      Double_t nBinsPrZ = TMath::Abs(binZMax-binZMin);
      
      std::cout << "NHCal segment: " << b << "\n" 
          << Form("--> X: %d - %d, delta %f", binXMin, binXMax, nBinsPrX) << "\n" 
          << Form("--> Y: %d - %d, delta %f", binYMin, binYMax, nBinsPrY) << "\n" 
          << Form("--> Z: %d - %d, delta %f", binZMin, binZMax, nBinsPrZ) << "\n" 
          << Form("--> Z cm: %f - %f", -rangeZNHCal[b], -rangeZNHCal[b+1])<< "\n" 
          << std::endl;
      
      hNHCalDIS_X[b]        = (TH1F*)h3DIS->ProjectionX(Form("hNHCalDIS_X_%i", b), binYMin, binYMax, binZMin, binZMax, "e");
      hNHCalDIS_X[b]->Scale(1./(nBinsPrY*nBinsPrZ));
      for (Int_t i = 1; i < hNHCalDIS_X[b]->GetNbinsX()+1; i++)
        hNHCalDIS_X[b]->SetBinError(i,0.);
      hNHCalPBeamGas_X[b]   = (TH1F*)h3PBeamGas->ProjectionX(Form("hNHCalPBeamGas_X_%i", b), binYMin, binYMax, binZMin, binZMax, "e");
      hNHCalPBeamGas_X[b]->Scale(1./(nBinsPrY*nBinsPrZ));
      for (Int_t i = 1; i < hNHCalPBeamGas_X[b]->GetNbinsX()+1; i++)
        hNHCalPBeamGas_X[b]->SetBinError(i,0.);
      hNHCalAll_X[b]        = (TH1F*)hNHCalDIS_X[b]->Clone(Form("hNHCalAll_X_%i", b));
      hNHCalAll_X[b]->Sumw2();
      hNHCalAll_X[b]->Add(hNHCalPBeamGas_X[b]);
      if (maxNHCal < hNHCalAll_X[b]->GetMaximum()) maxNHCal= hNHCalAll_X[b]->GetMaximum();
      
      hNHCalAllFirst5years_X[b]        = (TH1F*)hNHCalAll_X[b]->Clone(Form("hNHCalAllFirst5_X_%i", b));
      hNHCalAllFirst5years_X[b]->Scale(lumi5years);
      hNHCalAllFirst15years_X[b]       = (TH1F*)hNHCalAll_X[b]->Clone(Form("hNHCalAllFirst15_X_%i", b));
      hNHCalAllFirst15years_X[b]->Scale(lumi15years);

      // Projections to Y direction
      hNHCalDIS_Y[b]        = (TH1F*)h3DIS->ProjectionY(Form("hNHCalDIS_Y_%i", b), binXMin, binXMax, binZMin, binZMax, "e");
      hNHCalDIS_Y[b]->Scale(1./(nBinsPrX*nBinsPrZ));
      for (Int_t i = 1; i < hNHCalDIS_Y[b]->GetNbinsX()+1; i++)
        hNHCalDIS_Y[b]->SetBinError(i,0.);
      hNHCalPBeamGas_Y[b]   = (TH1F*)h3PBeamGas->ProjectionY(Form("hNHCalPBeamGas_Y_%i", b), binXMin, binXMax, binZMin, binZMax, "e");
      hNHCalPBeamGas_Y[b]->Scale(1./(nBinsPrX*nBinsPrZ));
      for (Int_t i = 1; i < hNHCalPBeamGas_Y[b]->GetNbinsX()+1; i++)
        hNHCalPBeamGas_Y[b]->SetBinError(i,0.);
      hNHCalAll_Y[b]        = (TH1F*)hNHCalDIS_Y[b]->Clone(Form("hNHCalAll_Y_%i", b));
      hNHCalAll_Y[b]->Sumw2();
      hNHCalAll_Y[b]->Add(hNHCalPBeamGas_Y[b]);
      
      hNHCalAllFirst5years_Y[b]        = (TH1F*)hNHCalAll_Y[b]->Clone(Form("hNHCalAllFirst5_Y_%i", b));
      hNHCalAllFirst5years_Y[b]->Scale(lumi5years);
      hNHCalAllFirst15years_Y[b]       = (TH1F*)hNHCalAll_Y[b]->Clone(Form("hNHCalAllFirst15_Y_%i", b));
      hNHCalAllFirst15years_Y[b]->Scale(lumi15years);
      
      // Projections to XY direction
      h3DIS->GetZaxis()->SetRange(h3DIS->GetZaxis()->FindBin(rangeZNHCal[b]), h3DIS->GetZaxis()->FindBin(rangeZNHCal[b+1]));
      hNHCalDIS_XY[b]        = (TH2F*)h3DIS->Project3D("yxe");
      hNHCalDIS_XY[b]->SetName(Form("hNHCalDIS_XY%i", b));
      // hNHCalDIS_XY[b]->Sumw2();
      hNHCalDIS_XY[b]->Scale(1./(nBinsPrZ));

      h3PBeamGas->GetZaxis()->SetRange(h3PBeamGas->GetZaxis()->FindBin(rangeZNHCal[b]), h3PBeamGas->GetZaxis()->FindBin(rangeZNHCal[b+1]));
      hNHCalPBeamGas_XY[b]        = (TH2F*)h3PBeamGas->Project3D("yxe");
      hNHCalPBeamGas_XY[b]->SetName(Form("hNHCalPBeamGas_XY%i", b));
      // hNHCalPBeamGas_XY[b]->Sumw2();
      hNHCalPBeamGas_XY[b]->Scale(1./(nBinsPrZ));
      
      hNHCalAll_XY[b]        = (TH2F*)hNHCalDIS_XY[b]->Clone(Form("hNHCalAll_XY_%i", b));
      hNHCalAll_XY[b]->Sumw2();
      hNHCalAll_XY[b]->Add(hNHCalPBeamGas_XY[b]);
   
    }
    
    Double_t minNHCal = hNHCalPBeamGas_X[0]->GetMinimum()/3;
   
    // Projections to ZX direction
    h3DIS->GetZaxis()->SetRange(h3DIS->GetZaxis()->FindBin(-rangeZNHCal[6]-10), h3DIS->GetZaxis()->FindBin(-rangeZNHCal[0]+10));
    h3DIS->GetYaxis()->SetRange(h3DIS->GetYaxis()->FindBin(-1.5),h3DIS->GetYaxis()->FindBin(1.5));
    TH2F* hNHCalDIS_ZX        = (TH2F*)h3DIS->Project3D("xze");
    hNHCalDIS_ZX->SetName("hNHCalDIS_ZX");
    hNHCalDIS_ZX->Scale(1./2);
    
    h3PBeamGas->GetZaxis()->SetRange(h3PBeamGas->GetZaxis()->FindBin(-rangeZNHCal[6]-10), h3DIS->GetZaxis()->FindBin(-rangeZNHCal[0]+10));
    h3PBeamGas->GetYaxis()->SetRange(h3PBeamGas->GetYaxis()->FindBin(-1.5),h3PBeamGas->GetYaxis()->FindBin(1.5));
    TH2F* hNHCalPBeamGas_ZX        = (TH2F*)h3PBeamGas->Project3D("xze");
    hNHCalPBeamGas_ZX->SetName("hNHCalPBeamGas_ZX");
    hNHCalPBeamGas_ZX->Scale(1./2);
    
    TH2F* hNHCalAll_ZX        = (TH2F*)hNHCalDIS_ZX->Clone("hNHCalAll_ZX");
    hNHCalAll_ZX->Sumw2();
    hNHCalAll_ZX->Add(hNHCalPBeamGas_ZX);

    // Unzoom again in Y
    h3DIS->GetYaxis()->SetRange(1,h3DIS->GetNbinsY());
    h3PBeamGas->GetYaxis()->SetRange(1,h3PBeamGas->GetNbinsY());
    
    // Unzoom again in Z
    h3DIS->GetZaxis()->SetRange(1, h3PBeamGas->GetNbinsZ());
    h3PBeamGas->GetZaxis()->SetRange(1, h3PBeamGas->GetNbinsZ());
    
    
    hNHCalDIS_X[0]->GetYaxis()->SetRangeUser(minNHCal, maxNHCal*4);
    hNHCalPBeamGas_X[0]->GetYaxis()->SetRangeUser(minNHCal, maxNHCal*4);
    hNHCalAll_X[0]->GetYaxis()->SetRangeUser(minNHCal, maxNHCal*4);
    hNHCalAllFirst5years_X[0]->GetYaxis()->SetRangeUser(minNHCal*lumi5years, maxNHCal*4*lumi5years);
    hNHCalAllFirst15years_X[0]->GetYaxis()->SetRangeUser(minNHCal*lumi15years, maxNHCal*4*lumi15years);

    hNHCalDIS_Y[0]->GetYaxis()->SetRangeUser(minNHCal, maxNHCal*4);
    hNHCalPBeamGas_Y[0]->GetYaxis()->SetRangeUser(minNHCal, maxNHCal*4);
    hNHCalAll_Y[0]->GetYaxis()->SetRangeUser(minNHCal, maxNHCal*4);
    hNHCalAllFirst5years_Y[0]->GetYaxis()->SetRangeUser(minNHCal*lumi5years, maxNHCal*4*lumi5years);
    hNHCalAllFirst15years_Y[0]->GetYaxis()->SetRangeUser(minNHCal*lumi15years, maxNHCal*4*lumi15years);
    
    
    //*****************************************************************************************************
    // Draw projections for LFHCal region
    //*****************************************************************************************************
    TString labelLayerNHCal[8]   = {"electronics", "seg. 1", "seg. 2", "seg. 3", "seg. 4", "seg. 5", "seg. 6", "seg. 7"};
    TLegend* legendNHCal = GetAndSetLegend2( 0.72, 0.84, 0.95, 0.96,textSizeRel*0.85, 2, "", 42,0.2);
                            
    //==============================================================================
    // Projections X
    //==============================================================================
    for (Int_t b = 0; b< 6; b++){
      SetMarkerDefaults(hNHCalDIS_X[b], "X (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalDIS_X[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hNHCalPBeamGas_X[b], "X (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalPBeamGas_X[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hNHCalAll_X[b], "X (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalAll_X[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hNHCalAllFirst5years_X[b], "X (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalAllFirst5years_X[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hNHCalAllFirst15years_X[b], "X (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalAllFirst15years_X[b]->GetXaxis()->SetRangeUser(-260,260);
      
      legendNHCal->AddEntry(hNHCalDIS_X[b],labelLayerNHCal[b].Data(), "p");
    }
    
    // DIS radiation radiation load alone
    c1->SetLogy();
    c1->cd();
      for (Int_t b = 0; b< 6; b++){
        if (b == 0) hNHCalDIS_X[b]->Draw("pe");
        else hNHCalDIS_X[b]->Draw("same,pe");
      }
      legendNHCal->SetHeader("DIS e-p");
      legendNHCal->Draw();
      DrawLabelsAndLinesNHCal1D(minNHCal, minNHCal*4, textSizeRel, 1.);
    c1->SaveAs(Form("%sNHCal_X_DIS.pdf",outputDir.Data()));
    
    // p-Had gas radiation load alone
    c1->cd();
      for (Int_t b = 0; b< 6; b++){
        if (b == 0) hNHCalPBeamGas_X[b]->Draw("pe");
        else hNHCalPBeamGas_X[b]->Draw("same,pe");
      }
      legendNHCal->SetHeader("p beam gas");
      legendNHCal->Draw();
      DrawLabelsAndLinesNHCal1D(minNHCal, minNHCal*4, textSizeRel, 1.);
    c1->SaveAs(Form("%sNHCal_X_PBeamGas.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load
    c1->cd();
      for (Int_t b = 0; b< 6; b++){
        if (b == 0) hNHCalAll_X[b]->Draw("pe");
        else hNHCalAll_X[b]->Draw("same,pe");
      }
      legendNHCal->SetHeader("DIS e-p + p beam gas");
      legendNHCal->Draw();
      DrawLabelsAndLinesNHCal1D(minNHCal, minNHCal*4, textSizeRel, 1.);    
    c1->SaveAs(Form("%sNHCal_X_All.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load 5 years
    c1->cd();
      for (Int_t b = 0; b< 6; b++){
        if (b == 0) hNHCalAllFirst5years_X[b]->Draw("pe");
        else hNHCalAllFirst5years_X[b]->Draw("same,pe");
      }
      legendNHCal->SetHeader("DIS e-p + p beam gas");
      legendNHCal->Draw();
      DrawLabelsAndLinesNHCal1D(minNHCal*lumi5years, minNHCal*4*lumi5years, textSizeRel, lumi5years);
    c1->SaveAs(Form("%sNHCal_X_All_5Years.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load 15 years
    c1->cd();
      for (Int_t b = 0; b< 6; b++){
        if (b == 0) hNHCalAllFirst15years_X[b]->Draw("pe");
        else hNHCalAllFirst15years_X[b]->Draw("same,pe");
      }
      legendNHCal->SetHeader("DIS e-p + p beam gas");
      legendNHCal->Draw();
      DrawLabelsAndLinesNHCal1D(minNHCal*lumi15years, minNHCal*4*lumi15years, textSizeRel, lumi5years);
    c1->SaveAs(Form("%sNHCal_X_All_15Years.pdf",outputDir.Data()));
    
    //==============================================================================
    // Projections Y
    //==============================================================================
    for (Int_t b = 0; b< 6; b++){
      SetMarkerDefaults(hNHCalDIS_Y[b], "Y (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalDIS_Y[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hNHCalPBeamGas_Y[b], "Y (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalPBeamGas_Y[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hNHCalAll_Y[b], "Y (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalAll_Y[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hNHCalAllFirst5years_Y[b], "Y (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalAllFirst5years_Y[b]->GetXaxis()->SetRangeUser(-260,260);
      SetMarkerDefaults(hNHCalAllFirst15years_Y[b], "Y (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalAllFirst15years_Y[b]->GetXaxis()->SetRangeUser(-260,260);
    }
    
    // DIS radiation radiation load alone
    c1->SetLogy();
    c1->cd();
      for (Int_t b = 0; b< 6; b++){
        if (b == 0) hNHCalDIS_Y[b]->Draw("pe");
        else hNHCalDIS_Y[b]->Draw("same,pe");
      }
      legendNHCal->SetHeader("DIS e-p");
      legendNHCal->Draw();
      DrawLabelsAndLinesNHCal1D(minNHCal, minNHCal*4, textSizeRel, 1.);
    c1->SaveAs(Form("%sNHCal_Y_DIS.pdf",outputDir.Data()));
    
    // p-Had gas radiation load alone
    c1->cd();
      for (Int_t b = 0; b< 6; b++){
        if (b == 0) hNHCalPBeamGas_Y[b]->Draw("pe");
        else hNHCalPBeamGas_Y[b]->Draw("same,pe");
      }
      legendNHCal->SetHeader("p beam gas");
      legendNHCal->Draw();
      DrawLabelsAndLinesNHCal1D(minNHCal, minNHCal*4, textSizeRel, 1.);
    c1->SaveAs(Form("%sNHCal_Y_PBeamGas.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load
    c1->cd();
      for (Int_t b = 0; b< 6; b++){
        if (b == 0) hNHCalAll_Y[b]->Draw("pe");
        else hNHCalAll_Y[b]->Draw("same,pe");
      }
      legendNHCal->SetHeader("DIS e-p + p beam gas");
      legendNHCal->Draw();
      DrawLabelsAndLinesNHCal1D(minNHCal, minNHCal*4, textSizeRel, 1.);    
    c1->SaveAs(Form("%sNHCal_Y_All.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load 5 years
    c1->cd();
      for (Int_t b = 0; b< 6; b++){
        if (b == 0) hNHCalAllFirst5years_Y[b]->Draw("pe");
        else hNHCalAllFirst5years_Y[b]->Draw("same,pe");
      }
      legendNHCal->SetHeader("DIS e-p + p beam gas");
      legendNHCal->Draw();
      DrawLabelsAndLinesNHCal1D(minNHCal*lumi5years, minNHCal*4*lumi5years, textSizeRel, lumi5years);
    c1->SaveAs(Form("%sNHCal_Y_All_5Years.pdf",outputDir.Data()));
    
    // DIS radiation + p-Had gas radiation load 15 years
    c1->cd();
      for (Int_t b = 0; b< 6; b++){
        if (b == 0) hNHCalAllFirst15years_Y[b]->Draw("pe");
        else hNHCalAllFirst15years_Y[b]->Draw("same,pe");
      }
      legendNHCal->SetHeader("DIS e-p + p beam gas");
      legendNHCal->Draw();
      DrawLabelsAndLinesNHCal1D(minNHCal*lumi15years, minNHCal*4*lumi15years, textSizeRel, lumi5years);
    c1->SaveAs(Form("%sNHCal_Y_All_15Years.pdf",outputDir.Data()));
    
    //==============================================================================
    // Projections X-Y
    //==============================================================================    
    for (Int_t b = 0; b< 6; b++){
      // DIS radiation radiation load alone
      c2->cd();
      c2->SetLogz();
        hNHCalDIS_XY[b]->GetZaxis()->SetRangeUser(minNHCal,maxNHCal);
        SetStyleHistoTH3ForGraphs( hNHCalDIS_XY[b], "X (cm)", "Y(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
        hNHCalDIS_XY[b]->Draw("colz");       
        DrawLatex(0.1, 0.88, Form("DIS e-p, %.0f fb^{-1}",1.), false,0.85*textSizeRel, 62,0.05, 1);
        DrawLatex(0.1, 0.92,Form("#it{NHCal} region, %s",labelLayerNHCal[b].Data()), false,textSizeRel, 62,0.05, 1);
      c2->SaveAs(Form("%sNHCal_XY_DIS_%i.pdf",outputDir.Data(),b));
      
      // p-Had gas radiation load alone
        hNHCalPBeamGas_XY[b]->GetZaxis()->SetRangeUser(minNHCal,maxNHCal);
        SetStyleHistoTH3ForGraphs( hNHCalPBeamGas_XY[b], "X (cm)", "Y(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
        hNHCalPBeamGas_XY[b]->Draw("colz");       
        DrawLatex(0.1, 0.88, Form("p beam gas e-p, %.0f fb^{-1}",1.), false,0.85*textSizeRel, 62,0.05, 1);
        DrawLatex(0.1, 0.92,Form("#it{NHCal} region, %s",labelLayerNHCal[b].Data()), false,textSizeRel, 62,0.05, 1);
      c2->SaveAs(Form("%sNHCal_XY_PBeamGas_%i.pdf",outputDir.Data(),b));

      // DIS radiation + p-Had gas radiation load
        hNHCalAll_XY[b]->GetZaxis()->SetRangeUser(minNHCal,maxNHCal);
        SetStyleHistoTH3ForGraphs( hNHCalAll_XY[b], "X (cm)", "Y(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
        hNHCalAll_XY[b]->Draw("colz");       
        DrawLatex(0.1, 0.88, Form("DIS e-p + p beam gas e-p, %.0f fb^{-1}",1.), false,0.85*textSizeRel, 62,0.05, 1);
        DrawLatex(0.1, 0.92,Form("#it{NHCal} region, %s",labelLayerNHCal[b].Data()), false,textSizeRel, 62,0.05, 1);
      c2->SaveAs(Form("%sNHCal_XY_All_%i.pdf",outputDir.Data(),b));
    }
    
    // DIS radiation radiation load alone
    c2->cd();
    c2->SetLogz();
      hNHCalDIS_ZX->GetZaxis()->SetRangeUser(minNHCal,maxNHCal);
      SetStyleHistoTH3ForGraphs( hNHCalDIS_ZX, "Z (cm)", "X(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hNHCalDIS_ZX->Draw("colz");       
      DrawLatex(0.72, 0.90, Form("DIS e-p, %.0f fb^{-1}",1.), true,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.72, 0.94,Form("#it{NHCal} region, %.1f < y < %.1f",-1.5,1.5), true,textSizeRel, 62,0.05, 1);
      DrawLabelsAndLinesNHCal2D(rangeZNHCal,labelLayerNHCal, textSizeRel );
    c2->SaveAs(Form("%sNHCal_ZX_DIS.pdf",outputDir.Data()));  
    
    // p-Had gas radiation load alone
    c2->cd();
    c2->SetLogz();
      hNHCalPBeamGas_ZX->GetZaxis()->SetRangeUser(minNHCal,maxNHCal);
      SetStyleHistoTH3ForGraphs( hNHCalPBeamGas_ZX, "Z (cm)", "X(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hNHCalPBeamGas_ZX->Draw("colz");       
      DrawLatex(0.72, 0.90, Form("p beam gas e-p, %.0f fb^{-1}",1.), true,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.72, 0.94,Form("#it{NHCal} region, %.1f < y < %.1f",-1.5,1.5), true,textSizeRel, 62,0.05, 1);
      DrawLabelsAndLinesNHCal2D(rangeZNHCal,labelLayerNHCal, textSizeRel );
    c2->SaveAs(Form("%sNHCal_ZX_PBeamGas.pdf",outputDir.Data()));  
    
    // DIS radiation + p-Had gas radiation load
    c2->cd();
    c2->SetLogz();
      hNHCalAll_ZX->GetZaxis()->SetRangeUser(minNHCal,maxNHCal);
      SetStyleHistoTH3ForGraphs( hNHCalAll_ZX, "Z (cm)", "X(cm)", "1 MEQ Neutron Fluence (cm^{-2})", 0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,0.85*textSizeRel, textSizeRel,  1., 1., 1);
      hNHCalAll_ZX->Draw("colz");       
      DrawLatex(0.72, 0.90, Form("DIS e-p + p beam gas e-p, %.0f fb^{-1}",1.), true,0.85*textSizeRel, 62,0.05, 1);
      DrawLatex(0.72, 0.94,Form("#it{NHCal} region, %.1f < y (cm) < %.1f",-1.5,1.5), true,textSizeRel, 62,0.05, 1);
      DrawLabelsAndLinesNHCal2D(rangeZNHCal,labelLayerNHCal, textSizeRel );
    c2->SaveAs(Form("%sNHCal_ZX_All.pdf",outputDir.Data()));  
    
// 
//     delete c1;
}
