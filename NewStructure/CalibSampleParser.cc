#include "CalibSampleParser.h"
#include <vector>
#include <algorithm>
#include <sstream>
#include <string>
#include <cstdlib>
#include "TROOT.h"
#include "TPaletteAxis.h"
#include "TProfile.h"
#include "TChain.h"
#include "TileSpectra.h"
#include "PlotHelper.h"
#include "CommonHelperFunctions.h"

#include "waveform_fitting/waveform_fit_base.h"
#include "waveform_fitting/crystal_ball_fit.h"
#include "waveform_fitting/max_sample_fit.h"


// ****************************************************************************
// Checking and opening input files
// ****************************************************************************
bool CalibSampleParser::CheckAndOpenIO(void){

  //Need to check first input to get the setup...I do not think it is necessary
  std::cout<<"Input file set to: '"<<inputFilePath.Data() <<std::endl;
  if(inputFilePath.IsNull()){
    std::cout<<"An input file is required, aborting"<<std::endl;
    return false;
  } 
  if( inputFilePath.Contains(".csv") ){
    if( optTOA ) {
      std::cout << "Reading in .csv file with TOA calibration output" << std::endl;
      if( doPlotting ){
        std::cout << "Plotting not implemented yet" << std::endl;
      }
    } else {
      std::cout << "Reading in .csv file with calibration output, and parsing it into .root file" << std::endl;
    }
  } else if( inputFilePath.Contains(".txt") ){
    std::cout << "Reading in config file with list of dead channels, and .json files with respective KCUs. " << std::endl;
    std::cout << "Additional .root file with calib object is required to run in this mode." << std::endl;
    SwitchPedestalCalib();
    if( doPlotting ) {
      std::cout << "Plotting not available in the current mode, no plots will be produced" << std::endl;
    }
  }

  if( !MapInputName.IsNull() && !optTOA){
    MapInput.open( MapInputName.Data(), std::ios::in);
    if( !MapInput.is_open() ){
      std::cout << "Could not open mapping file: " << MapInputName << std::endl;
      return false;
    }
  } else if( optTOA ){
    std::cout << "Mapping file not required" << std::endl;
  } else {
    std::cout << "A mapping file is required, aborting" << std::endl;
    return false;
  }

  if( !OutputFilename.IsNull()){
    std::cout << "Output root filename set to " << OutputFilename << std::endl;
  } else {
    if( optParse == 1 ){  
      OutputFilename = calibInputFile;
      OutputFilename = OutputFilename.ReplaceAll(".root","overwrittenPedestal.root");
    } else {
      OutputFilename = inputFilePath;
      OutputFilename = OutputFilename.ReplaceAll(".csv",".root");
    }
    std::cout << "Output root filename set by default to " << OutputFilename << std::endl;
  }

  if( !optTOA ){
    OutputHistFilename = OutputFilename;
    OutputHistFilename = OutputHistFilename.ReplaceAll(".root","_hist.root");

    RootOutput    = new TFile(OutputFilename.Data(), "RECREATE");

    if( optParse == 0 ){
      tOutTree      = new TTree("CalibSample","CalibSample");
      tOutTree->Branch("event",&event);
      tOutTree->SetDirectory(RootOutput);
    } else if ( optParse == 1 ){
        std::cout << "Reading a calib object from the file " << calibInputFile.Data() << std::endl;
        RootCalibInput = new TFile( calibInputFile.Data(), "READ" );
        if( RootCalibInput->IsZombie() ){
          std::cout << "Error opening " << calibInputFile.Data() <<", does the file exist?" << std::endl;
          return false;
        }
        TcalibIn  = (TTree*) RootCalibInput->Get("Calib");
        if( !TcalibIn ){
          std::cout << "Could not retrieve Calib, aborting" <<std::endl;
          return false;
        } else {
          int matchingbranch = TcalibIn->SetBranchAddress("calib",&calibptr);
          if( matchingbranch < 0 ){
            std::cout<<"Error retrieving calibration info form the tree"<<std::endl;
            return false;
          }
        }
    }

    TsetupOut     = new TTree("Setup","Setup");
    setup=Setup::GetInstance();
    TsetupOut->Branch("setup",&rsw);
    TsetupOut->SetDirectory( RootOutput );
    TcalibOut     = new TTree("Calib","Calib");
    TcalibOut->Branch("calib",&calib);
    TcalibOut->SetDirectory( RootOutput );
  }

  std::cout <<"=============================================================" << std::endl;
  std::cout <<" Basic setup complete" << std::endl;
  std::cout <<"=============================================================" << std::endl;
  return true;
}


// ****************************************************************************
// Primary process function 
// ****************************************************************************
bool CalibSampleParser::Process(void){
    bool status;
    if( optTOA ){
      status = ParseTOA();
    } else {
      if( optParse == 0 ){
        status = Parse();
        if( doPlotting ) status = ProcessAndPlotWaveforms();
      } else if ( optParse == 1 ) {
        status = ParsePedestalCalib();
      }
    }
    return status;
}


// ****************************************************************************
// Parsing routine
// ****************************************************************************
bool CalibSampleParser::Parse(){

    // initialize run number file
    if (RunListInputName.CompareTo("")== 0) {
        std::cerr << "ERROR: No run list file has been provided, please make sure you do so! Aborting!" << std::endl;
        return false;
    }
    std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);

    // initialize setup
    if (MapInputName.CompareTo("")== 0) {
        std::cerr << "ERROR: No mapping file has been provided, please make sure you do so! Aborting!" << std::endl;
        return false;
    }
    if( debug > 2) std::cout << "Setup max row " << setup->GetNMaxRow() << "\t max col " << setup->GetNMaxColumn() << "\t max layer " << setup->GetNMaxLayer() << "\t max module " << setup->GetNMaxModule() << std::endl;

    std::cout << "Creating mapping " << std::endl;
    setup->Initialize(MapInputName.Data(),debug);

    // initialize a dummy event - it will be a tree with one branch containing one event, event contains multiple tiles
    event.SetRunNumber( RunNr ); // to be fixed?
    event.SetROtype(ReadOut::Type::Hgcroc);
    event.SetBeamName( "dummy" );
    event.SetBeamID( 0 );
    event.SetBeamEnergy( 0 );

    Int_t temp_channel  = -1;
    int cell_id         = -1;
    int counter         = 1;
    int sample_counter  = 1;

    std::vector<Hgcroc>   samples;
    samples.clear();
    Hgcroc tmpTile;

    std::vector<int> temp_adc;
    std::vector<int> temp_toa;
    std::vector<int> temp_tot;
    temp_adc.clear();
    temp_toa.clear();
    temp_tot.clear();
    
    std::ifstream   calibSampleCsv( inputFilePath.Data() );
    std::string line;
    std::getline( calibSampleCsv, line ); // skip first line with dac value...
    std::getline( calibSampleCsv, line ); // skip header
    while (std::getline( calibSampleCsv, line )){
      std::stringstream ss(line);
      std::string token;
      std::vector<std::string> tokens;
      while(std::getline(ss,token,',')){
        tokens.push_back(token); // tokens[0] - channel, tokens[3] - ADC, tokens[4] - TOA, tokens[5] - TOT || tokens[1] - time, tokens[2] - phase - skipping
      }
      int channel = std::stoi(tokens[0]);
      int asic = 0; 

      if(temp_channel == -1) {
        temp_channel = channel;
        if( channel%76 < 37 ){          // correction in the channel number to remove calib channels: 37, 38
          channel = 72*(channel/76) + channel%76;
        } else if( channel%76 == 37 || channel%76 == 38){
          continue;
        } else {
          channel = 72*(channel/76) + (channel-2);
        }
        asic    = (channel/72);
        cell_id  = setup->GetCellID(asic, channel % 72);
        temp_adc.push_back( std::atoi(tokens[3].c_str()) ); 
        temp_toa.push_back( std::atoi(tokens[5].c_str()) );
        temp_tot.push_back( std::atoi(tokens[4].c_str()) );
      } else if( temp_channel == channel){
        sample_counter++;
        if( std::atof( tokens[1].c_str() ) == (sample_counter-1)*1.5625 ) {         // check the timing - some channels are missing entries from waveforms
          temp_adc.push_back( std::atoi(tokens[3].c_str()) ); 
          temp_toa.push_back( std::atoi(tokens[5].c_str()) );
          temp_tot.push_back( std::atoi(tokens[4].c_str()) );
        } else {
          if( debug > 3){
            std::cout << "missing entry, channel: " << channel << "\t time " << std::atof( tokens[1].c_str() ) << "\t target value: " << (sample_counter-1)*1.5625 << std::endl;
          }
          temp_adc.push_back( 0 );                                                  // pushing the missing values as 0
          temp_toa.push_back( 0 );        
          temp_tot.push_back( 0 );
          sample_counter++;                                                         // incrementing the sample counter to account for the missing entry
          temp_adc.push_back( std::atoi(tokens[3].c_str()) );                       // and then the read values
          temp_toa.push_back( std::atoi(tokens[5].c_str()) );
          temp_tot.push_back( std::atoi(tokens[4].c_str()) );
        }
      } else if( temp_channel != channel){
        // save the current tiles waveforms, and then push the tile to the samples vector
        tmpTile.SetNsample(sample_counter);
        tmpTile.SetCellID(cell_id);
        tmpTile.SetE(0);          // need to process waveform to set this
        tmpTile.SetTOA(0);        // need to process waveform to set this
        // TOT - the first non-zero value
        int tempTOT   = *(std::find_if(temp_tot.begin(), temp_tot.end(), [](int n){ return n!=0; }) );
        tmpTile.SetTOT( tempTOT );        // need to process waveform to set this - the first value that comes up 
        // SetIntegratedADC as max ADC 
        int tempIntADC    = *(std::max_element( temp_adc.begin(), temp_adc.end() ));
        tmpTile.SetIntegratedADC( tempIntADC );
        tmpTile.SetADCWaveform( temp_adc );
        tmpTile.SetTOAWaveform( temp_toa );
        tmpTile.SetTOTWaveform( temp_tot );
        samples.push_back( tmpTile );
        if(debug > 1) std::cout << "Channel: " << temp_channel << "\t Cell ID: " << cell_id << "\t NSamples: " << sample_counter << "\t ADC: " << tempIntADC << "\t ToT: " << tempTOT << std::endl;
        
        // we're moving to the next tile
        temp_adc.clear();
        temp_toa.clear();
        temp_tot.clear();
        sample_counter = 1;
        temp_channel = channel; 
        if( channel%76 < 37 ){      // correction in the channel number to remove calib channels: 37, 38
          channel = 72*(channel/76) + channel%76;
        } else if( channel%76 == 37 || channel%76 == 38){
          continue;
        } else {
          channel = 72*(channel/76) + (channel-2);
        }
        asic    = (channel/72);
        cell_id  = setup->GetCellID(asic, channel % 72);       
        if(debug > 1 ) std::cout << "layer " << setup->GetLayer(cell_id) << "\t module " << setup->GetModule(cell_id) << "\t column " << setup->GetColumn(cell_id) << "\t row " << setup->GetRow(cell_id) << std::endl;
        counter++; 
        temp_adc.push_back( std::atoi(tokens[3].c_str()) ); 
        temp_toa.push_back( std::atoi(tokens[4].c_str()) );
        temp_tot.push_back( std::atoi(tokens[5].c_str()) );
      }
    }

    if( cell_id != (*samples.end()).GetCellID() ){
      tmpTile.SetNsample(sample_counter);
      tmpTile.SetCellID(cell_id);
      tmpTile.SetE(0);
      tmpTile.SetTOA(0);
      // TOT - the first non-zero value
      int tempTOT   = *(std::find_if(temp_tot.begin(), temp_tot.end(), [](int n){ return n!=0; }) );
      tmpTile.SetTOT( tempTOT );        // need to process waveform to set this - the first value that comes up 
      
    // SetIntegratedADC as max ADC 
      int tempIntADC    = *(std::max_element( temp_adc.begin(), temp_adc.end() ));
      tmpTile.SetIntegratedADC( tempIntADC );
      tmpTile.SetADCWaveform( temp_adc );
      tmpTile.SetTOAWaveform( temp_toa );
      tmpTile.SetTOTWaveform( temp_tot );
      samples.push_back(tmpTile);
      if(debug > 1) std::cout << "Channel: " << temp_channel << "\t Cell ID: " << cell_id << "\t NSamples: " << sample_counter << "\t ADC: " << tempIntADC << "\t ToT: " << tempTOT << std::endl; 
    }
    if( debug > 1)  std::cout << "Total number of parsed samples: " << counter << std::endl;

    if( debug > 1) std::cout << "Samples saved in the event class: " << std::endl;
    int counter2 = 0; 
    for(auto it = samples.begin(); it!= samples.end(); ++it){
      if( debug > 1 ) {
        std::cout << "Sample: " << counter2 << "\t CellID:" << (*it).GetCellID() << std::endl;
      }
      counter2++;
      event.AddTile( new Hgcroc(*it) );
    }

    // fill the tree and write it out to file - should contain just one event
    RootOutput->cd();

    // setup 
    RootSetupWrapper rswtmp=RootSetupWrapper(setup);
    rsw=rswtmp;
    TsetupOut->Fill();
    TsetupOut->Write();
    // data
    tOutTree->Fill();
    tOutTree->Write();

    // close the file
    RootOutput->Close();

    std::cout <<"=============================================================" << std::endl;
    std::cout <<" Parsing complete" << std::endl;
    std::cout <<" Output saved to " << OutputFilename.Data() << std::endl;
    std::cout <<"=============================================================" << std::endl;


    return true;
}

bool CalibSampleParser::ProcessAndPlotWaveforms(){

  // initialize run number file
  if (RunListInputName.CompareTo("")== 0) {
      std::cerr << "ERROR: No run list file has been provided, please make sure you do so! Aborting!" << std::endl;
      return false;
  }
  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);

  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find( RunNr );

  RootOutputHist  = new TFile(OutputHistFilename.Data(), "RECREATE");
  RootOutputHist->mkdir("IndividualCells");
  RootOutput->cd();

  waveform_fit_base *waveform_builder = nullptr;
  waveform_builder = new max_sample_fit();

  std::map<int,TileSpectra>             hSpectra;
  std::map<int,TileSpectra>::iterator   ithSpectra;


  for(int j=0; j<event.GetNTiles(); j++){
    Hgcroc*   aTile   = (Hgcroc*)event.GetTile(j);
    ithSpectra  = hSpectra.find(aTile->GetCellID());

    double adc  = 0; 
    double tot  = 0; 
    double toa  = 0; 

    // dummy pedestal - just first ADC values from the tile:
    double ped   = (aTile->GetADCWaveform()).at(0);
    if(debug > 3) std::cout << "CellID " << aTile->GetCellID() << "\t (dummy) Pedestal " << ped << std::endl;

    std::vector<int> temp_totWaveform = aTile->GetTOTWaveform();
    std::vector<int> temp_toaWaveform = aTile->GetTOAWaveform();
    std::vector<int> temp_adcWaveform = aTile->GetADCWaveform();

    tot = *(std::find_if( temp_totWaveform.begin(), temp_totWaveform.end(), [](int val){return val>1;} ));  // placeholders for now
    toa = *(std::find_if( temp_toaWaveform.begin(), temp_toaWaveform.end(), [](int val){return val>1;} ));  // placeholders for now
    adc = *(std::max_element( temp_adcWaveform.begin(), temp_adcWaveform.end())) - ped;                     // placeholders for now

    if(debug > 5) std::cout << "tot: " <<  tot << "\t toa " << toa << "\t adc " << adc << std::endl;
 
    waveform_builder->set_waveform( aTile->GetADCWaveform() );
    waveform_builder->fit_with_average_ped(ped);

    aTile->SetIntegratedADC(waveform_builder->get_E());
    aTile->SetPedestal(waveform_builder->get_pedestal());
    
    if(ithSpectra!=hSpectra.end()){
      ithSpectra->second.FillExtHGCROC(adc, 0., tot, 0);
      ithSpectra->second.FillWaveformVsTimeParser(aTile->GetADCWaveform(),ped);
    } else {
      RootOutputHist->cd("IndividualCells");
      hSpectra[aTile->GetCellID()]=TileSpectra("ped",3,aTile->GetCellID(),nullptr,event.GetROtype(),debug);
      hSpectra[aTile->GetCellID()].FillExtHGCROC(adc, 0., tot, 0);
      hSpectra[aTile->GetCellID()].FillWaveformVsTimeParser(aTile->GetADCWaveform(),ped);
      hSpectra[aTile->GetCellID()].WriteExt(false);
      RootOutput->cd();
    }
  }
  
  int t_max = ((Hgcroc*)event.GetTile(0))->GetNsample() * 1562.5;

  Int_t textSizePixel = 30;

  TCanvas* canvas8PanelProf;
  TPad* pad8PanelProf[8];
  Double_t topRCornerXProf[8];
  Double_t topRCornerYProf[8];
  Double_t relSize8PProf[8];
  CreateCanvasAndPadsFor8PannelTBPlot(canvas8PanelProf, pad8PanelProf,  topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 0.045, "Prof", false);

  for(int l=0; l < setup->GetNMaxLayer()+1; l++){
    for(int m=0; m < setup->GetNMaxModule()+1; m++ ){
        if (l%10 == 0 && l > 0 && debug > 0) std::cout << "============================== layer " <<  l << " / " << setup->GetNMaxLayer() << " layers" << std::endl;    
        
          PlotCorr2D8MLayer(canvas8PanelProf, pad8PanelProf, topRCornerXProf, topRCornerYProf, relSize8PProf, textSizePixel, 
                            hSpectra, 1, 0, t_max, 1024, l, m,
                            Form("%s/Waveform_Mod%02d_Layer%02d.%s" ,outputDirPlots.Data(), m, l, "pdf"), it->second, 1);
    }
  }

  std::cout <<"=============================================================" << std::endl;
  std::cout <<" Plots saved to " << outputDirPlots.Data() << std::endl;
  std::cout <<"=============================================================" << std::endl;



  return true;
}

bool CalibSampleParser::ParsePedestalCalib(){

  // initialize setup
  if (MapInputName.CompareTo("")== 0) {
      std::cerr << "ERROR: No mapping file has been provided, please make sure you do so! Aborting!" << std::endl;
      return false;
  }
  if( debug > 2) std::cout << "Setup max row " << setup->GetNMaxRow() << "\t max col " << setup->GetNMaxColumn() << "\t max layer " << setup->GetNMaxLayer() << "\t max module " << setup->GetNMaxModule() << std::endl;
  std::cout << "Creating mapping " << std::endl;
  setup->Initialize(MapInputName.Data(),debug);

  // read in calib object to modify
  TcalibIn->GetEntry(0);
  calib.SetRunNumber( RunNr );

  // reading in config file with calib channels in the first line, list of KCUs # + .json file in following lines
  // example config: config_HGCROCPedestalCalibParser.txt
  //          8,0,37,38,75,76,113,114,151
  //          0,/home/ewa/EIC/DATA/HGCROCData/PedestalRun_PairWithRun051/103_PedestalCalib_pedecalib_20251029_182545.json

  // important arrays in .json file: dead channels - includes the list of calibration files that need to be skipped in the channel numbering
  //                    pede_values - pedestal values
  std::ifstream   jsonList ( inputFilePath.Data() );
  std::string     line;

  std::vector<int>  cellIDs;
  std::string     deadChannelsJson  = "    \"dead_channels\": [";
  std::string     pedestalValJson   = "    \"pede_values\": [";

  int   NChannels = 0;
  std::vector<int>  calibChannelsJson; 
  std::vector<int>  pedestalValues;
  int   channelCounter = 0; 

  // reading in the calib channels from the config file 
  std::getline( jsonList, line );
  std::stringstream         ss(line);
  std::string               token;
  std::vector<std::string>  tokens;
  while( std::getline( ss, token, ',') )    tokens.push_back(token);  
  NChannels   = std::stoi( tokens[0] );     // first comes the number of calib channels
  for(int i = 1; i < tokens.size(); i++){
    calibChannelsSet.push_back( std::stoi( tokens[i] ) );
  }
  if( calibChannelsSet.size() != NChannels ){
    std::cout << "Given number of calib channels " << NChannels << " doesn't match the number of listed calib channels " << calibChannelsSet.size() << std::endl;
    std::cout << "Check config file... Aborting" << std::endl;
    return false;
  }
  tokens.clear();

  // reading in the list of the .json files
  while( std::getline( jsonList, line) ){
    std::stringstream   s2(line);
    tokens.clear();
    while( std::getline( s2, token, ',') )  tokens.push_back( token ); // tokens[0] - KCU number, tokens[1] - path to .json file
    kcu = std::stoi( tokens[0] );

    std::ifstream tmpJson ( tokens[1] );
    while(std::getline( tmpJson, line ) ){

      // first check if the list of calib channels in the .json file matches the one given in the config
      if( line.compare(deadChannelsJson) == 0 ){
        std::cout << "Checking if the calib channels in .json file match the set-up calib channels" << std::endl;
        while( std::getline( tmpJson, line) ){
          if( line.compare("    ],") == 0 ) break;
          else {
            std::stringstream   s4(line); 
            tokens.clear();
            while( std::getline(s4,token,',') )   tokens.push_back(token);    // should only be one, tokens[0]
            calibChannelsJson.push_back( std::stoi( tokens[0] ) );
          }
        }
        if( calibChannelsJson.size() != NChannels ) {
          std::cout << "Unexpected length of the read-in calib channels array... Aborting" << std::endl;
          return false;
        }

        for(int i=0; i< (int)calibChannelsJson.size(); i++){
          if( calibChannelsJson.at(i) != calibChannelsSet.at(i) ){
            std::cout << "Read-in calib channels " << calibChannelsJson.at(i) << " different from known calib channel " << calibChannelsSet.at(i) << std::endl;
            std::cout << "Modify the config file... Aborting" << std::endl;
            return false;
          }
          if( debug > 5) std::cout << "Read-in calib channel " << calibChannelsJson.at(i) << ", set-up calib channel " << calibChannelsSet.at(i) << std::endl;
        }
      }

      // now lets read in the pedestal values
      if( line.compare(pedestalValJson) == 0 ){
        std::cout << "Calib channels checked, now time to read the pedestal values " << std::endl;
        while( std::getline( tmpJson, line) ){
          if( line.compare("    ],") == 0 ) break;
          else {
            std::stringstream   s4(line); 
            tokens.clear();
            while( std::getline(s4,token,',') )   tokens.push_back(token);    // should only be one, tokens[0] - pedestal value
            if( !IsCalibChannel( channelCounter ) ) {
              pedestalValues.push_back( std::stoi( tokens[0] ) );
            }
          }
          channelCounter++;
        }
      }
    }
  }

  // now we are looping over the pedestal values, with the int iterator being the HGCROC channel, to exchange the pedestal values in the calib object
  for(int i=0; i<pedestalValues.size(); i++){
    if( debug > 5)  std::cout << "channel: " << i << ", pedestal value " << pedestalValues.at(i) << std::endl;

    int channel   = i; 
    int asic      = kcu * 2 + (i/72);
    int cell_id   = setup->GetCellID( asic, channel%72 );
    if( cell_id != -1 ) cellIDs.push_back( cell_id );       // skipping not connected channels - those with cell_id = -1
    if( debug > 10) std::cout << "Channel " << i << ", kcu " << kcu << ", asic " << asic << ", cellID " << cell_id << std::endl;

    TileCalib*  tileCalib   = calib.GetTileCalib( cell_id );
    if( debug > 5 ) std::cout << "Setting pedestal for tile " << cell_id << " from " << tileCalib->PedestalMeanH << " to " << pedestalValues.at(i) << std::endl;
    tileCalib->PedestalMeanH = pedestalValues.at(i);
  }

  std::cout << "Pedestal values overwritten, and the calib object will be saved in " << OutputFilename.Data() << std::endl;

  // fill the tree and write it out to file - should contain just one event
  RootOutput->cd();
  
  // setup 
  RootSetupWrapper rswtmp=RootSetupWrapper(setup);
  rsw=rswtmp;
  TsetupOut->Fill();
  TsetupOut->Write();

  // calib
  TString   outputCalibTxt  = OutputFilename.ReplaceAll(".root","_calib.txt");
  calib.PrintCalibToFile( outputCalibTxt );
  TcalibOut->Fill();
  TcalibOut->Write();

  // close the file
  RootOutput->Close();
  std::cout <<"=============================================================" << std::endl;
  std::cout <<" Parsing of pedestal calib complete" << std::endl;
  std::cout <<" Output saved to " << OutputFilename.Data() << std::endl;
  std::cout <<"=============================================================" << std::endl;

  return true;
}


bool CalibSampleParser::ParseTOA(){

  // std::vector<int>  toa_values;
  // toa_values.clear();

  std::ifstream calibSampleCsv(   inputFilePath.Data() );
  std::string line;

  if( lines <= 0 ){
    std::cout << "Number of lines not valid... returning" << std::endl;
    return false;
  }

  while( lines-1 ){
    std::getline( calibSampleCsv,line);
    lines--;
  }

  std::getline( calibSampleCsv, line);      // should be just the last line?
  std::stringstream ss(line);
  std::string token;
  std::vector<int> toa_values;
  while(std::getline(ss,token,',')){
    toa_values.push_back( std::atoi(token.c_str()) );
    if(debug > 5) std::cout << token << "\t";
  }
  std::cout << std::endl;

  int     mod  = 0; 
  int     n_elem  = 0;
  double  sum     = 0; 
  for(int i = 0; i < (int)toa_values.size(); i++){
    if( i/38 == mod ){
      sum+= toa_values.at(i);
      n_elem = (toa_values.at(i)==0) ? n_elem : ++n_elem;
    } else {
      std::cout << "Average TOA from sector " << mod << ": " << 1.*sum/n_elem << std::endl;
      if(debug > 1) std::cout << "Sum " << sum << "\t n_elem " << n_elem << std::endl;
      n_elem=0; sum=0; 
      mod++; 
      sum+= toa_values.at(i);
      n_elem = (toa_values.at(i)==0) ? n_elem : ++n_elem;
    }
  }
  std::cout << "Average TOA from sector " << mod << ": " << 1.*sum/n_elem << std::endl;
  if(debug > 1) std::cout << "Sum " << sum << "\t n_elem " << n_elem << std::endl;
  std::cout << std::endl;


  std::cout <<"=============================================================" << std::endl;
  std::cout <<" Parsing of TOA calib complete" << std::endl;
  std::cout <<"=============================================================" << std::endl;

  
  return true;
}