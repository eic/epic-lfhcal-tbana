// Fault injection at the Analyses boundary, linked to the real CLI entry points
// and Calib implementation. No detector processing is performed by this fixture.
#include "Analyses.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TError.h"
#include <cerrno>
#include <cstdlib>
#include <string>

class FaultFile : public TFile {
 public:
  explicit FaultFile(const char* path) : TFile(path, "RECREATE") {}
  bool fail = false;
  bool shortWrite = false;
 protected:
  Int_t SysWrite(Int_t fd, const void* data, Int_t length) override {
    if (fail) { errno = EDQUOT; return -1; }
    if (shortWrite) return length > 0 ? length - 1 : 0;
    return TFile::SysWrite(fd, data, length);
  }
};

std::string mode() {
  const char* value = std::getenv("LFHCAL_IO_TEST");
  return value ? value : "good";
}

bool Analyses::CheckAndOpenIO() {
  gROOT->SetBatch(true);
  return mode() != "check_false";
}

bool Analyses::Process() {
  const std::string test = mode();
  if (test == "process_false") return false;
  if (test == "fit_error") { ::Error("Fit", "fixture rejected fit"); return true; }
  if (test == "warning") { ::Warning("TFile::Init", "fixture warning"); return true; }
  if (test == "text_full") { calib.PrintCalibToFile("/dev/full"); return true; }
  if (test == "text_open") {
    calib.PrintCalibToFile(RootOutputName + "/missing/calib.txt");
    return true;
  }
  if (test == "image") {
    TCanvas canvas("canvas", "test", 320, 240);
    TH1D h("image_h", "test", 10, 0, 10);
    h.Fill(3); h.Draw();
    canvas.SaveAs(RootOutputName + "/missing/test.png");
    return true;
  }
  auto* file = new FaultFile(RootOutputName);
  RootOutput = file;
  TH1D h("h", "test", 10, 0, 10);
  h.SetDirectory(nullptr);
  h.Fill(3);
  if (test == "root_write") file->fail = true;
  if (test == "root_short") file->shortWrite = true;
  h.Write();
  if (test == "root_close") {
    file->fail = true; // final buffered/header writes happen after Process returns
  } else {
    file->Close();
  }
  if (test == "good") calib.PrintCalibToFile(RootOutputName + ".txt");
  return true;
}
