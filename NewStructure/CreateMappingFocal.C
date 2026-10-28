#include <TROOT.h>
#include <TString.h>
#include <TObjString.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <iostream>
#include <fstream>
#include <utility>

struct FEBInfo
{
  FEBInfo() : FEBNr(0), FEBtype(0), rUnit(0), rHalf(0), modulePosX(0.), modulePosY(0.) {}
  int FEBNr;        // FEB number
  int FEBtype;      // FEB number
  int rUnit;        // readout unit number
  int rHalf;        // readot unit half (0 or 1)
  int orientation;  // orientation of the FEB
  float modulePosX; // module position in X
  float modulePosY; // module position in Y
};

struct unitInfo
{
  unitInfo() : chRU(0), rowInFEB(0), colInFEB(0) {}
  int chRU;     // channel read-out unit
  int rowInFEB; // row within FEB
  int colInFEB; // col within FEB
};

struct channelInfo
{
  channelInfo() : chRU(0), rUnit(0), ruID(0), nrAssembly(0), chAssembly(0), modNr(0), layer(0), rowAssembly(0), colAssembly(0), chID(0), posX(0.), posY(0.), posZ(0.), modposX(0.), modposY(0.) {}
  int chRU;
  int rUnit;
  int ruID;
  int nrAssembly;
  int chAssembly;
  int modNr;
  int layer;
  int rowAssembly;
  int colAssembly;
  int segSize;
  int chID;
  float posX;
  float posY;
  float posZ;
  float modposX;
  float modposY;
};

void PrintChannelInfo(channelInfo tempC)
{
  cout << "RU: " << tempC.rUnit << "\t"
       << "RU channel: " << tempC.chRU << "\t"
       << "RU ID: " << tempC.ruID << "\t"
       << "module Nr: " << tempC.modNr << "\t"
       << "Row in assembly: " << tempC.rowAssembly << "\t"
       << "Col in assembly: " << tempC.colAssembly << "\t"
       << "Layer Nr: " << tempC.layer << "\t"
       << "Channel ID: " << tempC.chID << "\t"
       << "Assembly Nr.: " << tempC.nrAssembly << "\t"
       << "Ch in assembly: " << tempC.chAssembly << "\t"
       << "Segment lenght: " << tempC.segSize << "\t"
       << "X: " << tempC.posX << "cm \t"
       << "Y: " << tempC.posY << "cm \t"
       << "Z: " << tempC.posZ << "cm \t"
       << "Mod X: " << tempC.modposX << "cm \t"
       << "Mod Y: " << tempC.modposY << "cm \t"
       << endl;
  return;
}

//__________________________________________________________________________________________________________
//__________________________________________________________________________________________________________
//__________________________________________________________________________________________________________
Int_t findChannelInUnit(std::vector<unitInfo> lUnit, int chA)
{
  Int_t currEntry = 0;
  Bool_t found = kFALSE;
  while (!found && currEntry < (Int_t)lUnit.size())
  {
    if (lUnit.at(currEntry).chRU == chA)
    {
      found = kTRUE;
    }
    else
    {
      if (currEntry < (Int_t)lUnit.size())
        currEntry++;
    }
  }

  if (currEntry == (Int_t)lUnit.size())
    return -1;
  else
  {
    return lUnit.at(currEntry).chRU;
  }
}

//__________________________________________________________________________________________________________
//__________________________________________________________________________________________________________
//__________________________________________________________________________________________________________
//__________________________________________________________________________________________________________
Int_t ReturnRowBoard(Int_t ch, std::vector<unitInfo> fUnit, Int_t inverted)
{
  Int_t row = -1;
  for (size_t i = 0; i < fUnit.size(); i++){
    if (fUnit.at(i).chRU == ch) {
      row = fUnit.at(i).rowInFEB;
      break;
    }
  }
  if (row == -1)
    return row;
  
  if (inverted > 0){
    return row;
  } else {
    return 3 - row;
  }
}

Int_t ReturnColumnBoard(Int_t ch, std::vector<unitInfo> fUnit, Int_t inverted)
{
  Int_t col = -1;
  for (size_t i = 0; i < fUnit.size(); i++)  {
    if (fUnit.at(i).chRU == ch) {
      col = fUnit.at(i).colInFEB;
      break;
    }
  }
  if (col == -1)
    return col;

  if (inverted > 0) {
    return col;
  } else{
    return 7 - col;
  }
}

float ReturnPosXInLayer(Int_t col)
{
  return 20 / 8 * col + 10 / 8; // in cm raneges from 0 to 20 cm in steps of 2.5 cm
}
float ReturnPosYInLayer(Int_t row)
{
  return 10 / 4 * row + 5 / 4; // in cm ranges from 0 to 10 cm in steps of 2.5 cm
}

float ReturnAbsX(float XInLayer, float ModX)
{
  return XInLayer + ModX;
}

float ReturnAbsY(float YInLayer, float ModY)
{
  return YInLayer + ModY;
}

void CreateMappingFocal(TString filenameUnitMapping0,
                        TString filenameUnitMapping1,
                        TString filenameModulePositions,
                        TString filenameMappingWrite,
                        int debug = 0)
{

  // ********************************************************************************************************
  // read folder and name from file
  // ********************************************************************************************************
  std::vector<FEBInfo> febs;
  std::vector<unitInfo> uChannels[2];
  ifstream inUnit0;
  inUnit0.open(filenameUnitMapping0, ios_base::in);
  if (!inUnit0)
  {
    std::cout << "ERROR: file " << filenameUnitMapping0.Data() << " not found!" << std::endl;
    return;
  }

  ifstream inUnit1;
  inUnit1.open(filenameUnitMapping1, ios_base::in);
  if (!inUnit1)
  {
    std::cout << "ERROR: file " << filenameUnitMapping1.Data() << " not found!" << std::endl;
    return;
  }

  ifstream inModulePositions;
  inModulePositions.open(filenameModulePositions, ios_base::in);
  if (!inModulePositions)
  {
    std::cout << "ERROR: file " << filenameModulePositions.Data() << " not found!" << std::endl;
    return;
  }

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++
  // mapping FEB 0
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++
  for (TString tempLine; tempLine.ReadLine(inUnit0, kTRUE);)
  {
    // check if line should be considered
    if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#"))
    {
      continue;
    }
    if (debug > 1)
      std::cout << tempLine.Data() << std::endl;

    // Separate the string according to tabulators
    TObjArray *tempArr = tempLine.Tokenize("\t");
    if (tempArr->GetEntries() < 1)
    {
      if (debug > 1)
        std::cout << "nothing to be done" << std::endl;
      delete tempArr;
      continue;
    }
    else if (tempArr->GetEntries() == 1)
    {
      // Separate the string according to space
      tempArr = tempLine.Tokenize(" ");
      if (tempArr->GetEntries() < 1)
      {
        if (debug > 1)
          std::cout << "nothing to be done" << std::endl;
        delete tempArr;
        continue;
      }
      else if (tempArr->GetEntries() == 1)
      {
        if (debug > 1)
          std::cout << ((TString)((TObjString *)tempArr->At(0))->GetString()).Data() << " has not been reset, no value given!" << std::endl;
        delete tempArr;
        continue;
      }
    }

    // Put them to the correct variables
    unitInfo tempUCh;
    tempUCh.chRU = ((TString)((TObjString *)tempArr->At(0))->GetString()).Atoi();
    tempUCh.rowInFEB = ((TString)((TObjString *)tempArr->At(1))->GetString()).Atoi();
    tempUCh.colInFEB = ((TString)((TObjString *)tempArr->At(2))->GetString()).Atoi();

    if (debug > 0)
      std::cout << "channel readout unit 0: " << tempUCh.chRU << "\t row in FEB: " << tempUCh.rowInFEB << "\t column in FEB: " << tempUCh.colInFEB << std::endl;
    uChannels[0].push_back(tempUCh);
  }
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++
  // mapping FEB 1
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++
  for (TString tempLine; tempLine.ReadLine(inUnit1, kTRUE);)
  {
    // check if line should be considered
    if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#"))
    {
      continue;
    }
    if (debug > 1)
      std::cout << tempLine.Data() << std::endl;

    // Separate the string according to tabulators
    TObjArray *tempArr = tempLine.Tokenize("\t");
    if (tempArr->GetEntries() < 1)
    {
      if (debug > 1)
        std::cout << "nothing to be done" << std::endl;
      delete tempArr;
      continue;
    }
    else if (tempArr->GetEntries() == 1)
    {
      // Separate the string according to space
      tempArr = tempLine.Tokenize(" ");
      if (tempArr->GetEntries() < 1)
      {
        if (debug > 1)
          std::cout << "nothing to be done" << std::endl;
        delete tempArr;
        continue;
      }
      else if (tempArr->GetEntries() == 1)
      {
        if (debug > 1)
          std::cout << ((TString)((TObjString *)tempArr->At(0))->GetString()).Data() << " has not been reset, no value given!" << std::endl;
        delete tempArr;
        continue;
      }
    }

    // Put them to the correct variables
    unitInfo tempUCh;
    tempUCh.chRU = ((TString)((TObjString *)tempArr->At(0))->GetString()).Atoi();
    tempUCh.rowInFEB = ((TString)((TObjString *)tempArr->At(1))->GetString()).Atoi();
    ;
    tempUCh.colInFEB = ((TString)((TObjString *)tempArr->At(2))->GetString()).Atoi();

    if (debug > 0)
      std::cout << "channel readout unit 1: " << tempUCh.chRU << "\t row in FEB: " << tempUCh.rowInFEB << "\t column in FEB: " << tempUCh.colInFEB << std::endl;
    uChannels[1].push_back(tempUCh);
  }

  // ++++++++++++++++++++++++++++++++++++++++++++++++++++
  // assigning FEBS/Modules to read-out units
  // ++++++++++++++++++++++++++++++++++++++++++++++++++++
  for (TString tempLine; tempLine.ReadLine(inModulePositions, kTRUE);)
  {
    if (tempLine.BeginsWith("%") || tempLine.BeginsWith("#"))
    {
      continue;
    }
    TObjArray *tempArr = tempLine.Tokenize("\t");
    if (tempArr->GetEntries() < 1)
    {
      if (debug > 1)
        std::cout << "nothing to be done" << std::endl;
      delete tempArr;
      continue;
    }
    else if (tempArr->GetEntries() == 1)
    {
      // Separate the string according to space
      tempArr = tempLine.Tokenize(" ");
      if (tempArr->GetEntries() < 1)
      {
        if (debug > 1)
          std::cout << "nothing to be done" << std::endl;
        delete tempArr;
        continue;
      }
      else if (tempArr->GetEntries() == 1)
      {
        if (debug > 1)
          std::cout << ((TString)((TObjString *)tempArr->At(0))->GetString()).Data() << " has not been reset, no value given!" << std::endl;
        delete tempArr;
        continue;
      }
    }

    // Put them to the correct variables
    FEBInfo tempFebI;
    tempFebI.FEBNr = ((TString)((TObjString *)tempArr->At(0))->GetString()).Atoi();
    tempFebI.FEBtype = ((TString)((TObjString *)tempArr->At(1))->GetString()).Atoi();
    tempFebI.rUnit = ((TString)((TObjString *)tempArr->At(2))->GetString()).Atoi();
    tempFebI.rHalf = ((TString)((TObjString *)tempArr->At(3))->GetString()).Atoi();
    tempFebI.modulePosX = ((TString)((TObjString *)tempArr->At(4))->GetString()).Atof();
    tempFebI.modulePosY = ((TString)((TObjString *)tempArr->At(5))->GetString()).Atof();
    tempFebI.orientation = ((TString)((TObjString *)tempArr->At(6))->GetString()).Atoi();
    febs.push_back(tempFebI);
  }

  std::vector<channelInfo> channels;
  fstream fileMappingClassic(filenameMappingWrite.Data(), ios::out);
  // fileMappingClassic.open(filenameMappingWrite, ios::out);
  fileMappingClassic << "sumOpt\t" << 0 << "\n";
  fileMappingClassic << "detector\t" << "FoCal-H" << "\n";
  fileMappingClassic << "#HGCROC board	HGCROC Ch	layer	assembly	board channel	row	column modNr modX modY segSize\n";
  TFile *outputRootFile = new TFile("mappingTree.root", "RECREATE");
  TTree *mapping_tree = new TTree("mapping_tree", "mapping_tree");
  mapping_tree->SetDirectory(outputRootFile);
  channelInfo tempChannel;

  mapping_tree->Branch("channel", &tempChannel, "ch_ru/I:ru/I:ruID/I:nr_ass/I:ch_ass/I:modNr/I:layer/I:row_ass/I:col_ass/I:chID/I:posX/F:posY/F:posZ/F:modposX/F:modposY/F:segSize/I");

  for (int f = 0; f < (int)febs.size(); f++){
    std::cout << "===============================================" << std::endl;
    std::cout << "FEB: " <<  "\t" << febs.at(f).FEBNr << "\t" << febs.at(f).FEBtype  << "\t" <<
    febs.at(f).orientation<<std::endl;
    // std::cout <<  "FEB ch" << "\t" << "ROunit" << "\t"<< "RO ch" << "\t"<< "row" << "\t"<< "col" << std::endl;
      
    for (int chFEB = 0; chFEB < 36; chFEB++){
      
      tempChannel.chRU       = chFEB + febs.at(f).rHalf * 36;
      tempChannel.rUnit      = febs.at(f).rUnit;
      tempChannel.ruID       = Int_t(tempChannel.rUnit << 9) + tempChannel.chRU; // 8 bit read-out unit number, 9 bit channel readout unit
      tempChannel.modNr      = febs.at(f).FEBNr;
      tempChannel.layer      = 0;
      tempChannel.nrAssembly = 0;
      tempChannel.chAssembly = 0;
      tempChannel.rowAssembly = ReturnRowBoard(chFEB, uChannels[febs.at(f).FEBtype], febs.at(f).orientation); 
      tempChannel.colAssembly = ReturnColumnBoard(chFEB, uChannels[febs.at(f).FEBtype], febs.at(f).orientation);

      // 11 bit module Nr., 2 bit row in assembly, 3 bit column in assembly
      tempChannel.chID    = Int_t(tempChannel.modNr << 5) + Int_t(tempChannel.rowAssembly << 3) + Int_t(tempChannel.colAssembly) ;
      tempChannel.posX    = ReturnPosXInLayer(tempChannel.colAssembly);
      tempChannel.posY    = ReturnPosYInLayer(tempChannel.rowAssembly);
      tempChannel.posZ    = 0;
      tempChannel.modposX = febs.at(f).modulePosX;
      tempChannel.modposY = febs.at(f).modulePosY;
      tempChannel.segSize = 1;
      channels.push_back(tempChannel);

      if (tempChannel.rowAssembly != -1 && tempChannel.colAssembly != -1){
        // std::cout <<  chFEB << "\t" << tempChannel.rUnit << "\t"<< tempChannel.chRU << "\t"<< tempChannel.rowAssembly << "\t"<< tempChannel.colAssembly << std::endl;
        fileMappingClassic 
          << tempChannel.rUnit << "\t" 
          << tempChannel.chRU << "\t" 
          << tempChannel.layer << "\t" 
          << 0 << "\t" 
          << chFEB << "\t" 
          << tempChannel.rowAssembly <<  "\t" 
          << tempChannel.colAssembly << "\t" 
          << tempChannel.modNr << "\t" 
          << tempChannel.modposX << "\t" 
          << tempChannel.modposY << "\t" 
          << tempChannel.segSize <<  "\n";
        PrintChannelInfo(tempChannel);
        mapping_tree->Fill();
      }
    }
  }

  fileMappingClassic.close();
  mapping_tree->Print();
  // mapping_tree->Write();
  outputRootFile->Write();
  outputRootFile->Close();
  return;
}
