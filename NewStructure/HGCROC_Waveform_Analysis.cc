#include "HGCROC_Waveform_Analysis.h"
#include <vector>
#include "TROOT.h"
#include <bitset>
#ifdef __APPLE__
#include <unistd.h>
#endif
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TChain.h"
#include "CommonHelperFunctions.h"
#include "TileSpectra.h"
#include "PlotHelper.h"
#include "TRandom3.h"
#include "TMath.h"
#include "Math/Minimizer.h"
#include "Math/MinimizerOptions.h"

#include "waveform_fitting/waveform_fit_base.h"
#include "waveform_fitting/crystal_ball_fit.h"
#include "waveform_fitting/max_sample_fit.h"

// ****************************************************************************
// Checking and opening input and output files
// ****************************************************************************
bool HGCROC_Waveform_Analysis::CheckAndOpenIO(void){
  int matchingbranch;

  //Need to check first input to get the setup...I do not think it is necessary
  std::cout <<"=============================================================" << std::endl;
  std::cout<<"Input name set to: "<<RootInputName.Data() <<std::endl;
  std::cout<<"Output name set to: "<<RootOutputName.Data() <<std::endl;
  std::cout<<"Calib name set to: "<<RootCalibInputName.Data() <<std::endl;
  std::cout <<"=============================================================" << std::endl;
  if(!RootInputName.IsNull()){
    //File exist?
    RootInput=new TFile(RootInputName.Data(),"READ");
    if(RootInput->IsZombie()){
      std::cout<<"Error opening '"<<RootInputName<<"', does the file exist?"<<std::endl;
      return false;
    }

    //Retrieve info, start with setup
    TsetupIn = (TTree*) RootInput->Get("Setup");
    if(!TsetupIn){
      std::cout<<"Could not retrieve the Setup tree, leaving"<<std::endl;
      return false;
    }
    setup=Setup::GetInstance();
    std::cout<<"Setup add "<<setup<<std::endl;
    //matchingbranch=TsetupIn->SetBranchAddress("setup",&setup);
    matchingbranch=TsetupIn->SetBranchAddress("setup",&rswptr);
    if(matchingbranch<0){
      std::cout<<"Error retrieving Setup info from the tree"<<std::endl;
      return false;
    }
    std::cout<<"Entries "<<TsetupIn->GetEntries()<<std::endl;
    TsetupIn->GetEntry(0);
    setup->Initialize(*rswptr);
    std::cout<<"Reading "<<RootInput->GetName()<<std::endl;
    std::cout<<"Setup Info "<<setup->IsInit()<<"  and  "<<setup->GetCellID(0,0)<<std::endl;
    //std::cout<<"Setup add now "<<setup<<std::endl;

    //Retrieve info, existing data?
    TdataIn = (TTree*) RootInput->Get("Data");
    if(!TdataIn){
      std::cout<<"Could not retrieve the Data tree, leaving"<<std::endl;
      return false;
    }
    matchingbranch=TdataIn->SetBranchAddress("event",&eventptr);
    if(matchingbranch<0){
      std::cout<<"Error retrieving Event info from the tree"<<std::endl;
      return false;
    }
    
    TcalibIn = (TTree*) RootInput->Get("Calib");
    if(!TcalibIn){
      std::cout<<"Could not retrieve Calib tree, leaving"<<std::endl;
      //return false;
    }
    else {
      matchingbranch=TcalibIn->SetBranchAddress("calib",&calibptr);
      if(matchingbranch<0){
        std::cout<<"Error retrieving calibration info from the tree"<<std::endl;
        TcalibIn=nullptr;
      }
    }
  }
    //Setup Output files
  if(!RootOutputName.IsNull()){    
    TString temp = RootOutputName;
    temp         = temp.ReplaceAll(".root","_Hists.root");
    SetRootOutputHists(temp);
    // std::cout << "creating additional histo file: " << RootOutputNameHist.Data() << " tree in : "<< RootOutputName.Data() << std::endl;
    
    bool sCOF = CreateOutputRootFile();
    if (!sCOF) return false;
    
    TsetupOut = new TTree("Setup","Setup");
    setup=Setup::GetInstance();
    //TsetupOut->Branch("setup",&setup);
    TsetupOut->Branch("setup",&rsw);
    TdataOut = new TTree("Data","Data");
    TdataOut->Branch("event",&event);
    TcalibOut = new TTree("Calib","Calib");
    TcalibOut->Branch("calib",&calib);
  }
  
  if(!RootCalibInputName.IsNull()){
    RootCalibInput=new TFile(RootCalibInputName.Data(),"READ");
    if(RootCalibInput->IsZombie()){
      std::cout<<"Error opening '"<<RootCalibInputName<<"', does the file exist?"<<std::endl;
      return false;
    }
    TcalibIn = nullptr;
    TcalibIn = (TTree*) RootCalibInput->Get("Calib");
    if(!TcalibIn){
      std::cout<<"Could not retrieve Calib tree, leaving"<<std::endl;
      return false;
    } else {
      std::cout<<"Retrieved calib object from external input!"<<std::endl;
    }
    matchingbranch=TcalibIn->SetBranchAddress("calib",&calibptr);
    if(matchingbranch<0){
      std::cout<<"Error retrieving calibration info from the tree"<<std::endl;
      return false;
    }else {
      std::cout<<"Correctly set branch for external calib input."<<std::endl;
    }
    
  }

  std::cout <<"=============================================================" << std::endl;
  std::cout <<" Basic setup complete" << std::endl;
  std::cout <<"=============================================================" << std::endl;
  return true;    
}

// ****************************************************************************
// Primary process function to start all calibrations
// ****************************************************************************
bool HGCROC_Waveform_Analysis::Process(void){
  bool status = true;
  ROOT::EnableImplicitMT();
  
  if (IsAnalyseWaveForm){
    status=AnalyseWaveForm();
  }

  return status;
}

// ****************************************************************************
// Analyse waveform
// ****************************************************************************
bool HGCROC_Waveform_Analysis::AnalyseWaveForm(void){
  std::cout<<"Analyse Waveform"<<std::endl;
  int evts=TdataIn->GetEntries();

  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  
  std::map<int,TileSpectra> hSpectra;
  std::map<int, TileSpectra>::iterator ithSpectra;
  std::map<int,TileSpectra> hSpectraTrigg;
  std::map<int, TileSpectra>::iterator ithSpectraTrigg;
  TcalibIn->GetEntry(0);
  // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }  
  int runNr = -1;

  //==================================================================================
  // create additional output hist
  //==================================================================================
  std::cout << "Additional Output with histos being created: " << RootOutputNameHist.Data() << std::endl;
  if(Overwrite){
    std::cout << "recreating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"RECREATE");
  } else{
    std::cout << "newly creating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"CREATE");
  }

  int maxChannelPerLayer             = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1)*(setup->GetNMaxModule()+1);
  int maxChannelPerLayerSingleMod    = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1);
  TH2D* hHighestADCAbovePedVsLayer   = new TH2D( "hHighestEAbovePedVsLayer","Highest ADC above PED; layer; brd channel; #Sigma(ADC) (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hHighestADCAbovePedVsLayer->SetDirectory(0);
  hHighestADCAbovePedVsLayer->Sumw2();


  TH2D* hspectraTOTvsCellID      = new TH2D( "hspectraTOTvsCellID","TOT spectrums CellID; cell ID; TOT (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4096,0,4096);
  hspectraTOTvsCellID->SetDirectory(0);
  TH2D* hspectraTOAvsCellID      = new TH2D( "hspectraTOAvsCellID","TOA spectrums CellID; cell ID; TOA (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 1024,0,1024);
  hspectraTOAvsCellID->SetDirectory(0);

  TH2D* hSampleTOAVsCellID       = new TH2D( "hSampleTOAvsCellID","#sample ToA; cell ID; #sample TOA",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 20,0,20);
  hSampleTOAVsCellID->SetDirectory(0);
  TH2D* hSampleMaxADCVsCellID       = new TH2D( "hSampleMaxADCvsCellID","#sample ToA; cell ID; #sample Max ADC",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 20,0,20);
  hSampleMaxADCVsCellID->SetDirectory(0);
  TH2D* hSampleDiffvsCellID       = new TH2D( "hSampleDiffvsCellID","#sample diff; cell ID; #sample ToA-#sample Max ADC",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 8,-0.5,7.5);
  hSampleDiffvsCellID->SetDirectory(0);
  
  TH1D* hSampleTOA               = new TH1D( "hSampleTOA","#sample ToA; #sample TOA",
                                              20,0,20);
  TH1D* hSampleMaxADC            = new TH1D( "hSampleMaxADC","#sample ToA; #sample maxADC",
                                              20,0,20);
  TH1D* hSampleAboveTh           = new TH1D( "hSampleAboveTh","#sample ToA; #sample above th",
                                              20,0,20);
  TH1D* hSampleDiff              = new TH1D( "hSampleDiff","#sample ToA-#sample Max ADC; #sample maxADC-#sampleMax ADC",
                                              8,-0.5,7.5);
  TH1D* hSampleDiffMin           = new TH1D( "hSampleDiffMin","#sample ToA-#sample above th; #sample maxADC-#sample  above th",
                                              8,-0.5,7.5);
  
  RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->mkdir("IndividualCellsTrigg");
  RootOutputHist->cd("IndividualCells");
  
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Migrad");  
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  ReadOut::Type typeRO = ReadOut::Type::Caen;
  
  //==================================================================================
  // setup waveform builder for HGCROC data
  //==================================================================================
  waveform_fit_base *waveform_builder = nullptr;
  waveform_builder = new max_sample_fit();
  double minNSigma = 5;
  int nCellsAboveTOA  = 0;
  int nCellsAboveMADC = 0;
  double totADCs      = 0.;
  //==================================================================================
  // process events from tree
  //==================================================================================
  for(int i=0; i<evts && i < maxEvents ; i++){
    if (i%5000 == 0&& i > 0 && debug > 0) std::cout << "Reading " <<  i << "/" << evts << " events"<< std::endl;
    if (debug > 2 && typeRO == ReadOut::Type::Hgcroc){
      std::cout << "************************************* NEW EVENT " << i << "  *********************************" << std::endl;
    }
    TdataIn->GetEntry(i);
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // reset calib object info 
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if (i == 0){
      runNr   = event.GetRunNumber();
      typeRO  = event.GetROtype();
      std::cout<< "original run numbers calib: "<<calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
      calib.SetRunNumber(runNr);
      calib.SetBeginRunTime(event.GetBeginRunTimeAlt());
      std::cout<< "reset run numbers calib: "<< calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
    }
   
    // initialize counters per event
    nCellsAboveTOA  = 0;
    nCellsAboveMADC = 0;
    totADCs         = 0.;
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // process tiles in event
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
    for(int j=0; j<event.GetNTiles(); j++){
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // histo filling for CAEN specific things
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++      
      if (typeRO == ReadOut::Type::Caen) {
        // nothing to be done
        return false;
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      // histo filling for HGCROC specific things & resetting of ped
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++        
      } else if (typeRO == ReadOut::Type::Hgcroc) {
        Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
        int cellID = aTile->GetCellID();
        ithSpectra=hSpectra.find(cellID);
        ithSpectraTrigg=hSpectraTrigg.find(cellID);
        
        // get pedestal values from calib object
        double ped    = calib.GetPedestalMeanL(cellID);
        double pedErr = calib.GetPedestalSigL(cellID);
        if (ped == -1000){
          ped     = calib.GetPedestalMeanH(cellID);
          pedErr  = calib.GetPedestalSigH(cellID);
          if (ped == -1000){
            ped     = aTile->GetPedestal();
            pedErr  = 5;
          }
        }
        // reevaluate waveform
        waveform_builder->set_waveform(aTile->GetADCWaveform());
        waveform_builder->fit_with_average_ped(ped);
        aTile->SetIntegratedADC(waveform_builder->get_E());
        aTile->SetPedestal(waveform_builder->get_pedestal());
        
        // obtain integrated tile quantities for histo filling
        double adc = aTile->GetIntegratedADC();
        double tot = aTile->GetTOT();
        double toa = aTile->GetTOA();
        totADCs         = totADCs+adc;
        // obtain samples in which TOA, TOT, maximum ADC are firing
        Int_t nSampTOA  = aTile->GetFirstTOASample();        
        Int_t nMaxADC   = aTile->GetMaxSampleADC();
        Int_t nADCFirst = aTile->GetFirstSampleAboveTh();
        Int_t nDiffFirstT= nSampTOA-nADCFirst;
        Int_t nDiffFirstM= nMaxADC-nADCFirst;
        Int_t nDiffMaxT  = nMaxADC-nSampTOA;
        if (adc > 10)
          nCellsAboveMADC++; 
        if (toa > 0)
          nCellsAboveTOA++;
                  
        if (toa > 0){
          hSampleTOA->Fill(nSampTOA);
          hSampleTOAVsCellID->Fill(cellID,nSampTOA);
        }
        if (adc > ped+2*pedErr){
          hSampleMaxADC->Fill(nMaxADC);
          hSampleMaxADCVsCellID->Fill(cellID,nMaxADC);
          hSampleAboveTh->Fill(nADCFirst);
        }
        
        if (toa > 0 && adc > ped+2*pedErr){
          hSampleDiff->Fill(nSampTOA-nMaxADC);
          hSampleDiffvsCellID->Fill(cellID,nSampTOA-nMaxADC);
          hSampleDiffMin->Fill(nSampTOA-nADCFirst);
        }
        
        hspectraTOAvsCellID->Fill(cellID,toa);
        hspectraTOTvsCellID->Fill(cellID,tot);
        
        int layer     = setup->GetLayer(cellID);
        int chInLayer = setup->GetChannelInLayerFull(cellID);
        // int offset    = nSampTOA-(nSampTOA-nADCFirst);
        int offset    = nADCFirst+nDiffFirstM;
        if (nDiffMaxT == 0 )
          offset--;
        // Detailed debug info with print of waveform
        if (debug > 3){
          aTile->PrintWaveFormDebugInfo(calib.GetPedestalMeanH(cellID), calib.GetPedestalMeanL(cellID), calib.GetPedestalSigL(cellID));
        }
        bool fillToACorr = false;
        if(fixedTOASample > -1 && fixedTOASample == nSampTOA)
          fillToACorr = true;
        else if(fixedTOASample == -1 )
          fillToACorr = true;
        // fill spectra histos
        if(ithSpectra!=hSpectra.end()){
          ithSpectra->second.FillExtHGCROC(adc,toa,tot,nSampTOA,fixedTOASample);
          if(fillToACorr) ithSpectra->second.FillWaveformVsTime(aTile->GetADCWaveform(), toa, calib.GetPedestalMeanH(cellID),offset);
        } else {
          RootOutputHist->cd("IndividualCells");
          hSpectra[cellID]=TileSpectra("full",3,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
          hSpectra[cellID].FillExtHGCROC(adc,toa,tot,nSampTOA, fixedTOASample);
          if(fillToACorr) hSpectra[cellID].FillWaveformVsTime(aTile->GetADCWaveform(), toa, calib.GetPedestalMeanH(cellID),offset);
          RootOutput->cd();
        }
        // fill spectra histos for signals with ToA
        if (toa > 0 ){      // needed for summing tests
        // if (toa > 0 && nADCFirst == 4 && nDiffFirstM ==1 && nDiffMaxT == 0){      // needed for summing tests
          // aTile->PrintWaveFormDebugInfo(calib.GetPedestalMeanH(cellID), calib.GetPedestalMeanL(cellID), calib.GetPedestalSigL(cellID));
          hHighestADCAbovePedVsLayer->Fill(layer,chInLayer, adc);
          if(ithSpectraTrigg!=hSpectraTrigg.end()){
            ithSpectraTrigg->second.FillExtHGCROC(adc,toa,tot,nSampTOA,fixedTOASample);
            if(fillToACorr) ithSpectraTrigg->second.FillWaveformVsTime(aTile->GetADCWaveform(), toa, calib.GetPedestalMeanH(cellID),offset);
          } else {
            RootOutputHist->cd("IndividualCellsTrigg");
            hSpectraTrigg[cellID]=TileSpectra("signal",3,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
            hSpectraTrigg[cellID].FillExtHGCROC(adc,toa,tot,nSampTOA,fixedTOASample);
            if(fillToACorr) hSpectraTrigg[cellID].FillWaveformVsTime(aTile->GetADCWaveform(), toa, calib.GetPedestalMeanH(cellID),offset);
            RootOutput->cd();
          }
        }
      }
    }
    RootOutput->cd();
    TdataOut->Fill();
  }
  //RootOutput->cd();
  TdataOut->Write();
  TsetupIn->CloneTree()->Write();

  //==================================================================================
  // Setup general plotting infos
  //==================================================================================    
  std::map<int,RunInfo>::iterator it=ri.find(runNr);
  TString outputDirPlots = GetPlotOutputDir();
  Double_t textSizeRel = 0.035;

  gSystem->Exec("mkdir -p "+outputDirPlots);
  StyleSettingsBasics("pdf");
  SetPlotStyle();  
  
  //==================================================================================
  // Create Summary plots
  //==================================================================================    
  TCanvas* canvas2DSigQA = new TCanvas("canvas2DSigQA","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas2DSigQA, 0.08, 0.13, 0.045, 0.07);

  PlotSimple2D( canvas2DSigQA, hSampleTOAVsCellID, (double)it->second.samples,setup->GetMaxCellID()+1, textSizeRel, Form("%s/SampleTOAvsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DSigQA, hSampleMaxADCVsCellID, (double)it->second.samples, setup->GetMaxCellID()+1, textSizeRel, Form("%s/SampleMaxADCvsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DSigQA, hSampleDiffvsCellID, -10000, setup->GetMaxCellID()+1, textSizeRel, Form("%s/SampleDiffvsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DSigQA, hspectraTOAvsCellID, -10000, setup->GetMaxCellID()+1, textSizeRel, Form("%s/TOAvsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DSigQA, hspectraTOTvsCellID, -10000, setup->GetMaxCellID()+1, textSizeRel, Form("%s/TOTvsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  
  canvas2DSigQA->SetLogz(1);
  PlotSimple2D( canvas2DSigQA, hHighestADCAbovePedVsLayer, -10000, -10000, textSizeRel, Form("%s/MaxADCAboveNoise_vsLayer.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);    
  
  TCanvas* canvas1DSimple = new TCanvas("canvas1DSimple","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas1DSimple, 0.08, 0.03, 0.03, 0.07);

  PlotSimple1D(canvas1DSimple, hSampleTOA, -10000, (double)it->second.samples, textSizeRel, Form("%s/NSampleToA.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1);
  PlotSimple1D(canvas1DSimple, hSampleMaxADC, -10000, (double)it->second.samples, textSizeRel, Form("%s/NSampleMaxADC.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1);
  PlotSimple1D(canvas1DSimple, hSampleAboveTh, -10000, (double)it->second.samples, textSizeRel, Form("%s/NSampleAboveTh.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1);
  PlotSimple1D(canvas1DSimple, hSampleDiff, -10000, (double)it->second.samples, textSizeRel, Form("%s/NSampleDiff.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1);
  PlotSimple1D(canvas1DSimple, hSampleDiffMin, -10000, (double)it->second.samples, textSizeRel, Form("%s/NSampleDiffMin.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1);

  // print general calib info
  calib.PrintGlobalInfo();  

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  // Find detector config for plotting
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  DetConf::Type detConf = setup->GetDetectorConfig();
  
  // set pixel text size
  Int_t textSizePixel = 30;

  TString toaSampleAdd  = "";
  TString outputDirPlotsMore = outputDirPlots;
  if (fixedTOASample != -1){
    ExtPlot             = 1;
    toaSampleAdd        = Form("/nTOA_%02d",fixedTOASample);
    outputDirPlotsMore  = Form("%s%s", outputDirPlots.Data(),toaSampleAdd.Data());
    gSystem->Exec("mkdir -p "+outputDirPlotsMore);
  }
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  //Single Module Plotting 
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
  if (detConf == DetConf::Type::Single8M || (detConf == DetConf::Type::Dual8M && ExtPlot > 2) ){
    TCanvas* canvas8Panel;
    TPad* pad8Panel[8];
    Double_t topRCornerX[8];
    Double_t topRCornerY[8];
    Double_t relSize8P[8];
    CreateCanvasAndPadsFor8PannelTBPlot(canvas8Panel, pad8Panel,  topRCornerX, topRCornerY, relSize8P, textSizePixel);

    TCanvas* canvas8PanelProf;
    TPad* pad8PanelProf[8];
    Double_t topRCornerXProf[8];
    Double_t topRCornerYProf[8];
    Double_t relSize8PProf[8];
    CreateCanvasAndPadsFor8PannelTBPlot(canvas8PanelProf, pad8PanelProf,  topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 0.045, "Prof", false);

    std::cout << "plotting single  8M layers" << std::endl;
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%5 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        if (!setup->IsLayerOn(l,m)){
          std::cout << "====> layer " << l << " in module " << m << " not enabled" << std::endl;
          continue;
        }
        PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                            textSizePixel, hSpectra, 1, -25000, (it->second.samples)*25000, 300, l, m,
                            Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),  m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                            textSizePixel, hSpectra, 2, 0, 1024, 300, l, m,
                            Form("%s/TOA_ADC_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(), m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                            textSizePixel, hSpectra, 3, 0, 1024, it->second.samples, l, m,
                            Form("%s/TOA_Sample_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                          textSizePixel, hSpectraTrigg, 1, -25000, (it->second.samples)*25000, 300, l, m,
                          Form("%s/WaveformSignal_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m, l, plotSuffix.Data()), it->second);            
        PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                            textSizePixel, hSpectraTrigg, 2, 0, 1024, 300, l, m,
                            Form("%s/TOA_ADC_Signal_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                            textSizePixel, hSpectraTrigg, 3, 0, 1024, it->second.samples, l, m,
                            Form("%s/TOA_Sample_Signal_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m,  l,plotSuffix.Data()), it->second);
        if (ExtPlot > 1){
          PlotSpectra8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                              hSpectra, 0, -100, 1024, 1.2, l, m,
                              Form("%s/Spectra_ADC_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          PlotSpectra8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                              hSpectra, 3, 0, 1024, 1.2, l, m,
                              Form("%s/TOA_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          PlotSpectra8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                              hSpectra, 4, 0, 4096, 1.2, l, m,
                              Form("%s/TOT_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
        }
      }
    }
    std::cout << "ending plotting single 8M layers" << std::endl;
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  //Single 2M horizontal Module Plotting 
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
  } else if (detConf == DetConf::Type::Single2MH){
    TCanvas* canvas2Panel;
    TPad* pad2Panel[2];
    Double_t topRCornerX[2];
    Double_t topRCornerY[2];
    Double_t relSizeP[2];
    CreateCanvasAndPadsFor2PannelTBPlot(canvas2Panel, pad2Panel,  topRCornerX, topRCornerY, relSizeP, textSizePixel);

    TCanvas* canvas2PanelProf;
    TPad* pad2PanelProf[2];
    Double_t topRCornerXProf[2];
    Double_t topRCornerYProf[2];
    Double_t relSizePProf[2];
    CreateCanvasAndPadsFor2PannelTBPlot(canvas2PanelProf, pad2PanelProf,  topRCornerXProf, topRCornerYProf, relSizePProf, textSizePixel, 0.075, "Prof", false);

    std::cout << "plotting single  2M layers" << std::endl;
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        PlotCorr2D2MLayer(canvas2PanelProf,pad2PanelProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                            textSizePixel, hSpectra, -25000, (it->second.samples)*25000, 1000, l, m,
                            Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D2MLayer(canvas2PanelProf,pad2PanelProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                          textSizePixel, hSpectraTrigg, -25000, (it->second.samples)*25000, 1000, l, 0,
                          Form("%s/WaveformSignal_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(), m, l, plotSuffix.Data()), it->second);            
        if (ExtPlot > 1){
          PlotSpectra2MLayer (canvas2Panel,pad2Panel, topRCornerX, topRCornerY, relSizeP, textSizePixel, 
                              hSpectra, 0, -100, 1024, 1.2, l, m,
                              Form("%s/Spectra_ADC_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
        }          
      }
    }   
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  //Single tile Plotting 
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++        
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  //Single Tile Plotting 
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
  } else if (detConf == DetConf::Type::SingleTile){
    TCanvas* canvasLayer = new TCanvas("canvasLayer","",0,0,620,600);
    DrawCanvasSettings( canvasLayer,0.12, 0.03, 0.03, 0.1);
    Double_t topRCornerX = 0.95;
    Double_t topRCornerY = 0.95;
    Double_t relSizeP = 30./620;
  
    TCanvas* canvasLayerProf = new TCanvas("canvasLayerProf","",0,0,620,600);
    DrawCanvasSettings( canvasLayerProf,0.138, 0.08, 0.03, 0.1);
    Double_t topRCornerXProf = 0.175;
    Double_t topRCornerYProf = 0.95;
    Double_t relSizePProf = 30./620;

    std::cout << "plotting single tile layers" << std::endl;
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
        PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                            textSizePixel, hSpectra, 1, -25000, (it->second.samples)*25000, 300, l, m,
                            Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                          textSizePixel, hSpectra, 2, 0, 1024, 300, l, 0,
                          Form("%s/TOA_ADC_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(), m, l, plotSuffix.Data()), it->second);            
        PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                          textSizePixel, hSpectra, 3,0, 1024, it->second.samples, l, 0,
                          Form("%s/TOA_Sample_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m, l, plotSuffix.Data()), it->second);            
        PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                          textSizePixel, hSpectraTrigg, 1, -25000, (it->second.samples)*25000, 300, l, 0,
                          Form("%s/WaveformSignal_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(), m, l, plotSuffix.Data()), it->second);            
        PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                          textSizePixel, hSpectraTrigg, 2, 0, 1024, 300, l, 0,
                          Form("%s/TOA_ADC_Signal_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(), m, l, plotSuffix.Data()), it->second);            
        PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                          textSizePixel, hSpectraTrigg, 3,0, 1024, it->second.samples, l, 0,
                          Form("%s/TOA_Sample_Signal_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m, l, plotSuffix.Data()), it->second);            
        if (ExtPlot > 1){
          PlotSpectra1MLayer (canvasLayer, topRCornerX, topRCornerY, relSizeP, textSizePixel, 
                              hSpectra, 0, -100, 1024, 1.2, l, m,
                              Form("%s/Spectra_ADC_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          PlotSpectra1MLayer (canvasLayer, topRCornerX, topRCornerY, relSizeP, textSizePixel, 
                              hSpectra, 3, 0, 1024, 1.2, l, m,
                              Form("%s/TOA_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          PlotSpectra1MLayer (canvasLayer, topRCornerX, topRCornerY, relSizeP, textSizePixel, 
                              hSpectra, 4, 0, 4096, 1.2, l, m,
                              Form("%s/TOT_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
        }          
      }
    }
  }
  
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  //Dual module plotting 
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
  if (detConf == DetConf::Type::Dual8M){
    TCanvas* canvas2ModPanel;
    TPad* pad2ModPanel[16];
    Double_t topRCornerX2Mod[16];
    Double_t topRCornerY2Mod[16];
    Double_t relSize2ModP[16];
    CreateCanvasAndPadsForDualModTBPlot(canvas2ModPanel, pad2ModPanel,  topRCornerX2Mod, topRCornerY2Mod, relSize2ModP, textSizePixel);

    TCanvas* canvas2ModPanelProf;
    TPad* pad2ModPanelProf[16];
    Double_t topRCornerX2ModProf[16];
    Double_t topRCornerY2ModProf[16];
    Double_t relSize2ModProf[16];
    CreateCanvasAndPadsForDualModTBPlot(canvas2ModPanelProf, pad2ModPanelProf,  topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf, textSizePixel, 0.045, "Prof", true);
    
    std::cout << "plotting dual module layers" << std::endl;
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      if (l%10 == 0 && l > 0 && debug > 0)
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }
      if (!calib.IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectra, 1, -25000, (it->second.samples)*25000, 300, l, 
                          Form("%s/Waveform_Layer%02d.%s" ,outputDirPlotsMore.Data(), l, plotSuffix.Data()), it->second);
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectra, 2, 0, 1024, 300, l, 
                          Form("%s/TOA_ADC_Layer%02d.%s" ,outputDirPlotsMore.Data(),l, plotSuffix.Data()), it->second);
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectra, 3, 0, 1024, it->second.samples, l,
                          Form("%s/TOA_Sample_Layer%02d.%s" ,outputDirPlotsMore.Data(), l, plotSuffix.Data()), it->second);
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectraTrigg, 1, -25000, (it->second.samples)*25000, 300, l, 
                          Form("%s/WaveformSignal_Layer%02d.%s" ,outputDirPlotsMore.Data(),l, plotSuffix.Data()), it->second);            
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectraTrigg, 2, 0, 1024, 300, l,
                          Form("%s/TOA_ADC_Signal_Layer%02d.%s" ,outputDirPlotsMore.Data(), l, plotSuffix.Data()), it->second);
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectraTrigg, 3, 0, 1024, it->second.samples, l,                        
                          Form("%s/TOA_Sample_Signal_Layer%02d.%s" ,outputDirPlotsMore.Data(), l, plotSuffix.Data()), it->second);
      if (ExtPlot > 1){
        PlotSpectra2ModLayer (canvas2ModPanel,pad2ModPanel, topRCornerX2Mod, topRCornerY2Mod, relSize2ModP, textSizePixel, 
                              hSpectra, 0, -100, 1024, 1.2, l,
                              Form("%s/Spectra_ADC_Layer%02d.%s" ,outputDirPlots.Data(), l, plotSuffix.Data()), it->second);
        PlotSpectra2ModLayer (canvas2ModPanel,pad2ModPanel, topRCornerX2Mod, topRCornerY2Mod, relSize2ModP, textSizePixel, 
                              hSpectra, 3, 0, 1024, 1.2, l,                             
                              Form("%s/TOA_Layer%02d.%s" ,outputDirPlots.Data(), l, plotSuffix.Data()), it->second);
        PlotSpectra2ModLayer (canvas2ModPanel,pad2ModPanel, topRCornerX2Mod, topRCornerY2Mod, relSize2ModP, textSizePixel, 
                              hSpectra, 4, 0, 4096, 1.2, l, 
                              Form("%s/TOT_Layer%02d.%s" ,outputDirPlots.Data(), l, plotSuffix.Data()), it->second);
      }
    }
    std::cout << "ending plotting dual module layers" << std::endl;
    
  }
  
  RootOutput->cd();
  
  std::cout<<"What is the value? <ped mean high>: "<<calib.GetAveragePedestalMeanHigh() << "\t good channels: " << calib.GetNumberOfChannelsWithBCflag(3) <<std::endl;
  
  TcalibOut->Fill();
  TcalibOut->Write();
  
  RootOutput->Close();
  RootOutputHist->cd();
  if (typeRO == ReadOut::Type::Hgcroc){
    hHighestADCAbovePedVsLayer->Write();
    hSampleTOA->Write();
    hSampleMaxADC->Write();
    hSampleTOAVsCellID->Write();
    hSampleMaxADCVsCellID->Write();
    hspectraTOAvsCellID->Write();
    hspectraTOTvsCellID->Write();
  }
  RootOutputHist->cd("IndividualCells");
  for(ithSpectra=hSpectra.begin(); ithSpectra!=hSpectra.end(); ++ithSpectra){
    ithSpectra->second.WriteExt(true);
  }
  RootOutputHist->cd("IndividualCellsTrigg");
  if (typeRO == ReadOut::Type::Hgcroc){
    for(ithSpectraTrigg=hSpectraTrigg.begin(); ithSpectraTrigg!=hSpectraTrigg.end(); ++ithSpectraTrigg){
      ithSpectraTrigg->second.WriteExt(true);
    }
  }

  
  RootInput->Close();
  return true;
}


//***********************************************************************************************
//*********************** Create output files ***************************************************
//***********************************************************************************************
bool HGCROC_Waveform_Analysis::CreateOutputRootFile(void){
  if(Overwrite){
    std::cout << "overwriting exisiting output file" << std::endl;
    RootOutput=new TFile(RootOutputName.Data(),"RECREATE");
  } else{
    std::cout << "creating output file" << std::endl;
    RootOutput = new TFile(RootOutputName.Data(),"CREATE");
  }
  if(RootOutput->IsZombie()){
    std::cout<<"Error opening '"<<RootOutput<<"'no reachable path? Exist without force mode to overwrite?..."<<std::endl;
    return false;
  }
  return true;
}
