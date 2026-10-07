#ifndef LFHCAL_CHECKED_IO_H
#define LFHCAL_CHECKED_IO_H

#include "TError.h"
#include "TFile.h"
#include <atomic>
#include <cstring>
#include <exception>
#include <initializer_list>
#include <iostream>

namespace lfhcal {

// One guard per command-line process. Keep ROOT's diagnostics and fatal-error
// behavior, while remembering I/O errors from APIs such as SaveAs that return void.
class RootIOErrors {
 public:
  RootIOErrors() {
    failed().store(false);
    previous() = SetErrorHandler(Handle);
  }
  ~RootIOErrors() { SetErrorHandler(previous()); }
  RootIOErrors(const RootIOErrors&) = delete;
  RootIOErrors& operator=(const RootIOErrors&) = delete;
  bool ok() const { return !failed().load(); }

 private:
  static std::atomic<bool>& failed() {
    static std::atomic<bool> value{false};
    return value;
  }
  static ErrorHandlerFunc_t& previous() {
    static ErrorHandlerFunc_t value = nullptr;
    return value;
  }
  static bool IsIOError(int level, const char* location) {
    if (level < kError || !location) return false;
    const char* prefixes[] = {"TFile::", "TDirectory", "TBranch", "TTree::",
      "TBufferFile::", "TKey::", "TASImage::", "TImage::", "TCanvas::",
      "TPad::", "TPDF::", "TPostScript::", "TSVG::"};
    for (const char* prefix : prefixes)
      if (std::strncmp(location, prefix, std::strlen(prefix)) == 0) return true;
    return false;
  }
  static void Handle(int level, Bool_t abort, const char* location, const char* message) {
    if (IsIOError(level, location)) failed().store(true);
    ErrorHandlerFunc_t handler = previous();
    if (handler && handler != Handle) handler(level, abort, location, message);
    else DefaultErrorHandler(level, abort, location, message);
  }
};

inline bool CloseAndCheckOutputs(std::initializer_list<TFile*> outputs) {
  bool ok = true;
  for (TFile* output : outputs) {
    if (!output) continue;
    if (output->IsOpen()) {
      output->Flush();
      output->Close();
    }
    if (output->IsZombie() || output->TestBit(TFile::kWriteError)) {
      std::cerr << "ROOT output failed: " << output->GetName() << std::endl;
      ok = false;
    }
  }
  return ok;
}

// Shared exit-status handling for Convert and DataPrep. Do not short-circuit
// the output check: Close can expose a buffered write failure after Process.
template <class Analysis>
int RunCheckedAnalysis(Analysis& analysis, const char* executable) {
  RootIOErrors io;
  try {
    if (!analysis.CheckAndOpenIO()) {
      std::cerr << executable << ": input/configuration check failed" << std::endl;
      return 1;
    }
    const bool processed = analysis.Process();
    const bool written = CloseAndCheckOutputs(
        {analysis.RootOutput, analysis.RootOutputHist, analysis.RootCalibOutput});
    if (!processed || !written || !io.ok()) {
      std::cerr << executable << ": processing or I/O failed" << std::endl;
      return 1;
    }
  } catch (const std::exception& error) {
    std::cerr << executable << ": processing or I/O failed: " << error.what() << std::endl;
    return 1;
  }
  std::cout << "Exiting" << std::endl;
  return 0;
}

} // namespace lfhcal
#endif
