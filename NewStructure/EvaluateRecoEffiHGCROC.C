#include <TROOT.h>
#include <TString.h>
#include <TObject.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TChain.h>
#include <TMath.h>
#include <TVector3.h>
#include <iostream>
#include <fstream>
#include <TParticlePDG.h>
#include <TDatabasePDG.h>
#include <TRandom3.h>

#include <TCanvas.h>
#include <TPad.h>
#include <TH1.h>
#include <TH1D.h>
#include <TH1F.h>
#include <TH2.h>
#include <TH3.h>
#include <TFile.h>
#include <TH2D.h>
#include <TH2F.h>
#include <TString.h>
#include <TDatime.h>
#include <TF1.h>
#include <TF2.h>
#include <THStack.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TGraphAsymmErrors.h>
#include <TLine.h>
#include <TLatex.h>
#include <TArrow.h>
#include <TGraphErrors.h>
#include <TGaxis.h>
#include <TLegend.h>
#include <TFrame.h>
#include <TLorentzVector.h>

// #include "CommonHelperFunctions.h"

struct runRecData{
  int runNr;
  int pid;
  int nFPGA;
  int triggers;
  int recEvents;
  double recEffi;
  std::map<int,int> triggersPerFPGA;
  std::map<int,int> recEventsPerFPGA;
  std::map<int,double> effiPerFPGA;
  std::map<int,int> abortedEventsPerFPGA;
  std::map<int,int> inProgEventsPerFPGA;
};

TString PrintRunRecData( runRecData info, bool simple = false){
  TString toPrint = "";
  toPrint += Form("%d\t%d\t%d\t%d\t%0.3f\n", info.runNr, info.pid, info.triggers, info.recEvents, info.recEffi);
  if (!simple){
    for (int f= 0; f < info.nFPGA; f++){
      toPrint +=Form("\t\t%d\t%d\t%d\t%d\t%d\t\t%0.3f\n", f, info.triggersPerFPGA[f], info.recEventsPerFPGA[f], info.abortedEventsPerFPGA[f], info.inProgEventsPerFPGA[f], info.effiPerFPGA[f] );
    }
  }
  return toPrint;
}

  struct RunInfo{
    RunInfo(): runNr(0), species(""), pdg(0), energy(0), vop(0), vbr(0), lgSet(0), hgSet(0), posX(0), posY(0), shapetime(0), assemblyNr(0), year(-1), month(-1), readout(""), facility(""), beamline(""), samples(0), trigDelay(0) {}
    int runNr;
    TString species;
    int pdg;
    float energy;
    float vop;
    float vbr;
    int lgSet;
    int hgSet;
    float posX;
    float posY;
    float shapetime;
    int assemblyNr;
    int year;
    int month;
    TString readout;
    TString facility;
    TString beamline;
    int samples;
    int trigDelay;
    int trigDead;
    int phase;
    int nFPGA;
    int nASIC;
  } ;

  TString GetStringFromRunInfo(RunInfo, Int_t);

  //__________________________________________________________________________________________________________
  //__________________ Read run infos from text fitriggersle _________________________________________________________
  //__________________________________________________________________________________________________________    
  // specialData: 0 - std. TB, 1 - SPE data ORNL
  inline std::map<int,RunInfo> readRunInfosFromFile(TString runListFileName, int debug, int specialData = 0 ){
    std::map<int,RunInfo> runs;
    //std::cout << "INFO: You have given the following run list file: " << runListFileName.Data() << std::endl;
    std::ifstream runListFile;
    runListFile.open(runListFileName,std::ios_base::in);
    if (!runListFile) {
      std::cout << "ERROR: file " << runListFileName.Data() << " not found!" << std::endl;
      return runs;
    }

    TString facility="";
    TString beamline="";
    TString readout="";
    int year = -1;
    int month = -1;
    for( TString tempLine; tempLine.ReadLine(runListFile, kTRUE); ) {
      // check if line should be considered
      if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
        continue;
      }
      if (debug > 1) std::cout << tempLine.Data() << std::endl;
      
      TObjArray *tempArr2  = tempLine.Tokenize(" ");
      if(tempArr2->GetEntries()>0){
        if (tempLine.BeginsWith("year")){
          year=((TString)((TObjString*)tempArr2->At(1))->GetString()).Atoi();
          continue;
        } else if (tempLine.BeginsWith("month")){
          month=((TString)((TObjString*)tempArr2->At(1))->GetString()).Atoi();
          continue;
        } else if (tempLine.BeginsWith("readout")){
          readout=((TString)((TObjString*)tempArr2->At(1))->GetString());
          continue;
        } else if (tempLine.BeginsWith("facility")){
          facility=((TString)((TObjString*)tempArr2->At(1))->GetString());
          continue;
        } else if (tempLine.BeginsWith("beam-line")){ 
          beamline=((TString)((TObjString*)tempArr2->At(1))->GetString());
          continue;
        }
      }
      // Separate the string according to tabulators
      TObjArray *tempArr  = tempLine.Tokenize(",");
      if(tempArr->GetEntries()<1){
        if (debug > 1) std::cout << "nothing to be done" << std::endl;
        delete tempArr;
        continue;
      } 

      // Put them to the correct variables    
      RunInfo tempRun;
      tempRun.facility= facility;
      tempRun.beamline= beamline;
      tempRun.readout = readout;
      tempRun.year    = year; 
      tempRun.month    = month; 
      tempRun.runNr    = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
      tempRun.species  =  (TString)((TObjString*)tempArr->At(1))->GetString();
      tempRun.pdg      = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atoi();
      tempRun.energy   = ((TString)((TObjString*)tempArr->At(3))->GetString()).Atof();
      tempRun.vop      = ((TString)((TObjString*)tempArr->At(4))->GetString()).Atof();
      tempRun.vbr      = ((TString)((TObjString*)tempArr->At(5))->GetString()).Atof();
      
      if (readout.CompareTo("CAEN") == 0){
        tempRun.hgSet    = ((TString)((TObjString*)tempArr->At(6))->GetString()).Atoi();
        tempRun.lgSet    = ((TString)((TObjString*)tempArr->At(7))->GetString()).Atoi();
        if (tempArr->GetEntries() > 10){
         tempRun.shapetime = ((TString)((TObjString*)tempArr->At(10))->GetString()).Atof();
        }
      } else {
        tempRun.trigDelay = ((TString)((TObjString*)tempArr->At(6))->GetString()).Atoi();
        tempRun.samples   = ((TString)((TObjString*)tempArr->At(7))->GetString()).Atoi();
        tempRun.trigDead  = ((TString)((TObjString*)tempArr->At(10))->GetString()).Atoi();
        tempRun.phase     = ((TString)((TObjString*)tempArr->At(11))->GetString()).Atoi();
        tempRun.nFPGA     = ((TString)((TObjString*)tempArr->At(12))->GetString()).Atoi();
        tempRun.nASIC     = ((TString)((TObjString*)tempArr->At(13))->GetString()).Atoi();
      }
      tempRun.posX    = ((TString)((TObjString*)tempArr->At(8))->GetString()).Atoi();
      tempRun.posY    = ((TString)((TObjString*)tempArr->At(9))->GetString()).Atoi();
      if (specialData == 1) tempRun.assemblyNr = ((TString)((TObjString*)tempArr->At(10))->GetString()).Atoi();
                  
      if (debug > 1) std::cout << "Run " << tempRun.runNr << "\t species: " << tempRun.species << "\t energy: "  << tempRun.energy << "\t Vop: " << tempRun.vop << "\t Vov: " << tempRun.vop-tempRun.vbr << "\t Xbeam: " << tempRun.posX<< "\t Ybeam: " << tempRun.posY << "\t shaping time: " << tempRun.shapetime << std::endl;
      runs[tempRun.runNr]=tempRun;
    }
    std::cout << year << "-" << month << "\t:\t" << facility.Data() << "-" << beamline.Data() << "\t Readout: " << readout.Data() << std::endl;
    std::cout << "registered " << runs.size() << " runs from  "<< runListFileName.Data() << std::endl;
    
    return runs;
  };

  inline int GetSpeciesIntFromRunInfo(RunInfo currRunInfo){
      if (currRunInfo.species.Contains("cosmics")){
          return  0; // cosmics
      } else if (currRunInfo.species.CompareTo("g") == 0){
          return  1; // gamma
      } else if (currRunInfo.species.Contains("muon") || currRunInfo.species.Contains("Muon") || currRunInfo.species.CompareTo("mu-") == 0){
          return  2; // muon
      } else if (currRunInfo.species.Contains("Electron") || currRunInfo.species.Contains("electron") || currRunInfo.species.CompareTo("e-") == 0 ){
          return  3; // electron
      } else if (currRunInfo.species.Contains("Positron") || currRunInfo.species.Contains("positron") || currRunInfo.species.CompareTo("e+") == 0 ){
          return  6; // positron
      } else if (currRunInfo.species.Contains("Pion") || currRunInfo.species.Contains("pion") || currRunInfo.species.CompareTo("pi-") == 0 || currRunInfo.species.CompareTo("pi+") == 0 ){
          return  4; // pion
      } else if (currRunInfo.species.Contains("Hadron") || currRunInfo.species.Contains("hadron") || currRunInfo.species.CompareTo("h+") == 0 || currRunInfo.species.CompareTo("h-") == 0 ){
          return  5; // hadron/proton
      }
      
      return -1;
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
  // Plot Corr with Fits for Full layer
  //__________________________________________________________________________________________________________
  void PlotTrending (TCanvas* canvas2Panel, Double_t topRCornerX,  Double_t topRCornerY, Double_t relSizeP, Int_t textSizePixel, 
                              TGraph* graph, Double_t xPMin, Double_t xPMax, TString nameOutput){
                              //, RunInfo currRunInfo){
                                  
    Double_t minY = 0;
    Double_t maxY = 1.05;
    
    canvas2Panel->cd();
    if (!graph) return;;
    TH1D* dummyhist = new TH1D("dummyhist", "", 100, xPMin, xPMax);
    SetStyleHistoTH1ForGraphs( dummyhist, graph->GetXaxis()->GetTitle(), graph->GetYaxis()->GetTitle(), 0.85*textSizePixel, textSizePixel, 0.85*textSizePixel, textSizePixel,0.9, 1.02, 510, 510, 43, 63);  
    // if (optionTrend == 6)std::cout << "\t" << graph->GetXaxis()->GetTitle() << "\t" << graph->GetYaxis()->GetTitle() << std::endl;
    SetMarkerDefaultsTGraph(graph, 20, 1, kBlue+1, kBlue+1);   
    dummyhist->GetYaxis()->SetRangeUser(minY,maxY);
    dummyhist->Draw("axis");
    graph->Draw("pe, same");
                
//     TString lab1 = Form("#it{#bf{LFHCal TB:}} %s", GetStringFromRunInfo(currRunInfo, 9).Data());
//     TString lab2 = GetStringFromRunInfo(currRunInfo, 8);
//     TString lab3 = GetStringFromRunInfo(currRunInfo, 10);
//     DrawLatex(topRCornerX-0.045, topRCornerY-1.2*relSizeP-1*0.85*relSizeP, lab1, true, 0.85*textSizePixel, 43);
//     DrawLatex(topRCornerX-0.045, topRCornerY-1.2*relSizeP-2*0.85*relSizeP, lab2, true, 0.85*textSizePixel, 43);
//     DrawLatex(topRCornerX-0.045, topRCornerY-1.2*relSizeP-3*0.85*relSizeP, lab3, true, 0.85*textSizePixel, 43);
//   
    canvas2Panel->SaveAs(nameOutput.Data());
  }
//   
//   //  
  
  
//__________________________________________________________________________________________________________
//_____________________MAIN function !!! ___________________________________________________________________
//__________________________________________________________________________________________________________
void EvaluateRecoEffiHGCROC( TString configFileName     = "", 
                             TString outputDir          = "Compare/",
                             TString runListFileName    = "configs/SPS_RunNumbers.txt",
                             Int_t verbosity            = 0
                          ){
      
    StyleSettingsBasics();
    // ********************************************************************************************************
    // read run list and corresponding settings
    // ********************************************************************************************************
    std::map<int,RunInfo> ri=readRunInfosFromFile(runListFileName.Data(),verbosity,0);
    std::map<int,runRecData> rED;
    // ********************************************************************************************************    
    // read folder and name from file
    // ********************************************************************************************************
    // std::vector<Int_t> runnumbers;
    // std::vector<TString> fileNames;
    ifstream in;
    in.open(configFileName,ios_base::in);
    if (!in) {
        std::cout << "ERROR: file " << configFileName.Data() << " not found!" << std::endl;
        return;
    }

    TGraph* graphRecEffi = new TGraph();
    std::map<int,RunInfo>::iterator it;
    
    for( TString tempLine; tempLine.ReadLine(in, kTRUE); ) {
      // check if line should be considered
      if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
          continue;
      }
      if (verbosity > 0) std::cout << tempLine.Data() << std::endl;

      // Separate the string according to tabulators
      TObjArray *tempArr  = tempLine.Tokenize("_");
      if(tempArr->GetEntries()<1){
          if (verbosity > 1) std::cout << "nothing to be done" << std::endl;
          delete tempArr;
          continue;
      } else if (tempArr->GetEntries()==1){
          if (verbosity > 1) std::cout << "File name doesn't contain an _ :"  << tempLine.Data()<< std::endl;
      }
      TString fileEnd  = ((TString)((TObjString*)tempArr->At(tempArr->GetEntries()-1))->GetString());
      TObjArray *tempArr2  = fileEnd.Tokenize(".");
      if(tempArr2->GetEntries()<=1 ){
          if (verbosity > 1) std::cout << "line doesn't have a file ending" << std::endl;
          delete tempArr;
          delete tempArr2;
          continue;
      }
      TString ending  = ((TString)((TObjString*)tempArr2->At(1))->GetString());
      if (ending.CompareTo("root") != 0){
          if (verbosity > 1) std::cout << "This is not a root file, skipping.... : " << tempLine.Data() << std::endl;
          delete tempArr;
          delete tempArr2;
          continue;
      }  
      Int_t temprun     = ((TString)((TObjString*)tempArr2->At(0))->GetString()).Atoi();
      
      std::cout << temprun << "\t" << tempLine.Data() << std::endl;
      // clean-up
      delete tempArr;
      delete tempArr2;
      
      TFile* tempFile = new TFile(tempLine.Data(), "OPEN");
      if (tempFile->IsZombie()){
          std::cout << tempLine.Data() << " is broken, please remove from list or fix!" << std::endl;
          delete tempFile;
          continue;
      }
      it=ri.find(temprun);
      
      TH1D* histEvents = nullptr;
      TH1D* histEventPerFPGA[10]; // currently setting list to maximum 10 FPGAs
      
      histEvents = (TH1D*)tempFile->Get("hNEvents");
      if (!histEvents){
        std::cout << "Event histo does not exist for run " << temprun << "Aborting!"<< std::endl;
        delete tempFile;
        continue;
      }
      for (int f = 0; f < it->second.nFPGA; f++){
        histEventPerFPGA[f] = nullptr;
        histEventPerFPGA[f] = (TH1D*)tempFile->Get(Form("hNEventsKCU%i",f));
        if (!histEventPerFPGA[f]){
          std::cout << "Event histo for KCU "<<  f<<" does not exist for run " << temprun << "Aborting!"<< std::endl;
          delete tempFile;
          continue;
        }
      }
      
      runRecData tempRunRec;
      tempRunRec.runNr      = temprun;
      tempRunRec.pid        = it->second.pdg;
      tempRunRec.nFPGA      = it->second.nFPGA;
      tempRunRec.triggers     = histEvents->GetBinContent(1);
      tempRunRec.recEvents    = histEvents->GetBinContent(2);
      if (tempRunRec.triggers > 0)
        tempRunRec.recEffi      = (double)tempRunRec.recEvents/tempRunRec.triggers;
      else 
        tempRunRec.recEffi      = 0;
      for (int f= 0; f < tempRunRec.nFPGA; f++){
        tempRunRec.triggersPerFPGA[f]       = histEventPerFPGA[f]->GetBinContent(1);
        tempRunRec.recEventsPerFPGA[f]      = histEventPerFPGA[f]->GetBinContent(2);
        if (histEventPerFPGA[f]->GetBinContent(1)!= 0)
          tempRunRec.effiPerFPGA[f]         = (double)(tempRunRec.recEventsPerFPGA[f])/tempRunRec.triggersPerFPGA[f];
        else 
          tempRunRec.effiPerFPGA[f]         = 0.;
        tempRunRec.abortedEventsPerFPGA[f]  = histEventPerFPGA[f]->GetBinContent(3);
        tempRunRec.inProgEventsPerFPGA[f]   = histEventPerFPGA[f]->GetBinContent(4);
      }
      
      if(tempRunRec.triggers!= 0)graphRecEffi->AddPoint(temprun,tempRunRec.recEffi);

      
      std::cout << (TString)(PrintRunRecData(tempRunRec, false)).Data() << std::endl;
      rED[temprun] = tempRunRec;
    } 

    for (const auto& run : rED){
      std::cout << (TString)(PrintRunRecData(run.second, true)).Data();
    }
  
    graphRecEffi->Sort();
    graphRecEffi->GetXaxis()->SetTitle("Run Nr.");
    graphRecEffi->GetYaxis()->SetTitle("#varepsilon_{rec}");
    
    TCanvas* canvas = new TCanvas("canvas","",0,0,800,600);
    DrawCanvasSettings( canvas,0.08, 0.01, 0.01, 0.1);

    PlotTrending (canvas, 0.95,  0.95, 0.035, 30, 
                  graphRecEffi, graphRecEffi->GetX()[0]-2 , graphRecEffi->GetX()[graphRecEffi->GetN()-1]+2, Form("%s/RecEffi.pdf",outputDir.Data()) );
  
  
//     std::cout<<"=============================================================="<<std::endl;
//     std::vector<runInfo> compRuns;
//     std::vector<TGraphErrors*>graphsMPV;
//     std::vector<TGraphErrors*>graphsMax;
//     
//     Int_t runsToComp = (Int_t)runnumbers.size();
//     
//     for (Int_t r = 0; r < runsToComp; r++){
//       
//       Int_t indexCRun = findCurrentRun(allRuns, runnumbers.at(r));
//       runInfo currentRunInfo;
//       if (indexCRun < 0){
//         std::cout << "run not in current list of runs, provided" << std::endl;
//         return;
//       } else {
//         std::cout << "Run "<< runnumbers.at(r) << "\t found at index " << indexCRun << std::endl;
//         currentRunInfo = GetRunInfoObject(allRuns,indexCRun);
//       }
//       compRuns.push_back(currentRunInfo);
//       std::cout <<  runnumbers.at(r) << "\t" << fileNames.at(r).Data() << "\t" << compRuns.at(r).vop << std::endl;
//       
//       TFile* tempFile = new TFile(fileNames.at(r).Data(), "OPEN");
//       if (tempFile->IsZombie()){
//           std::cout << fileNames.at(r).Data() << " is broken, please remove from list or fix!" << std::endl;
//       }
//       TGraphErrors* tempGraph = (TGraphErrors*)tempFile->Get("graphMPV_HG_channels");
//       graphsMPV.push_back(tempGraph);
//       tempGraph = (TGraphErrors*)tempFile->Get("graphMax_HG_channels");
//       graphsMax.push_back(tempGraph);
//     }
//     std::cout<<"=============================================================="<<std::endl;
// 
//     std::vector<mipDataPoint> mipPoints;
//     for (Int_t r = 0; r < runsToComp; r++){
//       std::cout <<  runnumbers.at(r) << "\t" << fileNames.at(r).Data() << "\t" << compRuns.at(r).species.Data() << "\t" << compRuns.at(r).vop << std::endl;
//       if (graphsMPV.at(r)->GetN() !=graphsMax.at(r)->GetN() ){
//         std::cout << "MPV and Max graph don't have same size ... something is wrong!" << std::endl;
//         continue;
//       }
//       for (Int_t e = 0; e < (Int_t)graphsMPV.at(r)->GetN(); e++){
//         mipDataPoint currMip;
//         currMip.layer   = (int)(graphsMPV.at(r)->GetX()[e]/10);
//         currMip.channel = (int)(graphsMPV.at(r)->GetX()[e]-10*currMip.layer);
//         currMip.mpvL    = (float)(graphsMPV.at(r)->GetY()[e]);
//         currMip.empvL   = (float)(graphsMPV.at(r)->GetEY()[e]);
//         currMip.max     = (float)(graphsMax.at(r)->GetY()[e]);
//         currMip.emax    = (float)(graphsMax.at(r)->GetEY()[e]);
//         currMip.vop     = (float)(compRuns.at(r).vop);
//         currMip.runnr   = (int)(compRuns.at(r).runNr);
//         currMip.beame   = (float)(compRuns.at(r).energy);
//         if (compRuns.at(r).species.CompareTo("h") == 0)
//           currMip.isHad   = true;
//         else 
//           currMip.isHad   = false;
//         currMip.lgSet   = (float)(compRuns.at(r).lgSet);
//         currMip.hgSet   = (float)(compRuns.at(r).hgSet);
//         
// //         std::cout << graphsMPV.at(r)->GetX()[e] << "\t l: " << currMip.layer << "\t c: " << currMip.channel << "\t vop: " << currMip.vop << std::endl;
//         mipPoints.push_back(currMip);
//       }      
//     }
  return;
}
                        
