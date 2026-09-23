#include "TFile.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "PlottingHeader.h"

//********************************************************************************************************
//********************************************************************************************************
//********************************** MAIN FUNCTION *******************************************************
//********************************************************************************************************
//********************************************************************************************************
void plotRadiationDose2023() {
    StyleSettingsBasics();
    // 1. Open the ROOT file
    //https://wiki.bnl.gov/EPIC/index.php?title=Radiation_Doses 
    TFile* fileDIS = TFile::Open("MEQ_ONLY_bryceCanyon_fluences_beamGas_275_central_7_8_2023.root");
    if (!fileDIS || fileDIS->IsZombie()) {
        printf("Error: Could not open file.\n");
        return;
    }
    TFile* filePBeamGas = TFile::Open("MEQ_ONLY_proton_bryceCanyon_fluences_beamGas_275_central_7_8_2023.root");
    if (!filePBeamGas || filePBeamGas->IsZombie()) {
        printf("Error: Could not open file.\n");
        return;
    }

    // Get 2D hists
    TH2F* h2DIS = (TH2F*)fileDIS->Get("neutron_MEQ_fluence");
    if (!h2DIS) {
        printf("Error: Could not find the 2D histogram.\n");
        fileDIS->Close();
        return;
    }
    TH2F* h2PBeamGas = (TH2F*)filePBeamGas->Get("neutron_MEQ_fluence");
    if (!h2PBeamGas) {
        printf("Error: Could not find the 2D histogram.\n");
        filePBeamGas->Close();
        return;
    }

    
    Double_t rangeZLFHCal[9] = {368, 378, 398, 418, 438, 458, 478, 488, 500 };
    TH1F* hLFHCalDIS[8];
    TH1F* hLFHCalPBeamGas[8];
    TH1F* hLFHCalAll[8];
    TH1F* hLFHCalAllFirst5years[8];
    TH1F* hLFHCalAllFirst15years[8];

    Double_t lumi5years   = 40; 
    Double_t lumi15years  = 150; 
    // Projections in various ranges
    Double_t max= 0;
    for (Int_t b = 0; b< 8; b++){
      hLFHCalDIS[b]        = (TH1F*)h2DIS->ProjectionY(Form("hLFHCalDIS_%i", b), h2DIS->GetXaxis()->FindBin(rangeZLFHCal[b]), h2DIS->GetXaxis()->FindBin(rangeZLFHCal[b+1]), "E");
      hLFHCalDIS[b]->Scale(1./(rangeZLFHCal[b+1]-rangeZLFHCal[b]));
      hLFHCalPBeamGas[b]   = (TH1F*)h2PBeamGas->ProjectionY(Form("hLFHCalPBeamGas%i", b), h2PBeamGas->GetXaxis()->FindBin(rangeZLFHCal[b]), h2PBeamGas->GetXaxis()->FindBin(rangeZLFHCal[b+1]), "E");
      hLFHCalPBeamGas[b]->Scale(1./(rangeZLFHCal[b+1]-rangeZLFHCal[b]));
      hLFHCalAll[b]        = (TH1F*)hLFHCalDIS[b]->Clone(Form("hLFHCalAll_%i", b));
      hLFHCalAll[b]->Sumw2();
      hLFHCalAll[b]->Add(hLFHCalPBeamGas[b]);
      if (max < hLFHCalAll[b]->GetMaximum()) max= hLFHCalAll[b]->GetMaximum();
      
      hLFHCalAllFirst5years[b]        = (TH1F*)hLFHCalAll[b]->Clone(Form("hLFHCalAllFirst5_%i", b));
      hLFHCalAllFirst5years[b]->Scale(lumi5years);
      hLFHCalAllFirst15years[b]       = (TH1F*)hLFHCalAll[b]->Clone(Form("hLFHCalAllFirst15_%i", b));
      hLFHCalAllFirst15years[b]->Scale(lumi15years);
    }
    
    Double_t textSizeRel = 0.035;
    Double_t min = hLFHCalPBeamGas[0]->GetMinimum()/3;
    
    hLFHCalDIS[0]->GetYaxis()->SetRangeUser(min, max*2);
    hLFHCalPBeamGas[0]->GetYaxis()->SetRangeUser(min, max*2);
    hLFHCalAll[0]->GetYaxis()->SetRangeUser(min, max*2);
    hLFHCalAllFirst5years[0]->GetYaxis()->SetRangeUser(min*lumi5years, max*2*lumi5years);
    hLFHCalAllFirst15years[0]->GetYaxis()->SetRangeUser(min*lumi15years, max*2*lumi15years);
    
    // 4. Draw the projected 1D histogram
    TCanvas* c1 = new TCanvas("c1", "Projection Canvas", 800, 600);
    DrawCanvasSettings(c1, 0.07, 0.02, 0.01, 0.07);
    
    TLegend* legend = GetAndSetLegend2( 0.72, 0.8, 0.95, 0.96,textSizeRel*0.85, 2, "", 42,0.2);
    
    Style_t markerStyle[10] = { 20, 21, 24, 25, 27, 28, 29, 33, 34, 43};
    Color_t color[10]       = { kBlack, kViolet+4, kBlue-3, kCyan+1, kGreen+1, 
                                kYellow-4, kOrange, kRed-4, kPink-5, kMagenta+2 };
    TString labelLayer[8]   = {"seg. 1", "seg. 2", "seg. 3", "seg. 4", "seg. 5", "seg. 6", "seg. 7", "electronics" };
                                
    for (Int_t b = 0; b< 8; b++){
      SetMarkerDefaults(hLFHCalDIS[b], "R (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalDIS[b]->GetXaxis()->SetRangeUser(0,280);
      SetMarkerDefaults(hLFHCalPBeamGas[b], "R (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalPBeamGas[b]->GetXaxis()->SetRangeUser(0,280);
      SetMarkerDefaults(hLFHCalAll[b], "R (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalAll[b]->GetXaxis()->SetRangeUser(0,280);

      SetMarkerDefaults(hLFHCalAllFirst5years[b], "R (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalAllFirst5years[b]->GetXaxis()->SetRangeUser(0,280);

      SetMarkerDefaults(hLFHCalAllFirst15years[b], "R (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hLFHCalAllFirst15years[b]->GetXaxis()->SetRangeUser(0,280);
      
      legend->AddEntry(hLFHCalDIS[b],labelLayer[b].Data(), "p");
    }

    c1->SetLogy();
    c1->cd();
    for (Int_t b = 0; b< 8; b++){
      if (b == 0) hLFHCalDIS[b]->Draw("pe");
      else hLFHCalDIS[b]->Draw("same,pe");
      
    }
    legend->SetHeader("DIS e-p");
    legend->Draw();
    
    DrawLines(20, 20, min,  min*4, 2, kGray+1, 7);
    DrawLines(40, 40, min,  min*4, 2, kGray+1, 7);
    DrawLines(100, 100, min,  min*4, 2, kGray+1, 7);
    DrawLines(250, 250, min,  min*4, 2, kGray+1, 7);
    
    DrawLatex(0.11, 0.10, "min R right", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.16, 0.14, "min R left", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.85, 0.10, "max R", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.13, 0.18, "insert", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.22, 0.18, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.56, 0.18, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    
    DrawLatex(0.4, 0.92, "#it{LFHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    c1->SaveAs("LFHCal_DIS.pdf");
    
    c1->cd();
    for (Int_t b = 0; b< 8; b++){
      if (b == 0) hLFHCalPBeamGas[b]->Draw("pe");
      else hLFHCalPBeamGas[b]->Draw("same,pe");
    }
    legend->SetHeader("p beam gas");
    legend->Draw();
    DrawLines(20, 20, min,  min*4, 2, kGray+1, 7);
    DrawLines(40, 40, min,  min*4, 2, kGray+1, 7);
    DrawLines(100, 100, min,  min*4, 2, kGray+1, 7);
    DrawLines(250, 250, min,  min*4, 2, kGray+1, 7);
    
    DrawLatex(0.11, 0.10, "min R right", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.16, 0.14, "min R left", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.85, 0.10, "max R", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.13, 0.18, "insert", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.22, 0.18, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.56, 0.18, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    
    DrawLatex(0.4, 0.92, "#it{LFHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("LFHCal_PBeamGas.pdf");
    
    
    c1->cd();
    for (Int_t b = 0; b< 8; b++){
      if (b == 0) hLFHCalAll[b]->Draw("pe");
      else hLFHCalAll[b]->Draw("same,pe");
    }
    legend->SetHeader("DIS e-p + p beam gas");
    legend->Draw();
    DrawLines(20, 20, min,  min*4, 2, kGray+1, 7);
    DrawLines(40, 40, min,  min*4, 2, kGray+1, 7);
    DrawLines(100, 100, min,  min*4, 2, kGray+1, 7);
    DrawLines(250, 250, min,  min*4, 2, kGray+1, 7);
    
    DrawLatex(0.11, 0.10, "min R right", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.16, 0.14, "min R left", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.85, 0.10, "max R", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.13, 0.18, "insert", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.22, 0.18, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.56, 0.18, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    
    DrawLatex(0.4, 0.92, "#it{LFHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("LFHCal_All.pdf");
    
    
    c1->cd();
    for (Int_t b = 0; b< 8; b++){
      if (b == 0) hLFHCalAllFirst5years[b]->Draw("pe");
      else hLFHCalAllFirst5years[b]->Draw("same,pe");
    }
    legend->SetHeader("DIS e-p + p beam gas");
    legend->Draw();
    DrawLines(20, 20, min*lumi5years,  min*4*lumi5years, 2, kGray+1, 7);
    DrawLines(40, 40, min*lumi5years,  min*4*lumi5years, 2, kGray+1, 7);
    DrawLines(100, 100, min*lumi5years,  min*4*lumi5years, 2, kGray+1, 7);
    DrawLines(250, 250, min*lumi5years,  min*4*lumi5years, 2, kGray+1, 7);
    
    DrawLatex(0.11, 0.10, "min R right", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.16, 0.14, "min R left", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.85, 0.10, "max R", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.13, 0.18, "insert", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.22, 0.18, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.56, 0.18, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    
    DrawLatex(0.4, 0.92, Form("#it{LFHCal} region, %.0f fb^{-1}", lumi5years), false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("LFHCal_All_5Years.pdf");
    
    c1->cd();
    for (Int_t b = 0; b< 8; b++){
      if (b == 0) hLFHCalAllFirst15years[b]->Draw("pe");
      else hLFHCalAllFirst15years[b]->Draw("same,pe");
    }
    legend->SetHeader("DIS e-p + p beam gas");
    legend->Draw();
    DrawLines(20, 20, min*lumi15years,  min*4*lumi15years, 2, kGray+1, 7);
    DrawLines(40, 40, min*lumi15years,  min*4*lumi15years, 2, kGray+1, 7);
    DrawLines(100, 100, min*lumi15years,  min*4*lumi15years, 2, kGray+1, 7);
    DrawLines(250, 250, min*lumi15years,  min*4*lumi15years, 2, kGray+1, 7);
    
    DrawLatex(0.11, 0.10, "min R right", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.16, 0.14, "min R left", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.85, 0.10, "max R", false,textSizeRel*0.85, 42,0.05, 1);
    DrawLatex(0.13, 0.18, "insert", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.22, 0.18, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.56, 0.18, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    
    DrawLatex(0.4, 0.92, Form("#it{LFHCal} region, %.0f fb^{-1}", lumi15years), false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("LFHCal_All_15Years.pdf");
    
    // BHCAL
    
    Double_t rangeRBHCal[9] = {180, 220, 240, 245, 250  };
    TH1F* hBHCalDIS[8];
    TH1F* hBHCalPBeamGas[8];
    TH1F* hBHCalAll[8];
    TH1F* hBHCalAllFirst5years[8];
    TH1F* hBHCalAllFirst15years[8];

    // Projections in various ranges
    Double_t maxBHcal= 0;
    for (Int_t b = 0; b< 4; b++){
      hBHCalDIS[b]        = (TH1F*)h2DIS->ProjectionX(Form("hBHCalDIS_%i", b), h2DIS->GetYaxis()->FindBin(rangeRBHCal[b]), h2DIS->GetYaxis()->FindBin(rangeRBHCal[b+1]), "E");
      hBHCalDIS[b]->Scale(1./(rangeRBHCal[b+1]-rangeRBHCal[b]));
      hBHCalPBeamGas[b]   = (TH1F*)h2PBeamGas->ProjectionX(Form("hBHCalPBeamGas%i", b), h2PBeamGas->GetYaxis()->FindBin(rangeRBHCal[b]), h2PBeamGas->GetYaxis()->FindBin(rangeRBHCal[b+1]), "E");
      hBHCalPBeamGas[b]->Scale(1./(rangeRBHCal[b+1]-rangeRBHCal[b]));
      hBHCalAll[b]        = (TH1F*)hBHCalDIS[b]->Clone(Form("hBHCalAll_%i", b));
      hBHCalAll[b]->Sumw2();
      hBHCalAll[b]->Add(hBHCalPBeamGas[b]);
      if (maxBHcal < hBHCalAll[b]->GetMaximum()) maxBHcal= hBHCalAll[b]->GetMaximum();
      
      hBHCalAllFirst5years[b]        = (TH1F*)hBHCalAll[b]->Clone(Form("hBHCalAllFirst5_%i", b));
      hBHCalAllFirst5years[b]->Scale(lumi5years);
      hBHCalAllFirst15years[b]       = (TH1F*)hBHCalAll[b]->Clone(Form("hBHCalAllFirst15_%i", b));
      hBHCalAllFirst15years[b]->Scale(lumi15years);
    }
    
    Double_t minBHCal = hLFHCalPBeamGas[0]->GetMinimum();
    maxBHcal = maxBHcal*10;
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
    DrawLatex(0.4, 0.92, "#it{BHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    c1->SaveAs("BHCal_MEQ_DIS.pdf");
    
    c1->cd();
    for (Int_t b = 0; b< 4; b++){
      if (b == 0) hBHCalPBeamGas[b]->Draw("pe");
      else hBHCalPBeamGas[b]->Draw("same,pe");
    }
    legendBHCal->SetHeader("p beam gas");
    legendBHCal->Draw();
    DrawLines(-328, -328, minBHCal,  minBHCal*4, 2, kGray+1, 7);
    DrawLines(328, 328, minBHCal,  minBHCal*4, 2, kGray+1, 7);
    DrawLatex(0.4, 0.92, "#it{BHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("BHCal_MEQ_PBeamGas.pdf");
    
    
    c1->cd();
    for (Int_t b = 0; b< 4; b++){
      if (b == 0) hBHCalAll[b]->Draw("pe");
      else hBHCalAll[b]->Draw("same,pe");
    }
    legendBHCal->SetHeader("DIS e-p + p beam gas");
    legendBHCal->Draw();
    DrawLatex(0.4, 0.92, "#it{BHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    DrawLines(-328, -328, minBHCal,  minBHCal*4, 2, kGray+1, 7);
    DrawLines(328, 328, minBHCal,  minBHCal*4, 2, kGray+1, 7);
    
    c1->SaveAs("BHCal_MEQ_All.pdf");
    
    
    c1->cd();
    for (Int_t b = 0; b< 4; b++){
      if (b == 0) hBHCalAllFirst5years[b]->Draw("pe");
      else hBHCalAllFirst5years[b]->Draw("same,pe");
    }
    legendBHCal->SetHeader("DIS e-p + p beam gas");
    legendBHCal->Draw();
    DrawLines(-328, -328, minBHCal*lumi5years,  minBHCal*4*lumi5years, 2, kGray+1, 7);
    DrawLines(328, 328, minBHCal*lumi5years,  minBHCal*4*lumi5years, 2, kGray+1, 7);
    DrawLatex(0.4, 0.92, Form("#it{BHCal} region, %.0f fb^{-1}", lumi5years), false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("BHCal_MEQ_All_5Years.pdf");
    
    c1->cd();
    for (Int_t b = 0; b< 4; b++){
      if (b == 0) hBHCalAllFirst15years[b]->Draw("pe");
      else hBHCalAllFirst15years[b]->Draw("same,pe");
    }
    legendBHCal->SetHeader("DIS e-p + p beam gas");
    legendBHCal->Draw();    
    DrawLines(-328, -328, minBHCal*lumi15years,  minBHCal*4*lumi15years, 2, kGray+1, 7);
    DrawLines(328, 328, minBHCal*lumi15years,  minBHCal*4*lumi15years, 2, kGray+1, 7);
    DrawLatex(0.4, 0.92, Form("#it{BHCal} region, %.0f fb^{-1}", lumi15years), false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("BHCal_MEQ_All_15Years.pdf");
    
    // NHCal 
    
    Double_t rangeZNHCal[7] = { 385, 400, 410, 420, 430, 440, 450 };
    TH1F* hNHCalDIS[8];
    TH1F* hNHCalPBeamGas[8];
    TH1F* hNHCalAll[8];
    TH1F* hNHCalAllFirst5years[8];
    TH1F* hNHCalAllFirst15years[8];

    // Projections in various ranges
    Double_t maxNHCal= 0;
    for (Int_t b = 0; b< 6; b++){
      hNHCalDIS[b]        = (TH1F*)h2DIS->ProjectionY(Form("hNHCalDIS_%i", b), h2DIS->GetXaxis()->FindBin(rangeZNHCal[b]), h2DIS->GetXaxis()->FindBin(rangeZNHCal[b+1]), "E");
      hNHCalDIS[b]->Scale(1./(TMath::Abs(rangeZNHCal[b+1]-rangeZNHCal[b])));
      hNHCalPBeamGas[b]   = (TH1F*)h2PBeamGas->ProjectionY(Form("hNHCalPBeamGas%i", b), h2PBeamGas->GetXaxis()->FindBin(rangeZNHCal[b]), h2PBeamGas->GetXaxis()->FindBin(rangeZNHCal[b+1]), "E");
      hNHCalPBeamGas[b]->Scale(1./(TMath::Abs(rangeZNHCal[b+1]-rangeZNHCal[b])));
      hNHCalAll[b]        = (TH1F*)hNHCalDIS[b]->Clone(Form("hNHCalAll_%i", b));
      hNHCalAll[b]->Sumw2();
      hNHCalAll[b]->Add(hNHCalPBeamGas[b]);
      if (maxNHCal < hNHCalAll[b]->GetMaximum()) maxNHCal= hNHCalAll[b]->GetMaximum();
      
      hNHCalAllFirst5years[b]        = (TH1F*)hNHCalAll[b]->Clone(Form("hNHCalAllFirst5_%i", b));
      hNHCalAllFirst5years[b]->Scale(lumi5years);
      hNHCalAllFirst15years[b]       = (TH1F*)hNHCalAll[b]->Clone(Form("hNHCalAllFirst15_%i", b));
      hNHCalAllFirst15years[b]->Scale(lumi15years);
    }
    
    Double_t minNHCal = hNHCalPBeamGas[0]->GetMinimum()/3;
    
    hNHCalDIS[0]->GetYaxis()->SetRangeUser(minNHCal, maxNHCal*2);
    hNHCalPBeamGas[0]->GetYaxis()->SetRangeUser(minNHCal, maxNHCal*2);
    hNHCalAll[0]->GetYaxis()->SetRangeUser(minNHCal, maxNHCal*2);
    hNHCalAllFirst5years[0]->GetYaxis()->SetRangeUser(minNHCal*lumi5years, maxNHCal*2*lumi5years);
    hNHCalAllFirst15years[0]->GetYaxis()->SetRangeUser(minNHCal*lumi15years, maxNHCal*2*lumi15years);
    
    TString labelLayerNHCal[8]   = {"electronics", "seg. 1", "seg. 2", "seg. 3", "seg. 4", "seg. 5", "seg. 6", "seg. 7"};
    TLegend* legendNHCal = GetAndSetLegend2( 0.72, 0.84, 0.95, 0.96,textSizeRel*0.85, 2, "", 42,0.2);
                                    
    for (Int_t b = 0; b< 6; b++){
      SetMarkerDefaults(hNHCalDIS[b], "R (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalDIS[b]->GetXaxis()->SetRangeUser(0,280);
      SetMarkerDefaults(hNHCalPBeamGas[b], "R (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalPBeamGas[b]->GetXaxis()->SetRangeUser(0,280);
      SetMarkerDefaults(hNHCalAll[b], "R (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalAll[b]->GetXaxis()->SetRangeUser(0,280);

      SetMarkerDefaults(hNHCalAllFirst5years[b], "R (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalAllFirst5years[b]->GetXaxis()->SetRangeUser(0,280);

      SetMarkerDefaults(hNHCalAllFirst15years[b], "R (cm)", "1 MEQ Neutron Fluence (cm^{-2})", markerStyle[b], 1, color[b], color[b], textSizeRel, 0.85*textSizeRel, 1., 1.);
      hNHCalAllFirst15years[b]->GetXaxis()->SetRangeUser(0,280);
      
      legendNHCal->AddEntry(hNHCalDIS[b],labelLayerNHCal[b].Data(), "p");
    }

    c1->SetLogy();
    c1->cd();
    for (Int_t b = 0; b< 6; b++){
      if (b == 0) hNHCalDIS[b]->Draw("pe");
      else hNHCalDIS[b]->Draw("same,pe");
      
    }
    legendNHCal->SetHeader("DIS e-p");
    legendNHCal->Draw();
    
    DrawLines(10, 10, minNHCal,  minNHCal*4, 2, kGray+1, 7);
    DrawLines(253, 253, minNHCal,  minNHCal*4, 2, kGray+1, 7);
        
    DrawLatex(0.4, 0.92, "#it{NHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    c1->SaveAs("NHCal_DIS.pdf");
    
    c1->cd();
    for (Int_t b = 0; b< 6; b++){
      if (b == 0) hNHCalPBeamGas[b]->Draw("pe");
      else hNHCalPBeamGas[b]->Draw("same,pe");
    }
    legendNHCal->SetHeader("p beam gas");
    legendNHCal->Draw();
    DrawLines(10, 10, minNHCal,  minNHCal*4, 2, kGray+1, 7);
    DrawLines(253, 253, minNHCal,  minNHCal*4, 2, kGray+1, 7);
    
    DrawLatex(0.4, 0.92, "#it{NHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("NHCal_PBeamGas.pdf");
    
    
    c1->cd();
    for (Int_t b = 0; b< 6; b++){
      if (b == 0) hNHCalAll[b]->Draw("pe");
      else hNHCalAll[b]->Draw("same,pe");
    }
    legendNHCal->SetHeader("DIS e-p + p beam gas");
    legendNHCal->Draw();
    DrawLines(10, 10, minNHCal,  minNHCal*4, 2, kGray+1, 7);
    DrawLines(253, 253, minNHCal,  minNHCal*4, 2, kGray+1, 7);

    DrawLatex(0.4, 0.92, "#it{NHCal} region, 1 fb^{-1}", false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("NHCal_All.pdf");
    
    
    c1->cd();
    for (Int_t b = 0; b< 6; b++){
      if (b == 0) hNHCalAllFirst5years[b]->Draw("pe");
      else hNHCalAllFirst5years[b]->Draw("same,pe");
    }
    legendNHCal->SetHeader("DIS e-p + p beam gas");
    legendNHCal->Draw();
    DrawLines(10, 10, minNHCal*lumi5years,  minNHCal*4*lumi5years, 2, kGray+1, 7);
    DrawLines(253, 253, minNHCal*lumi5years,  minNHCal*4*lumi5years, 2, kGray+1, 7);
    DrawLatex(0.4, 0.92, Form("#it{NHCal} region, %.0f fb^{-1}", lumi5years), false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("NHCal_All_5Years.pdf");
    
    c1->cd();
    for (Int_t b = 0; b< 6; b++){
      if (b == 0) hNHCalAllFirst15years[b]->Draw("pe");
      else hNHCalAllFirst15years[b]->Draw("same,pe");
    }
    legendNHCal->SetHeader("DIS e-p + p beam gas");
    legendNHCal->Draw();
    DrawLines(10, 10, minNHCal*lumi15years,  minNHCal*4*lumi15years, 2, kGray+1, 7);
    DrawLines(253, 253, minNHCal*lumi15years,  minNHCal*4*lumi15years, 2, kGray+1, 7);
    
    DrawLatex(0.4, 0.92, Form("#it{NHCal} region, %.0f fb^{-1}", lumi15years), false,textSizeRel, 62,0.05, 1);
    
    c1->SaveAs("NHCal_All_15Years.pdf");

    delete c1;
}
