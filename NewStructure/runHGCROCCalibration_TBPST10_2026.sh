#! /bin/bash

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
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

# run pedest extraction for different run numbers
if [ $2 = "pedestal" ]; then
  runs='';
  # different number of KCUs & asics
  if [ $3 = "Test" ]; then
    runs=''  
  # first scans
  elif [ $3 = "FirstPosScanMuons" ]; then
    runs='1 39'
  elif [ $3 = "HVScan1" ]; then
    runs='39 66 70 79 80 82 221 225 228 231' 
  # Full Scan A
  elif [ $3 = "FullSetA" ]; then
    runs='085 120' #pedestals
  # Full Scan B
  elif [ $3 = "FullSetB" ]; then
    runs='215' #pedestals
  # Full Scan C
  elif [ $3 = "FullSetC" ]; then
    runs='130 171' #pedestals
  # Full Scan D
  elif [ $3 = "FullSetD" ]; then
    runs='238 265' #pedestals
  # Full Scan E
  elif [ $3 = "FullSetE" ]; then
    runs='287 315' 
  fi
  for runNr in $runs; do 
    printf -v runNrPed "%03d" "$runNr"
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNrPed.root -f -o $dataDirOut/rawHGCROC_wPed_$runNrPed.root -O $PlotBaseDir/PlotsPedestal/Run$runNrPed -r $runList
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
      ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirOut/rawHGCROC_wPed_$runNrPed.root #-F png
    done
  elif [ $3 = "Electron" ]; then 
    runs='' # 1st HV scan
    for runNr in $runs; do 
      ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root #-F png
    done
  elif [ $3 = "Muon" ]; then 
    runs='' # 1st HV scan
    for runNr in $runs; do 
			./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_wPed_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirRaw/rawHGCROC_wPed_$runNr.root
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

	#first muon scans
# 	runs='FirstMuons'
#   runs='FMuonCent FMuon_5_0 FMuon_0_5 FMuon_5_5'
#   runs='FMuon_0_5 FMuon_5_-5 FMuon_0_-5 FMuon_-5_-5 FMuon_-5_0 FMuon_5_5'
  
  toaPhaseOffset=''
  if [ $4 == "FMuon" ]; then
    runPed='39'
    runs='FMuon_-5_-5 FMuon_0_5 FMuon_5_-5 FMuon_5_5 FMuon_5_0 FMuonCent'
  elif [ $4 = "HVScan42" ]; then
    runPed='70'
    runs='Muon_HVScan_42V'
  elif [ $4 = "HVScan42_5" ]; then
    runPed='66'
    runs='Muon_HVScan_42_5V'
  elif [ $4 = "HVScan42_5" ]; then
    runPed='66'
    runs='Muon_HVScan_42_5V'
  elif [ $4 = "FullSetE_1" ]; then
    runPed='287'
    runs='Muon_FullSetE_1'
    toaPhaseOffset='../configs/TB2025/ToAOffsets_TB2025_HadRun.csv'
  elif [ $4 = "FullSetE_2" ]; then
    runPed='315'
    runs='Muon_FullSetE_2'
    toaPhaseOffset='../configs/TB2025/ToAOffsets_TB2025_HadRun.csv'
  else 
    #Muon positions scan Full Scan 43 V
    runPed='85'
    runs='Muon_-5_-5 Muon_-5_0 Muon_0_5 Muon_5_5'
  fi

	badChannelMap=../configs/TB2026/badChannel_HGCROC_PSTB2026_dummy.txt
	
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset # -EP 2026/2/15
		#MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset
	done
fi

if [ $2 == "calibMuon2" ]; then
#   #Muon positions scan Full Scan 43 V
#   runPed='85'
# 	runs='Muon_-5_-5 Muon_-5_0 Muon_0_5 Muon_5_5'

	#first muon scans
  runPed='39'
  runs='FMuon_-5_0  FMuon_0_-5 FMuon_-5_5'


	badChannelMap=../configs/TB2026/badChannel_HGCROC_PSTB2026_dummy.txt
	toaPhaseOffset=''
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset # -EP 2026/2/15
		#MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset
	done
fi


