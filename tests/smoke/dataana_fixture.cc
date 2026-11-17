#include "Caen.h"
#include "Calib.h"
#include "Event.h"
#include "RootSetupWrapper.h"
#include "Setup.h"
#include "TFile.h"
#include "TTree.h"

int main(int argc, char** argv) {
  if (argc != 3) return 2;
  Setup* setup = Setup::GetInstance();
  if (!setup->Initialize(argv[1], 0)) return 3;

  RootSetupWrapper wrappedSetup(setup);
  Event event;
  event.SetRunNumber(42);
  event.SetEventID(1);
  event.SetTimeStamp(1000.);
  event.SetROtype(ReadOut::Type::Caen);
  event.SetVop(44.);
  event.SetVov(3.);
  Caen* tile = new Caen;
  tile->SetCellID(0);
  tile->SetE(1.4);
  tile->SetADCHigh(150);
  tile->SetADCLow(40);
  tile->SetLocalTriggerPrimitive(120.);
  event.AddTile(tile);

  Calib calib;
  calib.SetRunNumber(42);
  calib.SetVop(44.);
  calib.SetVov(3.);
  TileCalib* cell = calib.GetTileCalib(0);
  cell->PedestalMeanH = 10.;
  cell->PedestalSigH = 1.;
  cell->PedestalMeanL = 5.;
  cell->PedestalSigL = 1.;
  cell->ScaleH = 100.;
  cell->ScaleWidthH = 10.;
  cell->ScaleL = 25.;
  cell->ScaleWidthL = 3.;
  cell->LGHGCorr = 4.;
  cell->LGHGCorrOff = 0.;
  cell->HGLGCorr = .25;
  cell->HGLGCorrOff = 0.;
  cell->BadChannel = 3;

  TFile output(argv[2], "RECREATE");
  TTree setupTree("Setup", "Setup");
  setupTree.Branch("setup", &wrappedSetup);
  setupTree.Fill();
  setupTree.Write();
  TTree dataTree("Data", "Data");
  dataTree.Branch("event", &event);
  dataTree.Fill();
  dataTree.Write();
  TTree calibTree("Calib", "Calib");
  calibTree.Branch("calib", &calib);
  calibTree.Fill();
  calibTree.Write();
  output.Close();
  return output.TestBit(TFile::kWriteError) ? 4 : 0;
}
