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
    
    if (!IsAnalyseWaveForm && !IsExtractTimeWalk && !IsInvCrossTalk ){
      bool sCOF = CreateOutputRootFile();
      if (!sCOF) return false;
      
      TsetupOut = new TTree("Setup","Setup");
      
      //TsetupOut->Branch("setup",&setup);
      TsetupOut->Branch("setup",&rsw);
      TdataOut = new TTree("Data","Data");
      TdataOut->Branch("event",&event);
      TcalibOut = new TTree("Calib","Calib");
      TcalibOut->Branch("calib",&calib);
    }
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

  if (IsExtractTimeWalk){
    status=ExtractTimeWalk();
  }

  if (IsInvCrossTalk){
    status=InvestigateCrossTalk();
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
  if (ExternalToACalibOffSetFile.CompareTo("") != 0 ){
    calib.ReadExternalToAOffsets(ExternalToACalibOffSetFile,debug);
  }

  // reading first event in tree to extract runnumber & RO-type
  TdataIn->GetEntry(0);
  Int_t runNr           = event.GetRunNumber();
  ReadOut::Type typeRO  = event.GetROtype();
  std::cout<< "original run numbers calib: "<<calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
  calib.SetRunNumber(runNr);
  calib.SetBeginRunTime(event.GetBeginRunTimeAlt());
  std::cout<< "reset run numbers calib: "<< calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
  
  if (typeRO != ReadOut::Type::Hgcroc){
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "This routine is not meant to run on CAEN data! Please check you are loading the correct inputs! \n ABORTING!!!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    return false; 
  }

  //==================================================================================
  // create additional output hist
  //==================================================================================
  CreateOutputRootHistFile();
  
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

  TH2D* hTOAPsVsCellID       = new TH2D( "hTOAPsVsCellID","ToA (ns); cell ID; ToA (ps)",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 500,-250,0);
  hTOAPsVsCellID->SetDirectory(0);
  TH2D* hTOACorrNsVsCellID       = new TH2D( "hTOACorrNsVsCellID","ToA (ns); cell ID; ToA (ps)",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 500,-250,0);
  hTOACorrNsVsCellID->SetDirectory(0);

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
  
  TH2D* h2DToAvsADC[setup->GetNMaxROUnit()+1][2];
  TProfile* hToAvsADC[setup->GetNMaxROUnit()+1][2];
  TH2D* h2DWaveFormHalfAsic[setup->GetNMaxROUnit()+1][2];
  TProfile* hWaveFormHalfAsic[setup->GetNMaxROUnit()+1][2];
  for (Int_t ro = 0; ro < setup->GetNMaxROUnit()+1; ro++){
    for (int h = 0; h< 2; h++){
      hToAvsADC[ro][h]    = new TProfile(Form("ADCTOA_Asic_%d_Half_%d",ro,h),
                                         Form("ADC-TOA correlation %d %d; TOA (arb. units); ADC (arb. units)",ro,h),1024/4,0,1024, "");
      h2DToAvsADC[ro][h]  = new TH2D(Form("h2DADCTOA_Asic_%d_Half_%d",ro,h),
                                     Form("2D ADC vs TOA %d %d; TOA (arb. units); ADC (arb. units)",ro,h), 1024/4,0,1024,1124,-100,1024);
      
      hWaveFormHalfAsic[ro][h]    = new TProfile(Form("WaveForm_Asic_%d_Half_%d",ro,h),
                                         Form("ADC-TOA correlation %d %d;  t (ns) ; ADC (arb. units)",ro,h),2200,-50,500);
      h2DWaveFormHalfAsic[ro][h]  = new TH2D(Form("h2DWaveForm_Asic_%d_Half_%d",ro,h),
                                        Form("2D ADC vs TOA %d %d;  t (ns) ; ADC (arb. units)",ro,h), 
                                        2200,-50,500, 1034, -10, 1024);
      
    }
  }
  
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Migrad");  
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  
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
    if (debug > 2 ){
      std::cout << "************************************* NEW EVENT " << i << "  *********************************" << std::endl;
    }
    TdataIn->GetEntry(i);
   
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
        double adc      = aTile->GetIntegratedADC();
        double tot      = aTile->GetRawTOT();
        double toa      = aTile->GetRawTOA();
        bool hasTOA     = false;
        bool hasTOT     = false;
        
        // check if we have a toa
        if (toa > 0)
          hasTOA        = true;
        // check if we have a tot
        if (tot > 0)
          hasTOT        = true;
        Int_t nSampTOA  = aTile->GetFirstTOASample();        
        double toaNs      = (double)aTile->GetLinearizedRawTOA()*aTile->GetTOATimeResolution();
        double toaCorrNs  = toaNs;
        if (calib.GetToAOff(cellID) != -1000.){
          // correct ToA sample and return correct nSampleTOA
          nSampTOA      = aTile->SetCorrectedTOA(calib.GetToAOff(cellID)); 
          toaCorrNs     = (double)(aTile->GetCorrectedTOA())*aTile->GetTOATimeResolution();;
        }
        totADCs         = totADCs+adc;
        // obtain samples in which TOA, TOT, maximum ADC are firing
        
        Int_t nMaxADC   = aTile->GetMaxSampleADC();
        Int_t nADCFirst = aTile->GetFirstSampleAboveTh();
        Int_t nDiffFirstT= nSampTOA-nADCFirst;
        Int_t nDiffFirstM= nMaxADC-nADCFirst;
        Int_t nDiffMaxT  = nMaxADC-nSampTOA;
        Int_t nOffEmpty  = 2;
        
        if (adc > 10)
          nCellsAboveMADC++; 
        
        if (hasTOA){
          nCellsAboveTOA++;
          hSampleTOA->Fill(nSampTOA);
          hSampleTOAVsCellID->Fill(cellID,nSampTOA);
        }
        if (adc > ped+2*pedErr){
          hSampleMaxADC->Fill(nMaxADC);
          hSampleMaxADCVsCellID->Fill(cellID,nMaxADC);
          hSampleAboveTh->Fill(nADCFirst);
        }
        
        if (hasTOA && adc > ped+2*pedErr){
          hSampleDiff->Fill(nSampTOA-nMaxADC);
          hSampleDiffvsCellID->Fill(cellID,nSampTOA-nMaxADC);
          hSampleDiffMin->Fill(nSampTOA-nADCFirst);
        }
        
        if(hasTOA)  hspectraTOAvsCellID->Fill(cellID,toa);
        if(hasTOT)  hspectraTOTvsCellID->Fill(cellID,tot);

        Int_t asic      = setup->GetROunit(cellID);
        Int_t roCh      = setup->GetROchannel(cellID);
        bool fillToACorr = false;
        if(fixedTOASample > -1 && fixedTOASample == nSampTOA)
          fillToACorr = true;
        else if(fixedTOASample == -1 )
          fillToACorr = true;
        
        if (hasTOA){
          hTOAPsVsCellID->Fill(cellID,toaNs);
          hTOACorrNsVsCellID->Fill(cellID,toaCorrNs);
          // fill overview hists for half asics
          if( fillToACorr ){   
            hToAvsADC[asic][int(roCh/36)]->Fill(toa, adc);
            h2DToAvsADC[asic][int(roCh/36)]->Fill(toa, adc);
            for (int k = 0; k < (int)(aTile->GetADCWaveform()).size(); k++ ){
              double timetemp = ((k+nOffEmpty)*1024 + aTile->GetCorrectedTOA())*aTile->GetTOATimeResolution() ;
              hWaveFormHalfAsic[asic][int(roCh/36)]->Fill(timetemp,(aTile->GetADCWaveform()).at(k)-ped);
              h2DWaveFormHalfAsic[asic][int(roCh/36)]->Fill(timetemp,(aTile->GetADCWaveform()).at(k)-ped);
            }
          }
        }
        
        int layer     = setup->GetLayer(cellID);
        int chInLayer = setup->GetChannelInLayerFull(cellID);
        // Detailed debug info with print of waveform
        if (debug > 3){
          aTile->PrintWaveFormDebugInfo(calib.GetPedestalMeanH(cellID), calib.GetPedestalMeanL(cellID), calib.GetPedestalSigL(cellID));
        }
      
        // fill spectra histos
        if(ithSpectra!=hSpectra.end()){
          ithSpectra->second.FillExtHGCROC(adc,toa,tot,nSampTOA,fixedTOASample);
          if(fillToACorr) ithSpectra->second.FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), calib.GetPedestalMeanH(cellID),nOffEmpty);
        } else {
          RootOutputHist->cd("IndividualCells");
          hSpectra[cellID]=TileSpectra("full",3,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
          hSpectra[cellID].FillExtHGCROC(adc,toa,tot,nSampTOA, fixedTOASample);
          if(fillToACorr) hSpectra[cellID].FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), calib.GetPedestalMeanH(cellID),nOffEmpty);
        }
        // fill spectra histos for signals with ToA
        if (hasTOA){      // needed for summing tests
          hHighestADCAbovePedVsLayer->Fill(layer,chInLayer, adc);
          if(ithSpectraTrigg!=hSpectraTrigg.end()){
            ithSpectraTrigg->second.FillExtHGCROC(adc,toa,tot,nSampTOA,fixedTOASample);
            if(fillToACorr) ithSpectraTrigg->second.FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), calib.GetPedestalMeanH(cellID),nOffEmpty);
          } else {
            RootOutputHist->cd("IndividualCellsTrigg");
            hSpectraTrigg[cellID]=TileSpectra("signal",3,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
            hSpectraTrigg[cellID].FillExtHGCROC(adc,toa,tot,nSampTOA,fixedTOASample);
            if(fillToACorr) hSpectraTrigg[cellID].FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), calib.GetPedestalMeanH(cellID),nOffEmpty);
          }
        }
      }
    }
  }

  //==================================================================================
  // Setup general plotting infos
  //==================================================================================    
  std::map<int,RunInfo>::iterator it=ri.find(runNr);
  TString outputDirPlots = GetPlotOutputDir();
  Double_t textSizeRel = 0.035;

  gSystem->Exec("mkdir -p "+outputDirPlots);
  TString toaSampleAdd  = "";
  TString outputDirPlotsMore = outputDirPlots;
  if (fixedTOASample != -1){
    ExtPlot             = 1;
    toaSampleAdd        = Form("/nTOA_%02d",fixedTOASample);
    outputDirPlotsMore  = Form("%s%s", outputDirPlots.Data(),toaSampleAdd.Data());
    gSystem->Exec("mkdir -p "+outputDirPlotsMore);
  }

  
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

  PlotSimple2D( canvas2DSigQA, hTOAPsVsCellID, -10000, setup->GetMaxCellID()+1, textSizeRel, Form("%s/TOAPsvsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DSigQA, hTOACorrNsVsCellID, -10000, setup->GetMaxCellID()+1, textSizeRel, Form("%s/TOACorrPsvsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  
  canvas2DSigQA->SetLogz(1);
  PlotSimple2D( canvas2DSigQA, hHighestADCAbovePedVsLayer, -10000, -10000, textSizeRel, Form("%s/MaxADCAboveNoise_vsLayer.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);    
  
  Int_t diffBins = 4;
  for (Int_t ro = 0; ro < setup->GetNMaxROUnit()+1; ro++){
    for (int h = 0; h< 2; h++){
      Double_t largestDiff = 0.;
      Int_t bin         = 1;
      for (Int_t b = 1; b < hToAvsADC[ro][h]->GetNbinsX(); b++){
        Int_t maxBin  = (b+diffBins);
        if (maxBin > hToAvsADC[ro][h]->GetNbinsX()){
          maxBin      = maxBin-hToAvsADC[ro][h]->GetNbinsX();
        }
        Double_t diff = hToAvsADC[ro][h]->GetBinContent(maxBin) - hToAvsADC[ro][h]->GetBinContent(b);
        if (largestDiff < diff){
          largestDiff = diff;
          bin         = b;
        }
      }
      Int_t centerbin = bin+Int_t(diffBins/2);
      if (centerbin > (hToAvsADC[ro][h]->GetNbinsX()-diffBins/2))
        centerbin     = centerbin-(hToAvsADC[ro][h]->GetNbinsX()-diffBins/2);
      
      Int_t offset = (Int_t)(hToAvsADC[ro][h]->GetBinCenter(centerbin));
      std::cout << ro <<"\t" <<h << "\t" << offset << std::endl;
      
      Plot2DWithProfile( canvas2DSigQA, h2DToAvsADC[ro][h], hToAvsADC[ro][h], -10000, -10000, textSizeRel,
                        Form("%s/ToAvsADC_Asic_%d_Half_%d.%s", outputDirPlotsMore.Data(), ro, h, plotSuffix.Data()), 
                        it->second, 1, kFALSE, "colz",true, Form("Asic %d, Half %d",ro,h), offset);
      
      h2DWaveFormHalfAsic[ro][h]->GetXaxis()->SetRangeUser(-25, (it->second.samples)*25);
      Plot2DWithProfile( canvas2DSigQA, h2DWaveFormHalfAsic[ro][h], hWaveFormHalfAsic[ro][h], -10000, -10000, textSizeRel,
                        Form("%s/Waveform_Asic_%d_Half_%d.%s", outputDirPlotsMore.Data(), ro, h, plotSuffix.Data()), 
                        it->second, 1, kFALSE, "colz",true, Form("Asic %d, Half %d",ro,h), -1);
    }
  }
  
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
                            textSizePixel, hSpectra, 1, -25, (it->second.samples)*25, 300, l, m,
                            Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),  m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                            textSizePixel, hSpectra, 2, 0, 1024, 300, l, m,
                            Form("%s/TOA_ADC_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(), m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                            textSizePixel, hSpectra, 3, 0, 1024, it->second.samples, l, m,
                            Form("%s/TOA_Sample_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                          textSizePixel, hSpectraTrigg, 1, -25, (it->second.samples)*25, 300, l, m,
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
                            textSizePixel, hSpectra, -25, (it->second.samples)*25, 1000, l, m,
                            Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D2MLayer(canvas2PanelProf,pad2PanelProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                          textSizePixel, hSpectraTrigg, -25, (it->second.samples)*25, 1000, l, 0,
                          Form("%s/WaveformSignal_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(), m, l, plotSuffix.Data()), it->second);            
        if (ExtPlot > 1){
          PlotSpectra2MLayer (canvas2Panel,pad2Panel, topRCornerX, topRCornerY, relSizeP, textSizePixel, 
                              hSpectra, 0, -100, 1024, 1.2, l, m,
                              Form("%s/Spectra_ADC_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
        }          
      }
    }   
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
                            textSizePixel, hSpectra, 1, -25, (it->second.samples)*25, 300, l, m,
                            Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m,  l, plotSuffix.Data()), it->second);
        PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                          textSizePixel, hSpectra, 2, 0, 1024, 300, l, 0,
                          Form("%s/TOA_ADC_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(), m, l, plotSuffix.Data()), it->second);            
        PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                          textSizePixel, hSpectra, 3,0, 1024, it->second.samples, l, 0,
                          Form("%s/TOA_Sample_Mod_%02d_Layer%02d.%s" ,outputDirPlotsMore.Data(),m, l, plotSuffix.Data()), it->second);            
        PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                          textSizePixel, hSpectraTrigg, 1, -25, (it->second.samples)*25, 300, l, 0,
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
  if (detConf == DetConf::Type::Dual8M ){
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
      if (l%4 != 1 ){
        std::cout << "====> layer " << l << " skipping" << std::endl;
        continue;
      }
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }
      if (!calib.IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectra, 1, -50, (it->second.samples)*25, -80, 300, l, 
                          Form("%s/Waveform_Layer%02d.%s" ,outputDirPlotsMore.Data(), l, plotSuffix.Data()), it->second);
      // PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
      //                     textSizePixel, hSpectra, 2, 0, 1024, 0, 300, l, 
      //                     Form("%s/TOA_ADC_Layer%02d.%s" ,outputDirPlotsMore.Data(),l, plotSuffix.Data()), it->second);
      // PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
      //                     textSizePixel, hSpectra, 3, 0, 1024, 0, it->second.samples, l,
      //                     Form("%s/TOA_Sample_Layer%02d.%s" ,outputDirPlotsMore.Data(), l, plotSuffix.Data()), it->second);
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectraTrigg, 1, -50, (it->second.samples)*25, -80, 300, l, 
                          Form("%s/WaveformSignal_Layer%02d.%s" ,outputDirPlotsMore.Data(),l, plotSuffix.Data()), it->second);            
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectraTrigg, 2, 0, 1024, 0, 300, l,
                          Form("%s/TOA_ADC_Signal_Layer%02d.%s" ,outputDirPlotsMore.Data(), l, plotSuffix.Data()), it->second);
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectraTrigg, 3, 0, 1024, 0, it->second.samples, l,                        
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
  
  std::cout << "Writing output hist file" << std::endl;
  RootOutputHist->cd();
  hHighestADCAbovePedVsLayer->Write();
  hSampleTOA->Write();
  hSampleMaxADC->Write();
  hSampleTOAVsCellID->Write();
  hSampleMaxADCVsCellID->Write();
  hspectraTOAvsCellID->Write();
  hspectraTOTvsCellID->Write();

  for (Int_t ro = 0; ro < setup->GetNMaxROUnit()+1; ro++){
    for (int h = 0; h< 2; h++){
      hToAvsADC[ro][h]->Write();
      h2DToAvsADC[ro][h]->Write();
    }
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

// ****************************************************************************
// Extract Time Walk
// ****************************************************************************
bool HGCROC_Waveform_Analysis::ExtractTimeWalk(void){
  std::cout<<"Analyse Waveform"<<std::endl;
  int evts=TdataIn->GetEntries();

  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  
  std::map<int,TileSpectra> hSpectra;
  std::map<int, TileSpectra>::iterator ithSpectra;
  std::map<int,TileSpectra> hSpectraAlt;
  std::map<int, TileSpectra>::iterator ithSpectraAlt;
  // set calib entry pointer
  TcalibIn->GetEntry(0);

  // obtain runnumber & RO-type
  TdataIn->GetEntry(0);
  Int_t runNr          = event.GetRunNumber();
  ReadOut::Type typeRO = event.GetROtype();
  std::map<int,RunInfo>::iterator it=ri.find(runNr);
  
  //==================================================================================
  // create additional output hist
  //==================================================================================
  CreateOutputRootHistFile();

  TH2D* hTOACorrNsVsCellID        = new TH2D( "hTOACorrNsVsCellID","ToA (ns); cell ID; ToA (ns)",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 500,-250,0);
  hTOACorrNsVsCellID->SetDirectory(0);
  TH2D* hTOACorrVsCellID          = new TH2D( "hTOACorrVsCellID"," ; cell ID; linearized ToA",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 1280,-10*1024,0);
  hTOACorrVsCellID->SetDirectory(0);

  TH1D* hSampleTOA               = new TH1D( "hSampleTOA","#sample ToA; #sample TOA",
                                              it->second.samples,0,it->second.samples);
  TH1D* hSampleMaxADC            = new TH1D( "hSampleMaxADC","#sample ToA; #sample maxADC",
                                              it->second.samples,0,it->second.samples);
  TH1D* hSampleAboveTh           = new TH1D( "hSampleAboveTh","#sample ToA; #sample above th",
                                              it->second.samples,0,it->second.samples);
  
  RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->mkdir("IndividualCellsAlt");
  RootOutputHist->cd("IndividualCells");
  
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Migrad");  
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  
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
    // process tiles in event
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
    for(int j=0; j<event.GetNTiles(); j++){
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      // histo filling for HGCROC specific things & resetting of ped
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++        
      Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
      int cellID = aTile->GetCellID();
      ithSpectra=hSpectra.find(cellID);
      ithSpectraAlt=hSpectraAlt.find(cellID);
      
      // get pedestal values from calib object
      double ped    = aTile->GetPedestal();
      
      // obtain integrated tile quantities for histo filling
      double adc      = aTile->GetIntegratedADC();
      double toaRaw   = aTile->GetRawTOA();
      double toa      = (double)(aTile->GetCorrectedTOA());
      Int_t nSampTOA  = aTile->GetCorrectedFirstTOASample(calib.GetToAOff(cellID));
      bool hasTOA     = false;
      
      // check if we have a toa
      if (toaRaw > 0)
        hasTOA        = true;

      double toaNs    = toa*aTile->GetTOATimeResolution();;
      
      Int_t nMaxADC   = aTile->GetMaxSampleADC();
      Int_t nADCFirst = aTile->GetFirstSampleAboveTh();
      Int_t nOffEmpty  = 2;
      
      hSampleMaxADC->Fill(nMaxADC);
      hSampleAboveTh->Fill(nADCFirst);
      
      if (hasTOA){
        hSampleTOA->Fill(nSampTOA);
        hTOACorrNsVsCellID->Fill(cellID,toaNs);
        hTOACorrVsCellID->Fill(cellID,toa);
      }
              
      int layer     = setup->GetLayer(cellID);
      int chInLayer = setup->GetChannelInLayerFull(cellID);
      // Detailed debug info with print of waveform
      if (debug > 3){
        aTile->PrintWaveFormDebugInfo(calib.GetPedestalMeanH(cellID), calib.GetPedestalMeanL(cellID), calib.GetPedestalSigL(cellID));
      }
    
      // fill spectra histos
      if(ithSpectra!=hSpectra.end()){
        ithSpectra->second.FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), ped, nOffEmpty);
      } else {
        RootOutputHist->cd("IndividualCells");
        hSpectra[cellID]=TileSpectra("full",6,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
        hSpectra[cellID].FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), ped, nOffEmpty);
      }
      // fill spectra histos for signals with ToA
      if (hasTOA){      // needed for summing tests
        Double_t tempADC    = 0.;
        Int_t nSampleTOARaw = aTile->GetFirstTOASample(); 
        if (nMaxADC > 0 && nMaxADC < (int)it->second.samples){
          tempADC = (aTile->GetADCWaveform()).at(nMaxADC);
        }
        if(ithSpectraAlt!=hSpectraAlt.end()){
          ithSpectraAlt->second.FillExtHGCROC(tempADC,toa,-1,nSampTOA,-1);
          ithSpectraAlt->second.FillMaxVsTime(tempADC, aTile->GetCorrectedTOA(),nOffEmpty, nMaxADC);
        } else {
          RootOutputHist->cd("IndividualCellsAlt");
          hSpectraAlt[cellID]=TileSpectra("maxADC",5,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
          hSpectraAlt[cellID].FillExtHGCROC(tempADC,toa,-1,nSampTOA,-1);
          hSpectraAlt[cellID].FillMaxVsTime(tempADC, aTile->GetCorrectedTOA(), nOffEmpty, nMaxADC);
        }
      }
    }
  }

  //==================================================================================
  // Setup general plotting infos
  //==================================================================================    
  
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

  PlotSimple2D( canvas2DSigQA, hTOACorrVsCellID, -10000, setup->GetMaxCellID()+1, textSizeRel, Form("%s/TOACorrvsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DSigQA, hTOACorrNsVsCellID, -10000, setup->GetMaxCellID()+1, textSizeRel, Form("%s/TOACorrNsvsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  
  TCanvas* canvas1DSimple = new TCanvas("canvas1DSimple","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas1DSimple, 0.08, 0.03, 0.03, 0.07);

  PlotSimple1D(canvas1DSimple, hSampleTOA, -10000, (double)it->second.samples, textSizeRel, Form("%s/NSampleToA.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1);
  PlotSimple1D(canvas1DSimple, hSampleMaxADC, -10000, (double)it->second.samples, textSizeRel, Form("%s/NSampleMaxADC.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1);
  PlotSimple1D(canvas1DSimple, hSampleAboveTh, -10000, (double)it->second.samples, textSizeRel, Form("%s/NSampleAboveTh.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1);

  // print general calib info
  calib.PrintGlobalInfo();  

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  // Find detector config for plotting
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  DetConf::Type detConf = setup->GetDetectorConfig();
  
  // set pixel text size
  Int_t textSizePixel = 30;
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  //Dual module plotting 
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
  if (detConf == DetConf::Type::Dual8M ){
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
      // if (l%4 != 1 ){
        // std::cout << "====> layer " << l << " skipping" << std::endl;
        // continue;
      // }
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }
      if (!calib.IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectra, 1, -50, (it->second.samples)*25, -80, 800, l, 
                          Form("%s/Waveform_Layer%02d.%s" ,outputDirPlots.Data(), l, plotSuffix.Data()), it->second);
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectraAlt, 1, -50, (it->second.samples)*25, -80, 800, l, 
                          Form("%s/WaveformMaxAdcOnly_Layer%02d.%s" ,outputDirPlots.Data(),l, plotSuffix.Data()), it->second);            
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectraAlt, 2, -6*1024, -2*1024, 0, 1000, l,
                          Form("%s/TOA_ADC_MaxAdcOnly_Layer%02d.%s" ,outputDirPlots.Data(), l, plotSuffix.Data()), it->second);
    }
    std::cout << "ending plotting dual module layers" << std::endl;    
  }
  
  RootOutputHist->cd();
  if (typeRO == ReadOut::Type::Hgcroc){
    hSampleTOA->Write();
    hSampleMaxADC->Write();
    hSampleAboveTh->Write();
    hTOACorrVsCellID->Write();
    hTOACorrNsVsCellID->Write();
  }

  RootOutputHist->cd("IndividualCells");
  for(ithSpectra=hSpectra.begin(); ithSpectra!=hSpectra.end(); ++ithSpectra){
    ithSpectra->second.WriteExt(true);
  }
  RootOutputHist->cd("IndividualCellsAlt");
  for(ithSpectraAlt=hSpectraAlt.begin(); ithSpectraAlt!=hSpectraAlt.end(); ++ithSpectraAlt){
    ithSpectraAlt->second.WriteExt(true);
  }

  RootInput->Close();
  return true;
}


// ****************************************************************************
// Investigate Cross Talk
// ****************************************************************************
bool HGCROC_Waveform_Analysis::InvestigateCrossTalk(void){
  std::cout<<"Analyse Waveform"<<std::endl;
  int evts=TdataIn->GetEntries();

  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  
  std::map<int,TileSpectra> hSpectraXtalk;
  std::map<int, TileSpectra>::iterator ithSpectraXtalk;
  std::map<int,TileSpectra> hSampleXtalk;
  std::map<int, TileSpectra>::iterator ithSampleXtalk;
  std::map<int,TileSpectra> hSpectraSat;
  std::map<int, TileSpectra>::iterator ithSpectraSat;
  std::map<int,TileSpectra> hSampleSat;
  std::map<int, TileSpectra>::iterator ithSampleSat;
  // set calib entry pointer
  TcalibIn->GetEntry(0);

  // obtain runnumber & RO-type
  TdataIn->GetEntry(0);
  Int_t runNr          = event.GetRunNumber();
  ReadOut::Type typeRO = event.GetROtype();
  std::map<int,RunInfo>::iterator it=ri.find(runNr);

  bool isSingleCh     = false;
  if (GetFixedROChannel() != -1) isSingleCh = true;
  
  int maxChannelPerLayer             = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1)*(setup->GetNMaxModule()+1);
  int maxChannelPerLayerSingleMod    = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1);

  
  //==================================================================================
  // create additional output hist
  //==================================================================================
  CreateOutputRootHistFile();

  TH1D* hNEvts                    = new TH1D("hNEvts","Events counter; ", 3, 0, 3);
  hNEvts->SetDirectory(0);

  TH2D* hSatADCVsLayer   = new TH2D( "hSatADCVsLayer","Saturated ADC; layer; brd channel; saturated/event ",
                                      setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hSatADCVsLayer->SetDirectory(0);
  hSatADCVsLayer->Sumw2();
  TH2D* hNegCellVsLayer   = new TH2D( "hNegCellVsLayer","Negative ADC; layer; brd channel; saturated/event ",
                                      setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hNegCellVsLayer->SetDirectory(0);
  hNegCellVsLayer->Sumw2();

  TH2D* h2DNSatCellsVsAsicHalfs = new TH2D( "h2DNSatCellsVsAsicHalfs","# Saturated per halfs; halfs; nCells sat. ",
                                            (setup->GetNMaxROUnit()+1)*2, 0, (setup->GetNMaxROUnit()+1)*2,
                                            36, -0.5, 36-0.5);
  h2DNSatCellsVsAsicHalfs->SetDirectory(0);
  h2DNSatCellsVsAsicHalfs->Sumw2();
  TH2D* h2DNNegCellsVsAsicHalfs = new TH2D( "h2DNNegCellsVsAsicHalfs","# Negative ADC per halfs; halfs; nCells w/ neg ADC ",
                                            (setup->GetNMaxROUnit()+1)*2, 0, (setup->GetNMaxROUnit()+1)*2,
                                            36, -0.5, 36-0.5);
  h2DNNegCellsVsAsicHalfs->SetDirectory(0);
  h2DNNegCellsVsAsicHalfs->Sumw2();
  TH2D* h2DNNegCellsVsAsicHalfsWT = new TH2D( "h2DNNegCellsVsAsicHalfsWT","# Negative ADC per halfs w/ ToA; halfs; nCells w/ neg ADC, w/ ToA ",
                                            (setup->GetNMaxROUnit()+1)*2, 0, (setup->GetNMaxROUnit()+1)*2,
                                            36, -0.5, 36-0.5);
  h2DNNegCellsVsAsicHalfsWT->SetDirectory(0);
  h2DNNegCellsVsAsicHalfsWT->Sumw2();
  
  TH2D* h2DNSatCellsVsNegCells = new TH2D( "h2DNSatCellsVsNegCells","# Saturated vs #Neg ADC; #cells sat; #cells neg ",
                                            100, -0.5, 100-0.5,
                                            setup->GetNActiveCells(), -0.5, setup->GetNActiveCells()-0.5);
  h2DNSatCellsVsNegCells->SetDirectory(0);
  h2DNSatCellsVsNegCells->Sumw2();
  TH2D* h2DNNegCellsVsNegCellsWToA = new TH2D( "h2DNNegCellsVsNegCellsWToA","# Neg ADC vs #Neg ADC w/ToA; #cells neg; #cells neg w/ ToA",
                                            setup->GetNActiveCells(), -0.5, setup->GetNActiveCells()-0.5,
                                            setup->GetNActiveCells(), -0.5, setup->GetNActiveCells()-0.5);
  h2DNNegCellsVsNegCellsWToA->SetDirectory(0);
  h2DNNegCellsVsNegCellsWToA->Sumw2();
  
  TH2D* h2DNSatCellsVsNegCellsWToA = new TH2D( "h2DNSatCellsVsNegCellsWToA","#Sat ADC vs #Neg ADC w/ToA; #cells sat; #cells neg w/ ToA ",
                                            100, -0.5, 100-0.5,
                                            setup->GetNActiveCells(), -0.5, setup->GetNActiveCells()-0.5);
  h2DNSatCellsVsNegCellsWToA->SetDirectory(0);
  h2DNSatCellsVsNegCellsWToA->Sumw2();
  
  TH2D* h2DAsicSatCellVsNegCell[setup->GetNMaxROUnit()+1][2];
  TH2D* h2DAsicSatCellVsNegCellWToA[setup->GetNMaxROUnit()+1][2];
  TH2D* h2DAsicNegCellVsNegCellWToA[setup->GetNMaxROUnit()+1][2];
  
  for (Int_t ro = 0; ro < setup->GetNMaxROUnit()+1; ro++){
    for (int h = 0; h< 2; h++){
      h2DAsicSatCellVsNegCell[ro][h]    = new TH2D( Form("h2DNSatCellsVsNegCells_%d_%d", ro, h),
                                                    "# Saturated vs #Neg ADC; #cells sat; #cells neg ",
                                                    32, -0.5, 32-0.5,
                                                    32, -0.5, 32-0.5);
      h2DAsicSatCellVsNegCell[ro][h]->SetDirectory(0);
      h2DAsicSatCellVsNegCell[ro][h]->Sumw2();

      h2DAsicSatCellVsNegCellWToA[ro][h]    = new TH2D( Form("h2DNSatCellsVsNegCellsWToA_%d_%d", ro, h),
                                                    "# Saturated vs #Neg ADC w/ TOA; #cells sat; #cells neg w/ ToA ",
                                                    32, -0.5, 32-0.5,
                                                    32, -0.5, 32-0.5);
      h2DAsicSatCellVsNegCellWToA[ro][h]->SetDirectory(0);
      h2DAsicSatCellVsNegCellWToA[ro][h]->Sumw2();
      h2DAsicNegCellVsNegCellWToA[ro][h]    = new TH2D( Form("h2DAsicNegCellVsNegCellWToA_%d_%d", ro, h),
                                                    "# Neg ADC vs #Neg ADC w/ TOA; #cells neg; #cells neg w/ ToA ",
                                                    32, -0.5, 32-0.5,
                                                    32, -0.5, 32-0.5);
      h2DAsicNegCellVsNegCellWToA[ro][h]->SetDirectory(0);
      h2DAsicNegCellVsNegCellWToA[ro][h]->Sumw2();
    }
  }
  
  RootOutputHist->mkdir("IndividualCellsXtalk");
  RootOutputHist->mkdir("IndividualCellsSat");
  RootOutputHist->cd("IndividualCellsXtalk");
  
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Migrad");  
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  
  //==================================================================================
  // process events from tree
  //==================================================================================
  for(int i=0; i<evts && i < maxEvents ; i++){
    if (i%5000 == 0&& i > 0 && debug > 0) std::cout << "Reading " <<  i << "/" << evts << " events"<< std::endl;
    if (debug > 2 ){
      std::cout << "************************************* NEW EVENT " << i << "  *********************************" << std::endl;
    }
    TdataIn->GetEntry(i);
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // process tiles in event to eval triggers
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
    int nSatTilesS  = 0;
    int nNegTilesS  = 0;
    int nSatTiles[(setup->GetNMaxROUnit()+1)*2];
    int nNegTiles[(setup->GetNMaxROUnit()+1)*2];
    int nToA          = 0;
    int nNegTilesSwT  = 0;
    int nNegTileswT[(setup->GetNMaxROUnit()+1)*2];
    for (int l = 0; l < (setup->GetNMaxROUnit()+1)*2; l++ ){
      nSatTiles[l]    = 0;
      nNegTiles[l]    = 0;
      nNegTileswT[l]  = 0;
    }
    
    for(int j=0; j<event.GetNTiles(); j++){
      Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
      int cellID    = aTile->GetCellID();
      int layer     = setup->GetLayer(cellID);
      int chInLayer = setup->GetChannelInLayerFull(cellID);
      int asic      = setup->GetROunit(cellID);
      int roCh      = setup->GetROchannel(cellID);
      
      // Check if single channel eval is enabled
      if (isSingleCh){
        // only continue if we are looking at the channel under investigation
        if (GetFixedROChannel() != roCh)
          continue;
        else 
          if (debug> 3)std::cout << "triggered: " << (setup->DecodeCellID(cellID)).Data() << std::endl;
      }
      
      int aHalf     = int(roCh/36);
      int linHalfs  = asic*2+aHalf;
      double toa      = aTile->GetRawTOA();
      bool hasTOA     = false;
      // check if we have a toa
      if (toa > 0){
        hasTOA        = true;
        nToA++;
      }
      
      bool isSaturated = aTile->IsSaturatedADC();
      if (isSaturated){
        nSatTilesS++;
        nSatTiles[linHalfs]++;
        hSatADCVsLayer->Fill(layer,chInLayer);
      }
      int nSampBPed    = aTile->IsBelowPed(calib.GetPedestalSigH(cellID)*5.);
      bool isNegXtalk  = false;
      if (nSampBPed != -1){
        isNegXtalk     = true;
        nNegTilesS++;
        nNegTiles[linHalfs]++;
        hNegCellVsLayer->Fill(layer,chInLayer);
        if (hasTOA){
         nNegTilesSwT++;
         nNegTileswT[linHalfs]++;
        }
      }
    }
    hNEvts->Fill(0);
    if (nSatTilesS > 0){
      hNEvts->Fill(1);
      for (int l = 0; l < (setup->GetNMaxROUnit()+1)*2; l++ ){
        h2DNSatCellsVsAsicHalfs->Fill(l,nSatTiles[l]);
      }
    }
    if (nNegTilesS > 0){
      hNEvts->Fill(2);
      for (int l = 0; l < (setup->GetNMaxROUnit()+1)*2; l++ ){
        h2DNNegCellsVsAsicHalfs->Fill(l,nNegTiles[l]);
        h2DNNegCellsVsAsicHalfsWT->Fill(l,nNegTileswT[l]);
      }
    }
    
    h2DNSatCellsVsNegCells->Fill(nSatTilesS,nNegTilesS);
    h2DNSatCellsVsNegCellsWToA->Fill(nSatTilesS,nNegTilesSwT);
    h2DNNegCellsVsNegCellsWToA->Fill(nNegTilesS,nNegTilesSwT);
    
    for (int l = 0; l < (setup->GetNMaxROUnit()+1)*2; l++ ){
      int asic = int(l/2);
      int half = int(l%2);
      h2DAsicSatCellVsNegCell[asic][half]->Fill(nSatTiles[l],nNegTiles[l]);
      h2DAsicSatCellVsNegCellWToA[asic][half]->Fill(nSatTiles[l],nNegTileswT[l]);
      h2DAsicNegCellVsNegCellWToA[asic][half]->Fill(nNegTiles[l],nNegTileswT[l]);
    }
    
    if (nSatTilesS == 0 &&  nNegTilesS == 0)
      continue;
    else {
      if (debug > 3){
        if (nNegTilesS> 0){
          std::cout << "neg tile trigger: \t";
          for (int l = 0; l < (setup->GetNMaxROUnit()+1)*2; l++){
            std::cout << nNegTiles[l] << "\t";
          }
          std::cout << "\t tot\t: " << nNegTilesS <<  "\t" << "w/ toa\t" << nNegTilesSwT<< std::endl;
        }
        if (nSatTilesS >0){
          std::cout << "sat tile trigger: \t";
          for (int l = 0; l < (setup->GetNMaxROUnit()+1)*2; l++){
            std::cout << nSatTiles[l] << "\t";
          }
          std::cout << "\t tot\t: " << nSatTilesS << std::endl;
        }
      }
    }
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // process tiles in event to fill selected events
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
    for(int j=0; j<event.GetNTiles(); j++){
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
      // histo filling for HGCROC specific things & resetting of ped
      //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++        
      Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
      int cellID      = aTile->GetCellID();
      int roCh        = setup->GetROchannel(cellID);
      int aHalf       = int(roCh/36);
      int asic        = setup->GetROunit(cellID);
      int linHalfs    = asic*2+aHalf;

      ithSpectraSat   = hSpectraSat.find(cellID);
      ithSpectraXtalk = hSpectraXtalk.find(cellID);
      ithSampleSat    = hSampleSat.find(cellID);
      ithSampleXtalk  = hSampleXtalk.find(cellID);
      
      // get pedestal values from calib object
      double ped    = aTile->GetPedestal();
      
      // obtain integrated tile quantities for histo filling
      double toaRaw   = aTile->GetRawTOA();
      bool hasTOA     = false;
      Int_t nOffEmpty = 2;
      
      // check if we have a toa
      if (toaRaw > 0)
        hasTOA        = true;

      int trigAsicH = 0;
      if (GetFixedROChannel() > 31)
        trigAsicH   = 1;
      
      // Detailed debug info with print of waveform
      if (debug > 3){
        aTile->PrintWaveFormDebugInfo(calib.GetPedestalMeanH(cellID), calib.GetPedestalMeanL(cellID), calib.GetPedestalSigL(cellID));
      }
    
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // fill as function of sample, no TOA requirement
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if (!isSingleCh){
        if (nSatTilesS > 0){
          if(ithSampleSat!=hSampleSat.end()){
            ithSampleSat->second.FillWaveform(aTile->GetADCWaveform(), ped);
          } else {
            RootOutputHist->cd("IndividualCellsSat");
            hSampleSat[cellID]=TileSpectra("SatSample",7,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
            hSampleSat[cellID].FillWaveform(aTile->GetADCWaveform(), ped);
          }
        }
        if (nNegTilesS > 0 ){
          if(ithSampleXtalk!=hSampleXtalk.end()){
            ithSampleXtalk->second.FillWaveform(aTile->GetADCWaveform(), ped);
          } else {
            RootOutputHist->cd("IndividualCellsXtalk");
            hSampleXtalk[cellID]=TileSpectra("XtalkSample",7,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
            hSampleXtalk[cellID].FillWaveform(aTile->GetADCWaveform(), ped);
          }
        }
      } else {
        if (nSatTiles[asic*2 + trigAsicH] > 0 ){
          if(ithSampleSat!=hSampleSat.end()){
            ithSampleSat->second.FillWaveform(aTile->GetADCWaveform(), ped);
          } else {
            RootOutputHist->cd("IndividualCellsSat");
            hSampleSat[cellID]=TileSpectra("SatSample",7,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
            hSampleSat[cellID].FillWaveform(aTile->GetADCWaveform(), ped);
          }
        }
        
        if (nNegTiles[asic*2 + trigAsicH] > 0 ){
          if(ithSampleXtalk!=hSampleXtalk.end()){
            ithSampleXtalk->second.FillWaveform(aTile->GetADCWaveform(), ped);
          } else {
            RootOutputHist->cd("IndividualCellsXtalk");
            hSampleXtalk[cellID]=TileSpectra("XtalkSample",7,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
            hSampleXtalk[cellID].FillWaveform(aTile->GetADCWaveform(), ped);
          }
        }         
      }
      
    
      // check whether tile has TOA, don't continue if it doesn't
      if (!hasTOA) continue;
    
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      // fill as function of time, needs TOA
      //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if (!isSingleCh){
        if (nSatTilesS > 0){
          if(ithSpectraSat!=hSpectraSat.end()){
            ithSpectraSat->second.FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), ped, nOffEmpty);
          } else {
            RootOutputHist->cd("IndividualCellsSat");
            hSpectraSat[cellID]=TileSpectra("Saturated",6,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
            hSpectraSat[cellID].FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), ped, nOffEmpty);
          }
          
        }
        if (nNegTilesS > 0 ){
          if(ithSpectraXtalk!=hSpectraXtalk.end()){
            ithSpectraXtalk->second.FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), ped, nOffEmpty);
          } else {
            RootOutputHist->cd("IndividualCellsXtalk");
            hSpectraXtalk[cellID]=TileSpectra("Xtalk",6,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
            hSpectraXtalk[cellID].FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), ped, nOffEmpty);
          }
        }
      } else {
        if (nSatTiles[asic*2 + trigAsicH] > 0 ){
          if(ithSpectraSat!=hSpectraSat.end()){
            ithSpectraSat->second.FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), ped, nOffEmpty);
          } else {
            RootOutputHist->cd("IndividualCellsSat");
            hSpectraSat[cellID]=TileSpectra("Saturated",6,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
            hSpectraSat[cellID].FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), ped, nOffEmpty);
          }
        }
        
        if (nNegTiles[asic*2 + trigAsicH] > 0 ){
          if(ithSpectraXtalk!=hSpectraXtalk.end()){
            ithSpectraXtalk->second.FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), ped, nOffEmpty);
          } else {
            RootOutputHist->cd("IndividualCellsXtalk");
            hSpectraXtalk[cellID]=TileSpectra("Xtalk",6,cellID,calib.GetTileCalib(cellID),event.GetROtype(),debug);
            hSpectraXtalk[cellID].FillWaveformVsTime(aTile->GetADCWaveform(), aTile->GetCorrectedTOA(), ped, nOffEmpty);
          }
        } 
      }
    }
  }
  
  RootInput->Close();

  //==================================================================================
  // write outputs to root fiif (de)le
  //==================================================================================
  RootOutputHist->cd();
    h2DNNegCellsVsAsicHalfs->Write();
    h2DNNegCellsVsAsicHalfsWT->Write();
    h2DNSatCellsVsAsicHalfs->Write();
    h2DNSatCellsVsNegCells->Write();
    h2DNSatCellsVsNegCellsWToA->Write();
    h2DNNegCellsVsNegCellsWToA->Write();
    hSatADCVsLayer->Write();
    hNegCellVsLayer->Write();
    
    for (Int_t ro = 0; ro < setup->GetNMaxROUnit()+1; ro++){
      for (int h = 0; h< 2; h++){
        h2DAsicSatCellVsNegCell[ro][h]->Write();
        h2DAsicSatCellVsNegCellWToA[ro][h]->Write();
        h2DAsicNegCellVsNegCellWToA[ro][h]->Write();
      }
    }
    hNEvts->Write();


  RootOutputHist->cd("IndividualCellsXtalk");
    for(ithSpectraXtalk=hSpectraXtalk.begin(); ithSpectraXtalk!=hSpectraXtalk.end(); ++ithSpectraXtalk){
      ithSpectraXtalk->second.WriteExt(true);
    }
    for(ithSampleXtalk=hSampleXtalk.begin(); ithSampleXtalk!=hSampleXtalk.end(); ++ithSampleXtalk){
      ithSampleXtalk->second.WriteExt(true);
    }
  RootOutputHist->cd("IndividualCellsSat");
    for(ithSpectraSat=hSpectraSat.begin(); ithSpectraSat!=hSpectraSat.end(); ++ithSpectraSat){
      ithSpectraSat->second.WriteExt(true);
    }
    for(ithSampleSat=hSampleSat.begin(); ithSampleSat!=hSampleSat.end(); ++ithSampleSat){
      ithSampleSat->second.WriteExt(true);
    }
  RootOutputHist->Close();

  // create histos if they don't exist for plotting reasons
  for (int id = 0; id < setup->GetMaxCellID()+1; id++){
    // skip if not defined in setup
    if (!setup->ContainedInSetup(id)) continue; 
    ithSpectraSat   = hSpectraSat.find(id);
    ithSpectraXtalk = hSpectraXtalk.find(id);
    ithSampleSat    = hSampleSat.find(id);
    ithSampleXtalk  = hSampleXtalk.find(id);
    
    if(ithSpectraXtalk==hSpectraXtalk.end()){
      hSpectraXtalk[id]=TileSpectra("Xtalk",6,id,calib.GetTileCalib(id),typeRO,debug);
    }
    if(ithSpectraSat==hSpectraSat.end()){
      hSpectraSat[id]=TileSpectra("Saturated",6,id,calib.GetTileCalib(id),typeRO,debug);
    }
    if(ithSampleXtalk==hSampleXtalk.end()){
      hSampleXtalk[id]=TileSpectra("XtalkSample",7,id,calib.GetTileCalib(id),typeRO,debug);
    }
    if(ithSpectraSat==hSpectraSat.end()){
      hSampleSat[id]=TileSpectra("SatSample",7,id,calib.GetTileCalib(id),typeRO,debug);
    }
  }
  
  //==================================================================================
  // Setup general plotting infos
  //==================================================================================    
  
  TString outputDirPlots = GetPlotOutputDir();
  Double_t textSizeRel = 0.035;

  if (isSingleCh){
    outputDirPlots = Form("%s/Channel_%d", outputDirPlots.Data(), GetFixedROChannel());
  }
  gSystem->Exec("mkdir -p "+outputDirPlots);

  StyleSettingsBasics("pdf");
  SetPlotStyle();  
  
  //==================================================================================
  // Create Summary plots
  //==================================================================================    
  TCanvas* canvas2DSigQA = new TCanvas("canvas2DSigQA","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas2DSigQA, 0.08, 0.13, 0.045, 0.07);

  canvas2DSigQA->SetLogz();
  
  h2DNNegCellsVsAsicHalfs->Scale(1./maxEvents);
  PlotSimple2D( canvas2DSigQA, h2DNNegCellsVsAsicHalfs, -10000, -10000, textSizeRel, Form("%s/NegCellsvsAsicHalfs.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);  
  h2DNNegCellsVsAsicHalfsWT->Scale(1./maxEvents);
  PlotSimple2D( canvas2DSigQA, h2DNNegCellsVsAsicHalfsWT, -10000, -10000, textSizeRel, Form("%s/NegCellsvsAsicHalfsWithToA.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  h2DNSatCellsVsAsicHalfs->Scale(1./maxEvents);
  PlotSimple2D( canvas2DSigQA, h2DNSatCellsVsAsicHalfs, 20, -10000, textSizeRel, Form("%s/SatCellsvsAsicHalfs.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);

  h2DNSatCellsVsNegCells->Scale(1./maxEvents);
  PlotSimple2D( canvas2DSigQA, h2DNSatCellsVsNegCells, 200, 20, textSizeRel, Form("%s/SatCellsvsNegCells.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  h2DNSatCellsVsNegCellsWToA->Scale(1./maxEvents);
  PlotSimple2D( canvas2DSigQA, h2DNSatCellsVsNegCellsWToA, 100, 20, textSizeRel, Form("%s/SatCellsvsNegCellsWToA.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  h2DNNegCellsVsNegCellsWToA->Scale(1./maxEvents);
  PlotSimple2D( canvas2DSigQA, h2DNNegCellsVsNegCellsWToA, 20, 200, textSizeRel, Form("%s/NegCellsvsNegCellsWToA.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
 
  hSatADCVsLayer->Scale(1./maxEvents);
  PlotSimple2D( canvas2DSigQA, hSatADCVsLayer, -10000, -10000, textSizeRel, Form("%s/SatCellsvsLayer.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  hNegCellVsLayer->Scale(1./maxEvents);
  PlotSimple2D( canvas2DSigQA, hNegCellVsLayer, -10000, -10000, textSizeRel, Form("%s/NegCellsvsLayer.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);  
  
  for (Int_t ro = 0; ro < setup->GetNMaxROUnit()+1; ro++){
    for (int h = 0; h< 2; h++){
      h2DAsicSatCellVsNegCell[ro][h]->Scale(1./maxEvents);
      PlotSimple2D( canvas2DSigQA, h2DAsicSatCellVsNegCell[ro][h], -10000, 10,  textSizeRel,
                        Form("%s/SatCellVsNegCell_Asic_%d_Half_%d.%s", outputDirPlots.Data(), ro, h, plotSuffix.Data()), 
                        it->second, 1, kFALSE, "colz",true, Form("Asic %d, Half %d",ro,h));
      h2DAsicSatCellVsNegCellWToA[ro][h]->Scale(1./maxEvents);
      PlotSimple2D( canvas2DSigQA, h2DAsicSatCellVsNegCellWToA[ro][h], -10000, 10, textSizeRel,
                        Form("%s/SatCellsvsNegCellsWToA_Asic_%d_Half_%d.%s", outputDirPlots.Data(), ro, h, plotSuffix.Data()), 
                        it->second, 1, kFALSE, "colz",true, Form("Asic %d, Half %d",ro,h));
      h2DAsicNegCellVsNegCellWToA[ro][h]->Scale(1./maxEvents);
      PlotSimple2D( canvas2DSigQA, h2DAsicNegCellVsNegCellWToA[ro][h], -10000, -10000, textSizeRel,
                        Form("%s/NegCellsvsNegCellsWToA_Asic_%d_Half_%d.%s", outputDirPlots.Data(), ro, h, plotSuffix.Data()), 
                        it->second, 1, kFALSE, "colz",true, Form("Asic %d, Half %d",ro,h));
    }
  }

  
  
  TCanvas* canvas1DSimple = new TCanvas("canvas1DSimple","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas1DSimple, 0.08, 0.03, 0.03, 0.07);
  
  PlotSimple1D(canvas1DSimple, hNEvts, -10000, -10000, textSizeRel, Form("%s/NEvts.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1);

  // print general calib info
  calib.PrintGlobalInfo();  

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  // Find detector config for plotting
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  DetConf::Type detConf = setup->GetDetectorConfig();
  
  // set pixel text size
  Int_t textSizePixel = 30;
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++  
  //Dual module plotting 
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++    
  if (detConf == DetConf::Type::Dual8M && ExtPlot > 1 ){
    TCanvas* canvas2ModPanelProf;
    TPad* pad2ModPanelProf[16];
    Double_t topRCornerX2ModProf[16];
    Double_t topRCornerY2ModProf[16];
    Double_t relSize2ModProf[16];
    CreateCanvasAndPadsForDualModTBPlot(canvas2ModPanelProf, pad2ModPanelProf,  topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf, textSizePixel, 0.045, "Prof", true);
    
    std::cout << "plotting dual module layers" << std::endl;
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      if (l%5 == 0 && l > 0 && debug > 0)
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
      // if (l%4 != 1 ){
      //   std::cout << "====> layer " << l << " skipping" << std::endl;
      //   continue;
      // }
      if (!setup->IsLayerOn(l,-1)){
        std::cout << "====> layer " << l << " not enabled" << std::endl;
        continue;
      }
      if (!calib.IsLayerEnabled(l,-1)){
        std::cout << "====> layer " << l << " all channels masked" << std::endl;
        continue;
      }
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectraXtalk, 1, -50, (it->second.samples)*25, -80, 800, l, 
                          Form("%s/WaveformNeg_Layer%02d.%s" ,outputDirPlots.Data(), l, plotSuffix.Data()), it->second, 0, GetFixedROChannel());
      PlotCorr2D2ModLayer(canvas2ModPanelProf,pad2ModPanelProf, topRCornerX2ModProf, topRCornerY2ModProf, relSize2ModProf,
                          textSizePixel, hSpectraSat, 1, -50, (it->second.samples)*25, -80, 800, l, 
                          Form("%s/WaveformSat_Layer%02d.%s" ,outputDirPlots.Data(),l, plotSuffix.Data()), it->second, 0, GetFixedROChannel() );            
    }
    std::cout << "ending plotting dual module layers" << std::endl;    
  }
  
  TCanvas* canvasPanAsic;
  TPad* padAsic[64];
  Double_t topRCornerXAsic[64];
  Double_t topRCornerYAsic[64];
  Double_t relSizeAsic[64];
  CreateCanvasAndPadsForAsicLFHCalTBPlot(canvasPanAsic, padAsic,  topRCornerXAsic, topRCornerYAsic, relSizeAsic, textSizePixel, 0.045, "", true);

  std::cout << "plotting full asic" << std::endl;
  for (Int_t a = 0; a < setup->GetNMaxROUnit()+1; a++){  
    std::cout << "====> asic " << a << std::endl;
    PlotCorr2DAsicLFHCal(canvasPanAsic,padAsic, topRCornerXAsic, topRCornerYAsic, relSizeAsic,
                        textSizePixel, hSpectraXtalk, 1, -50, (it->second.samples)*25, -80, 800, a, 
                        Form("%s/WaveformNeg_Asic%02d.%s" ,outputDirPlots.Data(), a, plotSuffix.Data()), it->second, 0, GetFixedROChannel());
    PlotCorr2DAsicLFHCal(canvasPanAsic,padAsic, topRCornerXAsic, topRCornerYAsic, relSizeAsic,
                        textSizePixel, hSpectraSat, 1, -50, (it->second.samples)*25, -80, 800, a, 
                        Form("%s/WaveformSat_Asic%02d.%s" ,outputDirPlots.Data(),a, plotSuffix.Data()), it->second, 0, GetFixedROChannel() );            
    PlotCorr2DAsicLFHCal(canvasPanAsic,padAsic, topRCornerXAsic, topRCornerYAsic, relSizeAsic,
                        textSizePixel, hSampleXtalk, 1, 0, (it->second.samples), -80, 800, a, 
                        Form("%s/WaveformSampleNeg_Asic%02d.%s" ,outputDirPlots.Data(), a, plotSuffix.Data()), it->second, 0, GetFixedROChannel());
    PlotCorr2DAsicLFHCal(canvasPanAsic,padAsic, topRCornerXAsic, topRCornerYAsic, relSizeAsic,
                        textSizePixel, hSampleSat, 1, 0, (it->second.samples), -80, 800, a, 
                        Form("%s/WaveformSampleSat_Asic%02d.%s" ,outputDirPlots.Data(),a, plotSuffix.Data()), it->second, 0, GetFixedROChannel() );            
  }
  std::cout << "ending plotting full asic" << std::endl;    
    
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

//***********************************************************************************************
//*********************** Create output files ***************************************************
//***********************************************************************************************
bool HGCROC_Waveform_Analysis::CreateOutputRootHistFile(void){
  std::cout << "Additional Output with histos being created: " << RootOutputNameHist.Data() << std::endl;
  if(Overwrite){
    std::cout << "overwriting exisiting output file" << std::endl;
    RootOutputHist=new TFile(RootOutputNameHist.Data(),"RECREATE");
  } else{
    std::cout << "creating output file" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"CREATE");
  }
  if(RootOutputHist->IsZombie()){
    std::cout<<"Error opening '"<<RootOutputHist<<"'no reachable path? Exist without force mode to overwrite?..."<<std::endl;
    return false;
  }
  return true;
}
