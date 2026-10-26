#include "ShapeAnalyzer.h"
#include <vector>
#include "TROOT.h"
//#include <unistd.h> // Add for use on Mac OS -> Same goes for Analyse.cc
#include "TF1.h"
#include "TFitResult.h"
#include "TFitResultPtr.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3F.h"
#include "TPolyLine3D.h"
#include "TPaletteAxis.h"
#include "TProfile.h"
#include "TChain.h"
#include "CommonHelperFunctions2.h"
#include "TileSpectra.h"
#include "PlotHelper.h"
#include "MultiCanvas.h"
#include "TRandom3.h"
#include "TMath.h"
#include "Math/Minimizer.h"
#include "Math/MinimizerOptions.h"

// ****************************************************************************
// Checking and opening input and output files
// ****************************************************************************
bool ShapeAnalyzer::CheckAndOpenIO(void){
  int matchingbranch;

  //Need to check first input to get the setup...I do not think it is necessary
  std::cout<<"Input name set to: '"<<RootInputName.Data() <<std::endl;  
  if(!RootInputName.IsNull()){
    //File exist?
    RootInput=new TFile(RootInputName.Data(),"READ");
    if(RootInput->IsZombie()){
      std::cout<<"Error opening '"<<RootInputName<<"', does the file exist?"<<std::endl;
      return false;
    }

    //Retrieve info, start with setup
    TsetupIn = (TTree*) RootInput->Get("Setup");
    if(!TsetupIn){
      std::cout<<"Could not retrieve the Setup tree, leaving"<<std::endl;
      return false;
    }
    setup=Setup::GetInstance();
    std::cout<<"Setup add "<<setup<<std::endl;
    //matchingbranch=TsetupIn->SetBranchAddress("setup",&setup);
    matchingbranch=TsetupIn->SetBranchAddress("setup",&rswptr);
    if(matchingbranch<0){
      std::cout<<"Error retrieving Setup info from the tree"<<std::endl;
      return false;
    }
    std::cout<<"Entries "<<TsetupIn->GetEntries()<<std::endl;
    TsetupIn->GetEntry(0);
    setup->Initialize(*rswptr);
    std::cout<<"Reading "<<RootInput->GetName()<<std::endl;
    std::cout<<"Setup Info "<<setup->IsInit()<<"  and  "<<setup->GetCellID(0,0)<<std::endl;
    //std::cout<<"Setup add now "<<setup<<std::endl;

    //Retrieve info, existing data?
    TdataIn = (TTree*) RootInput->Get("Data");
    if(!TdataIn){
      std::cout<<"Could not retrieve the Data tree, leaving"<<std::endl;
      return false;
    }
    matchingbranch=TdataIn->SetBranchAddress("event",&eventptr);
    if(matchingbranch<0){
      std::cout<<"Error retrieving Event info from the tree"<<std::endl;
      return false;
    }
    
    TcalibIn = (TTree*) RootInput->Get("Calib");
    if(!TcalibIn){
      std::cout<<"Could not retrieve Calib tree, leaving"<<std::endl;
      return false;
    }
    else {
      matchingbranch=TcalibIn->SetBranchAddress("calib",&calibptr);
      if(matchingbranch<0){
        std::cout<<"Error retrieving calibration info from the tree"<<std::endl;
        TcalibIn=nullptr;
      } else {
        std::cout<<"Loaded the calib object"<<std::endl;
      }
    }
  } else {
    std::cout<<"An input file is required, aborting"<<std::endl;
    return false;
  }  

  return true;
}

// ****************************************************************************
// Primary process function to start all calibrations
// ****************************************************************************
bool ShapeAnalyzer::Process(void){
  bool status;
  ROOT::EnableImplicitMT();
  
  // copy full calibration to different file and calculate energy
  status=Plot();

  return status;
}


TH2D* ShapeAnalyzer::Normalize2D(TH2D* hist){

  int xnbins = hist->GetXaxis()->GetNbins();
  int ynbins = hist->GetYaxis()->GetNbins();
  double factor = 0;
  double bincontent = 0;
  for(int x = 1; x <= xnbins; x++){
    factor = hist->Integral(x,x,1,-1);
    for(int y = 1; y <= ynbins; y++){
      bincontent = hist->GetBinContent(x,y);
      hist->SetBinContent(x,y,bincontent/factor);
    }
  }
  
  return hist;
}

/*
bool ShapeAnalyzer::PassMuonCuts(Cluster clust){

  if(clust.hitTowers < 1.1){
  return true;
  }
  return false;

    
}
*/

void ShapeAnalyzer::Plot1D(TH1D* hist,std::string Name, int sample){

  std::cout << "plotting 1D: " << Name << std::endl;
  
  TString outputDirPlots = GetPlotOutputDir();
	  double maximum = hist->GetMaximum();
	  hist->GetYaxis()->SetRangeUser(maximum*0.00001,maximum*10);
	  TCanvas* Canvas1D = new TCanvas(Name.c_str(),Name.c_str());
	  Canvas1D->SetLeftMargin(0.15);
	  Canvas1D->SetBottomMargin(0.15);
	  Canvas1D->SetRightMargin(0.05);
	  Canvas1D->SetTopMargin(0.05);
	    

	  
	  Canvas1D->cd();
	  Canvas1D->SetLogy();
	  hist->GetXaxis()->SetTitle(Name.c_str());
	  hist->Draw("p E1");
	  if(sample == 0){
	    Canvas1D->SaveAs( Form("%s/Inclusive/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
	  }
	  if(sample == 1){
	    Canvas1D->SaveAs( Form("%s/Muon/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
	  }
	  if(sample == 2){
	    Canvas1D->SaveAs( Form("%s/nonMuon/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
	  }
	  Canvas1D->ResetDrawn();
}

void ShapeAnalyzer::Plot1D_Compare(TH1D* hist,std::string Name, TH1D* hist2, std::string Name2, int sample){

  std::cout << "plotting 1D Comparison: " << Name << " " << Name2<< std::endl;
  TString outputDirPlots = GetPlotOutputDir();
	  double maximum = hist->GetMaximum();
	  hist->GetYaxis()->SetRangeUser(maximum*0.00001,maximum*10);
	  TCanvas* Canvas1D = new TCanvas(Name.c_str(),Name.c_str());
	  
	  Canvas1D->SetLeftMargin(0.15);
	  Canvas1D->SetBottomMargin(0.15);
	  Canvas1D->SetRightMargin(0.05);
	  Canvas1D->SetTopMargin(0.05);
	  Canvas1D->cd();
	  Canvas1D->SetLogy();
	  hist->GetXaxis()->SetTitle(Name.c_str());
	  hist->SetLineColor(kBlue);
	  hist->Draw("p E1");
	  hist2->SetLineColor(kRed);
	  hist2->Draw("p E1 same");

	  TLegend* CompareL = new TLegend();
	  CompareL->SetBorderSize(0);
	  CompareL->AddEntry(hist,Name.c_str());
	  CompareL->AddEntry(hist2,Name2.c_str());
	  CompareL->Draw();
	  if(sample == 0){
	  Canvas1D->SaveAs( Form("%s/Comparisons/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
	  }
	  if(sample == 1){
	  Canvas1D->SaveAs( Form("%s/Clustering/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
	  }
	  
	  Canvas1D->ResetDrawn();
}
void ShapeAnalyzer::Plot1D_NonLog(TH1D* hist,std::string Name, int sample){

  std::cout << "plotting 1D NonLog: " << Name << std::endl;
  
  TString outputDirPlots = GetPlotOutputDir();
	  double maximum = hist->GetMaximum();
	  hist->GetYaxis()->SetRangeUser(0,1);
	  TCanvas* Canvas1D = new TCanvas(Name.c_str(),Name.c_str());
	  
	  Canvas1D->SetLeftMargin(0.15);
	  Canvas1D->SetBottomMargin(0.15);
	  Canvas1D->SetRightMargin(0.05);
	  Canvas1D->SetTopMargin(0.05);
	  Canvas1D->cd();
	  hist->GetXaxis()->SetTitle(Name.c_str());
	  hist->Draw("p E1");
	  if(sample == 0){
	    Canvas1D->SaveAs( Form("%s/Inclusive/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
	  }
	  if(sample == 1){
	    Canvas1D->SaveAs( Form("%s/Muon/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
	  }
	  if(sample == 2){
	    Canvas1D->SaveAs( Form("%s/nonMuon/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
	  }
	  Canvas1D->ResetDrawn();
}
void ShapeAnalyzer::Plot2D(TH2D* hist,std::string Name, int sample){
  std::cout << "Plotting 2D: " << Name << std::endl;
  
  TString outputDirPlots = GetPlotOutputDir();
	  double maximum = hist->GetMaximum();
	  hist->GetZaxis()->SetRangeUser(maximum*0.00001,maximum*10);
	  TCanvas* Canvas2D = new TCanvas(Name.c_str(),Name.c_str());
	  Canvas2D->SetLeftMargin(0.15);
	  Canvas2D->SetBottomMargin(0.15);
	  Canvas2D->SetRightMargin(0.15);
	  Canvas2D->SetTopMargin(0.05);
  Canvas2D->cd();
  Canvas2D->SetLogz();
  hist->Draw("colz");
  hist->GetXaxis()->SetTitle("Energy");
  hist->GetYaxis()->SetTitle(Name.c_str());
  if(sample == 0){
  Canvas2D->SaveAs( Form("%s/Inclusive/Correlations/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
  }
  if(sample == 1){
  Canvas2D->SaveAs( Form("%s/Muon/Correlations/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
  }
  if(sample == 2){
  Canvas2D->SaveAs( Form("%s/nonMuon/Correlations/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
  }
  if(sample == 3){
  Canvas2D->SaveAs( Form("%s/Inclusive/Correlations_AltCluster/EventDisplay_%s.%s", outputDirPlots.Data(),Name.c_str(), plotSuffix.Data()));
  }
  Canvas2D->ResetDrawn();
}


//***********************************************************************************************
//*********************** Plotting routine ***************************************************
//***********************************************************************************************
bool ShapeAnalyzer::Plot(){
  std::cout<<"Plotting events "<<plotEvt<<" to " << plotEvt+nEvts-1<<std::endl;
  if(plotMuonEvts) std::cout<<"Plotting muon-triggered events"<<std::endl;

  std::map<int,RunInfo> ri=readRunInfosFromFile(RunListInputName.Data(),debug,0);
  TdataIn->GetEntry(0);
  Int_t runNr = event.GetRunNumber();
  // Get run info object
  std::map<int,RunInfo>::iterator it=ri.find(runNr);

  // load calib
  TcalibIn->GetEntry(0);
  
  int species = -1;
  species = GetSpeciesIntFromRunInfo(it->second);
  if (species == -1){
      std::cout << "WARNING: species unknown: " << it->second.species.Data() << "  aborting"<< std::endl;
      return false;
  }
  
  std::cout << "debug level set to : " << debug << std::endl;
  
  // create 3D histo
  // int towersx         = setup->GetNMaxColumn()+1;/*GetNMAxColumn() returns */
  float towersx_min   = setup->GetMinX();
  float towersx_max   = setup->GetMaxX();
  int towersx         = (towersx_max-towersx_min)/setup->GetCellWidth();
  float towersy_min   = setup->GetMinY();
  float towersy_max   = setup->GetMaxY();
  int towersy         = (towersy_max-towersy_min)/setup->GetCellHeight();
  float towersz_min   = setup->GetMinZ();
  float towersz_max   = setup->GetMaxZ();
  int towersz         = (towersz_max-towersz_min)/setup->GetCellDepth();

  int windowsize = 3;
  int minwindow = 20;

  
    //CLUSTER HERE
  
    std::vector<Cluster> ClusterVec;




  TH1D* Cells_energy = new TH1D("Cells_energy","",100,0,300);
  TH1D* SeedCells_energy = new TH1D("SeedCells_energy","",100,0,300);
  TH1D* Towers_energy = new TH1D("Towers_energy","",100,0,300);
  TH1D* SeedTowers_energy = new TH1D("SeedTowers_energy","",100,0,300);
    
  ///
    //2D Shape Plots
  TH2D* ClustersXY = new TH2D("ClustersXY","",towersx*10,0.5,0.5+towersx,towersy*10,0.5,0.5+towersy);
 TH2D* SubClustersXY = new TH2D("SubClustersXY","",100,-3,3,100,-3,3);
  TH2D* MaxCellXY = new TH2D("MaxCellXY","",100,0.5,0.5+towersx,100,0.5,0.5+towersy);

  
  TH1D* Clusters_firstZ = new TH1D("Clusters_firstZ","",towersz,0.5,towersz+0.5);
  TH1D* Clusters_lastZ = new TH1D("Clusters_lastZ","",towersz,0.5,towersz+0.5);
  TH1D* Clusters_maxZ = new TH1D("ClustersZ_max","",towersz,0.5,towersz+0.5);
  TH1D* Clusters_sizeZ = new TH1D("Clusters_sizeZ","",towersz,0.5,8.5);
  TH1D* Clusters_energy = new TH1D("Clusters_energy","",50,0,1000);
  TH1D* Clusters_energy_first = new TH1D("Clusters_energy_first","",50,0,1000);
  TH1D* Clusters_energy_second = new TH1D("Clusters_energy_second","",50,0,1000);
  TH1D* Clusters_NClusters = new TH1D("Clusters_NClusters","",10,0.5,10.5);
  TH1D* Clusters_eventenergy = new TH1D("Clusters_eventenergy","",50,0,1000);
  TH1D* Clusters_windowminuseventenergy = new TH1D("Clusters_windowminuseventenergy","",50,0,1000);
  TH1D* Clusters_spreadX = new TH1D("Clusters_spreadX","",12,-0.5,11.5);
  TH1D* Clusters_Imbalance = new TH1D("Clusters_imbalance","",50,0,1);
  TH1D* Clusters_Z_Imbalance = new TH1D("Clusters_z_imbalance","",50,0,1);
  TH1D* Clusters_Z_HighestCell = new TH1D("Clusters_z_highestcell","",50,0,1);
  //2D Energy Correlation Plots
  TH2D* Clusters_Energy_Imbalance = new TH2D("Clusters_Energy_Imbalance","",50,0,1000,50,0,1);
  TH2D* Clusters_Energy_SeedCellsEnergy = new TH2D("Clusters_Energy_SeedCellsEnergy","",50,0,1000,100,0,300);
  TH2D* Clusters_Energy_MaxZ = new TH2D("Clusters_Energy_MaxZ","",50,0,1000,towersz,0.5,towersz+0.5);
  TH2D* Clusters_Energy_FirstZ = new TH2D("Clusters_Energy_FirstZ","",50,0,1000,towersz,0.5,towersz+0.5);
  TH2D* Clusters_Energy_LastZ = new TH2D("Clusters_Energy_LastZ","",50,0,1000,towersz,0.5,towersz+0.5);
  TH2D* Clusters_Energy_SizeZ = new TH2D("Clusters_Energy_SizeZ","",50,0,1000,towersz,0.5,8.5);
  TH2D* Clusters_Energy_SpreadX = new TH2D("Clusters_Energy_SpreadX","",50,0,1000,12,-0.5,11.5);


  //Muon
  TH2D* ClustersXY_Muon = new TH2D("ClustersXY_Muon","",towersx*10,0.5,0.5+towersx,towersy*10,0.5,0.5+towersy);
  TH2D* SubClustersXY_Muon = new TH2D("SubClustersXY_Muon","",100,-3,3,100,-3,3);
  TH2D* MaxCellXY_Muon = new TH2D("MaxCellXY_Muon","",100,0.5,0.5+towersx,100,0.5,0.5+towersy);
  TH1D* Clusters_firstZ_Muon = new TH1D("Clusters_firstZ_Muon","",towersz,0.5,towersz+0.5);
  TH1D* Clusters_lastZ_Muon = new TH1D("Clusters_lastZ_Muon","",towersz,0.5,towersz+0.5);
  TH1D* Clusters_maxZ_Muon = new TH1D("ClustersZ_max_Muon","",towersz,0.5,towersz+0.5);
  TH1D* Clusters_sizeZ_Muon = new TH1D("Clusters_sizeZ_Muon","",towersz,0.5,8.5);
  TH1D* Clusters_energy_Muon = new TH1D("Clusters_energy_Muon","",50,0,1000);
  TH1D* Clusters_spreadX_Muon = new TH1D("Clusters_spreadX_Muon","",12,-0.5,11.5);
  TH1D* Clusters_Imbalance_Muon = new TH1D("Clusters_imbalance_Muon","",50,0,1);
  TH1D* Clusters_Z_Imbalance_Muon = new TH1D("Clusters_z_imbalance_Muon","",50,0,1);
  TH1D* Clusters_Z_HighestCell_Muon = new TH1D("Clusters_z_highestcell_Muon","",50,0,1);
  TH2D* Clusters_Energy_Imbalance_Muon = new TH2D("Clusters_Energy_Imbalance_Muon","",50,0,1000,50,0,1);
  TH2D* Clusters_Energy_MaxZ_Muon = new TH2D("Clusters_Energy_MaxZ_Muon","",50,0,1000,towersz,0.5,towersz+0.5);
  TH2D* Clusters_Energy_FirstZ_Muon = new TH2D("Clusters_Energy_FirstZ_Muon","",50,0,1000,towersz,0.5,towersz+0.5);
  TH2D* Clusters_Energy_LastZ_Muon = new TH2D("Clusters_Energy_LastZ_Muon","",50,0,1000,towersz,0.5,towersz+0.5);
  TH2D* Clusters_Energy_SizeZ_Muon = new TH2D("Clusters_Energy_SizeZ_Muon","",50,0,1000,towersz,0.5,8.5);
  TH2D* Clusters_Energy_SpreadX_Muon = new TH2D("Clusters_Energy_SpreadX_Muon","",50,0,1000,12,-0.5,11.5);

  
  //NonMuon
  TH2D* ClustersXY_nonMuon = new TH2D("ClustersXY_nonMuon","",towersx*10,0.5,0.5+towersx,towersy*10,0.5,0.5+towersy);
  TH2D* SubClustersXY_nonMuon = new TH2D("SubClustersXY_nonMuon","",100,-3,3,100,-3,3);
  TH2D* MaxCellXY_nonMuon = new TH2D("MaxCellXY_nonMuon","",100,0.5,0.5+towersx,100,0.5,0.5+towersy);
  TH1D* Clusters_firstZ_nonMuon = new TH1D("Clusters_firstZ_nonMuon","",towersz,0.5,towersz+0.5);
  TH1D* Clusters_lastZ_nonMuon = new TH1D("Clusters_lastZ_nonMuon","",towersz,0.5,towersz+0.5);
  TH1D* Clusters_maxZ_nonMuon = new TH1D("ClustersZ_max_nonMuon","",towersz,0.5,towersz+0.5);
  TH1D* Clusters_sizeZ_nonMuon = new TH1D("Clusters_sizeZ_nonMuon","",towersz,0.5,8.5);
  TH1D* Clusters_energy_nonMuon = new TH1D("Clusters_energy_nonMuon","",50,0,1000);
  TH1D* Clusters_spreadX_nonMuon = new TH1D("Clusters_spreadX_nonMuon","",12,-0.5,11.5);
  TH1D* Clusters_Imbalance_nonMuon = new TH1D("Clusters_imbalance_nonMuon","",50,0,1);
  TH1D* Clusters_Z_Imbalance_nonMuon = new TH1D("Clusters_z_imbalance_nonMuon","",50,0,1);
  TH1D* Clusters_Z_HighestCell_nonMuon = new TH1D("Clusters_z_highestcell_nonMuon","",50,0,1);
  TH2D* Clusters_Energy_Imbalance_nonMuon = new TH2D("Clusters_Energy_Imbalance_nonMuon","",50,0,1000,50,0,1);
  TH2D* Clusters_Energy_MaxZ_nonMuon = new TH2D("Clusters_Energy_MaxZ_nonMuon","",50,0,1000,towersz,0.5,towersz+0.5);
  TH2D* Clusters_Energy_FirstZ_nonMuon = new TH2D("Clusters_Energy_FirstZ_nonMuon","",50,0,1000,towersz,0.5,towersz+0.5);
  TH2D* Clusters_Energy_LastZ_nonMuon = new TH2D("Clusters_Energy_LastZ_nonMuon","",50,0,1000,towersz,0.5,towersz+0.5);
  TH2D* Clusters_Energy_SizeZ_nonMuon = new TH2D("Clusters_Energy_SizeZ_nonMuon","",50,0,1000,towersz,0.5,8.5);
  TH2D* Clusters_Energy_SpreadX_nonMuon = new TH2D("Clusters_Energy_SpreadX_nonMuon","",50,0,1000,12,-0.5,11.5);

  TH1D* MuonPurity_Energy = new TH1D("MuonPurity_energy","",50,0,1000);

  
  
  
  TH3F*   hXYZMapEvt          = new TH3F("hXYZMapEvt","",towersz, towersz_min, towersz_max, towersx, towersx_min, towersx_max, towersy, towersy_min, towersy_max);
  TH3F*   hXYZMapEvt_Muon     = new TH3F("hXYZMapEvt_Muon","",towersz, towersz_min, towersz_max, towersx, towersx_min, towersx_max, towersy, towersy_min, towersy_max);
  TH3F*   hXYZMapEvt_nonMuon  = new TH3F("hXYZMapEvt_nonMuon","",towersz, towersz_min, towersz_max, towersx, towersx_min, towersx_max, towersy, towersy_min, towersy_max);

  

  TH1D* hX_energy_Evt    = new TH1D("hXenergyEvt","",towersx, towersx_min, towersx_max);
  hX_energy_Evt->Sumw2();
  TH1D* hY_energy_Evt    = new TH1D("hYenergyEvt","",towersy, towersy_min, towersy_max);
  hY_energy_Evt->Sumw2();
  TH1D* hZ_energy_Evt    = new TH1D("hZenergyEvt","",towersz, towersz_min, towersz_max);
  hZ_energy_Evt->Sumw2();

  TH1D* hX_energy_Evt_Muon    = new TH1D("hXenergyEvt_muon","",towersx, towersx_min, towersx_max);
  hX_energy_Evt_Muon->Sumw2();
  TH1D* hY_energy_Evt_Muon    = new TH1D("hYenergyEvt_muon","",towersy, towersy_min, towersy_max);
  hY_energy_Evt_Muon->Sumw2();
  TH1D* hZ_energy_Evt_Muon    = new TH1D("hZenergyEvt_muon","",towersz, towersz_min, towersz_max);
  hZ_energy_Evt_Muon->Sumw2();

  TH1D* hX_energy_Evt_nonMuon    = new TH1D("hXenergyEvt_nonMuon","",towersx, towersx_min, towersx_max);
  hX_energy_Evt_nonMuon->Sumw2();
  TH1D* hY_energy_Evt_nonMuon    = new TH1D("hYenergyEvt_nonMuon","",towersy, towersy_min, towersy_max);
  hY_energy_Evt_nonMuon->Sumw2();
  TH1D* hZ_energy_Evt_nonMuon    = new TH1D("hZenergyEvt_nonMuon","",towersz, towersz_min, towersz_max);
  hZ_energy_Evt_nonMuon->Sumw2();


  //Variables to read out at each step
  
    Cluster ClusterWindow;

  // creating plotting directory
  StyleSettingsBasics(plotSuffix);
  TString outputDirPlots = GetPlotOutputDir();
  gSystem->Exec("mkdir -p "+outputDirPlots);
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Inclusive");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Comparisons");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Muon");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/nonMuon");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Inclusive/Correlations");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/Muon/Correlations");
  gSystem->Exec("mkdir -p "+outputDirPlots+"/nonMuon/Correlations");
   
  
  
  // processing events
  int evts=TdataIn->GetEntries();
  int evtsMuon= 0;
  
  // print calib info
  calib.PrintGlobalInfo();
  double averagePedMeanHG = calib.GetAveragePedestalMeanHigh();
  double averagePedSigHG  = calib.GetAveragePedestalSigHigh();
  double averagePedMeanLG = calib.GetAveragePedestalMeanLow();
  double averagePedSigLG  = calib.GetAveragePedestalSigLow();
  std::cout<< averagePedMeanHG << "\t" << averagePedSigHG << "\t" << averagePedMeanLG << "\t" << averagePedSigLG << std::endl;

  Double_t eCutoff = 0.3;
  if (plotHGCROC && dataTypeHGCROC==0)
    eCutoff = calib.GetAveragePedestalSigHigh()*5.;
  std::cout << "setting energy cut-off to: " << eCutoff << std::endl;
  double Etottot = 0;
  
  //TPolyLine3D* testline = new TPolyLine3D();
  for(int i=plotEvt; i<plotEvt+nEvts; i++){
    if(i > evts ) {
      std::cout<<"Requested event outside of the range. Aborting..."<<std::endl;
      return false;
    }
    TdataIn->GetEntry(i);
    // if( i != 258){continue;}
    if (i%1000 == 0 && debug > 0) std::cout << "Reading " <<  i << " / " << evts << " events" << std::endl;
    double Etot = 0;
    int nCells  = 0;
    bool muontrigg = false;
    int locMuon = 0;
    std::map<int,Layer> layers;
    std::map<int, Layer>::iterator ithLayer;
    
    if( debug>1 ) std::cout << "Event has " << event.GetNTiles() << " active tiles" << std::endl;
    for(int j=0; j<event.GetNTiles(); j++){
      
      if(plotHGCROC){
        Hgcroc* aTile = (Hgcroc*)event.GetTile(j);
        // no "energy" threshold for now - just plot everything
        nCells++;
        int currLayer   = setup->GetLayer(aTile->GetCellID());
        ithLayer        = layers.find(currLayer);
        double energy = 0;
        switch( dataTypeHGCROC ){
          case 0:
            energy = aTile->GetIntegratedADC(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t ADC: " << aTile->GetIntegratedADC() << "\t energy " << energy << std::endl;
            break;
          case 1:
            energy = aTile->GetMaxSampleADC() - aTile->GetPedestal(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t ADC: " << aTile->GetIntegratedADC() << "\t pedestal (calib) " << calib.GetPedestalMeanH( aTile->GetCellID() ) << "\t pedestal (aTile) " << aTile->GetPedestal() << "\t energy " << energy << std::endl;
            break;
          case 2:
            energy = aTile->GetRawTOT(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t TOT: " << aTile->GetRawTOT() << "\t energy " << energy << std::endl;
            break;
          case 3:
            energy = aTile->GetIntegratedADC()/ calib.GetScaleHigh(aTile->GetCellID()); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t ADC: " << aTile->GetIntegratedADC() << "\t scale (calib) " << calib.GetScaleHigh( aTile->GetCellID() ) << "\t energy " << energy << std::endl;
            break;
          case 4:
            energy = aTile->GetE(); 
            if( debug>1 ) std::cout << "CellID: " << aTile->GetCellID() << "\t E: " << aTile->GetE() << std::endl;
            break;
        }

        if (dataTypeHGCROC == 4 && energy < energy) continue;
        
        if(ithLayer!=layers.end()){
          ithLayer->second.nCells+=1;
          ithLayer->second.energy+=energy;
          ithLayer->second.avX+=setup->GetX(aTile->GetCellID())*energy;
          ithLayer->second.avY+=setup->GetY(aTile->GetCellID())*energy;
        } else {
          layers[currLayer]=Layer();
          layers[currLayer].nCells+=1;
          layers[currLayer].energy+=energy;
          layers[currLayer].avX+=setup->GetX(aTile->GetCellID())*energy;
          layers[currLayer].avY+=setup->GetY(aTile->GetCellID())*energy;
        }
      } else {
        Caen* aTile=(Caen*)event.GetTile(j);
        if(aTile->GetE()>eCutoff ){ 
          nCells++;
          int currLayer = setup->GetLayer(aTile->GetCellID());
          ithLayer=layers.find(currLayer);
          if(ithLayer!=layers.end()){
            ithLayer->second.nCells+=1;
            ithLayer->second.energy+=aTile->GetE();
            ithLayer->second.avX+=setup->GetX(aTile->GetCellID())*aTile->GetE();
            ithLayer->second.avY+=setup->GetY(aTile->GetCellID())*aTile->GetE();
          } else {
            layers[currLayer]=Layer();
            layers[currLayer].nCells+=1;
            layers[currLayer].energy+=aTile->GetE();
            layers[currLayer].avX+=setup->GetX(aTile->GetCellID())*aTile->GetE();
            layers[currLayer].avY+=setup->GetY(aTile->GetCellID())*aTile->GetE();
          }
        }
        if (aTile->GetLocalTriggerBit() == 1){
          locMuon++;        
        }      
      }
    }
    //std::cout << "NCells hit: "  << nCells << "\t" << minTilesHit << std::endl;
    
    if (nCells > minTilesHit) {
      int nLayerSingleCell = 0;
      for(ithLayer=layers.begin(); ithLayer!=layers.end(); ++ithLayer){
        if (ithLayer->second.nCells == 1) 
            nLayerSingleCell++;
      }
      double fracLayer1Cell = (double)nLayerSingleCell/(int)layers.size();
      double fracLocMuon = (double)locMuon/nCells;
      if (fracLocMuon > 0.6){ 
        muontrigg = true;
	
	std::cout << "muon triggered " << std::endl;
        evtsMuon++;
      }
      if (muontrigg && debug > 1){
          std::cout << "Event: " << i << "\tMuon triggered:\t" <<  fracLayer1Cell << "\t" << fracLocMuon << std::endl;
      }
      Float_t minE = 1e10;
      Float_t maxE = 0;
      for(int j=0; j<event.GetNTiles(); j++){
        double energy  = 0;
        double x, y, z; 
        unsigned char muonTrigger = 0;
        if(plotHGCROC){
          Hgcroc* aTile   = (Hgcroc*)event.GetTile(j);
          if( calib.GetBadChannel(aTile->GetCellID())!=3 && calib.GetBCCalib() == true) continue;
          switch( dataTypeHGCROC ){
            case 0:
              energy = aTile->GetIntegratedADC();
              break;
            case 1:
              energy = aTile->GetIntegratedADC()- calib.GetPedestalMeanL(aTile->GetCellID());
              break;
            case 2:
              energy = aTile->GetRawTOT();
              break;
            case 3:
              energy = aTile->GetIntegratedADC()/calib.GetScaleHigh(aTile->GetCellID());
              break;
            case 4:
              energy = aTile->GetE();
              break;
          }
          x = (double) aTile->GetX();
          y = (double) aTile->GetY();
          z = (double) aTile->GetZ();
          if( debug>2 ) std::cout << "CellID: " << aTile->GetCellID() << "\t value: " << energy  << "\tx,y,z: " <<x<< "\t" <<y<< "\t" << z << std::endl;
          muonTrigger = (unsigned char)aTile->GetLocalTriggerBit();
        } else {
          Caen* aTile=(Caen*)event.GetTile(j);
          if( calib.GetBadChannel(aTile->GetCellID())!=3) continue; 
          energy = (Float_t)aTile->GetE();
          x = (double) aTile->GetX();
          y = (double) aTile->GetY();
          z = (double) aTile->GetZ();
          muonTrigger = (unsigned char)aTile->GetLocalTriggerBit();
        }
	Etottot+=energy;
        Etot+=energy;
        if (energy < minE) minE = energy;
        if (energy > maxE) maxE = energy;
        
	
        if(energy>eCutoff){ 
          hXYZMapEvt->Fill(z,x,y,energy);
          hX_energy_Evt->Fill(x, energy);
          hY_energy_Evt->Fill(y, energy);
          hZ_energy_Evt->Fill(z, energy);
          if(debug > 2) std::cout << "Event, x, y, z, E: " << i << "\t" <<x<< "\t" <<y<< "\t" << z<< "\t" <<energy<<std::endl;
          if (muonTrigger == 1){
            hXYZMapEvt_Muon->Fill(z,x,y,energy);
            hX_energy_Evt_Muon->Fill(x, energy);
            hY_energy_Evt_Muon->Fill(y, energy);
            hZ_energy_Evt_Muon->Fill(z, energy);
          } else {
            hXYZMapEvt_nonMuon->Fill(z,x,y,energy);
            hX_energy_Evt_nonMuon->Fill(x, energy);
            hY_energy_Evt_nonMuon->Fill(y, energy);
            hZ_energy_Evt_nonMuon->Fill(z, energy);
          }
	}
	
      }
   
    
    
    }


    TH3F* hXYZMapEvt_Window = (TH3F*) hXYZMapEvt->Clone("hXYZMapEvt_Window");
    TH3F* hXYZMapEvt_Seed = (TH3F*) hXYZMapEvt->Clone("hXYZMapEvt_Seed");
    TH3F* hXYZMapEvt_Temp = (TH3F*) hXYZMapEvt->Clone("hXYZMapEvt_Temp");
  

    //CLUSTER HERE
    auto start = std::chrono::high_resolution_clock::now();
    ClusterVec =  DoClustering_Seeded_3D(hXYZMapEvt_Window,minwindow,8);
    auto end = std::chrono::high_resolution_clock::now();

    Clusters_NClusters->Fill(ClusterVec.size());

    
    auto time_cluster = std::chrono::duration_cast<std::chrono::microseconds>(end-start);

    
    if(ClusterVec.size() == 0){
     hXYZMapEvt->Reset();
    hXYZMapEvt_Muon->Reset();
    hXYZMapEvt_nonMuon->Reset();
    delete hXYZMapEvt_Window;
    delete hXYZMapEvt_Seed;
    delete hXYZMapEvt_Temp;
    hX_energy_Evt->Reset();
    hY_energy_Evt->Reset();
    hZ_energy_Evt->Reset();
    hX_energy_Evt_Muon->Reset();
    hY_energy_Evt_Muon->Reset();
    hZ_energy_Evt_Muon->Reset();
    hX_energy_Evt_nonMuon->Reset();
    hY_energy_Evt_nonMuon->Reset();
    hZ_energy_Evt_nonMuon->Reset();
    continue;
    }
  
    ClusterWindow = ClusterVec[0];

    

    //Second clustering method



    double totalwindowenergy = 0;

    for(int cluster = 0; cluster < ClusterVec.size(); cluster++){


      
      ClusterWindow = ClusterVec[cluster];
      
    
      if(ClusterWindow.energy > minwindow){
	//Temporary Muon Cut
	if(ClusterWindow.hitTowers < 1.1){
    
	  ClustersXY_Muon->Fill(ClusterWindow.x,ClusterWindow.y);
	  SubClustersXY_Muon->Fill(ClusterWindow.x-ClusterWindow.highestcellx,ClusterWindow.y-ClusterWindow.highestcelly);
	  MaxCellXY_Muon->Fill(ClusterWindow.highestcellx,ClusterWindow.highestcelly);
	  Clusters_lastZ_Muon->Fill(ClusterWindow.lastz);
	  Clusters_maxZ_Muon->Fill(ClusterWindow.highestz);
	  Clusters_sizeZ_Muon->Fill(ClusterWindow.sizez);
	  Clusters_Z_Imbalance_Muon->Fill(ClusterWindow.highestzE/ClusterWindow.energy);
	  Clusters_Z_HighestCell_Muon->Fill(ClusterWindow.highestcellenergy);
	  Clusters_firstZ_Muon->Fill(ClusterWindow.firstz);
    
	  Clusters_energy_Muon->Fill(ClusterWindow.energy);
	  Clusters_spreadX_Muon->Fill(ClusterWindow.hitTowers);
	  Clusters_Imbalance_Muon->Fill(ClusterWindow.highesttowerenergy/ClusterWindow.energy);

	  
	  //2D Correlations
	  Clusters_Energy_SpreadX_Muon->Fill(ClusterWindow.energy,ClusterWindow.hitTowers);
	  Clusters_Energy_Imbalance_Muon->Fill(ClusterWindow.energy,ClusterWindow.highesttowerenergy/ClusterWindow.energy);
	  Clusters_Energy_FirstZ_Muon->Fill(ClusterWindow.energy,ClusterWindow.firstz);
	  Clusters_Energy_LastZ_Muon->Fill(ClusterWindow.energy,ClusterWindow.lastz);
	  Clusters_Energy_MaxZ_Muon->Fill(ClusterWindow.energy,ClusterWindow.highestz);
	  Clusters_Energy_SizeZ_Muon->Fill(ClusterWindow.energy,ClusterWindow.sizez);
	}
        else{
    
	  ClustersXY_nonMuon->Fill(ClusterWindow.x,ClusterWindow.y);
	  SubClustersXY_nonMuon->Fill(ClusterWindow.x-ClusterWindow.highestcellx,ClusterWindow.y-ClusterWindow.highestcelly);
	  MaxCellXY_nonMuon->Fill(ClusterWindow.highestcellx,ClusterWindow.highestcelly);
	  Clusters_lastZ_nonMuon->Fill(ClusterWindow.lastz);
	  Clusters_maxZ_nonMuon->Fill(ClusterWindow.highestz);
	  Clusters_sizeZ_nonMuon->Fill(ClusterWindow.sizez);
	  Clusters_Z_Imbalance_nonMuon->Fill(ClusterWindow.highestzE/ClusterWindow.energy);
	  Clusters_Z_HighestCell_nonMuon->Fill(ClusterWindow.highestzE);
	  Clusters_firstZ_nonMuon->Fill(ClusterWindow.firstz);
    
	  Clusters_energy_nonMuon->Fill(ClusterWindow.energy);
	  Clusters_spreadX_nonMuon->Fill(ClusterWindow.hitTowers);
	  Clusters_Imbalance_nonMuon->Fill(ClusterWindow.highesttowerenergy/ClusterWindow.energy);
    
	  Clusters_Energy_SpreadX_nonMuon->Fill(ClusterWindow.energy,ClusterWindow.hitTowers);
	  Clusters_Energy_Imbalance_nonMuon->Fill(ClusterWindow.energy,ClusterWindow.highestcellenergy/ClusterWindow.energy);
	  Clusters_Energy_FirstZ_nonMuon->Fill(ClusterWindow.energy,ClusterWindow.firstz);
	  Clusters_Energy_LastZ_nonMuon->Fill(ClusterWindow.energy,ClusterWindow.lastz);
	  Clusters_Energy_MaxZ_nonMuon->Fill(ClusterWindow.energy,ClusterWindow.highestz);
	  Clusters_Energy_SizeZ_nonMuon->Fill(ClusterWindow.energy,ClusterWindow.sizez);
	}


	//Inclusive
	
	SeedTowers_energy->Fill(ClusterWindow.highesttowerenergy);
	SeedCells_energy->Fill(ClusterWindow.highestcellenergy);
	Clusters_Energy_SeedCellsEnergy->Fill(ClusterWindow.energy,ClusterWindow.highestcellenergy);
	
	ClustersXY->Fill(ClusterWindow.x,ClusterWindow.y);
	SubClustersXY->Fill(ClusterWindow.x-ClusterWindow.highestcellx,ClusterWindow.y-ClusterWindow.highestcelly);
	MaxCellXY->Fill(ClusterWindow.highestcellx,ClusterWindow.highestcelly);
	Clusters_lastZ->Fill(ClusterWindow.lastz);
	Clusters_maxZ->Fill(ClusterWindow.highestz);
	Clusters_sizeZ->Fill(ClusterWindow.sizez);
	Clusters_Z_Imbalance->Fill(ClusterWindow.highestzE/ClusterWindow.energy);
	Clusters_Z_HighestCell->Fill(ClusterWindow.highestzE);
	Clusters_firstZ->Fill(ClusterWindow.firstz);

	Clusters_energy->Fill(ClusterWindow.energy);
	if(cluster == 0){Clusters_energy_first->Fill(ClusterWindow.energy);}
	if(cluster == 1){Clusters_energy_second->Fill(ClusterWindow.energy);}
	Clusters_eventenergy->Fill(ClusterWindow.eventenergy);
	if(cluster == 0){
	  Clusters_windowminuseventenergy->Fill(ClusterWindow.eventenergy-ClusterWindow.energy);
	}
	Clusters_spreadX->Fill(ClusterWindow.hitTowers);
	Clusters_Imbalance->Fill(ClusterWindow.highesttowerenergy/ClusterWindow.energy);
    
	Clusters_Energy_SpreadX->Fill(ClusterWindow.energy,ClusterWindow.hitTowers);
	Clusters_Energy_Imbalance->Fill(ClusterWindow.energy,ClusterWindow.highestcellenergy/ClusterWindow.energy);
	Clusters_Energy_FirstZ->Fill(ClusterWindow.energy,ClusterWindow.firstz);
	Clusters_Energy_LastZ->Fill(ClusterWindow.energy,ClusterWindow.lastz);
	Clusters_Energy_MaxZ->Fill(ClusterWindow.energy,ClusterWindow.highestz);
	Clusters_Energy_SizeZ->Fill(ClusterWindow.energy,ClusterWindow.sizez);
      }
    }
      
    
    
    //Fill in cell and tower info, not saved from clustering
    if(ClusterWindow.energy > minwindow){
      for(int x = 1; x <= towersx; x++){
	for(int y = 1; y <= towersy; y++){
	  double towerenergy = 0;
	  for(int z = 1; z <= towersz; z++){
	    if(hXYZMapEvt_Temp->GetBinContent(z,x,y) > 1){
	      towerenergy+=hXYZMapEvt_Temp->GetBinContent(z,x,y);
	      Cells_energy->Fill(hXYZMapEvt_Temp->GetBinContent(z,x,y));
	    }
	  }
	  Towers_energy->Fill(towerenergy);
	}
      }
    }
    
    ClusterVec.clear();
    
    hXYZMapEvt->Reset();
    hXYZMapEvt_Muon->Reset();
    hXYZMapEvt_nonMuon->Reset();
    
    delete hXYZMapEvt_Window;
    delete hXYZMapEvt_Seed;
    delete hXYZMapEvt_Temp;

  
    
    hX_energy_Evt->Reset();
    hY_energy_Evt->Reset();
    hZ_energy_Evt->Reset();
    hX_energy_Evt_Muon->Reset();
    hY_energy_Evt_Muon->Reset();
    hZ_energy_Evt_Muon->Reset();
    hX_energy_Evt_nonMuon->Reset();
    hY_energy_Evt_nonMuon->Reset();
    hZ_energy_Evt_nonMuon->Reset();
  }


  //DONE WITH EVENT

  //Purity histograms
  MuonPurity_Energy->Add(Clusters_energy_Muon);
  MuonPurity_Energy->Divide(Clusters_energy);
  //Fraction of events that pass muon cut

	  
  //Compare Muon and NonMuon
  Plot1D_Compare(Towers_energy,"Towers_Energy",SeedTowers_energy, "SeedTowers_Energy",0);
  Plot1D_Compare(Cells_energy,"Cells_Energy",SeedCells_energy, "SeedCells_Energy",0);
  Plot1D_Compare(Clusters_energy_Muon,"Muon Energy",Clusters_energy_nonMuon, "nonMuon Energy",0);

	  

	  
  Plot1D(Clusters_energy,"WindowEnergy",0);
  Plot1D(Cells_energy,"CellEnergy",0);
  Plot1D(SeedCells_energy,"SeedCellEnergy",0);
  Plot1D(Towers_energy,"TowerEnergy",0);
  Plot1D(SeedTowers_energy,"SeedTowerEnergy",0);
  Plot1D(Clusters_Imbalance,"ClusterImbalance",0);
  Plot1D(Clusters_Z_Imbalance,"ClusterImbalance_Z",0);
  Plot1D(Clusters_firstZ,"FirstHit_Z",0);
  Plot1D(Clusters_lastZ,"LastHit_Z",0);

  for(int i = 1; i <= towersz; i++){
    if(Clusters_maxZ->GetBinContent(i) == 0){
      Clusters_maxZ->SetBinContent(i,1000);
    }

  }
  Plot1D(Clusters_maxZ,"MaxZ",0);
  Plot1D(Clusters_sizeZ,"ZHits",0);
  Plot1D(Clusters_spreadX,"XYHits",0);

  Plot1D_NonLog(MuonPurity_Energy,"MuonPurity",0);
  //2D Correlation Plots
  //normalize along energy axis
  Normalize2D(Clusters_Energy_Imbalance);
  Normalize2D(Clusters_Energy_FirstZ);
  Normalize2D(Clusters_Energy_LastZ);
  Normalize2D(Clusters_Energy_MaxZ);
  Normalize2D(Clusters_Energy_SeedCellsEnergy);
  Normalize2D(Clusters_Energy_SizeZ);
  Normalize2D(Clusters_Energy_SpreadX);
  
  Plot2D(Clusters_Energy_Imbalance,"Imbalance",0);
  Plot2D(Clusters_Energy_FirstZ,"FirstHit_Z",0);
  Plot2D(Clusters_Energy_LastZ,"LastHit_Z",0);
  Plot2D(Clusters_Energy_MaxZ,"MaxZ",0);
  Plot2D(Clusters_Energy_SeedCellsEnergy,"SeedCellsEnergy",0);
  Plot2D(Clusters_Energy_SizeZ,"ZHits",0);
  Plot2D(Clusters_Energy_SpreadX,"XYHits",0);

  

  /////
  /////Muon
  /////
 
	  
	  
  Plot1D(Clusters_energy_Muon,"WindowEnergy_Muon",1);
  Plot1D(Clusters_Imbalance_Muon,"ClusterImbalance_Muon",1);
  Plot1D(Clusters_Z_Imbalance_Muon,"ClusterImbalance_Z_Muon",1);
  Plot1D(Clusters_firstZ_Muon,"FirstHit_Z_Muon",1);
  Plot1D(Clusters_lastZ_Muon,"LastHit_Z_Muon",1);
  Plot1D(Clusters_maxZ_Muon,"MaxZ_Muon",1);
  Plot1D(Clusters_sizeZ_Muon,"ZHits_Muon",1);
  Plot1D(Clusters_spreadX_Muon,"XYHits_Muon",1);

  //2D Correlation Plots
  //normalize along energy axis
  Normalize2D(Clusters_Energy_Imbalance_Muon);
  Normalize2D(Clusters_Energy_FirstZ_Muon);
  Normalize2D(Clusters_Energy_LastZ_Muon);
  Normalize2D(Clusters_Energy_MaxZ_Muon);
  Normalize2D(Clusters_Energy_SizeZ_Muon);
  Normalize2D(Clusters_Energy_SpreadX_Muon);
	  
  Plot2D(Clusters_Energy_Imbalance_Muon,"Imbalance_Muon",1);
  //Plot2D(Clusters_Energy_FirstZ_Muon,"FirstHit_Z_Muon",1);
  //Plot2D(Clusters_Energy_LastZ_Muon,"LastHit_Z_Muon",1);
  Plot2D(Clusters_Energy_MaxZ_Muon,"MaxZ_Muon",1);
  //Plot2D(Clusters_Energy_SizeZ_Muon,"ZHits_Muon",1);
  Plot2D(Clusters_Energy_SpreadX_Muon,"XYHits_Muon",1);
      

	  

  /////
  /////nonMuon
  /////
	  
	  
  Plot1D(Clusters_energy_nonMuon,"WindowEnergy_nonMuon",2);
  Plot1D(Clusters_Imbalance_nonMuon,"ClusterImbalance_nonMuon",2);
  Plot1D(Clusters_Z_Imbalance_nonMuon,"ClusterImbalance_Z_nonMuon",2);
  Plot1D(Clusters_firstZ_nonMuon,"FirstHit_Z_nonMuon",2);
  Plot1D(Clusters_lastZ_nonMuon,"LastHit_Z_nonMuon",2);
  Plot1D(Clusters_maxZ_nonMuon,"MaxZ_nonMuon",2);
  Plot1D(Clusters_sizeZ_nonMuon,"ZHits_nonMuon",2);
  Plot1D(Clusters_spreadX_nonMuon,"XYHits_nonMuon",2);

  //2D Correlation Plots
  //normalize along energy axis
  Normalize2D(Clusters_Energy_Imbalance_nonMuon);
  Normalize2D(Clusters_Energy_FirstZ_nonMuon);
  Normalize2D(Clusters_Energy_LastZ_nonMuon);
  Normalize2D(Clusters_Energy_MaxZ_nonMuon);
  Normalize2D(Clusters_Energy_SizeZ_nonMuon);
  Normalize2D(Clusters_Energy_SpreadX_nonMuon);
	  
  Plot2D(Clusters_Energy_Imbalance_nonMuon,"Imbalance_nonMuon",2);
  //Plot2D(Clusters_Energy_FirstZ_nonMuon,"FirstHit_Z_nonMuon",2);
  //Plot2D(Clusters_Energy_LastZ_nonMuon,"LastHit_Z_nonMuon",2);
  Plot2D(Clusters_Energy_MaxZ_nonMuon,"MaxZ_nonMuon",2);
  //Plot2D(Clusters_Energy_SizeZ_nonMuon,"ZHits_nonMuon",2);
  Plot2D(Clusters_Energy_SpreadX_nonMuon,"XYHits_nonMuon",2);

	  

  //DONE

  delete hXYZMapEvt;
  delete hXYZMapEvt_Muon;
  delete hXYZMapEvt_nonMuon;
  delete hX_energy_Evt;
  delete hY_energy_Evt;
  delete hZ_energy_Evt;
  delete hX_energy_Evt_Muon;
  delete hY_energy_Evt_Muon;
  delete hZ_energy_Evt_Muon;
  delete hX_energy_Evt_nonMuon;
  delete hY_energy_Evt_nonMuon;
  delete hZ_energy_Evt_nonMuon;



  std::cout << "done plotting!!" << std::endl;
  
  return true;
}
