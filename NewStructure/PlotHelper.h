#ifndef PLOTTHELPER_H
#define PLOTTHELPER_H

#include "TLegend.h"
#include "TAxis.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TBox.h"
#include "TPad.h"
#include "TFrame.h"
#include "TLatex.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TGaxis.h"
#include "TSystem.h"
#include "TStyle.h"
#include <TH3.h>
#include "TileSpectra.h"  
#include "TileTrend.h"  
#include "CalibSummary.h"  
#include "AnaSummary.h"
#include "CommonHelperFunctions.h"
#include "PlotHelper_general.h"
#include "PlotHelper_8MLayer.h"
#include "PlotHelper_2MLayer.h"
#include "PlotHelper_1MLayer.h"
#include "PlotHelper_2ModLayer.h"

  //__________________________________________________________________________________________________________
  // Plot 2D fit variables overview
  //__________________________________________________________________________________________________________  
  void PlotSimple2D( TCanvas* canvas2D, 
                     TH2* hist, double maxy, double maxx, 
                     Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                     int labelOpt = 1, Bool_t hasNeg = kFALSE, TString drwOpt ="colz", 
                     bool blegAbove = false, TString additionalLabel = ""
                    ){
      canvas2D->cd();
      SetStyleHistoTH2ForGraphs( hist, hist->GetXaxis()->GetTitle(), hist->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.9, 1.05);  
      hist->GetZaxis()->SetLabelSize(0.85*textSizeRel);
      hist->GetZaxis()->SetTitleOffset(1.06);
      hist->GetZaxis()->SetTitleSize(textSizeRel);
      // if (hist->GetYaxis()->GetTitle().CompareTo("") != 0)
        
      if (maxy > -10000)hist->GetYaxis()->SetRangeUser(hist->GetYaxis()->GetBinCenter(1)-0.1,maxy+0.1);
      if (maxx > -10000)hist->GetXaxis()->SetRangeUser(hist->GetXaxis()->GetBinCenter(1)-0.1,maxx+0.1);
      if (!hasNeg)
        hist->GetZaxis()->SetRangeUser(hist->GetMinimum(0),hist->GetMaximum());
      else 
        hist->GetZaxis()->SetRangeUser(hist->GetMinimum(),hist->GetMaximum());
    
      if (((TString)hist->GetZaxis()->GetTitle()).Contains("counts")){
        gStyle->SetPaintTextFormat(".0f");
        std::cout << "entered counts case" << std::endl;
      } else {
        gStyle->SetPaintTextFormat(".3f");
      }
      hist->DrawCopy(drwOpt.Data());
      
      if (!blegAbove)
        DrawLatex(0.85, 0.92, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);
      else 
        DrawLatex(0.92, 0.97, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);

      if (additionalLabel.CompareTo("") != 0){
        if (!blegAbove)
          DrawLatex(0.11, 0.92, additionalLabel, false, 0.85*textSizeRel, 42);
        else 
          DrawLatex(0.08, 0.97, additionalLabel, false, 0.85*textSizeRel, 42);
      }
      if (((TString)hist->GetXaxis()->GetTitle()).Contains("cell ID")){
        std::cout << "entered cell ID" << std::endl;
        if (maxx > -10000)
          DrawLines(hist->GetXaxis()->GetBinCenter(1)-0.1, maxx+0.1,0., 0., 5, kGray+1, 10);  
        else
          DrawLines(hist->GetXaxis()->GetBinCenter(1)-0.1,hist->GetXaxis()->GetBinCenter(hist->GetNbinsX()-1)+0.1,0., 0., 5, kGray+1, 10);  
      }
    canvas2D->SaveAs(nameOutput.Data());
  }

  //__________________________________________________________________________________________________________
  // Plot 2D fit variables overview
  //__________________________________________________________________________________________________________  
  void PlotSimple2D( TCanvas* canvas2D, 
                     TH2* hist, double miny, double maxy, double maxx, 
                     Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                     int labelOpt = 1, Bool_t hasNeg = kFALSE, TString drwOpt ="colz", 
                     bool blegAbove = false, TString additionalLabel = ""
                    ){
      canvas2D->cd();
      SetStyleHistoTH2ForGraphs( hist, hist->GetXaxis()->GetTitle(), hist->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.9, 1.05);  
      hist->GetZaxis()->SetLabelSize(0.85*textSizeRel);
      hist->GetZaxis()->SetTitleOffset(1.06);
      hist->GetZaxis()->SetTitleSize(textSizeRel);
      // if (hist->GetYaxis()->GetTitle().CompareTo("") != 0)
        
      if (miny > -10000 && maxy > -10000)hist->GetYaxis()->SetRangeUser(miny-0.1,maxy+0.1);
      if (maxx > -10000)hist->GetXaxis()->SetRangeUser(hist->GetXaxis()->GetBinCenter(1)-0.1,maxx+0.1);
      if (!hasNeg)
        hist->GetZaxis()->SetRangeUser(hist->GetMinimum(0),hist->GetMaximum());
      else 
        hist->GetZaxis()->SetRangeUser(hist->GetMinimum(),hist->GetMaximum());
    
      if (((TString)hist->GetZaxis()->GetTitle()).Contains("counts")){
        gStyle->SetPaintTextFormat(".0f");
        std::cout << "entered counts case" << std::endl;
      } else {
        gStyle->SetPaintTextFormat(".3f");
      }
      hist->DrawCopy(drwOpt.Data());
      
      if (!blegAbove)
        DrawLatex(0.85, 0.92, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);
      else 
        DrawLatex(0.92, 0.97, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);

      if (additionalLabel.CompareTo("") != 0){
        if (!blegAbove)
          DrawLatex(0.11, 0.92, additionalLabel, false, 0.85*textSizeRel, 42);
        else 
          DrawLatex(0.08, 0.97, additionalLabel, false, 0.85*textSizeRel, 42);
      }
      
      if (((TString)hist->GetXaxis()->GetTitle()).Contains("cell ID")){
        std::cout << "entered cell ID" << std::endl;
        if (maxx > -10000)
          DrawLines(hist->GetXaxis()->GetBinCenter(1)-0.1, maxx+0.1,0., 0., 5, kGray+1, 10);  
        else
          DrawLines(hist->GetXaxis()->GetBinCenter(1)-0.1,hist->GetXaxis()->GetBinCenter(hist->GetNbinsX()-1)+0.1,0., 0., 5, kGray+1, 10);  
      }
    canvas2D->SaveAs(nameOutput.Data());
  }

  //__________________________________________________________________________________________________________
  // Plot 2D distribution with graph on top
  //__________________________________________________________________________________________________________  
  void Plot2DWithGraph( TCanvas* canvas2D, 
                     TH2* hist, TGraphErrors* graph, double maxy, double maxx, 
                     Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                     int labelOpt = 1, Bool_t hasNeg = kFALSE, TString drwOpt ="colz", 
                     bool blegAbove = false, TString additionalLabel = ""
                    ){
      canvas2D->cd();
      
      SetStyleHistoTH2ForGraphs( hist, hist->GetXaxis()->GetTitle(), hist->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.9, 1.05);  
      hist->GetZaxis()->SetLabelSize(0.85*textSizeRel);
      hist->GetZaxis()->SetTitleOffset(1.06);
      hist->GetZaxis()->SetTitleSize(textSizeRel);
      // if (hist->GetYaxis()->GetTitle().CompareTo("") != 0)
      
      if (canvas2D->GetLogy() == 1){
        if (maxy > -10000)hist->GetYaxis()->SetRangeUser(0.05,maxy+0.1);
      } else {
        if (maxy > -10000)hist->GetYaxis()->SetRangeUser(hist->GetYaxis()->GetBinCenter(1)-0.1,maxy+0.1);
      }
      if (maxx > -10000)hist->GetXaxis()->SetRangeUser(hist->GetXaxis()->GetBinCenter(1)-0.1,maxx+0.1);
      if (!hasNeg)
        hist->GetZaxis()->SetRangeUser(hist->GetMinimum(0),hist->GetMaximum());
      else 
        hist->GetZaxis()->SetRangeUser(hist->GetMinimum(),hist->GetMaximum());
    
      if (((TString)hist->GetZaxis()->GetTitle()).Contains("counts")){
        gStyle->SetPaintTextFormat(".0f");
        std::cout << "entered counts case" << std::endl;
      } else {
        gStyle->SetPaintTextFormat(".3f");
      }
      hist->DrawCopy(drwOpt.Data());
      
      if (graph){
        SetMarkerDefaultsTGraphErr(  graph, 24, 2, kGray+1,kGray+1, 3, kFALSE, 0, kFALSE);
        graph->Draw("same,pe");
      }
      if (!blegAbove){
        DrawLatex(0.835, 0.935, Form("#it{#bf{LFHCal TB:} %s}",GetStringFromRunInfo(currRunInfo,7).Data()), true, textSizeRel, 42);
        DrawLatex(0.835, 0.90, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);
        DrawLatex(0.835, 0.865, Form("%s",GetStringFromRunInfo(currRunInfo,8).Data()), true, 0.85*textSizeRel, 42);
      }
      else 
        DrawLatex(0.92, 0.97, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);

      if (additionalLabel.CompareTo("") != 0){
        if (!blegAbove)
          DrawLatex(0.11, 0.92, additionalLabel, false, 0.85*textSizeRel, 42);
        else 
          DrawLatex(0.08, 0.97, additionalLabel, false, 0.85*textSizeRel, 42);
      }
      if (((TString)hist->GetXaxis()->GetTitle()).Contains("cell ID")){
        if (maxx > -10000)
          DrawLines(hist->GetXaxis()->GetBinCenter(1)-0.1, maxx+0.1,0., 0., 5, kGray+1, 10);  
        else
          DrawLines(hist->GetXaxis()->GetBinCenter(1)-0.1,hist->GetXaxis()->GetBinCenter(hist->GetNbinsX()-1)+0.1,0., 0., 5, kGray+1, 10);  
      }
    canvas2D->SaveAs(nameOutput.Data());
  }  
  
  //__________________________________________________________________________________________________________
  // Plot 2D distribution with profile on top
  //__________________________________________________________________________________________________________  
  void Plot2DWithProfile( TCanvas* canvas2D, 
                     TH2* hist, TProfile* profile, double maxy, double maxx, 
                     Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                     int labelOpt = 1, Bool_t hasNeg = kFALSE, TString drwOpt ="colz", 
                     bool blegAbove = false, TString additionalLabel = "", int vLine = -1
                    ){
      canvas2D->cd();
      
      SetStyleHistoTH2ForGraphs( hist, hist->GetXaxis()->GetTitle(), hist->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.9, 1.05);  
      hist->GetZaxis()->SetLabelSize(0.85*textSizeRel);
      hist->GetZaxis()->SetTitleOffset(1.06);
      hist->GetZaxis()->SetTitleSize(textSizeRel);
      // if (hist->GetYaxis()->GetTitle().CompareTo("") != 0)
      
      if (canvas2D->GetLogy() == 1){
        if (maxy > -10000)hist->GetYaxis()->SetRangeUser(0.05,maxy+0.1);
      } else {
        if (maxy > -10000)hist->GetYaxis()->SetRangeUser(hist->GetYaxis()->GetBinCenter(1)-0.1,maxy+0.1);
      }
      if (maxx > -10000)hist->GetXaxis()->SetRangeUser(hist->GetXaxis()->GetBinCenter(1)-0.1,maxx+0.1);
      if (!hasNeg)
        hist->GetZaxis()->SetRangeUser(hist->GetMinimum(0),hist->GetMaximum());
      else 
        hist->GetZaxis()->SetRangeUser(hist->GetMinimum(),hist->GetMaximum());
    
      if (((TString)hist->GetZaxis()->GetTitle()).Contains("counts")){
        gStyle->SetPaintTextFormat(".0f");
        std::cout << "entered counts case" << std::endl;
      } else {
        gStyle->SetPaintTextFormat(".3f");
      }
      hist->DrawCopy(drwOpt.Data());
      
      if (profile){
        SetMarkerDefaultsProfile(  profile, 24, 2, kBlue+1,kBlue+1);
        profile->Draw("same,pe");
        if (vLine != -1)
          DrawLines(vLine, vLine, 0, 500., 5, kGray+1, 10);  
      }
      if (!blegAbove){
        DrawLatex(0.835, 0.935, Form("#it{#bf{LFHCal TB:} %s}",GetStringFromRunInfo(currRunInfo,7).Data()), true, textSizeRel, 42);
        DrawLatex(0.835, 0.90, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);
        DrawLatex(0.835, 0.865, Form("%s",GetStringFromRunInfo(currRunInfo,8).Data()), true, 0.85*textSizeRel, 42);
      }
      else 
        DrawLatex(0.92, 0.97, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);

      if (additionalLabel.CompareTo("") != 0){
        if (!blegAbove)
          DrawLatex(0.11, 0.92, additionalLabel, false, 0.85*textSizeRel, 42);
        else 
          DrawLatex(0.08, 0.97, additionalLabel, false, 0.85*textSizeRel, 42);
      }
      if (((TString)hist->GetXaxis()->GetTitle()).Contains("cell ID")){
        if (maxx > -10000)
          DrawLines(hist->GetXaxis()->GetBinCenter(1)-0.1, maxx+0.1,0., 0., 5, kGray+1, 10);  
        else
          DrawLines(hist->GetXaxis()->GetBinCenter(1)-0.1,hist->GetXaxis()->GetBinCenter(hist->GetNbinsX()-1)+0.1,0., 0., 5, kGray+1, 10);  
      }
    canvas2D->SaveAs(nameOutput.Data());
  }    
  //__________________________________________________________________________________________________________
  // Plot 1D distribution
  //__________________________________________________________________________________________________________  
  void PlotSimple1D( TCanvas* canvas2D, 
                     TH1* hist, Int_t maxy, Int_t maxx, 
                     Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                     int labelOpt = 1,
                     TString additionalLabel = ""
                    ){
      canvas2D->cd();
      SetStyleHistoTH1ForGraphs( hist, hist->GetXaxis()->GetTitle(), hist->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.9, 1.05);  
      
      SetMarkerDefaults(hist, 20, 1, kBlue+1, kBlue+1, kFALSE);   
      // if (hist->GetYaxis()->GetTitle().CompareTo("") != 0)
        
      if (maxy > -10000)hist->GetYaxis()->SetRangeUser(-0.5,maxy+0.1);
      if (maxx > -10000)hist->GetXaxis()->SetRangeUser(-0.5,maxx+0.1);
    
      hist->Draw("p,e");
      
      DrawLatex(0.95, 0.92, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);
      if (additionalLabel.CompareTo("") != 0){
        DrawLatex(0.95, 0.92-textSizeRel, additionalLabel, true, 0.85*textSizeRel, 42);
      }
    canvas2D->SaveAs(nameOutput.Data());
  }

  //__________________________________________________________________________________________________________
  // Plot 1D distribution
  //__________________________________________________________________________________________________________  
  void PlotSimpleWithFit1D( TCanvas* canvas2D, 
                            TH1* hist, TF1* fit, Int_t maxy, Int_t maxx, 
                            Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                            int labelOpt = 1,
                            TString additionalLabel = ""
                          ){
      canvas2D->cd();
      SetStyleHistoTH1ForGraphs( hist, hist->GetXaxis()->GetTitle(), hist->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.9, 1.05);  
      
      SetMarkerDefaults(hist, 24, 1, kBlack, kBlack, kFALSE);
      // if (hist->GetYaxis()->GetTitle().CompareTo("") != 0)
        
      if (maxy > -10000)hist->GetYaxis()->SetRangeUser(-0.5,maxy+0.1);
      if (maxx > -10000)hist->GetXaxis()->SetRangeUser(-0.5,maxx+0.1);
    
      hist->Draw("p,e");
    
      if (fit){
        Double_t xFitMax = -10000;
        if (maxx > -10000) 
          xFitMax = hist->GetXaxis()->GetBinCenter(hist->GetNbinsX());
        else 
          xFitMax = maxx;
        SetStyleFit(fit , -10000, -10000, 7, 7, kRed+1);
        fit->Draw("same");
        
        DrawLines(fit->GetParameter(1),fit->GetParameter(1),0, hist->GetMaximum()*0.1, 5, kRed+1, 10);  
        DrawLines(fit->GetParameter(1)-fit->GetParameter(2),fit->GetParameter(1)-fit->GetParameter(2),0, hist->GetMaximum()*0.2, 5, kGray+1, 10);  
        DrawLines(fit->GetParameter(1)+fit->GetParameter(2),fit->GetParameter(1)+fit->GetParameter(2),0, hist->GetMaximum()*0.2, 5, kGray+1, 10);  
      }
    
      DrawLatex(0.95, 0.92, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);
      if (additionalLabel.CompareTo("") != 0){
        DrawLatex(0.95, 0.92-textSizeRel, additionalLabel, true, 0.85*textSizeRel, 42);
      }
    canvas2D->SaveAs(nameOutput.Data());
  }

  
  //__________________________________________________________________________________________________________
  // Plot 1D distribution
  //__________________________________________________________________________________________________________  
  void PlotContamination1D( TCanvas* canvas2D, 
                     TH1* histAll, TH1* histMuon, TH1* histPrim, Int_t maxy, Int_t maxx, 
                     Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                     int labelOpt = 1,
                     TString additionalLabel = ""
                    ){
      canvas2D->cd();
      SetStyleHistoTH1ForGraphs( histAll, histAll->GetXaxis()->GetTitle(), histAll->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.9, 1.05);  
      
      SetMarkerDefaults(histAll, 20, 1, kBlue+1, kBlue+1, kFALSE);   
      // if (hist->GetYaxis()->GetTitle().CompareTo("") != 0)
        
      if (maxy > -10000)histAll->GetYaxis()->SetRangeUser(-0.5,maxy+0.1);
      if (maxx > -10000)histAll->GetXaxis()->SetRangeUser(0.5,maxx+0.1);
    
      histAll->Draw("p,e");
      SetMarkerDefaults(histMuon, 25, 1, kGray+1, kGray+1, kFALSE);   
      histMuon->Draw("p,e,same");
      SetMarkerDefaults(histPrim, 24, 1, kRed+1, kRed+1, kFALSE);   
      histPrim->Draw("p,e,same");
    

      DrawLatex(0.95, 0.92, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);
      if (additionalLabel.CompareTo("") != 0){
        DrawLatex(0.95, 0.92-textSizeRel, additionalLabel, true, 0.85*textSizeRel, 42);
      }
      
      TLegend* legend = GetAndSetLegend2( 0.11, 0.93-3*textSizeRel, 0.4, 0.93,0.85*textSizeRel, 1, "", 42,0.1);
      legend->AddEntry(histAll, "no evt. selection", "p");
      legend->AddEntry(histMuon, "muon events", "p");
      legend->AddEntry(histPrim, "remaining events", "p");
      legend->Draw();
      
    canvas2D->SaveAs(nameOutput.Data());
  }
  

  //__________________________________________________________________________________________________________
  // Plot 1D distribution
  //__________________________________________________________________________________________________________  
  void PlotLayerOverlay( TCanvas* canvas2D, 
                     TH1D** histLayer, Float_t maxy, Float_t maxx, Float_t meanLayer, int maxLayer,
                     Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                     int labelOpt = 1,
                     TString additionalLabel = "", bool frebin = true
                    ){
      canvas2D->cd();
      canvas2D->SetLogy(1);
      
      SetStyleHistoTH1ForGraphs( histLayer[0], histLayer[0]->GetXaxis()->GetTitle(), histLayer[0]->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.9, 1.05);  
      
      // if (hist->GetYaxis()->GetTitle().CompareTo("") != 0)
      TString title = histLayer[0]->GetXaxis()->GetTitle();
      bool Exaxis       = false;
      bool Posxaxis     = false;
      bool NCellsxaxis  = false;
      if (title.Contains("E_{layer}")) 
        Exaxis = true;
      if (title.Contains("(cm)"))   
        Posxaxis = true;
      if (title.Contains("N_{cells,layer}"))   
        NCellsxaxis = true;
        
      if (maxy > -10000 )
        histLayer[0]->GetYaxis()->SetRangeUser(1,maxy);
      if (maxx > -10000 && !Posxaxis){
        std::cout << "resetting x range: 0 - " << maxx  << std::endl;
        histLayer[0]->GetXaxis()->SetRange(1,histLayer[0]->GetXaxis()->FindBin(maxx)+1);
      } else if (maxx > -10000 && Posxaxis){
        histLayer[0]->GetXaxis()->SetRange(histLayer[0]->GetXaxis()->FindBin(-maxx)-1,histLayer[0]->GetXaxis()->FindBin(maxx)+1);
      }
      histLayer[0]->DrawCopy("axis");
      
      Setup* setup = Setup::GetInstance();
      
      TLegend* legend   = nullptr;
      Int_t lineBottom  = (2+8);
      if (setup->GetNMaxLayer()+1 == 32)
        lineBottom      = (2+5);
      else if (!(setup->GetNMaxLayer()+1 == 64))
        lineBottom      = (2+4);
      else if ( setup->GetNMaxLayer()+1 == 64 && (NCellsxaxis || Posxaxis))
        lineBottom  = (2+5);
        
      if (setup->GetNMaxLayer()+1 == 64 && (NCellsxaxis || Posxaxis )){
        legend = GetAndSetLegend2( 0.11, 0.93-lineBottom*0.85*textSizeRel, 0.95, 0.93-2*0.85*textSizeRel,0.75*textSizeRel, 15, Form("Layer, #LTlayer#GT = %.2f",meanLayer), 42,0.4);
      } else if (setup->GetNMaxLayer()+1 == 64 ){
        legend = GetAndSetLegend2( 0.4, 0.93-lineBottom*0.85*textSizeRel, 0.95, 0.93-2*0.85*textSizeRel,0.75*textSizeRel, 8, Form("Layer, #LTlayer#GT = %.2f",meanLayer), 42,0.4);
      } else if (setup->GetNMaxLayer()+1 == 32 ){
        legend = GetAndSetLegend2( 0.4, 0.93-lineBottom*0.85*textSizeRel, 0.95, 0.93-2*0.85*textSizeRel,0.75*textSizeRel, 6, Form("Layer, #LTlayer#GT = %.2f",meanLayer), 42,0.4);
      } else {
        legend = GetAndSetLegend2( 0.4, 0.93-lineBottom*0.85*textSizeRel, 0.95, 0.93-2*0.85*textSizeRel,0.75*textSizeRel, 5, Form("Layer, #LTlayer#GT = %.2f",meanLayer), 42,0.2);
      }
      for (int l = 0; l< setup->GetNMaxLayer()+1; l++){
          if (Exaxis & frebin)histLayer[l]->Rebin(4);
          SetLineDefaults(histLayer[l], GetColorLayer(l), 4, GetLineStyleLayer(l));   
          histLayer[l]->Draw("same,hist");
          if (maxLayer == l)
            legend->AddEntry(histLayer[l],Form("#bf{%d}",l),"l");
          else 
            legend->AddEntry(histLayer[l],Form("%d",l),"l");
      }  
      histLayer[0]->DrawCopy("axis,same");
      legend->Draw();
      
      DrawLatex(0.95, 0.92, Form("#it{#bf{LFHCal TB:} %s}",GetStringFromRunInfo(currRunInfo,7).Data()), true, 0.85*textSizeRel, 42);
      DrawLatex(0.95, 0.885, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);
      if (additionalLabel.CompareTo("") != 0){
        DrawLatex(0.95, 0.885-textSizeRel, additionalLabel, true, 0.85*textSizeRel, 42);
      }
      if ( setup->GetNMaxLayer()+1 == 64 && (NCellsxaxis || Posxaxis))
        lineBottom--;
      DrawLatex(0.95, 0.935-(lineBottom+1)*0.85*textSizeRel, "#bf{bold #} indicates max layer", true, 0.75*textSizeRel, 42);
        
    canvas2D->SaveAs(nameOutput.Data());
  }
  
  
  //__________________________________________________________________________________________________________
  // Plot 2D fit variables overview
  //__________________________________________________________________________________________________________  
  void PlotSimple2DZRange( TCanvas* canvas2D, 
                     TH2* hist, Int_t maxy, Int_t maxx, double minZ, double maxZ,
                     Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                     int labelOpt = 1, TString drwOpt ="colz", 
                     bool blegAbove = false, TString additionalLabel = ""
                    ){
      canvas2D->cd();
      SetStyleHistoTH2ForGraphs( hist, hist->GetXaxis()->GetTitle(), hist->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.9, 1.05);  
      hist->GetZaxis()->SetLabelSize(0.85*textSizeRel);
      hist->GetZaxis()->SetTitleOffset(1.06);
      hist->GetZaxis()->SetTitleSize(textSizeRel);
      // if (hist->GetYaxis()->GetTitle().CompareTo("") != 0)
        
      if (maxy > -10000)hist->GetYaxis()->SetRangeUser(-0.5,maxy+0.1);
      if (maxx > -10000)hist->GetXaxis()->SetRangeUser(0.5,maxx+0.1);
      hist->GetZaxis()->SetRangeUser(minZ,maxZ);
    
      if (((TString)hist->GetZaxis()->GetTitle()).Contains("counts")){
        gStyle->SetPaintTextFormat(".0f");
        std::cout << "entered counts case" << std::endl;
      } else {
        gStyle->SetPaintTextFormat(".3f");
      }
      hist->Draw(drwOpt.Data());
      
      if (!blegAbove)
        DrawLatex(0.85, 0.92, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);
      else 
        DrawLatex(0.92, 0.97, GetStringFromRunInfo(currRunInfo,labelOpt), true, 0.85*textSizeRel, 42);

      if (additionalLabel.CompareTo("") != 0){
        if (!blegAbove)
          DrawLatex(0.11, 0.92, additionalLabel, false, 0.85*textSizeRel, 42);
        else 
          DrawLatex(0.08, 0.97, additionalLabel, false, 0.85*textSizeRel, 42);
      }
    canvas2D->SaveAs(nameOutput.Data());
  }
  
  
  //__________________________________________________________________________________________________________
  // Plot Mip with Fits for SingleTile
  //__________________________________________________________________________________________________________
  void PlotMipWithFitsSingleTile (TCanvas* canvasSingleTile, Double_t topRCornerX,  Double_t topRCornerY, Double_t relSize, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, std::map<int,TileSpectra> spectraTrigg, 
                                  bool isHG, Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                                  int cellID,  TString nameOutput, RunInfo currRunInfo){
                                  
    Double_t maxY = 0;
    std::map<int, TileSpectra>::iterator ithSpectra;
    std::map<int, TileSpectra>::iterator ithSpectraTrigg;
    
    Setup* setupT = Setup::GetInstance();
    int row = setupT->GetRow(cellID);
    int col = setupT->GetColumn(cellID);
    int lay = setupT->GetLayer(cellID);
    int mod = setupT->GetModule(cellID);
    
    ithSpectra=spectra.find(cellID);
    ithSpectraTrigg=spectraTrigg.find(cellID);
    if(ithSpectra==spectra.end()){
      std::cout << "WARNING: skipping cell ID: " << cellID << "\t row " << row << "\t column " << col << "\t layer " << lay << "\t module " << mod << std::endl;
      return;
    } 
    TH1D* tempHist = nullptr;
    if (isHG){
      tempHist = ithSpectra->second.GetHG();
    } else {
      tempHist = ithSpectra->second.GetLG();
    }
    if (maxY < FindLargestBin1DHist(tempHist, xPMin , xPMax)) maxY = FindLargestBin1DHist(tempHist, xPMin , xPMax);

    
    canvasSingleTile->cd();
    canvasSingleTile->SetLogy();

    double noiseWidth = 0;
    if (isHG){
        noiseWidth = ithSpectra->second.GetCalib()->PedestalSigH;
    } else {
        noiseWidth = ithSpectra->second.GetCalib()->PedestalSigL;
    }
    SetStyleHistoTH1ForGraphs( tempHist, tempHist->GetXaxis()->GetTitle(), tempHist->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.1, 510, 510, 43, 63);  
    SetMarkerDefaults(tempHist, 20, 1.8, kBlue+1, kBlue+1, kFALSE);   
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
    SetMarkerDefaults(tempHistT, 24, 1.8, kRed+1, kRed+1, kFALSE);   
    tempHistT->Draw("same,pe");
    
    double lineHeight = 1.05*relSize;
    DrawLatex(topRCornerX, topRCornerY-1*lineHeight, Form("#it{#bf{LFHCal TB}}: #it{%s}",GetStringFromRunInfo(currRunInfo, 6).Data()), true, relSize, 42);
    DrawLatex(topRCornerX, topRCornerY-2*lineHeight, GetStringFromRunInfo(currRunInfo, 1), true,relSize, 42);
    DrawLatex(topRCornerX, topRCornerY-3*lineHeight, Form("cell %d: row %d col %d layer %d", cellID, row, col, lay), true, relSize, 42);

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
        SetStyleFit(fit , 0, 2000, 20, 3, kRed+3);
      else 
        SetStyleFit(fit , 0, 2000, 20, 7, kBlue+3);  
      fit->Draw("same");
      TLegend* legend = GetAndSetLegend2( topRCornerX-9*relSize, topRCornerY-6*lineHeight, topRCornerX-0.04, topRCornerY-3.2*lineHeight,0.85*textSizePixel, 1, "", 43,0.1);
      if (isTrigFit)
        legend->AddEntry(fit, "Landau-Gauss fit, trigg.", "l");
      else 
        legend->AddEntry(fit, "Landau-Gauss fit", "l");  
      legend->AddEntry((TObject*)0, Form("#scale[0.8]{L MPV = %2.2f #pm %2.2f}",fit->GetParameter(1), fit->GetParError(1) ) , " ");
      legend->AddEntry((TObject*)0, Form("#scale[0.8]{Max = %2.2f}", maxFit ) , " ");
      legend->Draw();
      DrawLines(maxFit, maxFit,0.7, scaleYMax*maxY/10, 15, kRed+3, 7);  
    }
  
    DrawLines(noiseWidth*3, noiseWidth*3,0.7, scaleYMax*maxY, 8, kGray+1, 10);  
    DrawLines(noiseWidth*5, noiseWidth*5,0.7, scaleYMax*maxY, 8, kGray+1, 6);  
  
    canvasSingleTile->SaveAs(nameOutput.Data());
  }

  
  //__________________________________________________________________________________________________________
  // Simple event display with different highlighted triggers
  //__________________________________________________________________________________________________________
  void EventDisplayWithSliceHighlighted( TH3F* h3All, TH1D* h1XAll, TH1D* h1YAll, TH1D* h1ZAll, 
                                         TH3F* h3LocTrigg, TH1D* h1XLocTrigg, TH1D* h1YLocTrigg, TH1D* h1ZLocTrigg, 
                                         TH3F* h3Remain, TH1D* h1XRemain, TH1D* h1YRemain, TH1D* h1ZRemain, 
                                         Int_t evtNr, Float_t etot, Float_t maxE, 
                                         Float_t maxEX, Float_t maxEY, Float_t maxEZ, bool ktrigg,
                                         RunInfo currRunInfo, TString outputName, TString suffix = "pdf", TString unit = "mip eq/tile"
                                        ){
    Double_t textSizeRel = 0.035;
    Double_t textSizeSubpad = 0.06;
    
    TCanvas* canvas3D2 = new TCanvas("canvas3D2","",0,0,1400,750);  // gives the page size
    // DefaultCanvasSettings( canvas3D2, 0.05, 0.25, 0.05, 0.1);
    TPad* padEvt[4];
    padEvt[0] = new TPad("pad_0", "", 0, 0, 0.75, 0.9,-1, -1, -2);
    padEvt[1] = new TPad("pad_1", "", 0.75, 0.66, 1, 1,-1, -1, -2);
    padEvt[2] = new TPad("pad_2", "", 0.75, 0.33, 1, 0.66,-1, -1, -2);
    padEvt[3] = new TPad("pad_3", "", 0.75, 0., 1., 0.33,-1, -1, -2);
    
    DefaultPadSettings( padEvt[0], 0.04, 0.02, 0.0, 0.1);
    padEvt[0]->SetFillStyle(4000);
    padEvt[0]->SetLineStyle(0);
    DefaultPadSettings( padEvt[1], 0.12, 0.015, 0.02, 0.12);
    padEvt[1]->SetFillStyle(4000);
    padEvt[1]->SetLineStyle(0);
    DefaultPadSettings( padEvt[2], 0.12, 0.015, 0.02, 0.12);
    padEvt[2]->SetFillStyle(4000);
    padEvt[2]->SetLineStyle(0);
    DefaultPadSettings( padEvt[3], 0.12, 0.015, 0.02, 0.12);
    padEvt[3]->SetFillStyle(4000);
    padEvt[3]->SetLineStyle(0);
    
    canvas3D2->Draw();
    canvas3D2->cd();
    
    DrawLatex(0.01, 0.95, Form("#it{#bf{LFHCal TB}}: #it{%s}",GetStringFromRunInfo(currRunInfo, 6).Data()), false, textSizeRel, 42);
    DrawLatex(0.01, 0.92, GetStringFromRunInfo(currRunInfo, 1), false, 0.85*textSizeRel, 42);
    if(ktrigg) DrawLatex(0.01, 0.89, Form("Event %d, muon triggered",evtNr), false, 0.85*textSizeRel, 42);
    else DrawLatex(0.01, 0.89, Form("Event %d",evtNr), false, 0.85*textSizeRel, 42);
    DrawLatex(0.01, 0.86, Form("#it{E}_{tot} = %.2f %s", etot, unit.Data()), false, 0.85*textSizeRel, 42);
    DrawLatex(0.01, 0.83, Form("#it{E}_{max,cell} = %.2f %s", maxE, unit.Data()), false, 0.85*textSizeRel, 42);
    
    padEvt[0]->Draw();
    padEvt[0]->cd();
    padEvt[0]->SetTheta(50);
    padEvt[0]->SetPhi(40);
    
    SetStyleHistoTH3ForGraphs(h3All, "#it{z} (cm)  ", " #it{x} (cm)","#it{y} (cm)", 0.65*textSizeRel,0.85*textSizeRel, 0.65*textSizeRel,0.85*textSizeRel, 0.65*textSizeRel,0.85*textSizeRel, 1.5, 1.3, 0.7, 510, 505, 502);
    h3All->GetXaxis()->SetLabelOffset(-0.004);
    h3All->GetYaxis()->SetLabelOffset(-0.002);
    SetStyleHistoTH3ForGraphs(h3LocTrigg, "#it{z} (cm)  ", "#it{x} (cm)","#it{y} (cm)", 0.65*textSizeRel,0.85*textSizeRel, 0.65*textSizeRel,0.85*textSizeRel, 0.65*textSizeRel,0.85*textSizeRel,  1.5, 1.6, 0.6, 510, 505, 502);
    SetStyleHistoTH3ForGraphs(h3Remain, "#it{z} (cm)  ", "#it{x} (cm)","#it{y} (cm)", 0.65*textSizeRel,0.85*textSizeRel, 0.65*textSizeRel,0.85*textSizeRel, 0.65*textSizeRel,0.85*textSizeRel,  1.5, 1.6, 0.6, 510, 505, 502);
    h3All->SetMaximum(maxE);
    h3All->SetLineColor(kBlack);
    h3All->Draw("box");
    h3Remain->SetMaximum(maxE);
    h3Remain->SetLineColor(kBlue+1);
    h3Remain->Draw("box,same,X+,Y+,Z+");
    h3LocTrigg->SetMaximum(maxE);
    h3LocTrigg->SetLineColor(kRed+1);
    h3LocTrigg->SetFillColorAlpha(kRed+1, 0.5);
    h3LocTrigg->Draw("BOX3,same");
    
    canvas3D2->cd();
    padEvt[1]->Draw();
    padEvt[1]->cd();
    SetStyleHistoTH1ForGraphs( h1XAll, "#it{x} (cm)", Form("#it{E} (%s)",unit.Data()), 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
    SetMarkerDefaults(h1XAll, 24, 1, kBlack, kBlack, kFALSE);   
    SetMarkerDefaults(h1XLocTrigg, 20, 0.8, kRed+1, kRed+1, kFALSE);   
    SetMarkerDefaults(h1XRemain, 34, 0.8, kBlue+1, kBlue+1, kFALSE);   
    h1XAll->GetYaxis()->SetRangeUser(0.,1.1*maxEX);
    h1XAll->Draw("pe");
    h1XLocTrigg->Draw("pe,same");
    h1XRemain->Draw("pe,same");
    canvas3D2->cd();
    padEvt[2]->Draw();
    padEvt[2]->cd();
    SetStyleHistoTH1ForGraphs( h1YAll, "#it{y} (cm)", Form("#it{E} (%s)",unit.Data()), 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
    SetMarkerDefaults(h1YAll, 24, 1, kBlack, kBlack, kFALSE);   
    SetMarkerDefaults(h1YLocTrigg, 20, 0.8, kRed+1, kRed+1, kFALSE);   
    SetMarkerDefaults(h1YRemain, 34, 0.8, kBlue+1, kBlue+1, kFALSE);   
    h1YAll->GetYaxis()->SetRangeUser(0.,1.1*maxEY);
    h1YAll->Draw("pe");
    h1YLocTrigg->Draw("pe,same");
    h1YRemain->Draw("pe,same");
    
    canvas3D2->cd();
    padEvt[3]->Draw();
    padEvt[3]->cd();
    SetStyleHistoTH1ForGraphs( h1ZAll, "#it{z} (cm)", Form("#it{E} (%s)",unit.Data()), 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
    SetMarkerDefaults(h1ZAll, 24, 1, kBlack, kBlack, kFALSE);   
    SetMarkerDefaults(h1ZLocTrigg, 20, 0.8, kRed+1, kRed+1, kFALSE);   
    SetMarkerDefaults(h1ZRemain, 34, 0.8, kBlue+1, kBlue+1, kFALSE);   
    h1ZAll->GetYaxis()->SetRangeUser(0.,1.1*maxEZ);
    h1ZAll->Draw("pe");
    h1ZLocTrigg->Draw("pe,same");
    h1ZRemain->Draw("pe,same");
    
    canvas3D2->cd();
    TLegend* legend = GetAndSetLegend2( 0.6, 0.89, 0.75, 0.97, 0.85*textSizeRel, 1, "", 42, 0.2);
    legend->AddEntry(h1ZAll, "all cells", "pl");
    legend->AddEntry(h1ZLocTrigg, "local #mu triggered", "pl");
    legend->AddEntry(h1ZRemain, "remaining cells", "pl");
    legend->Draw();
    
    canvas3D2->SaveAs( Form("%s%06i.%s", outputName.Data(), evtNr, suffix.Data()));
    
    delete padEvt[0];
    delete padEvt[1];
    delete padEvt[2];
    delete padEvt[3];
    delete legend;
    delete canvas3D2;
    return;
  }

//__________________________________________________________________________________________________________
  // Simple event display with different highlighted triggers
  //__________________________________________________________________________________________________________
  void EventDisplayWithSlice( TH3F* h3All, TH1D* h1XAll, TH1D* h1YAll, TH1D* h1ZAll, 
                              Int_t evtNr, Float_t etot, Float_t maxE, 
                              Float_t maxEX, Float_t maxEY, Float_t maxEZ, bool ktrigg,
                              RunInfo currRunInfo, TString outputName, TString suffix = "pdf", TString unit = "mip eq/tile"
                            ){
    Double_t textSizeRel = 0.035;
    Double_t textSizeSubpad = 0.06;
    
    TCanvas* canvas3D2 = new TCanvas("canvas3D2","",0,0,1400,750);  // gives the page size
    // DefaultCanvasSettings( canvas3D2, 0.05, 0.25, 0.05, 0.1);
    TPad* padEvt[4];
    padEvt[0] = new TPad("pad_0", "", 0, 0, 0.75, 0.9,-1, -1, -2);
    padEvt[1] = new TPad("pad_1", "", 0.75, 0.66, 1, 1,-1, -1, -2);
    padEvt[2] = new TPad("pad_2", "", 0.75, 0.33, 1, 0.66,-1, -1, -2);
    padEvt[3] = new TPad("pad_3", "", 0.75, 0., 1., 0.33,-1, -1, -2);
    
    DefaultPadSettings( padEvt[0], 0.04, 0.02, 0.0, 0.1);
    padEvt[0]->SetFillStyle(4000);
    padEvt[0]->SetLineStyle(0);
    DefaultPadSettings( padEvt[1], 0.12, 0.015, 0.02, 0.12);
    padEvt[1]->SetFillStyle(4000);
    padEvt[1]->SetLineStyle(0);
    DefaultPadSettings( padEvt[2], 0.12, 0.015, 0.02, 0.12);
    padEvt[2]->SetFillStyle(4000);
    padEvt[2]->SetLineStyle(0);
    DefaultPadSettings( padEvt[3], 0.12, 0.015, 0.02, 0.12);
    padEvt[3]->SetFillStyle(4000);
    padEvt[3]->SetLineStyle(0);
    
    canvas3D2->Draw();
    canvas3D2->cd();
    
    DrawLatex(0.01, 0.95, Form("#it{#bf{LFHCal TB}}: #it{%s}",GetStringFromRunInfo(currRunInfo, 6).Data()), false, textSizeRel, 42);
    DrawLatex(0.01, 0.92, GetStringFromRunInfo(currRunInfo, 1), false, 0.85*textSizeRel, 42);
    if(ktrigg) DrawLatex(0.01, 0.89, Form("Event %d, muon triggered",evtNr), false, 0.85*textSizeRel, 42);
    else DrawLatex(0.01, 0.89, Form("Event %d",evtNr), false, 0.85*textSizeRel, 42);
    DrawLatex(0.01, 0.86, Form("#it{E}_{tot} = %.2f %s", etot, unit.Data()), false, 0.85*textSizeRel, 42);
    DrawLatex(0.01, 0.83, Form("#it{E}_{max,cell} = %.2f %s", maxE, unit.Data()), false, 0.85*textSizeRel, 42);
    
    padEvt[0]->Draw();
    padEvt[0]->cd();
    padEvt[0]->SetTheta(50);
    padEvt[0]->SetPhi(40);
    
    SetStyleHistoTH3ForGraphs(h3All, "#it{z} (cm)  ", " #it{x} (cm)","#it{y} (cm)", 0.65*textSizeRel,0.85*textSizeRel, 0.65*textSizeRel,0.85*textSizeRel, 0.65*textSizeRel,0.85*textSizeRel, 1.5, 1.3, 0.7, 510, 505, 502);
    h3All->GetXaxis()->SetLabelOffset(-0.004);
    h3All->GetYaxis()->SetLabelOffset(-0.002);
    h3All->SetMaximum(maxE);
    h3All->SetFillColor(kBlue+1);
    h3All->SetLineColor(kBlue+1);
    h3All->Draw("box1");
    
    canvas3D2->cd();
    padEvt[1]->Draw();
    padEvt[1]->cd();
    SetStyleHistoTH1ForGraphs( h1XAll, "#it{x} (cm)", Form("#it{E} (%s)",unit.Data()), 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
    SetMarkerDefaults(h1XAll, 24, 1, kBlue+1, kBlue+1, kFALSE);   
    h1XAll->GetYaxis()->SetRangeUser(0.,1.1*maxEX);
    h1XAll->Draw("pe");
    canvas3D2->cd();

    padEvt[2]->Draw();
    padEvt[2]->cd();
    SetStyleHistoTH1ForGraphs( h1YAll, "#it{y} (cm)", Form("#it{E} (%s)",unit.Data()), 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
    SetMarkerDefaults(h1YAll, 24, 1, kBlue+1, kBlue+1, kFALSE);   
    h1YAll->GetYaxis()->SetRangeUser(0.,1.1*maxEY);
    h1YAll->Draw("pe");
    
    canvas3D2->cd();
    padEvt[3]->Draw();
    padEvt[3]->cd();
    SetStyleHistoTH1ForGraphs( h1ZAll, "#it{z} (cm)", Form("#it{E} (%s)",unit.Data()), 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
    SetMarkerDefaults(h1ZAll, 24, 1, kBlue+1, kBlue+1, kFALSE);   
    h1ZAll->GetYaxis()->SetRangeUser(0.,1.1*maxEZ);
    h1ZAll->Draw("pe");
    
    canvas3D2->cd();
    canvas3D2->SaveAs( Form("%s%06i.%s", outputName.Data(), evtNr, suffix.Data()));
    
    delete padEvt[0];
    delete padEvt[1];
    delete padEvt[2];
    delete padEvt[3];
    delete canvas3D2;
    return;
  }    
  
  //__________________________________________________________________________________________________________
  // Plot 1D distribution
  //__________________________________________________________________________________________________________  
  void PlotCalibRunOverlay( TCanvas* canvas2D, Int_t option, 
                            std::map<int, CalibSummary> sumRuns, 
                            Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                            //int labelOpt = 1,
                            TString additionalLabel = "", int debug = 0
                            ){
      
    Double_t minY         = 0;
    Double_t maxY         = 0;
    Double_t minX         = 9999;
    Double_t maxX         = 0;
    bool isSameVoltage    = true;
    double commanVoltage  = 0;
    
    std::map<int, CalibSummary>::iterator itrun;
    Int_t nruns = 0;
    for(itrun=sumRuns.begin(); itrun!=sumRuns.end(); ++itrun){
      TH1D* tempH = nullptr; 
      if (option==0) tempH = itrun->second.GetHGped();
      else if (option==1) tempH = itrun->second.GetHGpedwidth();
      else if (option==2) tempH = itrun->second.GetLGped();
      else if (option==3) tempH = itrun->second.GetLGpedwidth();
      else if (option==4) tempH = itrun->second.GetHGScale();
      else if (option==5) tempH = itrun->second.GetHGScalewidth();
      else if (option==6) tempH = itrun->second.GetLGScale();
      else if (option==7) tempH = itrun->second.GetLGScalewidth();
      else if (option==8) tempH = itrun->second.GetLGHGcorr();
      else if (option==9) tempH = itrun->second.GetHGLGcorr();
      else if (option==10) tempH = itrun->second.GetLGScaleCalc();
      else if (option==11) tempH = itrun->second.GetLGHGOffcorr();
      else if (option==12) tempH = itrun->second.GetHGLGOffcorr();
      if (maxY < tempH->GetMaximum()) maxY = tempH->GetMaximum();
      if ( maxX < FindLastBinXAboveMin(tempH)) maxX = FindLastBinXAboveMin(tempH);
      if ( minX > FindFirstBinXAboveMin(tempH)) minX = FindFirstBinXAboveMin(tempH);
      if (nruns==0){
        commanVoltage = itrun->second.GetVoltage();
      } else {
        if (commanVoltage != itrun->second.GetVoltage())  isSameVoltage = false;
      }
      nruns++;
    }
    // std::cout << "min X\t"  << minX << "\t max X \t" << maxX << std::endl;
    
    TString label2          = Form("Common V_{op} = %2.1f V", commanVoltage);
    canvas2D->cd();
        
      TH1D* histos[30];
      if (debug > 0){
        if (nruns > 30) std::cout << "more than 30 runs are included in this, only 30 will be plotted, currently " << nruns << "\t runs were requested" << std::endl;
        else std::cout << nruns << " will be plotted" << std::endl;
      }
      double lineBottom  = 6;
      if (nruns < 6) lineBottom = 1;
      else if (nruns < 11) lineBottom = 2;
      else if (nruns < 16) lineBottom = 3;
      else if (nruns < 21) lineBottom = 4;
      else if (nruns < 26) lineBottom = 5;
      TLegend* legend = GetAndSetLegend2( 0.55, 0.88-lineBottom*textSizeRel, 0.95, 0.88,
                                          0.85*textSizeRel, 5, "",42,0.35);;
      int currRun = 0;
      for(itrun=sumRuns.begin(); (itrun!=sumRuns.end()) && (currRun < 30); ++itrun){
        histos[currRun] = nullptr;
        if (option==0) histos[currRun] = itrun->second.GetHGped();
        else if (option==1) histos[currRun]  = itrun->second.GetHGpedwidth();
        else if (option==2) histos[currRun]  = itrun->second.GetLGped();
        else if (option==3) histos[currRun]  = itrun->second.GetLGpedwidth();
        else if (option==4) histos[currRun]  = itrun->second.GetHGScale();
        else if (option==5) histos[currRun]  = itrun->second.GetHGScalewidth();
        else if (option==6) histos[currRun]  = itrun->second.GetLGScale();
        else if (option==7) histos[currRun]  = itrun->second.GetLGScalewidth();
        else if (option==8) histos[currRun]  = itrun->second.GetLGHGcorr();
        else if (option==9) histos[currRun]  = itrun->second.GetHGLGcorr();
        else if (option==10) histos[currRun] = itrun->second.GetLGScaleCalc();
        else if (option==11) histos[currRun] = itrun->second.GetLGHGOffcorr();
        else if (option==12) histos[currRun] = itrun->second.GetHGLGOffcorr();
        SetStyleHistoTH1ForGraphs( histos[currRun], histos[currRun]->GetXaxis()->GetTitle(), histos[currRun]->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.95, 1.02);  
        SetLineDefaults(histos[currRun], GetColorLayer(currRun), 4, GetLineStyleLayer(currRun));   
        if(currRun == 0){
          histos[currRun]->GetXaxis()->SetRangeUser(minX-5*histos[currRun]->GetBinWidth(1),maxX+5*histos[currRun]->GetBinWidth(1));
          histos[currRun]->GetYaxis()->SetRangeUser(minY,maxY*1.1);
          histos[currRun]->Draw("hist");
        } else {
          histos[currRun]->Draw("same,hist");
        }
        legend->AddEntry(histos[currRun],Form("%d",itrun->first),"l");
        currRun++;  
      }  
      histos[0]->DrawCopy("axis,same");
      legend->Draw();
      
      DrawLatex(0.95, 0.92, Form("#it{#bf{LFHCal TB:} %s}",GetStringFromRunInfo(currRunInfo,7).Data()), true, 0.85*textSizeRel, 42);
      DrawLatex(0.95, 0.885, GetStringFromRunInfo(currRunInfo,8), true, 0.85*textSizeRel, 42);
      if (isSameVoltage)
        DrawLatex(0.95, 0.88-0.5*0.85*textSizeRel-lineBottom*textSizeRel , label2, true, 0.85*textSizeRel, 42);
      
    canvas2D->SaveAs(nameOutput.Data());
  }  
  
  //__________________________________________________________________________________________________________
  // Plot 1D distribution
  //__________________________________________________________________________________________________________  
  void PlotAnalysisComparison( TCanvas* canvas2D, Int_t option, 
                            std::map<int, AnaSummary> sumRuns, 
                            Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                            int labelOpt = 1,
                            TString additionalLabel = "", int debug = 0
                            ){
    //hardcode max X
    Double_t minY         = 0.1;
    Double_t maxY         = 0;
    Double_t minX         = 9999;
    // Double_t minX         = 0;
    Double_t maxX         = 0;
    bool isSameVoltage    = true;
    double commanVoltage  = 0;
    
    std::map<int, AnaSummary>::iterator itrun;
    Int_t nruns = 0;
    for(itrun=sumRuns.begin(); itrun!=sumRuns.end(); ++itrun){
      TH1D* tempH; 
      if (option==0) tempH = itrun->second.GetDeltaTimeHist();
      else if (option==1) tempH = itrun->second.GetEnergyHist();
      else if (option==2) tempH = itrun->second.GetNCellsHist();
      
      std::cout << 2./tempH->GetEntries() << std::endl;
      if (maxY < tempH->GetMaximum()) maxY = tempH->GetMaximum();
      if ( maxX < FindLastBinXAboveMin(tempH,2./tempH->GetEntries())) maxX = FindLastBinXAboveMin(tempH,2./tempH->GetEntries());
      if ( minX > FindFirstBinXAboveMin(tempH,2./tempH->GetEntries())) minX = FindFirstBinXAboveMin(tempH,2./tempH->GetEntries());      
      if (minY > 2./tempH->GetEntries()) minY = 2./tempH->GetEntries();
      // std::cout << "min X\t"  << minX << "\t max X \t" << maxX << std::endl;
      if (nruns==0){
        commanVoltage = itrun->second.GetVoltage();
      } else {
        if (commanVoltage != itrun->second.GetVoltage())  isSameVoltage = false;
      }
      nruns++;
    }
    std::cout << "min X\t"  << minX << "\t max X \t" << maxX << std::endl;
    std::cout << "min Y\t"  << minY << "\t max Y \t" << maxY << std::endl;
    
    TString label2          = Form("Common V_{op} = %2.1f V", commanVoltage);
    canvas2D->cd();  
      TH1D* histos[30];
      if (debug > 0){
        if (nruns > 30) std::cout << "more than 30 runs are included in this, only 30 will be plotted, currently " << nruns << "\t runs were requested" << std::endl;
        else std::cout << nruns << " will be plotted" << std::endl;
      }
      double startLegend = 0.55;
      int columns  = 5;
      double columnwidth = 0.35;
      if (labelOpt == 2){
        columns     = 4;
        startLegend = 0.45;
        columnwidth = 0.17;
      }

      double lineBottom  = 6;
      if (nruns < columns+1) lineBottom = 1;
      else if (nruns < 2*columns+1) lineBottom = 2;
      else if (nruns < 3*columns+1) lineBottom = 3;
      else if (nruns < 4*columns+1) lineBottom = 4;
      else if (nruns < 5*columns+1) lineBottom = 5;

      TLegend* legend = GetAndSetLegend2( startLegend, 0.88-lineBottom*textSizeRel, 0.95, 0.88,
                                          0.75*textSizeRel, columns, "",42,columnwidth);;
      int currRun = 0;
      for(itrun=sumRuns.begin(); (itrun!=sumRuns.end()) && (currRun < 30); ++itrun){
        histos[currRun] = nullptr;
        if (option==0) histos[currRun] = itrun->second.GetDeltaTimeHist();
        else if (option==1){
          histos[currRun] = itrun->second.GetEnergyHist();
          histos[currRun]->Rebin(4);
        } else if (option==2){
          histos[currRun] = itrun->second.GetNCellsHist();
        }
        SetStyleHistoTH1ForGraphs( histos[currRun], histos[currRun]->GetXaxis()->GetTitle(), histos[currRun]->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.95, 1.02);  
        SetLineDefaults(histos[currRun], GetColorLayer(currRun), 4, GetLineStyleLayer(currRun));   
        if(currRun == 0){
          histos[currRun]->GetXaxis()->SetRangeUser(minX-5*histos[currRun]->GetBinWidth(1),maxX+5*histos[currRun]->GetBinWidth(1));
          if (option == 1)
            histos[currRun]->GetYaxis()->SetRangeUser(minY*4,maxY*4.4);
          else 
            histos[currRun]->GetYaxis()->SetRangeUser(minY,maxY*1.1);
          histos[currRun]->Draw("hist");
        } else {
          histos[currRun]->Draw("same,hist");
        }
        if (labelOpt == 2){
          TString species = GetSpeciesStringFromPDG(itrun->second.GetPDG());
          legend->AddEntry(histos[currRun],Form("%s %1.f GeV",species.Data(), itrun->second.GetEnergy()),"l");
        } else {
          legend->AddEntry(histos[currRun],Form("%d",itrun->second.GetRunNumber()),"l");
        }
        currRun++;  
      }  
      histos[0]->DrawCopy("axis,same");
      legend->Draw();
      
      DrawLatex(0.95, 0.92, Form("#it{#bf{LFHCal TB:} %s}",GetStringFromRunInfo(currRunInfo,7).Data()), true, 0.85*textSizeRel, 42);
      DrawLatex(0.95, 0.885, GetStringFromRunInfo(currRunInfo,8), true, 0.85*textSizeRel, 42);
      if (isSameVoltage)
        DrawLatex(0.95, 0.88-0.5*0.85*textSizeRel-lineBottom*textSizeRel , label2, true, 0.85*textSizeRel, 42);
      
    canvas2D->SaveAs(nameOutput.Data());
  }  

#endif
