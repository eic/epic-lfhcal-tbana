// Load the exact requested production library before interpreting the payload.
// ROOT can otherwise print an interpreter error but still exit successfully.
#include <TError.h>
#include <TROOT.h>
#include <TString.h>
#include <TSystem.h>
#include <iostream>

namespace {
bool smoke_root_error = false;
ErrorHandlerFunc_t smoke_previous_handler = nullptr;
void smoke_error_handler(Int_t level, Bool_t abort, const char* location, const char* message) {
  if (level >= kError) smoke_root_error = true;
  if (smoke_previous_handler) smoke_previous_handler(level, abort, location, message);
}
}

void driver() {
  const char* library = gSystem->Getenv("LFHCAL_SMOKE_LIBRARY");
  const char* payload = gSystem->Getenv("LFHCAL_SMOKE_PAYLOAD");
  smoke_previous_handler = SetErrorHandler(smoke_error_handler);
  if (library) {
    // The dictionary may request the library again by basename. Point ROOT at
    // the selected artifact directory, not at whichever checkout is the cwd.
    const TString directory = gSystem->DirName(library);
    gSystem->SetDynamicPath(directory + ":" + gSystem->GetDynamicPath());
  }
  if (!library || !payload || gSystem->Load(library) < 0) {
    std::cerr << "FAIL: cannot load requested LFHCal library/payload\n";
    gSystem->Exit(1);
    return;
  }
  if (gROOT->LoadMacro(payload) < 0) {
    std::cerr << "FAIL: cannot interpret smoke payload\n";
    gSystem->Exit(1);
    return;
  }
  Int_t error = 0;
  const Longptr_t result = gROOT->ProcessLine("lfhcal_smoke_payload()", &error);
  gSystem->Exit(error == 0 && result == 0 && !smoke_root_error ? 0 : 1);
}
