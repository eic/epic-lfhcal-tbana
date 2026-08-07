#include "EvaluateRecoEffiHGCROC.h"


// ****************************************************************************
// Checking and opening input files
// ****************************************************************************

bool EvaluateRecoEffiGHCROC::CheckAndOpenIO(void){

  //Need to check first input to get the setup...I do not think it is necessary
  std::cout<<"Input file set to: '"<<inputFilePath.Data() <<std::endl;
  if(inputFilePath.IsNull()){
    std::cout<<"An input file is required, aborting"<<std::endl;
    return false;
  } 

  if( outputDir.IsNull()){
    std::cout << "No output directory set, aborting... "  << std::endl;
    return false;
  } else {
    gSystem->Exec("mkdir -p "+outputDir);
    std::cout << "Output directory set by default to " << outputDir << std::endl;
  }

  if( RunListInputName.IsNull() ){
    std::cout << "No run list given, aborting..."<<std::endl;
    return false;
  }

  std::cout <<"=============================================================" << std::endl;
  std::cout <<" Basic setup complete" << std::endl;
  std::cout <<"=============================================================" << std::endl;
  return true;
}



// ****************************************************************************
// Primary process function 
// ****************************************************************************
bool EvaluateRecoEffiGHCROC::Process(){
    bool status;
    status = DoEvaluateRecoEffiHGCROC();
    return status;
}


// ****************************************************************************
// Actual function with evaluation 
// ****************************************************************************
bool EvaluateRecoEffiGHCROC::DoEvaluateRecoEffiHGCROC(){
    
      
    StyleSettingsBasics();
    // ********************************************************************************************************
    // read run list and corresponding settings
    // ********************************************************************************************************
    std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
    std::map<int,runRecData> rED;
    // ********************************************************************************************************    
    // read folder and name from file
    // ********************************************************************************************************
    // std::vector<Int_t> runnumbers;
    // std::vector<TString> fileNames;
    std::ifstream in;
    in.open(inputFilePath,std::ios_base::in);
    if (!in) {
        std::cout << "ERROR: file " << inputFilePath.Data() << " not found!" << std::endl;
        return false;
    }

    TGraph* graphRecEffi          = new TGraph();
    TGraph* graphRecEffiVsNAttemp = new TGraph();
    TGraph* graphRecEffiSpecies[5];   // 0: pedestal
                                      // 1: muon
                                      // 2: e+/e-
                                      // 3: pi+/pi-
                                      // 3: p/anti-p
    TGraph* graphRecEffiNAttSpecies[5];   // 0: pedestal
                                      // 1: muon
                                      // 2: e+/e-
                                      // 3: pi+/pi-
                                      // 3: p/anti-p                                      
    for (int i = 0; i < 5; i++){
      graphRecEffiSpecies[i]      = new TGraph();
      graphRecEffiNAttSpecies[i]  = new TGraph();
    }
    
    double maxTrigg               = 0;
    std::map<int,RunInfo>::iterator it;
    double minEffi                = 10;
    double maxEffi                = -10;
    int nZeroEff                  = 0;
    
    for( TString tempLine; tempLine.ReadLine(in, kTRUE); ) {
      // check if line should be considered
      if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
          continue;
      }
      if (debug > 0) std::cout << tempLine.Data() << std::endl;

      // Separate the string according to tabulators
      TObjArray *tempArr  = tempLine.Tokenize("_");
      if(tempArr->GetEntries()<1){
          if (debug > 1) std::cout << "nothing to be done" << std::endl;
          delete tempArr;
          continue;
      } else if (tempArr->GetEntries()==1){
          if (debug > 1) std::cout << "File name doesn't contain an _ :"  << tempLine.Data()<< std::endl;
      }
      TString fileEnd  = ((TString)((TObjString*)tempArr->At(tempArr->GetEntries()-1))->GetString());
      TObjArray *tempArr2  = fileEnd.Tokenize(".");
      if(tempArr2->GetEntries()<=1 ){
          if (debug > 1) std::cout << "line doesn't have a file ending" << std::endl;
          delete tempArr;
          delete tempArr2;
          continue;
      }
      TString ending  = ((TString)((TObjString*)tempArr2->At(1))->GetString());
      if (ending.CompareTo("root") != 0){
          if (debug > 1) std::cout << "This is not a root file, skipping.... : " << tempLine.Data() << std::endl;
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
      it=ri.find(temprun);
      
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
      
      if(tempRunRec.triggers!= 0){
        graphRecEffi->AddPoint(temprun,tempRunRec.recEffi);
        graphRecEffiVsNAttemp->AddPoint(tempRunRec.recEffi,tempRunRec.triggers);
        if (maxTrigg < tempRunRec.triggers)
          maxTrigg = tempRunRec.triggers;
        if (maxEffi < tempRunRec.recEffi)
          maxEffi = tempRunRec.recEffi;
        if (minEffi > tempRunRec.recEffi && minEffi > 0)
          minEffi = tempRunRec.recEffi;
        if (tempRunRec.recEffi <= 0.)
          nZeroEff++;
        
        // species dependent graphs
        std::cout << tempRunRec.pid << std::endl;
        if (tempRunRec.pid == 0){ // pedestal
          graphRecEffiSpecies[0]->AddPoint(temprun,tempRunRec.recEffi);
          graphRecEffiNAttSpecies[0]->AddPoint(tempRunRec.recEffi,tempRunRec.triggers);
        }
        if (tempRunRec.pid == -13 || tempRunRec.pid == 13 ){ // muon
          graphRecEffiSpecies[1]->AddPoint(temprun,tempRunRec.recEffi);
          graphRecEffiNAttSpecies[1]->AddPoint(tempRunRec.recEffi,tempRunRec.triggers);
        }
        if (tempRunRec.pid == -11 || tempRunRec.pid == 11 ){ // electron
          graphRecEffiSpecies[2]->AddPoint(temprun,tempRunRec.recEffi);
          graphRecEffiNAttSpecies[2]->AddPoint(tempRunRec.recEffi,tempRunRec.triggers);
        }
        if (tempRunRec.pid == 211 || tempRunRec.pid == -211){ // charged pion
          graphRecEffiSpecies[3]->AddPoint(temprun,tempRunRec.recEffi);
          graphRecEffiNAttSpecies[3]->AddPoint(tempRunRec.recEffi,tempRunRec.triggers);
        }
        if (tempRunRec.pid == 2212 || tempRunRec.pid == -2212){ // proton
          graphRecEffiSpecies[4]->AddPoint(temprun,tempRunRec.recEffi);
          graphRecEffiNAttSpecies[4]->AddPoint(tempRunRec.recEffi,tempRunRec.triggers);
        }
      }
      
      std::cout << (TString)(PrintRunRecData(tempRunRec, false)).Data() << std::endl;
      rED[temprun] = tempRunRec;
    } 
    std::cout << "runs completely unreconstructable " << nZeroEff << std::endl; 
    
    for (const auto& run : rED){
      std::cout << (TString)(PrintRunRecData(run.second, true)).Data();
    }
  
    graphRecEffi->Sort();
    graphRecEffi->GetXaxis()->SetTitle("Run Nr.");
    graphRecEffi->GetYaxis()->SetTitle("#varepsilon_{rec}");
    graphRecEffiVsNAttemp->Sort();
    graphRecEffiVsNAttemp->GetYaxis()->SetTitle("#N_{trigg received}");
    graphRecEffiVsNAttemp->GetXaxis()->SetTitle("#varepsilon_{rec}");
    for (int i = 0; i < 5; i++){
      graphRecEffiSpecies[i]->Sort();
      graphRecEffiSpecies[i]->GetXaxis()->SetTitle("Run Nr.");
      graphRecEffiSpecies[i]->GetYaxis()->SetTitle("#varepsilon_{rec}");
      graphRecEffiNAttSpecies[i]->Sort();
      graphRecEffiNAttSpecies[i]->GetYaxis()->SetTitle("#N_{trigg received}");
      graphRecEffiNAttSpecies[i]->GetXaxis()->SetTitle("#varepsilon_{rec}");
    }
    
    TCanvas* canvas = new TCanvas("canvas","",0,0,800,600);
    DrawCanvasSettings( canvas,0.08, 0.01, 0.01, 0.1);
    TCanvas* canvas2 = new TCanvas("canvas2","",0,0,800,600);
    DrawCanvasSettings( canvas2,0.10, 0.01, 0.01, 0.1);
    canvas2->SetLogy(1);
    
    PlotTrendingCorr (canvas, 0.95,  0.95, 0.035, 30, 
                  graphRecEffi, graphRecEffi->GetX()[0]-2 , graphRecEffi->GetX()[graphRecEffi->GetN()-1]+2, minEffi*0.8, 1.05, Form("%s/RecEffi",outputDir.Data()) );
    PlotTrendingMultiSpecies (canvas, 0.95,  0.95, 0.035, 30, 
                  graphRecEffiSpecies, graphRecEffi->GetX()[0]-2 , graphRecEffi->GetX()[graphRecEffi->GetN()-1]+2, minEffi*0.8, 1.05, Form("%s/RecEffiSpecies",outputDir.Data()) );
    PlotTrendingCorr (canvas2, 0.95,  0.95, 0.035, 30, 
                  graphRecEffiVsNAttemp, minEffi*0.95 , 1, 1e3, maxTrigg*2, Form("%s/RecEffiVsAttempTriggers",outputDir.Data()) );
    PlotTrendingMultiSpecies (canvas2, 0.95,  0.95, 0.035, 30, 
                  graphRecEffiNAttSpecies, minEffi*0.95 , 1, 1e3, maxTrigg*2, Form("%s/RecEffiVsAttempTriggersSpecies",outputDir.Data()) );
    
  return true;
}