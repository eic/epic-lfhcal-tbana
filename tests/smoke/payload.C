// Uses current production classes, not the old Tests/Setup demonstration class.
#include "../../NewStructure/Calib.h"
#include "../../NewStructure/Tile.h"
#include <TClass.h>
#include <TFile.h>
#include <TSystem.h>
#include <TTree.h>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void smoke_require(bool ok, const char* message) {
  if (!ok) throw std::runtime_error(message);
}
void smoke_calibration(Calib& calibration) {
  // Exact, deterministic control for production GetCalcScaleLow: 120 / 4 = 30.
  const double value = calibration.GetCalcScaleLow(7);
  smoke_require(std::isfinite(value) && std::abs(value - 30.) <= 1.e-12,
                "known calibration value must be 30");
  smoke_require(calibration.GetRunNumber() == 42, "calibration run number changed");
  smoke_require(calibration.GetBCCalib() && calibration.GetVop() == 44.
                && calibration.GetVov() == 3., "calibration metadata changed");
  const auto channel = calibration.find(7);
  smoke_require(channel != calibration.end() && channel->second.ScaleH == 120.
                && channel->second.LGHGCorr == 4. && channel->second.BadChannel == 3,
                "calibration channel constants changed");
}
}

int lfhcal_smoke_payload() {
  try {
    const char* path = gSystem->Getenv("LFHCAL_SMOKE_FILE");
    const char* requested_mode = gSystem->Getenv("LFHCAL_SMOKE_MODE");
    smoke_require(path && requested_mode, "missing smoke fixture configuration");
    const std::string mode(requested_mode);
    for (const char* name : {"Tile", "Calib", "TileCalib"}) {
      TClass* type = TClass::GetClass(name);
      smoke_require(type && type->IsLoaded(), "production ROOT dictionary is not loaded");
    }
    if (mode == "write") {
      Calib calibration;
      calibration.SetRunNumber(42);
      calibration.SetVop(44.);
      calibration.SetVov(3.);
      calibration.SetBCCalib(true);
      TileCalib* channel = calibration.GetTileCalib(7);
      channel->ScaleH = 120.;
      channel->LGHGCorr = 4.;
      channel->BadChannel = 3;
      smoke_calibration(calibration);
      TFile file(path, "CREATE");
      smoke_require(!file.IsZombie() && file.IsWritable(), "cannot create ROOT fixture");
      smoke_require(file.WriteObject(&calibration, "calibration") > 0,
                    "cannot write calibration object");
      {
        Tile tile(ReadOut::Type::Caen);
        tile.SetCellID(7);
        tile.SetE(2.5);
        tile.SetTOA(12.);
        Tile* address = &tile;
        TTree tree("events", "one synthetic LFHCal tile");
        smoke_require(tree.Branch("tile", &address) != nullptr, "cannot create Tile branch");
        smoke_require(tree.Fill() > 0 && tree.Write() > 0, "cannot write event tree");
      }
      file.Close();
      smoke_require(!file.TestBit(TFile::kWriteError), "ROOT reported a write/close error");
    } else if (mode == "read") {
      // A new ROOT process reads disk state, not an object retained by the writer.
      TFile file(path, "READ");
      smoke_require(!file.IsZombie() && !file.TestBit(TFile::kRecovered),
                    "fixture missing, corrupt, or recovered");
      Calib* calibration = nullptr;
      file.GetObject("calibration", calibration);
      smoke_require(calibration != nullptr, "missing calibration object");
      smoke_calibration(*calibration);
      TTree* tree = nullptr;
      file.GetObject("events", tree);
      smoke_require(tree && tree->GetEntries() == 1, "expected exactly one event");
      Tile* tile = nullptr;
      smoke_require(tree->SetBranchAddress("tile", &tile) >= 0, "cannot bind Tile branch");
      smoke_require(tree->GetEntry(0) > 0 && tile, "cannot load first event");
      smoke_require(tile->GetCellID() == 7 && tile->GetE() == 2.5 && tile->GetTOA() == 12.,
                    "Tile round-trip values changed");
      tree->ResetBranchAddresses();
      delete tile;
    } else {
      throw std::runtime_error("unknown smoke mode");
    }
    std::cout << "LFHCAL_SMOKE_OK: " << mode << '\n';
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "FAIL: " << error.what() << '\n';
    return 1;
  }
}
