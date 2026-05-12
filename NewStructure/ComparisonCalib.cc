#include "ComparisonCalib.h"
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
bool ComparisonCalib::CheckAndOpenIO(void){
  
  int matchingbranch;
  
  // *****************************************************************************************
  // Reading files from a text file
  // *****************************************************************************************
  if(!InputListName.IsNull()){
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // text file with only 1 calib-file per line 
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    if (expandedList == 0){
      std::cout << "*********************************************************" << std::endl;
      std::cout << "Simple reading from file list" << std::endl;
      std::cout << "*********************************************************" << std::endl;
      // File exist?
      std::cout << "reading from file list: " << InputListName.Data() << std::endl;
      std::fstream dummyTxt;
      dummyTxt.open(InputListName.Data(),std::ios::in);
      if(!dummyTxt.is_open()){
        std::cout<<"Error opening "<<InputListName.Data()<<", does the file exist?"<<std::endl;
      }
    
      // initialize root file name
      std::string dummyRootName;
      // set first root file name
      dummyTxt>>dummyRootName;
      // read files from text file
      int goodsetup;
      int goodcalib;
      while(dummyTxt.good()){
        std::cout << dummyRootName.data() << std::endl;
        // check that file exists and can be opened
        TFile dummyRoot=TFile(dummyRootName.c_str(),"READ");
        if(dummyRoot.IsZombie()){
          std::cout<<"Error opening '"<<dummyRootName<<", does the file exist?"<<std::endl;
          return false;
        }
        dummyRoot.Close();
        // Add file-name to setup and calib chain
        goodsetup=TsetupIn->AddFile(dummyRootName.c_str());
        goodcalib=TcalibIn->AddFile(dummyRootName.c_str());
        if(goodcalib==0){
          std::cout<<"Issues retrieving Calib tree from "<<dummyRootName<<", file is ignored"<<std::endl;
        }
        if(goodsetup==0){
          std::cout<<"Issues retrieving Setup tree from "<<dummyRootName<<", file is ignored"<<std::endl;
        }
        // set next root file name
        dummyTxt>>dummyRootName;
      }
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // text file with 2 files per line 1 calib file & 1 histo file
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    } else {  // read more complex files
      std::cout << "You need to provide a calib file and an output of the improved muon calib in the textfile in each line" << std::endl;
      std::fstream dummyTxt;
      dummyTxt.open(InputListName.Data(),std::ios::in);
      if(!dummyTxt.is_open()){
        std::cout<<"Error opening "<<InputListName.Data()<<", does the file exist?"<<std::endl;
      }
      std::string dummyRootCalibName;
      std::string dummyRootHistName;
      // set first root file names
      dummyTxt>>dummyRootCalibName >> dummyRootHistName;
      
      int goodsetup;
      int goodcalib;
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
  } // end read input from text file
  // *****************************************************************************************
  // Reading files by auto-expansion
  // - we did not provide a txt file with a list of root file we want to process but directly 
  //   the list of files to process
  // *****************************************************************************************
  else {
    std::cout << "*********************************************************" << std::endl;
    std::cout << "Simple reading from vector" << std::endl;
    std::cout << "*********************************************************" << std::endl;
    
    // check if file vector is empty
    if(RootInputNames.empty()){
      std::cout<<"No root files, neither text list file provided"<<std::endl;
      return false;
    }
    
    // read input files from string vector
    std::vector<TString>::iterator it;
    int goodsetup;
    int goodcalib;
    for(it=RootInputNames.begin(); it!=RootInputNames.end(); ++it){
      // check if file exists
      TFile dummy=TFile(it->Data(),"READ");
      if(dummy.IsZombie()){
        std::cout<<"Error opening '"<<it->Data()<<", does the file exist?"<<std::endl;
        return false;
      }
      dummy.Close();
      // Add file-name to setup and calib chain
      goodsetup=TsetupIn->AddFile(it->Data());
      goodcalib=TcalibIn->AddFile(it->Data());
      if(goodcalib==0){
        std::cout<<"Issues retrieving Calib tree from "<<it->Data()<<", file is ignored"<<std::endl;
      }
      if(goodsetup==0){
        std::cout<<"Issues retrieving Setup tree from "<<it->Data()<<", file is ignored"<<std::endl;
      }
    }
  } // end read files by auto-extension
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
} // end CheckAndOpenIO()

// ===========================================================================================
// Main function of this calibration comparison 
// ===========================================================================================
bool ComparisonCalib::ProcessCalib(void){
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
  std::map<int,RunInfo>::iterator it; // basic infos
  
  // ******************************************************************************************
  // Iterate over all entries (runs) in the calib tree
  // ******************************************************************************************
  for(int ientry=0; ientry<entries;ientry++){
    TcalibIn->GetEntry(ientry);
    TsetupIn->GetEntry(ientry);
    
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // set global iterator for runs to first run number in list to obtain beam-line, dates...
    if (ientry==0) it = ri.find(calib.GetRunNumber());
    
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Set X-values according to option
    // Xaxis:   
    //        0 - Run number dependence
    //        1 - Operational Voltage dependence
    //        2 - Time dependence
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    Int_t runNumber = calib.GetRunNumber();
    if(Xaxis==0){//depending on run #
      Xvalue=calib.GetRunNumber();
    } else if(Xaxis==1){
      Xvalue=calib.GetVop();
    } else if(Xaxis==2){
      Xvalue=calib.GetBeginRunTime()->GetSec();
    }
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
    TH2D* hTrigger2D    = nullptr;
    TH2D* hHG_LMPV2D    = nullptr;
    TH2D* hHG_LSigma2D  = nullptr;
    TH2D* hHG_GSigma2D  = nullptr;
    TH2D* hLG_LMPV2D    = nullptr;
    TH2D* hLG_LSigma2D  = nullptr;
    TH2D* hLG_GSigma2D  = nullptr;
    TH2D* hSB_Noise2D   = nullptr;
    TH2D* hSB_Signal2D  = nullptr;

    TFile* tempFile = nullptr;
    if (expandedList > 0){
      if (nRun < (int)RootInputNames.size()){
        std::cout << "reading hist file: " <<RootInputNames[nRun].Data() << " expanded list setting: " << expandedList << std::endl;
        tempFile      = new TFile(RootInputNames[nRun].Data(),"READ");
        
        if (expandedList == 1){
          hTrigger2D    = (TH2D*)tempFile->Get("hmipTriggers");
          hHG_LMPV2D    = (TH2D*)tempFile->Get("hspectraHGLMPVVsLayer");
          hHG_LSigma2D  = (TH2D*)tempFile->Get("hspectraHGLSigmaVsLayer");
          hHG_GSigma2D  = (TH2D*)tempFile->Get("hspectraHGGSigmaVsLayer");
          if (!isHGCROC){
            hLG_LMPV2D    = (TH2D*)tempFile->Get("hspectraLGLMPVVsLayer");
            hLG_LSigma2D  = (TH2D*)tempFile->Get("hspectraLGLSigmaVsLayer");
            hLG_GSigma2D  = (TH2D*)tempFile->Get("hspectraLGGSigmaVsLayer");
          }
          hSB_Noise2D   = (TH2D*)tempFile->Get("hSuppresionNoise");
          hSB_Signal2D  = (TH2D*)tempFile->Get("hSuppresionSignal");
          std::cout << "***********************************************************" << std::endl;
          if(hTrigger2D) std::cout << "found hmipTriggers hist" << std::endl;
          if(hHG_LMPV2D) std::cout << "found hspectraHGLMPVVsLayer hist" << std::endl;
          if(hHG_LSigma2D) std::cout << "found hspectraHGLSigmaVsLayer hist" << std::endl;
          if(hHG_GSigma2D) std::cout << "found hspectraHGGSigmaVsLayer hist" << std::endl;
          if(hSB_Noise2D) std::cout << "found hSuppresionNoise hist" << std::endl;
          if(hSB_Signal2D) std::cout << "found hSuppresionSignal hist" << std::endl;
          std::cout << "***********************************************************" << std::endl;
        } else if (expandedList == 2){
          hTrigger2D    = (TH2D*)tempFile->Get("hmipTriggers");
          hSB_Noise2D   = (TH2D*)tempFile->Get("hSuppresionNoise");
          hSB_Signal2D  = (TH2D*)tempFile->Get("hSuppresionSignal");          
        } 
      }
    } // end if expandedList
    
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // Loop over all cells in the calib object for trending plots
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    for(itcalib=calib.begin(); itcalib!=calib.end(); ++itcalib){

      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      // Reading additional cell histos from 2nd file
      // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%      
      int triggers      = 0;
      double hgLMPV     = 0.;
      double hgLSigma   = 0.;
      double hgGSigma   = 0.;
      double hgLMPV_E   = 0.;
      double hgLSigma_E = 0.;
      double hgGSigma_E = 0.;
      double lgLMPV     = 0.;
      double lgLSigma   = 0.;
      double lgGSigma   = 0.;
      double lgLMPV_E   = 0.;
      double lgLSigma_E = 0.;
      double lgGSigma_E = 0.;
      double sbNoise    = 0.;
      double sbSignal   = 0.;
      double lghgOff    = -10000.;
      double lghgOff_E  = 0.;
      double hglgOff    = -10000.;
      double hglgOff_E  = 0.;
      
      
      TH1D* histCellHG      = nullptr;
      TH1D* histCellLG      = nullptr;
      TProfile* profCellLGHG= nullptr;
      TF1* fitLGHG          = nullptr;
      TF1* fitHGLG          = nullptr;
      if (expandedList == 1){
        if (ExtPlot > 1){
          histCellHG      = (TH1D*)tempFile->Get(Form("IndividualCellsTrigg/hspectramipTriggHGCellID%i",itcalib->first));
          if (!isHGCROC){
            histCellLG      = (TH1D*)tempFile->Get(Form("IndividualCellsTrigg/hspectramipTriggLGCellID%i",itcalib->first));
            profCellLGHG    = (TProfile*)tempFile->Get(Form("IndividualCellsTrigg/hCoorspectramipTriggLGHGCellID%i",itcalib->first));
          }
        }
        int layer     = setup->GetLayer(itcalib->first);
        int chInLayer = setup->GetChannelInLayer(itcalib->first);
        triggers      = hTrigger2D->GetBinContent(hTrigger2D->FindBin(layer,chInLayer));
        hgLMPV        = hHG_LMPV2D->GetBinContent(hHG_LMPV2D->FindBin(layer,chInLayer));
        hgLMPV_E      = hHG_LMPV2D->GetBinError(hHG_LMPV2D->FindBin(layer,chInLayer));
        hgLSigma      = hHG_LSigma2D->GetBinContent(hHG_LSigma2D->FindBin(layer,chInLayer));
        hgLSigma_E    = hHG_LSigma2D->GetBinError(hHG_LSigma2D->FindBin(layer,chInLayer));
        hgGSigma      = hHG_GSigma2D->GetBinContent(hHG_GSigma2D->FindBin(layer,chInLayer));	
        hgGSigma_E    = hHG_GSigma2D->GetBinError(hHG_GSigma2D->FindBin(layer,chInLayer));
       	
			 if (!isHGCROC){
          lgLMPV        = hLG_LMPV2D->GetBinContent(hLG_LMPV2D->FindBin(layer,chInLayer));
          lgLMPV_E      = hLG_LMPV2D->GetBinError(hLG_LMPV2D->FindBin(layer,chInLayer));
          lgLSigma      = hLG_LSigma2D->GetBinContent(hLG_LSigma2D->FindBin(layer,chInLayer));
          lgLSigma_E    = hLG_LSigma2D->GetBinError(hLG_LSigma2D->FindBin(layer,chInLayer));
          lgGSigma      = hLG_GSigma2D->GetBinContent(hLG_GSigma2D->FindBin(layer,chInLayer));
          lgGSigma_E    = hLG_GSigma2D->GetBinError(hLG_GSigma2D->FindBin(layer,chInLayer));
        }
        sbNoise       = hSB_Noise2D->GetBinError(hSB_Noise2D->FindBin(layer,chInLayer));
        sbSignal      = hSB_Signal2D->GetBinError(hSB_Signal2D->FindBin(layer,chInLayer)); 
			} // end if expandedList == 1 
			else if (expandedList == 2){
        if (ExtPlot > 1){
          profCellLGHG    = (TProfile*)tempFile->Get(Form("IndividualCells/hCoorspectramip1stLGHGCellID%i",itcalib->first));
          histCellHG      = (TH1D*)tempFile->Get(Form("IndividualCellsTrigg/hspectramipTriggHGCellID%i",itcalib->first));
          histCellLG      = (TH1D*)tempFile->Get(Form("IndividualCellsTrigg/hspectramipTriggLGCellID%i",itcalib->first));       
        }
        fitLGHG         = (TF1*)tempFile->Get(Form("IndividualCells/fcorrmip1stLGHGCellID%i",itcalib->first));
        fitHGLG         = (TF1*)tempFile->Get(Form("IndividualCells/fcorrmip1stHGLGCellID%i",itcalib->first));
        int layer     = setup->GetLayer(itcalib->first);
        int chInLayer = setup->GetChannelInLayer(itcalib->first);
        triggers      = hTrigger2D->GetBinContent(hTrigger2D->FindBin(layer,chInLayer));
        sbNoise       = hSB_Noise2D->GetBinError(hSB_Noise2D->FindBin(layer,chInLayer));
        sbSignal      = hSB_Signal2D->GetBinError(hSB_Signal2D->FindBin(layer,chInLayer));
        if (fitLGHG){
          lghgOff     = fitLGHG->GetParameter(0);
          lghgOff_E   = fitLGHG->GetParError(0);
        }
        if (fitHGLG){
          hglgOff     = fitHGLG->GetParameter(0);
          hglgOff_E   = fitHGLG->GetParError(0);
        }
      } else if (expandedList == 3){
        if (ExtPlot > 1){
          std::cout<<"Layer Plotting: " << setup->GetLayer(itcalib->first) <<std::endl;
          profCellLGHG    = (TProfile*)tempFile->Get(Form("IndividualCells/hCoorspectraAllTriggersLGHGCellID%i",itcalib->first));
        }
      } else if (expandedList == 4){
        if (ExtPlot > 1){
          std::cout<<"Layer Plotting: " << setup->GetLayer(itcalib->first) <<std::endl;
          histCellHG      = (TH1D*)tempFile->Get(Form("IndividualCells/hspectrafullADCCellID%i",itcalib->first));
          profCellLGHG    = (TProfile*)tempFile->Get(Form("IndividualCells/wafeform1DfullCellID%i",itcalib->first));
          std::cout << histCellHG << "\t" << profCellLGHG << std::endl;
        }
      
      // Ped case  
      } else if (expandedList == 5){      
          if (debug > 1)std::cout<<"Nothing to do in this case" <<std::endl;
      }
      
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
        // fill minimal object
        itrend->second.Fill(Xvalue,itcalib->second, (int)calib.GetRunNumber(), (double)calib.GetVop());
        // fill with additional information
        if (expandedList == 1){
          itrend->second.FillExtended(Xvalue,triggers, (int)calib.GetRunNumber(), histCellHG, histCellLG, profCellLGHG); 
          itrend->second.FillMPV(Xvalue, hgLMPV, hgLMPV_E, lgLMPV, lgLMPV_E);
          itrend->second.FillLSigma(Xvalue, hgLSigma, hgLSigma_E, lgLSigma, lgLSigma_E);
          itrend->second.FillGSigma(Xvalue, hgGSigma, hgGSigma_E, lgGSigma, lgGSigma_E);
          itrend->second.FillSB(Xvalue, sbSignal, sbNoise);
        } else if (expandedList == 2){
          itrend->second.FillExtended(Xvalue,triggers, (int)calib.GetRunNumber(), histCellHG, histCellLG, profCellLGHG); 
          itrend->second.FillSB(Xvalue, sbSignal, sbNoise);
        } else if (expandedList == 3){
          std::cerr<<"Run Num: " << (int)calib.GetRunNumber()<<std::endl;
          itrend->second.FillExtended(Xvalue,1, (int)calib.GetRunNumber(), nullptr, nullptr, profCellLGHG); 
          // itrend->second.FillCorrOffset(Xvalue, lghgOff, lghgOff_E, hglgOff, hglgOff_E);
        } else if (expandedList == 4){
          std::cerr<<"Run Num: " << (int)calib.GetRunNumber()<<std::endl;
          itrend->second.FillExtended(Xvalue,1, (int)calib.GetRunNumber(), histCellHG, nullptr, profCellLGHG); 
        }
      // create new TileTrend object if not yet available in map
      } else {
        TileTrend atrend=TileTrend(itcalib->first,0, expandedList);
        // fill minimal object
        atrend.Fill(Xvalue,itcalib->second, (int)calib.GetRunNumber(), (double)calib.GetVop());
        // fill with additional information
        if (expandedList == 1){
          atrend.FillExtended(Xvalue,triggers, (int)calib.GetRunNumber(), histCellHG, histCellLG, profCellLGHG); 
          atrend.FillMPV(Xvalue, hgLMPV, hgLMPV_E, lgLMPV, lgLMPV_E);
          atrend.FillLSigma(Xvalue, hgLSigma, hgLSigma_E, lgLSigma, lgLSigma_E);
          atrend.FillGSigma(Xvalue, hgGSigma, hgGSigma_E, lgGSigma, lgGSigma_E);
          atrend.FillSB(Xvalue, sbSignal, sbNoise);
        } else if (expandedList == 2){
          atrend.FillExtended(Xvalue,triggers, (int)calib.GetRunNumber(), histCellHG, histCellLG, profCellLGHG); 
          atrend.FillSB(Xvalue, sbSignal, sbNoise);
          // atrend.FillCorrOffset(Xvalue, lghgOff, lghgOff_E, hglgOff, hglgOff_E);
        } else if (expandedList == 3){
          std::cout<<"Layer Filling: " << calib.GetRunNumber()<<": "<<setup->GetLayer(itcalib->first) <<std::endl;
          atrend.FillExtended(Xvalue,1, (int)calib.GetRunNumber(), nullptr, nullptr, profCellLGHG); 
        } else if (expandedList == 4){
          std::cout<<"Layer Filling: " << calib.GetRunNumber()<<": "<<setup->GetLayer(itcalib->first) <<std::endl;
          atrend.FillExtended(Xvalue,1, (int)calib.GetRunNumber(), histCellHG, nullptr, profCellLGHG); 
        }
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
  } else if (Xaxis == 1){
    Xmin= Xmin-0.5;
    Xmax= Xmax+0.5;
  } 

  TString             xaxisTitle = "";
  if (Xaxis==0)       xaxisTitle = "Run Nr. ";
  else if (Xaxis==1)  xaxisTitle = "V (V) ";
  else                xaxisTitle = "date";  
  
  for(itrend=trend.begin(); itrend!=trend.end(); ++itrend){    
    // sort graphs
    itrend->second.Sort();
    // set x axis title for trending graphs
    itrend->second.SetXAxisTitle(xaxisTitle);
    // write graphs for each cell to output
    itrend->second.Write(RootOutput);
  }

  //******************************************************************************
  // plotting overview for each run overlayed
  //******************************************************************************
  Int_t textSizePixel   = 30;
  Float_t textSizeRel   = 0.04;  
  TCanvas* canvas1DRunsOverlay = new TCanvas("canvas1DRunsOverlay","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvas1DRunsOverlay, 0.075, 0.015, 0.025, 0.09);

  PlotCalibRunOverlay( canvas1DRunsOverlay, 0, sumCalibs, textSizeRel, 
                      Form("%s/HGPedSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,"", debug);
  PlotCalibRunOverlay( canvas1DRunsOverlay, 1, sumCalibs, textSizeRel, 
                      Form("%s/HGPedWidthSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,"", debug);
  PlotCalibRunOverlay( canvas1DRunsOverlay, 2, sumCalibs, textSizeRel, 
                      Form("%s/LGPedSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,"", debug);
  PlotCalibRunOverlay( canvas1DRunsOverlay, 3, sumCalibs, textSizeRel, 
                      Form("%s/LGPedWidthSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,"", debug);
  if (globalStatus > 1){
    PlotCalibRunOverlay( canvas1DRunsOverlay, 4, sumCalibs, textSizeRel, 
                      Form("%s/HGScaleSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,"", debug);
    PlotCalibRunOverlay( canvas1DRunsOverlay, 5, sumCalibs, textSizeRel, 
                      Form("%s/HGScaleWidthSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,"", debug);
    if (!isHGCROC){
      PlotCalibRunOverlay( canvas1DRunsOverlay, 6, sumCalibs, textSizeRel, 
                          Form("%s/LGScaleSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second, "", debug);
      PlotCalibRunOverlay( canvas1DRunsOverlay, 7, sumCalibs, textSizeRel, 
                          Form("%s/LGScaleWidthSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second, "", debug);
      PlotCalibRunOverlay( canvas1DRunsOverlay, 8, sumCalibs, textSizeRel, 
                          Form("%s/LGHGCorr_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second, "", debug);
      PlotCalibRunOverlay( canvas1DRunsOverlay, 9, sumCalibs, textSizeRel, 
                          Form("%s/HGLGCorr_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second, "", debug);
      PlotCalibRunOverlay( canvas1DRunsOverlay, 10, sumCalibs, textSizeRel, 
                          Form("%s/LGScaleCalcSummary_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,"", debug);  
      PlotCalibRunOverlay( canvas1DRunsOverlay, 11, sumCalibs, textSizeRel, 
                          Form("%s/LGHGOffsetCorr_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second, "", debug);
      PlotCalibRunOverlay( canvas1DRunsOverlay, 12, sumCalibs, textSizeRel, 
                          Form("%s/HGLGOffsetCorr_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second, "", debug);
    }
  }
  std::cout << "row max: " << setup->GetNMaxRow() << "\t column max: "  << setup->GetNMaxColumn() << std::endl;
  
  
  // plotting individual layers/asics
  DetConf::Type detConf = setup->GetDetectorConfig();

  MultiCanvas panelPlot(detConf, "Injection");
  bool init1D = panelPlot.Initialize(1);
  
  MultiCanvas panelPlot2D(detConf, "Injection");
  bool init2D = panelPlot2D.Initialize(2);
    
  if (expandedList != 3 ){
    TString pedAName = "HG";
    if (isHGCROC) pedAName = "0thSample";
    panelPlot.PlotTrending(trend, 0, Xmin,Xmax, OutputNameDirPlots, Form("%sped",pedAName.Data()), plotSuffix, it->second, ExtPlot );
    panelPlot.PlotTrending(trend, 15, Xmin,Xmax, OutputNameDirPlots, Form("%spedwidth",pedAName.Data()), plotSuffix, it->second, ExtPlot );
    
    TString pedBName = "LG";
    if (isHGCROC) pedBName = "Wave";
    panelPlot.PlotTrending(trend, 1, Xmin,Xmax, OutputNameDirPlots, Form("%sped", pedBName.Data()), plotSuffix, it->second, ExtPlot );
    panelPlot.PlotTrending(trend, 16, Xmin,Xmax, OutputNameDirPlots, Form("%sLGpedwidth", pedBName.Data()), plotSuffix, it->second, ExtPlot );
  
    if (globalStatus >1 ){
      panelPlot.PlotTrending(trend, 2, Xmin,Xmax, OutputNameDirPlots, "HGScale", plotSuffix, it->second, ExtPlot );
      if (!isHGCROC){
        panelPlot.PlotTrending(trend, 3, Xmin,Xmax, OutputNameDirPlots, "LGScale", plotSuffix, it->second, ExtPlot );
        panelPlot.PlotTrending(trend, 4, Xmin,Xmax, OutputNameDirPlots, "LGHGCorr", plotSuffix, it->second, ExtPlot );
        panelPlot.PlotTrending(trend, 5, Xmin,Xmax, OutputNameDirPlots, "HGLGCorr", plotSuffix, it->second, ExtPlot );
        panelPlot.PlotTrending(trend, 17, Xmin,Xmax, OutputNameDirPlots, "LGHG_Offset", plotSuffix, it->second, ExtPlot );
        panelPlot.PlotTrending(trend, 18, Xmin,Xmax, OutputNameDirPlots, "HGLG_Offset", plotSuffix, it->second, ExtPlot );
      }
    }
  }  
  if (expandedList == 1 && globalStatus > 1 ){
    panelPlot.PlotTrending(trend, 9, Xmin,Xmax, OutputNameDirPlots, "HG_LandMPV", plotSuffix, it->second, ExtPlot );
    panelPlot.PlotTrending(trend, 11, Xmin,Xmax, OutputNameDirPlots, "HG_LandSigma", plotSuffix, it->second, ExtPlot );
    panelPlot.PlotTrending(trend, 13, Xmin,Xmax, OutputNameDirPlots, "HG_GaussSigma", plotSuffix, it->second, ExtPlot );
    if (!isHGCROC){
      panelPlot.PlotTrending(trend, 10, Xmin,Xmax, OutputNameDirPlots, "LG_LandMPV", plotSuffix, it->second, ExtPlot );
      panelPlot.PlotTrending(trend, 12, Xmin,Xmax, OutputNameDirPlots, "LG_LandSigma", plotSuffix, it->second, ExtPlot );
      panelPlot.PlotTrending(trend, 14, Xmin,Xmax, OutputNameDirPlots, "LG_GaussSigma", plotSuffix, it->second, ExtPlot );
    }
  }
  if (( expandedList == 1 || expandedList == 2)  && globalStatus > 1){
    panelPlot.PlotTrending(trend, 6, Xmin,Xmax, OutputNameDirPlots, "MuonTriggers", plotSuffix, it->second, ExtPlot );
    panelPlot.PlotTrending(trend, 7, Xmin,Xmax, OutputNameDirPlots, "SBSignal_MuonTriggers", plotSuffix, it->second, ExtPlot );
    panelPlot.PlotTrending(trend, 7, Xmin,Xmax, OutputNameDirPlots, "SBNoise_MuonTriggers", plotSuffix, it->second, ExtPlot );
  }
  
  if (ExtPlot > 1){
    if (expandedList == 1 || expandedList == 2 ){
      Int_t maxADC = 850;
      TString nameADC = "MuonTriggers_HGDist";
      if (isHGCROC){
        maxADC = 350;
        nameADC = "MuonTriggers_ADCDist";
      }
      panelPlot.PlotRunOverlaySpectra(trend, nRun, 0, -15, maxADC, OutputNameDirPlots, nameADC, plotSuffix, it->second, ExtPlot);
      if (!isHGCROC) 
        panelPlot.PlotRunOverlaySpectra(trend, nRun, 1, -10,210, OutputNameDirPlots, "MuonTriggers_LGDist", plotSuffix, it->second, ExtPlot);
    }
    if (expandedList > 0 && !isHGCROC){
      if (globalStatus > 1) panelPlot.PlotRunOverlayProfile(trend, nRun, 1, -20, 340, -20, 3900, OutputNameDirPlots, "MuonTriggers_LGHGCorr", plotSuffix, it->second, ExtPlot );
    }

    if (expandedList > 0 && isHGCROC && globalStatus > 1){
      panelPlot.PlotRunOverlayProfile(trend, nRun, 1, -20, 450000, -20, 1024, OutputNameDirPlots, "WaveOverlay", plotSuffix, it->second, ExtPlot );
      panelPlot.PlotRunOverlayProfile(trend, nRun, 1, -20, 450000, -0.01, 0.05, OutputNameDirPlots, "WaveOverlayScaled", plotSuffix, it->second, ExtPlot, 1, true);
    }    
  }
  
  return status;
}


// ===========================================================================================
// Create the output file 
// ===========================================================================================
bool ComparisonCalib::CreateOutputRootFile(void){
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
