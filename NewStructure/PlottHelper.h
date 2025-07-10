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
  //__________________________________________________________________________________________________________
  //__________________________________________________________________________________________________________
  //__________________________________________________________________________________________________________
  TString ReturnDateStr(){
      TDatime today;
      int iDate           = today.GetDate();
      int iYear           = iDate/10000;
      int iMonth          = (iDate%10000)/100;
      int iDay            = iDate%100;
      return Form("%i_%02d_%02d",iYear, iMonth, iDay);
  }


  //__________________________________________________________________________________________________________
  // find bin with largest content
  //__________________________________________________________________________________________________________
  Double_t FindLargestBin1DHist(TH1* hist, Double_t minX = -10000, Double_t maxX = -10000 ){
    Double_t largestContent     = 0;
    if (!hist){
        std::cout << "histogram pointer was empty, skipping!" << std::endl;
        return 0.;
    }
    Int_t minBin = 1;
    Int_t maxBin = hist->GetNbinsX()+1;
    if (minX != -10000) minBin = hist->GetXaxis()->FindBin(minX);
    if (maxX != -10000) maxBin = hist->GetXaxis()->FindBin(maxX)+0.0001;
    for (Int_t i= minBin; i < maxBin; i++){
        if (largestContent < hist->GetBinContent(i)){
            largestContent = hist->GetBinContent(i);
        }
    }
    return largestContent;
  }
  //__________________________________________________________________________________________________________
  // find bin with smallest content
  //__________________________________________________________________________________________________________
  Double_t FindSmallestBin1DHist(TH1* hist, Double_t maxStart = 1e6 ){
    Double_t smallesContent     = maxStart;
    for (Int_t i= 0; i < hist->GetNbinsX(); i++){
      if (hist->GetBinContent(i) != 0 && smallesContent > hist->GetBinContent(i)){
        smallesContent = hist->GetBinContent(i);
      }
    }
    return smallesContent;
  }

  //__________________________________________________________________________________________________________
  // find bin last filled X bin
  //__________________________________________________________________________________________________________
  Double_t FindLastBinXAboveMin(TH1* hist, Double_t min = 1 ){
    int i = hist->GetNbinsX();
    while (i > 0 && hist->GetBinContent(i) < min) i--;
    if (i != 1)
      return hist->GetBinCenter(i+1);
    else 
      return hist->GetBinCenter(hist->GetNbinsX()-1);
  }

  //__________________________________________________________________________________________________________
  // find bin first filled X bin
  //__________________________________________________________________________________________________________
  Double_t FindFirstBinXAboveMin(TH1* hist, Double_t min = 1 ){
    int i = 1;
    while (i < hist->GetNbinsX() && hist->GetBinContent(i) < min) i++;
    if (i != hist->GetNbinsX()-1)
      return hist->GetBinCenter(i+1);
    else 
      return hist->GetBinCenter(1);
  }
  
  
  
  // ---------------------------- Function definiton --------------------------------------------------------------------------------------------
  // StyleSettingsBasics will make some standard settings for gStyle
  
  void StyleSettingsBasics( TString format = ""){
      //gStyle->SetOptTitle(kFALSE);
      gStyle->SetOptDate(0);   //show day and time
      gStyle->SetOptStat(0);  //show statistic
      gStyle->SetPalette(1,0);
      gStyle->SetFrameBorderMode(0);
      gStyle->SetFrameFillColor(0);
      gStyle->SetTitleFillColor(0);
      gStyle->SetTextSize(0.5);
      gStyle->SetLabelSize(0.03,"xyz");
      gStyle->SetLabelOffset(0.006,"xyz");
      gStyle->SetTitleFontSize(0.04);
      gStyle->SetTitleOffset(1,"y");
      gStyle->SetTitleOffset(0.7,"x");
      gStyle->SetCanvasColor(0);
      gStyle->SetPadTickX(1);
      gStyle->SetPadTickY(1);
      // gStyle->SetPadTickZ(1);
      gStyle->SetLineWidth(1);
      gStyle->SetPaintTextFormat(".3f");
      
      gStyle->SetPadTopMargin(0.03);
      gStyle->SetPadBottomMargin(0.09);
      gStyle->SetPadRightMargin(0.03);
      gStyle->SetPadLeftMargin(0.13);


      TGaxis::SetMaxDigits(4);
      gErrorIgnoreLevel=kError;

      if (format.CompareTo("eps") == 0 ||format.CompareTo("pdf") == 0  ) gStyle->SetLineScalePS(1);
  }

  //__________________________________________________________________________________________________________
  void SetPlotStyle() {
  // 	const Int_t nRGBs = 7;
      const Int_t nRGBs = 5;
      const Int_t nCont = 255;

      Double_t stops[nRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
      Double_t red[nRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
      Double_t green[nRGBs] = { 0.31, 0.81, 1.00, 0.20, 0.00 };
      Double_t blue[nRGBs]  = { 0.51, 1., 0.12, 0.00, 0.00};

      TColor::CreateGradientColorTable(nRGBs, stops, red, green, blue, nCont);
      gStyle->SetNumberContours(nCont);
  }

  //__________________________________________________________________________________________________________
  void SetPlotStyleNConts(    Int_t nCont = 255) {
      const Int_t nRGBs = 5;
      Double_t stops[nRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
      Double_t red[nRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
      Double_t green[nRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
      Double_t blue[nRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
      TColor::CreateGradientColorTable(nRGBs, stops, red, green, blue, nCont);
      gStyle->SetNumberContours(nCont);
  }
  
  //__________________________________________________________________________________________________________  
  Color_t GetColorLayer(int l){
    Color_t colors[10] = {kBlack, kViolet+4, kBlue-3, kCyan+1, kGreen+1, kYellow-4, kOrange, kRed-4, kPink-5, kMagenta+2 };
    return colors[l%10];
  }

  //__________________________________________________________________________________________________________  
  Style_t GetLineStyleLayer(int l){
    Style_t styles[7] = {1, 3, 4, 6, 7, 10, 9};
    int bin = l/10;
    return styles[bin];
  }
  
  //__________________________________________________________________________________________________________
  void DrawCanvasSettings( TCanvas* c1,
                          Double_t leftMargin,
                          Double_t rightMargin,
                          Double_t topMargin,
                          Double_t bottomMargin){

      c1->SetTickx();
      c1->SetTicky();
      c1->SetGridx(0);
      c1->SetGridy(0);
      c1->SetLogy(0);
      c1->SetLeftMargin(leftMargin);
      c1->SetRightMargin(rightMargin);
      c1->SetTopMargin(topMargin);
      c1->SetBottomMargin(bottomMargin);
      c1->SetFillColor(0);
  }

  //__________________________________________________________________________________________________________
  TCanvas *GetAndSetCanvas( TString name,
                            Double_t leftmargin = 0.11,
                            Double_t bottommargin = 0.1,
                            Double_t x = 1400,
                            Double_t y = 1000){

      TCanvas *canvas =  new TCanvas(name,name,x,y);
      canvas->SetLeftMargin(leftmargin);
      canvas->SetRightMargin(0.015);
      canvas->SetTopMargin(0.03);
      canvas->SetBottomMargin(bottommargin);
      canvas->SetFillColor(0);

      return canvas;

  }

  //__________________________________________________________________________________________________________
  TLegend *GetAndSetLegend( Double_t positionX,
                          Double_t positionY,
                          Double_t entries,
                          Int_t Columns = 1,
                          TString header =""){

      if(header.CompareTo("") != 0) entries++;
      Double_t positionYPlus = 0.04*1.1*(Double_t)entries;
      TLegend *legend = new TLegend(positionX,positionY,positionX+(0.25*Columns),positionY+positionYPlus);
      legend->SetNColumns(Columns);
      legend->SetLineColor(0);
      legend->SetLineWidth(0);
      legend->SetFillColor(0);
      legend->SetFillStyle(0);
      legend->SetLineStyle(0);
      legend->SetTextSize(0.04);
      legend->SetTextFont(42);
      if(header.CompareTo("") != 0)legend->SetHeader(header);
      return legend;
  }
  
  //__________________________________________________________________________________________________________
  TLegend *GetAndSetLegend2(  Double_t positionX,
                              Double_t positionY,
                              Double_t positionXRight,
                              Double_t positionYUp,
                              Size_t textSize,
                              Int_t columns               = 1,
                              TString header              = "",
                              Font_t textFont             = 43,
                              Double_t margin             = 0
  ){

      TLegend *legend = new TLegend(positionX,positionY,positionXRight,positionYUp);
      legend->SetNColumns(columns);
      legend->SetLineColor(0);
      legend->SetLineWidth(0);
      legend->SetFillColor(0);
      legend->SetFillStyle(0);
      legend->SetLineStyle(0);
      legend->SetBorderSize(0);
      legend->SetTextFont(textFont);
      legend->SetTextSize(textSize);
      if (margin != 0) legend->SetMargin(margin);
      if (header.CompareTo("")!= 0) legend->SetHeader(header);
      return legend;
  }

  //__________________________________________________________________________________________________________
  void SetHistogramm( TH1 *hist,
                      TString xLabel,
                      TString yLabel,
                      Double_t rangeYlow  = -99.,
                      Double_t rangeYhigh = -99.,
                      Double_t xOffset    = 1.0,
                      Double_t yOffset    = 1.15,
                      Font_t font         = 42
  ){

      Double_t scale = 1./gPad->GetAbsHNDC();
      //hist->GetXaxis()->SetRangeUser(rangeX[0],rangeX[1]);
      if(rangeYlow != -99.) hist->GetYaxis()->SetRangeUser(rangeYlow,rangeYhigh);
      hist->SetTitle("");
      hist->SetXTitle(xLabel);
      hist->SetYTitle(yLabel);
      hist->GetYaxis()->SetDecimals();
      hist->GetYaxis()->SetTitleOffset(yOffset/scale);
      hist->GetXaxis()->SetTitleOffset(xOffset);
      hist->GetXaxis()->SetTitleSize(0.04*scale);
      hist->GetYaxis()->SetTitleSize(0.04*scale);
      hist->GetXaxis()->SetLabelSize(0.035*scale);
      hist->GetYaxis()->SetLabelSize(0.035*scale);
      hist->GetXaxis()->SetLabelFont(font);
      hist->GetYaxis()->SetLabelFont(font);
      hist->SetMarkerSize(1.);
      hist->SetMarkerStyle(20);
  }

  //__________________________________________________________________________________________________________
  void SetGraph( TGraph *graph,
                  TString xLabel,
                  TString yLabel,
                  Double_t rangeYlow = -99.,
                  Double_t rangeYhigh = -99.,
                  Double_t xOffset = 1.0,
                  Double_t yOffset = 1.15){

      Double_t scale = 1./gPad->GetAbsHNDC();
      //graph->GetXaxis()->SetRangeUser(rangeX[0],rangeX[1]);
      if(rangeYlow != -99.) graph->GetYaxis()->SetRangeUser(rangeYlow,rangeYhigh);
      graph->GetXaxis()->SetTitle(xLabel);
      graph->GetYaxis()->SetTitle(yLabel);
      graph->GetYaxis()->SetDecimals();
      graph->GetYaxis()->SetTitleOffset(yOffset/scale);
      graph->GetXaxis()->SetTitleOffset(xOffset);
      graph->GetXaxis()->SetTitleSize(0.04*scale);
      graph->GetYaxis()->SetTitleSize(0.04*scale);
      graph->GetXaxis()->SetLabelSize(0.035*scale);
      graph->GetYaxis()->SetLabelSize(0.035*scale);
      graph->GetXaxis()->SetLabelFont(42);
      graph->GetYaxis()->SetLabelFont(42);
      graph->SetMarkerSize(1.);
      graph->SetMarkerStyle(20);
  }

  //__________________________________________________________________________________________________________
  void SetMarkerDefaults(    TH1* histo1,
                              Style_t markerStyle,
                              Size_t markerSize,
                              Color_t markerColor,
                              Color_t lineColor,
                              Bool_t setFont = kTRUE) {
      histo1->SetMarkerStyle(markerStyle);
      histo1->SetMarkerSize(markerSize);
      histo1->SetMarkerColor(markerColor);
      histo1->SetLineColor(lineColor);
      if (setFont){
        histo1->GetYaxis()->SetLabelFont(42);
        histo1->GetXaxis()->SetLabelFont(42);
        histo1->GetYaxis()->SetTitleFont(62);
        histo1->GetXaxis()->SetTitleFont(62);
      }
  }
  //__________________________________________________________________________________________________________
  void SetMarkerDefaults(    TH1* histo1,
                              TString xtitle = "",
                              TString ytitle = "",
                              Style_t markerStyle = 20,
                              Size_t markerSize = 1,
                              Color_t markerColor = kBlack,
                              Color_t lineColor = kBlack,
                              double textsize = 0.045,
                              double labelsize = 0.045,
                              double xoffset = 1.,
                              double yoffset = 1. ) {
      histo1->SetTitle("");
      histo1->SetStats(0);
      histo1->SetMarkerStyle(markerStyle);
      histo1->SetMarkerSize(markerSize);
      histo1->SetMarkerColor(markerColor);
      histo1->SetLineColor(lineColor);
      histo1->GetYaxis()->SetLabelFont(42);
      histo1->GetXaxis()->SetLabelFont(42);
      histo1->GetYaxis()->SetLabelSize(labelsize);
      histo1->GetXaxis()->SetLabelSize(labelsize);
      histo1->GetYaxis()->SetTitleFont(62);
      histo1->GetXaxis()->SetTitleFont(62);
      histo1->GetYaxis()->SetTitleSize(textsize);
      histo1->GetXaxis()->SetTitleSize(textsize);
      if(!xtitle.EqualTo("")) histo1->GetXaxis()->SetTitle(xtitle);
      if(!ytitle.EqualTo("")) histo1->GetYaxis()->SetTitle(ytitle);
      histo1->GetXaxis()->SetTitleOffset(xoffset);
      histo1->GetYaxis()->SetTitleOffset(yoffset);
  }
  //__________________________________________________________________________________________________________
  void SetMarkerDefaultsProfile( TProfile* prof,
                                  Style_t markerStyle,
                                  Size_t markerSize,
                                  Color_t markerColor,
                                  Color_t lineColor ) {
      prof->SetMarkerStyle(markerStyle);
      prof->SetMarkerSize(markerSize);
      prof->SetMarkerColor(markerColor);
      prof->SetLineColor(lineColor);
      prof->GetYaxis()->SetLabelFont(42);
      prof->GetXaxis()->SetLabelFont(42);
      prof->GetYaxis()->SetTitleFont(62);
      prof->GetXaxis()->SetTitleFont(62);
  }

  //__________________________________________________________________________________________________________
  void SetLineDefaults(    TH1* histo1,
                              Int_t LineColor,
                              Int_t LineWidth,
                              Int_t LineStyle ) {
      histo1->SetLineColor(LineColor);
      histo1->SetMarkerColor(LineColor);
      histo1->SetLineWidth(LineWidth);
      histo1->SetLineStyle(LineStyle);
  }

  //__________________________________________________________________________________________________________
  void SetLineDefaultsTF1(  TF1*  Fit1,
                              Int_t LineColor,
                              Int_t LineWidth,
                              Int_t LineStyle ) {
      Fit1->SetLineColor(LineColor);
      Fit1->SetLineWidth(LineWidth);
      Fit1->SetLineStyle(LineStyle);
  }
  

    //__________________________________________________________________________________________________________
  void DefaultCancasSettings( TCanvas* c1,
                              Double_t leftMargin,
                              Double_t rightMargin,
                              Double_t topMargin,
                              Double_t bottomMargin){
      c1->SetTickx();
      c1->SetTicky();
      c1->SetGridx(0);
      c1->SetGridy(0);
      c1->SetLogy(0);
      c1->SetLeftMargin(leftMargin);
      c1->SetRightMargin(rightMargin);
      c1->SetTopMargin(topMargin);
      c1->SetBottomMargin(bottomMargin);
      c1->SetFillColor(0);
  }

  //__________________________________________________________________________________________________________
  void DefaultPadSettings( TPad* pad1,
                          Double_t leftMargin,
                          Double_t rightMargin,
                          Double_t topMargin,
                          Double_t bottomMargin){
      pad1->SetFillColor(0);
      pad1->GetFrame()->SetFillColor(0);
      pad1->SetBorderMode(0);
      pad1->SetLeftMargin(leftMargin);
      pad1->SetBottomMargin(bottomMargin);
      pad1->SetRightMargin(rightMargin);
      pad1->SetTopMargin(topMargin);
      pad1->SetTickx();
      pad1->SetTicky();
  }

  //__________________________________________________________________________________________________________
  void SetMarkerDefaultsTGraph(  TGraph* graph,
                                  Style_t markerStyle,
                                  Size_t markerSize,
                                  Color_t markerColor,
                                  Color_t lineColor,
                                  Width_t lineWidth       = 1,
                                  Style_t lineStyle       = 1,
                                  Bool_t boxes            = kFALSE,
                                  Color_t fillColor       = 0,
                                  Bool_t isHollow         = kFALSE
                                ) {
      graph->SetMarkerStyle(markerStyle);
      graph->SetMarkerSize(markerSize);
      graph->SetMarkerColor(markerColor);
      graph->SetLineColor(lineColor);
      graph->SetLineWidth(lineWidth);
      graph->SetLineWidth(lineStyle);
      if (boxes){
          graph->SetFillColor(fillColor);
          if (fillColor!=0){
              if (!isHollow){
                  graph->SetFillStyle(1001);
              } else {
                  graph->SetFillStyle(0);
              }
          } else {
              graph->SetFillStyle(0);
          }
      }
  }

  //__________________________________________________________________________________________________________
  void SetMarkerDefaultsTGraphErr(   TGraphErrors* graph,
                                      Style_t markerStyle,
                                      Size_t markerSize,
                                      Color_t markerColor,
                                      Color_t lineColor,
                                      Width_t lineWidth       = 1,
                                      Bool_t boxes            = kFALSE,
                                      Color_t fillColor       = 0,
                                      Bool_t isHollow         = kFALSE) {
      graph->SetMarkerStyle(markerStyle);
      graph->SetMarkerSize(markerSize);
      graph->SetMarkerColor(markerColor);
      graph->SetLineColor(lineColor);
      graph->SetLineWidth(lineWidth);
      if (boxes){
          graph->SetFillColor(fillColor);
          if (fillColor!=0){
              if (!isHollow){
                  graph->SetFillStyle(1001);
              } else {
                  graph->SetFillStyle(0);
              }
          } else {
              graph->SetFillStyle(0);
          }
      }
  }

  //__________________________________________________________________________________________________________
  void SetMarkerDefaultsTGraphAsym(  TGraphAsymmErrors* graph,
                                      Style_t markerStyle,
                                      Size_t markerSize,
                                      Color_t markerColor,
                                      Color_t lineColor,
                                      Width_t lineWidth   =1,
                                      Bool_t boxes        = kFALSE,
                                      Color_t fillColor   = 0,
                                      Bool_t isHollow     = kFALSE
                                    ) {
      if (!graph) return;
      graph->SetMarkerStyle(markerStyle);
      graph->SetMarkerSize(markerSize);
      graph->SetMarkerColor(markerColor);
      graph->SetLineColor(lineColor);
      graph->SetLineWidth(lineWidth);
      if (boxes){
          graph->SetFillColor(fillColor);
          if (fillColor!=0){
              if (!isHollow){
                  graph->SetFillStyle(1001);
              } else {
                  graph->SetFillStyle(0);
              }
          } else {
              graph->SetFillStyle(0);
          }
      }
  }

  //__________________________________________________________________________________________________________
  void SetMarkerDefaultsTF1( TF1* fit1,
                              Style_t lineStyle,
                              Size_t lineWidth,
                              Color_t lineColor ) {
      if (!fit1) return;
      fit1->SetLineColor(lineColor);
      fit1->SetLineStyle(lineStyle);
      fit1->SetLineWidth(lineWidth);
  }

  //__________________________________________________________________________________________________________
  void SetStyleTLatex( TLatex* text,
                      Size_t textSize,
                      Width_t lineWidth,
                      Color_t textColor = 1,
                      Font_t textFont = 42,
                      Bool_t kNDC = kTRUE,
                      Short_t align = 11
                  ){
      if (kNDC) {text->SetNDC();}
      text->SetTextFont(textFont);
      text->SetTextColor(textColor);
      text->SetTextSize(textSize);
      text->SetLineWidth(lineWidth);
      text->SetTextAlign(align);
  }

  //__________________________________________________________________________________________________________
  void DrawLatex(const double  PosX = 0.5, const double  PosY = 0.5, TString text = "", const bool alignRight = false, const double TextSize = 0.044, const int font = 42, const double dDist = 0.05, const int color = 1){

      std::vector<TString> Latex;
      
      TObjArray *textStr = text.Tokenize(";");
      for(Int_t i = 0; i<textStr->GetEntries() ; i++){
          TObjString* tempObj     = (TObjString*) textStr->At(i);
          Latex.push_back( tempObj->GetString());
      }
      for(unsigned int i = 0; i < Latex.size(); ++i){
          TLatex l(PosX, PosY - i*dDist, Latex[i]);
          l.SetNDC();
          l.SetTextFont(font);
          l.SetTextColor(color);
          l.SetTextSize(TextSize);
          if(alignRight) l.SetTextAlign(31);
          l.DrawClone("same");
      }
  }

  //__________________________________________________________________________________________________________
  void SetStyleHisto( TH1* histo,
                      Width_t lineWidth,
                      Style_t lineStyle,
                      Color_t lineColor) {
      if (!histo) return;
      histo->SetLineWidth(lineWidth);
      histo->SetLineStyle(lineStyle);
      histo->SetLineColor(lineColor);
  }

  //__________________________________________________________________________________________________________
  void SetStyleFit(   TF1* fit,
                      Double_t xRangeStart,
                      Double_t xRangeEnd,
                      Width_t lineWidth,
                      Style_t lineStyle,
                      Color_t lineColor) {
      if (!fit) return;
      fit->SetRange(xRangeStart,xRangeEnd);
      fit->SetLineWidth(lineWidth);
      fit->SetLineStyle(lineStyle);
      fit->SetLineColor(lineColor);
  }

  //__________________________________________________________________________________________________________
  void SetStyleHistoTH2ForGraphs( TH2* histo,
                                  TString XTitle,
                                  TString YTitle,
                                  Size_t xLableSize,
                                  Size_t xTitleSize,
                                  Size_t yLableSize,
                                  Size_t yTitleSize,
                                  Float_t xTitleOffset    = 1,
                                  Float_t yTitleOffset    = 1,
                                  Int_t xNDivisions       = 510,
                                  Int_t yNDivisions       = 510,
                                  Font_t textFontLabel    = 42,
                                  Font_t textFontTitle    = 62
                                ){
      histo->SetXTitle(XTitle);
      histo->SetYTitle(YTitle);
      histo->SetTitle("");
      histo->SetStats(0);

      histo->GetXaxis()->SetLabelFont(textFontLabel);
      histo->GetYaxis()->SetLabelFont(textFontLabel);
      histo->GetXaxis()->SetTitleFont(textFontTitle);
      histo->GetYaxis()->SetTitleFont(textFontTitle);

      histo->GetXaxis()->SetLabelSize(xLableSize);
      histo->GetXaxis()->SetTitleSize(xTitleSize);
      histo->GetXaxis()->SetTitleOffset(xTitleOffset);
      histo->GetXaxis()->SetNdivisions(xNDivisions,kTRUE);

      histo->GetYaxis()->SetDecimals();
      histo->GetYaxis()->SetLabelSize(yLableSize);
      histo->GetYaxis()->SetTitleSize(yTitleSize);
      histo->GetYaxis()->SetTitleOffset(yTitleOffset);
      histo->GetYaxis()->SetNdivisions(yNDivisions,kTRUE);
  }

  //__________________________________________________________________________________________________________
  void SetStyleHistoTH1ForGraphs( TH1* histo,
                                  TString XTitle,
                                  TString YTitle,
                                  Size_t xLableSize,
                                  Size_t xTitleSize,
                                  Size_t yLableSize,
                                  Size_t yTitleSize,
                                  Float_t xTitleOffset    = 1,
                                  Float_t yTitleOffset    = 1,
                                  Int_t xNDivisions       = 510,
                                  Int_t yNDivisions       = 510,
                                  Font_t textFontLabel    = 42,
                                  Font_t textFontTitle    = 62
                                ){
      histo->SetXTitle(XTitle);
      histo->SetYTitle(YTitle);
      histo->SetTitle("");

      histo->GetYaxis()->SetLabelFont(textFontLabel);
      histo->GetXaxis()->SetLabelFont(textFontLabel);
      histo->GetYaxis()->SetTitleFont(textFontTitle);
      histo->GetXaxis()->SetTitleFont(textFontTitle);

      histo->GetXaxis()->SetLabelSize(xLableSize);
      histo->GetXaxis()->SetTitleSize(xTitleSize);
      histo->GetXaxis()->SetTitleOffset(xTitleOffset);
      histo->GetXaxis()->SetNdivisions(xNDivisions,kTRUE);

      histo->GetYaxis()->SetDecimals();
      histo->GetYaxis()->SetLabelSize(yLableSize);
      histo->GetYaxis()->SetTitleSize(yTitleSize);
      histo->GetYaxis()->SetTitleOffset(yTitleOffset);
      histo->GetYaxis()->SetNdivisions(yNDivisions,kTRUE);
  }

//__________________________________________________________________________________________________________
void SetStyleHistoTH3ForGraphs( TH3* histo,
                                TString XTitle,
                                TString YTitle,
                                TString ZTitle,
                                Size_t xLableSize,
                                Size_t xTitleSize,
                                Size_t yLableSize,
                                Size_t yTitleSize,
                                Size_t zLableSize,
                                Size_t zTitleSize,
                                Float_t xTitleOffset    = 1,
                                Float_t yTitleOffset    = 1,
                                Float_t zTitleOffset    = 1,
                                Int_t xNDivisions       = 510,
                                Int_t yNDivisions       = 510,
                                Int_t zNDivisions       = 510,
                                Font_t textFontLabel    = 42,
                                Font_t textFontTitle    = 62
                              ){
  histo->SetXTitle(XTitle);
  histo->SetYTitle(YTitle);
  histo->SetZTitle(ZTitle);
  histo->SetTitle("");

  histo->GetXaxis()->SetLabelFont(textFontLabel);
  histo->GetYaxis()->SetLabelFont(textFontLabel);
  histo->GetZaxis()->SetLabelFont(textFontLabel);
  histo->GetXaxis()->SetTitleFont(textFontTitle);
  histo->GetYaxis()->SetTitleFont(textFontTitle);
  histo->GetZaxis()->SetTitleFont(textFontTitle);

  histo->GetXaxis()->SetDecimals();
  histo->GetXaxis()->SetLabelSize(xLableSize);
  histo->GetXaxis()->SetTitleSize(xTitleSize);
  histo->GetXaxis()->SetTitleOffset(xTitleOffset);
  histo->GetXaxis()->SetNdivisions(xNDivisions,kTRUE);

  histo->GetYaxis()->SetDecimals();
  histo->GetYaxis()->SetLabelSize(yLableSize);
  histo->GetYaxis()->SetTitleSize(yTitleSize);
  histo->GetYaxis()->SetTitleOffset(yTitleOffset);
  histo->GetYaxis()->SetNdivisions(yNDivisions,kTRUE);
  
  histo->GetZaxis()->SetDecimals();
  histo->GetZaxis()->SetLabelSize(zLableSize);
  histo->GetZaxis()->SetTitleSize(zTitleSize);
  histo->GetZaxis()->SetTitleOffset(zTitleOffset);
  histo->GetZaxis()->SetNdivisions(zNDivisions,kTRUE);
}

  //__________________________________________________________________________________________________________
  void SetStyleTProfile( TH1* histo,
                         TString XTitle,
                         TString YTitle,
                         Size_t xLableSize,
                         Size_t xTitleSize,
                         Size_t yLableSize,
                         Size_t yTitleSize,
                         Float_t xTitleOffset    = 1,
                         Float_t yTitleOffset    = 1,
                         Int_t xNDivisions       = 510,
                         Int_t yNDivisions       = 510,
                         Font_t textFontLabel    = 42,
                         Font_t textFontTitle    = 62
                        ){
      histo->SetXTitle(XTitle);
      histo->SetYTitle(YTitle);
      histo->SetTitle("");

      histo->GetYaxis()->SetLabelFont(textFontLabel);
      histo->GetXaxis()->SetLabelFont(textFontLabel);
      histo->GetYaxis()->SetTitleFont(textFontTitle);
      histo->GetXaxis()->SetTitleFont(textFontTitle);

      histo->GetXaxis()->SetLabelSize(xLableSize);
      histo->GetXaxis()->SetTitleSize(xTitleSize);
      histo->GetXaxis()->SetTitleOffset(xTitleOffset);
      histo->GetXaxis()->SetNdivisions(xNDivisions,kTRUE);

      histo->GetYaxis()->SetDecimals();
      histo->GetYaxis()->SetLabelSize(yLableSize);
      histo->GetYaxis()->SetTitleSize(yTitleSize);
      histo->GetYaxis()->SetTitleOffset(yTitleOffset);
      histo->GetYaxis()->SetNdivisions(yNDivisions,kTRUE);
  }
  
  /* // DrawLines will draw the lines in the histogram for you
  * startX - starting point of drawing in x
  * endX - end point of drawing in x
  * startY -starting point of drawing in y
  * endY - end point of drawing in y
  * linew - line width
  */
  void DrawLines(Float_t startX, Float_t endX,
                  Float_t startY, Float_t endY,
                  Float_t linew, Float_t lineColor = 4, Style_t lineStyle = 1, Float_t opacity = 1.){
      TLine * l1 = new TLine (startX,startY,endX,endY);
      l1->SetLineColor(lineColor);
      l1->SetLineWidth(linew);
      l1->SetLineStyle(lineStyle);
      if (opacity != 1.)
          l1->SetLineColorAlpha(lineColor,opacity);

      l1->Draw("same");
  }

  //********************************************************************************************************************************
  //********************************************************************************************************************************
  //********************************************************************************************************************************
  TBox* CreateBox(Color_t colorBox, Double_t xStart, Double_t yStart, Double_t xEnd, Double_t yEnd, Style_t fillStyle = 1001 ) {
      TBox* box = new TBox(xStart ,yStart , xEnd, yEnd);
      box->SetLineColor(colorBox);
      box->SetFillColor(colorBox);
      box->SetFillStyle(fillStyle);
      return box;
  }
  
  //********************************************************************************************************************************
  //********************** Returns default labeling strings  ***********************************************************************
  //********************************************************************************************************************************    
  TString GetStringFromRunInfo(RunInfo currRunInfo, Int_t option = 1){
      if (option == 1){
          if (currRunInfo.species.Contains("cosmics")){
              return  Form("cosmics, Run %d, #it{V}_{#it{op}} = %1.1f V", currRunInfo.runNr, currRunInfo.vop  );
          } else if (currRunInfo.species.CompareTo("g") == 0){
              return  Form("LED, Run %d, #it{V}_{#it{op}} = %1.1f V", currRunInfo.runNr, currRunInfo.vop  );
          } else {
              TString beam = currRunInfo.species.Data();
              if (beam.CompareTo("Muon +") == 0) beam = "#mu^{+}";
              if (beam.CompareTo("Electron") == 0) beam = "e^{-}";              
              if (beam.CompareTo("Pion -") == 0) beam = "#pi^{-}";              
              if (beam.CompareTo("Hadron +") == 0) beam = "h^{+}";              
              return  Form("%s-beam, #it{E}_{#it{b}}= %.0f GeV, Run %d, #it{V}_{#it{op}} = %1.1f V", beam.Data(), currRunInfo.energy, currRunInfo.runNr, currRunInfo.vop  );
          }
      } else if (option == 2){
          if (currRunInfo.species.CompareTo("cosmics") == 0){
              return  "cosmics";
          } else if (currRunInfo.species.CompareTo("g") == 0){
              return  "LED";
          } else {
              TString beam = currRunInfo.species.Data();
              if (beam.CompareTo("Muon +") == 0) beam = "#mu^{+}";
              if (beam.CompareTo("Electron") == 0) beam = "e^{-}";              
              if (beam.CompareTo("Pion -") == 0) beam = "#pi^{-}";              
              if (beam.CompareTo("Hadron +") == 0) beam = "h^{+}";              
              return  Form("%s-beam, #it{E}_{#it{b}}= %.0f GeV", beam.Data(), currRunInfo.energy);
          }
      } else if (option == 3){
          return Form("Run %d, #it{V}_{#it{op}} = %1.1f V", currRunInfo.runNr, currRunInfo.vop  )   ;
      } else if (option == 4){
          if (currRunInfo.species.CompareTo("cosmics") == 0){
              return Form("cosmics, Run %d, #it{V}_{#it{op}} = %1.1f V, HG = %1d, LG = %1d", currRunInfo.runNr, currRunInfo.vop, currRunInfo.hgSet, currRunInfo.lgSet);
          } else if (currRunInfo.species.CompareTo("g") == 0){
              return Form("LED, Run %d, #it{V}_{#it{op}} = %1.1f V, HG = %1d, LG = %1d", currRunInfo.runNr, currRunInfo.vop, currRunInfo.hgSet, currRunInfo.lgSet);
          } else{
              TString beam = currRunInfo.species.Data();
              if (beam.CompareTo("Muon +") == 0) beam = "#mu^{+}";
              if (beam.CompareTo("Electron") == 0) beam = "e^{-}";              
              if (beam.CompareTo("Pion -") == 0) beam = "#pi^{-}";              
              if (beam.CompareTo("Hadron +") == 0) beam = "h^{+}";                            
              return Form("%s-beam, #it{E}_{#it{b}}= %.0f GeV, Run %d, #it{V}_{#it{op}} = %1.1f V, HG = %1d, LG = %1d", beam.Data(), currRunInfo.energy, currRunInfo.runNr, currRunInfo.vop, currRunInfo.hgSet, currRunInfo.lgSet);
          }
      } else if (option == 5){
          return Form("pedestal, Run %d, #it{V}_{#it{op}} = %1.1f V", currRunInfo.runNr, currRunInfo.vop  )   ;
      } else if (option == 6){
          if (currRunInfo.facility.CompareTo("")!=0 && currRunInfo.beamline.CompareTo("")!=0 && currRunInfo.year != -1 && currRunInfo.month != -1 && currRunInfo.readout.CompareTo("")!=0)
          return Form("%s-%s, %02d-%d, %s read-out", currRunInfo.facility.Data(), currRunInfo.beamline.Data(),   currRunInfo.month, currRunInfo.year, currRunInfo.readout.Data());
          else if (currRunInfo.facility.CompareTo("")!=0 && currRunInfo.beamline.CompareTo("")!=0 && currRunInfo.year != -1 && currRunInfo.month != -1 )
          return Form("%s-%s, %02d-%d", currRunInfo.facility.Data(), currRunInfo.beamline.Data(),   currRunInfo.month, currRunInfo.year);
      } else if (option == 7){
          if (currRunInfo.facility.CompareTo("")!=0 && currRunInfo.beamline.CompareTo("")!=0 && currRunInfo.year != -1 && currRunInfo.month != -1 )
          return Form("%s-%s, %02d-%d", currRunInfo.facility.Data(), currRunInfo.beamline.Data(), currRunInfo.month, currRunInfo.year);
      } else if (option == 8){
          if ( currRunInfo.readout.CompareTo("")!=0)
          return Form("%s read-out",  currRunInfo.readout.Data());
      } else if (option == 9){
          if (currRunInfo.facility.CompareTo("")!=0 && currRunInfo.beamline.CompareTo("")!=0 )
          return Form("%s-%s", currRunInfo.facility.Data(), currRunInfo.beamline.Data());
      } else if (option == 10){
          if ( currRunInfo.year != -1 && currRunInfo.month != -1 )
          return Form("%02d-%d", currRunInfo.month, currRunInfo.year);
      }
      
      return "";
  }


  
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
      if (maxx > -10000)hist->GetXaxis()->SetRangeUser(0.5,maxx+0.1);
    
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
      if (!(setup->GetNMaxLayer()+1 == 64))
        lineBottom      = (2+4);
      else if ( setup->GetNMaxLayer()+1 == 64 && (NCellsxaxis || Posxaxis))
        lineBottom  = (2+5);
        
      if (setup->GetNMaxLayer()+1 == 64 && (NCellsxaxis || Posxaxis )){
        legend = GetAndSetLegend2( 0.11, 0.93-lineBottom*0.85*textSizeRel, 0.95, 0.93-2*0.85*textSizeRel,0.75*textSizeRel, 15, Form("Layer, #LTlayer#GT = %.2f",meanLayer), 42,0.4);
      } else if (setup->GetNMaxLayer()+1 == 64 ){
        legend = GetAndSetLegend2( 0.4, 0.93-lineBottom*0.85*textSizeRel, 0.95, 0.93-2*0.85*textSizeRel,0.75*textSizeRel, 8, Form("Layer, #LTlayer#GT = %.2f",meanLayer), 42,0.4);
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
  void ReturnCorrectValuesForCanvasScaling(   Int_t sizeX,
                                              Int_t sizeY,
                                              Int_t nCols,
                                              Int_t nRows,
                                              Double_t leftMargin,
                                              Double_t rightMargin,
                                              Double_t upperMargin,
                                              Double_t lowerMargin,
                                              Double_t* arrayBoundariesX,
                                              Double_t* arrayBoundariesY,
                                              Double_t* relativeMarginsX,
                                              Double_t* relativeMarginsY,
                                              int verbose = 1){
      Int_t realsizeX             = sizeX- (Int_t)(sizeX*leftMargin)- (Int_t)(sizeX*rightMargin);
      Int_t realsizeY             = sizeY- (Int_t)(sizeY*upperMargin)- (Int_t)(sizeY*lowerMargin);

      Int_t nPixelsLeftColumn     = (Int_t)(sizeX*leftMargin);
      Int_t nPixelsRightColumn    = (Int_t)(sizeX*rightMargin);
      Int_t nPixelsUpperColumn    = (Int_t)(sizeY*upperMargin);
      Int_t nPixelsLowerColumn    = (Int_t)(sizeY*lowerMargin);

      Int_t nPixelsSinglePlotX    = (Int_t) (realsizeX/nCols);
      Int_t nPixelsSinglePlotY    = (Int_t) (realsizeY/nRows);
      if(verbose > 0){
          std::cout << "Setup multi panel canvas"<< std::endl;
          std::cout << "X: "<< realsizeX << "\t" << nPixelsSinglePlotX << std::endl;
          std::cout << "Y: "<< realsizeY << "\t" << nPixelsSinglePlotY << std::endl;
          std::cout << "columns:"<< nPixelsLeftColumn << "\t" << nPixelsRightColumn  << "\t" << nPixelsLowerColumn << "\t" << nPixelsUpperColumn << std::endl;
      }
      Int_t pixel = 0;
      if(verbose > 1)std::cout << "boundaries X" << std::endl;
      for (Int_t i = 0; i < nCols+1; i++){
          if (i == 0){
              arrayBoundariesX[i] = 0.;
              pixel = pixel+nPixelsLeftColumn+nPixelsSinglePlotX;
          } else if (i == nCols){
              arrayBoundariesX[i] = 1.;
              pixel = pixel+nPixelsRightColumn;
          } else {
              arrayBoundariesX[i] = (Double_t)pixel/sizeX;
              pixel = pixel+nPixelsSinglePlotX;
          }
          if(verbose > 1)std::cout << "arrayBoundariesX: " << i << "\t" << arrayBoundariesX[i] << "\t" << pixel<<std::endl;
      }

      if(verbose > 1)std::cout << "boundaries Y" << std::endl;
      pixel = sizeY;
      for (Int_t i = 0; i < nRows+1; i++){
          if (i == 0){
              arrayBoundariesY[i] = 1.;
              pixel = pixel-nPixelsUpperColumn-nPixelsSinglePlotY;
          } else if (i == nRows){
              arrayBoundariesY[i] = 0.;
              pixel = pixel-nPixelsLowerColumn;
          } else {
              arrayBoundariesY[i] = (Double_t)pixel/sizeY;
              pixel = pixel-nPixelsSinglePlotY;
          }
          if(verbose > 1)std::cout << i << "\t" << arrayBoundariesY[i] <<"\t" << pixel<<std::endl;
      }

      relativeMarginsX[0]         = (Double_t)nPixelsLeftColumn/(nPixelsLeftColumn+nPixelsSinglePlotX);
      relativeMarginsX[1]         = 0;
      relativeMarginsX[2]         = (Double_t)nPixelsRightColumn/(nPixelsRightColumn+nPixelsSinglePlotX);;

      relativeMarginsY[0]         = (Double_t)nPixelsUpperColumn/(nPixelsUpperColumn+nPixelsSinglePlotY);
      relativeMarginsY[1]         = 0;
      relativeMarginsY[2]         = (Double_t)nPixelsLowerColumn/(nPixelsLowerColumn+nPixelsSinglePlotY);;

      return;
  }

  //__________________________________________________________________________________________________________
  void ReturnCorrectValuesTextSize(   TPad * pad,
                                      Double_t &textsizeLabels,
                                      Double_t &textsizeFac,
                                      Int_t textSizeLabelsPixel,
                                      Double_t dummyWUP){
      if(dummyWUP){}

      textsizeLabels = 0;
      textsizeFac = 0;
      if (pad->XtoPixel(pad->GetX2()) < pad->YtoPixel(pad->GetY1())){
          textsizeLabels = (Double_t)textSizeLabelsPixel/pad->XtoPixel(pad->GetX2()) ;
          textsizeFac = (Double_t)1./pad->XtoPixel(pad->GetX2()) ;
      } else {
          textsizeLabels = (Double_t)textSizeLabelsPixel/pad->YtoPixel(pad->GetY1());
          textsizeFac = (Double_t)1./pad->YtoPixel(pad->GetY1());
      }
      std::cout << textsizeLabels << std::endl;
      std::cout << textsizeFac << std::endl;

      return;

  }

  //********************************************************************************************************************************
  //******** CreateCanvasAndPadsFor8PannelTBPlot ***********************************************************************************
  //********************************************************************************************************************************
  void CreateCanvasAndPadsFor8PannelTBPlot(TCanvas* &canvas, TPad* pads[8],  Double_t* topRCornerX, Double_t* topRCornerY,  Double_t* relSize8P, Int_t textSizePixel = 30, Double_t marginLeft = 0.03, TString add = "", bool rightCorner = true, int debug = 0){
    Double_t arrayBoundsXIndMeasRatio[5];
    Double_t arrayBoundsYIndMeasRatio[3];
    Double_t relativeMarginsIndMeasRatioX[3];
    Double_t relativeMarginsIndMeasRatioY[3];
    ReturnCorrectValuesForCanvasScaling(2200,1200, 4, 2,marginLeft, 0.005, 0.005,0.05,arrayBoundsXIndMeasRatio,arrayBoundsYIndMeasRatio,relativeMarginsIndMeasRatioX,relativeMarginsIndMeasRatioY, debug);

    canvas = new TCanvas(Form("canvas8Panel%s", add.Data()),"",0,0,2200,1200);  // gives the page size
    canvas->cd();

    //*****************************************************************
    // Test beam geometry (beam coming from viewer)
    //===========================================================
    //||    8 (4)    ||    7 (5)   ||    6 (6)   ||    5 (7)   ||  row 0
    //===========================================================
    //||    1 (0)    ||    2 (1)   ||    3 (2)   ||    4 (3)   ||  row 1
    //===========================================================
    //    col 0     col 1       col 2     col  3
    // rebuild pad geom in similar way (numbering -1)
    //*****************************************************************
    
    pads[0] = new TPad(Form("pad8Pane%sl_0", add.Data()), "", arrayBoundsXIndMeasRatio[0], arrayBoundsYIndMeasRatio[2], arrayBoundsXIndMeasRatio[1], arrayBoundsYIndMeasRatio[1],-1, -1, -2);
    pads[1] = new TPad(Form("pad8Pane%sl_1", add.Data()), "", arrayBoundsXIndMeasRatio[1], arrayBoundsYIndMeasRatio[2], arrayBoundsXIndMeasRatio[2], arrayBoundsYIndMeasRatio[1],-1, -1, -2);
    pads[2] = new TPad(Form("pad8Pane%sl_2", add.Data()), "", arrayBoundsXIndMeasRatio[2], arrayBoundsYIndMeasRatio[2], arrayBoundsXIndMeasRatio[3], arrayBoundsYIndMeasRatio[1],-1, -1, -2);
    pads[3] = new TPad(Form("pad8Pane%sl_3", add.Data()), "", arrayBoundsXIndMeasRatio[3], arrayBoundsYIndMeasRatio[2], arrayBoundsXIndMeasRatio[4], arrayBoundsYIndMeasRatio[1],-1, -1, -2);
    pads[4] = new TPad(Form("pad8Pane%sl_4", add.Data()), "", arrayBoundsXIndMeasRatio[0], arrayBoundsYIndMeasRatio[1],arrayBoundsXIndMeasRatio[1], arrayBoundsYIndMeasRatio[0],-1, -1, -2);
    pads[5] = new TPad(Form("pad8Pane%sl_4", add.Data()), "", arrayBoundsXIndMeasRatio[1], arrayBoundsYIndMeasRatio[1],arrayBoundsXIndMeasRatio[2], arrayBoundsYIndMeasRatio[0],-1, -1, -2);
    pads[6] = new TPad(Form("pad8Pane%sl_4", add.Data()), "", arrayBoundsXIndMeasRatio[2], arrayBoundsYIndMeasRatio[1],arrayBoundsXIndMeasRatio[3], arrayBoundsYIndMeasRatio[0],-1, -1, -2);
    pads[7] = new TPad(Form("pad8Pane%sl_4", add.Data()), "", arrayBoundsXIndMeasRatio[3], arrayBoundsYIndMeasRatio[1],arrayBoundsXIndMeasRatio[4], arrayBoundsYIndMeasRatio[0],-1, -1, -2);
    
    DefaultPadSettings( pads[4], relativeMarginsIndMeasRatioX[0], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioY[0], relativeMarginsIndMeasRatioY[1]);
    DefaultPadSettings( pads[5], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioY[0], relativeMarginsIndMeasRatioY[1]);
    DefaultPadSettings( pads[6], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioY[0], relativeMarginsIndMeasRatioY[1]);
    DefaultPadSettings( pads[7], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioX[2], relativeMarginsIndMeasRatioY[0], relativeMarginsIndMeasRatioY[1]);
    DefaultPadSettings( pads[0], relativeMarginsIndMeasRatioX[0], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioY[1], relativeMarginsIndMeasRatioY[2]);
    DefaultPadSettings( pads[1], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioY[1], relativeMarginsIndMeasRatioY[2]);
    DefaultPadSettings( pads[2], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioY[1], relativeMarginsIndMeasRatioY[2]);
    DefaultPadSettings( pads[3], relativeMarginsIndMeasRatioX[1], relativeMarginsIndMeasRatioX[2], relativeMarginsIndMeasRatioY[1], relativeMarginsIndMeasRatioY[2]);

    topRCornerY[0]  = 1-relativeMarginsIndMeasRatioY[1];
    topRCornerY[1]  = 1-relativeMarginsIndMeasRatioY[1];
    topRCornerY[2]  = 1-relativeMarginsIndMeasRatioY[1];
    topRCornerY[3]  = 1-relativeMarginsIndMeasRatioY[1];
    topRCornerY[4]  = 1-relativeMarginsIndMeasRatioY[0];
    topRCornerY[5]  = 1-relativeMarginsIndMeasRatioY[0];
    topRCornerY[6]  = 1-relativeMarginsIndMeasRatioY[0];
    topRCornerY[7]  = 1-relativeMarginsIndMeasRatioY[0];      
    if (rightCorner){
      topRCornerX[0]  = 1-relativeMarginsIndMeasRatioX[1];
      topRCornerX[1]  = 1-relativeMarginsIndMeasRatioX[1];
      topRCornerX[2]  = 1-relativeMarginsIndMeasRatioX[1];
      topRCornerX[3]  = 1-relativeMarginsIndMeasRatioX[2];
      topRCornerX[4]  = 1-relativeMarginsIndMeasRatioX[1];
      topRCornerX[5]  = 1-relativeMarginsIndMeasRatioX[1];
      topRCornerX[6]  = 1-relativeMarginsIndMeasRatioX[1];
      topRCornerX[7]  = 1-relativeMarginsIndMeasRatioX[2];
    } else {
      topRCornerX[0]  = relativeMarginsIndMeasRatioX[0];
      topRCornerX[1]  = relativeMarginsIndMeasRatioX[1];
      topRCornerX[2]  = relativeMarginsIndMeasRatioX[1];
      topRCornerX[3]  = relativeMarginsIndMeasRatioX[1];
      topRCornerX[4]  = relativeMarginsIndMeasRatioX[0];
      topRCornerX[5]  = relativeMarginsIndMeasRatioX[1];
      topRCornerX[6]  = relativeMarginsIndMeasRatioX[1];
      topRCornerX[7]  = relativeMarginsIndMeasRatioX[1];
    }
    
    for (Int_t p = 0; p < 8; p++){
      if (pads[p]->XtoPixel(pads[p]->GetX2()) < pads[p]->YtoPixel(pads[p]->GetY1())){
        relSize8P[p]  = (Double_t)textSizePixel/pads[p]->XtoPixel(pads[p]->GetX2()) ;
      } else {
        relSize8P[p]  = (Double_t)textSizePixel/pads[p]->YtoPixel(pads[p]->GetY1());
      }
      if(debug > 1)std::cout << p << "\t" << topRCornerX[p]<< "\t" << topRCornerY[p] << "\t" << relSize8P[p] << std::endl;
    }
    return;
  }
  
  
  //__________________________________________________________________________________________________________
  // Plot Noise with Fits for Full layer
  //__________________________________________________________________________________________________________
  void PlotNoiseWithFitsFullLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
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
  void PlotNoiseAdvWithFitsFullLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
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
  void PlotMipWithFitsFullLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
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
  void PlotSpectraFullLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
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
  void PlotCorrWithFitsFullLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
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
  void PlotCorr2DFullLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
                                  std::map<int,TileSpectra> spectra, 
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

        tempProfile->Draw("pe, same");
                
        TString label           = Form("row %d col %d", r, c);
        if (p == 7){
          label = Form("row %d col %d layer %d", r, c, layer);
        }
        TLatex *labelChannel    = new TLatex(topRCornerX[p]+0.045,topRCornerY[p]-1.2*relSize8P[p],label);
        SetStyleTLatex( labelChannel, 0.85*textSizePixel,4,1,43,kTRUE,11);

        TF1* fit            = ithSpectra->second.GetCorrModel(0);
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
  // Plot Trigger Primitive with Fits for Full layer
  //__________________________________________________________________________________________________________
  void PlotTriggerPrimWithFitsFullLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, 
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
  // Simple event display with different highlighted triggers
  //__________________________________________________________________________________________________________
  void EventDisplayWithSliceHighlighted( TH3F* h3All, TH1D* h1XAll, TH1D* h1YAll, TH1D* h1ZAll, 
                                         TH3F* h3LocTrigg, TH1D* h1XLocTrigg, TH1D* h1YLocTrigg, TH1D* h1ZLocTrigg, 
                                         TH3F* h3Remain, TH1D* h1XRemain, TH1D* h1YRemain, TH1D* h1ZRemain, 
                                         Int_t evtNr, Float_t etot, Float_t maxE, 
                                         Float_t maxEX, Float_t maxEY, Float_t maxEZ, bool ktrigg,
                                         RunInfo currRunInfo, TString outputName, TString suffix = "pdf"
                                        ){
    Double_t textSizeRel = 0.035;
    Double_t textSizeSubpad = 0.06;
    
    TCanvas* canvas3D2 = new TCanvas("canvas3D2","",0,0,1400,750);  // gives the page size
    // DefaultCancasSettings( canvas3D2, 0.05, 0.25, 0.05, 0.1);
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
    DrawLatex(0.01, 0.86, Form("#it{E}_{tot} = %.2f mip eq/tile", etot), false, 0.85*textSizeRel, 42);
    DrawLatex(0.01, 0.83, Form("#it{E}_{max,cell} = %.2f mip eq/tile", maxE), false, 0.85*textSizeRel, 42);
    
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
    SetStyleHistoTH1ForGraphs( h1XAll, "#it{x} (cm)", "#it{E} (mip eq./tile)", 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
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
    SetStyleHistoTH1ForGraphs( h1YAll, "#it{y} (cm)", "#it{E} (mip eq./tile)", 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
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
    SetStyleHistoTH1ForGraphs( h1ZAll, "#it{z} (cm)", "#it{E} (mip eq./tile)", 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
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
                              RunInfo currRunInfo, TString outputName, TString suffix = "pdf"
                            ){
    Double_t textSizeRel = 0.035;
    Double_t textSizeSubpad = 0.06;
    
    TCanvas* canvas3D2 = new TCanvas("canvas3D2","",0,0,1400,750);  // gives the page size
    // DefaultCancasSettings( canvas3D2, 0.05, 0.25, 0.05, 0.1);
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
    DrawLatex(0.01, 0.86, Form("#it{E}_{tot} = %.2f mip eq/tile", etot), false, 0.85*textSizeRel, 42);
    DrawLatex(0.01, 0.83, Form("#it{E}_{max,cell} = %.2f mip eq/tile", maxE), false, 0.85*textSizeRel, 42);
    
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
    SetStyleHistoTH1ForGraphs( h1XAll, "#it{x} (cm)", "#it{E} (mip eq./tile)", 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
    SetMarkerDefaults(h1XAll, 24, 1, kBlue+1, kBlue+1, kFALSE);   
    h1XAll->GetYaxis()->SetRangeUser(0.,1.1*maxEX);
    h1XAll->Draw("pe");
    canvas3D2->cd();

    padEvt[2]->Draw();
    padEvt[2]->cd();
    SetStyleHistoTH1ForGraphs( h1YAll, "#it{y} (cm)", "#it{E} (mip eq./tile)", 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
    SetMarkerDefaults(h1YAll, 24, 1, kBlue+1, kBlue+1, kFALSE);   
    h1YAll->GetYaxis()->SetRangeUser(0.,1.1*maxEY);
    h1YAll->Draw("pe");
    
    canvas3D2->cd();
    padEvt[3]->Draw();
    padEvt[3]->cd();
    SetStyleHistoTH1ForGraphs( h1ZAll, "#it{z} (cm)", "#it{E} (mip eq./tile)", 0.85*textSizeSubpad, textSizeSubpad, 0.85*textSizeSubpad, textSizeSubpad,0.9, 0.9);  
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
  // Plot Corr with Fits for Full layer
  //__________________________________________________________________________________________________________
  void PlotTrendingPerLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
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
      TH1D* tempH; 
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
  // Plot Run overlay for all 8 tiles for all runs available
  //__________________________________________________________________________________________________________
  void PlotRunOverlayPerLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
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
  void PlotRunOverlayProfilePerLayer (TCanvas* canvas8Panel, TPad* pads[8], Double_t* topRCornerX,  Double_t* topRCornerY, Double_t* relSize8P, Int_t textSizePixel, 
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

  //__________________________________________________________________________________________________________
  // Plot 1D distribution
  //__________________________________________________________________________________________________________  
  void PlotAnalysisComparison( TCanvas* canvas2D, Int_t option, 
                            std::map<int, AnaSummary> sumRuns, 
                            Float_t textSizeRel, TString nameOutput, RunInfo currRunInfo, 
                            //int labelOpt = 1,
                            TString additionalLabel = "", int debug = 0
                            ){
    //hardcode max X
    Double_t minY         = 0.1;
    Double_t maxY         = 0;
    Double_t minX         = 9999;
    Double_t maxX         = 0;
    bool isSameVoltage    = true;
    double commanVoltage  = 0;
    
    std::map<int, AnaSummary>::iterator itrun;
    Int_t nruns = 0;
    for(itrun=sumRuns.begin(); itrun!=sumRuns.end(); ++itrun){
      TH1D* tempH; 
      if (option==0) tempH = itrun->second.GetDeltaTime();
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
        if (option==0) histos[currRun] = itrun->second.GetDeltaTime();
//	std::cout <<"currentRun" <<std::endl;
        SetStyleHistoTH1ForGraphs( histos[currRun], histos[currRun]->GetXaxis()->GetTitle(), histos[currRun]->GetYaxis()->GetTitle(), 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel,0.95, 1.02);  
        SetLineDefaults(histos[currRun], GetColorLayer(currRun), 4, GetLineStyleLayer(currRun));   
        if(currRun == 0){
          histos[currRun]->GetXaxis()->SetRangeUser(minX-5*histos[currRun]->GetBinWidth(1),maxX+5*histos[currRun]->GetBinWidth(1));
          histos[currRun]->GetYaxis()->SetRangeUser(minY,maxY*1.1);
          histos[currRun]->Draw("hist");
	//  std::cout<<"plot initial"<<std::endl;
        } else {
          histos[currRun]->Draw("same,hist");
	//  std::cout<<"plot next" <<std::endl;
        }
        legend->AddEntry(histos[currRun],Form("%d",itrun->second.GetRunNumber()),"l");
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
