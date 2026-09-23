// PlottingHeader.h


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
void SetStyleHistoTH3ForGraphs( TH2* histo,
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
void DrawLatex( const double PosX     = 0.5, 
                const double PosY     = 0.5, 
                TString text          = "", 
                const bool alignRight = false, 
                const double TextSize = 0.044, 
                const int font        = 42, 
                const double dDist    = 0.05, 
                const int color       = 1, 
                double angle          = 0.
               ){

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
        if (angle!= 0.)l.SetTextAngle(angle);
        l.DrawClone("same");
    }
}

void DrawLabelsAndLinesLFHCal1D(Double_t min, Double_t max, Double_t textSizeRel, Double_t lumi = 1, Double_t averageZ = 500, Int_t option = 1){
  
    if (option ==  1){ // projection in X
      Double_t posXIns = -(0.0527*averageZ+1.4774);      
      DrawLines(20, 20, min,  min*4, 2, kGray+1, 7);
      DrawLines(6.5, 6.5, min,  min*2, 2, kGray+1, 7);
      DrawLines(-40, -40, min,  min*4, 2, kGray+1, 7);
      DrawLines(posXIns, posXIns, min,  min*2, 2, kGray+1, 7);
      DrawLines(100, 100, min,  min*4, 2, kGray+1, 7);
      DrawLines(-100, -100, min,  min*4, 2, kGray+1, 7);
      DrawLines(250, 250, min,  min*4, 2, kGray+1, 7);
      DrawLines(-250, -250, min,  min*4, 2, kGray+1, 7);

      DrawLines(20, 20, 0.7*max,  max, 2, kGray+1, 7);
      DrawLines(6.5, 6.5, 0.7*max,  max, 2, kGray+1, 7);
      DrawLines(-40, -40, 0.7*max,  max, 2, kGray+1, 7);
      DrawLines(posXIns, posXIns, 0.7*max,  max, 2, kGray+1, 7);
      DrawLines(100, 100, 0.7*max,  max, 2, kGray+1, 7);
      DrawLines(-100, -100, 0.7*max,  max, 2, kGray+1, 7);
      DrawLines(250, 250, 0.7*max,  max, 2, kGray+1, 7);
      DrawLines(-250, -250, 0.7*max,  max, 2, kGray+1, 7);
      
      DrawLatex(0.5, 0.18, "insert", false,textSizeRel*0.85, 62,0.05, 1);
      DrawLatex(0.6, 0.21, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
      DrawLatex(0.35, 0.21, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
      DrawLatex(0.75, 0.18, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
      DrawLatex(0.125, 0.18, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
      DrawLatex(0.1, 0.92, Form("#it{LFHCal} region, %.0f fb^{-1}",lumi), false,textSizeRel, 62,0.05, 1);
    } else if (option == 2){ // projection in Y
      DrawLines(30, 30, min,  min*4, 2, kGray+1, 7);
      DrawLines(-30, -30, min,  min*4, 2, kGray+1, 7);
      DrawLines(100, 100, min,  min*4, 2, kGray+1, 7);
      DrawLines(-100, -100, min,  min*4, 2, kGray+1, 7);
      DrawLines(250, 250, min,  min*4, 2, kGray+1, 7);
      DrawLines(-250, -250, min,  min*4, 2, kGray+1, 7);
      
      DrawLatex(0.5, 0.18, "insert", false,textSizeRel*0.85, 62,0.05, 1);
      DrawLatex(0.6, 0.21, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
      DrawLatex(0.35, 0.21, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
      DrawLatex(0.75, 0.18, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
      DrawLatex(0.125, 0.18, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
      DrawLatex(0.1, 0.92, Form("#it{LFHCal} region, %.0f fb^{-1}",lumi), false,textSizeRel, 62,0.05, 1);
    }
}

void DrawLabelsAndLinesLFHCal2D(Double_t* zPos, TString* labels,  Double_t textSizeRel, Int_t option = 1){
    for (Int_t b =0; b < 9; b++){
      if (b == 0 || b == 8)
        DrawLines(zPos[b], zPos[b], -260,  260, 2, kBlack, 7);
      else 
        DrawLines(zPos[b], zPos[b], -260,  -210, 2, kGray+2, 7);
    }
    DrawLatex(0.185, 0.11, labels[0], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.26, 0.11, labels[1], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.35, 0.11, labels[2], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.45, 0.11, labels[3], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.54, 0.11, labels[4], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.64, 0.11, labels[5], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.71, 0.11, labels[6], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.76, 0.11, labels[7], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    
    DrawLines(zPos[0], zPos[8], -100,  -100, 2, kBlack, 7);
    DrawLines(zPos[0], zPos[8], 100,  100, 2, kBlack, 7);
    DrawLines(zPos[0], zPos[8], 20,  20, 2, kBlack, 7);
    DrawLines(zPos[0], zPos[8], 6.5,  6.5, 2, kBlack, 7);
    DrawLines(zPos[0], zPos[8], -40,  -40, 2, kBlack, 7);
    DrawLines(zPos[0], zPos[8], -20.85,  -27.8, 2, kBlack, 7);

    DrawLatex(0.17, 0.82, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.17, 0.63, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.17, 0.545, "insert", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.17, 0.465, "insert", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.17, 0.4, "3x3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
    DrawLatex(0.17, 0.25, "1.3x1.3 mm SiPMs", false,textSizeRel*0.85, 62,0.05, 1);
  
}

void DrawLabelsAndLinesNHCal1D(Double_t min, Double_t max, Double_t textSizeRel, Double_t lumi = 1, Int_t option = 1){
  
    if (option ==  1){ // projection in X
      DrawLines(10, 10, min,  min*4, 2, kGray+1, 7);
      DrawLines(253, 253, min,  min*4, 2, kGray+1, 7);
      DrawLines(-10, -10, min,  min*4, 2, kGray+1, 7);
      DrawLines(-253, -253, min,  min*4, 2, kGray+1, 7);
      
      DrawLatex(0.12, 0.92, Form("#it{NHCal} region, %.0f fb^{-1}",lumi), false,textSizeRel, 62,0.05, 1);
    } else if (option == 2){ // projection in Y
      DrawLines(10, 10, min,  min*4, 2, kGray+1, 7);
      DrawLines(253, 253, min,  min*4, 2, kGray+1, 7);
      DrawLines(-10, -10, min,  min*4, 2, kGray+1, 7);
      DrawLines(-253, -253, min,  min*4, 2, kGray+1, 7);
      
      DrawLatex(0.12, 0.92, Form("#it{NHCal} region, %.0f fb^{-1}",lumi), false,textSizeRel, 62,0.05, 1);
    }
}

void DrawLabelsAndLinesNHCal2D(Double_t* zPos, TString* labels,  Double_t textSizeRel, Int_t option = 1){
    for (Int_t b =0; b < 7; b++){
      if (b == 0 || b == 6)
        DrawLines(-zPos[b], -zPos[b], -260,  260, 2, kBlack, 7);
      else 
        DrawLines(-zPos[b], -zPos[b], -260,  -210, 2, kGray+2, 7);
    }
    DrawLatex(0.21, 0.11, labels[5], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.3, 0.11, labels[4], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.39, 0.11, labels[3], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.48, 0.11, labels[2], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.57, 0.11, labels[1], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    DrawLatex(0.69, 0.11, labels[0], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    
    DrawLines(-zPos[0], -zPos[6], 10,  10, 2, kBlack, 7);  
    DrawLines(-zPos[0], -zPos[6], -10,  -10, 2, kBlack, 7);  
}

void DrawLabelsAndLinesBHCal2D(Double_t* rPos, Double_t textSizeRel, Int_t option = 1){
    for (Int_t b =0; b < 5; b++){
      if (b == 0 || b == 4){
        DrawLines(-327.825, 327.825, -rPos[b], -rPos[b], 2, kBlack, 7);
        DrawLines(-327.825, 327.825, rPos[b], rPos[b], 2, kBlack, 7);
      } else {
        DrawLines(-327.825, -327.825+40, -rPos[b], -rPos[b], 2, kGray+2, 7);
        DrawLines(327.825-40, 327.825, -rPos[b], -rPos[b], 2, kGray+2, 7);
        DrawLines(-327.825, -327.825+40, rPos[b], rPos[b], 2, kGray+2, 7);
        DrawLines(327.825-40, 327.825, rPos[b], rPos[b], 2, kGray+2, 7);
      }
    }
    // DrawLatex(0.21, 0.11, labels[5], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    // DrawLatex(0.3, 0.11, labels[4], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    // DrawLatex(0.39, 0.11, labels[3], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    // DrawLatex(0.48, 0.11, labels[2], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    // DrawLatex(0.57, 0.11, labels[1], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    // DrawLatex(0.69, 0.11, labels[0], false,textSizeRel*0.85, 62,0.05, 1, 90.);
    
    DrawLines(-327.825, -327.825,-rPos[4],  rPos[4], 2, kBlack, 7);  
    DrawLines(327.825, 327.825, -rPos[4],  rPos[4], 2, kBlack, 7);  
}

//__________________________________________________________________________________________________________
void SetStyleMultiGraph(  TMultiGraph* mgraph,
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
  mgraph->GetXaxis()->SetTitle(XTitle);
  mgraph->GetYaxis()->SetTitle(YTitle);
  mgraph->SetTitle("");

  mgraph->GetYaxis()->SetLabelFont(textFontLabel);
  mgraph->GetXaxis()->SetLabelFont(textFontLabel);
  mgraph->GetYaxis()->SetTitleFont(textFontTitle);
  mgraph->GetXaxis()->SetTitleFont(textFontTitle);

  mgraph->GetXaxis()->SetLabelSize(xLableSize);
  mgraph->GetXaxis()->SetTitleSize(xTitleSize);
  mgraph->GetXaxis()->SetTitleOffset(xTitleOffset);
  mgraph->GetXaxis()->SetNdivisions(xNDivisions,kTRUE);

  mgraph->GetYaxis()->SetDecimals();
  mgraph->GetYaxis()->SetLabelSize(yLableSize);
  mgraph->GetYaxis()->SetTitleSize(yTitleSize);
  mgraph->GetYaxis()->SetTitleOffset(yTitleOffset);
  mgraph->GetYaxis()->SetNdivisions(yNDivisions,kTRUE);
}

