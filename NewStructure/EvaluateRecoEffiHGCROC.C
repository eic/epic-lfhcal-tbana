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

//__________________________________________________________________________________________________________
//_____________________MAIN function !!! ___________________________________________________________________
//__________________________________________________________________________________________________________
void EvaluateRecoEffiHGCROC( TString configFileName     = "", 
                             TString outputDir          = "Compare/",
                             TString runListFileName    = "configs/SPS_RunNumbers.txt",
                             Int_t verbosity            = 0
                          ){
                              
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
      std::map<int,RunInfo>::iterator it=ri.find(temprun);
      
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
      
      std::cout << (TString)(PrintRunRecData(tempRunRec, false)).Data() << std::endl;
      rED[temprun] = tempRunRec;
    } 

    for (const auto& run : rED){
      std::cout << (TString)(PrintRunRecData(run.second, true)).Data();
    }
  
  
  
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
                        
