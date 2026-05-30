#pragma once
#ifndef COMMONHELPERFUNCTIONS_H
#define COMMONHELPERFUNCTIONS_H

  #include <iostream>
  #include <fstream>
  #include "TString.h"
  #include "TObjString.h"
  #include <vector>
  #include <map>
  #include <utility>

  #include <cstdint>
  #include <string>
  
  struct Layer{
    Layer(): nCells(0), energy(0.), avX(0.), avY(0.) {}
    double nCells;
    double energy;
    double avX;
    double avY;
  } ;

  inline int GetMaxLayer(std::map<int,Layer> layers){
    int maxLayer      = -1;
    double maxELayer  = 0;
    std::map<int, Layer>::iterator ithLayer;
    for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
      if (maxELayer < ithLayer->second.energy ){
        maxELayer = ithLayer->second.energy;
        maxLayer  = ithLayer->first;
      }
    }
    return maxLayer;
  }
  inline double GetAverageLayer(std::map<int,Layer> layers){
    double avLayer    = 0;
    double totE  = 0;
    std::map<int, Layer>::iterator ithLayer;
    for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
      avLayer +=   ithLayer->first*ithLayer->second.energy;
      totE    += ithLayer->second.energy;
    }
    avLayer = avLayer/totE;
    return avLayer;
  }
  
  inline double GetXAverage(std::map<int,Layer> layers, int layerMax = -100){
    double avLayer    = 0;
    double totE  = 0;
    std::map<int, Layer>::iterator ithLayer;
    for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
      if ((layerMax != -100) && (ithLayer->first > layerMax) )
        continue;
      avLayer += ithLayer->second.avX*ithLayer->second.energy;
      totE    += ithLayer->second.energy;
    }
    avLayer = avLayer/totE;
    return avLayer;
  }

  inline double GetYAverage(std::map<int,Layer> layers, int layerMax = -100){
    double avLayer    = 0;
    double totE  = 0;
    std::map<int, Layer>::iterator ithLayer;
    for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
      if ( (layerMax !=-100) && (ithLayer->first > layerMax) )
        continue;
      avLayer += ithLayer->second.avY*ithLayer->second.energy;
      totE    += ithLayer->second.energy;
    }
    avLayer = avLayer/totE;
    return avLayer;
  }
  
  struct RunInfo{
    RunInfo(): runNr(0), species(""), pdg(0), energy(0), vop(0), vbr(0), lgSet(0), hgSet(0), posX(0), posY(0), shapetime(0), assemblyNr(0), year(-1), month(-1), readout(""), facility(""), beamline(""), samples(0), trigDelay(0), trigDead(0), phase(0), nFPGA(0), nASIC(0), rf(12), cf(10), cc(12), cfcomp(10), temp(20.), injMode(0), injDAC(0) {}
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
    int rf;
    int cf;
    int cc; 
    int cfcomp;
    float temp;
    int injMode;  // 0: low, 1: high, 2:2.5V
    int injDAC;
  } ;

  TString GetStringFromRunInfo(RunInfo, Int_t);

  //__________________________________________________________________________________________________________
  //__________________ Read run infos from text file _________________________________________________________
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
      
      // normal run 
      if (beamline.CompareTo("injection") != 0){
        tempRun.runNr    = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
        tempRun.species  =  (TString)((TObjString*)tempArr->At(1))->GetString();
        tempRun.pdg      = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atoi();
        tempRun.energy   = ((TString)((TObjString*)tempArr->At(3))->GetString()).Atof();
        tempRun.vop      = ((TString)((TObjString*)tempArr->At(4))->GetString()).Atof();
        tempRun.vbr      = ((TString)((TObjString*)tempArr->At(5))->GetString()).Atof();
        tempRun.temp      = -1.; // set default (might be overwritten)
        
        if (readout.CompareTo("CAEN") == 0){
          tempRun.hgSet    = ((TString)((TObjString*)tempArr->At(6))->GetString()).Atoi();
          tempRun.lgSet    = ((TString)((TObjString*)tempArr->At(7))->GetString()).Atoi();
          if (tempArr->GetEntries() > 10 & tempRun.year < 2024){
            tempRun.shapetime = ((TString)((TObjString*)tempArr->At(10))->GetString()).Atof();
          } else {
            tempRun.temp      = ((TString)((TObjString*)tempArr->At(10))->GetString()).Atof();
            tempRun.shapetime = ((TString)((TObjString*)tempArr->At(11))->GetString()).Atof();
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
        
        if (tempRun.year > 2025){
          tempRun.temp      = ((TString)((TObjString*)tempArr->At(14))->GetString()).Atof();
          tempRun.rf        = ((TString)((TObjString*)tempArr->At(15))->GetString()).Atoi();
          tempRun.cf        = ((TString)((TObjString*)tempArr->At(16))->GetString()).Atoi();
          tempRun.cc        = ((TString)((TObjString*)tempArr->At(17))->GetString()).Atoi();
          tempRun.cfcomp    = ((TString)((TObjString*)tempArr->At(18))->GetString()).Atoi();
        } else {
          tempRun.rf        = 12;
          tempRun.cf        = 10;
          tempRun.cc        = 12;
          tempRun.cfcomp    = 10;
        }
        
        tempRun.injMode   = -1;
        tempRun.injDAC    = -1;
       
        if (debug > 1) std::cout << "Run " << tempRun.runNr << "\t species: " << tempRun.species << "\t energy: "  << tempRun.energy << "\t Vop: " << tempRun.vop << "\t Vov: " << tempRun.vop-tempRun.vbr << "\t Xbeam: " << tempRun.posX<< "\t Ybeam: " << tempRun.posY << "\t shaping time: " << tempRun.shapetime << std::endl;
  
      // injection tests
      } else {
        tempRun.runNr    = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
        tempRun.species  =  (TString)((TObjString*)tempArr->At(1))->GetString();
        tempRun.pdg      = 0;
        tempRun.energy   = 0.;
        tempRun.vop      = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atof();
        tempRun.vbr      = ((TString)((TObjString*)tempArr->At(3))->GetString()).Atof();
        tempRun.posX     = 0.;
        tempRun.posY     = 0.;
        tempRun.samples   = ((TString)((TObjString*)tempArr->At(4))->GetString()).Atoi();
        tempRun.trigDelay = 0;
        tempRun.trigDead  = ((TString)((TObjString*)tempArr->At(5))->GetString()).Atoi();
        tempRun.phase     = ((TString)((TObjString*)tempArr->At(6))->GetString()).Atoi();
        tempRun.nFPGA     = ((TString)((TObjString*)tempArr->At(7))->GetString()).Atoi();
        tempRun.nASIC     = ((TString)((TObjString*)tempArr->At(8))->GetString()).Atoi();
        tempRun.rf        = ((TString)((TObjString*)tempArr->At(9))->GetString()).Atoi();
        tempRun.cf        = ((TString)((TObjString*)tempArr->At(10))->GetString()).Atoi();
        tempRun.cc        = ((TString)((TObjString*)tempArr->At(11))->GetString()).Atoi();
        tempRun.cfcomp    = ((TString)((TObjString*)tempArr->At(12))->GetString()).Atoi();
        tempRun.temp      = 20.;
        tempRun.injMode   = ((TString)((TObjString*)tempArr->At(13))->GetString()).Atoi();
        tempRun.injDAC    = ((TString)((TObjString*)tempArr->At(14))->GetString()).Atoi();
        if (debug > 1) std::cout << "Run " << tempRun.runNr << "\t type: " << tempRun.species << "\t Vop: " << tempRun.vop << "\t Vov: " << tempRun.vop-tempRun.vbr << "\t RF: " << tempRun.rf <<"\t CF: " << tempRun.cf<< "\t CC: " << tempRun.cc<< "\t CFcomp: "<< tempRun.cfcomp<< "\t inj mode: " << tempRun.injMode<< "\t inj value: "<< tempRun.injDAC<< std::endl;
      }
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

  inline TString GetSpeciesStringFromPDG(int pdg){
    switch (pdg){
      case 0:
        return "ped";
      case 11:
        return "e^{-}";
      case -11:
        return "e^{+}";
      case 13:
        return "#mu^{-}";
      case -13:
        return "#mu^{+}";
      case 211:
        return "#pi^{+}";
      case -211:
        return "#pi^{-}";
      case 2212:
        return "p";
      case -2212:
        return "#bar{p}";
      default: 
        return "";
    }
  }
  
  
  inline Double_t ReturnMipPlotRangeDepVov(double Vov, bool isHG, ReadOut::Type type){
    if (type == ReadOut::Type::Caen){
      if (isHG){
        if (Vov < 2)
          return 550.;
        else if (Vov < 3)
          return 750.;
        else if (Vov < 4)
          return 950.;
        else if (Vov < 5)
          return 1150.;
        else
          return 1350.;
      } else {
        if (Vov < 2)
          return 85.;
        else if (Vov < 3)
          return 105.;
        else if (Vov < 4)
          return 125.;
        else if (Vov < 5)
          return 145.;
        else
          return 165.;      
      }
    } else {
      return 250.;
    }
  }

  inline Double_t ReturnMipMinPlotRangeDepVov(double Vov, bool isHG, ReadOut::Type type){
    if (type == ReadOut::Type::Caen){
      if (isHG){
        return -100;
      } else {
        return -100;
      }
    } else {
      return -25.;
    }
  }
  
  inline Double_t ReturnRFValue(int rf, int debug = 0){
    if (debug) std::cout << "RF:  " << rf ;
    Double_t rfOhm = 0;
    Double_t ohmBit[4]  = {80., 40., 20., 10.}; // documentation version SiPM H2GCROC3b
    // Double_t ohmBit[4]  = {100., 66.66, 50., 25.}; // documentation version 1_4
    if ((rf - 8) >= 0){
      rfOhm = rfOhm+1/ohmBit[3];
      rf = rf-8;
    }  
    if ((rf - 4) >= 0){
      rfOhm = rfOhm+1/ohmBit[2];
      rf = rf-4;
    }  
    if ((rf - 2) >= 0){
      rfOhm = rfOhm+1/ohmBit[1];
      rf = rf-2;
    }  
    if ((rf - 1) == 0){
      rfOhm = rfOhm+1/ohmBit[0];
      rf = rf-1;
    }
    if (debug) std::cout << "\t" << rfOhm << " kOhm" << std::endl;
    return 1./rfOhm;
  }
  
  inline Double_t ReturnCFValue(int cf, int debug = 0){
    if (debug) std::cout << "CF:  " << cf ;
    Double_t cffF = 0;
    if ((cf - 8) >= 0){
      cffF = cffF+400;
      cf = cf-8;
    }  
    if ((cf - 4) >= 0){
      cffF = cffF+200;
      cf = cf-4;
    }  
    if ((cf - 2) >= 0){
      cffF = cffF+100;
      cf = cf-2;
    }  
    if ((cf - 1) == 0){
      cffF = cffF+50;
      cf = cf-1;
    }
    if (debug) std::cout << "\t" << cffF << " fF" << std::endl;
    return cffF;
  }
  
  inline Double_t ReturnCFCompValue(int cf, int debug = 0){
    if (debug) std::cout << "CFComp:  " << cf ;
    Double_t cffF = 0;
    if ((cf - 8) >= 0){
      cffF = cffF+400;
      cf = cf-8;
    }  
    if ((cf - 4) >= 0){
      cffF = cffF+200;
      cf = cf-4;
    }  
    if ((cf - 2) >= 0){
      cffF = cffF+100;
      cf = cf-2;
    }  
    if ((cf - 1) == 0){
      cffF = cffF+50;
      cf = cf-1;
    }
    if (debug) std::cout << "\t" << cffF << " fF" << std::endl;
    return cffF;
  }
  
  inline Double_t ReturnCCValue(int cc, int debug = 0){
    if (debug) std::cout << "CFComp:  " << cc ;
    Double_t ccVal = 0;
    if ((cc - 8) >= 0){
      ccVal = ccVal+0.2;
      cc = cc-8;
    }  
    if ((cc - 4) >= 0){
      ccVal = ccVal+0.1;
      cc = cc-4;
    }  
    if ((cc - 2) >= 0){
      ccVal = ccVal+0.05;
      cc = cc-2;
    }  
    if ((cc - 1) == 0){
      ccVal = ccVal+0.025;
      cc = cc-1;
    }
    if (debug) std::cout << "\t" << ccVal  << std::endl;
    return ccVal;
  }
  
  inline TString GetLabelHGCROCSettings(RunInfo currRunInfo){
    TString label = "";
    if (currRunInfo.rf > -1)
      label = Form("%sRF=%.1fk#Omega ",label.Data() , ReturnRFValue(currRunInfo.rf));
    if (currRunInfo.cf > -1)
      label = Form("%sCF=%.1ffF ",label.Data() , ReturnCFValue(currRunInfo.cf));
    if (currRunInfo.cfcomp > -1)
      label = Form("%sCF_{comp}=%.1ffF ",label.Data() , ReturnCFCompValue(currRunInfo.cfcomp));
    if (currRunInfo.cc > -1)
      label = Form("%sCC=%.3f",label.Data() , ReturnCCValue(currRunInfo.cc));
    return label;
  }
  inline TString GetLabelHGCROCSettingsCF(RunInfo currRunInfo){
    TString label = "";
    if (currRunInfo.cf > -1)
      label = Form("%sCF=%.1ffF ",label.Data() , ReturnCFValue(currRunInfo.cf));
    if (currRunInfo.cfcomp > -1)
      label = Form("%sCF_{comp}=%.1ffF",label.Data() , ReturnCFCompValue(currRunInfo.cfcomp));
    return label;
  }
  inline TString GetLabelHGCROCSettingsRFCC(RunInfo currRunInfo){
    TString label = "";
    if (currRunInfo.rf > -1)
      label = Form("%sRF=%.1fk#Omega ",label.Data() , ReturnRFValue(currRunInfo.rf));
    if (currRunInfo.cc > -1)
      label = Form("%sCC=%.3f",label.Data() , ReturnCCValue(currRunInfo.cc));
    return label;
  }
  
  // // Function to generate the CRC-32 lookup table at runtime (or compile time with C++11 constexpr)
  // void generate_crc32_table(uint32_t(&table)[256]) {
  //     uint32_t polynomial = 0x104C11DB7; // Common CRC-32 polynomial
  //     for (uint32_t i = 0; i < 256; i++) {
  //         uint32_t c = i;
  //         for (size_t j = 0; j < 8; j++) {
  //             if (c & 1) {
  //                 c = polynomial ^ (c >> 1);
  //             } else {
  //                 c >>= 1;
  //             }
  //         }
  //         table[i] = c;
  //     }
  // }
  // 
  // // Function to calculate the CRC-32 checksum for a buffer
  // uint32_t calculate_crc32(const uint8_t* data, size_t length) {
  //     uint32_t crc = 0x0;           // Initial value (standard for HGCROC)
  //     // uint32_t crc = 0xFFFFFFFF; // Initial value (standard for CRC-32)
  //     uint32_t table[256];
  //     generate_crc32_table(table);
  // 
  //     for (size_t i = 0; i < length; ++i) {
  //         crc = table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
  //     }
  // 
  //     return crc ^ 0xFFFFFFFF; // Final XOR (standard for CRC-32)
  // }  

  // Usage CRC
  // std::string message = "123456789"; // Standard test string for CRC-32 check value
  // The expected CRC-32 result for this string is 0xCBF43926
  //  uint32_t checksum = calculate_crc32(reinterpret_cast<const uint8_t*>(message.c_str()), message.length());
  //  std::cout << "CRC-32 Checksum: 0x" << std::hex << std::uppercase << checksum << std::endl;

  
#endif
