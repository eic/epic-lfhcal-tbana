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

//=================================================================================
// The main function to execute the plotting.
//=================================================================================
void plotDataSiPMIrrad( TString rootFileName = "") {

    // Create a TMultiGraph to hold all the individual graphs.
    TMultiGraph* mg1mm      = new TMultiGraph();
    TMultiGraph* mg1mmVoV   = new TMultiGraph();
    TMultiGraph* mg1mmVPeak = new TMultiGraph();
    TMultiGraph* mg3mm      = new TMultiGraph();
    TMultiGraph* mg3mmVoV   = new TMultiGraph();
    TMultiGraph* mg3mmVPeak = new TMultiGraph();
    TMultiGraph* mg1mmNP[7];
    TMultiGraph* mg1mmNPVoV[7];
    TMultiGraph* mg3mmNP[7];
    TMultiGraph* mg3mmNPVoV[7];
    for (Int_t k = 0; k < 7; k++){
        mg1mmNP[k]    = new TMultiGraph();
        mg1mmNPVoV[k] = new TMultiGraph();
        mg3mmNP[k]    = new TMultiGraph();
        mg3mmNPVoV[k] = new TMultiGraph();
    }
    TMultiGraph* mg1mmSiPM[4];
    TMultiGraph* mg1mmSiPMVoV[4];
    TMultiGraph* mg3mmSiPM[4];
    TMultiGraph* mg3mmSiPMVoV[4];
    for (Int_t k = 0; k < 4; k++){
        mg1mmSiPM[k]    = new TMultiGraph();
        mg1mmSiPMVoV[k] = new TMultiGraph();
        mg3mmSiPM[k]    = new TMultiGraph();
        mg3mmSiPMVoV[k] = new TMultiGraph();
    }

    // Define an array of colors and markers to use for the plots.
    int colors[] = {kRed+1, kBlue+1, kGreen + 2, kCyan+2}; //, kOrange, kMagenta
    int markers[] = {kFullCircle, kFullSquare, 33, 28, 34, 24, 25, 27, 42, 46, 43, 47, 29, 30};
    
    Int_t l[4] = {-1, -1, -1, -1};
    TString labelsNP[7] =  {"10^{0}", "10^{8}", "10^{9}", "10^{10}", "10^{11}", "10^{12}", "10^{13}"}; 
    TString outNP[7] =  {"10_0", "10_8", "10_9", "10_10", "10_11", "10_12", "10_13"}; 
    
    TString labelsSiPM[4] =  {"A", "B", "C", "D"}; 
    Double_t textSizeRel = 0.035;
  
    // Create a legend to identify the different plots.
    TLegend* legend1mm = GetAndSetLegend2( 0.13, 0.74, 0.44, 0.96,textSizeRel*0.85, 3, "#bf{S14160-1315}", 42,0.2);
    TLegend* legend3mm = GetAndSetLegend2( 0.13, 0.74, 0.44, 0.96,textSizeRel*0.85, 3, "#bf{S14160-3015}", 42,0.2);
      
    TLegend* legend1mmSiPM[4];
    TLegend* legend3mmSiPM[4];
    for (Int_t s = 0; s < 4; s++){
        legend1mmSiPM[s] = GetAndSetLegend2( 0.13, 0.75, 0.34, 0.90,textSizeRel*0.85, 2, "", 42,0.2);
        legend3mmSiPM[s] = GetAndSetLegend2( 0.13, 0.75, 0.34, 0.90,textSizeRel*0.85, 2, "", 42,0.2);
    }
    
    TFile* inputRootFile = new TFile(rootFileName.Data());
    // Loop through the list of filenames, create a graph for each, and add it to the TMultiGraph.
    for (Int_t s = 0; s < 4; s++){
      for (Int_t r = 0; r< 7; r++){
        TGraph* graph = (TGraph*)inputRootFile->Get(Form("S14160-1315/%s_%s",labelsSiPM[s].Data(), outNP[r].Data()));
        if (graph){
          SetMarkerDefaultsTGraph(graph, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg1mm->Add(graph);
          legend1mm->AddEntry(graph, Form("%s %s", labelsSiPM[s].Data(), labelsNP[r].Data() ), "p");
          
          TGraph* graph2 = (TGraph*)graph->Clone(Form("S14160-1315/%s_%s_2",labelsSiPM[s].Data(), outNP[r].Data()));
          SetMarkerDefaultsTGraph(graph2, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg1mmNP[r]->Add(graph2);
          
          TGraph* graph3 = (TGraph*)graph->Clone(Form("S14160-1315/%s_%s_3",labelsSiPM[s].Data(), outNP[r].Data()));
          SetMarkerDefaultsTGraph(graph3, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          legend1mmSiPM[s]->AddEntry(graph, labelsNP[r].Data(), "p");
          mg1mmSiPM[s]->Add(graph3);
        }
        TGraph* graphPeak1mm = (TGraph*)inputRootFile->Get(Form("S14160-1315/%s_%s_Log_Derivative",labelsSiPM[s].Data(), outNP[r].Data()));
        if (graphPeak1mm){
          SetMarkerDefaultsTGraph(graphPeak1mm, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg1mmVPeak->Add(graphPeak1mm);
        }
        TGraph* graphVoV = (TGraph*)inputRootFile->Get(Form("S14160-1315/%s_%s_VoV",labelsSiPM[s].Data(), outNP[r].Data()));
        if (graphVoV){
          SetMarkerDefaultsTGraph(graphVoV, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg1mmVoV->Add(graphVoV);
          
          TGraph* graph2VoV = (TGraph*)graphVoV->Clone(Form("S14160-1315/%s_%s_VoV_2",labelsSiPM[s].Data(), outNP[r].Data()));
          SetMarkerDefaultsTGraph(graph2VoV, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg1mmNPVoV[r]->Add(graph2VoV);
          
          TGraph* graph3VoV = (TGraph*)graphVoV->Clone(Form("S14160-1315/%s_%s_VoV_3",labelsSiPM[s].Data(), outNP[r].Data()));
          SetMarkerDefaultsTGraph(graph3VoV, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg1mmSiPMVoV[s]->Add(graph3VoV);
        }
        
        TGraph* graph3mm = (TGraph*)inputRootFile->Get(Form("S14160-3015/%s_%s",labelsSiPM[s].Data(), outNP[r].Data()));
        if (graph3mm){
          SetMarkerDefaultsTGraph(graph3mm, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg3mm->Add(graph3mm);
          legend3mm->AddEntry(graph3mm, Form("%s %s", labelsSiPM[s].Data(), labelsNP[r].Data() ), "p");
          
          TGraph* graph3mm2 = (TGraph*)graph3mm->Clone(Form("S14160-3015/%s_%s_2",labelsSiPM[s].Data(), outNP[r].Data()));
          SetMarkerDefaultsTGraph(graph3mm2, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg3mmNP[r]->Add(graph3mm2);
          
          TGraph* graph3mm3 = (TGraph*)graph3mm->Clone(Form("S14160-3015/%s_%s_3",labelsSiPM[s].Data(), outNP[r].Data()));
          SetMarkerDefaultsTGraph(graph3mm3, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          legend3mmSiPM[s]->AddEntry(graph3mm, labelsNP[r].Data(), "p");
          mg3mmSiPM[s]->Add(graph3mm3);
        }
        TGraph* graphPeak3mm = (TGraph*)inputRootFile->Get(Form("S14160-3015/%s_%s_Log_Derivative",labelsSiPM[s].Data(), outNP[r].Data()));
        if (graphPeak3mm){
          SetMarkerDefaultsTGraph(graphPeak3mm, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg3mmVPeak->Add(graphPeak3mm);
        }
        TGraph* graph3mmVoV = (TGraph*)inputRootFile->Get(Form("S14160-3015/%s_%s_VoV",labelsSiPM[s].Data(), outNP[r].Data()));
        if (graph3mmVoV){
          SetMarkerDefaultsTGraph(graph3mmVoV, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg3mmVoV->Add(graph3mmVoV);
          
          TGraph* graph3mmVoV2 = (TGraph*)graph3mmVoV->Clone(Form("S14160-3015/%s_%s_VoV_2",labelsSiPM[s].Data(), outNP[r].Data()));
          SetMarkerDefaultsTGraph(graph3mmVoV2, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg3mmNPVoV[r]->Add(graph3mmVoV2);
          
          TGraph* graph3mmVoV3 = (TGraph*)graph3mmVoV->Clone(Form("S14160-3015/%s_%s_VoV_3",labelsSiPM[s].Data(), outNP[r].Data()));
          SetMarkerDefaultsTGraph(graph3mmVoV3, markers[r], 1, colors[s], colors[s], 1, 1, kFALSE, 0, kFALSE);
          mg3mmSiPMVoV[s]->Add(graph3mmVoV3);
        } 
      }
    }
    
    //*************************************************
    // Combined plot for all S14160-1315
    //*************************************************
    TCanvas* canvas = new TCanvas("canvas", "", 800, 600);
    DefaultCancasSettings(canvas, 0.075, 0.01, 0.01, 0.075);
    canvas->SetLogy();
      // Set multigraph properties (e.g., title, axis labels).
      SetStyleMultiGraph( mg1mm, "#it{V}_{op} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
      // Draw the TMultiGraph.
      mg1mm->Draw("APL");
      // Draw the legend.
      legend1mm->Draw();
    canvas->Update();
    canvas->SaveAs("figsIrrad/S14160_1315_combined_plot.pdf");
    canvas->SetLogy();
      // Set multigraph properties (e.g., title, axis labels).
      SetStyleMultiGraph( mg1mmVoV, "#it{V}_{ov} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
      // Draw the TMultiGraph.
      mg1mmVoV->Draw("APL");
      // Draw the legend.
      legend1mm->Draw();
    canvas->Update();
    canvas->SaveAs("figsIrrad/S14160_1315_combined_plot_VoV.pdf");
    canvas->SetLogy(kFALSE);
      mg1mmVPeak->GetYaxis()->SetRangeUser(0,10);
      // Set multigraph properties (e.g., title, axis labels).
      SetStyleMultiGraph( mg1mmVPeak, "#it{V}_{op} (V)", "d(ln(#it{I}_{D})/dV", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
      // Draw the TMultiGraph.
      mg1mmVPeak->Draw("APL");
      // Draw the legend.
      legend1mm->Draw();
    canvas->Update();
    canvas->SaveAs("figsIrrad/S14160_1315_combined_plot_Derivative.pdf");

    //*************************************************
    // Combined plot for all S14160-1315
    //*************************************************
   canvas->SetLogy();
      SetStyleMultiGraph( mg3mm, "#it{V}_{op} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
      // Draw the TMultiGraph.
      mg3mm->Draw("APL");
      // Draw the legend.
      legend3mm->Draw();
    canvas->Update();
    canvas->SaveAs("figsIrrad/S14160_3015_combined_plot.pdf");
    canvas->SetLogy();
      SetStyleMultiGraph( mg3mmVoV, "#it{V}_{ov} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
      // Draw the TMultiGraph.
      mg3mmVoV->Draw("APL");
      // Draw the legend.
      legend3mm->Draw();
    canvas->Update();
    canvas->SaveAs("figsIrrad/S14160_3015_combined_plot_VoV.pdf");
    canvas->SetLogy(kFALSE);  
      // Set multigraph properties (e.g., title, axis labels).
      mg3mmVPeak->GetYaxis()->SetRangeUser(0,10);
      SetStyleMultiGraph( mg3mmVPeak, "#it{V}_{op} (V)", "d(ln(#it{I}_{D})/dV", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
      // Draw the TMultiGraph.
      mg3mmVPeak->Draw("APL");
      // Draw the legend.
      legend3mm->Draw();
    canvas->Update();
    canvas->SaveAs("figsIrrad/S14160_3015_combined_plot_Derivative.pdf");

    //*************************************************
    // Combined plot per radiation dose
    //*************************************************
    TCanvas* canvas2 = new TCanvas("canvas2", "", 800, 600);
    DefaultCancasSettings(canvas2, 0.075, 0.01, 0.01, 0.075);
    canvas2->SetLogy();
    
    for (Int_t k = 0; k < 7; k++){
        canvas2->cd();
        std::cout << "k\t" << k << std::endl;
        if (mg1mmNP[k]->GetListOfGraphs()){
          SetStyleMultiGraph( mg1mmNP[k], "#it{V}_{op} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
          mg1mmNP[k]->Draw("APL");
          DrawLatex(0.13, 0.92, Form("S14160-1315 %s  N_{p^{+}}",labelsNP[k].Data()), false, 0.044, 42);
          canvas2->Update();
          canvas2->SaveAs(Form("figsIrrad/S14160_1315_plot_%s.pdf",outNP[k].Data()));
        }
        if (mg1mmNPVoV[k]->GetListOfGraphs()){
          SetStyleMultiGraph( mg1mmNPVoV[k], "#it{V}_{ov} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
          mg1mmNPVoV[k]->Draw("APL");
          DrawLatex(0.13, 0.92, Form("S14160-1315 %s  N_{p^{+}}",labelsNP[k].Data()), false, 0.044, 42);
          canvas2->Update();
          canvas2->SaveAs(Form("figsIrrad/S14160_1315_plot_%s_VoV.pdf",outNP[k].Data()));
        }
        if (mg3mmNP[k]->GetListOfGraphs()){
          SetStyleMultiGraph( mg3mmNP[k], "#it{V}_{op} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
          mg3mmNP[k]->Draw("APL");
          DrawLatex(0.13, 0.92, Form("S14160-3015 %s  N_{p^{+}}",labelsNP[k].Data()), false, 0.044, 42);
          canvas2->Update();
          canvas2->SaveAs(Form("figsIrrad/S14160_3015_plot_%s.pdf",outNP[k].Data()));        
        }
        if (mg3mmNPVoV[k]->GetListOfGraphs()){
          SetStyleMultiGraph( mg3mmNPVoV[k], "#it{V}_{ov} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
          mg3mmNPVoV[k]->Draw("APL");
          DrawLatex(0.13, 0.92, Form("S14160-3015 %s  N_{p^{+}}",labelsNP[k].Data()), false, 0.044, 42);
          canvas2->Update();
          canvas2->SaveAs(Form("figsIrrad/S14160_3015_plot_%s_VoV.pdf",outNP[k].Data()));        
        }
    }

    //*************************************************
    // Combined plot per SiPM 
    //*************************************************    
    TCanvas* canvas3 = new TCanvas("canvas3", "", 800, 600);
    DefaultCancasSettings(canvas3, 0.075, 0.01, 0.01, 0.075);
    canvas3->SetLogy();
    
    for (Int_t s = 0; s < 4; s++){
        canvas3->cd();
        std::cout << "s\t" << s << std::endl;
        if (mg1mmSiPM[s]->GetListOfGraphs()){
          SetStyleMultiGraph( mg1mmSiPM[s], "#it{V}_{op} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
          mg1mmSiPM[s]->Draw("APL");
          // Draw the legend.
          DrawLatex(0.13, 0.92, Form("S14160-1315 %s",labelsSiPM[s].Data()), false, 0.044, 42);
          legend1mmSiPM[s]->Draw();
          canvas3->SaveAs(Form("figsIrrad/S14160_1315_plot_%s.pdf",labelsSiPM[s].Data()));
        } 
        if (mg1mmSiPMVoV[s]->GetListOfGraphs()){
          SetStyleMultiGraph( mg1mmSiPMVoV[s], "#it{V}_{ov} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
          mg1mmSiPMVoV[s]->Draw("APL");
          // Draw the legend.
          DrawLatex(0.13, 0.92, Form("S14160-1315 %s",labelsSiPM[s].Data()), false, 0.044, 42);
          legend1mmSiPM[s]->Draw();
          canvas3->SaveAs(Form("figsIrrad/S14160_1315_plot_%s_VoV.pdf",labelsSiPM[s].Data()));
        } 
        if (mg3mmSiPM[s]->GetListOfGraphs()){
          SetStyleMultiGraph( mg3mmSiPM[s], "#it{V}_{op} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
          mg3mmSiPM[s]->Draw("APL");
          // Draw the legend.
          DrawLatex(0.13, 0.92, Form("S14160-3015 %s",labelsSiPM[s].Data()), false, 0.044, 42);
          legend3mmSiPM[s]->Draw();
          canvas3->SaveAs(Form("figsIrrad/S14160_3015_plot_%s.pdf",labelsSiPM[s].Data()));
        } 
        if (mg3mmSiPMVoV[s]->GetListOfGraphs()){
          SetStyleMultiGraph( mg3mmSiPMVoV[s], "#it{V}_{ov} (V)", "#it{I}_{D} (A)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
          mg3mmSiPMVoV[s]->Draw("APL");
          // Draw the legend.
          DrawLatex(0.13, 0.92, Form("S14160-3015 %s",labelsSiPM[s].Data()), false, 0.044, 42);
          legend3mmSiPM[s]->Draw();
          canvas3->SaveAs(Form("figsIrrad/S14160_3015_plot_%s_VoV.pdf",labelsSiPM[s].Data()));
        } 
    }
    
    Float_t VoV[8]    = {2.0, 2.5, 3., 3.5, 4.0, 4.5, 5.0, 5.5};
    int colorsVoV[8]  = {kGray+1, kGray+2, kRed+1, kBlue+1, kGreen+2, kCyan+2, kBlue-8, kBlue-5};
    int markersVoV[8] = {24, 25, kFullCircle, kFullSquare, 33, 34, 28,  27}; //, 42, 46, 43, 47, 29, 30};

    TLegend* legendDCR1mm = GetAndSetLegend2( 0.13, 0.84, 0.44, 0.96,textSizeRel*0.85, 2, "#bf{S14160-1315}", 42,0.2);
    // TLegend* legendDCR1mm = GetAndSetLegend2( 0.13, 0.74, 0.44, 0.96,textSizeRel*0.85, 2, "#bf{S14160-1315}", 42,0.2);
    TLegend* legendDCR3mm = GetAndSetLegend2( 0.13, 0.84, 0.44, 0.96,textSizeRel*0.85, 2, "#bf{S14160-3015}", 42,0.2);
    // TLegend* legendDCR3mm = GetAndSetLegend2( 0.13, 0.74, 0.44, 0.96,textSizeRel*0.85, 2, "#bf{S14160-3015}", 42,0.2);
    
    TGraph* graphDCR1mm[8];
    TGraph* graphDCR3mm[8];
    TMultiGraph* mgDCR1mm   = new TMultiGraph();
    TMultiGraph* mgDCR3mm   = new TMultiGraph();
    for (Int_t v = 0; v< 8; v++){
      graphDCR1mm[v] = (TGraph*)inputRootFile->Get(Form("S14160-1315/DCR_%1.1fV", VoV[v]));
      graphDCR1mm[v]->Sort();
      graphDCR1mm[v]->Scale(1/1e6); // Convert to MHz
      SetMarkerDefaultsTGraph(graphDCR1mm[v], markersVoV[v], 1, colorsVoV[v], colorsVoV[v], 1, 1, kFALSE, 0, kFALSE);
      if (v == 0 || v == 2 || v == 4 || v == 6){
        legendDCR1mm->AddEntry(graphDCR1mm[v], Form("%1.1f V", VoV[v]), "p");
        mgDCR1mm->Add(graphDCR1mm[v]);
      }
      graphDCR1mm[v]->Print();
      
      graphDCR3mm[v] = (TGraph*)inputRootFile->Get(Form("S14160-3015/DCR_%1.1fV", VoV[v]));
      graphDCR3mm[v]->Sort();
      graphDCR3mm[v]->Scale(1/1e6); // Convert to MHz
      SetMarkerDefaultsTGraph(graphDCR3mm[v], markersVoV[v], 1, colorsVoV[v], colorsVoV[v], 1, 1, kFALSE, 0, kFALSE);
      if (v == 0 || v == 2 || v == 4 || v == 6){
        legendDCR3mm->AddEntry(graphDCR3mm[v], Form("%1.1f V", VoV[v]), "p");
        mgDCR3mm->Add(graphDCR3mm[v]);
      }
      graphDCR3mm[v]->Print();
    }

    
    TCanvas* canvas4 = new TCanvas("canvas4", "", 800, 600);
    DefaultCancasSettings(canvas4, 0.075, 0.01, 0.01, 0.075);
        
    mgDCR1mm->GetYaxis()->SetRangeUser(1e5,1e12); 
    
    canvas4->SetLogy();
      SetStyleMultiGraph( mgDCR1mm, "log(1 MEQ Neutron Fluence) (cm^{-2})", "#it{DCR} (MHz)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
      // Draw the TMultiGraph.
      mgDCR1mm->Draw("AP");
      // Draw the legend.
      legendDCR1mm->Draw();
    canvas4->Update();
    canvas4->SaveAs("figsIrrad/DCR_S14160_1315.pdf");
    
    // mgDCR1mm->GetYaxis()->SetRangeUser(1e5,1e12); 
    mgDCR1mm->GetYaxis()->SetRangeUser(1e-1,3e6); 
    mgDCR1mm->GetXaxis()->SetRangeUser(7,14); 
      // SetStyleMultiGraph( mgDCR1mm, "log(1 MEQ Neutron Fluence) (cm^{-2})", "#it{DCR} (Hz)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
      // Draw the TMultiGraph.
      mgDCR1mm->Draw("AP");
      // Draw the legend.
      legendDCR1mm->Draw();
    canvas4->Update();
    canvas4->SaveAs("figsIrrad/DCR_S14160_1315_zoomed.pdf");

    // mgDCR3mm->GetYaxis()->SetRangeUser(1e5,1e12); 
    mgDCR3mm->GetYaxis()->SetRangeUser(1e-1,3e6); 
    canvas4->SetLogy();
      SetStyleMultiGraph( mgDCR3mm, "log(1 MEQ Neutron Fluence) (cm^{-2})", "#it{DCR} (MHz)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
      // Draw the TMultiGraph.
      mgDCR3mm->Draw("AP");
      // Draw the legend.
      legendDCR3mm->Draw();
    canvas4->Update();
    canvas4->SaveAs("figsIrrad/DCR_S14160_3015.pdf");
    mgDCR3mm->GetXaxis()->SetRangeUser(7,14); 
      // SetStyleMultiGraph( mgDCR3mm, "log(1 MEQ Neutron Fluence) (cm^{-2})", "#it{DCR} (Hz)", 0.85*textSizeRel, textSizeRel, 0.85*textSizeRel, textSizeRel, 1., 1.1, 510, 510, 42, 62);
      // Draw the TMultiGraph.
      mgDCR3mm->Draw("AP");
      // Draw the legend.
      legendDCR3mm->Draw();
    canvas4->Update();
    canvas4->SaveAs("figsIrrad/DCR_S14160_3015_zoomed.pdf");
  
}
