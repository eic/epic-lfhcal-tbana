#include "DataAnalysis.h"
#include <vector>
#include "TROOT.h"
#include <cmath>  
#include <algorithm>
//#include <unistd.h> // Add for use on Mac OS -> Same goes for Analyse.cc
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

// ****************************************************************************
// Checking and opening input and output files
// ****************************************************************************
bool DataAnalysis::CheckAndOpenIO(void){
  int matchingbranch;

  //Need to check first input to get the setup...I do not think it is necessary
    std::cout<<"Input name set to: '"<<RootInputName.Data() <<std::endl;  
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
  } else {
    std::cout<<"An input file is required, aborting"<<std::endl;
    return false;
  }  
  
  //Setup Output files
  if(!RootOutputName.IsNull()){    
    
    bool sCOF = CreateOutputRootFile();
    if (!sCOF) return false;
    
    TsetupOut = new TTree("Setup","Setup");
    setup=Setup::GetInstance();
    TsetupOut->Branch("setup",&rsw);
    TdataOut = new TTree("Data","Data");
    TdataOut->Branch("event",&event);
    TcalibOut = new TTree("Calib","Calib");
    TcalibOut->Branch("calib",&calib);
  } 
  
  //Setup Output files
  if(!RootOutputNameHist.IsNull()){    
    std::cout<<"Creating output file for hists: "<< RootOutputNameHist.Data() <<std::endl;
    bool sCOF = CreateOutputRootFileHist();
    if (!sCOF) return false;
  }
  
  if ( RootOutputName.IsNull() && RootOutputNameHist.IsNull()){
    std::cout<<"Output file name is missing, please add"<<std::endl;
    return false;
  }  
  
  return true;
}

// ****************************************************************************
// Primary process function to start all calibrations
// ****************************************************************************
bool DataAnalysis::Process(void){
  bool status;
  ROOT::EnableImplicitMT();
  
  if(RunQA){
    status=QAData();
  }  
  
  if(RunSimpleQA){
    status=SimpleQAData();
  }  
  return status;
}



//***********************************************************************************************
//*********************** Advanced QA routine ***************************************************
//***********************************************************************************************
bool DataAnalysis::QAData(void){
  std::cout<<"QA data"<<std::endl;
  if(debug==1000){
    std::cerr<<"Time Min: "<<timemin<<std::endl;
    std::cerr<<"Time Max: "<<timemax<<std::endl;
  }
  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  TdataIn->GetEntry(0);
  Int_t runNr           = event.GetRunNumber();
  ReadOut::Type typeRO  = event.GetROtype();

  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find(runNr);

  int species = -1;
  species = GetSpeciesIntFromRunInfo(it->second);
  float beamE = it->second.energy;
  std::cout << "Beam energy:" << beamE << std::endl;
  if (species == -1){
      std::cout << "WARNING: species unknown: " << it->second.species.Data() << "  aborting"<< std::endl;
      return false;
  }
  
  Int_t maxCellsPerLayer = setup->GetMaxChannelInLayerFull()+1;
  //=============================================================================================
  // Create output histos
  //=============================================================================================
  // create HG and LG histo's per channel
  TH2D* hspectraHGCorrvsCellID      = nullptr;
  TH2D* hspectraLGCorrvsCellID      = nullptr;
  TH2D* hspectraHGvsCellID          = nullptr;
  TH2D* hspectraLGvsCellID          = nullptr;
  TH2D* hspectraTOTvsCellID         = nullptr;

  if (typeRO == ReadOut::Type::Caen) {
    hspectraHGCorrvsCellID      = new TH2D( "hspectraHGCorr_vsCellID","ADC spectrum High Gain corrected vs CellID; cell ID; ADC_{HG} (arb. units)  ; counts ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,-200,3800);
    hspectraHGCorrvsCellID->SetDirectory(0);
    hspectraLGCorrvsCellID      = new TH2D( "hspectraLGCorr_vsCellID","ADC spectrum Low Gain corrected vs CellID; cell ID; ADC_{LG} (arb. units)  ; counts  ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,-200,3800);
    hspectraLGCorrvsCellID->SetDirectory(0);
    hspectraHGvsCellID      = new TH2D( "hspectraHG_vsCellID","ADC spectrum High Gain vs CellID; cell ID; ADC_{HG} (arb. units)   ; counts ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,0,4000);
    hspectraHGvsCellID->SetDirectory(0);
    hspectraLGvsCellID      = new TH2D( "hspectraLG_vsCellID","ADC spectrum Low Gain vs CellID; cell ID; ADC_{LG} (arb. units)  ; counts",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,0,4000);
    hspectraLGvsCellID->SetDirectory(0);
  } else if (typeRO == ReadOut::Type::Hgcroc) {
    hspectraHGvsCellID      = new TH2D( "hspectraADCvsCellID","ADC vs CellID; cell ID; ADC (arb. units)  ; counts ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 1128,-100,1028);
    hspectraHGvsCellID->SetDirectory(0);
    hspectraTOTvsCellID= new TH2D( "hspectraTOTvsCellID","TOT vs CellID; cell ID; TOT (arb. units)  ; counts ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4096,0,4096);
    hspectraTOTvsCellID->SetDirectory(0);
  }
  TH2D* hspectraEnergyvsCellID  = new TH2D( "hspectraEnergy_vsCellID","Energy vs CellID; cell ID; E (mip eq./tile)    ; counts",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 8000,0,250);
  hspectraEnergyvsCellID->SetDirectory(0);

  // beam species dependent binning for energy hists:
  TH2D* hspectraEnergyTotvsNCells     = nullptr;
  TH2D* hspectraEnergyTotvsNCellsMuon = nullptr;
  TH2D* hspectraEnergyVsLayer         = nullptr;
  TH2D* hspectraEnergyVsLayerMuon     = nullptr;
  // muons 
  if (species == 0 || species == 2 ){
    hspectraEnergyTotvsNCells       = new TH2D( "hspectraTotEnergy_vsNCells","Energy vs CellID; N_{Cells}; E_{tot} (mip eq./tile) ; counts",
                                                setup->GetNActiveCells()+1, -0.5, setup->GetNActiveCells()+1-0.5, 4000,0,500);
    hspectraEnergyTotvsNCellsMuon   = new TH2D( "hspectraTotEnergy_vsNCellsMuon","Energy vs CellID; N_{Cells}; E_{tot} (mip eq./tile) ; counts",
                                                setup->GetNActiveCells()+1, -0.5, setup->GetNActiveCells()+1-0.5, 4000,0,500);    
    hspectraEnergyVsLayer           = new TH2D( "hspectraTotLayerEnergy_vsLayer","Energy in layer vs Layer; Layer; E_{layer} (mip eq./tile) ; counts",
                                                setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, 2000,0,400);
    hspectraEnergyVsLayerMuon       = new TH2D( "hspectraTotLayerEnergy_vsLayerMuon","Energy in layer vs Layer Muon triggers; Layer; E_{layer} (mip eq./tile) ; counts",
                                                setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, 2000,0,400);
  // em-probes
  } else if ( species == 1 || species == 3 || species == 6 ){
    hspectraEnergyTotvsNCells       = new TH2D( "hspectraTotEnergy_vsNCells","Energy vs CellID; N_{Cells}; E_{tot} (mip eq./tile) ; counts",
                                                setup->GetNActiveCells()+1, -0.5, setup->GetNActiveCells()+1-0.5, 8000,0,1000);
    hspectraEnergyTotvsNCellsMuon   = new TH2D( "hspectraTotEnergy_vsNCellsMuon","Energy vs CellID; N_{Cells}; E_{tot} (mip eq./tile) ; counts",
                                                setup->GetNActiveCells()+1, -0.5, setup->GetNActiveCells()+1-0.5, 8000,0,1000);    
    hspectraEnergyVsLayer           = new TH2D( "hspectraTotLayerEnergy_vsLayer","Energy in layer vs Layer; Layer; E_{layer} (mip eq./tile) ; counts",
                                                setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, 2000,0,400);
    hspectraEnergyVsLayerMuon       = new TH2D( "hspectraTotLayerEnergy_vsLayerMuon","Energy in layer vs Layer Muon triggers; Layer; E_{layer} (mip eq./tile) ; counts",
                                                setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, 2000,0,400);
  // hadrons
  } else if ( species == 4 || species == 5  ){
    hspectraEnergyTotvsNCells       = new TH2D( "hspectraTotEnergy_vsNCells","Energy vs CellID; N_{Cells}; E_{tot} (mip eq./tile) ; counts",
                                                setup->GetNActiveCells()+1, -0.5, setup->GetNActiveCells()+1-0.5, 16000,0,2000);
    hspectraEnergyTotvsNCellsMuon   = new TH2D( "hspectraTotEnergy_vsNCellsMuon","Energy vs CellID; N_{Cells}; E_{tot} (mip eq./tile) ; counts",
                                                setup->GetNActiveCells()+1, -0.5, setup->GetNActiveCells()+1-0.5, 16000,0,2000);
    hspectraEnergyVsLayer           = new TH2D( "hspectraTotLayerEnergy_vsLayer","Energy in layer vs Layer; Layer; E_{layer} (mip eq./tile) ; counts",
                                                setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, 4000,0,800);
    hspectraEnergyVsLayerMuon       = new TH2D( "hspectraTotLayerEnergy_vsLayerMuon","Energy in layer vs Layer Muon triggers; Layer; E_{layer} (mip eq./tile) ; counts",
                                                setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, 4000,0,800);
  }
  
  hspectraEnergyTotvsNCells->SetDirectory(0);
  hspectraEnergyTotvsNCells->Sumw2();
  hspectraEnergyTotvsNCellsMuon->SetDirectory(0);
  hspectraEnergyTotvsNCellsMuon->Sumw2();
  hspectraEnergyVsLayer->SetDirectory(0);
  hspectraEnergyVsLayer->Sumw2();
  hspectraEnergyVsLayerMuon->SetDirectory(0);
  hspectraEnergyVsLayerMuon->Sumw2();
  
  Double_t minXPos = setup->GetMinX();
  Double_t maxXPos = setup->GetMaxX();
  Double_t minYPos = setup->GetMinY();
  Double_t maxYPos = setup->GetMaxY();
  Int_t nBinsXPos  = (Int_t)((maxXPos-minXPos)/0.1);
  Int_t nBinsYPos  = (Int_t)((maxXPos-minXPos)/0.1);
  
  TH2D* hAverageXVsLayer  = new TH2D( "hAverageX_vsLayer","Av. X pos in layer vs Layer; Layer; X_{pos} (cm) ; counts",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, nBinsXPos,minXPos,maxXPos);
  hAverageXVsLayer->SetDirectory(0);
  hAverageXVsLayer->Sumw2();
  TH2D* hAverageXVsLayerMuon  = new TH2D( "hAverageX_vsLayerMuon","Av. X pos in layer vs Layer Muon triggers; Layer; X_{pos} (cm) ; counts",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, nBinsXPos,minXPos,maxXPos);
  hAverageXVsLayerMuon->SetDirectory(0);
  hAverageXVsLayer->Sumw2();
  TH2D* hAverageYVsLayer  = new TH2D( "hAverageX_vsLayer","Av. Y pos in layer vs Layer; Layer; Y_{pos} (cm) ; counts",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, nBinsYPos,minYPos,maxYPos);
  hAverageYVsLayer->SetDirectory(0);
  hAverageYVsLayer->Sumw2();
  TH2D* hAverageYVsLayerMuon  = new TH2D( "hAverageX_vsLayerMuon","Av. Y pos in layer vs Layer Muon triggers; Layer; Y_{pos} (cm) ; counts",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, nBinsYPos,minYPos,maxYPos);
  hAverageYVsLayerMuon->SetDirectory(0);
  hAverageYVsLayerMuon->Sumw2();
  TH2D* hNCellsVsLayer  = new TH2D( "hnCells_vsLayer","NCells in layer vs Layer; Layer; N_{cells,layer} ; counts",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxCellsPerLayer,-0.5,maxCellsPerLayer-0.5);
  hNCellsVsLayer->SetDirectory(0);
  hNCellsVsLayer->Sumw2();
  TH2D* hNCellsVsLayerMuon  = new TH2D( "hnCells_vsLayerMuon","NCells in layer vs Layer  Muon triggers; Layer; N_{cells,layer} ; counts",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxCellsPerLayer,-0.5,maxCellsPerLayer-0.5);
  hNCellsVsLayerMuon->SetDirectory(0);
  hNCellsVsLayerMuon->Sumw2();
  


  //Create 1D Histos for Delta time
  TH1D* hDeltaTime = new TH1D("hDeltaTime", "Time Difference between Events; Delta Time (#mus); Counts", 2000, 0, 100000);
  
  std::map<int,TileSpectra> hSpectra;
  std::map<int,TileSpectra> hSpectraTrigg;
  std::map<int, TileSpectra>::iterator ithSpectra;
  std::map<int, TileSpectra>::iterator ithSpectraTrigg;

  RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->mkdir("IndividualCellsTrigg");
  
  //=============================================================================================
  // Setup general variables for analysis
  //=============================================================================================
  std::cout << "starting to run QA"<< std::endl;
  // initialize calib
  TcalibIn->GetEntry(0);
  int actCh1st = 0;
  double averageScale = calib.GetAverageScaleHigh(actCh1st);
  std::cout << "average HG mip: " << averageScale << "\t active ch: "<< actCh1st<< std::endl;

  // get and set maximum event number
  int evts=TdataIn->GetEntries();
  if ((eventNumber > -1) && (eventNumber <= evts)){
    evts = eventNumber;
    std::cout<<"restricting number of events in QA to " << evts << std::endl;
  }
  int evtsMuon= 0;
  double last_time = 0;
  double DeltaTime = 1000000000;
  double minESave  = 0.;
  
  int nActiveLayers = setup->GetNActiveLayers();
  
  //=============================================================================================
  // Run over all events in tree
  //=============================================================================================  
  for(int i=0; i<evts; i++){
    TdataIn->GetEntry(i);
    if (i%5000 == 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Set up event variables
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    double Etot = 0;
    int nCells  = 0;
    bool muontrigg = false;
    int locMuon = 0;
    std::map<int,Layer> layers;
    std::map<int, Layer>::iterator ithLayer;
    
    // Find and fill delta time
    double current_time = event.GetTimeStamp();
    if(last_time != 0){
      DeltaTime = current_time - last_time;
      if (debug == 1000){
        std::cerr<< "current timestamp: "<< current_time<<std::endl;
      }
    }
    if (DeltaTime == 0){
      if(debug == 1001){
        std::cerr<< "Run Number: " << runNr <<"      Event Number: "<< i << std::endl;
        std::cerr<< "Previous Timestamp (us): "<< last_time <<"      Current Timestamp: "<< current_time<<std::endl;
      }
    }
        //cut based on DeltaTime range
    if(DeltaTime < timemin || DeltaTime > timemax){
      //std::cout<<"event rejected:"<< i <<std::endl;
      last_time = current_time;
      continue;
    }
    hDeltaTime->Fill(DeltaTime);
    last_time = current_time;
        
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    // Evaluate all cells in event
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    for(int j=0; j<event.GetNTiles(); j++){
      if (typeRO == ReadOut::Type::Caen) {
        Caen* aTile=(Caen*)event.GetTile(j);
        double corrHG = aTile->GetADCHigh()-calib.GetPedestalMeanH(aTile->GetCellID());
        double corrLG = aTile->GetADCLow()-calib.GetPedestalMeanL(aTile->GetCellID());
        hspectraHGvsCellID->Fill(aTile->GetCellID(), aTile->GetADCHigh());
        hspectraLGvsCellID->Fill(aTile->GetCellID(), aTile->GetADCLow());
        hspectraHGCorrvsCellID->Fill(aTile->GetCellID(), corrHG);
        hspectraLGCorrvsCellID->Fill(aTile->GetCellID(), corrLG);
        if(aTile->GetE()> minESave ){ 
          nCells++;
          int currLayer = setup->GetLayer(aTile->GetCellID());
          ithLayer=layers.find(currLayer);
          if(ithLayer!=layers.end()){
            ithLayer->second.nCells+=1;
            ithLayer->second.energy+=aTile->GetE();
            ithLayer->second.avX+=setup->GetX(aTile->GetCellID())*aTile->GetE();
            ithLayer->second.avY+=setup->GetY(aTile->GetCellID())*aTile->GetE();
          } else {
            layers[currLayer]=Layer();
            layers[currLayer].nCells+=1;
            layers[currLayer].energy+=aTile->GetE();
            layers[currLayer].avX+=setup->GetX(aTile->GetCellID())*aTile->GetE();
            layers[currLayer].avY+=setup->GetY(aTile->GetCellID())*aTile->GetE();
          }
        }
        // read current tile
        ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
        if (aTile->GetLocalTriggerBit() == 1){
          if(ithSpectraTrigg!=hSpectraTrigg.end()){
            ithSpectraTrigg->second.FillTrigger(aTile->GetLocalTriggerPrimitive());
            ithSpectraTrigg->second.FillSpectraCAEN(corrLG,corrHG);
          } else {
            RootOutputHist->cd("IndividualCellsTrigg");
            hSpectraTrigg[aTile->GetCellID()]=TileSpectra("muonTriggers",aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
            hSpectraTrigg[aTile->GetCellID()].FillTrigger(aTile->GetLocalTriggerPrimitive());
            hSpectraTrigg[aTile->GetCellID()].FillSpectraCAEN(corrLG,corrHG);
          }
          locMuon++;        
        }      
        
        ithSpectra=hSpectra.find(aTile->GetCellID());
        if (ithSpectra!=hSpectra.end()){
          ithSpectra->second.FillSpectraCAEN(corrLG,corrHG);
          ithSpectra->second.FillTrigger(aTile->GetLocalTriggerPrimitive());;
          ithSpectra->second.FillCorrCAEN(corrLG,corrHG);
        } else {
          RootOutputHist->cd("IndividualCells");
          hSpectra[aTile->GetCellID()]=TileSpectra("AllTriggers",aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
          hSpectra[aTile->GetCellID()].FillSpectraCAEN(corrLG,corrHG);;
          hSpectra[aTile->GetCellID()].FillTrigger(aTile->GetLocalTriggerPrimitive());;
          hSpectra[aTile->GetCellID()].FillCorrCAEN(corrLG,corrHG);
        }
      } else if (typeRO == ReadOut::Type::Hgcroc){
        Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
        double adc = aTile->GetIntegratedADC();
        double tot = aTile->GetRawTOT();
        double toa = aTile->GetRawTOA();
        hspectraHGvsCellID->Fill(aTile->GetCellID(), adc);
        hspectraTOTvsCellID->Fill(aTile->GetCellID(), tot);
        if(aTile->GetE() > minESave ){ 
          nCells++;
          int currLayer = setup->GetLayer(aTile->GetCellID());
          ithLayer=layers.find(currLayer);
          if(ithLayer!=layers.end()){
            ithLayer->second.nCells+=1;
            ithLayer->second.energy+=aTile->GetE();
            ithLayer->second.avX+=setup->GetX(aTile->GetCellID())*aTile->GetE();
            ithLayer->second.avY+=setup->GetY(aTile->GetCellID())*aTile->GetE();
          } else {
            layers[currLayer]=Layer();
            layers[currLayer].nCells+=1;
            layers[currLayer].energy+=aTile->GetE();
            layers[currLayer].avX+=setup->GetX(aTile->GetCellID())*aTile->GetE();
            layers[currLayer].avY+=setup->GetY(aTile->GetCellID())*aTile->GetE();
          }
        }
        // read current tile
        ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
        if (aTile->GetLocalTriggerBit() == 1){
          if(ithSpectraTrigg!=hSpectraTrigg.end()){
            ithSpectraTrigg->second.FillTrigger(aTile->GetLocalTriggerPrimitive());
            ithSpectraTrigg->second.FillHGCROC(adc,toa,tot);
          } else {
            RootOutputHist->cd("IndividualCellsTrigg");
            hSpectraTrigg[aTile->GetCellID()]=TileSpectra("muonTriggers",aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
            hSpectraTrigg[aTile->GetCellID()].FillTrigger(aTile->GetLocalTriggerPrimitive());
            hSpectraTrigg[aTile->GetCellID()].FillHGCROC(adc,toa,tot);
          }
          locMuon++;        
        }      
        
        ithSpectra=hSpectra.find(aTile->GetCellID());
        if (ithSpectra!=hSpectra.end()){
          ithSpectra->second.FillTrigger(aTile->GetLocalTriggerPrimitive());;
          ithSpectra->second.FillHGCROC(adc,toa,tot);
        } else {
          RootOutputHist->cd("IndividualCells");
          hSpectra[aTile->GetCellID()]=TileSpectra("AllTriggers",aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
          hSpectra[aTile->GetCellID()].FillTrigger(aTile->GetLocalTriggerPrimitive());;
          hSpectra[aTile->GetCellID()].FillHGCROC(adc,toa,tot);
        }
      }
    }
    if (nCells > 0) {

      int nLayerSingleCell = 0;
      for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
        if (ithLayer->second.nCells == 1) 
            nLayerSingleCell++;
      }
      double fracLayer1Cell = (double)nLayerSingleCell/(int)layers.size();
      // if ( fracLayer1Cell > 0.8){
      //   muontrigg = true;
      //   evtsMuon++;        
      // }
      double fracLocMuon = (double)locMuon/nCells;
      if (fracLocMuon > 0.6){ 
        muontrigg = true;
        evtsMuon++;
      }
      
      if (muontrigg && debug > 3){
          std::cout << "Muon triggered:\t" <<  fracLayer1Cell << "\t" << fracLocMuon << "\t" << nActiveLayers << "\t" << nCells<< std::endl;
      }
      
      for(int l = 0; l < setup->GetNMaxLayer()+1;l++){
        ithLayer=layers.find(l);
        if (ithLayer!=layers.end()){          
          double avx     = ithLayer->second.avX/ithLayer->second.energy;
          double avy     = ithLayer->second.avY/ithLayer->second.energy;
          hspectraEnergyVsLayer->Fill(l,ithLayer->second.energy);
          if (muontrigg){
            hspectraEnergyVsLayerMuon->Fill(l,ithLayer->second.energy);
            hAverageXVsLayerMuon->Fill(l,avx);
            hAverageYVsLayerMuon->Fill(l,avy);
            hNCellsVsLayerMuon->Fill(l,ithLayer->second.nCells);
          }
          hAverageXVsLayer->Fill(l,avx);
          hAverageYVsLayer->Fill(l,avy);
          hNCellsVsLayer->Fill(l,ithLayer->second.nCells);
          if (ithLayer->second.nCells == 1) 
            nLayerSingleCell++;
        } else {
          hspectraEnergyVsLayer->Fill(l,0.);
          if (muontrigg){
            hspectraEnergyVsLayerMuon->Fill(l,0.);
            hAverageXVsLayerMuon->Fill(l,-20.);
            hAverageYVsLayerMuon->Fill(l,-20.);
            hNCellsVsLayerMuon->Fill(l,0);            
          }
          hAverageXVsLayer->Fill(l,-20);
          hAverageYVsLayer->Fill(l,-20);
          hNCellsVsLayer->Fill(l,0);
        }
      }

      for(int j=0; j<event.GetNTiles(); j++){
        Caen* aTile=(Caen*)event.GetTile(j);
        // remove bad channels from output
        double energy = aTile->GetE();
        if(energy!=0){ 
          hspectraEnergyvsCellID->Fill(aTile->GetCellID(), energy);
          Etot=Etot+energy;
        } 
      }

      hspectraEnergyTotvsNCells->Fill(nCells,Etot);
      if(muontrigg) hspectraEnergyTotvsNCellsMuon->Fill(nCells,Etot);
    }
  }
  
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }
  
  RootInput->Close();      
  
  RootOutputHist->cd();

  hDeltaTime->Write();

  hspectraHGvsCellID->Write();
  if (typeRO == ReadOut::Type::Caen){
    hspectraLGvsCellID->Write();
    hspectraHGCorrvsCellID->Write();
    hspectraLGCorrvsCellID->Write();    
  } else if (typeRO == ReadOut::Type::Hgcroc){
    hspectraTOTvsCellID->Write();
  }
  
  hspectraEnergyvsCellID->Write();
  hspectraEnergyTotvsNCells->Write();
  hspectraEnergyTotvsNCellsMuon->Write();
  
  TH2D* hspectraEnergyTotvsNCells_WoMuon = (TH2D*)hspectraEnergyTotvsNCells->Clone("hspectraTotEnergy_vsNCells_woMuon");
  // hspectraEnergyTotvsNCells_WoMuon->Sumw2();
  hspectraEnergyTotvsNCells_WoMuon->Add(hspectraEnergyTotvsNCellsMuon, -1);
  hspectraEnergyTotvsNCells_WoMuon->Write();
  
  TH1D* hspectraEnergyTot = (TH1D*)hspectraEnergyTotvsNCells->ProjectionY();
  hspectraEnergyTot->SetDirectory(0);
  TH1D* hspectraNCells = (TH1D*)hspectraEnergyTotvsNCells->ProjectionX();
  hspectraNCells->SetDirectory(0);
  hspectraEnergyTot->Write("hTotEnergy");
  hspectraNCells->Write("hNCells");

  TH1D* hspectraEnergyTotMuon = (TH1D*)hspectraEnergyTotvsNCellsMuon->ProjectionY();
  hspectraEnergyTotMuon->SetDirectory(0);
  TH1D* hspectraNCellsMuon = (TH1D*)hspectraEnergyTotvsNCellsMuon->ProjectionX();
  hspectraNCellsMuon->SetDirectory(0);
  hspectraEnergyTotMuon->Write("hTotEnergyMuon");
  hspectraNCellsMuon->Write("hNCellsMuon");
  
  TH1D* hspectraEnergyTotNonMuon = (TH1D*)hspectraEnergyTotvsNCells_WoMuon->ProjectionY();
  hspectraEnergyTotNonMuon->SetDirectory(0);
  TH1D* hspectraNCellsNonMuon = (TH1D*)hspectraEnergyTotvsNCells_WoMuon->ProjectionX();
  hspectraNCellsNonMuon->SetDirectory(0);
  hspectraEnergyTotNonMuon->Write("hTotEnergyNonMuon");
  hspectraNCellsNonMuon->Write("hNCellsNonMuon");
  
  // position hists
  hAverageXVsLayer->Write();
  hAverageXVsLayerMuon->Write();
  TH2D* hAverageXVsLayer_WoMuon = (TH2D*)hAverageXVsLayer->Clone("hAverageX_vsLayer_woMuon");
  // hAverageXVsLayer_WoMuon->Sumw2();
  hAverageXVsLayer_WoMuon->Add(hAverageXVsLayerMuon, -1);
  hAverageXVsLayer_WoMuon->Write();
  
  hAverageYVsLayer->Write();
  hAverageYVsLayerMuon->Write();
  TH2D* hAverageYVsLayer_WoMuon = (TH2D*)hAverageYVsLayer->Clone("hAverageY_vsLayer_woMuon");
  // hAverageYVsLayer_WoMuon->Sumw2();
  hAverageYVsLayer_WoMuon->Add(hAverageYVsLayerMuon, -1);
  hAverageYVsLayer_WoMuon->Write();
  
  hNCellsVsLayer->Write();
  hNCellsVsLayerMuon->Write();
  TH2D* hNCellsVsLayer_WoMuon = (TH2D*)hNCellsVsLayer->Clone("hnCells_vsLayer_woMuon");
  // hNCellsVsLayer_WoMuon->Sumw2();
  hNCellsVsLayer_WoMuon->Add(hNCellsVsLayerMuon, -1);
  hNCellsVsLayer_WoMuon->Write();
  
  hspectraEnergyVsLayer->Write();
  // hspectraEnergyVsLayer->Sumw2();
  hspectraEnergyVsLayerMuon->Write();
  // hspectraEnergyVsLayerMuon->Sumw2();
  TH2D* hspectraEnergyVsLayer_WoMuon = (TH2D*)hspectraEnergyVsLayer->Clone("hspectraTotLayerEnergy_vsLayer_woMuon");
  // hspectraEnergyVsLayer_WoMuon->Sumw2();
  hspectraEnergyVsLayer_WoMuon->Add(hspectraEnergyVsLayerMuon, -1);
  hspectraEnergyVsLayer_WoMuon->Write();

  TH1D* histELayer_All[65];
  TH1D* histELayer_Muon[65];
  TH1D* histELayer_WoMuon[65];
  TH1D* histXLayer_All[65];
  TH1D* histXLayer_Muon[65];
  TH1D* histXLayer_WoMuon[65];
  TH1D* histYLayer_All[65];
  TH1D* histYLayer_Muon[65];
  TH1D* histYLayer_WoMuon[65];
  TH1D* histNCellsLayer_All[65];
  TH1D* histNCellsLayer_Muon[65];
  TH1D* histNCellsLayer_WoMuon[65];
  Float_t maxYLAll    = 0;
  Float_t maxYLMuon   = 0;
  Float_t maxYLWOMuon = 0;
  Float_t maxXLAll    = 0;
  Float_t maxXLMuon   = 0;
  Float_t maxXLWOMuon = 0;
  std::map<int,Layer> layersMeanAll;
  std::map<int, Layer>::iterator ithLayerMeanAll;
  std::map<int,Layer> layersMeanMuon;
  std::map<int, Layer>::iterator ithLayerMeanMuon;
  std::map<int,Layer> layersMeanWOMuon;
  std::map<int, Layer>::iterator ithLayerMeanWOMuon;
  
  TGraphErrors* graphMeanE_Layer        = new TGraphErrors();
  TGraphErrors* graphMeanE_Layer_muon   = new TGraphErrors();
  TGraphErrors* graphMeanE_Layer_woMuon = new TGraphErrors();
  
  for(int l = 0; l < setup->GetNMaxLayer()+1;l++){  
    
    bool layerOn = true;
    
    histNCellsLayer_All[l]     = (TH1D*)hNCellsVsLayer->ProjectionY(Form("histNCellsLayer_AllTrigg_Layer_%02d",l),l+1,l+1);
    histNCellsLayer_All[l]->Write();
    histNCellsLayer_Muon[l]    = (TH1D*)hNCellsVsLayerMuon->ProjectionY(Form("histNCellsLayer_MuonTrigg_Layer_%02d",l),l+1,l+1);
    histNCellsLayer_Muon[l]->Write();
    histNCellsLayer_WoMuon[l]  = (TH1D*)hNCellsVsLayer_WoMuon->ProjectionY(Form("histNCellsLayer_WOMuonTrigg_Layer_%02d",l),l+1,l+1);
    histNCellsLayer_WoMuon[l]->Write();

    
    if (histNCellsLayer_All[l]->GetMean() < 0.001) layerOn =false;
    
    histELayer_All[l]     = (TH1D*)hspectraEnergyVsLayer->ProjectionY(Form("histELayer_AllTrigg_Layer_%02d",l),l+1,l+1);
    // histELayer_All[l]->Sumw2();
    histELayer_All[l]->Write();
    histELayer_Muon[l]    = (TH1D*)hspectraEnergyVsLayerMuon->ProjectionY(Form("histELayer_MuonTrigg_Layer_%02d",l),l+1,l+1);
    // histELayer_Muon[l]->Sumw2();
    histELayer_Muon[l]->Write();
    histELayer_WoMuon[l]  = (TH1D*)hspectraEnergyVsLayer_WoMuon->ProjectionY(Form("histELayer_WOMuonTrigg_Layer_%02d",l),l+1,l+1);
    // histELayer_WoMuon[l]->Sumw2();
    histELayer_WoMuon[l]->Write();
    
    if (layerOn){
      if (maxYLAll < histELayer_All[l]->GetMaximum())maxYLAll =  histELayer_All[l]->GetMaximum();
      if (maxXLAll < FindLastBinXAboveMin(histELayer_All[l],2)) maxXLAll = FindLastBinXAboveMin(histELayer_All[l],2);
      if (maxYLMuon < histELayer_Muon[l]->GetMaximum() )maxYLMuon =  histELayer_Muon[l]->GetMaximum();
      if (maxXLMuon < FindLastBinXAboveMin(histELayer_Muon[l],2) ) maxXLMuon = FindLastBinXAboveMin(histELayer_Muon[l],2);
      if (maxYLWOMuon < histELayer_WoMuon[l]->GetMaximum() )maxYLWOMuon =  histELayer_WoMuon[l]->GetMaximum();
      if (maxXLWOMuon < FindLastBinXAboveMin(histELayer_WoMuon[l],2) ) maxXLWOMuon = FindLastBinXAboveMin(histELayer_WoMuon[l],2);
    }

    histXLayer_All[l]     = (TH1D*)hAverageXVsLayer->ProjectionY(Form("histXLayer_AllTrigg_Layer_%02d",l),l+1,l+1);
    histXLayer_All[l]->Write();
    histXLayer_Muon[l]    = (TH1D*)hAverageXVsLayerMuon->ProjectionY(Form("histXLayer_MuonTrigg_Layer_%02d",l),l+1,l+1);
    histXLayer_Muon[l]->Write();
    histXLayer_WoMuon[l]  = (TH1D*)hAverageXVsLayer_WoMuon->ProjectionY(Form("histXLayer_WOMuonTrigg_Layer_%02d",l),l+1,l+1);
    histXLayer_WoMuon[l]->Write();

    histYLayer_All[l]     = (TH1D*)hAverageYVsLayer->ProjectionY(Form("histYLayer_AllTrigg_Layer_%02d",l),l+1,l+1);
    histYLayer_All[l]->Write();
    histYLayer_Muon[l]    = (TH1D*)hAverageYVsLayerMuon->ProjectionY(Form("histYLayer_MuonTrigg_Layer_%02d",l),l+1,l+1);
    histYLayer_Muon[l]->Write();
    histYLayer_WoMuon[l]  = (TH1D*)hAverageYVsLayer_WoMuon->ProjectionY(Form("histYLayer_WOMuonTrigg_Layer_%02d",l),l+1,l+1);
    histYLayer_WoMuon[l]->Write();

    layersMeanAll[l]=Layer();
    layersMeanAll[l].energy+=histELayer_All[l]->GetMean();
    layersMeanAll[l].avX+=histXLayer_All[l]->GetMean();
    layersMeanAll[l].avY+=histYLayer_All[l]->GetMean();
    layersMeanAll[l].nCells+=histNCellsLayer_All[l]->GetMean();
    
    layersMeanMuon[l]=Layer();
    layersMeanMuon[l].energy+=histELayer_Muon[l]->GetMean();
    layersMeanMuon[l].avX+=histXLayer_Muon[l]->GetMean();
    layersMeanMuon[l].avY+=histYLayer_Muon[l]->GetMean();
    layersMeanMuon[l].nCells+=histNCellsLayer_Muon[l]->GetMean();
    
    layersMeanWOMuon[l]=Layer();
    layersMeanWOMuon[l].energy+=histELayer_WoMuon[l]->GetMean();
    layersMeanWOMuon[l].avX+=histXLayer_WoMuon[l]->GetMean();
    layersMeanWOMuon[l].avY+=histYLayer_WoMuon[l]->GetMean();
    layersMeanWOMuon[l].nCells+=histNCellsLayer_WoMuon[l]->GetMean();
    
    graphMeanE_Layer->SetPoint(graphMeanE_Layer->GetN(), l,histELayer_All[l]->GetMean());
    graphMeanE_Layer->SetPointError(graphMeanE_Layer->GetN()-1, 0,histELayer_All[l]->GetMeanError());
    // graphMeanE_Layer->SetPointError(graphMeanE_Layer->GetN()-1, 0,histELayer_All[l]->GetRMS());
    graphMeanE_Layer_muon->SetPoint(graphMeanE_Layer_muon->GetN(),l,histELayer_Muon[l]->GetMean());
    graphMeanE_Layer_muon->SetPointError(graphMeanE_Layer_muon->GetN()-1, 0,histELayer_Muon[l]->GetMeanError());
    // graphMeanE_Layer_muon->SetPointError(graphMeanE_Layer_muon->GetN()-1, 0,histELayer_Muon[l]->GetRMS());
    graphMeanE_Layer_woMuon->SetPoint(graphMeanE_Layer_woMuon->GetN(),l,histELayer_WoMuon[l]->GetMean());
    graphMeanE_Layer_woMuon->SetPointError(graphMeanE_Layer_woMuon->GetN()-1, 0,histELayer_WoMuon[l]->GetMeanError());
    // graphMeanE_Layer_woMuon->SetPointError(graphMeanE_Layer_woMuon->GetN()-1, 0,histELayer_WoMuon[l]->GetRMS());
  }
  
  std::cout<< "average per layer - All" << std::endl;
  graphMeanE_Layer->Write("graphMeanE_Layer");
  std::cout<< "average per layer - no Muon" << std::endl;
  graphMeanE_Layer_woMuon->Write("graphMeanE_Layer_woMuon");
  std::cout<< "average per layer - Muon" << std::endl;
  graphMeanE_Layer_muon->Write("graphMeanE_Layer_Muon");
  
  //**********************************************************************
  //********************* Plotting ***************************************
  //**********************************************************************  
  TString outputDirPlots = GetPlotOutputDir();
  gSystem->Exec("mkdir -p "+outputDirPlots);
  
  
  
  //**********************************************************************
  // Create canvases for channel overview plotting
  //**********************************************************************
  Double_t textSizeRel = 0.035;
  StyleSettingsBasics("pdf");
  SetPlotStyle();
  
  TCanvas* canvas2DCorr = new TCanvas("canvasCorrPlots","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas2DCorr, 0.08, 0.13, 0.045, 0.07);
  canvas2DCorr->SetLogz(1);
  TCanvas* canvas2DCorrWOLine = new TCanvas("canvasCorrPlotsWoLine","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas2DCorrWOLine, 0.08, 0.13, 0.01, 0.07);
  canvas2DCorrWOLine->SetLogz(1);
  
  TCanvas* canvasDeltaTime = new TCanvas("canvasDeltaTime","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvasDeltaTime, 0.08, 0.07, 0.01, 0.07);
  canvasDeltaTime->SetLogy(1);
  if(DeltaTimePlot>0){
    PlotSimple1D( canvasDeltaTime, hDeltaTime, -10000, timemax, textSizeRel, Form("%s/deltaTime.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1);
  } 

  PlotSimple2D( canvas2DCorr, hspectraHGvsCellID, -10000, -10000, textSizeRel, Form("%s/HG.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  if (typeRO == ReadOut::Type::Caen){
    PlotSimple2D( canvas2DCorr, hspectraLGvsCellID, -10000, -10000, textSizeRel, Form("%s/LG.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
    PlotSimple2D( canvas2DCorr, hspectraHGCorrvsCellID, -10000, -10000, textSizeRel, Form("%s/HGCorr.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
    PlotSimple2D( canvas2DCorr, hspectraLGCorrvsCellID, -10000, -10000, textSizeRel, Form("%s/LGCorr.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  } else if (typeRO == ReadOut::Type::Hgcroc){
    PlotSimple2D( canvas2DCorr, hspectraTOTvsCellID, -10000, -10000, textSizeRel, Form("%s/TOT.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);    
  }
  PlotSimple2D( canvas2DCorr, hspectraEnergyvsCellID, -10000, -10000, textSizeRel, Form("%s/EnergyVsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraEnergyTotvsNCells, -10000, -10000, textSizeRel, Form("%s/EnergyTotalVsNCells.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form("evts = %d",evts));
  PlotSimple2D( canvas2DCorr, hspectraEnergyTotvsNCellsMuon, -10000, -10000, textSizeRel, Form("%s/EnergyTotalVsNCells_MuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form("Muon evts = %d",evtsMuon));
  PlotSimple2D( canvas2DCorr, hspectraEnergyTotvsNCells_WoMuon, -10000, -10000, textSizeRel, Form("%s/EnergyTotalVsNCells_WoMuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form("Non Muon evts = %d",evts-evtsMuon));

  PlotSimple2D( canvas2DCorr, hspectraEnergyVsLayer, -10000, -10000, textSizeRel, Form("%s/EnergyVsLayer.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraEnergyVsLayerMuon, -10000, -10000, textSizeRel, Form("%s/EnergyVsLayer_MuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, "Muon triggers");
  PlotSimple2D( canvas2DCorr, hspectraEnergyVsLayer_WoMuon, -10000, -10000, textSizeRel, Form("%s/EnergyVsLayer_WOMuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, "Non Muon triggers");
  PlotSimple2D( canvas2DCorr, hAverageXVsLayer, -10000, -10000, textSizeRel, Form("%s/XPosVsLayer.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hAverageXVsLayerMuon, -10000, -10000, textSizeRel, Form("%s/XPosVsLayer_MuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, "Muon triggers");
  PlotSimple2D( canvas2DCorr, hAverageXVsLayer_WoMuon, -10000, -10000, textSizeRel, Form("%s/XPosVsLayer_WOMuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, "Non Muon triggers");
  PlotSimple2D( canvas2DCorr, hAverageYVsLayer, -10000, -10000, textSizeRel, Form("%s/YPosVsLayer.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hAverageYVsLayerMuon, -10000, -10000, textSizeRel, Form("%s/YPosVsLayer_MuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, "Muon triggers");
  PlotSimple2D( canvas2DCorr, hAverageYVsLayer_WoMuon, -10000, -10000, textSizeRel, Form("%s/YPosVsLayer_WOMuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, "Non Muon triggers");
  PlotSimple2D( canvas2DCorr, hNCellsVsLayer, -10000, -10000, textSizeRel, Form("%s/NcellsLayerVsLayer.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hNCellsVsLayerMuon, -10000, -10000, textSizeRel, Form("%s/NcellsLayerVsLayer_MuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, "Muon triggers");
  PlotSimple2D( canvas2DCorr, hNCellsVsLayer_WoMuon, -10000, -10000, textSizeRel, Form("%s/NcellsLayerVsLayer_WOMuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, "Non Muon triggers");
  
  double maxEProp = 400.;
  if (TMath::Abs(beamE -5.0) < 0.01) {
      maxEProp = 300;
      std::cout << "reset max for beam E" << beamE << "\t" << maxEProp << std::endl;
  }
  if (species == 0 || species == 2 ){
    Plot2DWithGraph( canvas2DCorrWOLine, hspectraEnergyVsLayerMuon, nullptr, maxEProp, -10000,  textSizeRel, Form("%s/EnergyVsLayer_Propaganda.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", false, "");
    Plot2DWithGraph( canvas2DCorrWOLine, hspectraEnergyVsLayerMuon, graphMeanE_Layer_muon, maxEProp, -10000,  textSizeRel, Form("%s/EnergyVsLayer_PropagandaWGraph.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", false, "");
  } else {
    Plot2DWithGraph( canvas2DCorrWOLine, hspectraEnergyVsLayer_WoMuon,  nullptr, maxEProp, -10000, textSizeRel, Form("%s/EnergyVsLayer_Propaganda.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", false, "");
    Plot2DWithGraph( canvas2DCorrWOLine, hspectraEnergyVsLayer_WoMuon,  graphMeanE_Layer_woMuon, maxEProp, -10000, textSizeRel, Form("%s/EnergyVsLayer_PropagandaWGraph.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", false, "");
  }
  canvas2DCorrWOLine->SetLogy();
  if (species == 0 || species == 2 ){
    Plot2DWithGraph( canvas2DCorrWOLine, hspectraEnergyVsLayerMuon, nullptr, 400, -10000,  textSizeRel, Form("%s/EnergyVsLayer_Propaganda_LogY.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", false, "");
    Plot2DWithGraph( canvas2DCorrWOLine, hspectraEnergyVsLayerMuon, graphMeanE_Layer_muon, 400, -10000,  textSizeRel, Form("%s/EnergyVsLayer_PropagandaWGraph_LogY.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", false, "");
  } else {
    Plot2DWithGraph( canvas2DCorrWOLine, hspectraEnergyVsLayer_WoMuon,  nullptr, 400, -10000, textSizeRel, Form("%s/EnergyVsLayer_Propaganda_LogY.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", false, "");
    Plot2DWithGraph( canvas2DCorrWOLine, hspectraEnergyVsLayer_WoMuon,  graphMeanE_Layer_woMuon, 400, -10000, textSizeRel, Form("%s/EnergyVsLayer_PropagandaWGraph_LogY.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", false, "");
  }
  canvas2DCorrWOLine->SetLogy(kFALSE);
  

  TCanvas* canvas1DSimple = new TCanvas("canvas1DSimple","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas1DSimple, 0.08, 0.01, 0.03, 0.07);
  hspectraEnergyTot->Rebin(4);
  hspectraEnergyTot->Scale(1./evts);
  hspectraEnergyTot->GetYaxis()->SetTitle("counts/event");
  
  TF1* fitGaussETot = new TF1("fitEtot","gaus",hspectraEnergyTot->GetMean()-1.5*hspectraEnergyTot->GetRMS(),hspectraEnergyTot->GetMean()+1.5*hspectraEnergyTot->GetRMS());
  hspectraEnergyTot->Fit(fitGaussETot,"QRMEN0S");
  std::cout<< "All trigg: "<<fitGaussETot->GetParameter(1) << "\t" << fitGaussETot->GetParameter(2) << std::endl;
  
  Double_t maxEPlot =  FindLastBinXAboveMin(hspectraEnergyTot,1./evts);
  std::cout << maxEPlot << std::endl;
  
  PlotSimple1D(canvas1DSimple, hspectraEnergyTot, -10000, maxEPlot, textSizeRel, Form("%s/EnergyTot.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, Form("#LT E_{Tot} #GT = %.1f #pm %.1f (mip/tile eq.)", hspectraEnergyTot->GetMean(), hspectraEnergyTot->GetRMS() ));

  
  hspectraEnergyTotMuon->Rebin(4);
  hspectraEnergyTotMuon->Scale(1./evts);
  hspectraEnergyTotNonMuon->Rebin(4);
  hspectraEnergyTotNonMuon->Scale(1./evts);

  TF1* fitGaussETotNonMuon = new TF1("fitEtot","gaus",hspectraEnergyTotNonMuon->GetMean()-1.5*hspectraEnergyTotNonMuon->GetRMS(),hspectraEnergyTotNonMuon->GetMean()+1.5*hspectraEnergyTotNonMuon->GetRMS());
  hspectraEnergyTotNonMuon->Fit(fitGaussETotNonMuon,"QRMEN0S");
  std::cout<< "Non muon trigg: "<<fitGaussETotNonMuon->GetParameter(1) << "\t" << fitGaussETotNonMuon->GetParameter(2) << std::endl;
  hspectraEnergyTotNonMuon->Fit(fitGaussETotNonMuon,"QRMEN0S","",fitGaussETotNonMuon->GetParameter(1)-1.*fitGaussETotNonMuon->GetParameter(2), fitGaussETotNonMuon->GetParameter(1)+1.*fitGaussETotNonMuon->GetParameter(2));
  std::cout<< "Non muon trigg refit: "<<fitGaussETotNonMuon->GetParameter(1) << "\t" << fitGaussETotNonMuon->GetParameter(2) << std::endl;
  
  
  PlotContamination1D(canvas1DSimple, hspectraEnergyTot,hspectraEnergyTotMuon, hspectraEnergyTotNonMuon, -10000, maxEPlot, textSizeRel, Form("%s/EnergyTotSplit.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, Form("#LT E_{Tot,non muon} #GT = %.1f#pm %.1f (mip/tile eq.)", hspectraEnergyTotNonMuon->GetMean(), hspectraEnergyTotNonMuon->GetRMS() ));

  PlotSimpleWithFit1D(canvas1DSimple, hspectraEnergyTotNonMuon, fitGaussETotNonMuon , -10000, maxEPlot, textSizeRel, Form("%s/EnergyTotNonMuonWithFit.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, Form("#LT E_{Tot,non muon} #GT = %.1f #pm %.1f (mip/tile eq.)", fitGaussETotNonMuon->GetParameter(1), fitGaussETotNonMuon->GetParameter(2) ));

  hspectraNCells->Scale(1./evts);
  hspectraNCells->GetYaxis()->SetTitle("counts/event");
  PlotSimple1D(canvas1DSimple, hspectraNCells, -10000, -10000, textSizeRel, Form("%s/NCells.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, Form("#LT N_{Cells} #GT = %.1f",hspectraNCells->GetMean() ));
  hspectraNCellsMuon->Scale(1./evts);
  hspectraNCellsNonMuon->Scale(1./evts);  
  PlotContamination1D( canvas1DSimple, hspectraNCells, hspectraNCellsMuon, hspectraNCellsNonMuon, -10000, -10000, textSizeRel, Form("%s/NCellsSplit.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, Form("#LT N_{Cells,non muon} #GT = %.1f",hspectraNCellsNonMuon->GetMean() ));
  
  // canvas1DSimple->SetLogy();
  PlotLayerOverlay(canvas1DSimple, histELayer_All, evts*10, maxXLAll ,GetAverageLayer(layersMeanAll), GetMaxLayer(layersMeanAll), textSizeRel, Form("%s/ELayerOverlay_AllTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "All triggers");
  PlotLayerOverlay(canvas1DSimple, histELayer_Muon, evtsMuon*10, maxXLMuon, GetAverageLayer(layersMeanMuon), GetMaxLayer(layersMeanMuon),textSizeRel, Form("%s/ELayerOverlay_MuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "Muon triggers");
  PlotLayerOverlay(canvas1DSimple, histELayer_WoMuon, (evts-evtsMuon)*10, maxXLAll, GetAverageLayer(layersMeanWOMuon), GetMaxLayer(layersMeanWOMuon),textSizeRel, Form("%s/ELayerOverlay_NonMuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "Non muon triggers");
  
  if (species == 0 || species == 2 ){
    PlotLayerOverlay(canvas1DSimple, histELayer_Muon, evtsMuon*10, maxXLMuon, GetAverageLayer(layersMeanMuon), GetMaxLayer(layersMeanMuon),textSizeRel, Form("%s/ELayerOverlay_Propaganda.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "", false);
  } else {
    PlotLayerOverlay(canvas1DSimple, histELayer_WoMuon, (evts-evtsMuon)*10, maxXLAll, GetAverageLayer(layersMeanWOMuon), GetMaxLayer(layersMeanWOMuon),textSizeRel, Form("%s/ELayerOverlay_Propaganda.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "", false);
  }

  
  PlotLayerOverlay(canvas1DSimple, histXLayer_All, evts*100, maxXPos-2. ,GetAverageLayer(layersMeanAll), GetMaxLayer(layersMeanAll), textSizeRel, Form("%s/XPosLayerOverlay_AllTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "All triggers");
  PlotLayerOverlay(canvas1DSimple, histXLayer_Muon, evtsMuon*100, maxXPos-2., GetAverageLayer(layersMeanMuon), GetMaxLayer(layersMeanMuon),textSizeRel, Form("%s/XPosLayerOverlay_MuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "Muon triggers");
  PlotLayerOverlay(canvas1DSimple, histXLayer_WoMuon, (evts-evtsMuon)*100, maxXPos-2., GetAverageLayer(layersMeanWOMuon), GetMaxLayer(layersMeanWOMuon),textSizeRel, Form("%s/XPosLayerOverlay_NonMuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "Non muon triggers");

  PlotLayerOverlay(canvas1DSimple, histYLayer_All, evts*100, maxYPos-2. ,GetAverageLayer(layersMeanAll), GetMaxLayer(layersMeanAll), textSizeRel, Form("%s/YPosLayerOverlay_AllTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "All triggers");
  PlotLayerOverlay(canvas1DSimple, histYLayer_Muon, evtsMuon*100, maxYPos-2., GetAverageLayer(layersMeanMuon), GetMaxLayer(layersMeanMuon),textSizeRel, Form("%s/YPosLayerOverlay_MuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "Muon triggers");
  PlotLayerOverlay(canvas1DSimple, histYLayer_WoMuon, (evts-evtsMuon)*100, maxYPos-2., GetAverageLayer(layersMeanWOMuon), GetMaxLayer(layersMeanWOMuon),textSizeRel, Form("%s/YPosLayerOverlay_NonMuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "Non muon triggers");

  PlotLayerOverlay(canvas1DSimple, histNCellsLayer_All, evts*100, maxCellsPerLayer-0.5 ,GetAverageLayer(layersMeanAll), GetMaxLayer(layersMeanAll), textSizeRel, Form("%s/NCellsLayerOverlay_AllTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "All triggers");
  PlotLayerOverlay(canvas1DSimple, histNCellsLayer_Muon, evtsMuon*100, maxCellsPerLayer-0.5, GetAverageLayer(layersMeanMuon), GetMaxLayer(layersMeanMuon),textSizeRel, Form("%s/NCellsLayerOverlay_MuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "Muon triggers");
  PlotLayerOverlay(canvas1DSimple, histNCellsLayer_WoMuon, (evts-evtsMuon)*100, maxCellsPerLayer-0.5, GetAverageLayer(layersMeanWOMuon), GetMaxLayer(layersMeanWOMuon),textSizeRel, Form("%s/NCellsLayerOverlay_NonMuonTrigg.%s", outputDirPlots.Data(), plotSuffix.Data()),it->second, 1, "Non muon triggers");

  if (ExtPlot > 0){
    gSystem->Exec("mkdir -p "+outputDirPlots+"/detailed");
    //***********************************************************************************************************
    //********************************* 8 Panel overview plot  **************************************************
    //***********************************************************************************************************
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
    TCanvas* canvas8Panel;
    TPad* pad8Panel[8];
    Double_t topRCornerX[8];
    Double_t topRCornerY[8];
    Int_t textSizePixel = 30;
    Double_t relSize8P[8];
    CreateCanvasAndPadsFor8PannelTBPlot(canvas8Panel, pad8Panel,  topRCornerX, topRCornerY, relSize8P, textSizePixel);
  
    TCanvas* canvas8PanelProf;
    TPad* pad8PanelProf[8];
    Double_t topRCornerXProf[8];
    Double_t topRCornerYProf[8];
    Double_t relSize8PProf[8];
    CreateCanvasAndPadsFor8PannelTBPlot(canvas8PanelProf, pad8PanelProf,  topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 0.045, "Prof", false);

    
    calib.PrintGlobalInfo();  
    Double_t maxHG = ReturnMipPlotRangeDepVov(calib.GetVov(),true, ReadOut::Type::Caen);
    Double_t maxLG = 3800;
    std::cout << "plotting single layers" << std::endl;
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      if (l%10 == 0 && l > 0 && debug > 0){
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;
      }
      PlotMipWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                              hSpectra, hSpectraTrigg, setup, true, -100, 3800, 1.2, l, 0,
                              Form("%s/detailed/LocTriggerMip_HG_Layer%02d_%.0f_%.0f.%s" ,outputDirPlots.Data(), l, timemin, timemax, plotSuffix.Data()), it->second);
      PlotMipWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                              hSpectra, hSpectraTrigg, setup, true, -100, maxHG, 1.2, l, 0,
                              Form("%s/detailed/LocTriggerMip_Zoomed_HG_Layer%02d_%.0f_%.0f.%s" ,outputDirPlots.Data(), l, timemin, timemax, plotSuffix.Data()), it->second);
      PlotTriggerPrimWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                        hSpectra, setup, averageScale, 0.8, 2.,
                                        0, 6000, 1.2, l, 0, Form("%s/detailed/All_TriggPrimitive_Layer%02d_%.0f_%.0f.%s" ,outputDirPlots.Data(), l, timemin, timemax, plotSuffix.Data()), it->second);
      if (ExtPlot > 1){
        PlotCorrWithFits8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 
                                hSpectra, 0, -20, 800, 1.2, l, 0,
                                Form("%s/detailed/LGHG_Corr_Layer%02d_%.0f_%.0f.%s" ,outputDirPlots.Data(), l, timemin, timemax, plotSuffix.Data()), it->second);
        PlotMipWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                hSpectra, hSpectraTrigg, setup, false, -20, maxLG, 1.2, l, 0,
                                Form("%s/detailed/LocTriggerMip_LG_Layer%02d_%.0f_%.0f.%s" ,outputDirPlots.Data(), l, timemin, timemax, plotSuffix.Data()), it->second);
        PlotTriggerPrimWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                        hSpectraTrigg, setup, averageScale, 0.8, 2.,
                                        0, maxHG*2, 1.2, l, 0, Form("%s/detailed/LocalMuon_TriggPrimitive_Layer%02d_%.0f_%.0f.%s" ,outputDirPlots.Data(), l, timemin, timemax, plotSuffix.Data()), it->second);
      }
    }
    std::cout << "done plotting" << std::endl;
  }
  RootOutputHist->Close();
  return true;
}


//***********************************************************************************************
//*********************** Simple QA routine ***************************************************
//***********************************************************************************************
bool DataAnalysis::SimpleQAData(void){
  std::cout<<"QA data"<<std::endl;

  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  TdataIn->GetEntry(0);
  Int_t runNr = event.GetRunNumber();
  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find(runNr);

  int species = -1;
  species = GetSpeciesIntFromRunInfo(it->second);
  float beamE = it->second.energy;
  std::cout << "Beam energy:" << beamE << std::endl;
  if (species == -1){
      std::cout << "WARNING: species unknown: " << it->second.species.Data() << "  aborting"<< std::endl;
      return false;
  }
  
  // create HG and LG histo's per channel
  TH1D* hDeltaTime = new TH1D("hDeltaTime", "Time Difference between Events; Delta Time (#mus); Counts", 2000, 0, 100000);
  TH2D* hspectraHGCorrvsCellID      = new TH2D( "hspectraHGCorr_vsCellID","ADC spectrum High Gain corrected vs CellID; cell ID; ADC_{HG} (arb. units)  ; counts ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,-200,3800);
  hspectraHGCorrvsCellID->SetDirectory(0);
  TH2D* hspectraLGCorrvsCellID      = new TH2D( "hspectraLGCorr_vsCellID","ADC spectrum Low Gain corrected vs CellID; cell ID; ADC_{LG} (arb. units)  ; counts  ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,-200,3800);
  hspectraLGCorrvsCellID->SetDirectory(0);
  TH2D* hspectraEnergyvsCellID  = new TH2D( "hspectraEnergy_vsCellID","Energy vs CellID; cell ID; E (mip eq./tile)    ; counts",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 8000,0,250);
  hspectraEnergyvsCellID->SetDirectory(0);
  
  std::map<int,TileSpectra> hSpectra;
  std::map<int, TileSpectra>::iterator ithSpectra;

  RootOutputHist->mkdir("IndividualCells");
  
  std::cout << "starting to run QA"<< std::endl;
  TcalibIn->GetEntry(0);
  int actCh1st = 0;
  double averageScale = calib.GetAverageScaleHigh(actCh1st);
  std::cout << "average HG mip: " << averageScale << "\t active ch: "<< actCh1st<< std::endl;
  
  int evts=TdataIn->GetEntries();
  double last_time = 0;
  double DeltaTime = 1000000000;
  
  
  if(percentmax!=100 || percentmin!=0){
    std::vector<double> deltat;
    deltat.reserve(evts);
    // Percentile calculations
    for (int i=0; i<evts; i++){
      TdataIn->GetEntry(i);
      
      double current_time = event.GetTimeStamp();
      if(last_time != 0){
        DeltaTime = current_time - last_time;
      }
      if (DeltaTime != 1000000000){
        deltat.push_back(DeltaTime);
      }
      last_time = current_time;
    }
    std::sort(deltat.begin(), deltat.end());
    int eventmax = static_cast<int>(std::round(percentmax*evts/100.0));
    int eventmin = static_cast<int>(std::round(percentmin*evts/100.0));
    std::cout << "eventmax: " << eventmax <<std::endl;
    std::cout <<"eventmin: " << eventmin << std::endl;
    if (percentmax != 100) timemax = deltat[eventmax-2];
    if (percentmin != 0) timemin = deltat[eventmin-1];
    std::cout << "timemax: " << timemax <<std::endl;
    std::cout <<"timemin: " << timemin << std::endl;
  }

  int rejected = 0;
  std::cerr<<"Run Number: " << runNr<< std::endl;
  std::cerr<< "Number of total evts: " << evts <<std::endl;
  // Event loop
  for(int i=0; i<evts; i++){
    TdataIn->GetEntry(i);
    
    // Find and fill delta time
    double current_time = event.GetTimeStamp();
    if(last_time != 0){
      DeltaTime = current_time - last_time;
      if (debug == 1000){
        std::cerr<< "current timestamp: "<< current_time<<std::endl;
      }
    }
    if (DeltaTime == 0){
      if(debug == 1001){
        std::cerr<< "Run Number: " << runNr <<"      Event Number: "<< i << std::endl;
        std::cerr<< "Previous Timestamp (us): "<< last_time <<"      Current Timestamp: "<< current_time<<std::endl;
      }
    }
        //cut based on DeltaTime range
    if(DeltaTime < timemin || DeltaTime > timemax){
      rejected++;
      last_time = current_time;
      continue;
    }
    hDeltaTime->Fill(DeltaTime);
    last_time = current_time;
    
    if (i%5000 == 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      Caen* aTile=(Caen*)event.GetTile(j);
      double corrHG = aTile->GetADCHigh()-calib.GetPedestalMeanH(aTile->GetCellID());
      double corrLG = aTile->GetADCLow()-calib.GetPedestalMeanL(aTile->GetCellID());
      hspectraHGCorrvsCellID->Fill(aTile->GetCellID(), corrHG);
      hspectraLGCorrvsCellID->Fill(aTile->GetCellID(), corrLG);
      if(aTile->GetE()!=0 ){ 
        hspectraEnergyvsCellID->Fill(aTile->GetCellID(), aTile->GetE());
      }      
      ithSpectra=hSpectra.find(aTile->GetCellID());
      if (ithSpectra!=hSpectra.end()){
        ithSpectra->second.FillSpectraCAEN(corrLG,corrHG);
        ithSpectra->second.FillTrigger(aTile->GetLocalTriggerPrimitive());;
        ithSpectra->second.FillCorrCAEN(corrLG,corrHG);
      } else {
        RootOutputHist->cd("IndividualCells");
        hSpectra[aTile->GetCellID()]=TileSpectra("AllTriggers",aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
        hSpectra[aTile->GetCellID()].FillSpectraCAEN(corrLG,corrHG);;
        hSpectra[aTile->GetCellID()].FillTrigger(aTile->GetLocalTriggerPrimitive());;
        hSpectra[aTile->GetCellID()].FillCorrCAEN(corrLG,corrHG);
      }
    }
  }
  if (debug == 1000){
    std::cerr<< "Number rejected: " << rejected <<std::endl;
    std::cerr<< "Number accepted: " << evts - rejected << std::endl;
  }
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }
  
  RootInput->Close();      
  
  RootOutputHist->cd();

  hspectraHGCorrvsCellID->Write();
  hspectraLGCorrvsCellID->Write();
  hspectraEnergyvsCellID->Write();

  hDeltaTime->Write();

  RootOutputHist->cd("IndividualCells");
  for(ithSpectra=hSpectra.begin(); ithSpectra!=hSpectra.end(); ++ithSpectra){
    ithSpectra->second.Write(false);
  }
  
  //**********************************************************************
  //********************* Plotting ***************************************
  //**********************************************************************  
  TString outputDirPlots = GetPlotOutputDir();
  gSystem->Exec("mkdir -p "+outputDirPlots);
  
  //**********************************************************************
  // Create canvases for channel overview plotting
  //**********************************************************************
  Double_t textSizeRel = 0.035;
  StyleSettingsBasics("pdf");
  SetPlotStyle();
  
  TCanvas* canvasDeltaTime = new TCanvas("canvasDeltaTime","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvasDeltaTime, 0.08, 0.07, 0.01, 0.07);
  canvasDeltaTime->SetLogy(1);
  
  if(DeltaTimePlot>0){
    PlotSimple1D( canvasDeltaTime, hDeltaTime, -10000, timemax, textSizeRel, Form("%s/deltaTime_%.0f_%.0f.%s", outputDirPlots.Data(),timemin, timemax, plotSuffix.Data()), it->second, 1);
  } 
  
  TCanvas* canvas2DCorr = new TCanvas("canvasCorrPlots","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas2DCorr, 0.08, 0.13, 0.045, 0.07);
  canvas2DCorr->SetLogz(1);
  TCanvas* canvas2DCorrWOLine = new TCanvas("canvasCorrPlotsWoLine","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas2DCorrWOLine, 0.08, 0.13, 0.01, 0.07);
  canvas2DCorrWOLine->SetLogz(1);
  PlotSimple2D( canvas2DCorr, hspectraHGCorrvsCellID, -10000, -10000, textSizeRel, Form("%s/HGCorr_%.0f_%.0f.%s", outputDirPlots.Data(),timemin, timemax, plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraLGCorrvsCellID, -10000, -10000, textSizeRel, Form("%s/LGCorr_%.0f_%.0f.%s", outputDirPlots.Data(),timemin, timemax, plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraEnergyvsCellID, -10000, -10000, textSizeRel, Form("%s/EnergyVsCellID_%.0f_%.0f.%s", outputDirPlots.Data(),timemin, timemax, plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
    
  if (ExtPlot > 0){
    gSystem->Exec("mkdir -p "+outputDirPlots+"/detailed");
    //***********************************************************************************************************
    //********************************* 8 Panel overview plot  **************************************************
    //***********************************************************************************************************
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
    TCanvas* canvas8Panel;
    TPad* pad8Panel[8];
    Double_t topRCornerX[8];
    Double_t topRCornerY[8];
    Int_t textSizePixel = 30;
    Double_t relSize8P[8];
    CreateCanvasAndPadsFor8PannelTBPlot(canvas8Panel, pad8Panel,  topRCornerX, topRCornerY, relSize8P, textSizePixel);
  
    TCanvas* canvas8PanelProf;
    TPad* pad8PanelProf[8];
    Double_t topRCornerXProf[8];
    Double_t topRCornerYProf[8];
    Double_t relSize8PProf[8];
    CreateCanvasAndPadsFor8PannelTBPlot(canvas8PanelProf, pad8PanelProf,  topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 0.045, "Prof", false);

    
    calib.PrintGlobalInfo();  
    Double_t maxHG = ReturnMipPlotRangeDepVov(calib.GetVov(),true, ReadOut::Type::Caen);
    Double_t maxLG = 3800;
    std::cout << "plotting single layers" << std::endl;
    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
      if (l%10 == 0 && l > 0 && debug > 0){
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;
      }
      PlotNoiseWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                hSpectra, 0, 0, maxHG, 1.2, l, 0,
                                Form("%s/detailed/Spectra_HG_Layer%02d_%.0f_%.0f.%s" ,outputDirPlots.Data(), l, timemin, timemax, plotSuffix.Data()), it->second);
      PlotNoiseWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                hSpectra, 1, 0, maxLG, 1.2, l, 0,
                                Form("%s/detailed/Spectra_LG_Layer%02d_%.0f_%.0f.%s" ,outputDirPlots.Data(), l, timemin, timemax, plotSuffix.Data()), it->second);
      
      PlotCorrWithFits8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 
                                    hSpectra, 0, -20, 340, 3800, l, 0,
                                    Form("%s/detailed/LGHG_Corr_Layer%02d_%.0f_%.0f.%s" ,outputDirPlots.Data(), l, timemin, timemax, plotSuffix.Data()), it->second);
    }
    std::cout << "done plotting" << std::endl;
  }
  RootOutputHist->Close();
  return true;
}

//***********************************************************************************************
//*********************** Create output files ***************************************************
//***********************************************************************************************
bool DataAnalysis::CreateOutputRootFile(void){
  if(Overwrite){
    RootOutput=new TFile(RootOutputName.Data(),"RECREATE");
  } else{
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
bool DataAnalysis::CreateOutputRootFileHist(void){
  if(Overwrite){
    RootOutputHist=new TFile(RootOutputNameHist.Data(),"RECREATE");
  } else{
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"CREATE");
  }
  if(RootOutputHist->IsZombie()){
    std::cout<<"Error opening '"<<RootOutputHist<<"'no reachable path? Exist without force mode to overwrite?..."<<std::endl;
    return false;
  }
  return true;
}
