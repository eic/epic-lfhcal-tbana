/**
 * @file CalibSummary.h
 * @brief Declaration of the CalibSummary class used to aggregate calibration-monitoring histograms.
 *
 * CalibSummary is a ROOT-aware summary container that collects the pedestal, gain, and
 * comparison distributions for a given calibration run. It is used to condense run-by-run
 * detector performance into compact histograms for plotting and QA studies.
 */

#ifndef CALIBSUMMARY_H
#define CALIBSUMMARY_H

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
#include "Setup.h"
#include "Tile.h"
#include "CommonHelperFunctions.h"

/**
 * @class CalibSummary
 * @brief Collect calibration summary histograms for a single run, channel set, or reference comparison.
 *
 * The class stores the distributions used to summarize pedestal stability, gain scale,
 * correlation behavior and run-to-run deviations. It provides the ROOT objects and accessors
 * needed to plot calibration quality across a detector configuration or test beam run.
 */
class CalibSummary: public TObject{

 public:
 /**
  * @brief Default constructor.
  */
 CalibSummary():TObject(){}

 /**
  * @brief Construct a calibration summary object and initialize the per-run histograms.
  * @param id Identifier used in the ROOT histogram names.
  * @param RunNum Run number associated with the summary.
  * @param v Detector operating voltage.
  * @param p Particle type metadata used by the run summary.
  * @param optHGCROC Readout selection flag for HGCROC-specific histograms.
  */
 CalibSummary(int id,int RunNum, double v, int p = 0, int optHGCROC = 0):TObject()
 {
    RunNr             = RunNum;
    RunNrRef          = RunNum;
    Voltage           = v;
    pdg               = p;
    rotype            = optHGCROC;
    hHGped            = TH1D(Form("hMeanPedHG_%i",id),"; #mu_{noise, HG} (arb. units); counts ", 500, -0.5, 500-0.5);
    hHGpedwidth       = TH1D(Form("hMeanPedHGwidth_%i",id),"; #sigma_{noise, HG} (arb. units); counts ", 400, -0.5*50/400, 50-(0.5*50/400));
    hLGped            = TH1D(Form("hMeanPedLG_%i",id),"; #mu_{noise, LG} (arb. units); counts ", 500, -0.5, 500-0.5);
    hLGpedwidth       = TH1D(Form("hMeanPedLGwidth_%i",id),"; #sigma_{noise, LG} (arb. units); counts ", 400, -0.5*20/400, 20-(0.5*20/400));
    hHGscale          = TH1D(Form("hHGScale_%i",id),";Max_{HG} (arb. units) ; counts ", 2000, -0.25, 1000-0.25);
    hHGscalewidth     = TH1D(Form("hHGScalewidth_%i",id),";Width_{HG} (arb. units) ; counts ", 2000, -0.25, 1000-0.25);
    hHGpedDiffRef     = TH1D(Form("hDiffPedvsRefHG_%i",id),"; #mu_{noise, HG}-#mu_{noise, HG, ref run} (arb. units); counts ", 501, -100, 100);
    hHGpedwidthDiffRef= TH1D(Form("hDiffPedWidthvsRefHG_%i",id),"; #sigma_{noise, HG}-#sigma_{noise, HG, ref run} (arb. units); counts ", 501, -100, 100);
    hLGpedDiffRef     = TH1D(Form("hDiffPedvsRefLG_%i",id),"; #mu_{noise, LG}-#mu_{noise, LG, ref run} (arb. units); counts ", 501, -100, 100);
    hHGscaleDiffRef   = TH1D(Form("hDiffHGScalevsRefHG_%i",id),"; Max_{HG}-Max_{HG,ref run} (arb. units); counts ", 1001, -250, 250);

    if (optHGCROC > 0){
      hHGscaleCorrRef   = TH2D(Form("hHGscaleCorrRef_%i",id),";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units); Max_{ADC, ref run} (arb. units) ; counts ", 350, -0.25, 350-0.25, 350, -0.25, 350-0.25);
      pHGscaleCorrRef   = TProfile(Form("pHGscaleCorrRef_%i",id),";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units)", 350, -0.25, 350-0.25);
    } else {
      hLGscale          = TH1D(Form("hLGScale_%i",id),";Max_{LG} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGscaleCalc      = TH1D(Form("hLGScaleCalc_%i",id),";Max_{LG,calc} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGscalewidth     = TH1D(Form("hHGScalewidth_%i",id),";Width_{LG} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGHGcorr         = TH1D(Form("hLGHGCorr_%i",id),"; a_{LG-HG} (arb. units) ; counts ", 400, 0, 20);
      hLGHGOffcorr      = TH1D(Form("hLGHGOffCorr_%i",id),"; b_{LG-HG} (arb. units) ; counts ", 1000, -200, 100);
      hHGLGcorr         = TH1D(Form("hHGLGCorr_%i",id),"; a_{HG-LG} (arb. units) ; counts ", 400, 0., 1.);
      hHGLGOffcorr      = TH1D(Form("hHGLGOffCorr_%i",id),"; b_{HG-LG} (arb. units) ; counts ", 1000, -100., 100.);
    
      hHGscaleCorrRef   = TH2D(Form("hHGscaleCorrRef_%i",id),";Max_{HG} (arb. units); Max_{HG, ref run} (arb. units) ; counts ", 1000, -0.25, 1000-0.25, 1000, -0.25, 1000-0.25);
      pHGscaleCorrRef   = TProfile(Form("pHGscaleCorrRef_%i",id), ";Max_{HG} (arb. units); Max_{HG, ref run} (arb. units)", 1000, -0.25, 1000-0.25);
      hLGscaleCorrRef   = TH2D(Form("hLGscaleCorrRef_%i",id),";Max_{LG} (arb. units); Max_{LG, ref run} (arb. units) ; counts ", 250, -0.25, 250-0.25, 250, -0.25, 250-0.25);
      pLGscaleCorrRef   = TProfile(Form("pLGscaleCorrRef_%i",id), ";Max_{LG} (arb. units); Max_{lG, ref run} (arb. units)", 250, -0.25, 250-0.25);
      hLGscaleDiffRef   = TH1D(Form("hDiffLGScalevsRefHG_%i",id),"; Max_{LG}-Max_{LG,ref run} (arb. units); counts ", 501, -100, 100);
      hLGscaleCalcDiffRef   = TH1D(Form("hDiffLGScaleCalcvsRefHG_%i",id),"; Max_{LG,calc}-Max_{LG,calc,ref run} (arb. units); counts ", 501, -100, 100);
      hLGHGcorrDiffRef  = TH1D(Form("hDiffLGScalevsRefHG_%i",id),"; Max_{LG}-Max_{LG,ref run} (arb. units); counts ", 501, -10, 10);

      hLGscaleCalcDiffLGscale       = TH1D(Form("hLGscaleCalcDiffLGscale%i",id),";Max_{LG,calc}/Max_{LG}; counts ", 200, 0, 2);
      hLGscaleCalcAlterDiffLGscale  = TH1D(Form("hLGscaleCalcAlterDiffLGscale%i",id),";Max_{LG,calc,alter}/Max_{LG}; counts ", 200, 0, 2);
      hLGscaleDiffHGscale           = TH1D(Form("hLGscaleDiffHGscale%i",id),";Max_{LG}/Max_{HG}; counts ", 200, 0, 1);
    }
    
  }

  /**
   * @brief Construct a calibration summary with layer-resolved HG scale histograms.
   * @param id Identifier used in the ROOT histogram names.
   * @param RunNum Run number associated with the summary.
   * @param layers Number of detector layers to initialize for per-layer monitoring.
   * @param v Detector operating voltage.
   * @param p Particle type metadata used by the run summary.
   * @param optHGCROC Readout selection flag for HGCROC-specific histograms.
   */
  CalibSummary(int id, int RunNum, int layers, double v, int p = 0,  int optHGCROC = 0):TObject()
  {
    std::cout << "Initializing with layer histos: " << RunNum << "\t" << layers << std::endl;
    
    RunNr             = RunNum;
    RunNrRef          = RunNum;
    Voltage           = v;
    pdg               = p;
    rotype            = optHGCROC;
    hHGped            = TH1D(Form("hMeanPedHG_%i",id),"; #mu_{noise, HG} (arb. units); counts ", 500, -0.5, 500-0.5);
    hHGpedwidth       = TH1D(Form("hMeanPedHGwidth_%i",id),"; #sigma_{noise, HG} (arb. units); counts ", 400, -0.5*50/400, 50-(0.5*50/400));
    hLGped            = TH1D(Form("hMeanPedLG_%i",id),"; #mu_{noise, LG} (arb. units); counts ", 500, -0.5, 500-0.5);
    hLGpedwidth       = TH1D(Form("hMeanPedLGwidth_%i",id),"; #sigma_{noise, LG} (arb. units); counts ", 400, -0.5*20/400, 20-(0.5*20/400));
    hHGscale          = TH1D(Form("hHGScale_%i",id),";Max_{HG} (arb. units) ; counts ", 2000, -0.25, 1000-0.25);
    hHGscalewidth     = TH1D(Form("hHGScalewidth_%i",id),";Width_{HG} (arb. units) ; counts ", 2000, -0.25, 1000-0.25);
    
    hHGpedDiffRef     = TH1D(Form("hDiffPedvsRefHG_%i",id),"; #mu_{noise, HG}-#mu_{noise, HG, ref run} (arb. units); counts ", 501, -100, 100);
    hHGpedwidthDiffRef= TH1D(Form("hDiffPedWidthvsRefHG_%i",id),"; #sigma_{noise, HG}-#sigma_{noise, HG, ref run} (arb. units); counts ", 501, -100, 100);
    hLGpedDiffRef     = TH1D(Form("hDiffPedvsRefLG_%i",id),"; #mu_{noise, LG}-#mu_{noise, LG, ref run} (arb. units); counts ", 501, -100, 100);
    hHGscaleDiffRef   = TH1D(Form("hDiffHGScalevsRefHG_%i",id),"; Max_{HG}-Max_{HG,ref run} (arb. units); counts ", 1001, -250, 250);
    
    if (optHGCROC > 0){
      hHGscaleCorrRef   = TH2D(Form("hHGscaleCorrRef_%i",id),";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units); Max_{ADC, ref run} (arb. units) ; counts ", 350, -0.25, 350-0.25, 350, -0.25, 350-0.25);
      pHGscaleCorrRef   = TProfile(Form("pHGscaleCorrRef_%i",id),";Max_{ADC} (arb. units); Max_{ADC, ref run} (arb. units)", 350, -0.25, 350-0.25);
    } else {
      hLGscale          = TH1D(Form("hLGScale_%i",id),";Max_{LG} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGscaleCalc      = TH1D(Form("hLGScaleCalc_%i",id),";Max_{LG,calc} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGscalewidth     = TH1D(Form("hHGScalewidth_%i",id),";Width_{LG} (arb. units) ; counts ", 2000, -0.5*250/2000, 250-(0.5*250/2000));
      hLGHGcorr         = TH1D(Form("hLGHGCorr_%i",id),"; a_{LG-HG} (arb. units) ; counts ", 400, 0, 20);
      hLGHGOffcorr      = TH1D(Form("hLGHGOffCorr_%i",id),"; b_{LG-HG} (arb. units) ; counts ", 1000, -200, 100);
      hHGLGcorr         = TH1D(Form("hHGLGCorr_%i",id),"; a_{HG-LG} (arb. units) ; counts ", 400, 0., 1.);
      hHGLGOffcorr      = TH1D(Form("hHGLGOffCorr_%i",id),"; b_{HG-LG} (arb. units) ; counts ", 1000, -100., 100.);

      hHGscaleCorrRef     = TH2D(Form("hHGscaleCorrRef_%i",id),";Max_{HG} (arb. units); Max_{HG, ref run} (arb. units) ; counts ", 1000, -0.25, 1000-0.25, 1000, -0.25, 1000-0.25);
      pHGscaleCorrRef     = TProfile(Form("pHGscaleCorrRef_%i",id), ";Max_{HG} (arb. units); Max_{HG, ref run} (arb. units)", 1000, -0.25, 1000-0.25);
      hLGscaleCorrRef     = TH2D(Form("hLGscaleCorrRef_%i",id),";Max_{LG} (arb. units); Max_{LG, ref run} (arb. units) ; counts ", 250, -0.25, 250-0.25, 250, -0.25, 250-0.25);
      pLGscaleCorrRef     = TProfile(Form("pLGscaleCorrRef_%i",id), ";Max_{LG} (arb. units); Max_{LG, ref run} (arb. units)", 250, -0.25, 250-0.25);
      hLGscaleDiffRef     = TH1D(Form("hDiffLGScalevsRefHG_%i",id),"; Max_{LG}-Max_{LG,ref run} (arb. units); counts ", 501, -100, 100);
      hLGscaleCalcDiffRef = TH1D(Form("hDiffLGScaleCalcvsRefHG_%i",id),"; Max_{LG,calc}-Max_{LG,calc,ref run} (arb. units); counts ", 501, -100, 100);
      hLGHGcorrDiffRef    = TH1D(Form("hDiffLGScalevsRefHG_%i",id),"; Max_{LG}-Max_{LG,ref run} (arb. units); counts ", 501, -10, 10);
      
      hLGscaleCalcDiffLGscale       = TH1D(Form("hLGscaleCalcDiffLGscale%i",id),";Max_{LG,calc}/Max_{LG}; counts ", 800, 0, 2);
      hLGscaleCalcAlterDiffLGscale  = TH1D(Form("hLGscaleCalcAlterDiffLGscale%i",id),";Max_{LG,calc,alter}/Max_{LG}; counts ", 800, 0, 2);
      hLGscaleDiffHGscale           = TH1D(Form("hLGscaleDiffHGscale%i",id),";Max_{LG}/Max_{HG}; counts ", 400, 0, 0.5);
    }    
    
    for (int l = 0; l < layers; l++){
      TH1D tempHScale   = TH1D(Form("hHGScale_layer_%d_%i",l,id),
                               ";Max_{HG} (arb. units) ; counts ", 1000, -0.25, 1000-0.25);
      hHGscaleLayer[l]  = tempHScale;
      TH1D tempHScaleWidth   =  TH1D(Form("hHGScalewidth_layer_%d_%i",l,id),
                                     ";Width_{HG} (arb. units) ; counts ", 1000, -0.25, 1000-0.25);
      hHGscalewidthLayer[l]  = tempHScaleWidth;
    }
  }

  /**
  * @brief Destroy the summary object.
  */
  ~CalibSummary(){}

  /**
  * @brief Analyse the current summary state and produce any derived values.
  * @param option Optional analysis mode selector.
  * @return Result code from the analysis routine.
  */
  int Analyse(int );

  /**
  * @brief Fill the summary histograms from a calibration constant structure.
  * @param tc Tile calibration constants to record.
  * @return True if the fill succeeded.
  */
  bool Fill(const TileCalib&);

  /**
  * @brief Fill the summary histograms from a calibration pointer.
  * @param tc Pointer to the TileCalib object to record.
  * @return True if the fill succeeded.
  */
	bool Fill(TileCalib*); // -EP

  /**
  * @brief Write all registered summary histograms to an output file.
  * @param f ROOT file to write into.
  * @return True if all histograms were stored successfully.
  */
  bool Write(TFile*);

  /**
  * @brief Attach a delta-time histogram to the calibration summary.
  * @param h Histogram containing delta-time information.
  * @return True when the histogram has been stored.
  */
  bool SetDeltaTimeHist(TH1D*);
   
  /**
  * @brief Fill the reference-run comparison histograms against another calibration set.
  * @param tc Current calibration constants.
  * @param tcRef Reference calibration constants.
  * @return True when the comparison fill completes.
  */
  bool FillRefRunProps( const TileCalib&, const TileCalib&);

  /**
  * @brief Fill the reference-run comparison histograms using pointers.
  * @param tc Current calibration constants.
  * @param tcRef Reference calibration constants.
  * @return True when the comparison fill completes.
  */
  bool FillRefRunProps( TileCalib*, TileCalib*);
   
  /**
  * @brief Fill the per-layer summary histograms for a given cell.
  * @param tc Calibration constants for the cell.
  * @param cellID Cell identifier used to resolve the layer.
  * @return True when the layer histogram is updated.
  */
  bool FillLayerProps(const TileCalib&, int);

  /**
  * @brief Fill the per-layer summary histograms using a pointer.
  * @param tc Calibration constants for the cell.
  * @param cellID Cell identifier used to resolve the layer.
  * @return True when the layer histogram is updated.
  */
  bool FillLayerProps(TileCalib*, int);
   
  /**
  * @brief Set the reference run number for comparison plots.
  * @param runNr Reference run number.
  */
  inline void SetRefRunNr(int runNr)  { RunNrRef = runNr; };

  /**
  * @brief Set a label for the summary object.
  * @param lab Label string to store.
  */
  inline void SetLabel(TString lab)   { label = lab; }

  /**
  * @brief Apply the run metadata to the summary object.
  * @param info RunInfo record containing the run-level settings.
  */
  void SetRunProperties (RunInfo);
   
   
  /**
   * @brief Return the delta-time histogram associated with this summary.
   * @return Pointer to the delta-time histogram.
   */
  inline TH1D* GetDeltaTime()     {return &hDeltaTime;};

  /**
   * @brief Return the high-gain pedestal histogram.
   * @return Pointer to the HG pedestal distribution.
   */
  inline TH1D* GetHGped()         {return &hHGped;};

  /**
   * @brief Return the high-gain pedestal width histogram.
   * @return Pointer to the HG pedestal-width distribution.
   */
  inline TH1D* GetHGpedwidth()    {return &hHGpedwidth;};

  /**
   * @brief Return the low-gain pedestal histogram.
   * @return Pointer to the LG pedestal distribution.
   */
  inline TH1D* GetLGped()         {return &hLGped;};

  /**
   * @brief Return the low-gain pedestal width histogram.
   * @return Pointer to the LG pedestal-width distribution.
   */
  inline TH1D* GetLGpedwidth()    {return &hLGpedwidth;};

  /**
   * @brief Return the high-gain scale histogram.
   * @return Pointer to the HG scale distribution.
   */
  inline TH1D* GetHGScale()       {return &hHGscale;};

  /**
   * @brief Return the high-gain scale-width histogram.
   * @return Pointer to the HG scale-width distribution.
   */
  inline TH1D* GetHGScalewidth()  {return &hHGscalewidth;};
   
  /**
   * @brief Return the low-gain scale histogram for CAEN-mode summaries.
   * @return Pointer to the LG scale distribution.
   */
  inline TH1D* GetLGScale()       {return &hLGscale;};

  /**
   * @brief Return the reconstructed low-gain scale histogram.
   * @return Pointer to the LG-scale calculation histogram.
   */
  inline TH1D* GetLGScaleCalc()   {return &hLGscaleCalc;};

  /**
   * @brief Return the low-gain scale-width histogram.
   * @return Pointer to the LG scale-width distribution.
   */
  inline TH1D* GetLGScalewidth()  {return &hLGscalewidth;};

  /**
   * @brief Return the LG/HG correlation histogram.
   * @return Pointer to the LG-HG correlation distribution.
   */
  inline TH1D* GetLGHGcorr()      {return &hLGHGcorr;};

  /**
   * @brief Return the LG/HG offset-correlation histogram.
   * @return Pointer to the LG/HG offset distribution.
   */
  inline TH1D* GetLGHGOffcorr()   {return &hLGHGOffcorr;};

  /**
   * @brief Return the HG/LG correlation histogram.
   * @return Pointer to the HG/LG correlation distribution.
   */
  inline TH1D* GetHGLGcorr()      {return &hHGLGcorr;};

  /**
   * @brief Return the HG/LG offset-correlation histogram.
   * @return Pointer to the HG/LG offset distribution.
   */
  inline TH1D* GetHGLGOffcorr()   {return &hHGLGOffcorr;};

  /**
   * @brief Return the ratio histogram between the calculated and nominal LG scale.
   * @return Pointer to the ratio distribution.
   */
  inline TH1D* GetLGScaleCalcDiffLGScale()      {return &hLGscaleCalcDiffLGscale;};

  /**
   * @brief Return the alternate LG-scale ratio histogram.
   * @return Pointer to the alternate ratio distribution.
   */
  inline TH1D* GetLGScaleCalcAlterDiffLGScale() {return &hLGscaleCalcAlterDiffLGscale;};

  /**
   * @brief Return the HG/LG scale ratio histogram.
   * @return Pointer to the LG-to-HG scale ratio distribution.
   */
  inline TH1D* GetLGScaleDiffHGScale()          {return &hLGscaleDiffHGscale;};
   
  /**
   * @brief Return the HG pedestal difference with respect to the reference run.
   * @return Pointer to the HG pedestal shift histogram.
   */
  inline TH1D* GetHGpedDiffRef()        {return &hHGpedDiffRef;};

  /**
   * @brief Return the HG pedestal-width difference with respect to the reference run.
   * @return Pointer to the HG pedestal-width shift histogram.
   */
  inline TH1D* GetHGpedwidthDiffRef()   {return &hHGpedwidthDiffRef;};

  /**
   * @brief Return the HG scale difference with respect to the reference run.
   * @return Pointer to the HG scale-shift histogram.
   */
  inline TH1D* GetHGscaleDiffRef()      {return &hHGscaleDiffRef;};

  /**
   * @brief Return the 2D HG scale correlation against the reference run.
   * @return Pointer to the HG scale comparison map.
   */
  inline TH2D* Get2DHGscaleCorrRef()    {return &hHGscaleCorrRef; };

  /**
   * @brief Return the profile version of the HG scale comparison to the reference run.
   * @return Pointer to the profiled HG scale correlation.
   */
  inline TProfile* GetProfHGscaleCorrRef()    {return &pHGscaleCorrRef; };
  TH1D* GetHGScaleLayer(int )  ;
  TH1D* GetHGScalewidthLayer(int );

  /**
   * @brief Return the LG pedestal difference versus the reference run.
   * @return Pointer to the LG pedestal-shift histogram.
   */
  inline TH1D* GetLGpedDiffRef()        {return &hLGpedDiffRef;};

  /**
   * @brief Return the LG scale difference versus the reference run.
   * @return Pointer to the LG scale-shift histogram.
   */
  inline TH1D* GetLGscaleDiffRef()      {return &hLGscaleDiffRef;};

  /**
   * @brief Return the calculated-LG scale difference versus the reference run.
   * @return Pointer to the LG reference-difference histogram.
   */
  inline TH1D* GetLGscaleCalcDiffRef()  {return &hLGscaleCalcDiffRef;};

  /**
   * @brief Return the LG/HG correlation difference versus the reference run.
   * @return Pointer to the LG/HG correlation-shift histogram.
   */
  inline TH1D* GetLGHGcorrDiffRef()     {return &hLGHGcorrDiffRef;};

  /**
   * @brief Return the 2D LG scale correlation against the reference run.
   * @return Pointer to the LG scale comparison map.
   */
  inline TH2D* Get2DLGscaleCorrRef()    {return &hLGscaleCorrRef; };

  /**
   * @brief Return the profile version of the LG scale comparison to the reference run.
   * @return Pointer to the profiled LG scale correlation.
   */
  inline TProfile* GetProfLGscaleCorrRef()    {return &pLGscaleCorrRef; };
   
  /**
   * @brief Return the detector bias voltage configured for the summary.
   * @return Voltage value stored in the summary.
   */
  inline double GetVoltage()      {return Voltage;};

  /**
   * @brief Return the run number associated with the summary.
   * @return Run number.
   */
  inline int GetRunNumber()       {return RunNr;};

  /**
   * @brief Return the reference run number used for comparison plots.
   * @return Reference run number.
   */
  inline int GetRunRefNumber()    {return RunNrRef;};

  /**
   * @brief Return the particle-type metadata value used in the summary.
   * @return PDG-like identifier.
   */
  inline int GetPdg()             {return pdg;};

  /**
   * @brief Return the user label assigned to the summary.
   * @return Label string.
   */
  inline TString GetLabel()       {return label; }
   
  /**
   * @brief Build the legend label for the summary based on common run properties.
   * @param commonRunInfo Run metadata used to assemble the legend.
   * @param nSameSettings Number of matching settings used in the comparison.
   * @return Formatted legend string.
   */
  TString GetLabelLegend( RunInfo commonRunInfo, int nSameSettings);
  
 protected:
  int id             ;
  int RunNr          ;
  int RunNrRef       ;
  int pdg            ;
  int rotype         ;
  double Voltage     ;
  double rf          ;
  double cf          ;
  double cc          ;
  double cfcomp      ;
  double energy      ;
  double injDAC      ;
  double temp        ;
  TString label      = "";
  TH1D hLGped        ;
  TH1D hLGpedwidth   ;
  TH1D hHGped        ;
  TH1D hHGpedwidth   ;
  TH1D hHGscale      ;
  TH1D hHGscalewidth ;
  TH1D hHGpedDiffRef        ;
  TH1D hHGpedwidthDiffRef   ;
  TH1D hHGscaleDiffRef      ;
  TH2D hHGscaleCorrRef      ;
  TProfile pHGscaleCorrRef  ;

  TH1D hDeltaTime    ;
  
  // CAEN only
  TH1D hLGscale      ;
  TH1D hLGscaleCalc  ;
  TH1D hLGscaleCalcDiffLGscale  ;
  TH1D hLGscaleCalcAlterDiffLGscale  ;
  TH1D hLGscaleDiffHGscale  ;
  TH1D hLGscalewidth ;
  TH1D hHGLGcorr     ;
  TH1D hHGLGOffcorr  ;
  TH1D hLGHGcorr     ;
  TH1D hLGHGOffcorr  ;

  TH1D hLGpedDiffRef        ;
  TH1D hLGscaleDiffRef      ;
  TH1D hLGscaleCalcDiffRef  ;
  TH1D hLGHGcorrDiffRef     ;
  TH2D hLGscaleCorrRef      ;
  TProfile pLGscaleCorrRef  ;
  
  
  std::map<int, TH1D> hHGscaleLayer;
  std::map<int, TH1D> hHGscalewidthLayer;
  
  ClassDef(CalibSummary,6);
};

#endif
