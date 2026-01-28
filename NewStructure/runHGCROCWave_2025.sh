#! /bin/bash

PlotBaseDir=..
runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv

function HGCInv()
{
	
	echo "=================================================================================="
	echo "option $1"
  echo "runNr" $2
	echo "dataRawDir: $3"
	echo "dataOutDir: $4"
	echo "OutNameRun:" $5
	echo "fixedSample" $6
# 	if [ $1 == "BC" ]; then 
# 		echo "badchannelMap:" $7
# 	fi
	printf -v runNr "%03d" "$2"
# 	printf -v runNrMuon "%03d" "$3"
# 	runNr=$2
  echo 
	echo "=================================================================================="
	if [ $1 == "wave" ]; then 
#     if [ -f "$4/rawHGCROC_wPed_$runNr_calib_mod.txt" ]; then
#       echo "overwriting original calib file with manually modified $4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt"
#       ./HGCROCStudy -d 1 -E 2 -f -w -i $3/calibratedHGCROC_$runNr.root -o $3/calibratedHGCROC_wave_$runNr.root -O $PlotBaseDir/HGCROC_PlotsWave/$5 -r $runNrFile -k $3/rawHGCROC_wPed_$runNrMuon_calib_mod.txt     
#     else 
      ./HGCROCStudy -d 1 -E 2 -f -w -i $3/calibratedHGCROC_Run_$runNr.root -o $3/calibratedHGCROC_wave_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsWave/$5 -r $runNrFile 
#     fi
  elif [ $1 == "waveNToA" ]; then 
    ./HGCROCStudy -d 1 -E 2 -f -w -i $3/calibratedHGCROC_Run_$runNr.root -o $3/calibratedHGCROC_wave_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsWave/$5 -r $runNrFile -s $6
	fi
	
}

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/202511_PST09/
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi


if [ $2 = "FullScanA" ]; then 
  runs='166' #electrons
#   runs='166 167 168 169 170' #electrons
	for runNr in $runs; do 
		HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $4
	done
fi
