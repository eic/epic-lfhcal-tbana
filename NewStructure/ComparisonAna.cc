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
#include "AnaSummary.h"
#include "CalibSummary.h"
#include "MultiCanvas.h"
#include "CommonHelperFunctions.h"
#include "PlotHelper.h"
#include "ComparisonAna.h"


//Input Setup

bool ComparisonAna::CheckAndOpenIO(void){
  
  int matchingbranch;
  
  // *****************************************************************************************
  // Reading files from a text file
  // *****************************************************************************************
  if(!InputListName.IsNull()){
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // text file with only 2 files per line 
    // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 
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
      //std::cout << dummyRootCalibName.data() << "\t" << dummyRootHistName.data() << std::endl;
      
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




// Main Loop
bool ComparisonAna::ProcessAna(void){
  std::cout<<"in process"<<std::endl;
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

  std::map<int, AnaSummary> sumCalibs;
  std::map<int, AnaSummary>::iterator isumCalibs;
  
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

    it = ri.find(calib.GetRunNumber());
    Xvalue=calib.GetRunNumber();

    // TODO: Change id from run number, look at constructor ()
    AnaSummary aSum = AnaSummary(nRun, calib.GetRunNumber(),calib.GetVop(),it->second.energy, it->second.pdg);

    TFile* tempFile = nullptr;
    if (nRun < (int)RootInputNames.size()){
     // std::cerr << "names: " << RootInputNames[nRun].Data() << std::endl;
      tempFile      = new TFile(RootInputNames[nRun].Data(),"READ");   
      TH1D* hTimeDiff   = nullptr;
      TH1D* hTotEnergy  = nullptr;
      TH1D* hNCells     = nullptr;
      // histo output from DataAna::QA()/SimpleQA()
      if (expandedList == 1){
        hTimeDiff   = (TH1D*)tempFile->Get("hDeltaTime");
        if (it->second.pdg == 13 || it->second.pdg == -13){
          hTotEnergy  = (TH1D*)tempFile->Get("hTotEnergyMuon");
          hNCells     = (TH1D*)tempFile->Get("hNCellsMuon");
        } else {
          hTotEnergy  = (TH1D*)tempFile->Get("hTotEnergyNonMuon");
          hNCells     = (TH1D*)tempFile->Get("hNCellsNonMuon");        
        }
      // histo output from DataPrep::Calibrate()
      // these aren't correctly protected for running with HGCROC
      } else if (expandedList == 2){  
        hTimeDiff                 = (TH1D*)tempFile->Get("hDeltaTime");
        hNCells                   = (TH1D*)tempFile->Get("hNCells");
        hTotEnergy                = (TH1D*)tempFile->Get("hTotEnergy");
        TH1D* hSatADCvsCellID     = (TH1D*)tempFile->Get("hSaturatedHGvsCellID");
        TH1D* hSatADC             = (TH1D*)tempFile->Get("hSaturatedADC");
        TH1D* hSatLGvsCellID      = (TH1D*)tempFile->Get("hSaturatedLGvsCellID");
        TH1D* hSatLG              = (TH1D*)tempFile->Get("hSaturatedLG");
        TH1D* hLGHGCorrOutCellID  = (TH1D*)tempFile->Get("hLGHGCorrOutsideBoundCellID");
        TH1D* hLGHGCorrOut        = (TH1D*)tempFile->Get("hNCellsLGHGCorrOutsideBound");

        aSum.SetSatADCCellIDHist(hSatADCvsCellID);
        aSum.SetSatLGCellIDHist(hSatLGvsCellID);
        aSum.SetLGHGOutCellIDHist(hLGHGCorrOutCellID);
        aSum.SetSatADCHist(hSatADC);
        aSum.SetSatLGHist(hSatLG);
        aSum.SetLGHGOutHist(hLGHGCorrOut);
        
        double intSatADC  = hSatADC->Integral(hSatADC->FindBin(1), hSatADC->GetNbinsX() )/hSatADC->GetEntries();
        double intSatLG   = hSatLG->Integral(hSatLG->FindBin(1), hSatLG->GetNbinsX() )/hSatLG->GetEntries();
        double intLGHGOut = hLGHGCorrOut->Integral(hLGHGCorrOut->FindBin(2), hLGHGCorrOut->GetNbinsX() )/hLGHGCorrOut->GetEntries();
        std::cout << "Run "<<Xvalue<<" \t frac >= 1 HG channel saturated " <<  intSatADC*100 << "\t frac >= 1 LG channel saturated " << intSatLG*100 << "\t frac events to be discarded:" << intLGHGOut*100 << std::endl;
      }
      aSum.SetDeltaTimeHist(hTimeDiff);
      aSum.SetEnergyHist(hTotEnergy);
      aSum.SetNCellsHist(hNCells);
      std::cout<<"Run "<<Xvalue<<" mean time diff: "<< hTimeDiff->GetMean()<< "\t mean Energy:" << hTotEnergy->GetMean()<< "\t mean NCells:" << hNCells->GetMean() <<std::endl;
    }
    // append AnaSummary object to map
    std::cout<<"filling for " << nRun <<std::endl; 
    sumCalibs[nRun]=aSum;
    nRun++;
  }
  Int_t textSizePixel   = 30;
  Float_t textSizeRel   = 0.04;  
  TCanvas* canvasOverlay1D = new TCanvas("canvasOverlay1D","",0,0,1450,1300);  // gives the page size
  DefaultCanvasSettings( canvasOverlay1D,  0.08, 0.03, 0.025, 0.09);
  canvasOverlay1D->SetLogy(1);
  PlotAnalysisComparison( canvasOverlay1D, 0, sumCalibs, textSizeRel, 
                      Form("%s/TimeDiff_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,1,"", debug, 0, colorByEV);
  
  PlotAnalysisComparison( canvasOverlay1D, 1, sumCalibs, textSizeRel, 
                      Form("%s/Energy_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,eoLabelOpt,"", debug, eoXmax, colorByEV);
  PlotAnalysisComparison( canvasOverlay1D, 2, sumCalibs, textSizeRel, 
                      Form("%s/NCells_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,eoLabelOpt, "", debug, 0, colorByEV);
  
  // these aren't correctly protected for running with HGCROC
  if (expandedList == 2){
    PlotAnalysisComparison( canvasOverlay1D, 3, sumCalibs, textSizeRel, 
                            Form("%s/SatADC_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,1,"", debug, 0, colorByEV);
    PlotAnalysisComparison( canvasOverlay1D, 4, sumCalibs, textSizeRel, 
                            Form("%s/SatLG_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,1,"", debug, 0, colorByEV);
    PlotAnalysisComparison( canvasOverlay1D, 5, sumCalibs, textSizeRel, 
                            Form("%s/LGHGOut_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,1,"", debug, 0, colorByEV);
    PlotAnalysisComparison( canvasOverlay1D, 6, sumCalibs, textSizeRel, 
                            Form("%s/SatADCvsCellID_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,1,"", debug, 0, colorByEV);
    PlotAnalysisComparison( canvasOverlay1D, 7, sumCalibs, textSizeRel, 
                            Form("%s/SatLGvsCellID_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,1,"", debug, 0, colorByEV);
    PlotAnalysisComparison( canvasOverlay1D, 8, sumCalibs, textSizeRel, 
                            Form("%s/LGHGOutvsCellID_RunOverlay.%s",OutputNameDirPlots.Data(),plotSuffix.Data()), it->second,1,"", debug, 0, colorByEV); 
  }
  return true;
}


// Make Output File
bool ComparisonAna::CreateOutputRootFile(void){

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
