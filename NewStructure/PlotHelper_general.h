#ifndef PLOTTHELPER_GENERAL_H
#define PLOTTHELPER_GENERAL_H


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
    int i = hist->GetNbinsX()-1;
    // std::cout<< "Find last bin" << std::endl;
    while (i > 0 && hist->GetBinContent(i) < min){ 
      // std::cout << i << "\t"<< hist->GetBinCenter(i) << "\t"<< hist->GetBinContent(i) <<std::endl;
      i--;
    }
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
    // std::cout<< "Find first bin" << std::endl;
    while (i < hist->GetNbinsX() && hist->GetBinContent(i) < min){
      // std::cout << i << "\t"<< hist->GetBinCenter(i) << "\t"<< hist->GetBinContent(i) <<std::endl;
      i++;
    }
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
  void DefaultCanvasSettings( TCanvas* c1,
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
      if (xRangeStart != -10000 && xRangeEnd != -10000 )fit->SetRange(xRangeStart,xRangeEnd);
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
  TBox* CreateBox(Color_t colorBox, Double_t xStart, Double_t yStart, Double_t xEnd, Double_t yEnd, Style_t fillStyle = 1001, Float_t opacity = 1.  ) {
      TBox* box = new TBox(xStart ,yStart , xEnd, yEnd);
      box->SetLineColor(colorBox);
      box->SetFillColorAlpha(colorBox,opacity);
      box->SetFillStyle(fillStyle);
      return box;
  }
  
  //********************************************************************************************************************************
  //********************************************************************************************************************************
  //********************************************************************************************************************************
  void DrawCorrectBadChannelBox (short bctemp, double minX, double minY, double maxX, double maxY){
    if (bctemp != -64 && bctemp < 3){
      Color_t boxCol = kGray;
      if (bctemp == 1)
        boxCol = kGray+1;
      else if (bctemp == 0)
        boxCol = kGray+2;
      TBox* badChannelArea =  CreateBox(boxCol, minX, minY, maxX,maxY, 1001 );
      badChannelArea->Draw();
    }
  }
  //********************************************************************************************************************************
  //********************************************************************************************************************************
  //********************************************************************************************************************************
  void DrawHighlightTrigg (double minX, double minY, double maxX, double maxY){
    Color_t boxCol = kOrange+6;
    TBox* highChannelArea =  CreateBox(boxCol, minX, minY, maxX,maxY, 1001, 0.35 );
    highChannelArea->Draw();
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
              if (beam.CompareTo("Positron") == 0) beam = "e^{+}";              
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
              if (beam.CompareTo("Positron") == 0) beam = "e^{+}";              
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
              if (beam.CompareTo("Positron") == 0) beam = "e^{+}";              
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
    Double_t arrayBoundX[5];
    Double_t arrayBoundY[3];
    Double_t relativeMarginsX[3];
    Double_t relativeMarginsY[3];
    ReturnCorrectValuesForCanvasScaling(2200,1200, 4, 2,marginLeft, 0.005, 0.005,0.05,arrayBoundX,arrayBoundY,relativeMarginsX,relativeMarginsY, debug);

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
    
    pads[0] = new TPad(Form("pad8Panel%s_0", add.Data()), "", arrayBoundX[0], arrayBoundY[2], arrayBoundX[1], arrayBoundY[1],-1, -1, -2);
    pads[1] = new TPad(Form("pad8Panel%s_1", add.Data()), "", arrayBoundX[1], arrayBoundY[2], arrayBoundX[2], arrayBoundY[1],-1, -1, -2);
    pads[2] = new TPad(Form("pad8Panel%s_2", add.Data()), "", arrayBoundX[2], arrayBoundY[2], arrayBoundX[3], arrayBoundY[1],-1, -1, -2);
    pads[3] = new TPad(Form("pad8Panel%s_3", add.Data()), "", arrayBoundX[3], arrayBoundY[2], arrayBoundX[4], arrayBoundY[1],-1, -1, -2);
    pads[4] = new TPad(Form("pad8Panel%s_4", add.Data()), "", arrayBoundX[0], arrayBoundY[1],arrayBoundX[1], arrayBoundY[0],-1, -1, -2);
    pads[5] = new TPad(Form("pad8Panel%s_5", add.Data()), "", arrayBoundX[1], arrayBoundY[1],arrayBoundX[2], arrayBoundY[0],-1, -1, -2);
    pads[6] = new TPad(Form("pad8Panel%s_6", add.Data()), "", arrayBoundX[2], arrayBoundY[1],arrayBoundX[3], arrayBoundY[0],-1, -1, -2);
    pads[7] = new TPad(Form("pad8Panel%s_7", add.Data()), "", arrayBoundX[3], arrayBoundY[1],arrayBoundX[4], arrayBoundY[0],-1, -1, -2);
    
    DefaultPadSettings( pads[4], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
    DefaultPadSettings( pads[5], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
    DefaultPadSettings( pads[6], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
    DefaultPadSettings( pads[7], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[0], relativeMarginsY[1]);
    DefaultPadSettings( pads[0], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
    DefaultPadSettings( pads[1], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
    DefaultPadSettings( pads[2], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
    DefaultPadSettings( pads[3], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[2]);

    topRCornerY[0]  = 1-relativeMarginsY[1];
    topRCornerY[1]  = 1-relativeMarginsY[1];
    topRCornerY[2]  = 1-relativeMarginsY[1];
    topRCornerY[3]  = 1-relativeMarginsY[1];
    topRCornerY[4]  = 1-relativeMarginsY[0];
    topRCornerY[5]  = 1-relativeMarginsY[0];
    topRCornerY[6]  = 1-relativeMarginsY[0];
    topRCornerY[7]  = 1-relativeMarginsY[0];      
    if (rightCorner){
      topRCornerX[0]  = 1-relativeMarginsX[1];
      topRCornerX[1]  = 1-relativeMarginsX[1];
      topRCornerX[2]  = 1-relativeMarginsX[1];
      topRCornerX[3]  = 1-relativeMarginsX[2];
      topRCornerX[4]  = 1-relativeMarginsX[1];
      topRCornerX[5]  = 1-relativeMarginsX[1];
      topRCornerX[6]  = 1-relativeMarginsX[1];
      topRCornerX[7]  = 1-relativeMarginsX[2];
    } else {
      topRCornerX[0]  = relativeMarginsX[0];
      topRCornerX[1]  = relativeMarginsX[1];
      topRCornerX[2]  = relativeMarginsX[1];
      topRCornerX[3]  = relativeMarginsX[1];
      topRCornerX[4]  = relativeMarginsX[0];
      topRCornerX[5]  = relativeMarginsX[1];
      topRCornerX[6]  = relativeMarginsX[1];
      topRCornerX[7]  = relativeMarginsX[1];
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

  //********************************************************************************************************************************
  //******** CreateCanvasAndPadsFor2PannelTBPlot ***********************************************************************************
  //********************************************************************************************************************************
  void CreateCanvasAndPadsFor2PannelTBPlot(TCanvas* &canvas, TPad* pads[2],  Double_t* topRCornerX, Double_t* topRCornerY,  Double_t* relSizeP, Int_t textSizePixel = 30, Double_t marginLeft = 0.03, TString add = "", bool rightCorner = true, int debug = 0){
    Double_t arrayBoundX[3];
    Double_t arrayBoundY[2];
    Double_t relativeMarginsX[3];
    Double_t relativeMarginsY[3];
    ReturnCorrectValuesForCanvasScaling(1200,600, 2, 1,marginLeft, 0.005, 0.015,0.1,arrayBoundX,arrayBoundY,relativeMarginsX,relativeMarginsY, 2);

    canvas = new TCanvas(Form("canvas2Panel%s", add.Data()),"",0,0,1200,600);  // gives the page size
    canvas->cd();

    //*****************************************************************
    // Test beam geometry (beam coming from viewer)
    //===========================================================
    //===========================================================
    //||    1 (0)    ||    2 (1)   ||  row 0
    //===========================================================
    //    col 0     col 1 
    // rebuild pad geom in similar way (numbering -1)
    //*****************************************************************
    
    std::cout << "0: " <<  arrayBoundX[0] << "\t" << arrayBoundY[0] << "\t" << arrayBoundX[1] << "\t" << arrayBoundY[1] << std::endl;
    std::cout << "1: " <<  arrayBoundX[1] << "\t" << arrayBoundY[0] << "\t" << arrayBoundX[2] << "\t" << arrayBoundY[1] << std::endl;
    
    pads[0] = new TPad(Form("pad2Panel%s_0", add.Data()), "", arrayBoundX[0], arrayBoundY[1], arrayBoundX[1], arrayBoundY[0],-1, -1, -2);
    pads[1] = new TPad(Form("pad2Panel%s_1", add.Data()), "", arrayBoundX[1], arrayBoundY[1], arrayBoundX[2], arrayBoundY[0],-1, -1, -2);
    
    
    DefaultPadSettings( pads[0], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[2]);
    DefaultPadSettings( pads[1], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[0], relativeMarginsY[2]);

    topRCornerY[0]  = 1-relativeMarginsY[2];
    topRCornerY[1]  = 1-relativeMarginsY[2];
    if (rightCorner){
      topRCornerX[0]  = 1-relativeMarginsX[1];
      topRCornerX[1]  = 1-relativeMarginsX[2];
    } else {
      topRCornerX[0]  = relativeMarginsX[0];
      topRCornerX[1]  = relativeMarginsX[1];
    }
    
    for (Int_t p = 0; p < 2; p++){
      if (pads[p]->XtoPixel(pads[p]->GetX2()) < pads[p]->YtoPixel(pads[p]->GetY1())){
        relSizeP[p]  = (Double_t)textSizePixel/pads[p]->XtoPixel(pads[p]->GetX2()) ;
      } else {
        relSizeP[p]  = (Double_t)textSizePixel/pads[p]->YtoPixel(pads[p]->GetY1());
      }
      // if(debug > 1)
        std::cout << p << "\t" << topRCornerX[p]<< "\t" << topRCornerY[p] << "\t" << relSizeP[p] << std::endl;
    }
    return;
  }  

  //********************************************************************************************************************************
  //******** CreateCanvasAndPadsFor8PannelTBPlot ***********************************************************************************
  //********************************************************************************************************************************
  void CreateCanvasAndPadsForDualModTBPlot(TCanvas* &canvas, TPad* pads[16],  Double_t* topRCornerX, Double_t* topRCornerY,  Double_t* relSize, 
                                           Int_t textSizePixel = 30, Double_t marginLeft = 0.03, TString add = "", bool rightCorner = true, int debug = 0){
    Double_t arrayBoundX[5];
    Double_t arrayBoundY[5];
    Double_t relativeMarginsX[3];
    Double_t relativeMarginsY[3];
    ReturnCorrectValuesForCanvasScaling(2200,2200, 4, 4,marginLeft, 0.005, 0.005,marginLeft,arrayBoundX,arrayBoundY,relativeMarginsX,relativeMarginsY, debug);

    canvas = new TCanvas(Form("canvas4x4Panel%s", add.Data()),"",0,0,2200,2200);  // gives the page size
    canvas->cd();

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
    
    pads[0]   = new TPad(Form("pad4x4Panel%s_0", add.Data()), "", arrayBoundX[0], arrayBoundY[4], arrayBoundX[1], arrayBoundY[3],-1, -1, -2);
    pads[1]   = new TPad(Form("pad4x4Panel%s_1", add.Data()), "", arrayBoundX[1], arrayBoundY[4], arrayBoundX[2], arrayBoundY[3],-1, -1, -2);
    pads[2]   = new TPad(Form("pad4x4Panel%s_2", add.Data()), "", arrayBoundX[2], arrayBoundY[4], arrayBoundX[3], arrayBoundY[3],-1, -1, -2);
    pads[3]   = new TPad(Form("pad4x4Panel%s_3", add.Data()), "", arrayBoundX[3], arrayBoundY[4], arrayBoundX[4], arrayBoundY[3],-1, -1, -2);
    pads[4]   = new TPad(Form("pad4x4Panel%s_4", add.Data()), "", arrayBoundX[0], arrayBoundY[3],arrayBoundX[1], arrayBoundY[2],-1, -1, -2);
    pads[5]   = new TPad(Form("pad4x4Panel%s_5", add.Data()), "", arrayBoundX[1], arrayBoundY[3],arrayBoundX[2], arrayBoundY[2],-1, -1, -2);
    pads[6]   = new TPad(Form("pad4x4Panel%s_6", add.Data()), "", arrayBoundX[2], arrayBoundY[3],arrayBoundX[3], arrayBoundY[2],-1, -1, -2);
    pads[7]   = new TPad(Form("pad4x4Panel%s_7", add.Data()), "", arrayBoundX[3], arrayBoundY[3],arrayBoundX[4], arrayBoundY[2],-1, -1, -2);
    pads[8]   = new TPad(Form("pad4x4Panel%s_8", add.Data()), "", arrayBoundX[0], arrayBoundY[2], arrayBoundX[1], arrayBoundY[1],-1, -1, -2);
    pads[9]   = new TPad(Form("pad4x4Panel%s_9", add.Data()), "", arrayBoundX[1], arrayBoundY[2], arrayBoundX[2], arrayBoundY[1],-1, -1, -2);
    pads[10]  = new TPad(Form("pad4x4Panel%s_10", add.Data()), "", arrayBoundX[2], arrayBoundY[2], arrayBoundX[3], arrayBoundY[1],-1, -1, -2);
    pads[11]  = new TPad(Form("pad4x4Panel%s_11", add.Data()), "", arrayBoundX[3], arrayBoundY[2], arrayBoundX[4], arrayBoundY[1],-1, -1, -2);
    pads[12]  = new TPad(Form("pad4x4Panel%s_12", add.Data()), "", arrayBoundX[0], arrayBoundY[1],arrayBoundX[1], arrayBoundY[0],-1, -1, -2);
    pads[13]  = new TPad(Form("pad4x4Panel%s_13", add.Data()), "", arrayBoundX[1], arrayBoundY[1],arrayBoundX[2], arrayBoundY[0],-1, -1, -2);
    pads[14]  = new TPad(Form("pad4x4Panel%s_14", add.Data()), "", arrayBoundX[2], arrayBoundY[1],arrayBoundX[3], arrayBoundY[0],-1, -1, -2);
    pads[15]  = new TPad(Form("pad4x4Panel%s_15", add.Data()), "", arrayBoundX[3], arrayBoundY[1],arrayBoundX[4], arrayBoundY[0],-1, -1, -2);
    
    
    
    
    DefaultPadSettings( pads[0], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
    DefaultPadSettings( pads[1], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
    DefaultPadSettings( pads[2], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
    DefaultPadSettings( pads[3], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[2]);
    DefaultPadSettings( pads[4], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
    DefaultPadSettings( pads[5], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
    DefaultPadSettings( pads[6], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
    DefaultPadSettings( pads[7], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[1]);
    DefaultPadSettings( pads[8], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
    DefaultPadSettings( pads[9], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
    DefaultPadSettings( pads[10], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
    DefaultPadSettings( pads[11], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[1]);
    DefaultPadSettings( pads[12], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
    DefaultPadSettings( pads[13], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
    DefaultPadSettings( pads[14], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
    DefaultPadSettings( pads[15], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[0], relativeMarginsY[1]);

    for (int i = 0; i < 12; i++)
      topRCornerY[i]  = 1-relativeMarginsY[1];
    for (int i = 12; i < 16; i++)
      topRCornerY[i]  = 1-relativeMarginsY[0];
    if (rightCorner){
      for (int i = 0; i < 16; i++){
        if ((i+1)%4 == 0)
          topRCornerX[i]  = 1-relativeMarginsX[2];
        else 
          topRCornerX[i]  = 1-relativeMarginsX[1];
      }
    } else {
      for (int i = 0; i < 16; i++){
        if (i%4 == 0)
          topRCornerX[i]  = 1-relativeMarginsX[0];
        else 
          topRCornerX[i]  = 1-relativeMarginsX[1];
      }
    }
    
    for (Int_t p = 0; p < 16; p++){
      if (pads[p]->XtoPixel(pads[p]->GetX2()) < pads[p]->YtoPixel(pads[p]->GetY1())){
        relSize[p]  = (Double_t)textSizePixel/pads[p]->XtoPixel(pads[p]->GetX2()) ;
      } else {
        relSize[p]  = (Double_t)textSizePixel/pads[p]->YtoPixel(pads[p]->GetY1());
      }
      if(debug > 1)std::cout << p << "\t" << topRCornerX[p]<< "\t" << topRCornerY[p] << "\t" << relSize[p] << std::endl;
    }
    return;
  }    
  
  //********************************************************************************************************************************
  //******** CreateCanvasAndPadsFor8PannelTBPlot ***********************************************************************************
  //********************************************************************************************************************************
  void CreateCanvasAndPadsForAsicLFHCalTBPlot(TCanvas* &canvas, TPad* pads[64],  Double_t* topRCornerX, Double_t* topRCornerY,  Double_t* relSize, 
                                           Int_t textSizePixel = 30, Double_t marginLeft = 0.03, TString add = "", bool rightCorner = true, int debug = 0){
    Double_t arrayBoundX[9];
    Double_t arrayBoundY[9];
    Double_t relativeMarginsX[3];
    Double_t relativeMarginsY[3];
    ReturnCorrectValuesForCanvasScaling(3300,3300, 8, 8,marginLeft, 0.005, 0.005,marginLeft,arrayBoundX,arrayBoundY,relativeMarginsX,relativeMarginsY, debug);

    canvas = new TCanvas(Form("canvas8x8Panel%s", add.Data()),"",0,0,3300,3300);  // gives the page size
    canvas->cd();

    for (int k = 0; k< 8; k++){
      pads[k+0*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+0*8), "", arrayBoundX[k], arrayBoundY[8], arrayBoundX[k+1], arrayBoundY[7],-1, -1, -2);
      pads[k+1*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+8), "", arrayBoundX[k], arrayBoundY[7], arrayBoundX[k+1], arrayBoundY[6],-1, -1, -2);
      pads[k+2*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+2*8), "", arrayBoundX[k], arrayBoundY[6], arrayBoundX[k+1], arrayBoundY[5],-1, -1, -2);
      pads[k+3*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+3*8), "", arrayBoundX[k], arrayBoundY[5], arrayBoundX[k+1], arrayBoundY[4],-1, -1, -2);
      pads[k+4*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+4*8), "", arrayBoundX[k], arrayBoundY[4], arrayBoundX[k+1], arrayBoundY[3],-1, -1, -2);
      pads[k+5*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+5*8), "", arrayBoundX[k], arrayBoundY[3], arrayBoundX[k+1], arrayBoundY[2],-1, -1, -2);
      pads[k+6*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+6*8), "", arrayBoundX[k], arrayBoundY[2], arrayBoundX[k+1], arrayBoundY[1],-1, -1, -2);
      pads[k+7*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+7*8), "", arrayBoundX[k], arrayBoundY[1], arrayBoundX[k+1], arrayBoundY[0],-1, -1, -2);
    }
        
    for (int k = 0; k < 64; k++){ 
      // 0th column
      if (k%8 ==0 && k > 7 && k < 56 )
        DefaultPadSettings( pads[k], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
      else if (k == 0)
        DefaultPadSettings( pads[k], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
      else if (k == 56)
        DefaultPadSettings( pads[k], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
      
      //7th column
      else if (k%8 ==7 && k > 7 && k < 56 )
        DefaultPadSettings( pads[k], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[1]);
      else if (k == 7)
        DefaultPadSettings( pads[k], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[2]);
      else if (k == 63)
        DefaultPadSettings( pads[k], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[0], relativeMarginsY[1]);
      
      // bottom row
      else if (k > 0 && k < 7)
        DefaultPadSettings( pads[k], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
      // top row
      else if (k > 56 && k < 63)
        DefaultPadSettings( pads[k], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
      // everything in the middle
      else 
        DefaultPadSettings( pads[k], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
    }
    
    // everything except top row
    for (int i = 0; i < 56; i++)
      topRCornerY[i]  = 1-relativeMarginsY[1];
    // top row
    for (int i = 56; i < 64; i++)
      topRCornerY[i]  = 1-relativeMarginsY[0];
    
    // right corner
    if (rightCorner){
      for (int i = 0; i < 64; i++){
        if (i%8 == 7)
          topRCornerX[i]  = 1-relativeMarginsX[2];
        else 
          topRCornerX[i]  = 1-relativeMarginsX[1];
      }
    // left corner
    } else {
      for (int i = 0; i < 64; i++){
        if (i%8 == 0)
          topRCornerX[i]  = relativeMarginsX[0];
        else 
          topRCornerX[i]  = relativeMarginsX[1];
      }
    }
    
    for (Int_t p = 0; p < 64; p++){
      if (pads[p]->XtoPixel(pads[p]->GetX2()) < pads[p]->YtoPixel(pads[p]->GetY1())){
        relSize[p]  = (Double_t)textSizePixel/pads[p]->XtoPixel(pads[p]->GetX2()) ;
      } else {
        relSize[p]  = (Double_t)textSizePixel/pads[p]->YtoPixel(pads[p]->GetY1());
      }
      if(debug > 1)std::cout << p << "\t" << topRCornerX[p]<< "\t" << topRCornerY[p] << "\t" << relSize[p] << std::endl;
    }
    return;
  }      
#endif
