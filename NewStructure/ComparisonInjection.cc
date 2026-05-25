#include "ComparisonInjection.h"
#include <vector>
#include "TROOT.h"
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
#include "TileSpectra.h"
#include "TileTrend.h"
#include "CalibSummary.h"
#include "MultiCanvas.h"
#include "CommonHelperFunctions.h"
#include "PlotHelper.h"


// ===========================================================================================
// Check input files and global settings
// ===========================================================================================
bool ComparisonInjection::CheckAndOpenIO(void){
  
  int matchingbranch;
  
  // *****************************************************************************************
  // Reading files from a text file
  // *****************************************************************************************
  if(!InputListName.IsNull()){
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // text file with 2 files per line 1 full file & 1 histo file
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    std::cout << "You need to provide data tree file with the setup & calib from the injection runs & a histo file" << std::endl;
    std::fstream dummyTxt;
    dummyTxt.open(InputListName.Data(),std::ios::in);
    if(!dummyTxt.is_open()){
      std::cout<<"Error opening "<<InputListName.Data()<<", does the file exist?"<<std::endl;
    }
    std::string dummyRootCalibName;
    std::string dummyRootHistName;
    // set first root file names
    dummyTxt>>dummyRootCalibName >> dummyRootHistName;
    
    int goodcalib;
    int goodsetup;
    while(dummyTxt.good()){
      std::cout << dummyRootCalibName.data() << "\t" << dummyRootHistName.data() << std::endl;
      
      // check that files exist and can be opened
      TFile dummyRootCalib=TFile(dummyRootCalibName.c_str(),"READ");
      if(dummyRootCalib.IsZombie()){
        std::cout<<"Error opening '"<<dummyRootCalibName<<", does the file exist?"<<std::endl;
        return false;
      }
      dummyRootCalib.Close();
      TFile dummyRootHist=TFile(dummyRootHistName.c_str(),"READ");
      if(dummyRootHist.IsZombie()){
        std::cout<<"Error opening '"<<dummyRootHistName<<", does the file exist?"<<std::endl;
        return false;
      }
      dummyRootHist.Close();
      
      // Add file-name to setup and calib chain as well string-vector
      AddInputFile(dummyRootHistName);
      goodsetup=TsetupIn->AddFile(dummyRootCalibName.c_str());
      goodcalib=TcalibIn->AddFile(dummyRootCalibName.c_str());
      if(goodcalib==0){
          std::cout<<"Issues retrieving Calib tree from "<<dummyRootCalibName<<", file is ignored"<<std::endl;
        }
      if(goodsetup==0){
        std::cout<<"Issues retrieving Setup tree from "<<dummyRootCalibName<<", file is ignored"<<std::endl;
      }
      // set next root file names
      dummyTxt>>dummyRootCalibName >> dummyRootHistName;
    }
  }
  // *****************************************************************************************
  // Setup Output files
  // *****************************************************************************************
  if(RootOutputName.IsNull()){
    return false;
  } else {
    if(!CreateOutputRootFile()){
      return false;
    }
  }

  // *****************************************************************************************
  // Setup TChain of setups and calibrations
  // *****************************************************************************************
  // intialize global variable setup
  setup=Setup::GetInstance();
  std::cout<<"Setup add "<<setup<<std::endl;
  matchingbranch=TsetupIn->SetBranchAddress("setup",&rswptr);
  if(matchingbranch<0){
    std::cout<<"Error retrieving Setup info from the tree"<<std::endl;
    return false;
  }
  std::cout<<"Entries "<<TsetupIn->GetEntries()<<std::endl;
  TsetupIn->GetEntry(0);
  setup->Initialize(*rswptr);
  // initialize calib with the correct branch
  matchingbranch=TcalibIn->SetBranchAddress("calib",&calibptr);
  if(matchingbranch<0){
    std::cout<<"Error retrieving calibration info from the tree"<<std::endl;
    return false;
  }
  
  return true;    
}

// ===========================================================================================
// Main function of this calibration comparison 
// ===========================================================================================
bool ComparisonInjection::ProcessInjectionCompare(void){
  // *****************************************************************************************
  // plotting settings
  // *****************************************************************************************
  gSystem->Exec("mkdir -p "+OutputNameDirPlots);
  if (ExtPlot > 0) gSystem->Exec("mkdir -p "+OutputNameDirPlots+"/SingleLayer");
  StyleSettingsBasics("pdf");
  SetPlotStyle();  

  // *****************************************************************************************
  // Some general setup
  // *****************************************************************************************
  bool status=true;
  // enbale implitcit root multithreading
  ROOT::EnableImplicitMT();
  // get nuber of entires from Calib tree (how many runs do we have)
  int entries=TcalibIn->GetEntries();
  std::cout << "Entries in calib tree: " << entries << std::endl;
  
  // *****************************************************************************************
  // global variable setup, common iterators and ranges
  // ******************************************************************************************
  std::map<int, TileTrend> trend;
  std::map<int, TileTrend>::iterator itrend;
  std::map<int, TileCalib>::const_iterator itcalib;

  std::map<int, CalibSummary> sumCalibs;
  std::map<int, CalibSummary>::iterator isumCalibs;
  
  double Xvalue;
  double Xmin= 9999.;
  double Xmax=-9999.;
  int nRun = 0;
  
  // ******************************************************************************************
  // ************* Get run data base to potentially obtain more information from file *********
  // ******************************************************************************************
  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  std::map<int,RunInfo>::iterator itRun; // basic infos
  int firstRunNr    = -1;
  
  // ******************************************************************************************
  // Iterate over all entries (runs) in the calib tree
  // ******************************************************************************************
  for(int ientry=0; ientry<entries;ientry++){
    TsetupIn->GetEntry(ientry);
    TcalibIn->GetEntry(ientry);
    
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // set global iterator for runs to first run number in list to obtain beam-line, dates...
    if (ientry==0) firstRunNr = calib.GetRunNumber();
    itRun = ri.find(calib.GetRunNumber());
    double set_rf     = (double)itRun->second.rf;
    double set_cf     = (double)itRun->second.cf;
    double set_cc     = (double)itRun->second.cc;
    double set_cfcomp = (double)itRun->second.cfcomp;
    std::cout <<calib.GetRunNumber() << "\t" << set_rf << "\t" << set_cf << "\t" << set_cc << "\t" << set_cfcomp << std::endl;
    
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Set X-values according to option
    // Xaxis:   
    //        0 - Run number dependence
    //        1 - Operational Voltage dependence
    //        2 - Time dependence
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    Int_t runNumber = calib.GetRunNumber();
    Xvalue=calib.GetRunNumber();
    if(Xvalue<Xmin) Xmin=Xvalue;
    if(Xvalue>Xmax) Xmax=Xvalue;

    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Initialize calib summary
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    calib.PrintGlobalInfo();
    CalibSummary aSum = CalibSummary(nRun, runNumber,calib.GetVop());
    
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Reading additional summary histos from 2nd file
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    TFile* tempFile = nullptr;    
    if (nRun < (int)RootInputNames.size()){
      std::cout << "reading hist file: " <<RootInputNames[nRun].Data() << " expanded list setting: " << expandedList << std::endl;
      tempFile      = new TFile(RootInputNames[nRun].Data(),"READ");
    }
    
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Loop over all cells in the calib object for trending plots
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    
    for(itcalib=calib.begin(); itcalib!=calib.end(); ++itcalib){

      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      // Reading additional cell histos from 2nd file
      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      
      TProfile* profCellWave= nullptr;
      TProfile* profCellTOA= nullptr;
      TProfile* profCellTOT= nullptr;
      // std::cout << "celld id: " << itcalib->first << "\t "<< calib.GetPedestalMeanH(itcalib->first);
      // reading Waveform
      profCellWave     = (TProfile*)tempFile->Get(Form("IndividualCells/waveform1DinjCellID%i",itcalib->first));
      // reading ToA
      profCellTOA      = (TProfile*)tempFile->Get(Form("IndividualCells/TOAinjCellID%i",itcalib->first));
      // reading ToT
      profCellTOT      = (TProfile*)tempFile->Get(Form("IndividualCells/TOTinjCellID%i",itcalib->first));
      // std::cout <<"\t Wave: "<< profCellWave << "\t TOA: " << profCellTOA << "\t TOT " << profCellTOT << std::endl;
      
      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      // fill calib summary object for specific cell
      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      aSum.Fill(itcalib->second);
      
      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      // fill trending object for a single cell
      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      // check if iterator points to end of map
      itrend=trend.find(itcalib->first);
      if(itrend!=trend.end()){
        // fill injection hists
        itrend->second.Fill(Xvalue,itcalib->second, (int)calib.GetRunNumber(), (double)calib.GetVop());
        itrend->second.FillInjection(Xvalue,calib.GetPedestalMeanH(itcalib->first),(int)calib.GetRunNumber(), 
                                     profCellWave, profCellTOA, profCellTOT, 
                                     set_rf, set_cf, set_cfcomp, set_cc );
      // create new TileTrend object if not yet available in map
      } else {
        TileTrend atrend=TileTrend(itcalib->first,0, 3);
        // fill injection hists
        atrend.Fill(Xvalue,itcalib->second, (int)calib.GetRunNumber(), (double)calib.GetVop());
        atrend.FillInjection(Xvalue,calib.GetPedestalMeanH(itcalib->first),(int)calib.GetRunNumber(), 
                             profCellWave, profCellTOA, profCellTOT, 
                             set_rf, set_cf, set_cfcomp, set_cc);
        // append TileTrend object to map
        trend[itcalib->first]=atrend;
      }
    } // end loop over cells in the calib object
    
    // append CalibSummary object to map
    sumCalibs[nRun]=aSum;
    // close additional files opened
    if (expandedList){
      tempFile->Close(); 
    }
    // increase run-counter
    nRun++;
  } // end loop over entries (runs) in calib tree
  
  // ******************************************************************************************
  // Print summary of calib runs
  // ******************************************************************************************    
  std::cout << "Calibs summary: "<< sumCalibs.size() << std::endl;
  int globalStatus = 0;
  for(isumCalibs=sumCalibs.begin(); isumCalibs!=sumCalibs.end(); ++isumCalibs){
    int calibstatus = isumCalibs->second.Analyse(debug);
    if (globalStatus < calibstatus) globalStatus = calibstatus;
  }
  std::cout << "Global calib status: " << globalStatus << std::endl;
  
  if (globalStatus == 0){
    std::cout << "!!!!!!!!!!!!!!!!!!!!!  ATTENTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "Aborting plotting: none of the files has either ped or mip scales filled" << std::endl; 
  }
  
  // ******************************************************************************************
  // Set X axis title and ranges 
  // ******************************************************************************************
  if (Xaxis == 0){
    Xmin= Xmin-10;
    Xmax= Xmax+10;
  } 

  TString             xaxisTitle = "";
  if (Xaxis==0)       xaxisTitle = "Run Nr. ";
  
  for(itrend=trend.begin(); itrend!=trend.end(); ++itrend){    
    // sort graphs
    itrend->second.Sort();
    // set x axis title for trending graphs
    itrend->second.SetXAxisTitle(xaxisTitle);
    // write graphs for each cell to output
    itrend->second.Write(RootOutput);
  }

  itRun = ri.find(firstRunNr);
  
  bool isSameVoltage    = true;
  double commonVoltage  = 0;
  bool isSameRF         = true;
  bool isSameCF         = true;
  bool isSameCFcomp     = true;
  bool isSameCC         = true;
  double commonRF       = 0;
  double commonCF       = 0;
  double commonCFcomp   = 0;
  double commonCC       = 0;
  itrend=trend.begin();
  for (int rc = 0; rc < itrend->second.GetNRuns() && rc < 30; rc++ ){
    if (rc == 0){
      commonVoltage = itrend->second.GetVoltage(rc);
      commonRF      = itrend->second.GetRF(rc);
      commonCF      = itrend->second.GetCF(rc);
      commonCC      = itrend->second.GetCC(rc);
      commonCFcomp  = itrend->second.GetCFComp(rc);
    } else {
      if (commonVoltage != itrend->second.GetVoltage(rc))   isSameVoltage = false;
      if (commonRF != itrend->second.GetRF(rc))        isSameRF = false;
      if (commonCF != itrend->second.GetCF(rc))        isSameCF = false;
      if (commonCFcomp != itrend->second.GetCFComp(rc))    isSameCFcomp = false;
      if (commonCC != itrend->second.GetCC(rc))        isSameCC = false;
    }
    std::cout << itrend->second.GetRunNr(rc) << "\t" <<  itrend->second.GetRF(rc) << "\t" << itrend->second.GetCF(rc) << "\t" << itrend->second.GetCC(rc) << "\t" << itrend->second.GetCFComp(rc) << "\tRF calc:"<< ReturnRFValue(itrend->second.GetRF(rc))<< std::endl;
    
  }
  std::cout << "Show all common settings" << std::endl;
  std::cout << "Common Voltage: "<< isSameVoltage << "\t"  << commonVoltage
            << "\t RF: " << isSameRF << "\t"  << commonRF
            << "\t CF: " << isSameCF << "\t"  << commonCF 
            << "\t CFcomp: " << isSameCFcomp << "\t"  << commonCFcomp 
            << "\t CC: " << isSameCC << "\t"  << commonCC 
            << std::endl;
  
  RunInfo commonRunInfo;
  commonRunInfo.nFPGA       = itRun->second.nFPGA;
  commonRunInfo.nASIC       = itRun->second.nASIC;
  commonRunInfo.energy      = itRun->second.energy;
  commonRunInfo.pdg         = itRun->second.pdg;
  commonRunInfo.species     = itRun->second.species;
  commonRunInfo.beamline    = itRun->second.beamline;
  commonRunInfo.facility    = itRun->second.facility;
  commonRunInfo.readout     = itRun->second.readout;
  commonRunInfo.month       = itRun->second.month;
  commonRunInfo.year        = itRun->second.year;
  commonRunInfo.samples     = itRun->second.samples;
  commonRunInfo.samples     = itRun->second.vbr;
  if (isSameRF)
    commonRunInfo.rf        = commonRF;
  else 
    commonRunInfo.rf        = -10.;
  if (isSameCF)
    commonRunInfo.cf        = commonCF;
  else 
    commonRunInfo.cf        = -10.;
  if (isSameCFcomp)
    commonRunInfo.cfcomp    = commonCFcomp;
  else 
    commonRunInfo.cfcomp    = -10.;
  if (isSameCC)
    commonRunInfo.cc        = commonCC;
  else 
    commonRunInfo.cc        = -10.;
  if (isSameVoltage)
    commonRunInfo.vop       = commonVoltage;
  else 
    commonRunInfo.vop       = -10.;
  //******************************************************************************
  // plotting overview for each run overlayed
  //******************************************************************************
  Int_t textSizePixel   = 30;
  Float_t textSizeRel   = 0.04;  
  TCanvas* canvas1DRunsOverlay = new TCanvas("canvas1DRunsOverlay","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas1DRunsOverlay, 0.075, 0.015, 0.025, 0.09);

  PlotCalibRunOverlay( canvas1DRunsOverlay, 0, sumCalibs, textSizeRel, 
                      Form("%s/HGPedSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), commonRunInfo,"", debug);
  PlotCalibRunOverlay( canvas1DRunsOverlay, 1, sumCalibs, textSizeRel, 
                      Form("%s/HGPedWidthSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), commonRunInfo,"", debug);

  
  // plotting individual layers/asics
  DetConf::Type detConf = DetConf::Type::Asic;
  // DetConf::Type detConf = DetConf::Type::Single8M;
  // DetConf::Type detConf = DetConf::Type::Dual8M;
  MultiCanvas panelPlot2D(detConf, "Injection");
  bool init2D = panelPlot2D.Initialize(2);
  
  // panelPlot2D.PlotTrending(trend, 0, Xmin,Xmax, OutputNameDirPlots, "PedADC", plotSuffix, commonRunInfo, ExtPlot );
  panelPlot2D.PlotRunOverlayProfile(trend, nRun, 1, 0, 18*7, -10, -10000, OutputNameDirPlots, "WaveOverlay", plotSuffix, commonRunInfo, ExtPlot );
  // panelPlot2D.PlotRunOverlayProfile(trend, nRun, 3, 0, 18*7, 0,-10000, OutputNameDirPlots, "TOTOverlay", plotSuffix, commonRunInfo, ExtPlot );
  // panelPlot2D.PlotRunOverlayProfile(trend, nRun, 2, 0, 18*7, 0,1024, OutputNameDirPlots, "TOAOverlay", plotSuffix, commonRunInfo, ExtPlot );

  detConf = DetConf::Type::SingleTile;
  MultiCanvas panelSingleTile(detConf, "InjectionTile");
  bool initSngle = panelSingleTile.Initialize(1);
  
  panelSingleTile.PlotRunOverlayProfile(trend, nRun, 1, 0, 18*7, -10, -10000, OutputNameDirPlots, "TileWaveOverlay", plotSuffix, commonRunInfo, ExtPlot );
  return status;
}


// ===========================================================================================
// Create the output file 
// ===========================================================================================
bool ComparisonInjection::CreateOutputRootFile(void){
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
