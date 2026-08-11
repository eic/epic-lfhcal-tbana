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

    TGraph* graphRecTrigger         = new TGraph();
    TGraph* graphTrigger            = new TGraph();
    TGraph* graphRecEffi            = new TGraph();
    TGraph* graphRecEffiVsNAttemp   = new TGraph();
    TGraph* graphRawFileSize        = new TGraph();
    TGraph* graphConvFileSize       = new TGraph();
    TGraph* graphCompression        = new TGraph();
    TGraph* graphReadPackets        = new TGraph();
    TGraph* graphBrokenPackets      = new TGraph();
    TGraph* graphResetsOffset       = new TGraph();
    TGraph* graphFracBrokenPackets  = new TGraph();
    
    // per FPGA triggers
    TGraph* graphEffiFPGA[8];
    TGraph* graphRecTriggerFPGA[8];
    TGraph* graphTriggerFPGA[8];
    for (int f = 0; f < 8; f++ ){
      graphEffiFPGA[f]              = new TGraph();
      graphTriggerFPGA[f]           = new TGraph();
      graphRecTriggerFPGA[f]        = new TGraph(); 
    }
    
    // per species effi
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
    int maxFPGA                   = 0;
    
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
      if (maxFPGA < tempRunRec.nFPGA) 
        maxFPGA             = tempRunRec.nFPGA;
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
  
        graphEffiFPGA[f]->AddPoint(temprun,tempRunRec.effiPerFPGA[f]);
        graphTriggerFPGA[f]->AddPoint(temprun,tempRunRec.triggersPerFPGA[f]);
        graphRecTriggerFPGA[f]->AddPoint(temprun,tempRunRec.recEventsPerFPGA[f]);
      }
      if (histEvents->GetNbinsX() > 2){
        tempRunRec.resetsOffset     = histEvents->GetBinContent(5)/2.;
        tempRunRec.packets          = histEvents->GetBinContent(3);
        tempRunRec.brokenPackets    = histEvents->GetBinContent(4);

        if (tempRunRec.packets!= 0)
          tempRunRec.fracBroken     = (double)tempRunRec.brokenPackets/tempRunRec.packets*100;
        else 
          tempRunRec.fracBroken     = 0.;
        
        if(tempRunRec.packets > 0) 
          graphReadPackets->AddPoint(temprun,tempRunRec.packets);
        if(tempRunRec.brokenPackets > 0) 
          graphBrokenPackets->AddPoint(temprun,tempRunRec.brokenPackets);
        graphFracBrokenPackets->AddPoint(temprun,tempRunRec.fracBroken);
        graphResetsOffset->AddPoint(temprun,tempRunRec.resetsOffset);        
      }
      
      graphRecTrigger->AddPoint(temprun,tempRunRec.recEvents);
      graphTrigger->AddPoint(temprun,tempRunRec.triggers);
      
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
      if (unconvertedDir.CompareTo("") != 0){
        TString sizeFile = gSystem->GetFromPipe("wc -c " +tempLine);
        // std::cout <<"full string:" <<  sizeFile.Data() << std::endl;
        TObjArray *tempArr3  = sizeFile.Tokenize(" ");
        Long64_t sizeC = ((TString)((TObjString*)tempArr3->At(0))->GetString()).Atoll();
        // std::cout << "first token: " << (TString)((TObjString*)tempArr3->At(0))->GetString().Data() << std::endl;
        TString rawFile = Form("%s/Run%03d.h2g", unconvertedDir.Data(), temprun);
        TString sizeFile2 = gSystem->GetFromPipe("wc -c " +rawFile);
        tempArr3  = sizeFile2.Tokenize(" ");
        Long64_t sizeR = ((TString)((TObjString*)tempArr3->At(0))->GetString()).Atoll();
        // std::cout <<"full string:" << sizeFile2.Data() << std::endl;
        Double_t sizeC_MB     = (Double_t)(sizeC)/1000./1000.;
        Double_t sizeR_MB     = (Double_t)(sizeR)/1000./1000.;
        Double_t compression  = sizeC_MB/sizeR_MB*100.;
        tempRunRec.sizeConv   = sizeC_MB;
        tempRunRec.sizeRaw    = sizeR_MB;
        tempRunRec.comp       = compression;
        
        // std::cout << "Sizes: "<< temprun << "\t" <<sizeC  <<"\t" << sizeC_MB << "\t"<< sizeR <<"\t" << sizeR_MB << "\t" << compression << std::endl;
        
        graphRawFileSize->AddPoint(temprun,tempRunRec.sizeRaw);
        graphConvFileSize->AddPoint(temprun,tempRunRec.sizeConv);
        graphCompression->AddPoint(temprun,tempRunRec.comp);
      }
      rED[temprun] = tempRunRec;
    } 
    std::cout << "runs completely unreconstructable " << nZeroEff << std::endl; 
    
    for (const auto& run : rED){
      std::cout << (TString)(PrintRunRecData(run.second, true)).Data();
    }
  
    graphRecTrigger->Sort();
    graphRecTrigger->GetXaxis()->SetTitle("Run Nr.");
    graphRecTrigger->GetYaxis()->SetTitle("Rec. Triggers");
    graphTrigger->Sort();
    graphTrigger->GetXaxis()->SetTitle("Run Nr.");
    graphTrigger->GetYaxis()->SetTitle("Triggers");
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
    DrawCanvasSettings( canvas2,0.09, 0.01, 0.01, 0.1);
    canvas2->SetLogy(1);
    TCanvas* canvas3 = new TCanvas("canvas3","",0,0,800,600);
    DrawCanvasSettings( canvas3,0.09, 0.09, 0.015, 0.09);
    canvas3->SetLogy(1);
    canvas3->SetTicky(0);
    
    Double_t textSizePixel = 28;
    Double_t relSize       = textSizePixel/600;
    
    Double_t minTrigg      = TMath::MinElement(graphTrigger->GetN(),graphTrigger->GetY());
    if (minTrigg <= 0)  
      minTrigg             = 1;
    
    PlotTrendingCorr (canvas, 0.95,  0.95, relSize, textSizePixel, 
                  graphRecEffi, graphRecEffi->GetX()[0]-2 , graphRecEffi->GetX()[graphRecEffi->GetN()-1]+2, minEffi*0.9, 1.05, Form("%s/RecEffi",outputDir.Data()) , it->second);
    PlotTrendingMultiSpecies (canvas, 0.95,  0.95, relSize, textSizePixel, 
                  graphRecEffiSpecies, graphRecEffi->GetX()[0]-2 , graphRecEffi->GetX()[graphRecEffi->GetN()-1]+2, minEffi*0.9, 1.05, Form("%s/RecEffiSpecies",outputDir.Data()) , it->second);
    PlotTrendingCorr (canvas2, 0.95,  0.95, relSize, textSizePixel, 
                  graphRecEffiVsNAttemp, minEffi*0.95 , 1, 1e3, maxTrigg*2, Form("%s/RecEffiVsAttempTriggers",outputDir.Data()) , it->second);
    PlotTrendingMultiSpecies (canvas2, 0.95,  0.95, relSize, textSizePixel, 
                  graphRecEffiNAttSpecies, minEffi*0.95 , 1, 1e3, maxTrigg*2, Form("%s/RecEffiVsAttempTriggersSpecies",outputDir.Data()) , it->second);

    PlotTrendingMultiGraph (canvas3, 0.95,  0.95, relSize, textSizePixel, 
                            graphTrigger, graphRecTrigger, graphRecEffi,
                            graphTrigger->GetX()[0]-2 , graphTrigger->GetX()[graphTrigger->GetN()-1]+2, 
                            minTrigg, TMath::MaxElement(graphTrigger->GetN(),graphTrigger->GetY())*50. , minEffi*0.9, 1.05, 
                            Form("%s/TriggersRecComp",outputDir.Data()), it->second, 
                            "received", "reco.", "#varepsilon_{rec}" );
    for (int f = 0; f < maxFPGA; f++){
      graphEffiFPGA[f]->Sort();
      graphEffiFPGA[f]->GetXaxis()->SetTitle("Run Nr.");
      graphEffiFPGA[f]->GetYaxis()->SetTitle(Form("#varepsilon_{rec,FPGA %d}",f));
      graphTriggerFPGA[f]->Sort();
      graphTriggerFPGA[f]->GetXaxis()->SetTitle("Run Nr.");
      graphTriggerFPGA[f]->GetYaxis()->SetTitle(Form("Triggers FPGA %d",f));
      graphRecTriggerFPGA[f]->Sort();
      graphRecTriggerFPGA[f]->GetXaxis()->SetTitle("Run Nr.");
      graphRecTriggerFPGA[f]->GetYaxis()->SetTitle(Form("Triggers rec. FPGA %d",f));
      
      PlotTrendingMultiGraph (canvas3, 0.95,  0.95, relSize, textSizePixel, 
                              graphTriggerFPGA[f], graphRecTriggerFPGA[f], graphEffiFPGA[f],
                              graphTriggerFPGA[f]->GetX()[0]-2 , graphTriggerFPGA[f]->GetX()[graphTriggerFPGA[f]->GetN()-1]+2, 
                              minTrigg, TMath::MaxElement(graphTriggerFPGA[f]->GetN(),graphTriggerFPGA[f]->GetY())*50. , minEffi*0.9, 1.05, 
                              Form("%s/TriggersRecComp_FPGA_%d",outputDir.Data(),f), it->second, 
                              "received", "reco.", "#varepsilon_{rec, FPGA}" );
    }
    
    PlotTrendingMultiFPGA (canvas2, 0.95,  0.95, relSize, textSizePixel, 
                        graphTrigger,  graphTriggerFPGA, maxFPGA,
                        graphTrigger->GetX()[0]-2 , graphTrigger->GetX()[graphTrigger->GetN()-1]+2,
                        minTrigg, TMath::MaxElement(graphTrigger->GetN(),graphTrigger->GetY())*50, 
                        Form("%s/TriggersFPGAOverview",outputDir.Data()),
                        it->second);
    PlotTrendingMultiFPGA (canvas2, 0.95,  0.95, relSize, textSizePixel, 
                        graphRecTrigger,  graphRecTriggerFPGA, maxFPGA,
                        graphRecTrigger->GetX()[0]-2 , graphRecTrigger->GetX()[graphRecTrigger->GetN()-1]+2,
                        minTrigg, TMath::MaxElement(graphRecTrigger->GetN(),graphRecTrigger->GetY())*50, 
                        Form("%s/RecTriggersFPGAOverview",outputDir.Data()),
                        it->second);
    PlotTrendingMultiFPGA (canvas, 0.95,  0.95, relSize, textSizePixel, 
                        graphRecEffi,  graphEffiFPGA, maxFPGA,
                        graphRecEffi->GetX()[0]-2 , graphRecEffi->GetX()[graphRecEffi->GetN()-1]+2,
                        minEffi*0.9, 1.05, 
                        Form("%s/EffiFPGAOverview",outputDir.Data()),
                        it->second);    
    
    graphBrokenPackets->Sort();
    graphBrokenPackets->GetXaxis()->SetTitle("Run Nr.");
    graphBrokenPackets->GetYaxis()->SetTitle("broken packets");
    graphReadPackets->Sort();
    graphReadPackets->GetXaxis()->SetTitle("Run Nr.");
    graphReadPackets->GetYaxis()->SetTitle("read packets");
    graphFracBrokenPackets->Sort();
    graphFracBrokenPackets->GetXaxis()->SetTitle("Run Nr.");
    graphFracBrokenPackets->GetYaxis()->SetTitle("f (%)");
    graphResetsOffset->Sort();
    graphResetsOffset->GetXaxis()->SetTitle("Run Nr.");
    graphResetsOffset->GetYaxis()->SetTitle("offset resets");
    
    PlotTrendingCorr (canvas2, 0.95,  0.95, relSize, textSizePixel, 
                  graphBrokenPackets, graphBrokenPackets->GetX()[0]-2 , graphBrokenPackets->GetX()[graphBrokenPackets->GetN()-1]+2, 1, TMath::MaxElement(graphBrokenPackets->GetN(),graphBrokenPackets->GetY())*10. , Form("%s/DataBrokenPackets",outputDir.Data()) , it->second);
    PlotTrendingCorr (canvas2, 0.95,  0.95, relSize, textSizePixel, 
                  graphReadPackets, graphReadPackets->GetX()[0]-2 , graphReadPackets->GetX()[graphReadPackets->GetN()-1]+2, 1, TMath::MaxElement(graphReadPackets->GetN(),graphReadPackets->GetY())*10. , Form("%s/DataReadPacket",outputDir.Data()) , it->second);
    PlotTrendingCorr (canvas, 0.95,  0.95, relSize, textSizePixel, 
                  graphFracBrokenPackets, graphFracBrokenPackets->GetX()[0]-2 , graphFracBrokenPackets->GetX()[graphFracBrokenPackets->GetN()-1]+2, 0, 10.5, Form("%s/FracBrokenPackset",outputDir.Data()) , it->second);

    graphReadPackets->GetYaxis()->SetTitle("data size (packets)");
    PlotTrendingMultiGraph (canvas3, 0.95,  0.95, relSize, textSizePixel, 
                            graphReadPackets, graphBrokenPackets, graphFracBrokenPackets,
                            graphReadPackets->GetX()[0]-2 , graphReadPackets->GetX()[graphReadPackets->GetN()-1]+2, 
                            1, TMath::MaxElement(graphReadPackets->GetN(),graphReadPackets->GetY())*10. , 0.00, 10.5, 
                            Form("%s/DataPacketsComp",outputDir.Data()), it->second, 
                            "read", "invalid", "f = invalid/read" );

    PlotTrendingCorr (canvas, 0.95,  0.95, relSize, textSizePixel, 
                  graphResetsOffset, graphResetsOffset->GetX()[0]-2 , graphResetsOffset->GetX()[graphResetsOffset->GetN()-1]+2, 0, TMath::MaxElement(graphResetsOffset->GetN(),graphResetsOffset->GetY())*1.2, Form("%s/OffsetResets",outputDir.Data()) , it->second);
    
    if (unconvertedDir.CompareTo("") != 0){
      Double_t minSize = 1;
      if (TMath::MinElement(graphRawFileSize->GetN(),graphRawFileSize->GetY()) > minSize)
        minSize = TMath::MinElement(graphRawFileSize->GetN(),graphRawFileSize->GetY());
      Double_t minSizeConv = 1;
      if (TMath::MinElement(graphConvFileSize->GetN(),graphConvFileSize->GetY()) > minSizeConv)
        minSizeConv = TMath::MinElement(graphConvFileSize->GetN(),graphConvFileSize->GetY());
      
      graphCompression->Sort();
      graphCompression->GetXaxis()->SetTitle("Run Nr.");
      graphCompression->GetYaxis()->SetTitle("c (%)");
      
      graphRawFileSize->Sort();
      graphRawFileSize->GetXaxis()->SetTitle("Run Nr.");
      graphRawFileSize->GetYaxis()->SetTitle("File size raw (MB)");

      graphConvFileSize->Sort();
      graphConvFileSize->GetXaxis()->SetTitle("Run Nr.");
      graphConvFileSize->GetYaxis()->SetTitle("File size converted (MB)");

      PlotTrendingCorr (canvas, 0.95,  0.95, relSize, textSizePixel, 
                  graphCompression, graphCompression->GetX()[0]-2 , graphCompression->GetX()[graphCompression->GetN()-1]+2, 0, 105, Form("%s/DataCompression",outputDir.Data()) , it->second);
      PlotTrendingCorr (canvas2, 0.95,  0.95, relSize, textSizePixel, 
                  graphRawFileSize, graphRawFileSize->GetX()[0]-2 , graphRawFileSize->GetX()[graphRawFileSize->GetN()-1]+2, minSize*0.9, TMath::MaxElement(graphRawFileSize->GetN(),graphRawFileSize->GetY())*10. , Form("%s/DataRawFileSize",outputDir.Data()), it->second );
      PlotTrendingCorr (canvas2, 0.95,  0.95, relSize, textSizePixel, 
                  graphConvFileSize, graphConvFileSize->GetX()[0]-2 , graphConvFileSize->GetX()[graphConvFileSize->GetN()-1]+2, minSizeConv*0.9, TMath::MaxElement(graphConvFileSize->GetN(),graphConvFileSize->GetY())*10., Form("%s/DataConvertedFileSize",outputDir.Data()), it->second );
      
      graphRawFileSize->GetYaxis()->SetTitle("File size (MB)");
      PlotTrendingMultiGraph (canvas3, 0.95,  0.95, relSize, textSizePixel, 
                              graphRawFileSize, graphConvFileSize, graphCompression,
                              graphRawFileSize->GetX()[0]-2 , graphRawFileSize->GetX()[graphRawFileSize->GetN()-1]+2, 
                              minSize*0.9, TMath::MaxElement(graphRawFileSize->GetN(),graphRawFileSize->GetY())*10. , 0.0, 105, 
                              Form("%s/DataFileSizeComp",outputDir.Data()), it->second , 
                            "raw file", "rec. file", "c = rec/raw");
    }
    
  return true;
}
