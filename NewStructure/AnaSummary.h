/**
 * @file AnaSummary.h
 * @brief Declaration of the AnaSummary class used to aggregate analysis-level run summaries.
 *
 * AnaSummary stores the per-run ROOT histograms used to summarize detector performance,
 * trigger occupancy, saturation behavior, and energy distributions in a compact form.
 */

#ifndef ANASUMMARY_H
#define ANASUMMARY_H

#include "TObject.h"
#include "TString.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TPad.h" 
#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "Calib.h"

/**
 * @class AnaSummary
 * @brief Aggregate compact analysis-level histograms for a single run.
 *
 * The class records the summary histograms used in detector-quality monitoring, such as
 * delta-time, energy, cell count, and saturation distributions. It acts as a lightweight
 * run-level container that can be serialized into ROOT files for later plotting.
 */
class AnaSummary : public TObject {
public:
 /**
  * @brief Default constructor.
  */
 AnaSummary():TObject(){}

 /**
  * @brief Construct a run summary object for the given detector configuration.
  * @param id Identifier used to distinguish the summary object.
  * @param RunNum Run number associated with the summary.
  * @param v Operating voltage for the run.
  * @param e Beam or event energy metadata.
  * @param pdg Particle type metadata.
  * @param ext Optional analysis extension flag.
  */
 AnaSummary(int id, int RunNum, double v, double e, int pdg, int ext = 0):TObject()
 {
   RunNr             = RunNum;
   Voltage           = v;
   Pdg               = pdg;
   Energy            = e;
 }

 /**
  * @brief Destroy the summary object.
  */
 ~AnaSummary(){}

 /**
  * @brief Execute the run summary analysis and emit status information.
  * @return True if the analysis step completed successfully.
  */
 bool Analyse();

 /**
  * @brief Fill the summary run-level information from a calibration object.
  * @param tc Calibration object used for the summary update.
  * @return True on successful fill.
  */
 bool Fill(const TileCalib&);

 /**
  * @brief Write the tracked summary histograms to a ROOT file.
  * @param f Output ROOT file.
  * @return True when the write succeeds.
  */
 bool Write(TFile*);

 /**
  * @brief Store a delta-time histogram for the run summary.
  * @param Hist Histogram to attach.
  * @return True when the histogram has been copied successfully.
  */
 bool SetDeltaTimeHist(TH1D*);

 /**
  * @brief Store an energy histogram for the run summary.
  * @param Hist Histogram to attach.
  * @return True when the histogram has been copied successfully.
  */
 bool SetEnergyHist(TH1D*);

 /**
  * @brief Store a cell-count histogram for the run summary.
  * @param Hist Histogram to attach.
  * @return True when the histogram has been copied successfully.
  */
 bool SetNCellsHist(TH1D*);

 /**
  * @brief Store the saturated-ADC cell-ID histogram.
  * @param Hist Histogram to attach.
  * @return True when the histogram has been copied successfully.
  */
 bool SetSatADCCellIDHist(TH1D*);

 /**
  * @brief Store the saturated-LG cell-ID histogram.
  * @param Hist Histogram to attach.
  * @return True when the histogram has been copied successfully.
  */
 bool SetSatLGCellIDHist(TH1D*);

 /**
  * @brief Store the LG/HG-outlier cell-ID histogram.
  * @param Hist Histogram to attach.
  * @return True when the histogram has been copied successfully.
  */
 bool SetLGHGOutCellIDHist(TH1D*);

 /**
  * @brief Store the saturated-ADC spectrum histogram.
  * @param Hist Histogram to attach.
  * @return True when the histogram has been copied successfully.
  */
 bool SetSatADCHist(TH1D*);

 /**
  * @brief Store the saturated-LG spectrum histogram.
  * @param Hist Histogram to attach.
  * @return True when the histogram has been copied successfully.
  */
 bool SetSatLGHist(TH1D*);

 /**
  * @brief Store the LG/HG-outlier spectrum histogram.
  * @param Hist Histogram to attach.
  * @return True when the histogram has been copied successfully.
  */
 bool SetLGHGOutHist(TH1D*);

 /**
  * @brief Return the delta-time histogram for this run summary.
  * @return Pointer to the delta-time histogram.
  */
 inline TH1D* GetDeltaTimeHist()     {return &hDeltaTime;};

 /**
  * @brief Return the energy histogram for this run summary.
  * @return Pointer to the energy histogram.
  */
 inline TH1D* GetEnergyHist()        {return &hEnergy;};

 /**
  * @brief Return the histogram of the number of active cells.
  * @return Pointer to the cell-count histogram.
  */
 inline TH1D* GetNCellsHist()        {return &hNCells;};

 /**
  * @brief Return the histogram of saturated ADC cell IDs.
  * @return Pointer to the saturated-ADC cell-id histogram.
  */
 inline TH1D* GetSatADCCellIDHist()  {return &hSatADCCellID;};

 /**
  * @brief Return the histogram of saturated LG cell IDs.
  * @return Pointer to the saturated-LG cell-id histogram.
  */
 inline TH1D* GetSatLGCellIDHist()   {return &hSatLGCellID;};

 /**
  * @brief Return the histogram of LG/HG outlier cell IDs.
  * @return Pointer to the LG/HG outlier cell-id histogram.
  */
 inline TH1D* GetLGHGOutCellIDHist() {return &hLGHGOutCellID;};

 /**
  * @brief Return the saturated-ADC spectrum histogram.
  * @return Pointer to the saturated-ADC histogram.
  */
 inline TH1D* GetSatADCHist()        {return &hSatADC;};

 /**
  * @brief Return the saturated-LG spectrum histogram.
  * @return Pointer to the saturated-LG histogram.
  */
 inline TH1D* GetSatLGHist()         {return &hSatLG;};

 /**
  * @brief Return the LG/HG outlier spectrum histogram.
  * @return Pointer to the LG/HG outlier histogram.
  */
 inline TH1D* GetLGHGOutHist()       {return &hLGHGOut;};
   
 /**
  * @brief Return the detector operating voltage for the run.
  * @return Voltage value.
  */
 inline double GetVoltage()      {return Voltage;};

 /**
  * @brief Return the energy metadata associated with the run.
  * @return Energy value.
  */
 inline double GetEnergy()       {return Energy;};

 /**
  * @brief Return the particle ID metadata for the run.
  * @return PDG-like particle identifier.
  */
 inline int GetPDG()             {return Pdg;};

 /**
  * @brief Return the run number for the summary.
  * @return Run identifier.
  */
 inline int GetRunNumber()       {return RunNr;};
   
 protected:
  int id             ;
  int RunNr          ;
  double Voltage     ;
  double Energy      ;
  int Pdg            ;
  TH1D hDeltaTime    ;
  TH1D hEnergy       ;
  TH1D hNCells       ;
  TH1D hSatADCCellID ;
  TH1D hSatADC       ;
  TH1D hSatLGCellID  ;
  TH1D hSatLG        ;
  TH1D hLGHGOutCellID;
  TH1D hLGHGOut      ;

  ClassDef(AnaSummary,3);
};

#endif
