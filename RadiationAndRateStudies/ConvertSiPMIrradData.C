// A C++ script to read data from a CSV file and plot multiple files on the same graph
// using the ROOT framework.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Include necessary ROOT headers
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TAxis.h"
#include "TLegend.h"
#include "TLatex.h"

//================================================================================
// Function to read data from a single CSV file and create a TGraph object.
//================================================================================
TGraph* createGraphFromFile(TString filename, const char* title, Bool_t logY = kFALSE, Double_t shiftX = 0.) {
  // Open the CSV file.
  std::ifstream inputFile(filename.Data());

  // Check if the file was successfully opened.
  if (!inputFile.is_open()) {
    std::cerr << "Error: Could not open file " << filename.Data() << std::endl;
    return nullptr;
  }

  // Vectors to store the current and voltage data.
  std::vector<double> voltage;
  std::vector<double> current;

  // Skip the header line.
  std::string headerLine;
  std::getline(inputFile, headerLine);

  // Read data line by line from the CSV.
  std::string line;
  while (std::getline(inputFile, line)) {
    // Find the comma separator.
    size_t commaPos = line.find(',');
    if (commaPos != std::string::npos) {
      // Extract voltage and current strings.
      std::string voltageStr = line.substr(0, commaPos);
      std::string currentStr = line.substr(commaPos + 1);

      // Convert string data to double and store in vectors.
      try {
        voltage.push_back(std::stod(voltageStr)-shiftX);
        if (logY){
          current.push_back(TMath::Log(std::stod(currentStr)));
        } else {
          current.push_back(std::stod(currentStr));
        }
      } catch (const std::exception& e) {
        std::cerr << "Error converting data: " << e.what() << std::endl;
        continue;
      }
    }
  }
  inputFile.close();

  // Check if any valid data was read.
  if (voltage.empty() || current.empty() || voltage.size() != current.size()) {
    std::cerr << "Error: No valid data found or mismatch in data size for file " << filename.Data() << std::endl;
    return nullptr;
  }

  // Create a TGraph object and return it.
  TGraph* graph = new TGraph(voltage.size(), voltage.data(), current.data());
  graph->SetTitle(title);
  return graph;
}

//================================================================================
// The main function to convert the files to root graphs
//================================================================================
void ConvertSiPMIrradData() {

    // Define an array of colors and markers to use for the plots.
    int colors[] = {kRed+1, kBlue+1, kGreen + 2, kCyan+1}; //, kOrange, kMagenta
    int markers[] = {kFullCircle, kFullSquare, 33, 28, 34, 24, 25, 27, 42, 46, 43, 47, 29, 30};
    
    Int_t l[4] = {-1, -1, -1, -1};
    TString labelsNP[7] =  {"10^{0}", "10^{8}", "10^{9}", "10^{10}", "10^{11}", "10^{12}", "10^{13}"}; 
    TString outNP[7] =  {"10_0", "10_8", "10_9", "10_10", "10_11", "10_12", "10_13"}; 
    Double_t NP[7] =  {1e0, 1e8, 1e9, 1e10, 1e11, 1e12, 1e13}; 
    
    TString labelsSiPM[4] =  {"A", "B", "C", "D"}; 

    //*****************************************************************************
    // Original data from: 
    // https://arxiv.org/pdf/2503.14622
    // https://zenodo.org/records/14520936
    //*****************************************************************************
    // Define a list of file names you want to plot.
    std::vector<std::string> filenamesS14_1315 = {
        "Data/S14_1315/New_Irradiated/1315A00_UTC_20240815__21_40_converted.csv",
        "Data/S14_1315/New_Irradiated/1315A08_UTC_20240815__22_43_converted.csv",
        "Data/S14_1315/New_Irradiated/1315A09_UTC_20240815__22_59_converted.csv",
        "Data/S14_1315/New_Irradiated/1315B9_UTC_20240816__18_13_converted.csv",
        "Data/S14_1315/New_Irradiated/1315C9_UTC_20240816__19_33_converted.csv",
        "Data/S14_1315/New_Irradiated/1315A10_UTC_20240815__23_14_converted.csv",
        "Data/S14_1315/New_Irradiated/1315B10_UTC_20240816__18_26_converted.csv",  
        "Data/S14_1315/New_Irradiated/1315C10_UTC_20240816__19_45_converted.csv",
        "Data/S14_1315/New_Irradiated/1315A11_UTC_20240815__23_25_converted.csv",
        "Data/S14_1315/New_Irradiated/1315B11_UTC_20240816__18_48_converted.csv",
        "Data/S14_1315/New_Irradiated/1315C11_UTC_20240816__19_56_converted.csv",
        "Data/S14_1315/New_Irradiated/1315A12_UTC_20240815__23_50_converted.csv", 
        "Data/S14_1315/New_Irradiated/1315B12_UTC_20240816__19_00_converted.csv",
        "Data/S14_1315/New_Irradiated/1315C12_UTC_20240816__20_06_converted.csv",
        "Data/S14_1315/New_Irradiated/1315A13_UTC_20240816__00_10_converted.csv",
        "Data/S14_1315/New_Irradiated/1315B13_UTC_20240816__19_15_converted.csv",
        // Add more file names here as needed.
    };

    std::vector<TString> labelsS14_1315 = {
        "A 10^{0} N_{p^{+}}",
        "A 10^{8} N_{p^{+}}",
        "A 10^{9} N_{p^{+}}",
        "B 10^{9} N_{p^{+}}",
        "C 10^{9} N_{p^{+}}",
        "A 10^{10} N_{p^{+}}",
        "B 10^{10} N_{p^{+}}",
        "C 10^{10} N_{p^{+}}",
        "A 10^{11} N_{p^{+}}",
        "B 10^{11} N_{p^{+}}",
        "C 10^{11} N_{p^{+}}",
        "A 10^{12} N_{p^{+}}",
        "B 10^{12} N_{p^{+}}",
        "C 10^{12} N_{p^{+}}",
        "A 10^{13} N_{p^{+}}",
        "B 10^{13} N_{p^{+}}",
        // Add more file names here as needed.
    };

    std::vector<std::string> filenamesS14_3015 = {
      "Data/S14_3015/New_Nonirradiated/Hex_Nonirradiated_Retest_SiPM_A_UTC_20240726__16_57_converted.txt",
      "Data/S14_3015/New_Nonirradiated/Hex_Nonirradiated_Retest_SiPM_B_UTC_20240726__17_16_converted.txt",
      "Data/S14_3015/New_Nonirradiated/Hex_Nonirradiated_Retest_SiPM_C_UTC_20240726__17_28_converted.txt",
      "Data/S14_3015/New_Nonirradiated/Hex_Nonirradiated_UTC_20240724__00_19_converted.txt",
      "Data/S14_3015/New_Irradiated/Hex_3015A08_UTC_20240724__23_44_coverted.csv",
      // "Data/S14_3015/New_Irradiated/Hex_3015A09_UTC_20240725__00_04_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015A9_Retest_UTC_20240726__17_59_converted.csv",
      // "Data/S14_3015/New_Irradiated/Hex_3015B9_UTC_20240725__00_15_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015B9_Retest_UTC_20240726__18_19_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015A10_UTC_20240725__16_55_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015B10_UTC_20240725__17_13_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015A11_UTC_20240725__17_35_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015B11_UTC_20240725__17_47_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015C11_UTC_20240725__18_11_converted.csv",
      // "Data/S14_3015/New_Irradiated/Hex_3015A12_UTC_20240724__18_12_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015A12_Retest_UTC_20240726__19_00_converted.csv",
      // "Data/S14_3015/New_Irradiated/Hex_3015B12_UTC_20240724__22_20_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015B12_Retest_UTC_20240726__19_31_converted.csv",
      // "Data/S14_3015/New_Irradiated/Hex_3015C12_UTC_20240724__22_33_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015C12_Retest_UTC_20240726__21_26_converted.csv",
      "Data/S14_3015/New_Irradiated/Hex_3015A13_UTC_20240725__20_03_converted.csv",
    };

    std::vector<TString> labelsS14_3015 = {
        "A 10^{0} N_{p^{+}}",
        "B 10^{0} N_{p^{+}}",
        "C 10^{0} N_{p^{+}}",
        "D 10^{0} N_{p^{+}}",
        "A 10^{8} N_{p^{+}}",
        "A 10^{9} N_{p^{+}}",
        // "A 10^{9} N_{p^{+}}",
        "B 10^{9} N_{p^{+}}",
        // "B 10^{9} N_{p^{+}}",
        "A 10^{10} N_{p^{+}}",
        "B 10^{10} N_{p^{+}}",
        "A 10^{11} N_{p^{+}}",
        "B 10^{11} N_{p^{+}}",
        "C 10^{11} N_{p^{+}}",
        "A 10^{12} N_{p^{+}}",
        // "A 10^{12} N_{p^{+}}",
        "B 10^{12} N_{p^{+}}",
        // "B 10^{12} N_{p^{+}}",
        "C 10^{12} N_{p^{+}}",
        // "C 10^{12} N_{p^{+}}",
        "A 10^{13} N_{p^{+}}",
        // Add more file names here as needed.
    };
    
    TFile* outputRoot = new TFile("Data/IrradiationStudies.root","UPDATE");
    outputRoot->mkdir("S14160-1315");
    outputRoot->cd("S14160-1315");
    
    TGraph* graphSignalS14_1315 = createGraphFromFile("Data/S14_1315/TimeVsAmplitude.csv", "SignalShape-13mm");
    graphSignalS14_1315->SetLineColor(kBlue+2);
    graphSignalS14_1315->SetMarkerColor(kBlue+2);
    graphSignalS14_1315->SetMarkerStyle(20);
    graphSignalS14_1315->GetXaxis()->SetTitle("#it{t} (ns)");
    graphSignalS14_1315->GetYaxis()->SetTitle("#it{A} (a.U.)");
    graphSignalS14_1315->Print();

    TGraph* graphPhotonEffi = createGraphFromFile("Data/S14_1315/WaveLengthvsPhotonEffi.csv", "PhotonEfficiencyVsWavelength");
    graphPhotonEffi->SetLineColor(kBlue+2);
    graphPhotonEffi->SetMarkerColor(kBlue+2);
    graphPhotonEffi->SetMarkerStyle(20);
    graphPhotonEffi->GetXaxis()->SetTitle("#lambda (nm)");
    graphPhotonEffi->GetYaxis()->SetTitle("#PDE (%)");
    // graphPhotonEffi->Print();

    TGraph* graphGain = createGraphFromFile("Data/S14_1315/OverVoltageVsGain.csv", "Gain");
    graphGain->SetLineColor(kBlue+2);
    graphGain->SetMarkerColor(kBlue+2);
    graphGain->SetMarkerStyle(20);
    graphGain->GetXaxis()->SetTitle("#it{V_{ov}} (V)");
    graphGain->GetYaxis()->SetTitle("#it{G} (a.U.)");
    graphGain->Print();
    
    TGraph* graphSignalS14_3015 = createGraphFromFile("Data/S14_3015/TimeVsAmplitude.csv", "SignalShape-30mm");
    graphSignalS14_3015->SetLineColor(kBlue+2);
    graphSignalS14_3015->SetMarkerColor(kBlue+2);
    graphSignalS14_3015->SetMarkerStyle(20);
    graphSignalS14_3015->GetXaxis()->SetTitle("#it{t} (ns)");
    graphSignalS14_3015->GetYaxis()->SetTitle("#it{A} (a.U.)");
    
    TGraph* DCRvsRad1mm[8];
    TGraph* DCRvsRad3mm[8];
    for (Int_t v= 0; v< 8; v++){
        DCRvsRad1mm[v] = new TGraph(100);
        DCRvsRad3mm[v] = new TGraph(100);
    }
    Int_t nPoints1mm = 0;
    Int_t nPoints3mm = 0;
    // Loop through the list of filenames, create a graph for each, and add it to the TMultiGraph.
    for (size_t i = 0; i < filenamesS14_1315.size(); ++i) {
      std::string title = "File " + std::to_string(i + 1);
      TGraph* graph = createGraphFromFile(Form("%s",filenamesS14_1315[i].c_str()), title.c_str());
      std::cout << filenamesS14_1315[i].c_str() << std::endl;
      Int_t j = -1;
      if (graph) {
          // Set properties for the graph.
          if (labelsS14_1315[i].BeginsWith("A")) j = 0; 
          if (labelsS14_1315[i].BeginsWith("B")) j = 1; 
          if (labelsS14_1315[i].BeginsWith("C")) j = 2; 

          if (labelsS14_1315[i].Contains("10^{0}")) l[j] = 0; 
          if (labelsS14_1315[i].Contains("10^{8}")) l[j] = 1; 
          if (labelsS14_1315[i].Contains("10^{9}")) l[j] = 2; 
          if (labelsS14_1315[i].Contains("10^{10}")) l[j] = 3; 
          if (labelsS14_1315[i].Contains("10^{11}")) l[j] = 4; 
          if (labelsS14_1315[i].Contains("10^{12}")) l[j] = 5; 
          if (labelsS14_1315[i].Contains("10^{13}")) l[j] = 6; 
          SetMarkerDefaultsTGraph(graph, markers[l[j]], 1, colors[j], colors[j], 1, 1, kFALSE, 0, kFALSE);
          
          // Add the graph to the multigraph and the legend.
          graph->Write(Form("%s_%s",labelsSiPM[j].Data(),outNP[l[j]].Data() ),TObject::kOverwrite);
     
          std::string title2 = "File " + std::to_string(i + 1)+"log";
          TGraph* graphLogI = createGraphFromFile(Form("%s",filenamesS14_1315[i].c_str()), title2.c_str(),kTRUE);
          SetMarkerDefaultsTGraph(graphLogI, markers[l[j]], 1, colors[j], colors[j], 1, 1, kFALSE, 0, kFALSE);
          graphLogI->Write(Form("%s_%s_Log",labelsSiPM[j].Data(),outNP[l[j]].Data() ),TObject::kOverwrite);   
          
          TGraph* graphDer = new TGraph(graphLogI->GetN()-2);
          TF1* fitLocal = new TF1("lin","[0]+[1]*x");
          for (Int_t p = 1; p < graphLogI->GetN()-1; p++){
            graphLogI->Fit(fitLocal,"C0NQ", "", graphLogI->GetX()[p-1], graphLogI->GetX()[p+1]);
            graphDer->SetPoint(p-1, graphLogI->GetX()[p-1], fitLocal->GetParameter(1));  
          }
          SetMarkerDefaultsTGraph(graphDer, markers[l[j]], 1, colors[j], colors[j], 1, 1, kFALSE, 0, kFALSE);
          while(graphDer->GetX()[graphDer->GetN()-1] > 42) graphDer->RemovePoint(graphDer->GetN()-1);
          graphDer->Write(Form("%s_%s_Log_Derivative",labelsSiPM[j].Data(),outNP[l[j]].Data() ),TObject::kOverwrite);

          // Find maximum of Derivative curve to determine approximate break down voltage
          Double_t maxY = TMath::MaxElement(graphDer->GetN(),graphDer->GetY());
          Int_t pMax    = 0;
          while (maxY != graphDer->GetY()[pMax] && pMax < graphDer->GetN()) pMax++;
          Double_t maxX = graphDer->GetX()[pMax];
          std::cout << maxX << "\t"<< maxY<< std::endl;              
          // Fixing Vbr for non irradiated SiPMs
          if (j == 0 && l[j] == 0) maxX = 39.35;
          if (j == 0 && l[j] == 1) maxX = 38.54;
          if (j == 0 && l[j] == 2) maxX = 38.54;
          if (j == 1 && l[j] == 2) maxX = 38.54;
          if (j == 2 && l[j] == 2) maxX = 38.54;
          std::string title3 = "File " + std::to_string(i + 1)+"_3";
          TGraph* graphvsVoV = createGraphFromFile(Form("%s",filenamesS14_1315[i].c_str()), title3.c_str(),kFALSE, maxX);
          SetMarkerDefaultsTGraph(graphvsVoV, markers[l[j]], 1, colors[j], colors[j], 1, 1, kFALSE, 0, kFALSE);
          graphvsVoV->Write(Form("%s_%s_VoV",labelsSiPM[j].Data(),outNP[l[j]].Data() ),TObject::kOverwrite);   
          
          for (Int_t v = 0; v < 8; v++){
              Double_t vov  = (Double_t)v*0.5+2;
              Double_t iD   = graphvsVoV->Eval(vov);
              Double_t gain = graphGain->Eval(vov);
              Double_t qe   = 1.602e-19;
              Double_t drc  = iD/(gain*qe);
              std::cout <<"Vov: " << vov << "\t I_D: "<< iD<< "\t gain: "<< gain<< "\t DCR: "<< drc<< std::endl;
              DCRvsRad1mm[v]->SetPoint(nPoints1mm, log10(NP[l[j]]), drc);
          }
          nPoints1mm++;
      }
    }
    graphSignalS14_1315->Write("SignalShape",TObject::kOverwrite);
    graphPhotonEffi->Write("PhotonEfficiencyVsWavelength",TObject::kOverwrite);
    graphGain->Write("GainVsOvervoltage",TObject::kOverwrite);    
    for (Int_t v= 0; v< 8; v++){
      Double_t vov  = (Double_t)v*0.5+2;
      while (DCRvsRad1mm[v]->GetN()> nPoints1mm  ) DCRvsRad1mm[v]->RemovePoint(DCRvsRad1mm[v]->GetN()-1);
      DCRvsRad1mm[v]->Write(Form("DCR_%1.1fV",vov),TObject::kOverwrite);    
      DCRvsRad1mm[v]->Print();
    }
    
    outputRoot->cd();
    outputRoot->mkdir("S14160-3015");
    outputRoot->cd("S14160-3015");
    // Loop through the list of filenames, create a graph for each, and add it to the TMultiGraph.
    for (size_t i = 0; i < filenamesS14_3015.size(); ++i) {
      std::string title = "File " + std::to_string(i + 1);
      TGraph* graph = createGraphFromFile(Form("%s",filenamesS14_3015[i].c_str()), title.c_str());
      std::cout << filenamesS14_3015[i].c_str() << std::endl;
      Int_t j = -1;
      if (graph) {
          // Set properties for the graph.
          if (labelsS14_3015[i].BeginsWith("A")) j = 0; 
          if (labelsS14_3015[i].BeginsWith("B")) j = 1; 
          if (labelsS14_3015[i].BeginsWith("C")) j = 2; 
          if (labelsS14_3015[i].BeginsWith("D")) j = 3; 
          
          if (labelsS14_3015[i].Contains("10^{0}")) l[j] = 0; 
          if (labelsS14_3015[i].Contains("10^{8}")) l[j] = 1; 
          if (labelsS14_3015[i].Contains("10^{9}")) l[j] = 2; 
          if (labelsS14_3015[i].Contains("10^{10}")) l[j] = 3; 
          if (labelsS14_3015[i].Contains("10^{11}")) l[j] = 4; 
          if (labelsS14_3015[i].Contains("10^{12}")) l[j] = 5; 
          if (labelsS14_3015[i].Contains("10^{13}")) l[j] = 6; 
          SetMarkerDefaultsTGraph(graph, markers[l[j]], 1, colors[j], colors[j], 1, 1, kFALSE, 0, kFALSE);
          
          // Add the graph to the multigraph and the legend.
          graph->Write(Form("%s_%s",labelsSiPM[j].Data(),outNP[l[j]].Data() ),TObject::kOverwrite);

          std::string title2 = "File " + std::to_string(i + 1)+"log";
          TGraph* graphLogI = createGraphFromFile(Form("%s",filenamesS14_3015[i].c_str()), title2.c_str(),kTRUE);
          SetMarkerDefaultsTGraph(graphLogI, markers[l[j]], 1, colors[j], colors[j], 1, 1, kFALSE, 0, kFALSE);
          graphLogI->Write(Form("%s_%s_Log",labelsSiPM[j].Data(),outNP[l[j]].Data() ),TObject::kOverwrite);   
          
          TGraph* graphDer = new TGraph(graphLogI->GetN()-2);
          TF1* fitLocal = new TF1("lin","[0]+[1]*x");
          for (Int_t p = 1; p < graphLogI->GetN()-1; p++){
            graphLogI->Fit(fitLocal,"C0NQ", "", graphLogI->GetX()[p-1], graphLogI->GetX()[p+1]);
            graphDer->SetPoint(p-1, graphLogI->GetX()[p-1], fitLocal->GetParameter(1));  
          }
          SetMarkerDefaultsTGraph(graphDer, markers[l[j]], 1, colors[j], colors[j], 1, 1, kFALSE, 0, kFALSE);
          while(graphDer->GetX()[graphDer->GetN()-1] > 42) graphDer->RemovePoint(graphDer->GetN()-1);
          graphDer->Write(Form("%s_%s_Log_Derivative",labelsSiPM[j].Data(),outNP[l[j]].Data() ),TObject::kOverwrite);
        
          // Find maximum of Derivative curve to determine approximate break down voltage
          Double_t maxY = TMath::MaxElement(graphDer->GetN(),graphDer->GetY());
          Int_t pMax    = 0;
          while (maxY != graphDer->GetY()[pMax] && pMax < graphDer->GetN()) pMax++;
          Double_t maxX = graphDer->GetX()[pMax];
          std::cout << maxX << "\t"<< maxY<< std::endl;              
          // Fixing Vbr for non irradiated SiPMs
          if (j == 0 && l[j] == 0) maxX = 38.96;
          if (j == 1 && l[j] == 0) maxX = 38.96;
          if (j == 2 && l[j] == 0) maxX = 38.85;
          if (j == 3 && l[j] == 0) maxX = 38.71;
          std::string title3 = "File " + std::to_string(i + 1)+"_3";
          TGraph* graphvsVoV = createGraphFromFile(Form("%s",filenamesS14_3015[i].c_str()), title3.c_str(),kFALSE, maxX);
          SetMarkerDefaultsTGraph(graphvsVoV, markers[l[j]], 1, colors[j], colors[j], 1, 1, kFALSE, 0, kFALSE);
          graphvsVoV->Write(Form("%s_%s_VoV",labelsSiPM[j].Data(),outNP[l[j]].Data() ),TObject::kOverwrite);   
          
           for (Int_t v = 0; v < 8; v++){
              Double_t vov  = (Double_t)v*0.5+2;
              Double_t iD   = graphvsVoV->Eval(vov);
              Double_t gain = graphGain->Eval(vov);
              Double_t qe   = 1.602e-19;
              Double_t drc  = iD/(gain*qe);
              std::cout <<"Vov: " << vov << "\t I_D: "<< iD<< "\t gain: "<< gain<< "\t DCR: "<< drc<< std::endl;
              DCRvsRad3mm[v]->SetPoint(nPoints3mm, log10(NP[l[j]]), drc);
          }
          nPoints3mm++;
      }
    }
        
    graphSignalS14_3015->Write("SignalShape",TObject::kOverwrite);
    graphPhotonEffi->Write("PhotonEfficiencyVsWavelength",TObject::kOverwrite);
    graphGain->Write("GainVsOvervoltage",TObject::kOverwrite);
    for (Int_t v= 0; v< 8; v++){
      Double_t vov  = (Double_t)v*0.5+2;
      while (DCRvsRad3mm[v]->GetN()> nPoints3mm  ) DCRvsRad3mm[v]->RemovePoint(DCRvsRad3mm[v]->GetN()-1);
      DCRvsRad3mm[v]->Write(Form("DCR_%1.1fV",vov),TObject::kOverwrite);    
      DCRvsRad3mm[v]->Print();
    }

    
    outputRoot->Write();
    outputRoot->Close();
}
