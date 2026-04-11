#!/bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

PlotBaseDir=..
runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/202604_PST10/
elif [ $1 = "ehagen" ]; then 
	dataDirRaw=/Users/hagen/Githubs/TB_data
	dataDirOut=/Users/hagen/Githubs/TB_data
	PlotBaseDir=/Users/hagen/Githubs/TB_data/plots
elif [ $1 = "egpott" ]; then
  dataDirRaw=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot
  dataDirOut=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot
  PlotBaseDir=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/plots
elif [ $1 = "dkapukch" ]; then
  dataDirRaw=/Users/davidkapukchyan/Documents/ePIC/LFHCal_TestBeam/HvScan42VFromDaq/ConvertedData
  dataDirOut=/Users/davidkapukchyan/Documents/ePIC/LFHCal_TestBeam/HvScan42VFromDaq/ConvertedData
  PlotBaseDir=/Users/davidkapukchyan/Documents/ePIC/LFHCal_TestBeam/HvScan42VFromDaq/ConvertedData/Plots
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

# run pedest extraction for different run numbers
if [ $2 = "pedestal" ]; then
  runs='70';
  # different number of KCUs & asics
  if [[ $3 = "Test" ]]; then
    runs='0'  
  # reference pedestal runs for various campaigns
  elif [[ $3 = "Ref" ]]; then
    runs='39'
  # muon runs
  elif [[ $3 = "Muon" ]]; then
    runs='71' 
  # electron runs
  elif [[ $3 = "Electron" ]]; then
    runs='0'
  fi
  for runNr in $runs; do 
    printf -v runNrPed "%03d" "$runNr"
    ./build/DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNrPed.root -f -o $dataDirOut/rawHGCROC_wPed_$runNrPed.root -O $PlotBaseDir/PlotsPedestal/Run$runNrPed -r $runList
  done
fi

if [ $2 = "toaPhase" ]; then 
  echo "Add runnumbers before you try this & remove exit"
  exit
  runNrPed='208'
  if [ $3 = "Hadron" ]; then 
    runs='184'
    for rn in $runs; do 
      printf -v runNr "%03d" "$rn"
      ./build/DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirOut/rawHGCROC_wPed_$runNrPed.root #-F png
    done
  elif [ $3 = "Electron" ]; then 
    runs='' # 1st HV scan
    for runNr in $runs; do 
      ./build/DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root #-F png
    done
  elif [ $3 = "Muon" ]; then 
    runs='' # 1st HV scan
    for runNr in $runs; do 
			./build/DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_wPed_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirRaw/rawHGCROC_wPed_$runNr.root
      #./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root #-F png
    done
  fi

fi

if [ $2 = "wave" ]; then  
  echo "Add runnumbers before you try this & remove exit"
  exit
  runs=''
	for runNr in $runs; do 
		WaveformHGCROC $3 $runNr $dataDirRaw Run_$runNr
	done
fi


if [ $2 == "calibMuon" ]; then
  runPed='70'
	#runs='Muon_-5_-5 Muon_-5_0 Muon_0_5 Muon_5_5'
  runs='071'
	badChannelMap=../configs/TB2026/badChannel_HGCROC_PSTB2026_dummy.txt
	toaPhaseOffset=''
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset # -EP 2026/2/15
		#MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset
	done
fi
