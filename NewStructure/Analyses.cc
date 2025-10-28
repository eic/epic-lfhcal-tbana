#include "Analyses.h"
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

#include "HGCROC_Convert.h"

#include "waveform_fitting/waveform_fit_base.h"
#include "waveform_fitting/crystal_ball_fit.h"
#include "waveform_fitting/max_sample_fit.h"

// ****************************************************************************
// Checking and opening input and output files
// ****************************************************************************
bool Analyses::CheckAndOpenIO(void){
  int matchingbranch;
  if(!ASCIIinputName.IsNull()){
    std::cout << "Input to be converted into correct format :" <<  ASCIIinputName.Data() << std::endl;
    ASCIIinput.open(ASCIIinputName.Data(),std::ios::in);
    if(!ASCIIinput.is_open()){
      std::cout<<"Could not open input file: "<<std::endl;
      return false;
    }
  }

  //Need to check first input to get the setup...I do not think it is necessary
  std::cout <<"=============================================================" << std::endl;
  std::cout<<"Input name set to: "<<RootInputName.Data() <<std::endl;
  std::cout<<"Output name set to: "<<RootOutputName.Data() <<std::endl;
  if (!Convert) std::cout<<"Calib name set to: "<<RootCalibInputName.Data() <<std::endl;
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
    
    //Do I really want this?
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
    //End of do I really want this?
    
    //We want to retrieve also calibration if do not specify ApplyTransferCalib from external file
    //In other words, the pedestal was potentially already done and we have an existing calib object
    if((!ApplyTransferCalib && ExtractScaling) || (!ApplyTransferCalib && ExtractScalingImproved) || (!ApplyTransferCalib && ReextractNoise)){
      TcalibIn = (TTree*) RootInput->Get("Calib");
      if(!TcalibIn){
        std::cout<<"Could not retrieve Calib tree, leaving"<<std::endl;
        return false;
      }
      //calib=Calib::GetInstance();
      matchingbranch=TcalibIn->SetBranchAddress("calib",&calibptr);
      if(matchingbranch<0){
        std::cout<<"Error retrieving calibration info from the tree"<<std::endl;
        return false;
      }
    }
    //All good
  }
  else if(!Convert){
    std::cout<<"Explicit Input option mandatory except for convertion ASCII -> ROOT"<<std::endl;
    return false;
  }  
  
  //Setup Output files
  if(!RootOutputName.IsNull()){    
    if (!Convert){
      TString temp = RootOutputName;
      temp         = temp.ReplaceAll(".root","_Hists.root");
      SetRootOutputHists(temp);
      // std::cout << "creating additional histo file: " << RootOutputNameHist.Data() << " tree in : "<< RootOutputName.Data() << std::endl;
    }
    
    bool sCOF = CreateOutputRootFile();
    if (!sCOF) return false;
    
    TsetupOut = new TTree("Setup","Setup");
    setup=Setup::GetInstance();
    //TsetupOut->Branch("setup",&setup);
    TsetupOut->Branch("setup",&rsw);
    TdataOut = new TTree("Data","Data");
    TdataOut->Branch("event",&event);
    TcalibOut = new TTree("Calib","Calib");
    TcalibOut->Branch("calib",&calib);
  }
  else if (!SaveCalibOnly){
    std::cout<<"Output option mandatory except when converting"<<std::endl;
    return false;
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

  if(!RootCalibOutputName.IsNull() && SaveCalibOnly){
    std::cout << "entered here" << std::endl;
    RootCalibOutput=new TFile(RootCalibOutputName.Data(),"RECREATE");
    if(RootCalibOutput->IsZombie()){
      std::cout<<"Error opening '"<<RootCalibOutputName<<"', does the file exist?"<<std::endl;
      return false;
    }
    
    if (RootOutputName.IsNull()){
      TsetupOut = new TTree("Setup","Setup");
      setup=Setup::GetInstance();
      //TsetupOut->Branch("setup",&setup);
      TsetupOut->Branch("setup",&rsw);
      TcalibOut = new TTree("Calib","Calib");
      TcalibOut->Branch("calib",&calib);
    }
  }

  if(!RootPedestalInputName.IsNull()){
    RootPedestalInput = new TFile(RootPedestalInputName.Data(),"READ");
    if(RootPedestalInput->IsZombie()){
      std::cout<<"Error opening '"<<RootPedestalInputName<<"', does the file exist?"<<std::endl;
      return false;
    }
    TcalibIn = (TTree*) RootPedestalInput->Get("Calib");
    if(!TcalibIn){
      std::cout<<"Could not retrieve Calib tree, leaving"<<std::endl;
      return false;
    } else {
      std::cout<<"Retrieved calib object from external input for pedestals!"<<std::endl;
    }
    matchingbranch=TcalibIn->SetBranchAddress("calib",&calibptr);
    if(matchingbranch<0){
      std::cout<<"Error retrieving calibration info from the tree"<<std::endl;
      return false;
    }else {
      std::cout<<"Correctly set branch for external calib for pedestal input."<<std::endl;
    }
    //std::cout<<"Did the address changed? "<<&calib<<std::endl;
  }
  if(!MapInputName.IsNull()){
    MapInput.open(MapInputName.Data(),std::ios::in);
    if(!MapInput.is_open()){
      std::cout<<"Could not open mapping file: "<<MapInputName<<std::endl;
      return false;
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
bool Analyses::Process(void){
  bool status = true;
  ROOT::EnableImplicitMT();
  
  if(Convert){
    std::cout << "Converting !" << std::endl;
    if (HGCROC) {
      // Set waveform builder
      waveform_fit_base *waveform_builder = nullptr;
      // waveform_builder = new crystal_ball_fit();

      // // Set the parameters based off what I found in the stand alone analysis
      // // Alpha
      // waveform_builder->set_parameter(0, 1.1);  // Initial value
      // waveform_builder->set_parameter(10, 1);   // Lower bound
      // waveform_builder->set_parameter(20, 1.2); // Uppser bound

      // // n
      // waveform_builder->set_parameter(1, 0.3);
      // waveform_builder->set_parameter(11, 0.2);
      // waveform_builder->set_parameter(21, 0.6);

      // // x_bar
      // waveform_builder->set_parameter(2, 0.7);
      // waveform_builder->set_parameter(12, 0.5);
      // waveform_builder->set_parameter(22, 4.5);

      // // sigma
      // waveform_builder->set_parameter(3, 0.3);
      // waveform_builder->set_parameter(13, 0.25);
      // waveform_builder->set_parameter(23, 0.65);

      // // N
      // waveform_builder->set_parameter(4, 0);
      // waveform_builder->set_parameter(14, 0);
      // waveform_builder->set_parameter(24, 2000);

      // // Offset
      // waveform_builder->set_parameter(5, 100);  // This needs to become pedestals eventually... 
      // waveform_builder->set_parameter(15, 0);
      // waveform_builder->set_parameter(25, 160);

      waveform_builder = new max_sample_fit();

      std::cout << "Running HGCROC conversion" << std::endl;
      status=run_hgcroc_conversion(this, waveform_builder);
    } else {
      if (!(GetASCIIinputName().EndsWith(".root"))){
        status=ConvertASCII2Root();
      } else {
        std::cout << "WARNING: This option should only be used for the 2023 SPS test beam for which the CAEN raw data was lost!" << std::endl;
        status=ConvertOldRootFile2Root();
      }
    }
  }
  
  // extract pedestal from pure pedestal runs (internal triggers)
  if(ExtractPedestal){
    status=GetPedestal();
  }
  
  // copy existing calibration to other file
  if(ApplyTransferCalib && !IsAnalyseWaveForm){
    status=TransferCalib();
  }
  
  if (IsAnalyseWaveForm){
    status=AnalyseWaveForm();
  }
  // extract mip calibration 
  if(ExtractScaling){
    status=GetScaling();
  }
  
  // extract noise sample from trigger flagged files
  if(ReextractNoise){
    status=GetNoiseSampleAndRefitPedestal();
  }
  
  // rerun mip calibration based on triggers
  if (ExtractScalingImproved){
    status=GetImprovedScaling();
  }
  
  // copy full calibration to different file and calculate energy
  if(ApplyCalibration){
    status=Calibrate();
  }
  
  // reduce file to only noise triggers
  if(SaveNoiseOnly){
    status=SaveNoiseTriggersOnly();
  }
  
  // reduce file to only mip triggers
  if(SaveMipsOnly){
    status=SaveMuonTriggersOnly();
  }
  
  // skim HGCROC data to discard pure noise events
  if(SkimHGCROC){
    status=SkimHGCROCData();
  }
  
  // reduce file to only mip triggers
  if(EvalLocalTriggers){
    status=RunEvalLocalTriggers();
  }
  
  // save calib to output only
  if (SaveCalibOnly){
    status = SaveCalibToOutputOnly();
  }
  return status;
}


// ****************************************************************************
// convert original ASCII file from CAEN output to root format
// ****************************************************************************
bool Analyses::ConvertASCII2Root(void){
  //============================================
  //Init first
  //============================================
  // initialize setup
  if (MapInputName.CompareTo("")== 0) {
      std::cerr << "ERROR: No mapping file has been provided, please make sure you do so! Aborting!" << std::endl;
      return false;
  }
  std::cout << "creating mapping " << std::endl;
  setup->Initialize(MapInputName.Data(),debug);
  // initialize run number file
  if (RunListInputName.CompareTo("")== 0) {
      std::cerr << "ERROR: No run list file has been provided, please make sure you do so! Aborting!" << std::endl;
      return false;
  }
  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  
  // Clean up file headers
  TObjArray* tok=ASCIIinputName.Tokenize("/");
  // get file name
  TString file=((TObjString*)tok->At(tok->GetEntries()-1))->String();
  delete tok;
  tok=file.Tokenize("_");
  TString header=((TObjString*)tok->At(0))->String();
  delete tok;
  
  // Get run number from file & find necessary run infos
  TString RunString=header(3,header.Sizeof());
  std::map<int,RunInfo>::iterator it=ri.find(RunString.Atoi());
  //std::cout<<RunString.Atoi()<<"\t"<<it->second.species<<std::endl;
  event.SetRunNumber(RunString.Atoi());
  event.SetROtype(ReadOut::Type::Caen);
  event.SetBeamName(it->second.species);
  event.SetBeamID(it->second.pdg);
  event.SetBeamEnergy(it->second.energy);
  event.SetVop(it->second.vop);
  event.SetVov(it->second.vop-it->second.vbr);
  event.SetBeamPosX(it->second.posX);
  event.SetBeamPosY(it->second.posY);
  calib.SetRunNumber(RunString.Atoi());
  calib.SetVop(it->second.vop);
  calib.SetVov(it->second.vop-it->second.vbr);  
  calib.SetBCCalib(false);
  //============================================
  // Start decoding file
  //============================================
  TString aline         = "";
  TString versionCAEN   = "";
  TObjArray* tokens;
  std::map<int,std::vector<Caen> > tmpEvent;
  std::map<int,double> tmpTime;
  std::map<int,std::vector<Caen> >::iterator itevent;
  long tempEvtCounter   = 0;
  long writeEvtCounter  = 0;
  while(!ASCIIinput.eof()){                                                     // run till end of file is reached and read line by line
    aline.ReadLine(ASCIIinput);
    if(!ASCIIinput.good()) break;
    if(aline[0]=='/'){
      if (aline.Contains("File Format Version")){
        tokens      = aline.Tokenize(" ");
        versionCAEN = ((TObjString*)tokens->At(4))->String();
        std::cout << "File version: " << ((TObjString*)tokens->At(4))->String() << std::endl;
        
        if (!(versionCAEN.CompareTo("3.3") == 0 || versionCAEN.CompareTo("3.1") == 0)){
          std::cerr << "This version cannot be converted with the current code, please add the corresponding file format to the converter" << std::endl;
          tokens->Clear();
          delete tokens;
          return false;
        }  
        
        tokens->Clear();
        delete tokens;
      }
      else if(aline.Contains("Run start time")){
        tokens    = aline.Tokenize(" ");
        int year=((TObjString*)tokens->At(8))->String().Atoi();
        int month;
        TString Stringmonth=((TObjString*)tokens->At(5))->String();
        if(Stringmonth=="Jan") month=1;
        else if(Stringmonth=="Feb") month=2;
        else if(Stringmonth=="Mar") month=3;
        else if(Stringmonth=="Apr") month=4;
        else if(Stringmonth=="May") month=5;
        else if(Stringmonth=="Jun") month=6;
        else if(Stringmonth=="Jul") month=7;
        else if(Stringmonth=="Aug") month=8;
        else if(Stringmonth=="Sep") month=9;
        else if(Stringmonth=="Oct") month=10;
        else if(Stringmonth=="Nov") month=11;
        else if(Stringmonth=="Dec") month=12;
        int day=((TObjString*)tokens->At(6))->String().Atoi();
        int hour=((TString)((TObjString*)tokens->At(7))->String()(0,2)).Atoi();
        int min=((TString)((TObjString*)tokens->At(7))->String()(3,5)).Atoi();
        int sec=((TString)((TObjString*)tokens->At(7))->String()(6,8)).Atoi();
        TTimeStamp t=TTimeStamp(year,month,day,hour,min,sec);
        event.SetBeginRunTime(t);
        calib.SetBeginRunTime(t);
        tokens->Clear();
        delete tokens;
      }
      continue;
    }
    tokens=aline.Tokenize(" \t");
    tokens->SetOwner(true);
    
    if (versionCAEN.CompareTo("3.3") == 0){
      int Nfields=tokens->GetEntries();
      // std::cout << Nfields << std::endl;
      if(((TObjString*)tokens->At(0))->String()=="Brd") {
        tokens->Clear();
        delete tokens;
        continue;
      }
      //================================================================================
      // data format example
      // Brd  Ch       LG       HG        Tstamp_us        TrgID		NHits
      // 7  00       51       68     10203578.136            0		64
      // 7  01       55       75 
      //================================================================================
      if(Nfields!=7){
        std::cout<<"Unexpected number of fields"<<std::endl;
        std::cout<<"Expected 7, got: "<<Nfields<<", exit"<<std::endl;
        for(int j=0; j<Nfields;j++) std::cout<<j<<"  "<<((TObjString*)tokens->At(j))->String()<<std::endl;
        tokens->Clear();
        delete tokens;
        return -1;
      }
      int TriggerID=((TObjString*)tokens->At(5))->String().Atoi();
      int NHits=((TObjString*)tokens->At(6))->String().Atoi();
      double Time = ((TObjString*)tokens->At(4))->String().Atof();
      Caen aTile;
      int aBoard=((TObjString*)tokens->At(0))->String().Atoi();
      int achannel=((TObjString*)tokens->At(1))->String().Atoi();
      aTile.SetE(((TObjString*)tokens->At(3))->String().Atoi());//To Test
      aTile.SetADCHigh(((TObjString*)tokens->At(3))->String().Atoi());
      aTile.SetADCLow (((TObjString*)tokens->At(2))->String().Atoi());
      tokens->Clear();
      delete tokens;
      aTile.SetCellID(setup->GetCellID(aBoard,achannel));
      itevent=tmpEvent.find(TriggerID);

      if(itevent!=tmpEvent.end()){
        tmpTime[TriggerID]+=Time;
        if (aTile.GetCellID() != -1){
          itevent->second.push_back(aTile);
        } else {
          if(debug ==10) std::cout << "cell " << aBoard << "\t" << achannel << " wasn't active according to mapping file!" << std::endl;
        }
        for(int ich=1; ich<NHits; ich++){
          aline.ReadLine(ASCIIinput);
          tokens=aline.Tokenize(" ");
          tokens->SetOwner(true);
          Nfields=tokens->GetEntries();
          if(Nfields!=4){
            std::cout<<"Expecting 4 fields but read "<<Nfields<<std::endl;
            return -1;
          }
          achannel=((TObjString*)tokens->At(1))->String().Atoi();
          aTile.SetE(((TObjString*)tokens->At(3))->String().Atoi());//To Test
          aTile.SetADCHigh(((TObjString*)tokens->At(3))->String().Atoi());
          aTile.SetADCLow (((TObjString*)tokens->At(2))->String().Atoi());
          aTile.SetCellID(setup->GetCellID(aBoard,achannel));

          if (aTile.GetCellID() != -1){
            itevent->second.push_back(aTile);
          } else {
            if(debug ==10) std::cout << "cell " << aBoard << "\t" << achannel << " wasn't active according to mapping file!" << std::endl;
          }
          tokens->Clear();
          delete tokens;
        }
        // std::cout << itevent->second.size() << "\t" << setup->GetTotalNbChannels() << std::endl;

        if((int)itevent->second.size()==setup->GetTotalNbChannels()/*8*64*/){
          //Fill the tree the event is complete and erase from the map
          event.SetTimeStamp(tmpTime[TriggerID]/setup->GetNMaxROUnit());
          if(debug==1000){
            std::cerr<<event.GetTimeStamp()<<std::endl;
          }
          event.SetEventID(itevent->first);
          for(std::vector<Caen>::iterator itv=itevent->second.begin(); itv!=itevent->second.end(); ++itv){
            event.AddTile(new Caen(*itv));
          }
          TdataOut->Fill();
          writeEvtCounter++;
          tmpEvent.erase(itevent);
          tmpTime.erase(TriggerID);
        } 
      } else {
        //This is a new event;
        tempEvtCounter++;                                                                   // in crease event counts for monitoring of progress
        if (tempEvtCounter%5000 == 0 && debug > 0) std::cout << "Converted " <<  tempEvtCounter << " events" << std::endl;
        // if (tempEvtCounter > 1000) continue;
        std::vector<Caen> vCaen;
        if (aTile.GetCellID() != -1){
          vCaen.push_back(aTile);
        } else {
          if(debug ==10) std::cout << "cell " << aBoard << "\t" << achannel << " wasn't active according to mapping file!" << std::endl;
        }
        for(int ich=1; ich<NHits; ich++){
          aline.ReadLine(ASCIIinput);
          tokens=aline.Tokenize(" ");
          tokens->SetOwner(true);
          Nfields=tokens->GetEntries();
          if(Nfields!=4){
            std::cout<<"Expecting 4 fields but read "<<Nfields<<std::endl;
            return -1;
          }
          achannel=((TObjString*)tokens->At(1))->String().Atoi();
          aTile.SetE(((TObjString*)tokens->At(3))->String().Atoi());//To Test
          aTile.SetADCHigh(((TObjString*)tokens->At(3))->String().Atoi());
          aTile.SetADCLow (((TObjString*)tokens->At(2))->String().Atoi());
          aTile.SetCellID(setup->GetCellID(aBoard,achannel));
          if (aTile.GetCellID() != -1){
            vCaen.push_back(aTile);
          } else {
            if(debug ==10) std::cout << "cell " << aBoard << "\t" << achannel << " wasn't active according to mapping file!" << std::endl;
          }
          tokens->Clear();
          delete tokens;
        }       
        tmpTime[TriggerID]=Time;
        tmpEvent[TriggerID]=vCaen;
        
        // std::cout << vCaen.size() << "\t" << setup->GetTotalNbChannels() << std::endl;
        if((int)vCaen.size()==setup->GetTotalNbChannels()/*8*64*/){
          itevent=tmpEvent.find(TriggerID);
          //Fill the tree the event is complete and erase from the map
          event.SetTimeStamp(tmpTime[TriggerID]/setup->GetNMaxROUnit());
          if(debug==1000){
            std::cerr<<event.GetTimeStamp()<<std::endl;
          }
          event.SetEventID(itevent->first);
          for(std::vector<Caen>::iterator itv=vCaen.begin(); itv!=vCaen.end(); ++itv){
            event.AddTile(new Caen(*itv));
          }
          TdataOut->Fill();
          writeEvtCounter++;
          tmpEvent.erase(itevent);
          tmpTime.erase(TriggerID);
        } 
      }
    } else if (versionCAEN.CompareTo("3.1") == 0){
      int Nfields=tokens->GetEntries();
      if(((TObjString*)tokens->At(0))->String()=="Tstamp_us") {
        tokens->Clear();
        delete tokens;
        continue;
      }
      
      //================================================================================
      // data format example
      //   Tstamp_us        TrgID   Brd  Ch       LG       HG
      // 4970514.360            0    01  00       49       46 
      //                             01  01       49       35 
      //================================================================================
      
      if(Nfields!=6){
        std::cout<<"Unexpected number of fields"<<std::endl;
        std::cout<<"Expected 6, got: "<<Nfields<<", exit"<<std::endl;
        for(int j=0; j<Nfields;j++) std::cout<<j<<"  "<<((TObjString*)tokens->At(j))->String()<<std::endl;
        tokens->Clear();
        delete tokens;
        return -1;
      }
      
      // std::cout << aline.Data() << std::endl;
      int TriggerID = ((TObjString*)tokens->At(1))->String().Atoi();
      int NHits     = 64;                                           // temporary fix
      double Time   = ((TObjString*)tokens->At(0))->String().Atof();
      Caen aTile;
      int aBoard    =((TObjString*)tokens->At(2))->String().Atoi();
      int achannel  =((TObjString*)tokens->At(3))->String().Atoi();
      aTile.SetE(((TObjString*)tokens->At(5))->String().Atoi());//To Test
      aTile.SetADCHigh(((TObjString*)tokens->At(5))->String().Atoi());
      aTile.SetADCLow (((TObjString*)tokens->At(4))->String().Atoi());
      tokens->Clear();
      delete tokens;
      aTile.SetCellID(setup->GetCellID(aBoard,achannel));
      itevent=tmpEvent.find(TriggerID);
      
      if(itevent!=tmpEvent.end()){
        tmpTime[TriggerID]+=Time;
        if (aTile.GetCellID() != -1){
          itevent->second.push_back(aTile);
        } else {
          if(debug ==10) std::cout << "cell " << aBoard << "\t" << achannel << " wasn't active according to mapping file!" << std::endl;
        }
        for(int ich=1; ich<NHits; ich++){
            aline.ReadLine(ASCIIinput);
            // std::cout << aline.Data() << std::endl;
            tokens=aline.Tokenize(" ");
            tokens->SetOwner(true);
            Nfields=tokens->GetEntries();
            
            if(Nfields!=4){
              std::cout<< "Current line :" << aline.Data() << std::endl;
              std::cout<<"Expecting 4 fields but read "<<Nfields<<std::endl;
              return -1;
            }
            achannel=((TObjString*)tokens->At(1))->String().Atoi();
            aTile.SetE(((TObjString*)tokens->At(3))->String().Atoi());//To Test
            aTile.SetADCHigh(((TObjString*)tokens->At(3))->String().Atoi());
            aTile.SetADCLow (((TObjString*)tokens->At(2))->String().Atoi());
            aTile.SetCellID(setup->GetCellID(aBoard,achannel));
            
            if (aTile.GetCellID() != -1){
              itevent->second.push_back(aTile);
            } else {
              if(debug ==10) std::cout << "cell " << aBoard << "\t" << achannel << " wasn't active according to mapping file!" << std::endl;
            }
            tokens->Clear();
            delete tokens;
        }
        if((int)itevent->second.size()==setup->GetTotalNbChannels()){
          //Fill the tree the event is complete and erase from the map
          event.SetTimeStamp(tmpTime[TriggerID]/setup->GetNMaxROUnit());
          event.SetEventID(itevent->first);
          if(debug==1000){
            std::cerr<<event.GetTimeStamp()<<std::endl;
          }
          for(std::vector<Caen>::iterator itv=itevent->second.begin(); itv!=itevent->second.end(); ++itv){
            event.AddTile(new Caen(*itv));
          }
          TdataOut->Fill();
          writeEvtCounter++;
          tmpEvent.erase(itevent);
          tmpTime.erase(TriggerID);
        } else {
          std::cout << "didn't fill" << (int)itevent->second.size()  <<  setup->GetTotalNbChannels()<< std::endl; 
        }
      } else {
        //This is a new event;
        tempEvtCounter++;                                                                   // in crease event counts for monitoring of progress
        if (tempEvtCounter%5000 == 0 && debug > 0) std::cout << "Converted " <<  tempEvtCounter << " events" << std::endl;
        std::vector<Caen> vCaen;
        
        if (aTile.GetCellID() != -1){
          vCaen.push_back(aTile);
        } else {
          if(debug ==10) std::cout << "cell " << aBoard << "\t" << achannel << " wasn't active according to mapping file!" << std::endl;
        }
        for(int ich=1; ich<NHits; ich++){
          aline.ReadLine(ASCIIinput);
          // std::cout << aline.Data() << std::endl;
          tokens=aline.Tokenize(" ");
          tokens->SetOwner(true);
          Nfields=tokens->GetEntries();
          if(Nfields!=4){
            std::cout<< "Current line :" << aline.Data() << std::endl;
            std::cout<<"Expecting 4 fields but read "<<Nfields<<std::endl;
            return -1;
          }
          achannel=((TObjString*)tokens->At(1))->String().Atoi();
          aTile.SetE(((TObjString*)tokens->At(3))->String().Atoi());//To Test
          aTile.SetADCHigh(((TObjString*)tokens->At(3))->String().Atoi());
          aTile.SetADCLow (((TObjString*)tokens->At(2))->String().Atoi());
          aTile.SetCellID(setup->GetCellID(aBoard,achannel));
          if (aTile.GetCellID() != -1){
            vCaen.push_back(aTile);
          } else {
            if(debug ==10) std::cout << "cell " << aBoard << "\t" << achannel << " wasn't active according to mapping file!" << std::endl;
          }
          tokens->Clear();
          delete tokens;
        }
        tmpTime[TriggerID]=Time;
        tmpEvent[TriggerID]=vCaen;
        
        if((int)vCaen.size()==setup->GetTotalNbChannels()/*8*64*/){
          itevent=tmpEvent.find(TriggerID);
          //Fill the tree the event is complete and erase from the map
          event.SetTimeStamp(tmpTime[TriggerID]/setup->GetNMaxROUnit());
          if(debug==1000){
            std::cerr<<event.GetTimeStamp()<<std::endl;
          }
          event.SetEventID(itevent->first);
          for(std::vector<Caen>::iterator itv=vCaen.begin(); itv!=vCaen.end(); ++itv){
            event.AddTile(new Caen(*itv));
          }
          TdataOut->Fill();
          writeEvtCounter++;
          tmpEvent.erase(itevent);
          tmpTime.erase(TriggerID);
        }
        
      }      
    }
  } // finished reading in file
  // 
  if (debug > 0) std::cout << "Converted a total of " <<  tempEvtCounter << " events" << std::endl;
  
  //============================================
  // Fill & write all trees to output file 
  //============================================
  RootOutput->cd();
  // setup 
  RootSetupWrapper rswtmp=RootSetupWrapper(setup);
  rsw=rswtmp;
  TsetupOut->Fill();
  // calib
  TcalibOut->Fill();
  TcalibOut->Write();
  // event data
  TdataOut->Fill();
  TsetupOut->Write();
  TdataOut->Write();

  std::cout << "Events written to file: " << writeEvtCounter << std::endl;
  if (writeEvtCounter < 2){
    std::cout << "ERROR: Only " << writeEvtCounter << " events were written, something didn't go right, please check your mapping file!" << std::endl; 
  }
  RootOutput->Close();
  return true;
}



// ****************************************************************************
// convert already processed root file from CAEN output to new root format
// ****************************************************************************
bool Analyses::ConvertOldRootFile2Root(void){
  //============================================
  //Init first
  //============================================
  // initialize setup
  if (MapInputName.CompareTo("")== 0) {
      std::cerr << "ERROR: No mapping file has been provided, please make sure you do so! Aborting!" << std::endl;
      return false;
  }
  setup->Initialize(MapInputName.Data(),debug);
  // initialize run number file
  if (RunListInputName.CompareTo("")== 0) {
      std::cerr << "ERROR: No run list file has been provided, please make sure you do so! Aborting!" << std::endl;
      return false;
  }
  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  
  // Clean up file headers
  TObjArray* tok=ASCIIinputName.Tokenize("/");
  // get file name
  TString file=((TObjString*)tok->At(tok->GetEntries()-1))->String();
  delete tok;
  tok=file.Tokenize("_");
  TString header=((TObjString*)tok->At(0))->String();
  delete tok;
  
  // Get run number from file & find necessary run infos
  TString RunString=header(3,header.Sizeof());
  std::map<int,RunInfo>::iterator it=ri.find(RunString.Atoi());
  //std::cout<<RunString.Atoi()<<"\t"<<it->second.species<<std::endl;
  event.SetRunNumber(RunString.Atoi());
  event.SetROtype(ReadOut::Type::Caen);
  event.SetBeamName(it->second.species);
  event.SetBeamID(it->second.pdg);
  event.SetBeamEnergy(it->second.energy);
  event.SetVop(it->second.vop);
  event.SetVov(it->second.vop-it->second.vbr);
  event.SetBeamPosX(it->second.posX);
  event.SetBeamPosY(it->second.posY);
  calib.SetRunNumber(RunString.Atoi());
  calib.SetVop(it->second.vop);
  calib.SetVov(it->second.vop-it->second.vbr);  
  calib.SetBCCalib(false);
    // load tree
  TChain *const tt_event = new TChain("tree");
  if (ASCIIinputName.EndsWith(".root")) {                     // are we loading a single root tree?
      std::cout << "loading a single root file" << std::endl;
      tt_event->AddFile(ASCIIinputName);
      TFile testFile(ASCIIinputName.Data());
      if (testFile.IsZombie()){
        std::cout << Form("The file %s is not a root file or doesn't exit, please fix the file path", ASCIIinputName.Data()) << std::endl;
        return false;
      }

  } else {
      std::cout << "please try again this isn't a root file" << std::endl;
  } 
  if(!tt_event){ std::cout << "tree not found... returning!"<< std::endl; return false;}

  // Define tree variables
  Long64_t gTrID;
  Double_t gTRtimeStamp;
  const int gMaxChannels = 64;
  Long64_t* gBoard          = new Long64_t[gMaxChannels];
  Long64_t* gChannel        = new Long64_t[gMaxChannels];
  Long64_t* gLG             = new Long64_t[gMaxChannels];
  Long64_t* gHG             = new Long64_t[gMaxChannels];

  if (tt_event->GetBranchStatus("t_stamp") ){
    tt_event->SetBranchAddress("trgid",            &gTrID);
    tt_event->SetBranchAddress("t_stamp",          &gTRtimeStamp);
    tt_event->SetBranchAddress("board",            gBoard);
    tt_event->SetBranchAddress("channel",          gChannel);
    tt_event->SetBranchAddress("LG",               gLG);
    tt_event->SetBranchAddress("HG",               gHG);
  }
  
  Long64_t nEntriesTree                 = tt_event->GetEntries();
  std::cout << "Number of events in tree: " << nEntriesTree << std::endl;

  std::map<int,std::vector<Caen> > tmpEvent;
  std::map<int,double> tmpTime;
  for (Long64_t i=0; i<nEntriesTree;i++) {
    // load current event
    tt_event->GetEntry(i);  
    if (i%5000 == 0 && debug > 0) std::cout << "Converted " <<  i << " events" << std::endl;    
    int TriggerID = gTrID;
    double Time   = gTRtimeStamp;
    std::vector<Caen> vCaen;
    
    for(Int_t ch=0; ch<gMaxChannels; ch++){   
      Caen aTile;
      int aBoard      = gBoard[ch];
      int achannel    = gChannel[ch];
      aTile.SetE(gHG[ch]);//To Test
      aTile.SetADCHigh(gHG[ch]);
      aTile.SetADCLow (gLG[ch]);
      aTile.SetCellID(setup->GetCellID(aBoard,achannel));
      if (aTile.GetCellID() != -1){
        vCaen.push_back(aTile);
      } else {
        if(debug ==10) std::cout << "cell " << aBoard << "\t" << achannel << " wasn't active according to mapping file!" << std::endl;
      }
    }
    
     if((int)vCaen.size()==setup->GetTotalNbChannels()){
      //Fill the tree the event is complete and erase from the map
      event.SetTimeStamp(Time);
      if(debug==1000){
            std::cerr<<event.GetTimeStamp()<<std::endl;
      }
      event.SetEventID(TriggerID);
      for(std::vector<Caen>::iterator itv=vCaen.begin(); itv!=vCaen.end(); ++itv){
        event.AddTile(new Caen(*itv));
      }
      TdataOut->Fill();
      vCaen.clear();
    }
  } // finished reading the file

  // 
  if (debug > 0) std::cout << "Converted a total of " <<  nEntriesTree << " events" << std::endl;
  
  //============================================
  // Fill & write all trees to output file 
  //============================================
  RootOutput->cd();
  // setup 
  RootSetupWrapper rswtmp=RootSetupWrapper(setup);
  rsw=rswtmp;
  TsetupOut->Fill();
  // calib
  TcalibOut->Fill();
  TcalibOut->Write();
  // event data
  TdataOut->Fill();
  TsetupOut->Write();
  TdataOut->Write();

  RootOutput->Close();
  return true;
}



// ****************************************************************************
// extract pedestral from dedicated data run, no other data in run 
// ****************************************************************************
bool Analyses::GetPedestal(void){
  std::cout<<"GetPedestal for maximium "<< setup->GetMaxCellID() << " cells" <<std::endl;
  
  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  
  int maxChannelPerLayer = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1);
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Migrad");  
  
  // create HG and LG histo's per channel
  TH2D* hspectraHGvsCellID      = new TH2D( "hspectraHG_vsCellID","ADC spectrum High Gain vs CellID; cell ID; ADC_{HG} (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,0,4000);
  hspectraHGvsCellID->SetDirectory(0);
  TH2D* hspectraLGvsCellID      = new TH2D( "hspectraLG_vsCellID","ADC spectrum Low Gain vs CellID; cell ID; ADC_{LG} (arb. units)  ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,0,4000);
  hspectraLGvsCellID->SetDirectory(0);
  TH1D* hMeanPedHGvsCellID      = new TH1D( "hMeanPedHG_vsCellID","mean Ped High Gain vs CellID ; cell ID; #mu_{noise, HG} (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5);
  hMeanPedHGvsCellID->SetDirectory(0);
  TH2D* hspectraHGMeanVsLayer   = new TH2D( "hspectraHGMeanVsLayer","Mean Ped High Gain vs CellID; layer; brd channel; #mu_{Ped HG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGMeanVsLayer->SetDirectory(0);
  TH2D* hspectraHGSigmaVsLayer  = new TH2D( "hspectraHGSigmaVsLayer","Mean Ped High Gain vs CellID; layer; brd channel; #sigma_{Ped HG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGSigmaVsLayer->SetDirectory(0);
  TH1D* hMeanPedLGvsCellID      = new TH1D( "hMeanPedLG_vsCellID","mean Ped Low Gain vs CellID ; cell ID; #mu_{noise, LG} (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5);
  hMeanPedLGvsCellID->SetDirectory(0);
  TH2D* hspectraLGMeanVsLayer   = new TH2D( "hspectraLGMeanVsLayer","Mean Ped Low Gain vs CellID; layer; brd channel; #mu_{PED LG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGMeanVsLayer->SetDirectory(0);
  TH2D* hspectraLGSigmaVsLayer  = new TH2D( "hspectraLGSigmaVsLayer","Mean Ped Low Gain vs CellID; layer; brd channel; #sigma_{Ped LG} (arb. units)",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGSigmaVsLayer->SetDirectory(0);

  TH2D* hPedMeanHGvsLG          = new TH2D( "hPedMeanHGvsLG","Mean Ped High Gain vs Low Gain; #mu_{noise, HG} (arb. units); #mu_{noise, LG} (arb. units)", 500, 0, 250, 500, 0, 250);
  hPedMeanHGvsLG->SetDirectory(0);
  
  std::map<int,TileSpectra> hSpectra;
  std::map<int, TileSpectra>::iterator ithSpectra;

  std::cout << "Additional Output with histos being created: " << RootOutputNameHist.Data() << std::endl;
  if(Overwrite){
    std::cout << "recreating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"RECREATE");
  } else{
    std::cout << "newly creating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"CREATE");
  }
  // entering histoOutput file
  RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->cd("IndividualCells");

  RootOutput->cd();
  // Event loop to fill histograms & output tree
  std::cout << "N max layers: " << setup->GetNMaxLayer() << "\t columns: " <<  setup->GetNMaxColumn() << "\t row: " << setup->GetNMaxRow() << "\t module: " <<  setup->GetNMaxModule() << std::endl;
  if(TcalibIn) TcalibIn->GetEntry(0);
  // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }
  
  int evts=TdataIn->GetEntries();
  int runNr = -1;
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  ReadOut::Type typeRO = ReadOut::Type::Caen;
  
  for(int i=0; i<evts && i < maxEvents; i++){
    TdataIn->GetEntry(i);
    if (i == 0){
      runNr   = event.GetRunNumber();
      typeRO  = event.GetROtype();
      std::cout<< "original run numbers calib: "<<calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
      if (typeRO == ReadOut::Type::Caen) std::cout << "Read-out type CAEN" << std::endl;
      else{
        std::cout << "Read-out type HGCROC" << std::endl;
        hPedMeanHGvsLG->GetYaxis()->SetTitle("#mu_{noise, wave} (arb. units)");
        hPedMeanHGvsLG->GetXaxis()->SetTitle("#mu_{noise, 0th sample} (arb. units)");
        hPedMeanHGvsLG->SetTitle("Pedestal Eval 0th sample vs wave");
        hspectraLGvsCellID->SetYTitle("ADC (arb. units) all samples");
      }
      calib.SetRunNumberPed(runNr);
      calib.SetBeginRunTimePed(event.GetBeginRunTimeAlt());
      std::cout<< "reset run numbers calib: "<< calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
    }
    if (i%5000 == 0&& i > 0 && debug > 0) std::cout << "Reading " <<  i << "/" << evts << " events"<< std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      if (typeRO == ReadOut::Type::Caen) {
        Caen* aTile=(Caen*)event.GetTile(j);
        if (i == 0 && debug > 2){
          std::cout << ((TString)setup->DecodeCellID(aTile->GetCellID())).Data() << std::endl;
        }
        ithSpectra=hSpectra.find(aTile->GetCellID());
        if(ithSpectra!=hSpectra.end()){
          ithSpectra->second.Fill(aTile->GetADCLow(),aTile->GetADCHigh());
        } else {
          RootOutputHist->cd("IndividualCells");
          hSpectra[aTile->GetCellID()]=TileSpectra("1stExtraction",aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()), event.GetROtype(), debug);
          hSpectra[aTile->GetCellID()].Fill(aTile->GetADCLow(),aTile->GetADCHigh());
          RootOutput->cd();
        }
        hspectraHGvsCellID->Fill(aTile->GetCellID(), aTile->GetADCHigh());
        hspectraLGvsCellID->Fill(aTile->GetCellID(), aTile->GetADCLow());
      }
      else if (typeRO == ReadOut::Type::Hgcroc){ // Process HGCROC Data
        Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
        if (i == 0 && debug == 3 ){
          std::cout << ((TString)setup->DecodeCellID(aTile->GetCellID())).Data() << std::endl;
        }
        if (debug > 3 && aTile->GetCellID() == 2 ){
          std::bitset<10> pedBit(aTile->GetPedestal());
          std::bitset<5> pedBit5bit(aTile->GetPedestal());
          std::cout << aTile->GetPedestal() << "\t" << pedBit << "\t" << pedBit5bit << std::endl;
        }
        
        ithSpectra=hSpectra.find(aTile->GetCellID());
        if(ithSpectra!=hSpectra.end()){
          // Get the tile spectra if it already exists
          ithSpectra->second.FillExtPed(aTile->GetADCWaveform(),aTile->GetPedestal());
        } else {
          // Make a new tile spectra if it isn't found
          RootOutputHist->cd("IndividualCells");
          hSpectra[aTile->GetCellID()]= TileSpectra("1stExtraction", 2, aTile->GetCellID(), calib.GetTileCalib(aTile->GetCellID()), event.GetROtype(), debug);
          hSpectra[aTile->GetCellID()].FillExtPed(aTile->GetADCWaveform(),aTile->GetPedestal());
          RootOutput->cd();
        }
        // std::cout << "Cell ID: " << aTile->GetCellID() << ", Tile E: " << aTile->GetPedestal() << std::endl;
        hspectraHGvsCellID->Fill(aTile->GetCellID(), aTile->GetPedestal());
        for (int k = 0; k < (int)(aTile->GetADCWaveform()).size(); k++ ){
          hspectraLGvsCellID->Fill(aTile->GetCellID(),aTile->GetADCWaveform().at(k));
        }
      }
    }
    RootOutput->cd();
    TdataOut->Fill();
  }
  
  // fit pedestal
  double* parameters=new double[8];
  bool isGood  = true;
  bool isGood2 = true;

  for(ithSpectra=hSpectra.begin(); ithSpectra!=hSpectra.end(); ++ithSpectra){
    if ( debug > 2) std::cout << ((TString)setup->DecodeCellID(ithSpectra->second.GetCellID())).Data() << std::endl;
    // std::cerr << "Fitting noise for cell ID: " << ithSpectra->second.GetCellID() << std::endl;
    isGood=ithSpectra->second.FitNoise(parameters, yearData, false);
    if (!isGood && !(typeRO == ReadOut::Type::Hgcroc)) {
      std::cerr << "Noise fit failed for cell ID: " << ithSpectra->second.GetCellID() << std::endl;
      continue;
    }

    if (typeRO == ReadOut::Type::Hgcroc){
      isGood2=ithSpectra->second.FitPedConstWave(debug);

      if (!isGood && !isGood2) {
        std::cerr << "both noise fits failed " << ithSpectra->second.GetCellID() << std::endl;
        continue;
      } else {
        if (!isGood2) std::cerr << "Noise-wave form fit failed for cell ID: " << ithSpectra->second.GetCellID() << std::endl;
        if (!isGood){
          std::cerr << "1D Noise fit failed for cell ID: " << ithSpectra->second.GetCellID() << std::endl;
          parameters[4] = -1;
          parameters[5] = -1;
          parameters[6] = -1;
          parameters[7] = -1;
        }
      }
    }
    
    int layer     = setup->GetLayer(ithSpectra->second.GetCellID());
    int chInLayer = setup->GetChannelInLayer(ithSpectra->second.GetCellID());

    hMeanPedHGvsCellID->SetBinContent(hMeanPedHGvsCellID->FindBin(ithSpectra->second.GetCellID()), parameters[4]);
    hMeanPedHGvsCellID->SetBinError  (hMeanPedHGvsCellID->FindBin(ithSpectra->second.GetCellID()), parameters[6]);
    hspectraHGMeanVsLayer->SetBinContent(hspectraHGMeanVsLayer->FindBin(layer,chInLayer), parameters[4]);
    hspectraHGMeanVsLayer->SetBinError(hspectraHGMeanVsLayer->FindBin(layer,chInLayer), parameters[5]);
    hspectraHGSigmaVsLayer->SetBinContent(hspectraHGSigmaVsLayer->FindBin(layer,chInLayer), parameters[6]);
    hspectraHGSigmaVsLayer->SetBinError(hspectraHGSigmaVsLayer->FindBin(layer,chInLayer), parameters[7]);
    if (typeRO == ReadOut::Type::Caen){
      hMeanPedLGvsCellID->SetBinContent(hMeanPedLGvsCellID->FindBin(ithSpectra->second.GetCellID()), parameters[0]);
      hMeanPedLGvsCellID->SetBinError  (hMeanPedLGvsCellID->FindBin(ithSpectra->second.GetCellID()), parameters[2]);
      hspectraLGMeanVsLayer->SetBinContent(hspectraLGMeanVsLayer->FindBin(layer,chInLayer), parameters[0]);
      hspectraLGMeanVsLayer->SetBinError(hspectraLGMeanVsLayer->FindBin(layer,chInLayer), parameters[1]);
      hspectraLGSigmaVsLayer->SetBinContent(hspectraLGSigmaVsLayer->FindBin(layer,chInLayer), parameters[2]);
      hspectraLGSigmaVsLayer->SetBinError(hspectraLGSigmaVsLayer->FindBin(layer,chInLayer), parameters[3]);
      
      hPedMeanHGvsLG->Fill(parameters[4],parameters[0]);
    } else if (isGood2 && typeRO == ReadOut::Type::Hgcroc){
      hMeanPedLGvsCellID->SetBinContent(hMeanPedLGvsCellID->FindBin(ithSpectra->second.GetCellID()), calib.GetPedestalMeanL(ithSpectra->second.GetCellID()));
      hMeanPedLGvsCellID->SetBinError  (hMeanPedLGvsCellID->FindBin(ithSpectra->second.GetCellID()), calib.GetPedestalSigL(ithSpectra->second.GetCellID()));
      hspectraLGMeanVsLayer->SetBinContent(hspectraLGMeanVsLayer->FindBin(layer,chInLayer), calib.GetPedestalMeanL(ithSpectra->second.GetCellID()));
      hspectraLGMeanVsLayer->SetBinError(hspectraLGMeanVsLayer->FindBin(layer,chInLayer), calib.GetPedestalSigL(ithSpectra->second.GetCellID()));
      
      hPedMeanHGvsLG->Fill(parameters[4],calib.GetPedestalMeanL(ithSpectra->second.GetCellID()));
    }
  }
  
  RootOutput->cd();
  // write output tree (copy of raw data)
  TdataOut->Write();
  // write setup tree (copy of raw data)
  TsetupIn->CloneTree()->Write();
  
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }
  
  TcalibOut->Fill();
  TcalibOut->Write();
  RootOutput->Write();
  RootOutput->Close();
  
  // entering histoOutput file
  //RootOutputHist->cd();
  //RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->cd("IndividualCells");
  for(ithSpectra=hSpectra.begin(); ithSpectra!=hSpectra.end(); ++ithSpectra){
    ithSpectra->second.Write(true);
  }
  RootOutputHist->cd();
  hspectraHGvsCellID->Write();
  hMeanPedHGvsCellID->Write();
  hspectraHGMeanVsLayer->Write();
  hspectraHGSigmaVsLayer->Write();
  hPedMeanHGvsLG->Write();
  if (typeRO == ReadOut::Type::Caen){
    hspectraLGvsCellID->Write();
    hMeanPedLGvsCellID->Write();
    hspectraLGMeanVsLayer->Write();
    hspectraLGSigmaVsLayer->Write();
  } else {
    hspectraLGvsCellID->Write("hAllADC_vsCellID");
    hMeanPedLGvsCellID->GetYaxis()->SetTitle("#mu_{Ped ADC, wave} (arb. units)");
    hMeanPedLGvsCellID->Write("hMeanPedWave_vsCellID");
    hspectraLGMeanVsLayer->GetZaxis()->SetTitle("#mu_{Ped ADC, wave} (arb. units)");
    hspectraLGMeanVsLayer->Write("hspectraPedWaveMeanVsLayer");
  }
  // fill calib tree & write it
  // close open root files
  RootOutputHist->Write();
  RootOutputHist->Close();

  RootInput->Close();

  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find(runNr);
  
  // create directory for plot output
  TString outputDirPlots = GetPlotOutputDir();
  gSystem->Exec("mkdir -p "+outputDirPlots);
  
  double averagePedMeanHG = calib.GetAveragePedestalMeanHigh();
  double averagePedSigHG  = calib.GetAveragePedestalSigHigh();
  double averagePedMeanLG = calib.GetAveragePedestalMeanLow();
  double averagePedSigLG  = calib.GetAveragePedestalSigLow();

  //**********************************************************************
  // Create canvases for channel overview plotting
  //**********************************************************************
  Double_t textSizeRel = 0.035;
  StyleSettingsBasics("pdf");
  SetPlotStyle();
  
  TCanvas* canvas2DCorr = new TCanvas("canvasCorrPlots","",0,0,1450,1200);  // gives the page size
  DefaultCancasSettings( canvas2DCorr, 0.08, 0.13, 0.045, 0.07);
  canvas2DCorr->SetLogz();
  
  if (typeRO == ReadOut::Type::Hgcroc) hspectraHGvsCellID->GetYaxis()->SetTitle("Pedestal ADC (arb units)");
  PlotSimple2D( canvas2DCorr, hspectraHGvsCellID, 300, setup->GetMaxCellID()+1, textSizeRel, Form("%s/HG_Noise.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 5, kFALSE, "colz", true);
  
  
  if (typeRO == ReadOut::Type::Caen){
    PlotSimple2D( canvas2DCorr, hspectraLGvsCellID, 300, setup->GetMaxCellID()+1, textSizeRel, Form("%s/LG_Noise.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 5, kFALSE, "colz", true);
  } else {
    PlotSimple2D( canvas2DCorr, hspectraLGvsCellID, 300, setup->GetMaxCellID()+1, textSizeRel, Form("%s/AllSampleADC.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 5, kFALSE, "colz", true);
  }
  
  canvas2DCorr->SetLogz(0);
  PlotSimple2D( canvas2DCorr, hspectraHGMeanVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_NoiseMean.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 5, kFALSE, "colz", true, Form("#LT#mu_{HG}#GT = %2.2f", averagePedMeanHG));
  PlotSimple2D( canvas2DCorr, hspectraHGSigmaVsLayer,-10000, -10000, textSizeRel, Form("%s/HG_NoiseSigma.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 5,  kFALSE, "colz", true, Form("#LT#sigma_{HG}#GT = %2.2f", averagePedSigHG));
  PlotSimple2D( canvas2DCorr, hPedMeanHGvsLG, -10000, -10000, textSizeRel, Form("%s/PedMean_HG_LG.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 5, kFALSE, "colz", true, "");

  if (typeRO == ReadOut::Type::Caen){
    PlotSimple2D( canvas2DCorr, hspectraLGMeanVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_NoiseMean.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 5, kFALSE, "colz", true, Form("#LT#mu_{LG}#GT = %2.2f", averagePedMeanLG));
    PlotSimple2D( canvas2DCorr, hspectraLGSigmaVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_NoiseSigma.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 5, kFALSE, "colz", true, Form("#LT#sigma_{LG}#GT = %2.2f", averagePedSigLG));
  } else {
    PlotSimple2D( canvas2DCorr, hspectraLGMeanVsLayer, -10000, -10000, textSizeRel, Form("%s/PedWave_NoiseMean.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 5, kFALSE, "colz", true, Form("#LT#mu_{wave}#GT = %2.2f", averagePedMeanLG));
  }
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
 
  
  for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
    for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
      PlotNoiseWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                  hSpectra, setup, true, 0, 275, 1.2, l, m,
                                  Form("%s/Noise_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
      if (typeRO == ReadOut::Type::Caen){
        PlotNoiseWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                  hSpectra, setup, false, 0, 275, 1.2, l, m,
                                  Form("%s/Noise_LG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
      } else if (typeRO == ReadOut::Type::Hgcroc){
        PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, hSpectra, 0, it->second.samples+1, 300, l, m,
                                    Form("%s/Waveform_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
        PlotNoiseWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                  hSpectra, setup, false, 0, 275, 1.2, l, m,
                                  Form("%s/AllSampleADC_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
      }
    }
  }

  
  return true;
}

// ****************************************************************************
// extract pedestral from dedicated data run, no other data in run 
// ****************************************************************************
bool Analyses::TransferCalib(void){
  std::cout<<"Transfer calib"<<std::endl;
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
  
  int runNr = -1;

  std::map<int,short> bcmap;
  std::map<int,short>::iterator bcmapIte;
  if (CalcBadChannel == 1)
    bcmap = ReadExternalBadChannelMap();

  // *******************************************************************
  // ***** create additional output hist *******************************
  // *******************************************************************
  std::cout << "Additional Output with histos being created: " << RootOutputNameHist.Data() << std::endl;
  if(Overwrite){
    std::cout << "recreating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"RECREATE");
  } else{
    std::cout << "newly creating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"CREATE");
  }
  TH2D* hspectraADCvsCellID      = new TH2D( "hspectraADCvsCellID","ADC spectrums CellID; cell ID; ADC (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 1024,0,1024);
  hspectraADCvsCellID->SetDirectory(0);
  TH2D* hspectraTOTvsCellID      = new TH2D( "hspectraTOTvsCellID","TOT spectrums CellID; cell ID; TOT (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4096,0,4096);
  hspectraTOTvsCellID->SetDirectory(0);
  TH2D* hspectraTOAvsCellID      = new TH2D( "hspectraTOAvsCellID","TOA spectrums CellID; cell ID; TOA (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 1024,0,1024);
  hspectraTOAvsCellID->SetDirectory(0);
  
  TH2D* hNCellsWTOAVsTotADC          = new TH2D( "hNCellsWTOAVsTotADC","NCells above TOA vs tot ADC; NCells above TOA; #Sigma(ADC) (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 5000,0,10000);
  hNCellsWTOAVsTotADC->SetDirectory(0);
  TH2D* hNCellsWmADCVsTotADC          = new TH2D( "hNCellsWmADCVsTotADC","NCells w. ADC > 10 vs tot ADC; NCells above min ADC; #Sigma(ADC) (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 5000,0,10000);
  hNCellsWmADCVsTotADC->SetDirectory(0);
  
  TH2D* hspectraADCPedvsCellID      = new TH2D( "hspectraADCPedvsCellID","Pedestal ADC spectrums CellID; cell ID; ADC (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 1024,0,1024);
  hspectraADCPedvsCellID->SetDirectory(0);
  
  int maxChannelPerLayer = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1);
  TH2D* hHighestADCAbovePedVsLayer   = new TH2D( "hHighestEAbovePedVsLayer","Highest ADC above PED; layer; brd channel; #Sigma(ADC) (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hHighestADCAbovePedVsLayer->SetDirectory(0);
  hHighestADCAbovePedVsLayer->Sumw2();
  
  RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->mkdir("IndividualCellsTrigg");
  RootOutputHist->cd("IndividualCells");
  
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Migrad");  
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  ReadOut::Type typeRO = ReadOut::Type::Caen;
  
  // setup waveform builder for HGCROC data
  waveform_fit_base *waveform_builder = nullptr;
  waveform_builder = new max_sample_fit();
  double minNSigma = 5;
  int nCellsAboveTOA  = 0;
  int nCellsAboveMADC = 0;
  double totADCs      = 0.;
  for(int i=0; i<evts && i < maxEvents ; i++){
    if (i%5000 == 0&& i > 0 && debug > 0) std::cout << "Reading " <<  i << "/" << evts << " events"<< std::endl;
    if (debug > 2 && typeRO == ReadOut::Type::Hgcroc){
      std::cout << "************************************* NEW EVENT " << i << "  *********************************" << std::endl;
    }
    TdataIn->GetEntry(i);
    if (i == 0){
      runNr   = event.GetRunNumber();
      typeRO  = event.GetROtype();
      std::cout<< "original run numbers calib: "<<calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
      calib.SetRunNumber(runNr);
      calib.SetBeginRunTime(event.GetBeginRunTimeAlt());
      std::cout<< "reset run numbers calib: "<< calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
    }
   
    nCellsAboveTOA  = 0;
    nCellsAboveMADC = 0;
    totADCs         = 0.;
    // if (CalcBadChannel > 0 || ExtPlot > 0){
      for(int j=0; j<event.GetNTiles(); j++){
        if (typeRO == ReadOut::Type::Caen) {
          Caen* aTile=(Caen*)event.GetTile(j);
          ithSpectra=hSpectra.find(aTile->GetCellID());
          double hgCorr = aTile->GetADCHigh()-calib.GetPedestalMeanH(aTile->GetCellID());
          double lgCorr = aTile->GetADCLow()-calib.GetPedestalMeanL(aTile->GetCellID());
          
          if(ithSpectra!=hSpectra.end()){
            ithSpectra->second.FillExt(lgCorr,hgCorr, 0., 0.);
          } else {
            RootOutputHist->cd("IndividualCells");
            hSpectra[aTile->GetCellID()]=TileSpectra("ped",2,aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
            hSpectra[aTile->GetCellID()].FillExt(lgCorr,hgCorr, 0., 0.);
            RootOutput->cd();
          }
        } else if (typeRO == ReadOut::Type::Hgcroc) {
          Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
          ithSpectra=hSpectra.find(aTile->GetCellID());
          ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
          
          double ped = calib.GetPedestalMeanL(aTile->GetCellID());
          if (ped == -1000){
            ped = calib.GetPedestalMeanH(aTile->GetCellID());
            if (ped == -1000){
              ped = aTile->GetPedestal();
            }
          }
          waveform_builder->set_waveform(aTile->GetADCWaveform());
          waveform_builder->fit_with_average_ped(ped);
          aTile->SetIntegratedADC(waveform_builder->get_E());
          aTile->SetPedestal(waveform_builder->get_pedestal());
          
          double adc = aTile->GetIntegratedADC();
          double tot = aTile->GetTOT();
          double toa = aTile->GetTOA();
          totADCs         = totADCs+adc;
          if (adc > 10)
            nCellsAboveMADC++; 
          if (toa > 0)
            nCellsAboveTOA++;
          
          hspectraADCvsCellID->Fill(aTile->GetCellID(),adc);
          hspectraADCPedvsCellID->Fill(aTile->GetCellID(),aTile->GetPedestal());
          hspectraTOTvsCellID->Fill(aTile->GetCellID(),tot);
          hspectraTOAvsCellID->Fill(aTile->GetCellID(),toa);
          
          int layer     = setup->GetLayer(aTile->GetCellID());
          int chInLayer = setup->GetChannelInLayer(aTile->GetCellID());          
          if (debug > 2){
            // if (debug > 3 || adc > minNSigma*calib.GetPedestalSigL(aTile->GetCellID()) || aTile->GetTOA() > 0 ){
            if (debug > 3 || adc > 10 || aTile->GetTOA() > 0 ){
              std::cout << "Cell ID:" << aTile->GetCellID()<< "\t" << layer <<"\t" << chInLayer << "\t RO channel:\t" << setup->GetROchannel(aTile->GetCellID()) << "\t" << calib.GetPedestalMeanH(aTile->GetCellID()) << "\t" << calib.GetPedestalMeanL(aTile->GetCellID()) << "\t" << calib.GetPedestalSigL(aTile->GetCellID());
              // if (debug > 3 || adc > minNSigma*calib.GetPedestalSigL(aTile->GetCellID()) || aTile->GetTOA() > 0 ){
                std::cout << "\n \tADC-wave " ;
                for (int k = 0; k < (int)aTile->GetADCWaveform().size(); k++ ){
                  std::cout << aTile->GetADCWaveform().at(k) << "\t" ;
                }
                // std::cout << "\n \tTOT-Wave ";
                // for (int k = 0; k < (int)aTile->GetTOTWaveform().size(); k++ ){
                //   std::cout << aTile->GetTOTWaveform().at(k) << "\t" ;
                // }
                std::cout << "\n \tTOA-Wave ";
                for (int k = 0; k < (int)aTile->GetTOAWaveform().size(); k++ ){
                  std::cout << aTile->GetTOAWaveform().at(k) << "\t" ;
                }
              std::cout <<"\n\t\t\t";
              for (int k = 0; k < (int)aTile->GetTOAWaveform().size(); k++ )
                std::cout <<"\t";  
              std::cout << " integ: "<<adc <<"\t"<< aTile->GetTOT() << "\t" << aTile->GetTOA() << std::endl;
            }
          }
          if(ithSpectra!=hSpectra.end()){
            ithSpectra->second.FillExt(tot,adc, 0., 0.);
            ithSpectra->second.FillWaveform(aTile->GetADCWaveform(),0);
          } else {
            RootOutputHist->cd("IndividualCells");
            hSpectra[aTile->GetCellID()]=TileSpectra("ped",2,aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
            hSpectra[aTile->GetCellID()].FillExt(tot,adc, 0., 0.);
            hSpectra[aTile->GetCellID()].FillWaveform(aTile->GetADCWaveform(),0);
            RootOutput->cd();
          }
                
          if (toa > 0){      
          // if (adc > 10 & toa > 0){
              // if (adc > minNSigma*calib.GetPedestalSigL(aTile->GetCellID()))
              hHighestADCAbovePedVsLayer->Fill(layer,chInLayer, adc);
            
            if(ithSpectraTrigg!=hSpectraTrigg.end()){
              ithSpectraTrigg->second.FillExt(tot,adc, 0., 0.);
              ithSpectraTrigg->second.FillWaveform(aTile->GetADCWaveform(),0);
            } else {
              RootOutputHist->cd("IndividualCellsTrigg");
              hSpectraTrigg[aTile->GetCellID()]=TileSpectra("signal",2,aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
              hSpectraTrigg[aTile->GetCellID()].FillExt(tot,adc, 0., 0.);
              hSpectraTrigg[aTile->GetCellID()].FillWaveform(aTile->GetADCWaveform(),0);
              RootOutput->cd();
            }
          }
        }
      }
      if (typeRO == ReadOut::Type::Hgcroc) {
        hNCellsWmADCVsTotADC->Fill(nCellsAboveMADC, totADCs);
        hNCellsWTOAVsTotADC->Fill(nCellsAboveTOA, totADCs);
      }
    // }
    RootOutput->cd();
    TdataOut->Fill();
  }
  //RootOutput->cd();
  TdataOut->Write();
  TsetupIn->CloneTree()->Write();

  std::map<int,RunInfo>::iterator it=ri.find(runNr);
  TString outputDirPlots = GetPlotOutputDir();
  Double_t textSizeRel = 0.035;

  if (CalcBadChannel > 0 || ExtPlot > 0) {
    gSystem->Exec("mkdir -p "+outputDirPlots);
    StyleSettingsBasics("pdf");
    SetPlotStyle();  
  }
  
  if (CalcBadChannel > 0 ){
    //***********************************************************************************************
    //***** Monitoring histos for calib parameters & fits results of 1st iteration ******************
    //***********************************************************************************************
    int maxChannelPerLayer        = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1);
    // monitoring applied pedestals
    TH1D* hBCvsCellID      = new TH1D( "hBC_vsCellID","Bad Channel vs CellID ; cell ID; BC flag ",
                                              setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5);
    hBCvsCellID->SetDirectory(0);
    TH2D* hBCVsLayer   = new TH2D( "hBCVsLayer","Bad Channel Map; layer; brd channel; BC flag  ",
                                              setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
    hBCVsLayer->SetDirectory(0);

    int currCells = 0;
    if ( debug > 0 && CalcBadChannel == 2)
      std::cout << "============================== starting bad channel extraction" << std::endl;
    if ( debug > 0 && CalcBadChannel == 1)
      std::cout << "============================== setting bad channel according to external map" << std::endl;

    for(ithSpectra=hSpectra.begin(); ithSpectra!=hSpectra.end(); ++ithSpectra){
      if (currCells%20 == 0 && currCells > 0 && debug > 0)
        std::cout << "============================== cell " <<  currCells << " / " << hSpectra.size() << " cells" << std::endl;
      currCells++;
      short bc   = 3;
      if (CalcBadChannel == 2){
        bc        = ithSpectra->second.DetermineBadChannel();
      } else if (CalcBadChannel == 1 && bcmap.size() > 0 ) {
        bcmapIte  = bcmap.find(ithSpectra->second.GetCellID());
        if(bcmapIte!=bcmap.end())
          bc        = bcmapIte->second;
        else 
          bc        = 3;
      } 
      
      long cellID   = ithSpectra->second.GetCellID();
      if (CalcBadChannel == 1)
        ithSpectra->second.SetBadChannelInCalib(bc);
      
      // initializing pedestal fits from calib file
      ithSpectra->second.InitializeNoiseFitsFromCalib();
      
      int layer     = setup->GetLayer(cellID);
      int chInLayer = setup->GetChannelInLayer(cellID);
      if (debug > 0 && bc > -1 && bc < 3)
        std::cout << "\t" << cellID << "\t" << layer << "\t" << setup->GetRow(cellID) << "\t" << setup->GetColumn(cellID)<< "\t" << setup->GetModule(cellID) << " - quality flag: " << bc << "\t" << calib.GetBadChannel(cellID) << "\t ped H: " << calib.GetPedestalMeanH(cellID) << "\t ped L: " << calib.GetPedestalMeanL(cellID)<< std::endl;

      hBCvsCellID->SetBinContent(hBCvsCellID->FindBin(cellID), calib.GetBadChannel(cellID));
      int bin2D     = hBCVsLayer->FindBin(layer,chInLayer);
      hBCVsLayer->SetBinContent(bin2D, calib.GetBadChannel(cellID));
      
      if (bc < 2 && typeRO == ReadOut::Type::Hgcroc){
        hHighestADCAbovePedVsLayer->SetBinContent(bin2D, 0);
      }
    }
    hBCvsCellID->Write();
    hBCVsLayer->Write();

    //**********************************************************************
    // Create canvases for channel overview plotting
    //**********************************************************************
    // Get run info object
    // create directory for plot output
  
    TCanvas* canvas2DCorr = new TCanvas("canvasCorrPlots","",0,0,1450,1300);  // gives the page size
    DefaultCancasSettings( canvas2DCorr, 0.08, 0.13, 0.045, 0.07);

    canvas2DCorr->SetLogz(0);
    
    PlotSimple2DZRange( canvas2DCorr, hBCVsLayer, -10000, -10000, -0.1, 3.1, textSizeRel, Form("%s/BadChannelMap.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, "colz", true);    
    calib.SetBCCalib(true);
  }
    
  if (ExtPlot > 0){
    
    if (typeRO == ReadOut::Type::Hgcroc){
      TCanvas* canvas2DSigQA = new TCanvas("canvas2DSigQA","",0,0,1450,1300);  // gives the page size
      DefaultCancasSettings( canvas2DSigQA, 0.08, 0.13, 0.045, 0.07);

      canvas2DSigQA->SetLogz(1);
      PlotSimple2DZRange( canvas2DSigQA, hHighestADCAbovePedVsLayer, -10000, -10000, 0.1, 20000, textSizeRel, Form("%s/MaxADCAboveNoise_vsLayer.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, "colz", true);    
    }

    Double_t maxHG = 600;
    Double_t maxLG = 200;
    calib.PrintGlobalInfo();  
    
    std::cout << "row max: " << setup->GetNMaxRow() << "\t column max: "  << setup->GetNMaxColumn() << std::endl;
    
    if (setup->GetNMaxRow()+1 == 2 && setup->GetNMaxColumn()+1 == 4){
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

      std::cout << "plotting single  8M layers" << std::endl;
      for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
        for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
          if (l%10 == 0 && l > 0 && debug > 0)
            std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
          if (typeRO == ReadOut::Type::Caen) {
            PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                                textSizePixel, hSpectra, -20, 340, 4000, l, m,
                                Form("%s/LGHG2D_Corr_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          } else {
            PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                                textSizePixel, hSpectra, 0, it->second.samples+1, 1000, l, m,
                                Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(),m,  l, plotSuffix.Data()), it->second);
            PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                              textSizePixel, hSpectraTrigg, 0, it->second.samples+1, 1000, l, 0,
                              Form("%s/WaveformSignal_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);            
          }
          if (ExtPlot > 1){
            PlotNoiseWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                        hSpectra, setup, true, -100, maxHG, 1.2, l, m,
                                        Form("%s/SpectraWithNoiseFit_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
            if (typeRO == ReadOut::Type::Caen){
              PlotNoiseWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                          hSpectra, setup, false, -20, maxLG, 1.2, l, m,
                                          Form("%s/SpectraWithNoiseFit_LG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
            }
          }
        }
      }
      std::cout << "ending plotting single 8M layers" << std::endl;
    } else if (setup->GetNMaxRow()+1 == 1 && setup->GetNMaxColumn()+1 == 2){
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
      TCanvas* canvas2Panel;
      TPad* pad2Panel[2];
      Double_t topRCornerX[2];
      Double_t topRCornerY[2];
      Int_t textSizePixel = 30;
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
          if (typeRO == ReadOut::Type::Caen) {
            PlotCorr2D2MLayer(canvas2PanelProf,pad2PanelProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                                textSizePixel, hSpectra, -20, 340, 4000, l, m,
                                Form("%s/LGHG2D_Corr_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          } else {
            PlotCorr2D2MLayer(canvas2PanelProf,pad2PanelProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                                textSizePixel, hSpectra, 0, it->second.samples+1, 1000, l, m,
                                Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(),m,  l, plotSuffix.Data()), it->second);
            PlotCorr2D2MLayer(canvas2PanelProf,pad2PanelProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                              textSizePixel, hSpectraTrigg, 0, it->second.samples+1, 1000, l, 0,
                              Form("%s/WaveformSignal_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);            
          }
          if (ExtPlot > 1){
            PlotNoiseWithFits2MLayer (canvas2Panel,pad2Panel, topRCornerX, topRCornerY, relSizeP, textSizePixel, 
                                        hSpectra, setup, true, -100, maxHG, 1.2, l, m,
                                        Form("%s/SpectraWithNoiseFit_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
            if (typeRO == ReadOut::Type::Caen){
              PlotNoiseWithFits2MLayer (canvas2Panel,pad2Panel, topRCornerX, topRCornerY, relSizeP, textSizePixel, 
                                          hSpectra, setup, false, -20, maxLG, 1.2, l, m,
                                          Form("%s/SpectraWithNoiseFit_LG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
            }
          }          
        }
      }          
    }
  }
  
  RootOutput->cd();
  
  std::cout<<"What is the value? <ped mean high>: "<<calib.GetAveragePedestalMeanHigh() << "\t good channels: " << calib.GetNumberOfChannelsWithBCflag(3) <<std::endl;
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }
  
  TcalibOut->Fill();
  TcalibOut->Write();
  
  RootOutput->Close();
  RootOutputHist->cd();
  if (typeRO == ReadOut::Type::Hgcroc){
     hspectraADCvsCellID->Write();
     hspectraADCPedvsCellID->Write();
     hspectraTOTvsCellID->Write();
     hspectraTOAvsCellID->Write();
     hHighestADCAbovePedVsLayer->Write();
     hNCellsWmADCVsTotADC->Write();
     hNCellsWTOAVsTotADC->Write();
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
// Analyse waveform
// ****************************************************************************
bool Analyses::AnalyseWaveForm(void){
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
  
  int runNr = -1;

  std::map<int,short> bcmap;
  std::map<int,short>::iterator bcmapIte;
  if (CalcBadChannel == 1)
    bcmap = ReadExternalBadChannelMap();

  // *******************************************************************
  // ***** create additional output hist *******************************
  // *******************************************************************
  std::cout << "Additional Output with histos being created: " << RootOutputNameHist.Data() << std::endl;
  if(Overwrite){
    std::cout << "recreating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"RECREATE");
  } else{
    std::cout << "newly creating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"CREATE");
  }

  int maxChannelPerLayer = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1);
  TH2D* hHighestADCAbovePedVsLayer   = new TH2D( "hHighestEAbovePedVsLayer","Highest ADC above PED; layer; brd channel; #Sigma(ADC) (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hHighestADCAbovePedVsLayer->SetDirectory(0);
  hHighestADCAbovePedVsLayer->Sumw2();
  
  RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->mkdir("IndividualCellsTrigg");
  RootOutputHist->cd("IndividualCells");
  
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Migrad");  
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  ReadOut::Type typeRO = ReadOut::Type::Caen;
  
  // setup waveform builder for HGCROC data
  waveform_fit_base *waveform_builder = nullptr;
  waveform_builder = new max_sample_fit();
  double minNSigma = 5;
  int nCellsAboveTOA  = 0;
  int nCellsAboveMADC = 0;
  double totADCs      = 0.;
  for(int i=0; i<evts && i < maxEvents ; i++){
    if (i%5000 == 0&& i > 0 && debug > 0) std::cout << "Reading " <<  i << "/" << evts << " events"<< std::endl;
    if (debug > 2 && typeRO == ReadOut::Type::Hgcroc){
      std::cout << "************************************* NEW EVENT " << i << "  *********************************" << std::endl;
    }
    TdataIn->GetEntry(i);
    if (i == 0){
      runNr   = event.GetRunNumber();
      typeRO  = event.GetROtype();
      std::cout<< "original run numbers calib: "<<calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
      calib.SetRunNumber(runNr);
      calib.SetBeginRunTime(event.GetBeginRunTimeAlt());
      std::cout<< "reset run numbers calib: "<< calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
    }
   
    nCellsAboveTOA  = 0;
    nCellsAboveMADC = 0;
    totADCs         = 0.;
    for(int j=0; j<event.GetNTiles(); j++){
      if (typeRO == ReadOut::Type::Caen) {
        return false;
      } else if (typeRO == ReadOut::Type::Hgcroc) {
        Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
        ithSpectra=hSpectra.find(aTile->GetCellID());
        ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
        
        double ped = calib.GetPedestalMeanL(aTile->GetCellID());
        if (ped == -1000){
          ped = calib.GetPedestalMeanH(aTile->GetCellID());
          if (ped == -1000){
            ped = aTile->GetPedestal();
          }
        }
        waveform_builder->set_waveform(aTile->GetADCWaveform());
        waveform_builder->fit_with_average_ped(ped);
        aTile->SetIntegratedADC(waveform_builder->get_E());
        aTile->SetPedestal(waveform_builder->get_pedestal());
        
        double adc = aTile->GetIntegratedADC();
        double tot = aTile->GetTOT();
        double toa = aTile->GetTOA();
        totADCs         = totADCs+adc;
        if (adc > 10)
          nCellsAboveMADC++; 
        if (toa > 0)
          nCellsAboveTOA++;
                  
        Int_t nSampTOA = 0;
        for (int k = 0; k < (int)aTile->GetTOAWaveform().size(); k++ ){
          if (aTile->GetTOAWaveform().at(k) > 0){
            nSampTOA = k;
            break;
          }
        }
        int lowestTOASample = 5;
        double toaCorr = ((nSampTOA-lowestTOASample)*25000+toa*25)/25;      
        
        int layer     = setup->GetLayer(aTile->GetCellID());
        int chInLayer = setup->GetChannelInLayer(aTile->GetCellID());          
        if (debug > 2){
          // if (debug > 3 || adc > minNSigma*calib.GetPedestalSigL(aTile->GetCellID()) || aTile->GetTOA() > 0 ){
          if (debug > 3 ){ //|| adc > 10 || aTile->GetTOA() > 0 
            std::cout << "Cell ID:" << aTile->GetCellID()<< "\t" << layer <<"\t" << chInLayer << "\t RO channel:\t" << setup->GetROchannel(aTile->GetCellID()) << "\t" << calib.GetPedestalMeanH(aTile->GetCellID()) << "\t" << calib.GetPedestalMeanL(aTile->GetCellID()) << "\t" << calib.GetPedestalSigL(aTile->GetCellID());
            // if (debug > 3 || adc > minNSigma*calib.GetPedestalSigL(aTile->GetCellID()) || aTile->GetTOA() > 0 ){
              std::cout << "\n \tADC-wave " ;
              for (int k = 0; k < (int)aTile->GetADCWaveform().size(); k++ ){
                std::cout << aTile->GetADCWaveform().at(k) << "\t" ;
              }
              // std::cout << "\n \tTOT-Wave ";
              // for (int k = 0; k < (int)aTile->GetTOTWaveform().size(); k++ ){
              //   std::cout << aTile->GetTOTWaveform().at(k) << "\t" ;
              // }
              std::cout << "\n \tTOA-Wave ";
              for (int k = 0; k < (int)aTile->GetTOAWaveform().size(); k++ ){
                std::cout << aTile->GetTOAWaveform().at(k) << "\t" ;
              }
            std::cout <<"\n\t\t\t";
            for (int k = 0; k < (int)aTile->GetTOAWaveform().size(); k++ )
              std::cout <<"\t";  
            std::cout << " integ: "<<adc <<"\t"<< aTile->GetTOT() << "\t" << aTile->GetTOA() << "\t nTOA = " << nSampTOA << "\t corr: " << toaCorr<< std::endl;
          }
        }
        if(ithSpectra!=hSpectra.end()){
          ithSpectra->second.FillExt(tot,adc, 0., 0.);
          // if (((nSampTOA == 5 && toa < 500) || (nSampTOA == 6)))      // needed for summing tests
            ithSpectra->second.FillWaveformVsTime(aTile->GetADCWaveform(), toa, calib.GetPedestalMeanH(aTile->GetCellID()));
        } else {
          RootOutputHist->cd("IndividualCells");
          hSpectra[aTile->GetCellID()]=TileSpectra("full",3,aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
          hSpectra[aTile->GetCellID()].FillExt(tot,adc, 0., 0.);
          // if (((nSampTOA == 5 && toa < 500) || (nSampTOA == 6)))      // needed for summing tests
            hSpectra[aTile->GetCellID()].FillWaveformVsTime(aTile->GetADCWaveform(), toa, calib.GetPedestalMeanH(aTile->GetCellID()));
          RootOutput->cd();
        }
        // if (toa > 0 && ((nSampTOA == 5 && toa < 500) || (nSampTOA == 6))){      // needed for summing tests
        if (toa > 0 ){      
        // // if (adc > 10 & toa > 0){
            // if (adc > minNSigma*calib.GetPedestalSigL(aTile->GetCellID()))
            hHighestADCAbovePedVsLayer->Fill(layer,chInLayer, adc);
          
          if(ithSpectraTrigg!=hSpectraTrigg.end()){
            ithSpectraTrigg->second.FillExt(tot,adc, 0., 0.);
            ithSpectraTrigg->second.FillWaveformVsTime(aTile->GetADCWaveform(), toa, calib.GetPedestalMeanH(aTile->GetCellID()));
          } else {
            RootOutputHist->cd("IndividualCellsTrigg");
            hSpectraTrigg[aTile->GetCellID()]=TileSpectra("signal",3,aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
            hSpectraTrigg[aTile->GetCellID()].FillExt(tot,adc, 0., 0.);
            hSpectraTrigg[aTile->GetCellID()].FillWaveformVsTime(aTile->GetADCWaveform(), toa, calib.GetPedestalMeanH(aTile->GetCellID()));
            RootOutput->cd();
          }
        }
      }
    }
    RootOutput->cd();
    TdataOut->Fill();
  }
  //RootOutput->cd();
  TdataOut->Write();
  TsetupIn->CloneTree()->Write();

  std::map<int,RunInfo>::iterator it=ri.find(runNr);
  TString outputDirPlots = GetPlotOutputDir();
  Double_t textSizeRel = 0.035;

  if ( ExtPlot > 0) {
    gSystem->Exec("mkdir -p "+outputDirPlots);
    StyleSettingsBasics("pdf");
    SetPlotStyle();  
  }
  
  if (ExtPlot > 0){
    
    if (typeRO == ReadOut::Type::Hgcroc){
      TCanvas* canvas2DSigQA = new TCanvas("canvas2DSigQA","",0,0,1450,1300);  // gives the page size
      DefaultCancasSettings( canvas2DSigQA, 0.08, 0.13, 0.045, 0.07);

      canvas2DSigQA->SetLogz(1);
      PlotSimple2DZRange( canvas2DSigQA, hHighestADCAbovePedVsLayer, -10000, -10000, 0.1, 20000, textSizeRel, Form("%s/MaxADCAboveNoise_vsLayer.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, "colz", true);    
    }

    Double_t maxHG = 600;
    Double_t maxLG = 200;
    calib.PrintGlobalInfo();  
    
    std::cout << "row max: " << setup->GetNMaxRow() << "\t column max: "  << setup->GetNMaxColumn() << std::endl;
    
    if (setup->GetNMaxRow()+1 == 2 && setup->GetNMaxColumn()+1 == 4){
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

      std::cout << "plotting single  8M layers" << std::endl;
      for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){    
        for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){
          if (l%10 == 0 && l > 0 && debug > 0)
            std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;     
          PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                              textSizePixel, hSpectra, 0, it->second.samples+1, 1000, l, m,
                              Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(),m,  l, plotSuffix.Data()), it->second);
          PlotCorr2D8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf,
                            textSizePixel, hSpectraTrigg, 0, it->second.samples+1, 1000, l, 0,
                            Form("%s/WaveformSignal_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);            
          if (ExtPlot > 1){
            PlotNoiseWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                        hSpectra, setup, true, -100, maxHG, 1.2, l, m,
                                        Form("%s/SpectraWithNoiseFit_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          }
        }
      }
      std::cout << "ending plotting single 8M layers" << std::endl;
    } else if (setup->GetNMaxRow()+1 == 1 && setup->GetNMaxColumn()+1 == 2){
      //***********************************************************************************************************
      //********************************* 2 Panel overview plot  **************************************************
      //***********************************************************************************************************
      //*****************************************************************
        // Test beam geometry (beam coming from viewer)
        //===========================================================
        //||    1 (0)    ||    2 (1)   || row 0
        //===========================================================
        //    col 0     col 1 
        // rebuild pad geom in similar way (numbering -1)
      //*****************************************************************
      TCanvas* canvas2Panel;
      TPad* pad2Panel[2];
      Double_t topRCornerX[2];
      Double_t topRCornerY[2];
      Int_t textSizePixel = 30;
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
                              textSizePixel, hSpectra, 0, it->second.samples+1, 1000, l, m,
                              Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(),m,  l, plotSuffix.Data()), it->second);
          PlotCorr2D2MLayer(canvas2PanelProf,pad2PanelProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                            textSizePixel, hSpectraTrigg, 0, it->second.samples+1, 1000, l, 0,
                            Form("%s/WaveformSignal_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);            
          if (ExtPlot > 1){
            PlotNoiseWithFits2MLayer (canvas2Panel,pad2Panel, topRCornerX, topRCornerY, relSizeP, textSizePixel, 
                                        hSpectra, setup, true, -100, maxHG, 1.2, l, m,
                                        Form("%s/SpectraWithNoiseFit_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          }          
        }
      }          
    } else if (setup->GetNMaxRow()+1 == 1 && setup->GetNMaxColumn()+1 == 1){
      //***********************************************************************************************************
      //********************************* Single tile overview plot  **************************************************
      //***********************************************************************************************************
      TCanvas* canvasLayer = new TCanvas("canvasLayer","",0,0,620,600);
      DrawCanvasSettings( canvasLayer,0.12, 0.03, 0.03, 0.1);
      Double_t topRCornerX = 0.95;
      Double_t topRCornerY = 0.95;
      Int_t textSizePixel = 30;
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
          // PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
          //                     textSizePixel, hSpectra, 0, it->second.samples+1, 1000, l, m,
          //                     Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(),m,  l, plotSuffix.Data()), it->second);
          // PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
          //                   textSizePixel, hSpectraTrigg, 0, it->second.samples+1, 1000, l, 0,
          //                   Form("%s/WaveformSignal_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);            
          PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                              textSizePixel, hSpectra, 0, 500000, 1000, l, m,
                              Form("%s/Waveform_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(),m,  l, plotSuffix.Data()), it->second);
          PlotCorr2D1MLayer(canvasLayerProf, topRCornerXProf, topRCornerYProf, relSizePProf,
                            textSizePixel, hSpectraTrigg, -25000, 500000-25000, 1000, l, 0,
                            Form("%s/WaveformSignal_Mod_%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);            
          if (ExtPlot > 1){
            PlotNoiseWithFits1MLayer (canvasLayer, topRCornerX, topRCornerY, relSizeP, textSizePixel, 
                                        hSpectra, setup, true, -100, maxHG, 1.2, l, m,
                                        Form("%s/SpectraWithNoiseFit_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          }          
        }
      }          
    }
  }
  
  RootOutput->cd();
  
  std::cout<<"What is the value? <ped mean high>: "<<calib.GetAveragePedestalMeanHigh() << "\t good channels: " << calib.GetNumberOfChannelsWithBCflag(3) <<std::endl;
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }
  
  TcalibOut->Fill();
  TcalibOut->Write();
  
  RootOutput->Close();
  RootOutputHist->cd();
  if (typeRO == ReadOut::Type::Hgcroc){
     hHighestADCAbovePedVsLayer->Write();
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



//***********************************************************************************************
//*********************** intial scaling calculation function *********************************
//***********************************************************************************************
bool Analyses::GetScaling(void){
  std::cout<<"GetScaling"<<std::endl;
  
  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  
  std::map<int,TileSpectra> hSpectra;
  std::map<int,TileSpectra> hSpectraTrigg;
  std::map<int, TileSpectra>::iterator ithSpectra;
  std::map<int, TileSpectra>::iterator ithSpectraTrigg;
  
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Migrad");  
  
  std::cout << "Additional Output with histos being created: " << RootOutputNameHist.Data() << std::endl;
  if(Overwrite){
    std::cout << "recreating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"RECREATE");
  } else{
    std::cout << "newly creating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"CREATE");
  }
    
  //***********************************************************************************************
  //************************* first pass over tree to extract spectra *****************************
  //***********************************************************************************************
  // entering histoOutput file
  RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->cd("IndividualCells");

  TcalibIn->GetEntry(0);
  // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }
  
  int evts=TdataIn->GetEntries();
  int runNr = -1;
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  ReadOut::Type typeRO = ReadOut::Type::Caen;
  int evtDeb = 5000;
  for(int i=0; i<evts && i < maxEvents; i++){
    TdataIn->GetEntry(i);
    if (i == 0){
      runNr = event.GetRunNumber();
      typeRO = event.GetROtype();
      if (typeRO != ReadOut::Type::Caen)
        evtDeb = 400;
      std::cout<< "Total number of events: " << evts << std::endl;
      std::cout<< "original run numbers calib: "<<calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
      calib.SetRunNumberMip(runNr);
      calib.SetBeginRunTimeMip(event.GetBeginRunTimeAlt());
      std::cout<< "reset run numbers calib: "<< calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
    }
    if (i%evtDeb == 0 && i > 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    if (i == 0 && debug > 2) std::cout << "decoding cell IDs" << std::endl ;
    for(int j=0; j<event.GetNTiles(); j++){
      
      if (typeRO == ReadOut::Type::Caen) {
        Caen* aTile=(Caen*)event.GetTile(j);
        if (i == 0 && debug > 2) std::cout << ((TString)setup->DecodeCellID(aTile->GetCellID())).Data() << std::endl;
        
        ithSpectra=hSpectra.find(aTile->GetCellID());
        double hgCorr = aTile->GetADCHigh()-calib.GetPedestalMeanH(aTile->GetCellID());
        double lgCorr = aTile->GetADCLow()-calib.GetPedestalMeanL(aTile->GetCellID());
            
        if(ithSpectra!=hSpectra.end()){
          ithSpectra->second.FillSpectra(lgCorr,hgCorr);
          if (hgCorr > 3*calib.GetPedestalSigH(aTile->GetCellID()) && lgCorr > 3*calib.GetPedestalSigL(aTile->GetCellID()) && hgCorr < 3900 )
            ithSpectra->second.FillCorr(lgCorr,hgCorr);
        } else {
          RootOutputHist->cd("IndividualCells");
          hSpectra[aTile->GetCellID()]=TileSpectra("mip1st",aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(), debug);
          hSpectra[aTile->GetCellID()].FillSpectra(lgCorr,hgCorr);;
          if (hgCorr > 3*calib.GetPedestalSigH(aTile->GetCellID()) && lgCorr > 3*calib.GetPedestalSigL(aTile->GetCellID() && hgCorr < 3900) )
            hSpectra[aTile->GetCellID()].FillCorr(lgCorr,hgCorr);;
          RootOutput->cd();
        }
      } else if (typeRO == ReadOut::Type::Hgcroc) {
        Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
        if (i == 0 && debug > 2) std::cout << ((TString)setup->DecodeCellID(aTile->GetCellID())).Data() << std::endl;
        
        ithSpectra=hSpectra.find(aTile->GetCellID());
        double adc = aTile->GetIntegratedADC();
        double tot = aTile->GetIntegratedTOT();
            
        if(ithSpectra!=hSpectra.end()){
          ithSpectra->second.FillSpectra(tot,adc);
          ithSpectra->second.FillCorr(tot,adc);
        } else {
          RootOutputHist->cd("IndividualCells");
          hSpectra[aTile->GetCellID()]=TileSpectra("mip1st",aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(), debug);
          hSpectra[aTile->GetCellID()].FillSpectra(tot,adc);;
          hSpectra[aTile->GetCellID()].FillCorr(tot,adc);;
          RootOutput->cd();
        }
        
      }
    } 
    RootOutput->cd();
  }
  // TdataOut->Write();
  TsetupIn->CloneTree()->Write();
  
  RootOutputHist->cd();
 
  //***********************************************************************************************
  //***** Monitoring histos for calib parameters & fits results of 1st iteration ******************
  //***********************************************************************************************
  int maxChannelPerLayer        = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1);
  // monitoring applied pedestals
  TH1D* hMeanPedHGvsCellID      = new TH1D( "hMeanPedHG_vsCellID","mean Ped High Gain vs CellID ; cell ID; #mu_{noise, HG} (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5);
  hMeanPedHGvsCellID->SetDirectory(0);
  TH1D* hMeanPedHG              = new TH1D( "hMeanPedHG","mean Ped High Gain ; #mu_{noise, HG} (arb. units); counts ",
                                            500, -0.5, 500-0.5);
  hMeanPedHG->SetDirectory(0);
  TH2D* hspectraHGMeanVsLayer   = new TH2D( "hspectraHGMeanVsLayer","Mean Ped High Gain; layer; brd channel; #mu_{Ped HG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGMeanVsLayer->SetDirectory(0);
  TH2D* hspectraHGSigmaVsLayer  = new TH2D( "hspectraHGSigmaVsLayer","Sigma Ped High Gain; layer; brd channel; #sigma_{Ped HG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGSigmaVsLayer->SetDirectory(0);
  TH1D* hMeanPedLGvsCellID      = new TH1D( "hMeanPedLG_vsCellID","mean Ped Low Gain vs CellID ; cell ID; #mu_{noise, LG} (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5);
  hMeanPedLGvsCellID->SetDirectory(0);
  TH1D* hMeanPedLG             = new TH1D( "hMeanPedLG","mean Ped Low Gain ; #mu_{noise, LG} (arb. units); counts ",
                                            500, -0.5, 500-0.5);
  hMeanPedLG->SetDirectory(0);
  TH2D* hspectraLGMeanVsLayer   = new TH2D( "hspectraLGMeanVsLayer","Mean Ped Low Gain; layer; brd channel; #mu_{PED LG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGMeanVsLayer->SetDirectory(0);
  TH2D* hspectraLGSigmaVsLayer  = new TH2D( "hspectraLGSigmaVsLayer","Sigma Ped Low Gain; layer; brd channel; #sigma_{Ped LG} (arb. units)",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGSigmaVsLayer->SetDirectory(0);
  // monitoring 1st iteration mip fits
  TH2D* hspectraHGMaxVsLayer1st   = new TH2D( "hspectraHGMaxVsLayer_1st","Max High Gain; layer; brd channel; Max_{HG, 1^{st}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGMaxVsLayer1st->SetDirectory(0);
  TH2D* hspectraHGFWHMVsLayer1st   = new TH2D( "hspectraHGFWHMVsLayer_1st","FWHM High Gain; layer; brd channel; FWHM_{HG, 1^{st}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGFWHMVsLayer1st->SetDirectory(0);
  TH2D* hspectraHGLMPVVsLayer1st   = new TH2D( "hspectraHGLMPVVsLayer_1st","MPV High Gain; layer; brd channel; MPV_{HG, 1^{st}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGLMPVVsLayer1st->SetDirectory(0);
  TH2D* hspectraHGLSigmaVsLayer1st = new TH2D( "hspectraHGLSigmaVsLayer_1st","Sigma Landau High Gain; layer; brd channel; #sigma_{L,HG, 1^{st}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGLSigmaVsLayer1st->SetDirectory(0);
  TH2D* hspectraHGGSigmaVsLayer1st = new TH2D( "hspectraHGGSigmaVsLayer_1st","Sigma Gauss High Gain; layer; brd channel; #sigma_{G,HG, 1^{st}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGGSigmaVsLayer1st->SetDirectory(0);
  TH2D* hLGHGCorrVsLayer = new TH2D( "hLGHGCorrVsLayer","LG-HG corr; layer; brd channel; a_{LG-HG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hLGHGCorrVsLayer->SetDirectory(0);
  TH2D* hHGLGCorrVsLayer = new TH2D( "hHGLGCorrVsLayer","HG-LG corr; layer; brd channel; a_{HG-LG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hHGLGCorrVsLayer->SetDirectory(0);
  TH2D* hLGHGCorrOffsetVsLayer = new TH2D( "hLGHGCorrOffsetVsLayer","LG-HG corr offset; layer; brd channel; b_{LG-HG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hLGHGCorrOffsetVsLayer->SetDirectory(0);
  TH2D* hHGLGCorrOffsetVsLayer = new TH2D( "hHGLGCorrVsLayer","HG-LG corr offset; layer; brd channel; b_{HG-LG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hHGLGCorrOffsetVsLayer->SetDirectory(0);
  
  TH1D* hMaxHG1st             = new TH1D( "hMaxHG1st","Max High Gain ;Max_{HG, 1^{st}} (arb. units) ; counts ",
                                            2000, -0.5, 2000-0.5);
  hMaxHG1st->SetDirectory(0);
  TH1D* hLGHGCorr             = new TH1D( "hLGHGCorr","LG-HG corr ; a_{LG-HG} (arb. units) ; counts ",
                                            400, -20, 20);
  hLGHGCorr->SetDirectory(0);
  TH1D* hHGLGCorr             = new TH1D( "hHGLGCorr","LG-HG corr ; a_{HG-LG} (arb. units) ; counts ",
                                            400, -1., 1.);
  hHGLGCorr->SetDirectory(0);

  
  // fit pedestal
  double* parameters    = new double[6];
  double* parErrAndRes  = new double[6];
  bool isGood;
  int currCells = 0;
  if ( debug > 0)
    std::cout << "============================== starting fitting 1st iteration" << std::endl;
  
  for(ithSpectra=hSpectra.begin(); ithSpectra!=hSpectra.end(); ++ithSpectra){
    if (currCells%20 == 0 && currCells > 0 && debug > 0)
      std::cout << "============================== cell " <<  currCells << " / " << hSpectra.size() << " cells" << std::endl;
    currCells++;
    for (int p = 0; p < 6; p++){
      parameters[p]   = 0;
      parErrAndRes[p] = 0;
    }
    isGood        = ithSpectra->second.FitMipHG(parameters, parErrAndRes, debug, yearData, false, calib.GetVov(), 1);
    long cellID   = ithSpectra->second.GetCellID();
    int layer     = setup->GetLayer(cellID);
    int chInLayer = setup->GetChannelInLayer(cellID);
    
    // fill cross-check histos
    hMeanPedHGvsCellID->SetBinContent(hMeanPedHGvsCellID->FindBin(cellID), calib.GetPedestalMeanH(cellID));
    hMeanPedLGvsCellID->SetBinContent(hMeanPedLGvsCellID->FindBin(cellID), calib.GetPedestalMeanL(cellID));
    hMeanPedHG->Fill(calib.GetPedestalMeanH(cellID));
    hMeanPedLG->Fill(calib.GetPedestalMeanL(cellID));
    
    int bin2D     = hspectraHGMeanVsLayer->FindBin(layer,chInLayer);
    hspectraHGMeanVsLayer->SetBinContent(bin2D, calib.GetPedestalMeanH(cellID));
    hspectraHGSigmaVsLayer->SetBinContent(bin2D, calib.GetPedestalSigH(cellID));
    hspectraLGMeanVsLayer->SetBinContent(bin2D, calib.GetPedestalMeanL(cellID));
    hspectraLGSigmaVsLayer->SetBinContent(bin2D, calib.GetPedestalSigL(cellID));

    if (isGood){
      hspectraHGMaxVsLayer1st->SetBinContent(bin2D, parameters[4]);
      hspectraHGFWHMVsLayer1st->SetBinContent(bin2D, parameters[5]);
      hspectraHGLMPVVsLayer1st->SetBinContent(bin2D, parameters[1]);
      hspectraHGLMPVVsLayer1st->SetBinError(bin2D, parErrAndRes[1]);
      hspectraHGLSigmaVsLayer1st->SetBinContent(bin2D, parameters[0]);
      hspectraHGLSigmaVsLayer1st->SetBinError(bin2D, parErrAndRes[0]);
      hspectraHGGSigmaVsLayer1st->SetBinContent(bin2D, parameters[3]);
      hspectraHGGSigmaVsLayer1st->SetBinError(bin2D, parErrAndRes[3]);
      
      hMaxHG1st->Fill(parameters[4]);
    }
    
    if (typeRO == ReadOut::Type::Caen) {
      isGood=ithSpectra->second.FitCorr(debug);
      if (ithSpectra->second.GetCorrModel(0)){
        hLGHGCorrVsLayer->SetBinContent(bin2D,ithSpectra->second.GetCorrModel(0)->GetParameter(1));
        hLGHGCorrVsLayer->SetBinError(bin2D,ithSpectra->second.GetCorrModel(0)->GetParError(1));
        hLGHGCorrOffsetVsLayer->SetBinContent(bin2D,ithSpectra->second.GetCorrModel(0)->GetParameter(0));
        hLGHGCorrOffsetVsLayer->SetBinError(bin2D,ithSpectra->second.GetCorrModel(0)->GetParError(0));
        hLGHGCorr->Fill(ithSpectra->second.GetCorrModel(0)->GetParameter(1));
      } 
      if (ithSpectra->second.GetCorrModel(1)){
        hHGLGCorrVsLayer->SetBinContent(bin2D,ithSpectra->second.GetCorrModel(1)->GetParameter(1));
        hHGLGCorrVsLayer->SetBinError(bin2D,ithSpectra->second.GetCorrModel(1)->GetParError(1));    
        hHGLGCorrOffsetVsLayer->SetBinContent(bin2D,ithSpectra->second.GetCorrModel(1)->GetParameter(0));
        hHGLGCorrOffsetVsLayer->SetBinError(bin2D,ithSpectra->second.GetCorrModel(1)->GetParError(0));    
        hHGLGCorr->Fill(ithSpectra->second.GetCorrModel(1)->GetParameter(1));
      }
    }
  }
  if ( debug > 0)
    std::cout << "============================== done fitting 1st iteration" << std::endl;

  TH1D* hHGTileSum[20];
  for (int c = 0; c < maxChannelPerLayer; c++ ){
    hHGTileSum[c] = new TH1D( Form("hHGTileSum_absChB%d",c),"av ADC surrounding cells ; ADC (arb. units); counts ",
                              4000, -0.5, 4000-0.5);
    hHGTileSum[c]->SetDirectory(0);
  }
  
  // setup trigger sel
  TRandom3* rand    = new TRandom3();
  Int_t localTriggerTiles = 4;
  double factorMinTrigg   = 0.5;
  double factorMaxTrigg   = 4.;
  if (yearData == 2023){
    localTriggerTiles = 6;
    factorMaxTrigg    = 3.;
  }
  if (typeRO == ReadOut::Type::Hgcroc){
    localTriggerTiles = 6;
    factorMinTrigg    = 0.3;
  }
  
  RootOutputHist->mkdir("IndividualCellsTrigg");
  RootOutputHist->cd("IndividualCellsTrigg");  
  //***********************************************************************************************
  //************************* first pass over tree to extract spectra *****************************
  //***********************************************************************************************  
  int actCh1st = 0;
  double averageScale = calib.GetAverageScaleHigh(actCh1st);
  double meanFWHMHG   = calib.GetAverageScaleWidthHigh();
  double avHGLGCorr   = calib.GetAverageHGLGCorr();
  double avHGLGOffCorr= calib.GetAverageHGLGCorrOff();
  double avLGHGCorr   = calib.GetAverageLGHGCorr();
  double avLGHGOffCorr= calib.GetAverageLGHGCorrOff();
  std::cout << "average HG mip: " << averageScale << "\t active ch: "<< actCh1st<< std::endl;
  for(int i=0; i<evts && i < maxEvents; i++){
    TdataIn->GetEntry(i);
    if (i%evtDeb == 0 && i > 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      if (typeRO == ReadOut::Type::Caen) {
        Caen* aTile=(Caen*)event.GetTile(j);
        if (i == 0 && debug > 2) std::cout << ((TString)setup->DecodeCellID(aTile->GetCellID())).Data() << std::endl;
        long currCellID = aTile->GetCellID();
        
        // read current tile
        ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
        double hgCorr = aTile->GetADCHigh()-calib.GetPedestalMeanH(aTile->GetCellID());
        double lgCorr = aTile->GetADCLow()-calib.GetPedestalMeanL(aTile->GetCellID());

        aTile->SetLocalTriggerPrimitive(event.CalculateLocalMuonTrigg(calib, rand, currCellID, localTriggerTiles, avLGHGCorr));
        // estimate local muon trigger
        bool localMuonTrigg = event.InspectIfLocalMuonTrigg(currCellID, averageScale, factorMinTrigg, factorMaxTrigg);
        int chInLayer = setup->GetChannelInLayer(currCellID);    
        hHGTileSum[chInLayer]->Fill(aTile->GetLocalTriggerPrimitive());
        
        if(ithSpectraTrigg!=hSpectraTrigg.end()){
          ithSpectraTrigg->second.FillTrigger(aTile->GetLocalTriggerPrimitive());
        } else {
          RootOutputHist->cd("IndividualCellsTrigg");
          hSpectraTrigg[currCellID]=TileSpectra("mipTrigg",currCellID,calib.GetTileCalib(currCellID),event.GetROtype(),debug);
          hSpectraTrigg[currCellID].FillTrigger(aTile->GetLocalTriggerPrimitive());;
          RootOutput->cd();
        }
        // only fill tile spectra if 4 surrounding tiles on average are compatible with muon response
        if (localMuonTrigg){
          aTile->SetLocalTriggerBit(1);
          ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
          ithSpectraTrigg->second.FillSpectra(lgCorr,hgCorr);
          if (hgCorr > 3*calib.GetPedestalSigH(currCellID) && lgCorr > 3*calib.GetPedestalSigL(currCellID) && hgCorr < 3900 )
            ithSpectraTrigg->second.FillCorr(lgCorr,hgCorr);
        }
      } else if (typeRO == ReadOut::Type::Hgcroc) {
        Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
        if (i == 0 && debug > 2) std::cout << ((TString)setup->DecodeCellID(aTile->GetCellID())).Data() << std::endl;
        long currCellID = aTile->GetCellID();
        
        ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
        // double adc = aTile->GetPedestal()+aTile->GetIntegratedADC();
        double adc = aTile->GetIntegratedADC();
        double tot = aTile->GetIntegratedTOT();
            
        aTile->SetLocalTriggerPrimitive(event.CalculateLocalMuonTrigg(calib, rand, currCellID, localTriggerTiles, 0));
        // estimate local muon trigger
        bool localMuonTrigg = event.InspectIfLocalMuonTrigg(currCellID, averageScale, factorMinTrigg, factorMaxTrigg);
        int chInLayer = setup->GetChannelInLayer(currCellID);    
        hHGTileSum[chInLayer]->Fill(aTile->GetLocalTriggerPrimitive());
        
        if(ithSpectraTrigg!=hSpectraTrigg.end()){
          ithSpectraTrigg->second.FillTrigger(aTile->GetLocalTriggerPrimitive());
        } else {
          RootOutputHist->cd("IndividualCellsTrigg");
          hSpectraTrigg[currCellID]=TileSpectra("mipTrigg",currCellID,calib.GetTileCalib(currCellID),event.GetROtype(),debug);
          hSpectraTrigg[currCellID].FillTrigger(aTile->GetLocalTriggerPrimitive());;
          RootOutput->cd();
        }
        // only fill tile spectra if 4 surrounding tiles on average are compatible with muon response
        if (localMuonTrigg){
          aTile->SetLocalTriggerBit(1);
          ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
          ithSpectraTrigg->second.FillSpectra(tot,adc);
          ithSpectraTrigg->second.FillCorr(tot,adc);
        }
      }
    }
    RootOutput->cd();
    TdataOut->Fill();
  }
  TdataOut->Write();
  
  //***********************************************************************************************
  //***** Monitoring histos for fits results of 2nd iteration ******************
  //***********************************************************************************************
  RootOutputHist->cd();
  
  // monitoring trigger 
  TH2D* hmipTriggers              = new TH2D( "hmipTriggers","muon triggers; layer; brd channel; counts ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hmipTriggers->SetDirectory(0);
  TH2D* hSuppresionNoise          = new TH2D( "hSuppresionNoise","S/B noise region; layer; brd channel; S/B noise region",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hSuppresionNoise->SetDirectory(0);
  TH2D* hSuppresionSignal         = new TH2D( "hSuppresionSignal","S/B signal region; layer; brd channel; S/B signal region",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hSuppresionSignal->SetDirectory(0);

  // monitoring 2nd iteration mip fits
  TH2D* hspectraHGMaxVsLayer2nd   = new TH2D( "hspectraHGMaxVsLayer_2nd","Max High Gain; layer; brd channel; Max_{HG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGMaxVsLayer2nd->SetDirectory(0);
  TH2D* hspectraHGFWHMVsLayer2nd   = new TH2D( "hspectraHGFWHMVsLayer_2nd","FWHM High Gain; layer; brd channel; FWHM_{HG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGFWHMVsLayer2nd->SetDirectory(0);
  TH2D* hspectraHGLMPVVsLayer2nd   = new TH2D( "hspectraHGLMPVVsLayer_2nd","MPV High Gain; layer; brd channel; MPV_{HG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGLMPVVsLayer2nd->SetDirectory(0);
  TH2D* hspectraHGLSigmaVsLayer2nd = new TH2D( "hspectraHGLSigmaVsLayer_2nd","Sigma Landau High Gain; layer; brd channel; #sigma_{L,HG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGLSigmaVsLayer2nd->SetDirectory(0);
  TH2D* hspectraHGGSigmaVsLayer2nd = new TH2D( "hspectraHGGSigmaVsLayer_2nd","Sigma Gauss High Gain; layer; brd channel; #sigma_{G,HG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGGSigmaVsLayer2nd->SetDirectory(0);
  TH2D* hspectraLGMaxVsLayer2nd   = new TH2D( "hspectraLGMaxVsLayer_2nd","Max High Gain; layer; brd channel; Max_{LG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGMaxVsLayer2nd->SetDirectory(0);
  TH2D* hspectraLGFWHMVsLayer2nd   = new TH2D( "hspectraLGFWHMVsLayer_2nd","FWHM High Gain; layer; brd channel; FWHM_{LG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGFWHMVsLayer2nd->SetDirectory(0);
  TH2D* hspectraLGLMPVVsLayer2nd   = new TH2D( "hspectraLGLMPVVsLayer_2nd","MPV High Gain; layer; brd channel; MPV_{LG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGLMPVVsLayer2nd->SetDirectory(0);
  TH2D* hspectraLGLSigmaVsLayer2nd = new TH2D( "hspectraLGLSigmaVsLayer_2nd","Sigma Landau High Gain; layer; brd channel; #sigma_{L,LG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGLSigmaVsLayer2nd->SetDirectory(0);
  TH2D* hspectraLGGSigmaVsLayer2nd = new TH2D( "hspectraLGGSigmaVsLayer_2nd","Sigma Gauss High Gain; layer; brd channel; #sigma_{G,LG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGGSigmaVsLayer2nd->SetDirectory(0);

  TH1D* hMaxHG2nd             = new TH1D( "hMaxHG2nd","Max High Gain ;Max_{HG, 2^{nd}} (arb. units) ; counts ",
                                            2000, -0.5, 2000-0.5);
  hMaxHG2nd->SetDirectory(0);
  TH1D* hMaxLG2nd             = new TH1D( "hMaxLG2nd","Max High Gain ;Max_{LG, 2^{nd}} (arb. units) ; counts ",
                                            400, -0.5, 400-0.5);
  hMaxLG2nd->SetDirectory(0);


  currCells = 0;
  if ( debug > 0)
    std::cout << "============================== starting fitting 2nd iteration" << std::endl;
  for(ithSpectraTrigg=hSpectraTrigg.begin(); ithSpectraTrigg!=hSpectraTrigg.end(); ++ithSpectraTrigg){
    if (currCells%20 == 0 && currCells > 0 && debug > 0)
      std::cout << "============================== cell " <<  currCells << " / " << hSpectraTrigg.size() << " cells" << std::endl;
    currCells++;
    for (int p = 0; p < 6; p++){
      parameters[p]   = 0;
      parErrAndRes[p] = 0;
    }
    isGood=ithSpectraTrigg->second.FitMipHG(parameters, parErrAndRes, debug, yearData, false, calib.GetVov(), averageScale);
    
    long cellID     = ithSpectraTrigg->second.GetCellID();
    int layer       = setup->GetLayer(cellID);
    int chInLayer   = setup->GetChannelInLayer(cellID);    
    int bin2D       = hspectraHGMeanVsLayer->FindBin(layer,chInLayer);

    double pedSigHG = 0;
    double maxBin   = 0;
    if (typeRO == ReadOut::Type::Caen){
      pedSigHG = calib.GetPedestalSigH(cellID);
      maxBin   = 3800;
    } else {
      pedSigHG = 20;
      maxBin   = 1024;
    }
    Int_t binNLow   = ithSpectraTrigg->second.GetHG()->FindBin(-1*pedSigHG);
    Int_t binNHigh  = ithSpectraTrigg->second.GetHG()->FindBin(3*pedSigHG);
    Int_t binSHigh  = ithSpectraTrigg->second.GetHG()->FindBin(maxBin);
    
    double S_NoiseR = ithSpectraTrigg->second.GetHG()->Integral(binNLow, binNHigh);
    double S_SigR   = ithSpectraTrigg->second.GetHG()->Integral(binNHigh, binSHigh);
    
    ithSpectra      = hSpectra.find(cellID);
    double B_NoiseR = ithSpectra->second.GetHG()->Integral(binNLow , binNHigh);
    double B_SigR   = ithSpectra->second.GetHG()->Integral(binNHigh, binSHigh);
    
    double SB_NoiseR  = (B_NoiseR != 0.) ? S_NoiseR/B_NoiseR : 0;
    double SB_SigR    = (B_SigR != 0.) ? S_SigR/B_SigR : 0;
    
    hmipTriggers->SetBinContent(bin2D, ithSpectraTrigg->second.GetHG()->GetEntries());
    hSuppresionNoise->SetBinContent(bin2D, SB_NoiseR);
    hSuppresionSignal->SetBinContent(bin2D, SB_SigR);
    if (isGood){
      hspectraHGMaxVsLayer2nd->SetBinContent(bin2D, parameters[4]);
      hspectraHGFWHMVsLayer2nd->SetBinContent(bin2D, parameters[5]);
      hspectraHGLMPVVsLayer2nd->SetBinContent(bin2D, parameters[1]);
      hspectraHGLMPVVsLayer2nd->SetBinError(bin2D, parErrAndRes[1]);
      hspectraHGLSigmaVsLayer2nd->SetBinContent(bin2D, parameters[0]);
      hspectraHGLSigmaVsLayer2nd->SetBinError(bin2D, parErrAndRes[0]);
      hspectraHGGSigmaVsLayer2nd->SetBinContent(bin2D, parameters[3]);
      hspectraHGGSigmaVsLayer2nd->SetBinError(bin2D, parErrAndRes[3]);
      hMaxHG2nd->Fill(parameters[4]);
    }
    
    if (typeRO == ReadOut::Type::Caen) {
      for (int p = 0; p < 6; p++){
        parameters[p]   = 0;
        parErrAndRes[p] = 0;
      }
      isGood=ithSpectraTrigg->second.FitMipLG(parameters, parErrAndRes, debug, yearData, false, 1);
      if (isGood){
        hspectraLGMaxVsLayer2nd->SetBinContent(bin2D, parameters[4]);
        hspectraLGFWHMVsLayer2nd->SetBinContent(bin2D, parameters[5]);
        hspectraLGLMPVVsLayer2nd->SetBinContent(bin2D, parameters[1]);
        hspectraLGLMPVVsLayer2nd->SetBinError(bin2D, parErrAndRes[1]);
        hspectraLGLSigmaVsLayer2nd->SetBinContent(bin2D, parameters[0]);
        hspectraLGLSigmaVsLayer2nd->SetBinError(bin2D, parErrAndRes[0]);
        hspectraLGGSigmaVsLayer2nd->SetBinContent(bin2D, parameters[3]);
        hspectraLGGSigmaVsLayer2nd->SetBinError(bin2D, parErrAndRes[3]);
        hMaxLG2nd->Fill(parameters[4]);
      }
    }
  }
  if ( debug > 0)
    std::cout << "============================== done fitting 2nd iteration" << std::endl;
  int actCh2nd = 0;
  double averageScaleUpdated    = calib.GetAverageScaleHigh(actCh2nd);
  double meanFWHMHGUpdated      = calib.GetAverageScaleWidthHigh();
  double averageScaleLowUpdated = 0.;
  double meanFWHMLGUpdated      = 0.;
  if (typeRO == ReadOut::Type::Caen){
    averageScaleLowUpdated = calib.GetAverageScaleLow();
    meanFWHMLGUpdated      = calib.GetAverageScaleWidthLow();
  }
  std::cout << "average 1st iteration HG mip: " << averageScale << "\t act. channels: " <<   actCh1st << std::endl;
  std::cout << "average 2nd iteration  HG mip: " << averageScaleUpdated << "\t LG mip: " << averageScaleLowUpdated << "\t act. channels: " <<   actCh2nd << std::endl;
  
  RootOutput->cd();
  
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }

  TcalibOut->Fill();
  TcalibOut->Write();
  RootOutput->Close();


  RootOutputHist->cd("IndividualCells");
    for(ithSpectra=hSpectra.begin(); ithSpectra!=hSpectra.end(); ++ithSpectra){
      ithSpectra->second.Write(true);
    }
  RootOutputHist->cd("IndividualCellsTrigg");
    for(ithSpectra=hSpectraTrigg.begin(); ithSpectra!=hSpectraTrigg.end(); ++ithSpectra){
      ithSpectra->second.Write(true);
    }
  RootOutputHist->cd();
    hMeanPedHGvsCellID->Write();
    hMeanPedHG->Write();
    
    hspectraHGMeanVsLayer->Write();
    hspectraHGSigmaVsLayer->Write();
    hspectraHGMaxVsLayer1st->Write();
    hspectraHGFWHMVsLayer1st->Write();
    hspectraHGLMPVVsLayer1st->Write();
    hspectraHGLSigmaVsLayer1st->Write();
    hspectraHGGSigmaVsLayer1st->Write();
    hMaxHG1st->Write();
    
    
    hspectraHGMaxVsLayer2nd->Write();
    hspectraHGFWHMVsLayer2nd->Write();
    hspectraHGLMPVVsLayer2nd->Write();
    hspectraHGLSigmaVsLayer2nd->Write();
    hspectraHGGSigmaVsLayer2nd->Write();
    hMaxHG2nd->Write();
    
    if (typeRO == ReadOut::Type::Caen) {
      hMeanPedLGvsCellID->Write();
      hMeanPedLG->Write();
      hspectraLGMeanVsLayer->Write();
      hspectraLGSigmaVsLayer->Write();
      hLGHGCorrVsLayer->Write();
      hLGHGCorrOffsetVsLayer->Write();
      hHGLGCorrVsLayer->Write();
      hHGLGCorrOffsetVsLayer->Write();
      hLGHGCorr->Write();
      hHGLGCorr->Write();
      hspectraLGMaxVsLayer2nd->Write();
      hspectraLGFWHMVsLayer2nd->Write();
      hspectraLGLMPVVsLayer2nd->Write();
      hspectraLGLSigmaVsLayer2nd->Write();
      hspectraLGGSigmaVsLayer2nd->Write();
      hMaxLG2nd->Write();
    }
    for (int c = 0; c< maxChannelPerLayer; c++)
      hHGTileSum[c]->Write();
    hmipTriggers->Write();
    hSuppresionNoise->Write();
    hSuppresionSignal->Write();
  // fill calib tree & write it
  // close open root files
  RootOutputHist->Write();
  RootOutputHist->Close();

  RootInput->Close();

  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find(runNr);
  
  // create directory for plot output
  TString outputDirPlots = GetPlotOutputDir();
  gSystem->Exec("mkdir -p "+outputDirPlots);
  
  //**********************************************************************
  // Create canvases for channel overview plotting
  //**********************************************************************
  Double_t textSizeRel = 0.035;
  StyleSettingsBasics("pdf");
  SetPlotStyle();

  TCanvas* canvas2DCorr = new TCanvas("canvasCorrPlots","",0,0,1450,1300);  // gives the page size
  DefaultCancasSettings( canvas2DCorr, 0.08, 0.13, 0.045, 0.07);

  canvas2DCorr->SetLogz(0);
  PlotSimple2D( canvas2DCorr, hspectraHGMeanVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_NoiseMean.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGSigmaVsLayer,-10000, -10000, textSizeRel, Form("%s/HG_NoiseSigma.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1,  kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGMaxVsLayer1st, -10000, -10000, textSizeRel, Form("%s/HG_MaxMip_1st.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "#LT Max_{HG} #GT = %.1f", averageScale));
  PlotSimple2D( canvas2DCorr, hspectraHGFWHMVsLayer1st, -10000, -10000, textSizeRel, Form("%s/HG_FWHMMip_1st.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "#LT FWHM_{HG} #GT = %.1f", meanFWHMHG));
  PlotSimple2D( canvas2DCorr, hspectraHGLMPVVsLayer1st, -10000, -10000, textSizeRel, Form("%s/HG_LandMPVMip_1st.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGLSigmaVsLayer1st, -10000, -10000, textSizeRel, Form("%s/HG_LandSigMip_1st.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGGSigmaVsLayer1st, -10000, -10000, textSizeRel, Form("%s/HG_GaussSigMip_1st.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGMaxVsLayer2nd, -10000, -10000, textSizeRel, Form("%s/HG_MaxMip_2nd.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "#LT Max_{HG} #GT = %.1f", averageScaleUpdated));
  PlotSimple2D( canvas2DCorr, hspectraHGFWHMVsLayer2nd, -10000, -10000, textSizeRel, Form("%s/HG_FWHMMip_2nd.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "#LT FWHM_{HG} #GT = %.1f", meanFWHMHGUpdated));
  PlotSimple2D( canvas2DCorr, hspectraHGLMPVVsLayer2nd, -10000, -10000, textSizeRel, Form("%s/HG_LandMPVMip_2nd.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGLSigmaVsLayer2nd, -10000, -10000, textSizeRel, Form("%s/HG_LandSigMip_2nd.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGGSigmaVsLayer2nd, -10000, -10000, textSizeRel, Form("%s/HG_GaussSigMip_2nd.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  canvas2DCorr->SetLogz(1);
  TString drawOpt = "colz";
  if (yearData == 2023)
    drawOpt = "colz,text";
  PlotSimple2D( canvas2DCorr, hmipTriggers, -10000, -10000, textSizeRel, Form("%s/MuonTriggers.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, drawOpt, true, Form( "evt. collected = %d", evts));
  PlotSimple2D( canvas2DCorr, hSuppresionNoise, -10000, -10000, textSizeRel, Form("%s/SuppressionNoise.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, drawOpt, true);
  PlotSimple2D( canvas2DCorr, hSuppresionSignal, -10000, -10000, textSizeRel, Form("%s/SuppressionSignal.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, drawOpt, true);
  
  canvas2DCorr->SetLogz(0);
  
  if (typeRO == ReadOut::Type::Caen) {
    PlotSimple2D( canvas2DCorr, hspectraLGMeanVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_NoiseMean.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
    PlotSimple2D( canvas2DCorr, hspectraLGSigmaVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_NoiseSigma.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
    PlotSimple2D( canvas2DCorr, hspectraLGMaxVsLayer2nd, -10000, -10000, textSizeRel, Form("%s/LG_MaxMip_2nd.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "#LT Max_{LG} #GT = %.1f", averageScaleLowUpdated));
    PlotSimple2D( canvas2DCorr, hspectraLGFWHMVsLayer2nd, -10000, -10000, textSizeRel, Form("%s/LG_FWHMMip_2nd.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "#LT FWHM_{LG} #GT = %.1f", meanFWHMLGUpdated));
    PlotSimple2D( canvas2DCorr, hspectraLGLMPVVsLayer2nd, -10000, -10000, textSizeRel, Form("%s/LG_LandMPVMip_2nd.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
    PlotSimple2D( canvas2DCorr, hspectraLGLSigmaVsLayer2nd, -10000, -10000, textSizeRel, Form("%s/LG_LandSigMip_2nd.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
    PlotSimple2D( canvas2DCorr, hspectraLGGSigmaVsLayer2nd, -10000, -10000, textSizeRel, Form("%s/LG_GaussSigMip_2nd.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);

    PlotSimple2D( canvas2DCorr, hLGHGCorrVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_HG_Corr.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kTRUE, "colz", true, Form( "#LT a_{LGHG} #GT = %.1f", avLGHGCorr));
    PlotSimple2D( canvas2DCorr, hLGHGCorrOffsetVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_HG_CorrOffset.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kTRUE, "colz", true, Form( "#LT b_{LGHG} #GT = %.1f", avLGHGOffCorr));
    PlotSimple2D( canvas2DCorr, hHGLGCorrVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_LG_Corr.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kTRUE, "colz", true, Form( "#LT a_{HGLG} #GT = %.1f", avHGLGCorr));
    PlotSimple2D( canvas2DCorr, hHGLGCorrOffsetVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_LG_CorrOffset.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kTRUE, "colz", true, Form( "#LT b_{HGLG} #GT = %.1f", avHGLGOffCorr));
  }
  if (ExtPlot > 0){
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
    Double_t maxHG = ReturnMipPlotRangeDepVov(calib.GetVov(),true, typeRO);
    Double_t maxLG = ReturnMipPlotRangeDepVov(calib.GetVov(),false, typeRO);
    std::cout << "plotting single layers" << std::endl;

    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){    
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;
        if (ExtPlot > 0){
          PlotMipWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                    hSpectra, hSpectraTrigg, setup, true, -100, maxHG, 1.2, l, m,
                                    Form("%s/MIP_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          Double_t maxTriggPPlot = maxHG*2;
          if (typeRO != ReadOut::Type::Caen)
            maxTriggPPlot = 500;

          PlotTriggerPrimWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                            hSpectraTrigg, setup, averageScale, factorMinTrigg, factorMaxTrigg,
                                            0, maxTriggPPlot, 1.2, l, m, Form("%s/TriggPrimitive_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          if (typeRO == ReadOut::Type::Caen) {
            PlotCorrWithFits8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 
                                    hSpectra, 0, -20, 800, 3900, l, m,
                                    Form("%s/LGHG_Corr_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          }
        }
        if (ExtPlot > 1 && typeRO == ReadOut::Type::Caen) {
          PlotMipWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                    hSpectra, hSpectraTrigg, setup, false, -30, maxLG, 1.2, l, m,
                                    Form("%s/MIP_LG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          PlotCorrWithFits8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 
                                    hSpectra, 1, -100, 4000, 340, l, m,
                                    Form("%s/HGLG_Corr_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
        }
      }
    }
    std::cout << "done plotting single layers" << std::endl;  
  }
  return true;
}

//***********************************************************************************************
//*********************** improved scaling calculation function *********************************
//***********************************************************************************************
bool Analyses::GetImprovedScaling(void){
  std::cout<<"GetImprovedScaling"<<std::endl;
  
  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  
  std::map<int,TileSpectra> hSpectra;
  std::map<int,TileSpectra> hSpectraTrigg;
  std::map<int, TileSpectra>::iterator ithSpectra;
  std::map<int, TileSpectra>::iterator ithSpectraTrigg;
  
  std::cout << "Additional Output with histos being created: " << RootOutputNameHist.Data() << std::endl;
  if(Overwrite){
    std::cout << "recreating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"RECREATE");
  } else{
    std::cout << "newly creating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"CREATE");
  }
    
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer("Minuit2", "Migrad");
  // setup trigger sel
  double factorMinTrigg   = 0.8;
  double factorMaxTrigg   = 2.5;
  if (yearData == 2023){
    factorMinTrigg    = 0.9;
    factorMaxTrigg    = 2.;
  }
  
  RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->mkdir("IndividualCellsTrigg");
  RootOutputHist->cd("IndividualCellsTrigg");  
  //***********************************************************************************************
  //************************* first pass over tree to extract spectra *****************************
  //***********************************************************************************************  
  TcalibIn->GetEntry(0);
  // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }
  
  int evts=TdataIn->GetEntries();
  int runNr = -1;
  int actChI  = 0;
  ReadOut::Type typeRO = ReadOut::Type::Caen;
  int evtDeb = 5000;
  
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  
  double averageScale     = calib.GetAverageScaleHigh(actChI);
  double averageScaleLow  = calib.GetAverageScaleLow();
  std::cout << "average HG mip: " << averageScale << " LG mip: "<< averageScaleLow << "\t act. ch: "<< actChI << std::endl;
  
  for(int i=0; i<maxEvents; i++){
    TdataIn->GetEntry(i);    
    if (i == 0){
      runNr = event.GetRunNumber();
      typeRO = event.GetROtype();
      if (typeRO != ReadOut::Type::Caen){
        evtDeb = 400;
        factorMinTrigg    = 0.5;
        std::cout << "reseting lower trigger factor limit to: " << factorMinTrigg << std::endl;
      }
      std::cout<< "Total number of events: " << evts << std::endl;
      std::cout<< "original run numbers calib: "<<calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
    }

    TdataIn->GetEntry(i);
    if (i%evtDeb == 0 && i > 0 && debug > 0) std::cout << "Reading " <<  i << " / " << maxEvents << " events" << std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      // CAEN treatment
      if (typeRO == ReadOut::Type::Caen) {
        Caen* aTile=(Caen*)event.GetTile(j);
        if (i == 0 && debug > 2) std::cout << ((TString)setup->DecodeCellID(aTile->GetCellID())).Data() << std::endl;
        long currCellID = aTile->GetCellID();
        
        // read current tile
        ithSpectraTrigg=hSpectraTrigg.find(currCellID);
        double hgCorr = aTile->GetADCHigh()-calib.GetPedestalMeanH(currCellID);
        double lgCorr = aTile->GetADCLow()-calib.GetPedestalMeanL(currCellID);

        // estimate local muon trigger
        bool localMuonTrigg = event.InspectIfLocalMuonTrigg(currCellID, averageScale, factorMinTrigg, factorMaxTrigg);
        
        if(ithSpectraTrigg!=hSpectraTrigg.end()){
          ithSpectraTrigg->second.FillTrigger(aTile->GetLocalTriggerPrimitive());
        } else {
          RootOutputHist->cd("IndividualCellsTrigg");
          hSpectraTrigg[currCellID]=TileSpectra("mipTrigg",currCellID,calib.GetTileCalib(currCellID),event.GetROtype(),debug);
          hSpectraTrigg[currCellID].FillTrigger(aTile->GetLocalTriggerPrimitive());;
          RootOutput->cd();
        }
        
        ithSpectra=hSpectra.find(currCellID);
        if (ithSpectra!=hSpectra.end()){
          ithSpectra->second.FillSpectra(lgCorr,hgCorr);
          if (hgCorr > 3*calib.GetPedestalSigH(currCellID) && lgCorr > 3*calib.GetPedestalSigL(currCellID) && hgCorr < 3900 )
            ithSpectra->second.FillCorr(lgCorr,hgCorr);
        } else {
          RootOutputHist->cd("IndividualCells");
          hSpectra[currCellID]=TileSpectra("mip1st",currCellID,calib.GetTileCalib(currCellID),event.GetROtype(),debug);
          hSpectra[currCellID].FillSpectra(lgCorr,hgCorr);;
          if (hgCorr > 3*calib.GetPedestalSigH(currCellID) && lgCorr > 3*calib.GetPedestalSigL(currCellID && hgCorr < 3900) )
            hSpectra[currCellID].FillCorr(lgCorr,hgCorr);;

          RootOutput->cd();
        }
        
      
        // only fill tile spectra if 4 surrounding tiles on average are compatible with muon response
        if (localMuonTrigg){
          aTile->SetLocalTriggerBit(1);
          ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
          ithSpectraTrigg->second.FillSpectra(lgCorr,hgCorr);
          if (hgCorr > 3*calib.GetPedestalSigH(currCellID) && lgCorr > 3*calib.GetPedestalSigL(currCellID) && hgCorr < 3900 )
            ithSpectraTrigg->second.FillCorr(lgCorr,hgCorr);
        }
      // HGCROC treatment
      } else if (typeRO == ReadOut::Type::Hgcroc) {
        Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
        if (i == 0 && debug > 2) std::cout << ((TString)setup->DecodeCellID(aTile->GetCellID())).Data() << std::endl;
        long currCellID = aTile->GetCellID();

        ithSpectraTrigg=hSpectraTrigg.find(currCellID);
        // double adc = aTile->GetPedestal()+aTile->GetIntegratedADC();
        double adc = aTile->GetIntegratedADC();
        double tot = aTile->GetIntegratedTOT();

        // estimate local muon trigger
        bool localMuonTrigg = event.InspectIfLocalMuonTrigg(currCellID, averageScale, factorMinTrigg, factorMaxTrigg);
        
        if(ithSpectraTrigg!=hSpectraTrigg.end()){
          ithSpectraTrigg->second.FillTrigger(aTile->GetLocalTriggerPrimitive());
        } else {
          RootOutputHist->cd("IndividualCellsTrigg");
          hSpectraTrigg[currCellID]=TileSpectra("mipTrigg",currCellID,calib.GetTileCalib(currCellID),event.GetROtype(),debug);
          hSpectraTrigg[currCellID].FillTrigger(aTile->GetLocalTriggerPrimitive());;
          RootOutput->cd();
        }

        ithSpectra=hSpectra.find(currCellID);
        if (ithSpectra!=hSpectra.end()){
          ithSpectra->second.FillSpectra(tot,adc);
          ithSpectra->second.FillCorr(tot,adc);
        } else {
          RootOutputHist->cd("IndividualCells");
          hSpectra[currCellID]=TileSpectra("mip1st",currCellID,calib.GetTileCalib(currCellID),event.GetROtype(),debug);
          hSpectra[currCellID].FillSpectra(tot,adc);;
          hSpectra[currCellID].FillCorr(tot,adc);;
          RootOutput->cd();
        }
        
        // only fill tile spectra if 4 surrounding tiles on average are compatible with muon response
        if (localMuonTrigg){
          aTile->SetLocalTriggerBit(1);
          ithSpectraTrigg=hSpectraTrigg.find(currCellID);
          ithSpectraTrigg->second.FillSpectra(tot,adc);
          ithSpectraTrigg->second.FillCorr(tot,adc);
        }
      }
    }
    RootOutput->cd();
    TdataOut->Fill();
  }
  TdataOut->Write();
  TsetupIn->CloneTree()->Write();
  
  //***********************************************************************************************
  //***** Monitoring histos for fits results of 2nd iteration ******************
  //***********************************************************************************************
  RootOutputHist->cd();
  int maxChannelPerLayer        = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1);
  // monitoring trigger 
  TH2D* hmipTriggers              = new TH2D( "hmipTriggers","muon triggers; layer; brd channel; counts ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hmipTriggers->SetDirectory(0);
  TH2D* hSuppresionNoise          = new TH2D( "hSuppresionNoise","S/B noise region; layer; brd channel; S/B noise region",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hSuppresionNoise->SetDirectory(0);
  TH2D* hSuppresionSignal         = new TH2D( "hSuppresionSignal","S/B signal region; layer; brd channel; S/B signal region",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hSuppresionSignal->SetDirectory(0);

  // monitoring 2nd iteration mip fits
  TH2D* hspectraHGMaxVsLayer   = new TH2D( "hspectraHGMaxVsLayer","Max High Gain; layer; brd channel; Max_{HG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGMaxVsLayer->SetDirectory(0);
  TH2D* hspectraHGFWHMVsLayer   = new TH2D( "hspectraHGFWHMVsLayer","FWHM High Gain; layer; brd channel; FWHM_{HG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGFWHMVsLayer->SetDirectory(0);
  TH2D* hspectraHGLMPVVsLayer   = new TH2D( "hspectraHGLMPVVsLayer","MPV High Gain; layer; brd channel; MPV_{HG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGLMPVVsLayer->SetDirectory(0);
  TH2D* hspectraHGLSigmaVsLayer = new TH2D( "hspectraHGLSigmaVsLayer","Sigma Landau High Gain; layer; brd channel; #sigma_{L,HG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGLSigmaVsLayer->SetDirectory(0);
  TH2D* hspectraHGGSigmaVsLayer = new TH2D( "hspectraHGGSigmaVsLayer","Sigma Gauss High Gain; layer; brd channel; #sigma_{G,HG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGGSigmaVsLayer->SetDirectory(0);
  TH2D* hspectraLGMaxVsLayer   = new TH2D( "hspectraLGMaxVsLayer","Max High Gain; layer; brd channel; Max_{LG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGMaxVsLayer->SetDirectory(0);
  TH2D* hspectraLGFWHMVsLayer   = new TH2D( "hspectraLGFWHMVsLayer","FWHM High Gain; layer; brd channel; FWHM_{LG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGFWHMVsLayer->SetDirectory(0);
  TH2D* hspectraLGLMPVVsLayer   = new TH2D( "hspectraLGLMPVVsLayer","MPV High Gain; layer; brd channel; MPV_{LG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGLMPVVsLayer->SetDirectory(0);
  TH2D* hspectraLGLSigmaVsLayer = new TH2D( "hspectraLGLSigmaVsLayer","Sigma Landau High Gain; layer; brd channel; #sigma_{L,LG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGLSigmaVsLayer->SetDirectory(0);
  TH2D* hspectraLGGSigmaVsLayer = new TH2D( "hspectraLGGSigmaVsLayer","Sigma Gauss High Gain; layer; brd channel; #sigma_{G,LG, 2^{nd}} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGGSigmaVsLayer->SetDirectory(0);

  TH1D* hMaxHG             = new TH1D( "hMaxHG","Max High Gain ;Max_{HG} (arb. units) ; counts ",
                                            2000, -0.5, 2000-0.5);
  hMaxHG->SetDirectory(0);
  TH1D* hMaxLG             = new TH1D( "hMaxLG","Max Low Gain ;Max_{LG} (arb. units) ; counts ",
                                            400, -0.5, 400-0.5);
  hMaxLG->SetDirectory(0);


  int currCells = 0;
  double* parameters    = new double[6];
  double* parErrAndRes  = new double[6];
  bool isGood;
  double meanSB_NoiseR  = 0;
  double meanSB_SigR    = 0;
  if ( debug > 0)
    std::cout << "============================== start fitting improved iteration" << std::endl;  
  
  for(ithSpectraTrigg=hSpectraTrigg.begin(); ithSpectraTrigg!=hSpectraTrigg.end(); ++ithSpectraTrigg){
    if (currCells%20 == 0 && currCells > 0 && debug > 0)
      std::cout << "============================== cell " <<  currCells << " / " << hSpectraTrigg.size() << " cells" << std::endl;
    currCells++;
    for (int p = 0; p < 6; p++){
      parameters[p]   = 0;
      parErrAndRes[p] = 0;
    }
    isGood=ithSpectraTrigg->second.FitMipHG(parameters, parErrAndRes, debug, yearData, true, calib.GetVov(), averageScale);
    
    long cellID     = ithSpectraTrigg->second.GetCellID();
    int layer       = setup->GetLayer(cellID);
    int chInLayer   = setup->GetChannelInLayer(cellID);    
    int bin2D       = hspectraHGMaxVsLayer->FindBin(layer,chInLayer);

    double pedSigHG = 0;
    double maxBin   = 0;
    if (typeRO == ReadOut::Type::Caen){
      pedSigHG = calib.GetPedestalSigH(cellID);
      maxBin   = 3800;
    } else {
      pedSigHG = 20;
      maxBin   = 1024;
    }

    Int_t binNLow   = ithSpectraTrigg->second.GetHG()->FindBin(-1*pedSigHG);
    Int_t binNHigh  = ithSpectraTrigg->second.GetHG()->FindBin(3*pedSigHG);
    Int_t binSHigh  = ithSpectraTrigg->second.GetHG()->FindBin(maxBin);
    
    double S_NoiseR = ithSpectraTrigg->second.GetHG()->Integral(binNLow, binNHigh);
    double S_SigR   = ithSpectraTrigg->second.GetHG()->Integral(binNHigh, binSHigh);
    
    ithSpectra      = hSpectra.find(cellID);
    double B_NoiseR = ithSpectra->second.GetHG()->Integral(binNLow , binNHigh);
    double B_SigR   = ithSpectra->second.GetHG()->Integral(binNHigh, binSHigh);
    
    double SB_NoiseR  = (B_NoiseR != 0.) ? S_NoiseR/B_NoiseR : 0;
    double SB_SigR    = (B_SigR != 0.) ? S_SigR/B_SigR : 0;
    
    meanSB_NoiseR += SB_NoiseR;
    meanSB_SigR += SB_SigR;
    
    hmipTriggers->SetBinContent(bin2D, ithSpectraTrigg->second.GetHG()->GetEntries());
    hSuppresionNoise->SetBinContent(bin2D, SB_NoiseR);
    hSuppresionSignal->SetBinContent(bin2D, SB_SigR);
    if (isGood){
      hspectraHGMaxVsLayer->SetBinContent(bin2D, parameters[4]);
      hspectraHGFWHMVsLayer->SetBinContent(bin2D, parameters[5]);
      hspectraHGLMPVVsLayer->SetBinContent(bin2D, parameters[1]);
      hspectraHGLMPVVsLayer->SetBinError(bin2D, parErrAndRes[1]);
      hspectraHGLSigmaVsLayer->SetBinContent(bin2D, parameters[0]);
      hspectraHGLSigmaVsLayer->SetBinError(bin2D, parErrAndRes[0]);
      hspectraHGGSigmaVsLayer->SetBinContent(bin2D, parameters[3]);
      hspectraHGGSigmaVsLayer->SetBinError(bin2D, parErrAndRes[3]);
      hMaxHG->Fill(parameters[4]);
    }
    
    if (typeRO == ReadOut::Type::Caen) {
      for (int p = 0; p < 6; p++){
        parameters[p]   = 0;
        parErrAndRes[p] = 0;
      }
      isGood=ithSpectraTrigg->second.FitMipLG(parameters, parErrAndRes, debug, yearData, true, averageScaleLow);
      if (isGood){
        hspectraLGMaxVsLayer->SetBinContent(bin2D, parameters[4]);
        hspectraLGFWHMVsLayer->SetBinContent(bin2D, parameters[5]);
        hspectraLGLMPVVsLayer->SetBinContent(bin2D, parameters[1]);
        hspectraLGLMPVVsLayer->SetBinError(bin2D, parErrAndRes[1]);
        hspectraLGLSigmaVsLayer->SetBinContent(bin2D, parameters[0]);
        hspectraLGLSigmaVsLayer->SetBinError(bin2D, parErrAndRes[0]);
        hspectraLGGSigmaVsLayer->SetBinContent(bin2D, parameters[3]);
        hspectraLGGSigmaVsLayer->SetBinError(bin2D, parErrAndRes[3]);
        hMaxLG->Fill(parameters[4]);
      }
    }
  }
  if ( debug > 0)
    std::cout << "============================== done fitting improved iteration" << std::endl;

  
  meanSB_NoiseR = meanSB_NoiseR/hSpectraTrigg.size();
  meanSB_SigR   = meanSB_SigR/hSpectraTrigg.size();
  
  RootOutput->cd();
  
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }
  
  TcalibOut->Fill();
  TcalibOut->Write();
  int actChA                     = 0;
  double averageScaleUpdated     = calib.GetAverageScaleHigh(actChA);
  double averageScaleUpdatedLow  = 0.;
  double meanFWHMHG              = calib.GetAverageScaleWidthHigh();
  double meanFWHMLG              = 0.;

  if (typeRO == ReadOut::Type::Caen) {
    averageScaleUpdatedLow  = calib.GetAverageScaleLow();
    meanFWHMLG              = calib.GetAverageScaleWidthLow();
  }
  
  std::cout << "average input HG mip: " << averageScale << " LG mip: "<< averageScaleLow << "\t act. ch: "<< actChI<< std::endl;
  std::cout << "average updated HG mip: " << averageScaleUpdated << " LG mip: "<< averageScaleUpdatedLow << "\t act. ch: "<< actChA<< std::endl;

  RootOutput->Close();


  RootOutputHist->cd("IndividualCellsTrigg");
    for(ithSpectra=hSpectraTrigg.begin(); ithSpectra!=hSpectraTrigg.end(); ++ithSpectra){
      ithSpectra->second.Write(true);
    }
  RootOutputHist->cd();
    
    hspectraHGMaxVsLayer->Write();
    hspectraHGFWHMVsLayer->Write();
    hspectraHGLMPVVsLayer->Write();
    hspectraHGLSigmaVsLayer->Write();
    hspectraHGGSigmaVsLayer->Write();
    hMaxHG->Write();
    
    if (typeRO == ReadOut::Type::Caen){
      hspectraLGMaxVsLayer->Write();
      hspectraLGFWHMVsLayer->Write();
      hspectraLGLMPVVsLayer->Write();
      hspectraLGLSigmaVsLayer->Write();
      hspectraLGGSigmaVsLayer->Write();
      hMaxLG->Write();
    }
    hmipTriggers->Write();
    hSuppresionNoise->Write();
    hSuppresionSignal->Write();
  // fill calib tree & write it
  // close open root files
  RootOutputHist->Write();
  RootOutputHist->Close();

  RootInput->Close();

  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find(runNr);
  
  // create directory for plot output
  TString outputDirPlots = GetPlotOutputDir();
  gSystem->Exec("mkdir -p "+outputDirPlots);
  
  //**********************************************************************
  // Create canvases for channel overview plotting
  //**********************************************************************
  Double_t textSizeRel = 0.035;
  StyleSettingsBasics("pdf");
  SetPlotStyle();

  TCanvas* canvas2DCorr = new TCanvas("canvasCorrPlots","",0,0,1450,1300);  // gives the page size
  DefaultCancasSettings( canvas2DCorr, 0.08, 0.13, 0.045, 0.07);

  canvas2DCorr->SetLogz(0);
  PlotSimple2D( canvas2DCorr, hspectraHGMaxVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_MaxMip.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "#LT Max_{HG} #GT = %.1f", averageScaleUpdated) );
  PlotSimple2D( canvas2DCorr, hspectraHGFWHMVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_FWHMMip.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "#LT FWHM_{HG} #GT = %.1f", meanFWHMHG));
  PlotSimple2D( canvas2DCorr, hspectraHGLMPVVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_LandMPVMip.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGLSigmaVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_LandSigMip.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGGSigmaVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_GaussSigMip.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  canvas2DCorr->SetLogz(1);
  TString drawOpt = "colz";
  if (yearData == 2023)
    drawOpt = "colz,text";
  PlotSimple2D( canvas2DCorr, hmipTriggers, -10000, -10000, textSizeRel, Form("%s/MuonTriggers.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, drawOpt, true, Form( "evt. collected = %d", evts));
  PlotSimple2D( canvas2DCorr, hSuppresionNoise, -10000, -10000, textSizeRel, Form("%s/SuppressionNoise.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, drawOpt, true, Form( "#LT S/B noise #GT = %.3f", meanSB_NoiseR));
  PlotSimple2D( canvas2DCorr, hSuppresionSignal, -10000, -10000, textSizeRel, Form("%s/SuppressionSignal.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, drawOpt, true, Form( "#LT S/B signal #GT = %.3f", meanSB_SigR));
  
  if (typeRO == ReadOut::Type::Caen){
    canvas2DCorr->SetLogz(0);
    PlotSimple2D( canvas2DCorr, hspectraLGMaxVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_MaxMip.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "#LT Max_{LG} #GT = %.1f", averageScaleUpdatedLow));
    PlotSimple2D( canvas2DCorr, hspectraLGFWHMVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_FWHMMip.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "#LT FWHM_{LG} #GT = %.1f", meanFWHMLG));
    PlotSimple2D( canvas2DCorr, hspectraLGLMPVVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_LandMPVMip.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
    PlotSimple2D( canvas2DCorr, hspectraLGLSigmaVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_LandSigMip.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
    PlotSimple2D( canvas2DCorr, hspectraLGGSigmaVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_GaussSigMip.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  }
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
 
  calib.PrintGlobalInfo();  
  Double_t maxHG = ReturnMipPlotRangeDepVov(calib.GetVov(),true, typeRO);
  Double_t maxLG = ReturnMipPlotRangeDepVov(calib.GetVov(),false, typeRO);
  std::cout << "plotting single layers" << std::endl;
  for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){   
    for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){ 
      if (l%10 == 0 && l > 0 && debug > 0)
        std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;
      PlotMipWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                hSpectra, hSpectraTrigg, setup, true, -100, maxHG, 1.2, l, m,
                                Form("%s/MIP_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
      Double_t maxTriggPPlot = maxHG*2;
      if (typeRO != ReadOut::Type::Caen)
        maxTriggPPlot = 500;
      PlotTriggerPrimWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                        hSpectraTrigg, setup, averageScale, factorMinTrigg, factorMaxTrigg,
                                        0, maxTriggPPlot, 1.2, l, m, Form("%s/TriggPrimitive_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
      if (typeRO == ReadOut::Type::Caen){
        PlotMipWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                  hSpectra, hSpectraTrigg, setup, false, -20, maxLG, 1.2, l, m,
                                  Form("%s/MIP_LG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
      }
    }
  }
  std::cout << "done plotting" << std::endl;
  
  if (ExtPlot > 0){
    TString outputDirPlotsSingle = Form("%s/SingleTiles",outputDirPlots.Data());
    gSystem->Exec("mkdir -p "+outputDirPlotsSingle);

    
    TCanvas* canvasSTile = new TCanvas("canvasSignleTile","",0,0,1600,1300);  // gives the page size
    DefaultCancasSettings( canvasSTile, 0.08, 0.01, 0.01, 0.082);

    int counter = 0;
    for(ithSpectraTrigg=hSpectraTrigg.begin(); ithSpectraTrigg!=hSpectraTrigg.end(); ++ithSpectraTrigg){
      counter++;
      long cellID     = ithSpectraTrigg->second.GetCellID();
      int row = setup->GetRow(cellID);
      int col = setup->GetColumn(cellID);
      int lay = setup->GetLayer(cellID);
      int mod = setup->GetModule(cellID);

      PlotMipWithFitsSingleTile (canvasSTile, 0.95,  0.95, Double_t(textSizePixel)*2/1600., textSizePixel*2, 
                                hSpectra, hSpectraTrigg, true,  -100, maxHG, 1.2, cellID,  Form("%s/MIP_HG_Tile_M%02d_L%02d_R%02d_C%02d.%s" ,outputDirPlotsSingle.Data(), mod, lay, row, col, plotSuffix.Data()), it->second);
    }
  }
  return true;
}



//***********************************************************************************************
//*********************** improved scaling calculation function *********************************
//***********************************************************************************************
bool Analyses::GetNoiseSampleAndRefitPedestal(void){
  std::cout<<"GetNoiseSampleAndRefitPedestal"<<std::endl;
  
  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  
  std::map<int,TileSpectra> hSpectra;
  std::map<int,TileSpectra> hSpectraTrigg;
  std::map<int, TileSpectra>::iterator ithSpectra;
  std::map<int, TileSpectra>::iterator ithSpectraTrigg;
  
  std::cout << "Additional Output with histos being created: " << RootOutputNameHist.Data() << std::endl;
  if(Overwrite){
    std::cout << "recreating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"RECREATE");
  } else{
    std::cout << "newly creating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"CREATE");
  }
    
  // setup trigger sel
  double factorMinTrigg   = 0.5;
  if(yearData == 2023)
    factorMinTrigg        = 0.1;
  // create HG and LG histo's per channel
  TH2D* hspectraHGvsCellID      = new TH2D( "hNoiseTriggeredSpectraHG_vsCellID","Noise trigg ADC spectrum High Gain vs CellID; cell ID; ADC_{HG} (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,0,4000);
  hspectraHGvsCellID->SetDirectory(0);
  TH2D* hspectraLGvsCellID      = new TH2D( "hNoiseTriggeredSpectraLG_vsCellID","Noise trigg ADC spectrum Low Gain vs CellID; cell ID; ADC_{LG} (arb. units)  ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,0,4000);
  hspectraLGvsCellID->SetDirectory(0);

  
  RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->mkdir("IndividualCellsTrigg");
  RootOutputHist->cd("IndividualCellsTrigg");  
  
  //***********************************************************************************************
  //************************* first pass over tree to extract spectra *****************************
  //***********************************************************************************************  
  TcalibIn->GetEntry(0);
  // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }
  
  int evts=TdataIn->GetEntries();
  int runNr = -1;
  int actCh = 0;
  double averageScale     = calib.GetAverageScaleHigh(actCh);
  double averageScaleLow  = calib.GetAverageScaleLow();
  std::cout << "average HG mip: " << averageScale << " LG mip: "<< averageScaleLow << std::endl;
  for(int i=0; i<evts; i++){
    TdataIn->GetEntry(i);
    if (i == 0)runNr = event.GetRunNumber();
    TdataIn->GetEntry(i);
    if (i%5000 == 0 && i > 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      Caen* aTile=(Caen*)event.GetTile(j);
      if (i == 0 && debug > 2) std::cout << ((TString)setup->DecodeCellID(aTile->GetCellID())).Data() << std::endl;
      long currCellID = aTile->GetCellID();
      
      // read current tile
      ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
      // estimate local muon trigger
      bool localNoiseTrigg = event.InspectIfNoiseTrigg(currCellID, averageScale, factorMinTrigg);
      
      if(ithSpectraTrigg!=hSpectraTrigg.end()){
        ithSpectraTrigg->second.FillTrigger(aTile->GetLocalTriggerPrimitive());
      } else {
        RootOutputHist->cd("IndividualCellsTrigg");
        hSpectraTrigg[currCellID]=TileSpectra("mipTrigg",currCellID,calib.GetTileCalib(currCellID),event.GetROtype(),debug);
        hSpectraTrigg[currCellID].FillTrigger(aTile->GetLocalTriggerPrimitive());;
        RootOutput->cd();
      }
      
      ithSpectra=hSpectra.find(aTile->GetCellID());
      if (ithSpectra!=hSpectra.end()){
        ithSpectra->second.FillSpectra(aTile->GetADCLow(),aTile->GetADCHigh());
      } else {
        RootOutputHist->cd("IndividualCells");
        hSpectra[currCellID]=TileSpectra("mip1st",currCellID,calib.GetTileCalib(currCellID),event.GetROtype(),debug);
        hSpectra[aTile->GetCellID()].FillSpectra(aTile->GetADCLow(),aTile->GetADCHigh());;

        RootOutput->cd();
      }
     
      // only fill tile spectra if X surrounding tiles on average are compatible with pure noise
      if (localNoiseTrigg){
        aTile->SetLocalTriggerBit(2);
        ithSpectraTrigg=hSpectraTrigg.find(aTile->GetCellID());
        ithSpectraTrigg->second.FillSpectra(aTile->GetADCLow(),aTile->GetADCHigh());
        
        hspectraHGvsCellID->Fill(aTile->GetCellID(), aTile->GetADCHigh());
        hspectraLGvsCellID->Fill(aTile->GetCellID(), aTile->GetADCLow());
      }
    }
    RootOutput->cd();
    TdataOut->Fill();
  }
  TdataOut->Write();
  TsetupIn->CloneTree()->Write();
  
  //***********************************************************************************************
  //***** Monitoring histos for fits results of 2nd iteration ******************
  //***********************************************************************************************
  RootOutputHist->cd();
  int maxChannelPerLayer        = (setup->GetNMaxColumn()+1)*(setup->GetNMaxRow()+1);
  // monitoring trigger 
  TH2D* hnoiseTriggers              = new TH2D( "hnoiseTriggers","muon triggers; layer; brd channel; counts ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hnoiseTriggers->SetDirectory(0);
  TH1D* hMeanPedHGvsCellID      = new TH1D( "hMeanPedHG_vsCellID","mean Ped High Gain vs CellID ; cell ID; #mu_{noise, HG} (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5);
  hMeanPedHGvsCellID->SetDirectory(0);
  TH2D* hspectraHGMeanVsLayer   = new TH2D( "hspectraHGMeanVsLayer","Mean Ped High Gain vs CellID; layer; brd channel; #mu_{Ped HG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGMeanVsLayer->SetDirectory(0);
  TH2D* hspectraHGSigmaVsLayer  = new TH2D( "hspectraHGSigmaVsLayer","Mean Ped High Gain vs CellID; layer; brd channel; #sigma_{Ped HG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraHGSigmaVsLayer->SetDirectory(0);
  TH1D* hMeanPedLGvsCellID      = new TH1D( "hMeanPedLG_vsCellID","mean Ped Low Gain vs CellID ; cell ID; #mu_{noise, LG} (arb. units) ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5);
  hMeanPedLGvsCellID->SetDirectory(0);
  TH2D* hspectraLGMeanVsLayer   = new TH2D( "hspectraLGMeanVsLayer","Mean Ped Low Gain vs CellID; layer; brd channel; #mu_{PED LG} (arb. units) ",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGMeanVsLayer->SetDirectory(0);
  TH2D* hspectraLGSigmaVsLayer  = new TH2D( "hspectraLGSigmaVsLayer","Mean Ped Low Gain vs CellID; layer; brd channel; #sigma_{Ped LG} (arb. units)",
                                            setup->GetNMaxLayer()+1, -0.5, setup->GetNMaxLayer()+1-0.5, maxChannelPerLayer, -0.5, maxChannelPerLayer-0.5);
  hspectraLGSigmaVsLayer->SetDirectory(0);

  if ( debug > 0)
    std::cout << "============================== starting fitting" << std::endl;

  int currCells = 0;
  double* parameters    = new double[6];
  for(ithSpectraTrigg=hSpectraTrigg.begin(); ithSpectraTrigg!=hSpectraTrigg.end(); ++ithSpectraTrigg){
    for (int p = 0; p < 6; p++){
      parameters[p]   = 0;
    }

    if (currCells%20 == 0 && currCells > 0 && debug > 0)
      std::cout << "============================== cell " <<  currCells << " / " << hSpectraTrigg.size() << " cells" << std::endl;
    currCells++;
    if ( debug > 2) std::cout << ((TString)setup->DecodeCellID(ithSpectraTrigg->second.GetCellID())).Data() << std::endl;
    ithSpectraTrigg->second.FitNoise(parameters, yearData, true);
    hMeanPedHGvsCellID->SetBinContent(hMeanPedHGvsCellID->FindBin(ithSpectraTrigg->second.GetCellID()), parameters[4]);
    hMeanPedHGvsCellID->SetBinError  (hMeanPedHGvsCellID->FindBin(ithSpectraTrigg->second.GetCellID()), parameters[6]);
    hMeanPedLGvsCellID->SetBinContent(hMeanPedLGvsCellID->FindBin(ithSpectraTrigg->second.GetCellID()), parameters[0]);
    hMeanPedLGvsCellID->SetBinError  (hMeanPedLGvsCellID->FindBin(ithSpectraTrigg->second.GetCellID()), parameters[2]);
    
    int layer     = setup->GetLayer(ithSpectraTrigg->second.GetCellID());
    int chInLayer = setup->GetChannelInLayer(ithSpectraTrigg->second.GetCellID());
  
    hspectraHGMeanVsLayer->SetBinContent(hspectraHGMeanVsLayer->FindBin(layer,chInLayer), parameters[4]);
    hspectraHGMeanVsLayer->SetBinError(hspectraHGMeanVsLayer->FindBin(layer,chInLayer), parameters[5]);
    hspectraHGSigmaVsLayer->SetBinContent(hspectraHGSigmaVsLayer->FindBin(layer,chInLayer), parameters[6]);
    hspectraHGSigmaVsLayer->SetBinError(hspectraHGSigmaVsLayer->FindBin(layer,chInLayer), parameters[7]);
    hspectraLGMeanVsLayer->SetBinContent(hspectraLGMeanVsLayer->FindBin(layer,chInLayer), parameters[0]);
    hspectraLGMeanVsLayer->SetBinError(hspectraLGMeanVsLayer->FindBin(layer,chInLayer), parameters[1]);
    hspectraLGSigmaVsLayer->SetBinContent(hspectraLGSigmaVsLayer->FindBin(layer,chInLayer), parameters[2]);
    hspectraLGSigmaVsLayer->SetBinError(hspectraLGSigmaVsLayer->FindBin(layer,chInLayer), parameters[3]);
    
    hnoiseTriggers->SetBinContent(hnoiseTriggers->FindBin(layer,chInLayer), ithSpectraTrigg->second.GetHG()->GetEntries());
  }
  if ( debug > 0)
    std::cout << "============================== done fitting" << std::endl;
  
  RootOutput->cd();
  
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }


  TcalibOut->Fill();
  TcalibOut->Write();
  
  RootOutput->Write();
  RootOutput->Close();


  RootOutputHist->cd("IndividualCellsTrigg");
    for(ithSpectra=hSpectraTrigg.begin(); ithSpectra!=hSpectraTrigg.end(); ++ithSpectra){
      ithSpectra->second.Write(true);
    }
  RootOutputHist->cd();
    
    hMeanPedHGvsCellID->Write();
    hMeanPedLGvsCellID->Write();
    hspectraHGMeanVsLayer->Write();
    hspectraHGSigmaVsLayer->Write();
    hspectraLGMeanVsLayer->Write(); 
    hspectraLGSigmaVsLayer->Write();
    hspectraHGvsCellID->Write();
    hspectraLGvsCellID->Write();
        
    hnoiseTriggers->Write();
  // close open root files
  RootOutputHist->Write();
  RootOutputHist->Close();

  RootInput->Close();

  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find(runNr);
  
  // create directory for plot output
  TString outputDirPlots = GetPlotOutputDir();
  gSystem->Exec("mkdir -p "+outputDirPlots);
  
  //**********************************************************************
  // Create canvases for channel overview plotting
  //**********************************************************************
  Double_t textSizeRel = 0.035;
  StyleSettingsBasics("pdf");
  SetPlotStyle();

  TCanvas* canvas2DCorr = new TCanvas("canvasCorrPlots","",0,0,1450,1300);  // gives the page size
  DefaultCancasSettings( canvas2DCorr, 0.08, 0.13, 0.045, 0.07);

  canvas2DCorr->SetLogz(0);
  PlotSimple2D( canvas2DCorr, hspectraHGMeanVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_NoiseMean.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true );
  PlotSimple2D( canvas2DCorr, hspectraHGSigmaVsLayer, -10000, -10000, textSizeRel, Form("%s/HG_NoiseSigma.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraLGMeanVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_NoiseMean.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraLGSigmaVsLayer, -10000, -10000, textSizeRel, Form("%s/LG_NoiseSigma.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  canvas2DCorr->SetLogz(1);
  PlotSimple2D( canvas2DCorr, hspectraHGvsCellID, -10000, -10000, textSizeRel, Form("%s/HG_Noise.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraLGvsCellID, -10000, -10000, textSizeRel, Form("%s/LG_Noise.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  
  PlotSimple2D( canvas2DCorr, hnoiseTriggers, -10000, -10000, textSizeRel, Form("%s/LG_Noise.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, Form( "evt. coll = %d", evts));
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
 
  for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
    for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){    
      PlotNoiseAdvWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                      hSpectra, hSpectraTrigg, true, 0, 450, 1.2, l, m,
                                      Form("%s/NoiseTrigg_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
    }
  }

  
  return true;
}


//***********************************************************************************************
//*********************** Calibration routine ***************************************************
//***********************************************************************************************
bool Analyses::RunEvalLocalTriggers(void){
  std::cout<<"EvalLocalTriggers"<<std::endl;

  RootOutput->cd();
  std::cout << "starting to run trigger eval: " << TcalibIn <<  "\t" << TcalibIn->GetEntry(0) << std::endl;
  TcalibIn->GetEntry(0);
  // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }
  
  int actCh1st = 0;
  double averageScale = calib.GetAverageScaleHigh(actCh1st);
  double avLGHGCorr   = calib.GetAverageLGHGCorr();
  std::cout << "average HG mip: " << averageScale << "\t active ch: "<< actCh1st<< std::endl;
  
  // setup local trigger sel
  TRandom3* rand    = new TRandom3();
  Int_t localTriggerTiles = 4;
  if (yearData == 2023){
    localTriggerTiles = 6;
  }
  double factorMinTrigg   = 0.8;
  double factorMinTriggNoise = 0.2;
  double factorMaxTrigg   = 2.;
  if (yearData == 2023){
    factorMinTrigg    = 0.9;
    factorMaxTrigg    = 2.;
  }
  
  int outCount      = 1000;
  int evts=TdataIn->GetEntries();
  
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }
  
  if (evts < 10000)
    outCount  = 500;
  if (evts > 100000)
    outCount  = 5000;
  int runNr = -1;
  for(int i=0; i<evts && i < maxEvents; i++){
    TdataIn->GetEntry(i);
    if(debug==1000){
        std::cerr<<event.GetTimeStamp()<<std::endl;
      }
    if (i == 0){
      runNr = event.GetRunNumber();
      std::cout<< "original run numbers calib: "<<calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
      calib.SetRunNumber(runNr);
      calib.SetBeginRunTime(event.GetBeginRunTimeAlt());
      std::cout<< "reset run numbers calib: "<< calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
    }
    
    if (i%outCount == 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      Caen* aTile=(Caen*)event.GetTile(j);      
      // calculate trigger primitives
      aTile->SetLocalTriggerPrimitive(event.CalculateLocalMuonTrigg(calib, rand, aTile->GetCellID(), localTriggerTiles, avLGHGCorr));
      bool localMuonTrigg   = event.InspectIfLocalMuonTrigg(aTile->GetCellID(), averageScale, factorMinTrigg, factorMaxTrigg);
      bool localNoiseTrigg  = event.InspectIfNoiseTrigg(aTile->GetCellID(), averageScale, factorMinTriggNoise);
      aTile->SetLocalTriggerBit(0);
      if (localMuonTrigg) aTile->SetLocalTriggerBit(1);
      if (localNoiseTrigg) aTile->SetLocalTriggerBit(2);
    }
    TdataOut->Fill();
  }
  TdataOut->Write();
  TsetupIn->CloneTree()->Write();
  
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }
  
  TcalibOut->Fill();
  TcalibOut->Write();
  
  RootOutput->Close();
  RootInput->Close();      
  
  return true;
}

//***********************************************************************************************
//*********************** Calibration routine ***************************************************
//***********************************************************************************************
bool Analyses::Calibrate(void){
  std::cout<<"Calibrate"<<std::endl;

  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);

  std::cout << "Additional Output with histos being created: " << RootOutputNameHist.Data() << std::endl;
  if(Overwrite){
    std::cout << "recreating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"RECREATE");
  } else{
    std::cout << "newly creating file with hists" << std::endl;
    RootOutputHist = new TFile(RootOutputNameHist.Data(),"CREATE");
  }
  
  // create HG and LG histo's per channel
  TH2D* hspectraHGCorrvsCellID      = new TH2D( "hspectraHGCorr_vsCellID","ADC spectrum High Gain corrected vs CellID; cell ID; ADC_{HG} (arb. units)  ; counts ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,-200,3800);
  hspectraHGCorrvsCellID->SetDirectory(0);
  TH2D* hspectraHGCorrvsCellIDNoise      = new TH2D( "hspectraHGCorr_vsCellID_Noise","ADC spectrum High Gain corrected vs CellID Noise; cell ID; ADC_{HG} (arb. units)  ; counts ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,-200,3800);
  hspectraHGCorrvsCellIDNoise->SetDirectory(0);
  TH2D* hspectraLGCorrvsCellID      = new TH2D( "hspectraLGCorr_vsCellID","ADC spectrum Low Gain corrected vs CellID; cell ID; ADC_{LG} (arb. units)  ; counts  ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,-200,3800);
  hspectraLGCorrvsCellID->SetDirectory(0);
  TH2D* hspectraLGCorrvsCellIDNoise      = new TH2D( "hspectraLGCorr_vsCellID_Noise","ADC spectrum Low Gain corrected vs CellID Noise; cell ID; ADC_{LG} (arb. units)  ; counts  ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,-200,3800);
  hspectraLGCorrvsCellIDNoise->SetDirectory(0);
  TH2D* hspectraHGvsCellID      = new TH2D( "hspectraHG_vsCellID","ADC spectrum High Gain vs CellID; cell ID; ADC_{HG} (arb. units)   ; counts ",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,0,4000);
  hspectraHGvsCellID->SetDirectory(0);
  TH2D* hspectraLGvsCellID      = new TH2D( "hspectraLG_vsCellID","ADC spectrum Low Gain vs CellID; cell ID; ADC_{LG} (arb. units)  ; counts",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 4000,0,4000);
  hspectraLGvsCellID->SetDirectory(0);
  TH2D* hspectraEnergyvsCellID  = new TH2D( "hspectraEnergy_vsCellID","Energy vs CellID; cell ID; E (mip eq./tile)    ; counts",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 6000,0,200);
  hspectraEnergyvsCellID->SetDirectory(0);
  TH2D* hspectraEnergyTotvsNCells  = new TH2D( "hspectraTotEnergy_vsNCells","Energy vs CellID; N_{Cells}; E_{tot} (mip eq./tile) ; counts",
                                            setup->GetMaxCellID()+1, -0.5, setup->GetMaxCellID()+1-0.5, 6000,0,1000);
  hspectraEnergyTotvsNCells->SetDirectory(0);

  std::map<int,TileSpectra> hSpectra;
  std::map<int, TileSpectra>::iterator ithSpectra;
  std::map<int,TileSpectra> hSpectraNoise;
  std::map<int, TileSpectra>::iterator ithSpectraNoise;
  
  // entering histoOutput file
  RootOutputHist->mkdir("IndividualCells");
  RootOutputHist->cd("IndividualCells");
  RootOutputHist->mkdir("IndividualCellsNoise");
  RootOutputHist->cd("IndividualCellsNoise");

  Int_t runNr = -1;
  RootOutput->cd();
  std::cout << "starting to run calibration: " << TcalibIn <<  "\t" << TcalibIn->GetEntry(0) << std::endl;
  TcalibIn->GetEntry(0);
  // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }

  int actCh1st = 0;
  double averageScale = calib.GetAverageScaleHigh(actCh1st);
  double avLGHGCorr   = calib.GetAverageLGHGCorr();
  std::cout << "average HG mip: " << averageScale << "\t active ch: "<< actCh1st<< std::endl;
  
  // setup local trigger sel
  TRandom3* rand    = new TRandom3();
  Int_t localTriggerTiles = 4;
  if (yearData == 2023){
    localTriggerTiles = 6;
  }
  double factorMinTrigg   = 0.8;
  double factorMinTriggNoise = 0.2;
  double factorMaxTrigg   = 2.;
  if (yearData == 2023){
    factorMinTrigg    = 0.9;
    factorMaxTrigg    = 2.;
  }

  
  double minMipFrac = 0.3;
  int corrHGADCSwap = 3500;
  int outCount      = 5000;
  int evts=TdataIn->GetEntries();
  
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }

  if (evts < 10000)
    outCount  = 500;
  for(int i=0; i<evts && i < maxEvents; i++){
    if(debug==1000){
        std::cerr<<event.GetTimeStamp()<<std::endl;
      }
    TdataIn->GetEntry(i);
    if (i%outCount == 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    if (i == 0){
      runNr = event.GetRunNumber();
      std::cout<< "original run numbers calib: "<<calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
      calib.SetRunNumber(runNr);
      calib.SetBeginRunTime(event.GetBeginRunTimeAlt());
      std::cout<< "reset run numbers calib: "<< calib.GetRunNumber() << "\t" << calib.GetRunNumberPed() << "\t" << calib.GetRunNumberMip() << std::endl;
    }
    double Etot = 0;
    int nCells  = 0;
    for(int j=0; j<event.GetNTiles(); j++){
      Caen* aTile=(Caen*)event.GetTile(j);
      // remove bad channels from output
      if (calib.GetBCCalib() && calib.GetBadChannel(aTile->GetCellID())!= 3 ){
        event.RemoveTile(aTile);
        j--;        
        continue;
      }
      
      double energy = 0;
      double corrHG = aTile->GetADCHigh()-calib.GetPedestalMeanH(aTile->GetCellID());
      double corrLG = aTile->GetADCLow()-calib.GetPedestalMeanL(aTile->GetCellID());
      double corrLG_HGeq = corrLG*calib.GetLGHGCorr(aTile->GetCellID()) + calib.GetLGHGCorrOff(aTile->GetCellID());
      if(corrHG<corrHGADCSwap){
        if(corrHG/calib.GetScaleHigh(aTile->GetCellID()) > minMipFrac){
          energy=corrHG/calib.GetScaleHigh(aTile->GetCellID());
        }
      } else {
        energy=corrLG/calib.GetCalcScaleLow(aTile->GetCellID());
      }
      if (debug > 1 && corrHG >= corrHGADCSwap-100 && corrHG < 4000-calib.GetPedestalMeanH(aTile->GetCellID())){
          std::cout << "-> Cell ID: " <<  aTile->GetCellID() << "\t HG\t" << corrHG << "\t" << corrHG/calib.GetScaleHigh(aTile->GetCellID()) << "\t LG \t" << corrLG << "\t" <<  corrLG/calib.GetCalcScaleLow(aTile->GetCellID()) << "\t"<< corrLG/calib.GetScaleLow(aTile->GetCellID()) << "\t delta: \t"<< corrHG/calib.GetScaleHigh(aTile->GetCellID())-(corrLG/calib.GetCalcScaleLow(aTile->GetCellID())) << "\tLGHG\t" << calib.GetLGHGCorr(aTile->GetCellID())<< std::endl;
      }
      // calculate trigger primitives
      bool localMuonTrigg   = false;
      bool localNoiseTrigg  = false;

      if (!UseLocTriggFromFile()){
        aTile->SetLocalTriggerPrimitive(event.CalculateLocalMuonTrigg(calib, rand, aTile->GetCellID(), localTriggerTiles, avLGHGCorr));
        aTile->SetLocalTriggerBit(0);
        localMuonTrigg   = event.InspectIfLocalMuonTrigg(aTile->GetCellID(), averageScale, factorMinTrigg, factorMaxTrigg);
        localNoiseTrigg  = event.InspectIfNoiseTrigg(aTile->GetCellID(), averageScale, factorMinTriggNoise);
        if (localMuonTrigg) aTile->SetLocalTriggerBit(1);
        if (localNoiseTrigg) aTile->SetLocalTriggerBit(2);
      } else {
        if (aTile->GetLocalTriggerBit() == 2)  localNoiseTrigg = true;
      }
      
      hspectraHGvsCellID->Fill(aTile->GetCellID(), aTile->GetADCHigh());
      hspectraLGvsCellID->Fill(aTile->GetCellID(), aTile->GetADCLow());
      hspectraHGCorrvsCellID->Fill(aTile->GetCellID(), corrHG);
      hspectraLGCorrvsCellID->Fill(aTile->GetCellID(), corrLG);
      
      if (localNoiseTrigg){
        hspectraHGCorrvsCellIDNoise->Fill(aTile->GetCellID(), corrHG);
        hspectraLGCorrvsCellIDNoise->Fill(aTile->GetCellID(), corrLG);
      
      }
      ithSpectra=hSpectra.find(aTile->GetCellID());
      if(ithSpectra!=hSpectra.end()){
        ithSpectra->second.FillExt(corrLG,corrHG,energy,corrLG_HGeq);
      } else {
        RootOutputHist->cd("IndividualCells");
        hSpectra[aTile->GetCellID()]=TileSpectra("Calibrate",1,aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
        hSpectra[aTile->GetCellID()].FillExt(corrLG,corrHG,energy,corrLG_HGeq);
        RootOutput->cd();
      }

      if (localNoiseTrigg){
        ithSpectraNoise=hSpectraNoise.find(aTile->GetCellID());
        if(ithSpectraNoise!=hSpectraNoise.end()){
          ithSpectraNoise->second.FillExt(corrLG,corrHG,energy,corrLG_HGeq);
        } else {
          RootOutputHist->cd("IndividualCellsNoise");
          hSpectraNoise[aTile->GetCellID()]=TileSpectra("CalibrateNoise",1,aTile->GetCellID(),calib.GetTileCalib(aTile->GetCellID()),event.GetROtype(),debug);
          hSpectraNoise[aTile->GetCellID()].FillExt(corrLG,corrHG,energy,corrLG_HGeq);
          RootOutput->cd();
        }
      }
      
      if(energy!=0){ 
        aTile->SetE(energy);
        hspectraEnergyvsCellID->Fill(aTile->GetCellID(), energy);
        Etot=Etot+energy;
        nCells++;
      } else {
        event.RemoveTile(aTile);
        j--;
      }
    }
    hspectraEnergyTotvsNCells->Fill(nCells,Etot);
    RootOutput->cd();
    TdataOut->Fill();
  }
  TdataOut->Write();
  TsetupIn->CloneTree()->Write();
  
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }
  
  TcalibOut->Fill();
  TcalibOut->Write();
  
  
  RootOutput->Close();
  RootInput->Close();      
  
  RootOutputHist->cd();

  hspectraHGvsCellID->Write();
  hspectraLGvsCellID->Write();
  hspectraHGCorrvsCellID->Write();
  hspectraLGCorrvsCellID->Write();
  hspectraHGCorrvsCellIDNoise->Write();
  hspectraLGCorrvsCellIDNoise->Write();
  hspectraEnergyvsCellID->Write();
  hspectraEnergyTotvsNCells->Write();
  
  TH1D* hspectraEnergyTot = (TH1D*)hspectraEnergyTotvsNCells->ProjectionY();
  hspectraEnergyTot->SetDirectory(0);
  TH1D* hspectraNCells = (TH1D*)hspectraEnergyTotvsNCells->ProjectionX();
  hspectraNCells->SetDirectory(0);
  hspectraEnergyTot->Write("hTotEnergy");
  hspectraNCells->Write("hNCells");

  RootOutputHist->cd("IndividualCells");
  for(ithSpectra=hSpectra.begin(); ithSpectra!=hSpectra.end(); ++ithSpectra){
    ithSpectra->second.FitLGHGCorr(debug,false);
    ithSpectra->second.WriteExt(true);
  }
  RootOutputHist->cd("IndividualCellsNoise");
  for(ithSpectraNoise=hSpectraNoise.begin(); ithSpectraNoise!=hSpectraNoise.end(); ++ithSpectraNoise){
    ithSpectraNoise->second.WriteExt(true);
  }
  
  RootOutputHist->Close();
  //**********************************************************************
  //********************* Plotting ***************************************
  //**********************************************************************
  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find(runNr);
  
  TString outputDirPlots = GetPlotOutputDir();
  gSystem->Exec("mkdir -p "+outputDirPlots);
  
  //**********************************************************************
  // Create canvases for channel overview plotting
  //**********************************************************************
  Double_t textSizeRel = 0.035;
  StyleSettingsBasics("pdf");
  SetPlotStyle();
  
  TCanvas* canvas2DCorr = new TCanvas("canvasCorrPlots","",0,0,1450,1300);  // gives the page size
  DefaultCancasSettings( canvas2DCorr, 0.08, 0.13, 0.045, 0.07);
  canvas2DCorr->SetLogz(1);
  PlotSimple2D( canvas2DCorr, hspectraHGvsCellID, -10000, -10000, textSizeRel, Form("%s/HG.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraLGvsCellID, -10000, -10000, textSizeRel, Form("%s/LG.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGCorrvsCellID, -10000, -10000, textSizeRel, Form("%s/HGCorr.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGCorrvsCellID, 300, -10000, textSizeRel, Form("%s/HGCorr_zoomed.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraHGCorrvsCellIDNoise, -50, 200, -10000, textSizeRel, Form("%s/HGCorr_Noise.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, "Local Noise triggered");
  PlotSimple2D( canvas2DCorr, hspectraLGCorrvsCellID, -10000, -10000, textSizeRel, Form("%s/LGCorr.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraLGCorrvsCellID, 200, -10000, textSizeRel, Form("%s/LGCorr_zoomed.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraLGCorrvsCellIDNoise, -50, 200, -10000, textSizeRel, Form("%s/LGCorr_Noise.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true, "Local Noise triggered");
  PlotSimple2D( canvas2DCorr, hspectraEnergyvsCellID, -10000, -10000, textSizeRel, Form("%s/EnergyVsCellID.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  PlotSimple2D( canvas2DCorr, hspectraEnergyTotvsNCells, -10000, -10000, textSizeRel, Form("%s/EnergyTotalVsNCells.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, kFALSE, "colz", true);
  
  TCanvas* canvas1DSimple = new TCanvas("canvas1DSimple","",0,0,1450,1300);  // gives the page size
  DefaultCancasSettings( canvas1DSimple, 0.08, 0.03, 0.03, 0.07);
  hspectraEnergyTot->Scale(1./evts);
  hspectraEnergyTot->GetYaxis()->SetTitle("counts/event");
  PlotSimple1D(canvas1DSimple, hspectraEnergyTot, -10000, -10000, textSizeRel, Form("%s/EnergyTot.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, Form("#LT E_{Tot} #GT = %.1f (mip/tile eq.)",hspectraEnergyTot->GetMean() ));
  hspectraNCells->Scale(1./evts);
  hspectraNCells->GetYaxis()->SetTitle("counts/event");
  PlotSimple1D(canvas1DSimple, hspectraNCells, -10000, -10000, textSizeRel, Form("%s/NCells.%s", outputDirPlots.Data(), plotSuffix.Data()), it->second, 1, Form("#LT N_{Cells} #GT = %.1f",hspectraNCells->GetMean() ));
  
  if (ExtPlot > 0){
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
    std::cout << "plotting single layers" << std::endl;

    for (Int_t l = 0; l < setup->GetNMaxLayer()+1; l++){
      for (Int_t m = 0; m < setup->GetNMaxModule()+1; m++){    
        if (l%10 == 0 && l > 0 && debug > 0)
          std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;
        if (ExtPlot > 0){
          PlotNoiseAdvWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                      hSpectra, hSpectraNoise, true, -50, 100, 1.2, l, m,
                                      Form("%s/NoiseTrigg_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          PlotNoiseAdvWithFits8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                      hSpectra, hSpectraNoise, false, -50, 100, 1.2, l, m,
                                      Form("%s/NoiseTrigg_LG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          PlotSpectra8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                    hSpectra, 0, -100, 4000, 1.2, l, m,
                                    Form("%s/Spectra_HG_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          PlotSpectra8MLayer (canvas8Panel,pad8Panel, topRCornerX, topRCornerY, relSize8P, textSizePixel, 
                                    hSpectra, 2, -2, 100, 1.2, l, m,
                                    Form("%s/Spectra_Comb_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          PlotCorrWithFits8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 
                                    hSpectra, 0, -20, 800, 50., l, m,
                                    Form("%s/LGHG_Corr_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
          PlotCorrWithFits8MLayer(canvas8PanelProf,pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 
                                    hSpectra, 2, -100, 340, 300., l, m,
                                    Form("%s/LGLGhgeq_Corr_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, plotSuffix.Data()), it->second);
        }
      }
    }
    std::cout << "done plotting single layers" << std::endl;  
  }
  
  return true;
}


//***********************************************************************************************
//*********************** Save Noise triggers only ***************************************************
//***********************************************************************************************
bool Analyses::SaveNoiseTriggersOnly(void){
  std::cout<<"Save noise triggers into separate file"<<std::endl;
  TcalibIn->GetEntry(0);
  // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }
  
  int evts=TdataIn->GetEntries();
  for(int i=0; i<evts; i++){
    TdataIn->GetEntry(i);
    if (i%5000 == 0 && i > 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      Caen* aTile=(Caen*)event.GetTile(j);
      // testing for noise trigger
      if(aTile->GetLocalTriggerBit()!= (char)2){
        event.RemoveTile(aTile);
        j--;
      }
    }
    RootOutput->cd();
    TdataOut->Fill();
  }
  TdataOut->Write();
  TsetupIn->CloneTree()->Write();
  
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }

  TcalibOut->Fill();
  TcalibOut->Write();
  RootOutput->Close();
  RootInput->Close();      
  
  return true;
}

//***********************************************************************************************
//*********************** Save Noise triggers only ***************************************************
//***********************************************************************************************
bool Analyses::SaveCalibToOutputOnly(void){
  std::cout<<"Save calib into separate file: "<< GetRootCalibOutputName().Data() <<std::endl;
  RootCalibOutput->cd();
  TcalibIn->GetEntry(0);  
  TsetupIn->CloneTree()->Write();
  
  // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }
  
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = GetRootCalibOutputName();
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }

  TcalibOut->Fill();
  TcalibOut->Write();
  RootCalibOutput->Close();
  
  return true;
}


//***********************************************************************************************
//*********************** Save local muon triggers only ***************************************************
//***********************************************************************************************
bool Analyses::SaveMuonTriggersOnly(void){
  std::cout<<"Save local muon triggers into separate file"<<std::endl;
  TcalibIn->GetEntry(0);
    // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }

  int evts=TdataIn->GetEntries();
  for(int i=0; i<evts; i++){
    TdataIn->GetEntry(i);
    if (i%5000 == 0 && i > 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      Caen* aTile=(Caen*)event.GetTile(j);
      // testing for muon trigger
      if(aTile->GetLocalTriggerBit()!= (char)1){
        event.RemoveTile(aTile);
        j--;
      }
    }
    RootOutput->cd();
    TdataOut->Fill();
  }
  TdataOut->Write();
  TsetupIn->CloneTree()->Write();
  
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }

  TcalibOut->Fill();
  TcalibOut->Write();
  RootOutput->Close();
  RootInput->Close();      
  
  return true;
}


//***********************************************************************************************
//*********************** Skim HGCROC data ******************************************************
//***********************************************************************************************
bool Analyses::SkimHGCROCData(void){
  std::cout<<"Skim HGCROC data from pure noise"<<std::endl;
  TcalibIn->GetEntry(0);
    // check whether calib should be overwritten based on external text file
  if (OverWriteCalib){
    calib.ReadCalibFromTextFile(ExternalCalibFile,debug);
  }

  int evts=TdataIn->GetEntries();
  if (maxEvents == -1){
    maxEvents = evts;
  } else {
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    std::cout << "ATTENTION: YOU ARE RESETTING THE MAXIMUM NUMBER OF EVENTS TO BE PROCESSED TO: " << maxEvents << ". THIS SHOULD ONLY BE USED FOR TESTING!" << std::endl;
    std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  }

  long evtTrigg = 0;
  
  for(int i=0; i<evts && i < maxEvents; i++){
    TdataIn->GetEntry(i);
    if (i%5000 == 0 && i > 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    // std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    bool triggered = false;
    for(int j=0; j<event.GetNTiles(); j++){
      Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
      // testing for any signal beyond noise
      // std::cout << "Cell ID:" << aTile->GetCellID() << "\t" << calib.GetPedestalMeanH(aTile->GetCellID()) << "\t" << calib.GetPedestalMeanL(aTile->GetCellID()) << "\n \tADC-wave " ;
      // for (int k = 0; k < (int)aTile->GetADCWaveform().size(); k++ ){
      //   std::cout << aTile->GetADCWaveform().at(k) << "\t" ;
      // }
      // // std::cout << "\n \tTOT-Wave ";
      // // for (int k = 0; k < (int)aTile->GetTOTWaveform().size(); k++ ){
      // //   std::cout << aTile->GetTOTWaveform().at(k) << "\t" ;
      // // }
      // std::cout << "\n \tTOA-Wave ";
      // for (int k = 0; k < (int)aTile->GetTOAWaveform().size(); k++ ){
      //   std::cout << aTile->GetTOAWaveform().at(k) << "\t" ;
      // }
      // std::cout <<"\n\t\t\t";
      // for (int k = 0; k < (int)aTile->GetTOAWaveform().size(); k++ )
      //   std::cout <<"\t";  
      // std::cout << " integ: "<< aTile->GetTOT() << "\t" << aTile->GetTOA() << std::endl;
      
      if (aTile->GetTOA() > 0) triggered= true;
      // if(aTile->GetLocalTriggerBit()!= (char)1){
        // event.RemoveTile(aTile);
        // j--;
      // }
    }
    
    if (!triggered && debug == 3){
      for(int j=0; j<event.GetNTiles(); j++){
        Hgcroc* aTile=(Hgcroc*)event.GetTile(j);
        // testing for any signal beyond noise
        std::cout << "Cell ID:" << aTile->GetCellID() << "\t" << calib.GetPedestalMeanH(aTile->GetCellID()) << "\t" << calib.GetPedestalMeanL(aTile->GetCellID()) << "\n \tADC-wave " ;
        for (int k = 0; k < (int)aTile->GetADCWaveform().size(); k++ ){
          std::cout << aTile->GetADCWaveform().at(k) << "\t" ;
        }
        std::cout << "\n \tTOT-Wave ";
        for (int k = 0; k < (int)aTile->GetTOTWaveform().size(); k++ ){
          std::cout << aTile->GetTOTWaveform().at(k) << "\t" ;
        }
        std::cout << "\n \tTOA-Wave ";
        for (int k = 0; k < (int)aTile->GetTOAWaveform().size(); k++ ){
          std::cout << aTile->GetTOAWaveform().at(k) << "\t" ;
        }
        std::cout <<"\n\t\t\t";
        for (int k = 0; k < (int)aTile->GetTOAWaveform().size(); k++ )
          std::cout <<"\t";  
        std::cout << " integ: "<< aTile->GetTOT() << "\t" << aTile->GetTOA() << std::endl;
      }
    }
    
    RootOutput->cd();
    if (triggered){
      evtTrigg++;
      TdataOut->Fill();
    }
  }
  TdataOut->Write();
  TsetupIn->CloneTree()->Write();
  
  std::cout << "Evts in: " << maxEvents << "\t skimmed: " << evtTrigg << std::endl;
   
  if (IsCalibSaveToFile()){
    TString fileCalibPrint = RootOutputName;
    fileCalibPrint         = fileCalibPrint.ReplaceAll(".root","_calib.txt");
    calib.PrintCalibToFile(fileCalibPrint);
  }

  TcalibOut->Fill();
  TcalibOut->Write();
  RootOutput->Close();
  RootInput->Close();      
  
  return true;
}


//***********************************************************************************************
//*********************** Create output files ***************************************************
//***********************************************************************************************
bool Analyses::CreateOutputRootFile(void){
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
//*********************** Read external bad channel map *****************************************
//***********************************************************************************************
std::map<int,short> Analyses::ReadExternalBadChannelMap(void){
  
  std::cout << "Reading in external mapping file" << std::endl;
  std::map<int,short> bcmap;
  
  std::ifstream bcmapFile;
  bcmapFile.open(ExternalBadChannelMap,std::ios_base::in);
  if (!bcmapFile) {
    std::cout << "ERROR: file " << ExternalBadChannelMap.Data() << " not found!" << std::endl;
    return bcmap;
  }

  for( TString tempLine; tempLine.ReadLine(bcmapFile, kTRUE); ) {
    // check if line should be considered
    if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
      continue;
    }
    if (debug > 1) std::cout << tempLine.Data() << std::endl;

    // Separate the string according to tabulators
    TObjArray *tempArr  = tempLine.Tokenize(" ");
    if(tempArr->GetEntries()<2){
      if (debug > 1) std::cout << "nothing to be done" << std::endl;
      delete tempArr;
      continue;
    } 
    
    int mod     = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
    int layer   = ((TString)((TObjString*)tempArr->At(1))->GetString()).Atoi();
    int row     = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atoi();
    int col     = ((TString)((TObjString*)tempArr->At(3))->GetString()).Atoi();
    short bc    = short(((TString)((TObjString*)tempArr->At(4))->GetString()).Atoi());
    
    int cellID  = setup->GetCellID( row, col, layer, mod);    
                
    if (debug > 1) std::cout << "cellID " << cellID << "\t BC status: " << bc<< std::endl;
    bcmap[cellID]=bc;
  }
  std::cout << "registered " << bcmap.size() << " bad channels!" << std::endl;
  return bcmap;  
  
}
