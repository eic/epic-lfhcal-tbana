#include "MultiCanvas.h"

ClassImp(MultiCanvas);

//==========================================================================
// Initialize function
//==========================================================================
bool MultiCanvas::Initialize(int opt){
  
  //***********************************************************************
  //**** Create 1D canvas and pads
  //***********************************************************************
  if (opt == 1){
    if (init){
      std::cout << "1D version of MultiCanvas already initialized for " << addName.Data() << std::endl;
      return init;
    }
    
    textSize = 30;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Create Canvas and Pads depending on detector type set
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Single tile setup
    if ( detType ==  DetConf::Type::SingleTile){
      canvasMulti = new TCanvas(Form("canvasLayer%s",addName.Data()),"",0,0,620,600);
      DefaultCanvasSettings( canvasMulti,0.12, 0.03, 0.03, 0.1);
      legPlace_X[0] = 0.95;
      legPlace_Y[0] = 0.95;
      relTextSize[0] = {30./620};
      init = true;
      
    // 2M module setup horizontal
    } else if ( detType ==  DetConf::Type::Single2MH){
      CreateCanvasAndPadsFor2PannelTBPlot(textSize, 0.03, addName);
      init = true;
      
    // 2M module setup vertical
    } else if ( detType ==  DetConf::Type::Single2MV){
      init = false;
      std::cout << "Setup-1d: this option hasn't been implemented yet!" << std::endl;

    // 4M module setup
    } else if (detType == DetConf::Type::Single4M){
      init = false;
      std::cout << "Setup-1d: this option hasn't been implemented yet!" << std::endl;
    
    // 8M module setup
    } else if (detType ==  DetConf::Type::Single8M){
      CreateCanvasAndPadsFor8PannelTBPlot(textSize, 0.03, addName);
      maxPads       = 8;
      std::cout << canvasMulti << std::endl;
      init = true;
      
    // Dual 8M module setup  
    } else if ( detType == DetConf::Type::Dual8M){
      CreateCanvasAndPadsForDualModTBPlot(textSize, 0.03, addName);        
      maxPads       = 16;
      init = true;
    // large TB setup 2026 (2x4 8M modules)
    } else if ( detType ==  DetConf::Type::LargeTB){
      init = false;
      std::cout << "Setup-1d: this option hasn't been implemented yet!" << std::endl;
    // full asic plot
    } else if ( detType ==  DetConf::Type::Asic){
      CreateCanvasAndPadsForAsicLFHCalTBPlot( textSize, 0.045, addName);
      maxPads       = 64;      
      init = true;
    }

    if (init)
      return true;
    else 
      return false;

    
  //***********************************************************************
  //**** Create 2D canvas and pads
  //***********************************************************************    
  } else if (opt == 2){
    if (init){
      std::cout << "2D version of MultiCanvas already initialized for " << addName.Data() << std::endl;
      return init;
    }
    
    textSize = 30;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Create Canvas and Pads depending on detector type set
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Single tile setup
    if ( detType ==  DetConf::Type::SingleTile){
      canvasMulti = new TCanvas(Form("canvasLayer%sProf",addName.Data()),"",0,0,620,600);
      DefaultCanvasSettings( canvasMulti,0.138, 0.08, 0.03, 0.1);
      legPlace_X[0] = 0.175;
      legPlace_Y[0] = 0.95;
      relTextSize[0] = 30./620;
      maxPads       = 1;
      init = true;
    // 4M module setup
    } else if ( detType ==  DetConf::Type::Single4M){
      init = false;
      std::cout << "Setup-2d: this option hasn't been implemented yet!" << std::endl;
    
    // 2M module setup horizontal
    } else if ( detType ==  DetConf::Type::Single2MH){
      CreateCanvasAndPadsFor2PannelTBPlot( textSize, 0.075, addName+"Prof", false);
      maxPads       = 2;
      init = true;

    // 2M module setup vertical
    } else if ( detType ==  DetConf::Type::Single2MV){
      init = false;
      std::cout << "Setup-2d: this option hasn't been implemented yet!" << std::endl;
    
    // 8M module setup
    } else if (detType == DetConf::Type::Single8M){
      CreateCanvasAndPadsFor8PannelTBPlot(textSize, 0.045, addName+"Prof", false);
      maxPads       = 8;
      init = true;

    // Dual 8M module setup  
    } else if ( detType ==  DetConf::Type::Dual8M){
      CreateCanvasAndPadsForDualModTBPlot(textSize, 0.045, addName+"Prof", true);
      maxPads       = 16;
      init = true;

    // large TB setup 2026 (2x4 8M modules)
    } else if ( detType == DetConf::Type::LargeTB){
      init = false;
      std::cout << "Setup-2d: this option hasn't been implemented yet!" << std::endl;
        
    // full asic plot
    } else if ( detType == DetConf::Type::Asic){
      CreateCanvasAndPadsForAsicLFHCalTBPlot( textSize, 0.045, addName+"Prof", true);
      maxPads       = 64;
      init = true;
    }

    if (init)
      return true;
    else 
      return false;
    
    return true;
  } else {
    std::cout << "This option isn't defined please check MultiCanvas::Initialize() for correct setup options!" << std::endl;
    return false;
  }   
}


//__________________________________________________________________________________________________________
void MultiCanvas::ReturnCorrectValuesForCanvasScaling(   Int_t sizeX,
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
                                            int verbose){
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
void MultiCanvas::ReturnCorrectValuesTextSize(   TPad * pad,
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
void MultiCanvas::CreateCanvasAndPadsFor8PannelTBPlot(Int_t textSizePixel, 
                                                      Double_t marginLeft, 
                                                      TString add , 
                                                      bool rightCorner, 
                                                      int debug ){
  Double_t arrayBoundX[5];
  Double_t arrayBoundY[3];
  Double_t relativeMarginsX[3];
  Double_t relativeMarginsY[3];
  ReturnCorrectValuesForCanvasScaling(2200,1200, 4, 2,marginLeft, 0.005, 0.005,0.05,arrayBoundX,arrayBoundY,relativeMarginsX,relativeMarginsY, debug);

  
  
  canvasMulti = new TCanvas(Form("canvas8Panel%s", add.Data()),"",0,0,2200,1200);  // gives the page size
  canvasMulti->cd();

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
  padMulti[0] = new TPad(Form("pad8Panel%s_0", add.Data()), "", arrayBoundX[0], arrayBoundY[2], arrayBoundX[1], arrayBoundY[1],-1, -1, -2);
  padMulti[1] = new TPad(Form("pad8Panel%s_1", add.Data()), "", arrayBoundX[1], arrayBoundY[2], arrayBoundX[2], arrayBoundY[1],-1, -1, -2);
  padMulti[2] = new TPad(Form("pad8Panel%s_2", add.Data()), "", arrayBoundX[2], arrayBoundY[2], arrayBoundX[3], arrayBoundY[1],-1, -1, -2);
  padMulti[3] = new TPad(Form("pad8Panel%s_3", add.Data()), "", arrayBoundX[3], arrayBoundY[2], arrayBoundX[4], arrayBoundY[1],-1, -1, -2);
  padMulti[4] = new TPad(Form("pad8Panel%s_4", add.Data()), "", arrayBoundX[0], arrayBoundY[1],arrayBoundX[1], arrayBoundY[0],-1, -1, -2);
  padMulti[5] = new TPad(Form("pad8Panel%s_5", add.Data()), "", arrayBoundX[1], arrayBoundY[1],arrayBoundX[2], arrayBoundY[0],-1, -1, -2);
  padMulti[6] = new TPad(Form("pad8Panel%s_6", add.Data()), "", arrayBoundX[2], arrayBoundY[1],arrayBoundX[3], arrayBoundY[0],-1, -1, -2);
  padMulti[7] = new TPad(Form("pad8Panel%s_7", add.Data()), "", arrayBoundX[3], arrayBoundY[1],arrayBoundX[4], arrayBoundY[0],-1, -1, -2);
  
  DefaultPadSettings( padMulti[4], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[5], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[6], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[7], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[0], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[0], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
  DefaultPadSettings( padMulti[1], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
  DefaultPadSettings( padMulti[2], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
  DefaultPadSettings( padMulti[3], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[2]);

  legPlace_Y[0]  = 1-relativeMarginsY[1];
  legPlace_Y[1]  = 1-relativeMarginsY[1];
  legPlace_Y[2]  = 1-relativeMarginsY[1];
  legPlace_Y[3]  = 1-relativeMarginsY[1];
  legPlace_Y[4]  = 1-relativeMarginsY[0];
  legPlace_Y[5]  = 1-relativeMarginsY[0];
  legPlace_Y[6]  = 1-relativeMarginsY[0];
  legPlace_Y[7]  = 1-relativeMarginsY[0];      
  if (rightCorner){
    legPlace_X[0]  = 1-relativeMarginsX[1];
    legPlace_X[1]  = 1-relativeMarginsX[1];
    legPlace_X[2]  = 1-relativeMarginsX[1];
    legPlace_X[3]  = 1-relativeMarginsX[2];
    legPlace_X[4]  = 1-relativeMarginsX[1];
    legPlace_X[5]  = 1-relativeMarginsX[1];
    legPlace_X[6]  = 1-relativeMarginsX[1];
    legPlace_X[7]  = 1-relativeMarginsX[2];
  } else {
    legPlace_X[0]  = relativeMarginsX[0];
    legPlace_X[1]  = relativeMarginsX[1];
    legPlace_X[2]  = relativeMarginsX[1];
    legPlace_X[3]  = relativeMarginsX[1];
    legPlace_X[4]  = relativeMarginsX[0];
    legPlace_X[5]  = relativeMarginsX[1];
    legPlace_X[6]  = relativeMarginsX[1];
    legPlace_X[7]  = relativeMarginsX[1];
  }
  
  for (Int_t p = 0; p < 8; p++){
    if (padMulti[p]->XtoPixel(padMulti[p]->GetX2()) < padMulti[p]->YtoPixel(padMulti[p]->GetY1())){
      relTextSize[p]  = (Double_t)textSizePixel/padMulti[p]->XtoPixel(padMulti[p]->GetX2()) ;
    } else {
      relTextSize[p]  = (Double_t)textSizePixel/padMulti[p]->YtoPixel(padMulti[p]->GetY1());
    }
    if(debug > 1)std::cout << p << "\t" << legPlace_X[p]<< "\t" << legPlace_Y[p] << "\t" << relTextSize[p] << std::endl;
  }
  return;
}  

//********************************************************************************************************************************
//******** CreateCanvasAndPadsFor2PannelTBPlot ***********************************************************************************
//********************************************************************************************************************************
void MultiCanvas::CreateCanvasAndPadsFor2PannelTBPlot( Int_t textSizePixel, 
                                                        Double_t marginLeft, 
                                                        TString add, 
                                                        bool rightCorner, 
                                                        int debug
                                                    ){
  Double_t arrayBoundX[3];
  Double_t arrayBoundY[2];
  Double_t relativeMarginsX[3];
  Double_t relativeMarginsY[3];
  ReturnCorrectValuesForCanvasScaling(1200,600, 2, 1,marginLeft, 0.005, 0.015,0.1,arrayBoundX,arrayBoundY,relativeMarginsX,relativeMarginsY, 2);

  canvasMulti = new TCanvas(Form("canvas2Panel%s", add.Data()),"",0,0,1200,600);  // gives the page size
  canvasMulti->cd();

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
  
  padMulti[0] = new TPad(Form("pad2Panel%s_0", add.Data()), "", arrayBoundX[0], arrayBoundY[1], arrayBoundX[1], arrayBoundY[0],-1, -1, -2);
  padMulti[1] = new TPad(Form("pad2Panel%s_1", add.Data()), "", arrayBoundX[1], arrayBoundY[1], arrayBoundX[2], arrayBoundY[0],-1, -1, -2);
  
  
  DefaultPadSettings( padMulti[0], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[2]);
  DefaultPadSettings( padMulti[1], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[0], relativeMarginsY[2]);

  legPlace_Y[0]  = 1-relativeMarginsY[2];
  legPlace_Y[1]  = 1-relativeMarginsY[2];
  if (rightCorner){
    legPlace_X[0]  = 1-relativeMarginsX[1];
    legPlace_X[1]  = 1-relativeMarginsX[2];
  } else {
    legPlace_X[0]  = relativeMarginsX[0];
    legPlace_X[1]  = relativeMarginsX[1];
  }
  
  for (Int_t p = 0; p < 2; p++){
    if (padMulti[p]->XtoPixel(padMulti[p]->GetX2()) < padMulti[p]->YtoPixel(padMulti[p]->GetY1())){
      relTextSize[p]  = (Double_t)textSizePixel/padMulti[p]->XtoPixel(padMulti[p]->GetX2()) ;
    } else {
      relTextSize[p]  = (Double_t)textSizePixel/padMulti[p]->YtoPixel(padMulti[p]->GetY1());
    }
    // if(debug > 1)
      std::cout << p << "\t" << legPlace_X[p]<< "\t" << legPlace_Y[p] << "\t" << relTextSize[p] << std::endl;
  }
  return;
}  

//********************************************************************************************************************************
//******** CreateCanvasAndPadsFor8PannelTBPlot ***********************************************************************************
//********************************************************************************************************************************
void MultiCanvas::CreateCanvasAndPadsForDualModTBPlot(Int_t textSizePixel, 
                                                      Double_t marginLeft, 
                                                      TString add, 
                                                      bool rightCorner,
                                                      int debug
                                                      ){
  Double_t arrayBoundX[5];
  Double_t arrayBoundY[5];
  Double_t relativeMarginsX[3];
  Double_t relativeMarginsY[3];
  ReturnCorrectValuesForCanvasScaling(2200,2200, 4, 4,marginLeft, 0.005, 0.005,marginLeft,arrayBoundX,arrayBoundY,relativeMarginsX,relativeMarginsY, debug);

  canvasMulti = new TCanvas(Form("canvas4x4Panel%s", add.Data()),"",0,0,2200,2200);  // gives the page size
  canvasMulti->cd();

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
  
  padMulti[0]   = new TPad(Form("pad4x4Panel%s_0", add.Data()), "", arrayBoundX[0], arrayBoundY[4], arrayBoundX[1], arrayBoundY[3],-1, -1, -2);
  padMulti[1]   = new TPad(Form("pad4x4Panel%s_1", add.Data()), "", arrayBoundX[1], arrayBoundY[4], arrayBoundX[2], arrayBoundY[3],-1, -1, -2);
  padMulti[2]   = new TPad(Form("pad4x4Panel%s_2", add.Data()), "", arrayBoundX[2], arrayBoundY[4], arrayBoundX[3], arrayBoundY[3],-1, -1, -2);
  padMulti[3]   = new TPad(Form("pad4x4Panel%s_3", add.Data()), "", arrayBoundX[3], arrayBoundY[4], arrayBoundX[4], arrayBoundY[3],-1, -1, -2);
  padMulti[4]   = new TPad(Form("pad4x4Panel%s_4", add.Data()), "", arrayBoundX[0], arrayBoundY[3],arrayBoundX[1], arrayBoundY[2],-1, -1, -2);
  padMulti[5]   = new TPad(Form("pad4x4Panel%s_5", add.Data()), "", arrayBoundX[1], arrayBoundY[3],arrayBoundX[2], arrayBoundY[2],-1, -1, -2);
  padMulti[6]   = new TPad(Form("pad4x4Panel%s_6", add.Data()), "", arrayBoundX[2], arrayBoundY[3],arrayBoundX[3], arrayBoundY[2],-1, -1, -2);
  padMulti[7]   = new TPad(Form("pad4x4Panel%s_7", add.Data()), "", arrayBoundX[3], arrayBoundY[3],arrayBoundX[4], arrayBoundY[2],-1, -1, -2);
  padMulti[8]   = new TPad(Form("pad4x4Panel%s_8", add.Data()), "", arrayBoundX[0], arrayBoundY[2], arrayBoundX[1], arrayBoundY[1],-1, -1, -2);
  padMulti[9]   = new TPad(Form("pad4x4Panel%s_9", add.Data()), "", arrayBoundX[1], arrayBoundY[2], arrayBoundX[2], arrayBoundY[1],-1, -1, -2);
  padMulti[10]  = new TPad(Form("pad4x4Panel%s_10", add.Data()), "", arrayBoundX[2], arrayBoundY[2], arrayBoundX[3], arrayBoundY[1],-1, -1, -2);
  padMulti[11]  = new TPad(Form("pad4x4Panel%s_11", add.Data()), "", arrayBoundX[3], arrayBoundY[2], arrayBoundX[4], arrayBoundY[1],-1, -1, -2);
  padMulti[12]  = new TPad(Form("pad4x4Panel%s_12", add.Data()), "", arrayBoundX[0], arrayBoundY[1],arrayBoundX[1], arrayBoundY[0],-1, -1, -2);
  padMulti[13]  = new TPad(Form("pad4x4Panel%s_13", add.Data()), "", arrayBoundX[1], arrayBoundY[1],arrayBoundX[2], arrayBoundY[0],-1, -1, -2);
  padMulti[14]  = new TPad(Form("pad4x4Panel%s_14", add.Data()), "", arrayBoundX[2], arrayBoundY[1],arrayBoundX[3], arrayBoundY[0],-1, -1, -2);
  padMulti[15]  = new TPad(Form("pad4x4Panel%s_15", add.Data()), "", arrayBoundX[3], arrayBoundY[1],arrayBoundX[4], arrayBoundY[0],-1, -1, -2);
  
  
  
  
  DefaultPadSettings( padMulti[0], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
  DefaultPadSettings( padMulti[1], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
  DefaultPadSettings( padMulti[2], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
  DefaultPadSettings( padMulti[3], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[2]);
  DefaultPadSettings( padMulti[4], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[5], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[6], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[7], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[8], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[9], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[10], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[11], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[12], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[13], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[14], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
  DefaultPadSettings( padMulti[15], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[0], relativeMarginsY[1]);

  for (int i = 0; i < 12; i++)
    legPlace_Y[i]  = 1-relativeMarginsY[1];
  for (int i = 12; i < 16; i++)
    legPlace_Y[i]  = 1-relativeMarginsY[0];
  if (rightCorner){
    for (int i = 0; i < 16; i++){
      if ((i+1)%4 == 0)
        legPlace_X[i]  = 1-relativeMarginsX[2];
      else 
        legPlace_X[i]  = 1-relativeMarginsX[1];
    }
  } else {
    for (int i = 0; i < 16; i++){
      if (i%4 == 0)
        legPlace_X[i]  = 1-relativeMarginsX[0];
      else 
        legPlace_X[i]  = 1-relativeMarginsX[1];
    }
  }
  
  for (Int_t p = 0; p < 16; p++){
    if (padMulti[p]->XtoPixel(padMulti[p]->GetX2()) < padMulti[p]->YtoPixel(padMulti[p]->GetY1())){
      relTextSize[p]  = (Double_t)textSizePixel/padMulti[p]->XtoPixel(padMulti[p]->GetX2()) ;
    } else {
      relTextSize[p]  = (Double_t)textSizePixel/padMulti[p]->YtoPixel(padMulti[p]->GetY1());
    }
    if(debug > 1)std::cout << p << "\t" << legPlace_X[p]<< "\t" << legPlace_Y[p] << "\t" << relTextSize[p] << std::endl;
  }
  return;
}    

//********************************************************************************************************************************
//******** CreateCanvasAndPadsFor8PannelTBPlot ***********************************************************************************
//********************************************************************************************************************************
void MultiCanvas::CreateCanvasAndPadsForAsicLFHCalTBPlot(Int_t textSizePixel,
                                                         Double_t marginLeft, 
                                                         TString add, 
                                                         bool rightCorner, 
                                                         int debug
                                                         ){
  Double_t arrayBoundX[9];
  Double_t arrayBoundY[9];
  Double_t relativeMarginsX[3];
  Double_t relativeMarginsY[3];
  ReturnCorrectValuesForCanvasScaling(3300,3300, 8, 8,marginLeft, 0.005, 0.005,marginLeft,arrayBoundX,arrayBoundY,relativeMarginsX,relativeMarginsY, debug);

  canvasMulti = new TCanvas(Form("canvas8x8Panel%s", add.Data()),"",0,0,3300,3300);  // gives the page size
  canvasMulti->cd();

  for (int k = 0; k< 8; k++){
    padMulti[k+0*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+0*8), "", arrayBoundX[k], arrayBoundY[8], arrayBoundX[k+1], arrayBoundY[7],-1, -1, -2);
    padMulti[k+1*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+8), "", arrayBoundX[k], arrayBoundY[7], arrayBoundX[k+1], arrayBoundY[6],-1, -1, -2);
    padMulti[k+2*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+2*8), "", arrayBoundX[k], arrayBoundY[6], arrayBoundX[k+1], arrayBoundY[5],-1, -1, -2);
    padMulti[k+3*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+3*8), "", arrayBoundX[k], arrayBoundY[5], arrayBoundX[k+1], arrayBoundY[4],-1, -1, -2);
    padMulti[k+4*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+4*8), "", arrayBoundX[k], arrayBoundY[4], arrayBoundX[k+1], arrayBoundY[3],-1, -1, -2);
    padMulti[k+5*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+5*8), "", arrayBoundX[k], arrayBoundY[3], arrayBoundX[k+1], arrayBoundY[2],-1, -1, -2);
    padMulti[k+6*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+6*8), "", arrayBoundX[k], arrayBoundY[2], arrayBoundX[k+1], arrayBoundY[1],-1, -1, -2);
    padMulti[k+7*8]   = new TPad(Form("pad8x8Panel%s_%d", add.Data(),k+7*8), "", arrayBoundX[k], arrayBoundY[1], arrayBoundX[k+1], arrayBoundY[0],-1, -1, -2);
  }
      
  for (int k = 0; k < 64; k++){ 
    // 0th column
    if (k%8 ==0 && k > 7 && k < 56 )
      DefaultPadSettings( padMulti[k], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
    else if (k == 0)
      DefaultPadSettings( padMulti[k], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
    else if (k == 56)
      DefaultPadSettings( padMulti[k], relativeMarginsX[0], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
    
    //7th column
    else if (k%8 ==7 && k > 7 && k < 56 )
      DefaultPadSettings( padMulti[k], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[1]);
    else if (k == 7)
      DefaultPadSettings( padMulti[k], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[1], relativeMarginsY[2]);
    else if (k == 63)
      DefaultPadSettings( padMulti[k], relativeMarginsX[1], relativeMarginsX[2], relativeMarginsY[0], relativeMarginsY[1]);
    
    // bottom row
    else if (k > 0 && k < 7)
      DefaultPadSettings( padMulti[k], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[2]);
    // top row
    else if (k > 56 && k < 63)
      DefaultPadSettings( padMulti[k], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[0], relativeMarginsY[1]);
    // everything in the middle
    else 
      DefaultPadSettings( padMulti[k], relativeMarginsX[1], relativeMarginsX[1], relativeMarginsY[1], relativeMarginsY[1]);
  }
  
  // everything except top row
  for (int i = 0; i < 56; i++)
    legPlace_Y[i]  = 1-relativeMarginsY[1];
  // top row
  for (int i = 56; i < 64; i++)
    legPlace_Y[i]  = 1-relativeMarginsY[0];
  
  // right corner
  if (rightCorner){
    for (int i = 0; i < 64; i++){
      if (i%8 == 7)
        legPlace_X[i]  = 1-relativeMarginsX[2];
      else 
        legPlace_X[i]  = 1-relativeMarginsX[1];
    }
  // left corner
  } else {
    for (int i = 0; i < 64; i++){
      if (i%8 == 0)
        legPlace_X[i]  = relativeMarginsX[0];
      else 
        legPlace_X[i]  = relativeMarginsX[1];
    }
  }
  
  for (Int_t p = 0; p < 64; p++){
    if (padMulti[p]->XtoPixel(padMulti[p]->GetX2()) < padMulti[p]->YtoPixel(padMulti[p]->GetY1())){
      relTextSize[p]  = (Double_t)textSizePixel/padMulti[p]->XtoPixel(padMulti[p]->GetX2()) ;
    } else {
      relTextSize[p]  = (Double_t)textSizePixel/padMulti[p]->YtoPixel(padMulti[p]->GetY1());
    }
    if(debug > 1)std::cout << p << "\t" << legPlace_X[p]<< "\t" << legPlace_Y[p] << "\t" << relTextSize[p] << std::endl;
  }
  return;
}   


//__________________________________________________________________________________________________________
void MultiCanvas::DefaultCanvasSettings( TCanvas* c1,
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
void MultiCanvas::DefaultPadSettings( TPad* pad1,
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


void MultiCanvas::PlotNoiseWithFits( std::map<int,TileSpectra> spectra, int option, 
                        Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                        TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, int debug ){
  
  std::cout << "plotting: " << nameOutputBase.Data() << std::endl;
  Setup* setup = Setup::GetInstance();
  if (detType ==  DetConf::Type::SingleTile){
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }    
        PlotNoiseWithFits1MLayer (canvasMulti, legPlace_X[0], legPlace_Y[0], relTextSize[0], textSize, 
                                  spectra,  option, xPMin, xPMax, scaleYMax, l, m,
                                  Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);
      }
    }
  } else if (detType ==  DetConf::Type::Single2MH){    
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }  
        PlotNoiseWithFits2MLayer (canvasMulti, padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                                  spectra, option, xPMin, xPMax, scaleYMax, l, m,
                                    Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);
        
      }
    }
  } else if (detType ==  DetConf::Type::Single8M){
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }
        PlotNoiseWithFits8MLayer (canvasMulti, padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                                  spectra, option, xPMin, xPMax, scaleYMax, l, m,
                                    Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);
      }
    }
  } else if (detType ==  DetConf::Type::Dual8M){  
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){      
      if (l%5 == 0 && l > 0 && debug > 0)
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }    
      if (!calib->IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }        
      
      PlotNoiseWithFits2ModLayer (canvasMulti, padMulti, legPlace_X, legPlace_Y, 
                                  relTextSize, textSize, 
                                  spectra, option, xPMin, xPMax, scaleYMax, l,
                                  Form("%s_Layer%02d.%s" ,nameOutputBase.Data(), l, suffix.Data()), currRunInfo);
    }
  }
}  
  
  
//_____________________________________________________________________________________________________________
// plotting wrapper for 2D correlation plots
//_____________________________________________________________________________________________________________
void MultiCanvas::PlotCorr2DLayer(  std::map<int,TileSpectra> spectra, int option, 
                                    Double_t xPMin, Double_t xPMax, Double_t minY, Double_t maxY, 
                                    TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, 
                                    int noCalib, int triggerCha, int debug ){
  
  std::cout << "plotting: " << nameOutputBase.Data() << std::endl;
  Setup* setup = Setup::GetInstance();
  // Single tile plotting
  if (detType ==  DetConf::Type::SingleTile){
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }    
        if (!calib->IsLayerEnabled(l,m)){
          std::cout << "====> layer " << l <<" in module " << m << " all channels masked" << std::endl;
          continue;
        }        
        
        PlotCorr2D1MLayer (canvasMulti, legPlace_X[0], legPlace_Y[0], relTextSize[0], textSize, 
                                  spectra,  option, xPMin, xPMax, maxY, l, m,
                                  Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);
      }
    }
  // Single 2M horizontal plotting    
  } else if (detType ==  DetConf::Type::Single2MH){    
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }
        PlotCorr2D2MLayer(canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                          spectra, option, xPMin, xPMin, maxY, l, m,
                          Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo, noCalib);
      }
    }
  // 2M module setup vertical
  } else if ( detType ==  DetConf::Type::Single2MV){
    std::cout << "PlotCorr2DLayer: this option hasn't been implemented yet!" << std::endl;
  // Single 8M plotting    
  } else if (detType ==  DetConf::Type::Single8M){
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }
        PlotCorr2D8MLayer(canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize,
                          spectra, option, xPMin, xPMax, maxY, l, m,
                          Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo, noCalib);        
      }
    }
  // Dual 8M plotting - 2025 TB setup
  } else if (detType ==  DetConf::Type::Dual8M){  
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){      
      if (l%5 == 0 && l > 0 && debug > 0)
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }    
      if (!calib->IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }        

      PlotCorr2D2ModLayer (canvasMulti, padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                           spectra, option, xPMin, xPMax, minY, maxY, l,
                           Form("%s_Layer%02d.%s" ,nameOutputBase.Data(), l, suffix.Data()), currRunInfo, noCalib, triggerCha);
    }
  // 2x4 8M module plotting - 2026 TB setup
  } else if ( detType == DetConf::Type::LargeTB){
    std::cout << "PlotCorr2DLayer: this option hasn't been implemented yet!" << std::endl;
        
  // full asic plot
  } else if ( detType == DetConf::Type::Asic){
    for (Int_t a = 0; a < setup->GetNMaxROUnit()+1; a++){  
      std::cout << "====> asic " << a << std::endl;
      PlotCorr2DAsicLFHCal(canvasMulti, padMulti, legPlace_X, legPlace_Y, relTextSize, textSize,
                        spectra, option, xPMin, xPMax, minY, maxY, a, 
                        Form("%s_Asic%02d.%s" ,nameOutputBase.Data(), a, suffix.Data()), currRunInfo, noCalib, triggerCha);      
    }
  }
}  
  
//_____________________________________________________________________________________________________________
// plotting wrapper for Spectra plots in noise region with fit and triggered distribution
//_____________________________________________________________________________________________________________
void MultiCanvas::PlotNoiseAdvWithFits(  std::map<int,TileSpectra> spectra, std::map<int,TileSpectra> spectraNoise, 
                                         int option, Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                                         TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, int debug ){
  
  std::cout << "plotting: " << nameOutputBase.Data() << std::endl;
  Setup* setup = Setup::GetInstance();
  // Single tile plotting
  if (detType ==  DetConf::Type::SingleTile){
    std::cout << "PlotNoiseAdvWithFits: this option hasn't been implemented yet!" << std::endl;
  // Single 2M horizontal plotting    
  } else if (detType ==  DetConf::Type::Single2MH){    
    std::cout << "PlotNoiseAdvWithFits: this option hasn't been implemented yet!" << std::endl;
  // 2M module setup vertical
  } else if ( detType ==  DetConf::Type::Single2MV){
    std::cout << "PlotNoiseAdvWithFits: this option hasn't been implemented yet!" << std::endl;
  // Single 8M plotting    
  } else if (detType ==  DetConf::Type::Single8M){
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }
        PlotNoiseAdvWithFits8MLayer (canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                                      spectra, spectraNoise, option, xPMin, xPMax, scaleYMax, l, m,
                                     Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);
      }
    }
  // Dual 8M plotting - 2025 TB setup
  } else if (detType ==  DetConf::Type::Dual8M){  
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){      
      if (l%5 == 0 && l > 0 && debug > 0)
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }    
      if (!calib->IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }        
      PlotNoiseAdvWithFits2ModLayer (canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                                     spectra, spectraNoise, option, xPMin, xPMax, scaleYMax, l, 
                                     Form("%s_Layer%02d.%s" ,nameOutputBase.Data(), l, suffix.Data()), currRunInfo);
    }
  // 2x4 8M module plotting - 2026 TB setup
  } else if ( detType == DetConf::Type::LargeTB){
    std::cout << "PlotNoiseAdvWithFits: this option hasn't been implemented yet!" << std::endl;
        
  // full asic plot
  } else if ( detType == DetConf::Type::Asic){
    std::cout << "PlotNoiseAdvWithFits: this option hasn't been implemented yet!" << std::endl;
  }
}  

//_____________________________________________________________________________________________________________
// plotting wrapper Spectra plots
//_____________________________________________________________________________________________________________
void MultiCanvas::PlotSpectra(  std::map<int,TileSpectra> spectra, int option, 
                                    Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                                    TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, 
                                    int debug ){
  
  std::cout << "plotting: " << nameOutputBase.Data() << std::endl;
  Setup* setup = Setup::GetInstance();
  // Single tile plotting
  if (detType ==  DetConf::Type::SingleTile){
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }    
        if (!calib->IsLayerEnabled(l,m)){
          std::cout << "====> layer " << l <<" in module " << m << " all channels masked" << std::endl;
          continue;
        }        
        PlotSpectra1MLayer (canvasMulti, legPlace_X[0], legPlace_Y[0], relTextSize[0], textSize, 
                            spectra, option, xPMin, xPMax, scaleYMax, l, m,
                            Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);
      }
    }
  // Single 2M horizontal plotting    
  } else if (detType ==  DetConf::Type::Single2MH){    
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }
        PlotSpectra2MLayer (canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                            spectra, option, xPMin, xPMax, scaleYMax, l, m,
                            Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);
      }
    }
  // 2M module setup vertical
  } else if ( detType ==  DetConf::Type::Single2MV){
    std::cout << "PlotSpectra: this option hasn't been implemented yet!" << std::endl;
  // Single 8M plotting    
  } else if (detType ==  DetConf::Type::Single8M){
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }
        PlotSpectra8MLayer (canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                            spectra, option, xPMin, xPMax, scaleYMax, l, m,
                            Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);        
      }
    }
  // Dual 8M plotting - 2025 TB setup
  } else if (detType ==  DetConf::Type::Dual8M){  
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){      
      if (l%5 == 0 && l > 0 && debug > 0)
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }    
      if (!calib->IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }        
      PlotSpectra2ModLayer (canvasMulti, padMulti, legPlace_X, legPlace_Y, relTextSize, textSize,
                            spectra, option, xPMin, xPMax, scaleYMax, l,
                            Form("%s_Layer%02d.%s" ,nameOutputBase.Data(), l, suffix.Data()), currRunInfo);
    }
  // 2x4 8M module plotting - 2026 TB setup
  } else if ( detType == DetConf::Type::LargeTB){
    std::cout << "PlotSpectra: this option hasn't been implemented yet!" << std::endl;
        
  // full asic plot
  } else if ( detType == DetConf::Type::Asic){
    std::cout << "PlotSpectra: this option hasn't been implemented yet!" << std::endl;
  }
}    

//_____________________________________________________________________________________________________________
// plotting wrapper for 2D correlation plots with Fits
//_____________________________________________________________________________________________________________
void MultiCanvas::PlotCorrWithFits( std::map<int,TileSpectra> spectra, int option, 
                                    Double_t xPMin, Double_t xPMax, Double_t minY, Double_t maxY, 
                                    TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, 
                                    int debug ){
  
  std::cout << "plotting: " << nameOutputBase.Data() << std::endl;
  Setup* setup = Setup::GetInstance();
  // Single tile plotting
  if (detType ==  DetConf::Type::SingleTile){
    std::cout << "PlotCorrWithFits: this option hasn't been implemented yet!" << std::endl;
  // Single 2M horizontal plotting    
  } else if (detType ==  DetConf::Type::Single2MH){    
    std::cout << "PlotCorrWithFits: this option hasn't been implemented yet!" << std::endl;
  // 2M module setup vertical
  } else if ( detType ==  DetConf::Type::Single2MV){
    std::cout << "PlotCorrWithFits: this option hasn't been implemented yet!" << std::endl;
  // Single 8M plotting    
  } else if (detType ==  DetConf::Type::Single8M){
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }
        PlotCorrWithFits8MLayer(canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                                spectra, option, xPMin, xPMax, maxY, l, m,
                                Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);        
      }
    }
  // Dual 8M plotting - 2025 TB setup
  } else if (detType ==  DetConf::Type::Dual8M){  
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){      
      if (l%5 == 0 && l > 0 && debug > 0)
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }    
      if (!calib->IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }        
      PlotCorrWithFits2ModLayer (canvasMulti, padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                                spectra, option, xPMin, xPMax, maxY, l,
                                Form("%s_Layer%02d.%s" ,nameOutputBase.Data(), l, suffix.Data()), currRunInfo);
    }
  // 2x4 8M module plotting - 2026 TB setup
  } else if ( detType == DetConf::Type::LargeTB){
    std::cout << "PlotCorrWithFits: this option hasn't been implemented yet!" << std::endl;
        
  // full asic plot
  } else if ( detType == DetConf::Type::Asic){
    std::cout << "PlotCorrWithFits: this option hasn't been implemented yet!" << std::endl;
  }
}    


//_____________________________________________________________________________________________________________
// plotting wrapper for Spectra plots with triggered Mip spectra and fit
//_____________________________________________________________________________________________________________
void MultiCanvas::PlotMipWithFits(  std::map<int,TileSpectra> spectra, std::map<int,TileSpectra> spectraTrigg, 
                                    int option, Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                                    TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, int debug ){
  
  std::cout << "plotting: " << nameOutputBase.Data() << std::endl;
  Setup* setup = Setup::GetInstance();
  // Single tile plotting
  if (detType ==  DetConf::Type::SingleTile){
    std::cout << "PlotMipWithFits: this option hasn't been implemented yet!" << std::endl;
  // Single 2M horizontal plotting    
  } else if (detType ==  DetConf::Type::Single2MH){    
    std::cout << "PlotMipWithFits: this option hasn't been implemented yet!" << std::endl;
  // 2M module setup vertical
  } else if ( detType ==  DetConf::Type::Single2MV){
    std::cout << "PlotMipWithFits: this option hasn't been implemented yet!" << std::endl;
  // Single 8M plotting    
  } else if (detType ==  DetConf::Type::Single8M){
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }
        PlotMipWithFits8MLayer (canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                                spectra, spectraTrigg, option, xPMin, xPMax, scaleYMax, l, m,
                                Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);
      }
    }
  // Dual 8M plotting - 2025 TB setup
  } else if (detType ==  DetConf::Type::Dual8M){  
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){      
      if (l%5 == 0 && l > 0 && debug > 0)
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }    
      if (!calib->IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }        
      PlotMipWithFits2ModLayer (canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                                spectra, spectraTrigg, option, xPMin, xPMax, scaleYMax, l,
                                Form("%s_Layer%02d.%s" ,nameOutputBase.Data(), l, suffix.Data()), currRunInfo);
    }
  // 2x4 8M module plotting - 2026 TB setup
  } else if ( detType == DetConf::Type::LargeTB){
    std::cout << "PlotMipWithFits: this option hasn't been implemented yet!" << std::endl;
        
  // full asic plot
  } else if ( detType == DetConf::Type::Asic){
    std::cout << "PlotMipWithFits: this option hasn't been implemented yet!" << std::endl;
  }
}  

//_____________________________________________________________________________________________________________
// plotting wrapper for Trigger Primitives
//_____________________________________________________________________________________________________________
void MultiCanvas::PlotTriggerPrim(  std::map<int,TileSpectra> spectra, 
                                    double avMip, double facLow, double facHigh,
                                    Double_t xPMin, Double_t xPMax, Double_t scaleYMax, 
                                    TString nameOutputBase, TString suffix,  RunInfo currRunInfo, Calib* calib, 
                                    int debug){
  
  
  std::cout << "plotting: " << nameOutputBase.Data() << std::endl;
  // std::cout << avMip << "\t" << facLow << "\t" << facHigh << std::endl;
  // std::cout << xPMin << "\t" << xPMax << "\t" << scaleYMax << std::endl;
  Setup* setup = Setup::GetInstance();
  // Single tile plotting
  if (detType ==  DetConf::Type::SingleTile){
    std::cout << "PlotTriggerPrim: this option hasn't been implemented yet!" << std::endl;
  // Single 2M horizontal plotting    
  } else if (detType ==  DetConf::Type::Single2MH){    
    std::cout << "PlotTriggerPrim: this option hasn't been implemented yet!" << std::endl;
  // 2M module setup vertical
  } else if ( detType ==  DetConf::Type::Single2MV){
    std::cout << "PlotTriggerPrim: this option hasn't been implemented yet!" << std::endl;
  // Single 8M plotting    
  } else if (detType ==  DetConf::Type::Single8M){
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }
        PlotTriggerPrim8MLayer (canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                                spectra, avMip, facLow, facHigh, xPMin, xPMax, scaleYMax, l, m,
                                Form("%s_Mod%02d_Layer%02d.%s" ,nameOutputBase.Data(), m, l, suffix.Data()), currRunInfo);
      }
    }
  // Dual 8M plotting - 2025 TB setup
  } else if (detType ==  DetConf::Type::Dual8M){  
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){      
      if (l%5 == 0 && l > 0 && debug > 0)
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }    
      if (!calib->IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }        
      PlotTriggerPrim2ModLayer (canvasMulti,padMulti, legPlace_X, legPlace_Y, relTextSize, textSize, 
                              spectra, avMip, facLow, facHigh, xPMin, xPMax, scaleYMax, l, 
                              Form("%s_Layer%02d.%s" ,nameOutputBase.Data(), l, suffix.Data()), currRunInfo);
    }
  // 2x4 8M module plotting - 2026 TB setup
  } else if ( detType == DetConf::Type::LargeTB){
    std::cout << "PlotTriggerPrim: this option hasn't been implemented yet!" << std::endl;
        
  // full asic plot
  } else if ( detType == DetConf::Type::Asic){
    std::cout << "PlotTriggerPrim: this option hasn't been implemented yet!" << std::endl;
  }  
}
