#include "EventDisplay.h"
#include <vector>
#include "TROOT.h"
//#include <unistd.h> // Add for use on Mac OS -> Same goes for Analyse.cc
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3F.h"
#include "TPaletteAxis.h"
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
bool EventDisplay::CheckAndOpenIO(void){
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
      return false;
    }
    else {
      matchingbranch=TcalibIn->SetBranchAddress("calib",&calibptr);
      if(matchingbranch<0){
        std::cout<<"Error retrieving calibration info from the tree"<<std::endl;
        TcalibIn=nullptr;
      } else {
        std::cout<<"Loaded the calib object"<<std::endl;
      }
    }
  } else {
    std::cout<<"An input file is required, aborting"<<std::endl;
    return false;
  }  

  return true;
}

// ****************************************************************************
// Primary process function to start all calibrations
// ****************************************************************************
bool EventDisplay::Process(void){
  bool status;
  ROOT::EnableImplicitMT();
  
  // copy full calibration to different file and calculate energy
  status=Plot();

  return status;
}


//***********************************************************************************************
//*********************** Plotting routine ***************************************************
//***********************************************************************************************
bool EventDisplay::Plot(){
  std::cout<<"Plotting events "<<plotEvt<<" to " << plotEvt+nEvts-1<<std::endl;
  if(plotMuonEvts) std::cout<<"Plotting muon-triggered events"<<std::endl;

  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  TdataIn->GetEntry(0);
  Int_t runNr = event.GetRunNumber();
  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find(runNr);

  // load calib
  TcalibIn->GetEntry(0);
  
  int species = -1;
  species = GetSpeciesIntFromRunInfo(it->second);
  if (species == -1){
      std::cout << "WARNING: species unknown: " << it->second.species.Data() << "  aborting"<< std::endl;
      return false;
  }
  
  std::cout << "debug level set to : " << debug << std::endl;
  
  // create 3D histo
  // int towersx         = setup->GetNMaxColumn()+1;/*GetNMAxColumn() returns */
  float towersx_min   = setup->GetMinX();
  float towersx_max   = setup->GetMaxX();
  int towersx         = (towersx_max-towersx_min)/setup->GetCellWidth();
  float towersy_min   = setup->GetMinY();
  float towersy_max   = setup->GetMaxY();
  int towersy         = (towersy_max-towersy_min)/setup->GetCellHeight();
  float towersz_min   = setup->GetMinZ();
  float towersz_max   = setup->GetMaxZ();
  int towersz         = (towersz_max-towersz_min)/setup->GetCellDepth();

  TH3F*   hXYZMapEvt          = new TH3F("hXYZMapEvt","",towersz, towersz_min, towersz_max, towersx, towersx_min, towersx_max, towersy, towersy_min, towersy_max);
  TH3F*   hXYZMapEvt_Muon     = new TH3F("hXYZMapEvt_Muon","",towersz, towersz_min, towersz_max, towersx, towersx_min, towersx_max, towersy, towersy_min, towersy_max);
  TH3F*   hXYZMapEvt_nonMuon  = new TH3F("hXYZMapEvt_nonMuon","",towersz, towersz_min, towersz_max, towersx, towersx_min, towersx_max, towersy, towersy_min, towersy_max);

  TH1D* hX_energy_Evt    = new TH1D("hXenergyEvt","",towersx, towersx_min, towersx_max);
  hX_energy_Evt->Sumw2();
  TH1D* hY_energy_Evt    = new TH1D("hYenergyEvt","",towersy, towersy_min, towersy_max);
  hY_energy_Evt->Sumw2();
  TH1D* hZ_energy_Evt    = new TH1D("hZenergyEvt","",towersz, towersz_min, towersz_max);
  hZ_energy_Evt->Sumw2();

  TH1D* hX_energy_Evt_Muon    = new TH1D("hXenergyEvt_muon","",towersx, towersx_min, towersx_max);
  hX_energy_Evt_Muon->Sumw2();
  TH1D* hY_energy_Evt_Muon    = new TH1D("hYenergyEvt_muon","",towersy, towersy_min, towersy_max);
  hY_energy_Evt_Muon->Sumw2();
  TH1D* hZ_energy_Evt_Muon    = new TH1D("hZenergyEvt_muon","",towersz, towersz_min, towersz_max);
  hZ_energy_Evt_Muon->Sumw2();

  TH1D* hX_energy_Evt_nonMuon    = new TH1D("hXenergyEvt_nonMuon","",towersx, towersx_min, towersx_max);
  hX_energy_Evt_nonMuon->Sumw2();
  TH1D* hY_energy_Evt_nonMuon    = new TH1D("hYenergyEvt_nonMuon","",towersy, towersy_min, towersy_max);
  hY_energy_Evt_nonMuon->Sumw2();
  TH1D* hZ_energy_Evt_nonMuon    = new TH1D("hZenergyEvt_nonMuon","",towersz, towersz_min, towersz_max);
  hZ_energy_Evt_nonMuon->Sumw2();  

  // creating plotting directory
  StyleSettingsBasics(plotSuffix);
  TString outputDirPlots = GetPlotOutputDir();
  gSystem->Exec("mkdir -p "+outputDirPlots);
  
  TCanvas* canvas3D = new TCanvas("canvas3D","",0,0,1400,750);  // gives the page size
  DefaultCanvasSettings( canvas3D, 0.12, 0.08, 0.05, 0.1);
  // processing events
  int evts=TdataIn->GetEntries();
  int evtsMuon= 0;
  
  // print calib info
  calib.PrintGlobalInfo();
  double averagePedMeanHG = calib.GetAveragePedestalMeanHigh();
  double averagePedSigHG  = calib.GetAveragePedestalSigHigh();
  double averagePedMeanLG = calib.GetAveragePedestalMeanLow();
  double averagePedSigLG  = calib.GetAveragePedestalSigLow();
  std::cout<< averagePedMeanHG << "\t" << averagePedSigHG << "\t" << averagePedMeanLG << "\t" << averagePedSigLG << std::endl;

  Double_t eCutoff = 0.3;
  if (plotHGCROC && dataTypeHGCROC==0)
    eCutoff = calib.GetAveragePedestalSigHigh()*5.;
  std::cout << "setting energy cut-off to: " << eCutoff << std::endl;
  
  for(int i=plotEvt; i<plotEvt+nEvts; i++){
    if(i > evts ) {
      std::cout<<"Requested event outside of the range. Aborting..."<<std::endl;
      return false;
    }
    TdataIn->GetEntry(i);
    if (i%1 == 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    double Etot = 0;
    int nCells  = 0;
    bool muontrigg = false;
    int locMuon = 0;
    std::map<int,Layer> layers;
    std::map<int, Layer>::iterator ithLayer;
    
    if( debug>1 ) std::cout << "Event has " << event.GetNTiles() << " active tiles" << std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      
      if(plotHGCROC){
        Hgcroc* aTile = (Hgcroc*)event.GetTile(j);
        // no "energy" threshold for now - just plot everything
        nCells++;
        int currLayer   = setup->GetLayer(aTile->GetCellID());
        ithLayer        = layers.find(currLayer);
        double energy = 0;
        switch( dataTypeHGCROC ){
          case 0:
            energy = aTile->GetIntegratedADC(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t ADC: " << aTile->GetIntegratedADC() << "\t energy " << energy << std::endl;
            break;
          case 1:
            energy = aTile->GetMaxSampleADC() - aTile->GetPedestal(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t ADC: " << aTile->GetIntegratedADC() << "\t pedestal (calib) " << calib.GetPedestalMeanH( aTile->GetCellID() ) << "\t pedestal (aTile) " << aTile->GetPedestal() << "\t energy " << energy << std::endl;
            break;
          case 2:
            energy = aTile->GetTOT(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t TOT: " << aTile->GetTOT() << "\t energy " << energy << std::endl;
            break;
          case 3:
            energy = aTile->GetIntegratedADC()/ calib.GetScaleHigh(aTile->GetCellID()); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t ADC: " << aTile->GetIntegratedADC() << "\t scale (calib) " << calib.GetScaleHigh( aTile->GetCellID() ) << "\t energy " << energy << std::endl;
            break;
          case 4:
            energy = aTile->GetE(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t E: " << aTile->GetE() << std::endl;
            break;
        }

        if (dataTypeHGCROC == 4 && energy < energy) continue;
        
        if(ithLayer!=layers.end()){
          ithLayer->second.nCells+=1;
          ithLayer->second.energy+=energy;
          ithLayer->second.avX+=setup->GetX(aTile->GetCellID())*energy;
          ithLayer->second.avY+=setup->GetY(aTile->GetCellID())*energy;
        } else {
          layers[currLayer]=Layer();
          layers[currLayer].nCells+=1;
          layers[currLayer].energy+=energy;
          layers[currLayer].avX+=setup->GetX(aTile->GetCellID())*energy;
          layers[currLayer].avY+=setup->GetY(aTile->GetCellID())*energy;
        }
      } else {
        Caen* aTile=(Caen*)event.GetTile(j);
        if(aTile->GetE()>eCutoff ){ 
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
        if (aTile->GetLocalTriggerBit() == 1){
          locMuon++;        
        }      
      }
    }
    std::cout << "NCells hit: "  << nCells << "\t" << minTilesHit << std::endl;
    
    if (nCells > minTilesHit) {
      int nLayerSingleCell = 0;
      for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
        if (ithLayer->second.nCells == 1) 
            nLayerSingleCell++;
      }
      double fracLayer1Cell = (double)nLayerSingleCell/(int)layers.size();
      double fracLocMuon = (double)locMuon/nCells;
      if (fracLocMuon > 0.6){ 
        muontrigg = true;
        evtsMuon++;
      }
      if (muontrigg && debug > 1){
          std::cout << "Event: " << i << "\tMuon triggered:\t" <<  fracLayer1Cell << "\t" << fracLocMuon << std::endl;
      }
      Float_t minE = 1e10;
      Float_t maxE = 0;
      for(int j=0; j<event.GetNTiles(); j++){
        double energy  = 0;
        double x, y, z; 
        unsigned char muonTrigger = 0;
        if(plotHGCROC){
          Hgcroc* aTile   = (Hgcroc*)event.GetTile(j);
          if( calib.GetBadChannel(aTile->GetCellID())!=3 && calib.GetBCCalib() == true) continue;
          switch( dataTypeHGCROC ){
            case 0:
              energy = aTile->GetIntegratedADC();
              break;
            case 1:
              energy = aTile->GetIntegratedADC()- calib.GetPedestalMeanL(aTile->GetCellID());
              break;
            case 2:
              energy = aTile->GetTOT();
              break;
            case 3:
              energy = aTile->GetIntegratedADC()/calib.GetScaleHigh(aTile->GetCellID());
              break;
            case 4:
              energy = aTile->GetE();
              break;
          }
          x = (double) aTile->GetX();
          y = (double) aTile->GetY();
          z = (double) aTile->GetZ();
          if( debug>2 ) std::cout << "CellID: " << aTile->GetCellID() << "\t value: " << energy  << "\tx,y,z: " <<x<< "\t" <<y<< "\t" << z << std::endl;
          muonTrigger = (unsigned char)aTile->GetLocalTriggerBit();
        } else {
          Caen* aTile=(Caen*)event.GetTile(j);
          if( calib.GetBadChannel(aTile->GetCellID())!=3) continue; 
          energy = (Float_t)aTile->GetE();
          x = (double) aTile->GetX();
          y = (double) aTile->GetY();
          z = (double) aTile->GetZ();
          muonTrigger = (unsigned char)aTile->GetLocalTriggerBit();
        }
        Etot+=energy;
        if (energy < minE) minE = energy;
        if (energy > maxE) maxE = energy;
        
        if(energy>eCutoff){ 
          hXYZMapEvt->Fill(z,x,y,energy);
          hX_energy_Evt->Fill(x, energy);
          hY_energy_Evt->Fill(y, energy);
          hZ_energy_Evt->Fill(z, energy);
          if(debug > 2) std::cout << "Event, x, y, z, E: " << i << "\t" <<x<< "\t" <<y<< "\t" << z<< "\t" <<energy<<std::endl;
          if (muonTrigger == 1){
            hXYZMapEvt_Muon->Fill(z,x,y,energy);
            hX_energy_Evt_Muon->Fill(x, energy);
            hY_energy_Evt_Muon->Fill(y, energy);
            hZ_energy_Evt_Muon->Fill(z, energy);
          } else {
            hXYZMapEvt_nonMuon->Fill(z,x,y,energy);
            hX_energy_Evt_nonMuon->Fill(x, energy);
            hY_energy_Evt_nonMuon->Fill(y, energy);
            hZ_energy_Evt_nonMuon->Fill(z, energy);
          }
        } 
      }
      std::cout << "Etot: "  << Etot << std::endl;

      //**********************************************************************
      //********************* Plotting ***************************************
      //**********************************************************************  
      Float_t maxEX = 0;
      Float_t maxEY = 0;
      Float_t maxEZ = 0;
      for (Int_t k = 1; k < hX_energy_Evt->GetNbinsX()+1; k++){
        if (maxEX < hX_energy_Evt->GetBinContent(k)) maxEX = hX_energy_Evt->GetBinContent(k);
      }
      for (Int_t k = 1; k < hY_energy_Evt->GetNbinsX()+1; k++){
        if (maxEY < hY_energy_Evt->GetBinContent(k)) maxEY = hY_energy_Evt->GetBinContent(k);
      }
      for (Int_t k = 1; k < hZ_energy_Evt->GetNbinsX()+1; k++){
        if (maxEZ < hZ_energy_Evt->GetBinContent(k)) maxEZ = hZ_energy_Evt->GetBinContent(k);
      }
      //**********************************************************************
      // Create canvases for channel overview plotting
      //**********************************************************************
      Double_t textSizeRel = 0.035;
      StyleSettingsBasics("pdf");
      SetPlotStyle();
      TString unit = "mip eq/tile";
      if (plotHGCROC){
        switch( dataTypeHGCROC ){
            case 0:
            case 1:
              unit = "ADC units";
              break;
            case 2:
              unit = "TOT units";
              break;
          }
      }
      
      if (Etot > 0){
        if( (muontrigg&&plotMuonEvts) || !plotMuonEvts){
          EventDisplayWithSliceHighlighted( hXYZMapEvt, hX_energy_Evt, hY_energy_Evt, hZ_energy_Evt, 
                                          hXYZMapEvt_Muon, hX_energy_Evt_Muon, hY_energy_Evt_Muon, hZ_energy_Evt_Muon, 
                                          hXYZMapEvt_nonMuon, hX_energy_Evt_nonMuon, hY_energy_Evt_nonMuon, hZ_energy_Evt_nonMuon, 
                                          i, Etot, maxE, maxEX, maxEY, maxEZ,  muontrigg,
                                          it->second, Form("%s/EventDisplay_muonHighlighed_evt", outputDirPlots.Data()), plotSuffix, unit);    
        }

        if( (muontrigg&&plotMuonEvts) || !plotMuonEvts){
          EventDisplayWithSlice(  hXYZMapEvt, hX_energy_Evt, hY_energy_Evt, hZ_energy_Evt, 
                                  i, Etot, maxE, maxEX, maxEY, maxEZ,  muontrigg,
                                  it->second, Form("%s/EventDisplay_MonoChrome_evt", outputDirPlots.Data()), plotSuffix, unit);    

          canvas3D->cd();

          SetStyleHistoTH3ForGraphs(hXYZMapEvt, "z", "x","y", 0.85*textSizeRel,textSizeRel, 0.85*textSizeRel,textSizeRel, 0.85*textSizeRel,textSizeRel, 1.1, 1.1, 1.15, 505, 510,510);
          hXYZMapEvt->SetMaximum(maxE);
          hXYZMapEvt->DrawCopy("box2z");
          DrawLatex(0.05, 0.94, GetStringFromRunInfo(it->second, 1), false, 0.85*textSizeRel, 42);
          if(muontrigg) DrawLatex(0.05, 0.90, Form("Event %d, muon triggered",i), false, 0.85*textSizeRel, 42);
          else DrawLatex(0.05, 0.90, Form("Event %d",i), false, 0.85*textSizeRel, 42);
          
          canvas3D->SaveAs( Form("%s/EventDisplay_Colored_evt%06i.%s", outputDirPlots.Data(), i, plotSuffix.Data()));
          canvas3D->ResetDrawn();
        }
      }
    }

    hXYZMapEvt->Reset();
    hXYZMapEvt_Muon->Reset();
    hXYZMapEvt_nonMuon->Reset();
    hX_energy_Evt->Reset();
    hY_energy_Evt->Reset();
    hZ_energy_Evt->Reset();
    hX_energy_Evt_Muon->Reset();
    hY_energy_Evt_Muon->Reset();
    hZ_energy_Evt_Muon->Reset();
    hX_energy_Evt_nonMuon->Reset();
    hY_energy_Evt_nonMuon->Reset();
    hZ_energy_Evt_nonMuon->Reset();
  }

  delete hXYZMapEvt;
  delete hXYZMapEvt_Muon;
  delete hXYZMapEvt_nonMuon;
  delete hX_energy_Evt;
  delete hY_energy_Evt;
  delete hZ_energy_Evt;
  delete hX_energy_Evt_Muon;
  delete hY_energy_Evt_Muon;
  delete hZ_energy_Evt_Muon;
  delete hX_energy_Evt_nonMuon;
  delete hY_energy_Evt_nonMuon;
  delete hZ_energy_Evt_nonMuon;
  delete canvas3D;
  
  return true;
}
