#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

PlotBaseDir=..
runList=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
runNrFile=$runList
# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
  dataDirRaw=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
  dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
  PlotBaseDir=/media/fbock/Lennard4TB/202511_PST09/
elif [ $1 = "ehagen" ]; then 
  dataDirRaw=/Users/hagen/Githubs/epic-lfhcal-tbana/TB_Data
  dataDirOut=/Users/hagen/Githubs/epic-lfhcal-tbana/TB_Data
  PlotBaseDir=/Users/hagen/Githubs/epic-lfhcal-tbana/plots
elif [ $1 = "egpott" ]; then
  dataDirRaw=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan2/rawroot
  dataDirOut=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan2/rawroot
  PlotBaseDir=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan2/plots
  PlotDirCompCal=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan2/plots/CompareCalib
elif [ $1 = "yale" ]; then	
  dataDirRaw=/media/lfhcal/LFHCal_Backup_1/Test_Beams/202511_PST09/rawroot
  dataDirOut=/media/lfhcal/LFHCal_Backup_1/Test_Beams/202511_PST09/rawroot
  PlotBaseDir=/media/lfhcal/LFHCal_Backup_1/Test_Beams/202511_PST09/plots
	
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi


# run pedest extraction for different run numbers
if [ $2 = "pedestal" ]; then
  runs='';
  # different number of KCUs & asics
  if [ $3 = "Test" ]; then
    runs='122 123 124 125 126 128 129'  
  # reference pedestal runs for various campaigns
  elif [ $3 = "Ref" ]; then
    runs='270'
  elif [ $3 = "FullSetA" ]; then
    runs='Muon1_ped_FullSetA Muon2_ped_FullSetA'
 	elif [ $3 = "FullSetB" ]; then
		runs='Muon1_ped_FullSetB Muon2_ped_FullSetB'
	elif [ $3 = "DepthScan1" ]; then
		echo "No pedestal run taken!! Getting pedestal from muon run (273), check this is good!!!"
		runs='273'
	elif [ $3 = "DepthScan2" ]; then
		runs='ped_DepthScan2'
	elif [ $3 = "DepthScan3" ]; then
		runs='ped_DepthScan3'
	elif [ $3 = "DepthScan4" ]; then
		runs='ped_DepthScan4'
	elif [ $3 = "ElectronScan" ]; then
		runs='ped_ElectronScan'
	elif [ $3 = "HVScan1" ]; then
		echo "No pedestal run taken! Getting pedestal from 41V run (033), check this is good!!!"
		runs='Muon_41V_MuonScan1'
	elif [ $3 = "HVScan2" ]; then
		runs='ped_MuonScan2'
  fi
  
  for runNr in $runs; do 
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNrPed.root -f -o $dataDirOut/rawHGCROC_wPed_$runNrPed.root -O $PlotBaseDir/PlotsPedestal/Run$runNrPed -r $runNrFile
  done
fi

if [ $2 = "toaPhase" ]; then 
  if [ $3 = "Hadron" ]; then
		runPed='161'
    runs='178'
  elif [ $3 = "Electron" ]; then 
    runPed=''
		runs=''
	elif [ $3 = "Muon" ]; then
		runPed='161'
		runs='070'
	elif [ $3 = "FullSetA_1" ]; then
		runPed='Muon1_ped_FullSetA'
		runs=''
	elif [ $3 = "FullSetA_2" ]; then
		runPed='Muon2_ped_FullSetA'
		runs=''
	elif [ $3 = "FullSetB_1" ]; then
		runPed='Muon1_ped_FullSetB'
		runs=''
	elif [ $3 = "FullSetB_2" ]; then
		runPed='Muon2_ped_FullSetB'
		runs=''
  fi
	for runNr in $runs; do
    ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseD    ir/ToAPhaseExtraction/Run$runNr -r $runNrFile -g $dataDirRaw/rawHGCROC_wPed_$runNrPed.root
  done
	#./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$    runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runNrFile -g $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$ru    nNr.root #-F png done
fi

if [ $2 = "wave" ]; then  
#   runs='165 166 167 168 169 170'
#   runs='170'
#   runs='165'
#   runs='184'
  runs='FullSetA_2'
  runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
  for runNr in $runs; do 
    WaveformHGCROC $3 $runNr $dataDirRaw Run_$runNr
  done
fi


if [ $2 == "calibMuon" ]; then
  runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv

  if [ $4 == "FullSetA_1" ]; then
    runPed='Muon1_ped_FullSetA'
    runs='Muon1_FullSetA'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  elif [ $4 == "FullSetA_2" ]; then
    runPed='Muon2_ped_FullSetA'
    runs='Muon2_FullSetA'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  elif [ $4 == "FullSetB_1" ]; then
    runPed='Muon1_ped_FullSetB'
    runs='Muon1_FullSetB'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  elif [ $4 == "FullSetB_2" ]; then
    runPed='Muon2_ped_FullSetB'
    runs='Muon2_FullSetB'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  elif [ $4 == "DepthScan1" ]; then
    runPed='273'
    runs='Muon_DepthScan1'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  elif [ $4 == "DepthScan2" ]; then
    runPed='ped_DepthScan2'
    runs='Muon_DepthScan2'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  elif [ $4 == "DepthScan3" ]; then
    runPed='ped_DepthScan3'
    runs='Muon_DepthScan3'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  elif [ $4 == "DepthScan4" ]; then
    runPed='ped_DepthScan4'
    runs='Muon_DepthScan4'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  elif [ $4 == "ElectronScan" ]; then
    runPed='ped_ElectronScan'
    runs='Muon_ElectronScan'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  elif [ $4 == "HVScan1" ]; then
    runPed='Muon_41V_MuonScan1'
    runs='Muon_41V_MuonScan1 Muon_42V_MuonScan1 Muon_43V_MuonScan1 Muon_44V_MuonScan1 Muon_45V_MuonScan1 Muon_46V_MuonScan1'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  elif [ $4 == "HVScan2" ]; then
    runPed='ped_MuonScan2'
    runs='Muon_41V_MuonScan2 Muon_42V_MuonScan2 Muon_42_5V_MuonScan2 Muon_43V_MuonScan2 Muon_43_5V_MuonScan2 Muon_44V_MuonScan2 Muon_45V_MuonScan2 Muon_46V_MuonScan2'
    badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
    toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
	fi
  for runNr in $runs; do 
    MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset 
  done
fi

if [ $2 == "calibMuon2" ]; then
#   runPed='68'
# 	runs='FullSetA_1'
#   runPed='208'
# 	runs='FullSetA_2'
#   runPed='210'
# 	runs='FullSetB_1'
  runPed='259'
  runs='FullSetB_2'
  badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
  runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
  toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  for runNr in $runs; do 
    MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset
  done
fi

if [ $2 == "compareCalib" ]; then
  # currently set up for extended file list read-in where each line in the txt file has (separated by a space):
  #  - calibration file (produced by stripCalib_2025.sh)
  #  - hist file from improved mip fit
  inputFileList=calibFileList_TB2025_HVscan1.txt # include relative path if not in NewStructure
  outFileName=compareCalib_28_33.root
  trendingPlotsX=V # =V: plot as function of Vop. =R: plot as function of run number
  runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
  # -e 1 --> do extended plotting
  # -d 0 --> debug level 0
  # -f   --> force to write output if already exist
  # -H   --> use HGCROC (instead of CAEN)
  # -I   --> extended input file list
  ./CompareCalib -e 1 -d 0 -f -H -$trendingPlotsX -I $inputFileList -o $outFileName -O $PlotDirCompCal -r $runNrFile
fi
