#include "ComparisonWaveform.h"
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
bool ComparisonWaveform::CheckAndOpenIO(void){
  
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
bool ComparisonWaveform::ProcessWaveformCompare(void){
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
  
  std::vector<RunInfo> runList;
  
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
    runList.push_back(itRun->second);
    double set_rf     = (double)itRun->second.rf;
    double set_cf     = (double)itRun->second.cf;
    double set_cc     = (double)itRun->second.cc;
    double set_cfcomp = (double)itRun->second.cfcomp;
    double set_injec  = -10000.;
    std::cout <<calib.GetRunNumber() << "\t" << set_rf << "\t" << set_cf << "\t" << set_cc << "\t" << set_cfcomp << "\t" << set_injec<< std::endl;
    
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
    CalibSummary aSum = CalibSummary(nRun, runNumber,calib.GetVop(), 0);
    aSum.SetRunProperties(itRun->second);
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
      TH1D* histCellTOA= nullptr;
      TH1D* histCellTOT= nullptr;
      
      // std::cout << "celld id: " << itcalib->first << "\t "<< calib.GetPedestalMeanH(itcalib->first);
      // reading Waveform
      profCellWave     = (TProfile*)tempFile->Get(Form("IndividualCells/waveform1DfullCellID%i",itcalib->first));
      // reading ToA
      histCellTOA      = (TH1D*)tempFile->Get(Form("IndividualCells/hspectrafullTOACellID%i",itcalib->first));
      // reading ToT
      histCellTOT      = (TH1D*)tempFile->Get(Form("IndividualCells/hspectrafullTOTCellID%i",itcalib->first));
      // std::cout <<"\t Wave: "<< profCellWave << "\t TOA: " << histCellTOA << "\t TOT " << histCellTOT << std::endl;
      int entries = 0;
      if (profCellWave)
        entries = profCellWave->GetEntries();
      
      if (entries == 0) continue;
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
        itrend->second.Fill(Xvalue,itcalib->second, (int)calib.GetRunNumber(), (double)calib.GetVop(),itRun->second.pdg, -10000., -10000.,itRun->second.energy,itRun->second.temp);
        itrend->second.FillExtended(Xvalue,profCellWave->GetEntries(), (int)calib.GetRunNumber(), histCellTOT, histCellTOA, nullptr, profCellWave);
        itrend->second.FillHGCROCSetting (set_rf, set_cf, set_cfcomp, set_cc, set_injec);
      // create new TileTrend object if not yet available in map
      } else {
        TileTrend atrend=TileTrend(itcalib->first,0, 2);
        // fill injection hists
        atrend.Fill(Xvalue,itcalib->second, (int)calib.GetRunNumber(), (double)calib.GetVop(), itRun->second.pdg, -10000., -10000.,itRun->second.energy,itRun->second.temp );
        atrend.FillExtended(Xvalue,profCellWave->GetEntries(), (int)calib.GetRunNumber(),histCellTOT,  histCellTOA, nullptr, profCellWave);
        atrend.FillHGCROCSetting (set_rf, set_cf, set_cfcomp, set_cc, set_injec);
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
  // Extract common run infos
  // ******************************************************************************************
  RunInfo commonRunInfo = GetCommonRunInfoFromList(runList);
  PrintSettingsRunInfo(commonRunInfo);
  int labelOpt        = 1;
  Int_t nSameSettings = GetNSameSettings(commonRunInfo, labelOpt);

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
    for (int i = 0; i <itrend->second.GetNRuns(); i++ ){
      if (Xaxis == 3){
        itrend->second.SetLabelPerRun(Form("it. %d", i));
      } else if (Xaxis == 4){
        itrend->second.SetLabelPerRun(Form("int. %d", i));
      } else {
        itrend->second.SetLabelPerRun(itrend->second.GetLabelLegend( commonRunInfo, i, nSameSettings));
      }
      // std::cout << i << "\t Run: "<<itrend->second.GetRunNr(i) << "\t Label: " << itrend->second.GetLabel(i) << std::endl;     
    }
  }

  int cCalib = 0;
  for (isumCalibs = sumCalibs.begin(); isumCalibs!=sumCalibs.end(); ++isumCalibs){
    if (Xaxis == 3){
      isumCalibs->second.SetLabel(Form("it. %d", cCalib));
    } else if (Xaxis == 4){
      isumCalibs->second.SetLabel(Form("int. %d", cCalib));
    } else {
      isumCalibs->second.SetLabel(isumCalibs->second.GetLabelLegend( commonRunInfo,  nSameSettings));
    }
    // std::cout << cCalib << "\t Run: "<<isumCalibs->second.GetRunNumber() << "\t Label: " << isumCalibs->second.GetLabel() << std::endl;     
    cCalib++;
  }

  
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
  MultiCanvas panelPlot2D(detConf, "Waveform");
  bool init2D = panelPlot2D.Initialize(2);
  
  // panelPlot2D.PlotTrending(trend, 0, Xmin,Xmax, OutputNameDirPlots, "PedADC", plotSuffix, commonRunInfo, ExtPlot );
  panelPlot2D.PlotRunOverlayProfile(trend, nRun, 1, -25, 25*commonRunInfo.samples, -10, -10000, OutputNameDirPlots, "WaveOverlay", plotSuffix, commonRunInfo, ExtPlot );
  // panelPlot2D.PlotRunOverlayProfile(trend, nRun, 3, 0, 18*7, 0,-10000, OutputNameDirPlots, "TOTOverlay", plotSuffix, commonRunInfo, ExtPlot );
  // panelPlot2D.PlotRunOverlayProfile(trend, nRun, 2, 0, 18*7, 0,1024, OutputNameDirPlots, "TOAOverlay", plotSuffix, commonRunInfo, ExtPlot );

  std::vector <int> cellVec;
  if (cellList.CompareTo("")!= 0){
    std::cout << "cell List set: "  << cellList.Data() << std::endl;
    std::fstream cellTxt;
    cellTxt.open(cellList.Data(),std::ios::in);
    if(!cellTxt.is_open()){
      std::cout<<"Error opening "<<cellList.Data()<<", does the file exist?"<<std::endl;
    }
    while(cellTxt.good()){
      TString dummyCell;
      // set first root file names
      cellTxt>>dummyCell;
      std::cout << "\t" << dummyCell.Data() << std::endl;
      if (dummyCell.CompareTo("") != 0)
        cellVec.push_back(dummyCell.Atoi());
    }
    std::cout << "registered: " << cellVec.size() << " single cells to be plotted" << std::endl;
    for (int i = 0; i < cellVec.size(); i++){
        std::cout << cellVec.at(i) << "," ;
    }
    std::cout << std::endl;
  }
  
  if (cellVec.size() > 0){
    
    detConf = DetConf::Type::SingleTile;
    MultiCanvas panelSingleTile(detConf, "WaveformTile");
    panelSingleTile.SetCellVector(cellVec);
    bool initSngle = panelSingleTile.Initialize(1);
    panelSingleTile.SetLabelOpt(1);
    
    panelSingleTile.PlotRunOverlayProfile(trend, nRun, 1, -25, 25*commonRunInfo.samples, -10, 1324, OutputNameDirPlots, "TileWaveOverlay", plotSuffix, commonRunInfo, ExtPlot);
    panelSingleTile.PlotRunOverlaySpectra(trend, nRun, 0, 0, 4148, OutputNameDirPlots, "TileTOTSpectra", plotSuffix, commonRunInfo, ExtPlot, debug, 0);
    panelSingleTile.PlotRunOverlaySpectra(trend, nRun, 1, 0, 1024, OutputNameDirPlots, "TileTOASpectra", plotSuffix, commonRunInfo, ExtPlot, debug, 0);
    
  }
  return status;
}


// ===========================================================================================
// Create the output file 
// ===========================================================================================
bool ComparisonWaveform::CreateOutputRootFile(void){

  std::string testing = RootOutputName.Data();
  std::size_t found = testing.find_last_of("/\\");
  std::cout << " path: " << testing.substr(0,found) << '\n';
  std::cout << " file: " << testing.substr(found+1) << '\n';
  std::string path = testing.substr(0,found);
  if (path.size() > 0){
    std::cout << "Checking whether directory needs to be created: " << path.data() << std::endl;
    gSystem->Exec(Form("mkdir -p %s",path.data()));
  }

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
