#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

PlotBaseDir=..
runList=../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/Lennard4TB/202605_SPSH2/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/202605_SPSH2/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/202605_SPSH2/
elif [ $1 = "atamis" ]; then 
	dataDirRaw=/home/drewtam20/Documents/eic/LFHCALDATA/Converted
	dataDirOut=/home/drewtam20/Documents/eic/LFHCALDATA/Converted
	PlotBaseDir=/home/drewtam20/Documents/eic/LFHCALDATA/Plots
elif [ $1 = "ehagen" ]; then 
	dataDirRaw=/Users/hagen/Githubs/TB_data
	dataDirOut=/Users/hagen/Githubs/TB_data
	PlotBaseDir=/Users/hagen/Githubs/TB_data/plots
elif [ $1 = "egpott" ]; then
  dataDirRaw=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot
  dataDirOut=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot
  PlotBaseDir=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/plots
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

# run pedest extraction for different run numbers
if [ $2 = "pedestal" ]; then
  runs='';
  # different number of KCUs & asics
  if [ $3 = "Test" ]; then
    runs='72'  
  elif [ $3 = "FullSetA" ]; then
    runs='48' #pedestals
  # Full Scan B
  elif [ $3 = "FullSetB" ]; then
#     runs='71' #pedestals
    runs='126' #pedestals
  elif [ $3 = "FullSetC" ]; then
#     runs='134 135 137' #pedestals
    runs='135' #pedestals
  fi
  for runNr in $runs; do 
    printf -v runNrPed "%03d" "$runNr"
    Pedestal $2 $runNrPed $dataDirRaw $dataDirOut Run$runNrPed pdf
#     ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNrPed.root -f -o $dataDirOut/rawHGCROC_wPed_$runNrPed.root -O $PlotBaseDir/PlotsPedestal/Run$runNrPed -r $runList
  done
fi

if [ $2 = "toaPhase" ]; then   
  if [ $3 = "FullSetB" ]; then
    runNrPed='071'
    if [ $4 = "Hadron" ]; then 
      runs=''
    elif [ $4 = "Muon" ]; then 
      runs='072' # 1st 0,0
    fi
  fi
  if [ $4 = "Hadron" ]; then 
    for runNr in $runs; do 
      ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirOut/rawHGCROC_wPed_$runNrPed.root #-F png
    done
  elif [ $4 = "Muon" ]; then 
    for runNr in $runs; do 
      echo $runNr
			./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_wPed_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirRaw/rawHGCROC_wPed_$runNrPed.root
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
  toaPhaseOffset=''
  if [ $4 = "FullSetB_A" ]; then
    runPed='071'
    runs='072'
    toaPhaseOffset='../configs/TB2026/ToAOffsets_TBSPS2026_FullSetB.csv'
    
  elif [ $4 = "FullSetB_1" ]; then
    runPed='071'
    runs='Muon_FullSetB_1'
    toaPhaseOffset='../configs/TB2026/ToAOffsets_TBSPS2026_FullSetB.csv'
  elif [ $4 = "FullSetB_ele" ]; then
    runPed='071'
    runs='085'
    toaPhaseOffset='../configs/TB2026/ToAOffsets_TBSPS2026_FullSetB.csv'
  elif [ $4 = "FullSetB_pi" ]; then
    runPed='071'
#     runs='113'
    runs='114 115 116 117 118 119 120 121 122 123 124'
    toaPhaseOffset='../configs/TB2026/ToAOffsets_TBSPS2026_FullSetB.csv'
  elif [ $4 = "FullSetB_piMax" ]; then
    runPed='071'
    runs='125'
    toaPhaseOffset='../configs/TB2026/ToAOffsets_TBSPS2026_FullSetB.csv'
  else 
    echo "No run selected, exiting..."
    exit
  fi

	badChannelMap=../configs/TB2026/badChannel_HGCROC_SPSTB2026_dummy.txt
	
	for runNr in $runs; do 
    echo "$runNr   $runPed"
    MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset 	
  done
fi

