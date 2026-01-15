#include "AnaSummary.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

ClassImp(AnaSummary);

bool AnaSummary::Fill(const TileCalib& tc){
  return true;
}

bool AnaSummary::Write(TFile* f){
  f->cd();
  return true;
}

bool AnaSummary::Analyse(){
  std::cout << "***********************************************************************************************************************" << std::endl;
  std::cout << "Run Nr.: "<< RunNr <<  "\t Voltage: "<< Voltage << std::endl;
  std::cout << "***********************************************************************************************************************" << std::endl;
  return true;
}

bool AnaSummary::SetDeltaTimeHist(TH1D* Hist) {
    if (Hist){
      TH1D temp = *Hist;
      temp.SetName(Form("%s_Run%i",Hist->GetName(),RunNr));
      temp.SetDirectory(0);
      temp.Scale(1/temp.GetEntries());
      temp.GetYaxis()->SetTitle("Counts/event");
      hDeltaTime = temp;
      return true;
    }
    else{
        return false;
    }
    return false;
}

bool AnaSummary::SetEnergyHist(TH1D* Hist) {
    if (Hist){
      TH1D temp = *Hist;
      temp.SetName(Form("%s_Run%i",Hist->GetName(),RunNr));
      temp.SetDirectory(0);
      temp.Scale(1/temp.GetEntries());
      temp.GetYaxis()->SetTitle("Counts/event");
      hEnergy = temp;
      return true;
    }
    else{
        return false;
    }
    return false;
}


bool AnaSummary::SetNCellsHist(TH1D* Hist) {
    if (Hist){
      TH1D temp = *Hist;
      temp.SetName(Form("%s_Run%i",Hist->GetName(),RunNr));
      temp.SetDirectory(0);
      temp.Scale(1/temp.GetEntries());
      temp.GetYaxis()->SetTitle("Counts/event");
      hNCells = temp;
      return true;
    }
    else{
        return false;
    }
    return false;
}
