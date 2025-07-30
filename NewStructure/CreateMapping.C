#include <TROOT.h>
#include <TString.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <fstream>

struct layerInfo{
    layerInfo(): layerNrAbs(0), layerLabel(""), rUnit(0), layerUnit(0), moduleNr(0){}
    int layerNrAbs;       // absolute layer number
    TString layerLabel;   // assembly name
    int rUnit;            // readout unit number
    int layerUnit;        // layer within readout unit
    int moduleNr;
} ;

struct unitInfo{
    unitInfo(): chRU(0), layerUnit(0), chAssembly(0){}
    int chRU;         //channel read-out unit
    int layerUnit;    // layer within readout unit
    int chAssembly;   // channel within one assembly
} ;

struct channelInfo{
    channelInfo(): chRU(0), rUnit(0), ruID(0), nrAssembly(0), chAssembly(0), modNr(0), layer(0), rowAssembly(0), colAssembly(0), chID(0), posX(0.), posY(0.), posZ(0.){}
    int chRU;
    int rUnit;
    int ruID;
    int nrAssembly;
    int chAssembly;
    int modNr;
    int layer;  
    int rowAssembly;
    int colAssembly;
    int chID;
    float posX;
    float posY;
    float posZ;
};


void PrintChannelInfo(channelInfo tempC){
  cout  << "RU: " <<tempC.rUnit << "\t"
        << "RU channel: "<< tempC.chRU  << "\t"
        << "RU ID: "<< tempC.ruID  << "\t"
        << "module Nr: "<< tempC.modNr  << "\t"
        << "Row in assembly: "<< tempC.rowAssembly  << "\t"
        << "Col in assembly: "<< tempC.colAssembly  << "\t"
        << "Layer Nr: "<< tempC.layer  << "\t"
        << "Channel ID"<< tempC.chID  << "\t"
        << "Assembly Nr.: "<< tempC.nrAssembly  << "\t"
        << "Ch in assembly: "<< tempC.chAssembly  << "\t"
        << "X: " << tempC.posX  << "cm \t"
        << "Y: " << tempC.posY  << "cm \t"
        << "Z: " << tempC.posZ  << "cm \t"
        << endl;
   return;                                
}
          
//__________________________________________________________________________________________________________
//__________________________________________________________________________________________________________
//__________________________________________________________________________________________________________
Int_t findChannelInUnit(std::vector<unitInfo> lUnit, int layer, int chA ){
    Int_t currEntry = 0;
    // cout << "searching for: " << layer << "\t" << chA << endl;
    // while (lUnit.at(currEntry).layerUnit != layer && lUnit.at(currEntry).chAssembly != chA &&  currEntry < (Int_t)lUnit.size()){
    //   cout << "-> "<< lUnit.at(currEntry).chAssembly << "\t" << lUnit.at(currEntry).layerUnit << endl;
    //   currEntry++;
    // }
    Bool_t found = kFALSE;
    while (!found && currEntry < (Int_t)lUnit.size()){
      if (lUnit.at(currEntry).layerUnit == layer && lUnit.at(currEntry).chAssembly == chA){
        // cout << "-> "<< lUnit.at(currEntry).layerUnit<< "\t" << lUnit.at(currEntry).chAssembly  << endl;
        found = kTRUE;
      } else {
        // cout << currEntry << "/" << lUnit.size() << endl;
        if (currEntry < (Int_t)lUnit.size()) currEntry++;
      }
    }
    
    if (currEntry == (Int_t)lUnit.size()) return -1;
    else{
      // cout << "correct: "<< lUnit.at(currEntry).layerUnit<< "\t" << lUnit.at(currEntry).chAssembly  << endl;
      return lUnit.at(currEntry).chRU;
    }
}


//__________________________________________________________________________________________________________
//__________________________________________________________________________________________________________
//__________________________________________________________________________________________________________
Int_t ReturnRowBoard(Int_t ch){
    if (ch < 5) return 1;
    else return 0;
}
Int_t ReturnColumnBoard(Int_t ch){
    if (ch == 1 || ch == 8) return 0;
    if (ch == 2 || ch == 7) return 1;
    if (ch == 3 || ch == 6) return 2;
    if (ch == 4 || ch == 5) return 3;
    return -1;
}
float ReturnPosXInLayer(Int_t ch){
  switch(ch){
    case 1: 
    case 8: 
      return -7.5; // in cm
      break;
    case 2: 
    case 7: 
      return -2.5; // in cm
      break;
    case 3: 
    case 6: 
      return 2.5; // in cm
      break;
    case 4: 
    case 5: 
      return 7.5; // in cm
      break;
    default: 
      return -10000; // SOMETHING went wrong
      break;
  }
}
float ReturnPosYInLayer(Int_t ch){
  switch(ch){
    case 1: 
    case 2: 
    case 3: 
    case 4: 
      return 2.5; // in cm
      break;
    case 5: 
    case 6: 
    case 7: 
    case 8: 
      return -2.5; // in cm
      break;
    default:
      return -10000; // SOMETHING went wrong
      break;    
  }
}
float ReturnPosZAbs(Int_t layer){
 return (layer+1)*2.0;
}




void CreateMapping(   TString filenameUnitMapping,
                      TString filenameLayerMapping,
                      TString filenameMappingWrite,
                      int readout = 0,                // 0 - CAEN readout, 1 - HGCROC readout
                      int debug = 0
                      
                  ){

    // ********************************************************************************************************    
    // read folder and name from file
    // ********************************************************************************************************
    std::vector<layerInfo> layers;
    std::vector<unitInfo> uChannels;
    ifstream inUnit;
    inUnit.open(filenameUnitMapping,ios_base::in);
    if (!inUnit) {
        std::cout << "ERRlayerUniOR: file " << filenameUnitMapping.Data() << " not found!" << std::endl;
        return;
    }

    ifstream inLayerMap;
    inLayerMap.open(filenameLayerMapping,ios_base::in);
    if (!inLayerMap) {
        std::cout << "ERROR: file " << filenameLayerMapping.Data() << " not found!" << std::endl;
        return;
    }

    
    
    for( TString tempLine; tempLine.ReadLine(inUnit, kTRUE); ) {
        // check if line should be considered
        if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
            continue;
        }
        if (debug > 1) std::cout << tempLine.Data() << std::endl;

        // Separate the string according to tabulators
        TObjArray *tempArr  = tempLine.Tokenize("\t");
        if(tempArr->GetEntries()<1){
            if (debug > 1) std::cout << "nothing to be done" << std::endl;
            delete tempArr;
            continue;
        } else if (tempArr->GetEntries() == 1 ){
            // Separate the string according to space
            tempArr       = tempLine.Tokenize(" ");
            if(tempArr->GetEntries()<1){
                if (debug > 1) std::cout << "nothing to be done" << std::endl;
                delete tempArr;
                continue;
            } else if (tempArr->GetEntries() == 1  ) {
                if (debug > 1) std::cout << ((TString)((TObjString*)tempArr->At(0))->GetString()).Data() << " has not been reset, no value given!" << std::endl;
                delete tempArr;
                continue;
            }
        }

        // Put them to the correct variables    
        unitInfo tempUCh;
        tempUCh.chRU        = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
        tempUCh.layerUnit   = ((TString)((TObjString*)tempArr->At(1))->GetString()).Atoi();;
        tempUCh.chAssembly  = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atoi();
            
        if (debug > 0) std::cout << "channel readout unit " << tempUCh.chRU << "\t layer in unit: " << tempUCh.layerUnit << "\t channel within assembly: "  << tempUCh.chAssembly  << std::endl;
        uChannels.push_back(tempUCh);
    }

    for( TString tempLine; tempLine.ReadLine(inLayerMap, kTRUE); ) {
        // check if line should be considered
        if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#")){
            continue;
        }
        if (debug > 1) std::cout << tempLine.Data() << std::endl;

        // Separate the string according to tabulators
        TObjArray *tempArr  = tempLine.Tokenize("\t");
        if(tempArr->GetEntries()<1){
            if (debug > 1) std::cout << "nothing to be done" << std::endl;
            delete tempArr;
            continue;
        } else if (tempArr->GetEntries() == 1 ){
            // Separate the string according to space
            tempArr       = tempLine.Tokenize(" ");
            if(tempArr->GetEntries()<1){
                if (debug > 1) std::cout << "nothing to be done" << std::endl;
                delete tempArr;
                continue;
            } else if (tempArr->GetEntries() == 1  ) {
                if (debug > 1) std::cout << ((TString)((TObjString*)tempArr->At(0))->GetString()).Data() << " has not been reset, no value given!" << std::endl;
                delete tempArr;
                continue;
            }
        }

        // Put them to the correct variables    
        layerInfo tempLayer;
        tempLayer.layerNrAbs    = ((TString)((TObjString*)tempArr->At(0))->GetString()).Atoi();
        tempLayer.layerLabel  = (TString)((TObjString*)tempArr->At(1))->GetString();
        tempLayer.rUnit   = ((TString)((TObjString*)tempArr->At(2))->GetString()).Atoi();
        tempLayer.layerUnit      = ((TString)((TObjString*)tempArr->At(3))->GetString()).Atoi();
        tempLayer.moduleNr       = ((TString)((TObjString*)tempArr->At(4))->GetString()).Atoi();
        std::cerr << "Module Number: " << tempLayer.moduleNr<< std::endl;
        if (debug > 0) std::cout << "layer " << tempLayer.layerNrAbs << "\t assembly name: " << tempLayer.layerLabel << "\t CAEN unit Nr.: "  << tempLayer.rUnit << "\t layer in unit: " << tempLayer.layerUnit << std::endl;
        layers.push_back(tempLayer);
    }
    
    std::vector<channelInfo> channels;
    fstream fileMappingClassic(filenameMappingWrite.Data(), ios::out);
    // fileMappingClassic.open(filenameMappingWrite, ios::out);
    if (readout ==0 )fileMappingClassic << "#CAEN board	CAEN Ch	layer	assembly	board channel	row	column modNr\n" ;
    if (readout ==1 )fileMappingClassic << "#HGCROC board	HGCROC Ch	layer	assembly	board channel	row	column modNr\n" ;
    TFile* outputRootFile       = new TFile("mappingTree.root","RECREATE");
    TTree* mapping_tree           = new TTree("mapping_tree", "mapping_tree");
    mapping_tree->SetDirectory(outputRootFile);
    channelInfo tempChannel;
    
    mapping_tree->Branch("channel", &tempChannel, "ch_ru/I:ru/I:ruID/I:nr_ass/I:ch_ass/I:modNr/I:layer/I:row_ass/I:col_ass/I:chID/I:posX/F:posY/F:posZ/F");
    
    for (int l = 0; l < (int)layers.size();l++){
      for (int chA = 1; chA < 9; chA++){
          Int_t channel = findChannelInUnit(uChannels, layers.at(l).layerUnit, chA);
          
          tempChannel.chRU        = channel;
          tempChannel.rUnit       = layers.at(l).rUnit;
          tempChannel.ruID        = Int_t(tempChannel.rUnit<<8)+tempChannel.chRU;   // 8 bit read-out unit number, 8 bit channel readout unit
          tempChannel.modNr       = layers.at(l).moduleNr;
          tempChannel.layer       = layers.at(l).layerNrAbs;
          tempChannel.nrAssembly  = (int)(((TString)layers.at(l).layerLabel.ReplaceAll("C","")).Atoi());
          tempChannel.chAssembly  = chA;
          tempChannel.rowAssembly = ReturnRowBoard(chA);
          tempChannel.colAssembly = ReturnColumnBoard(chA);
                                    // 11 bit module Nr., 1 bit row in assembly, 2 bit column in assembly, 6 bit layer in stack
          tempChannel.chID        = Int_t(tempChannel.modNr<<9)+Int_t(tempChannel.rowAssembly<<8)+Int_t(tempChannel.colAssembly<<6)+tempChannel.layer;
          tempChannel.posX        = ReturnPosXInLayer(chA);
          tempChannel.posY        = ReturnPosYInLayer(chA);
          tempChannel.posZ        = ReturnPosZAbs(layers.at(l).layerNrAbs);
          channels.push_back(tempChannel);

          fileMappingClassic << layers.at(l).rUnit << "\t" << channel << "\t" << layers.at(l).layerNrAbs << "\t" << layers.at(l).layerLabel << "\t" <<  chA << "\t" << ReturnRowBoard(chA) << "\t" << ReturnColumnBoard(chA) << "\t" << tempChannel.modNr <<"\n";
          PrintChannelInfo(tempChannel);
          
          // printf("%b\t%b\t%b\t%b\t%b\n", tempChannel.modNr, tempChannel.rowAssembly, tempChannel.colAssembly, tempChannel.layer, tempChannel.chID);
          mapping_tree->Fill();
      }
    }

    
    fileMappingClassic.close();
    mapping_tree->Print();
    // mapping_tree->Write();
    outputRootFile->Write();
    outputRootFile->Close();
  return;
}
