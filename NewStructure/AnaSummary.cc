/**
 * @file AnaSummary.cc
 * @brief Implementation of the run-level analysis summary helper.
 *
 * AnaSummary stores compact ROOT histograms summarizing detector performance and event
 * quality for a single analysis run.
 */

#include "AnaSummary.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"

ClassImp(AnaSummary);

/**
 * @brief Fill the run summary from a calibration object.
 * @param tc Calibration object to process.
 * @return Always true for the current implementation.
 */
bool AnaSummary::Fill(const TileCalib& tc){
  return true;
}

/**
 * @brief Write the summary histograms to a ROOT file.
 * @param f Output ROOT file.
 * @return Always true for the current implementation.
 */
bool AnaSummary::Write(TFile* f){
  f->cd();
  return true;
}

/**
 * @brief Print the run summary metadata and confirm the analysis step completed.
 * @return True after emitting the summary information.
 */
bool AnaSummary::Analyse(){
  std::cout << "***********************************************************************************************************************" << std::endl;
  std::cout << "Run Nr.: "<< RunNr <<  "\t Voltage: "<< Voltage << std::endl;
  std::cout << "***********************************************************************************************************************" << std::endl;
  return true;
}

/**
 * @brief Copy and normalize a delta-time histogram into the run summary.
 * @param Hist Histogram to record.
 * @return True if the histogram is accepted; false if no histogram is provided.
 */
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

/**
 * @brief Copy and normalize an energy histogram into the run summary.
 * @param Hist Histogram to record.
 * @return True if the histogram is accepted; false if no histogram is provided.
 */
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


/**
 * @brief Copy and normalize the cell-count histogram into the run summary.
 * @param Hist Histogram to record.
 * @return True if the histogram is accepted; false if no histogram is provided.
 */
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

/**
 * @brief Copy and normalize the saturated-ADC histogram into the run summary.
 * @param Hist Histogram to record.
 * @return True if the histogram is accepted; false if no histogram is provided.
 */
bool AnaSummary::SetSatADCHist(TH1D* Hist) {
    if (Hist){
      TH1D temp = *Hist;
      temp.SetName(Form("%s_Run%i",Hist->GetName(),RunNr));
      temp.SetDirectory(0);
      temp.Scale(1/temp.GetEntries());
      temp.GetYaxis()->SetTitle("Counts/event");
      hSatADC = temp;
      return true;
    }
    else{
        return false;
    }
    return false;
}
  
/**
 * @brief Copy and normalize the saturated-LG histogram into the run summary.
 * @param Hist Histogram to record.
 * @return True if the histogram is accepted; false if no histogram is provided.
 */
bool AnaSummary::SetSatLGHist(TH1D* Hist) {
    if (Hist){
      TH1D temp = *Hist;
      temp.SetName(Form("%s_Run%i",Hist->GetName(),RunNr));
      temp.SetDirectory(0);
      temp.Scale(1/temp.GetEntries());
      temp.GetYaxis()->SetTitle("Counts/event");
      hSatLG = temp;
      return true;
    }
    else{
        return false;
    }
    return false;
}
   
/**
 * @brief Copy and normalize the LG/HG outlier spectrum into the run summary.
 * @param Hist Histogram to record.
 * @return True if the histogram is accepted; false if no histogram is provided.
 */
bool AnaSummary::SetLGHGOutHist(TH1D* Hist) {
    if (Hist){
      TH1D temp = *Hist;
      temp.SetName(Form("%s_Run%i",Hist->GetName(),RunNr));
      temp.SetDirectory(0);
      temp.Scale(1/temp.GetEntries());
      temp.GetYaxis()->SetTitle("Counts/event");
      hLGHGOut = temp;
      return true;
    }
    else{
        return false;
    }
    return false;
}
   
/**
 * @brief Copy and normalize the saturated-ADC cell-ID histogram into the run summary.
 * @param Hist Histogram to record.
 * @return True if the histogram is accepted; false if no histogram is provided.
 */
bool AnaSummary::SetSatADCCellIDHist(TH1D* Hist) {
    if (Hist){
      TH1D temp = *Hist;
      temp.SetName(Form("%s_Run%i",Hist->GetName(),RunNr));
      temp.SetDirectory(0);
      temp.Scale(1/temp.GetEntries());
      temp.GetYaxis()->SetTitle("Counts/event");
      hSatADCCellID = temp;
      return true;
    }
    else{
        return false;
    }
    return false;
}

/**
 * @brief Copy and normalize the saturated-LG cell-ID histogram into the run summary.
 * @param Hist Histogram to record.
 * @return True if the histogram is accepted; false if no histogram is provided.
 */
bool AnaSummary::SetSatLGCellIDHist(TH1D* Hist) {
    if (Hist){
      TH1D temp = *Hist;
      temp.SetName(Form("%s_Run%i",Hist->GetName(),RunNr));
      temp.SetDirectory(0);
      temp.Scale(1/temp.GetEntries());
      temp.GetYaxis()->SetTitle("Counts/event");
      hSatLGCellID = temp;
      return true;
    }
    else{
        return false;
    }
    return false;
}

/**
 * @brief Copy and normalize the LG/HG outlier cell-ID histogram into the run summary.
 * @param Hist Histogram to record.
 * @return True if the histogram is accepted; false if no histogram is provided.
 */
bool AnaSummary::SetLGHGOutCellIDHist(TH1D* Hist) {
    if (Hist){
      TH1D temp = *Hist;
      temp.SetName(Form("%s_Run%i",Hist->GetName(),RunNr));
      temp.SetDirectory(0);
      temp.Scale(1/temp.GetEntries());
      temp.GetYaxis()->SetTitle("Counts/event");
      hLGHGOutCellID = temp;
      return true;
    }
    else{
        return false;
    }
    return false;
}
