#! /bin/bash

PlotBaseDir=..
runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
toAOffSetCalib=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv

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
# 	printf -v runNr "%03d" "$2"
# 	printf -v runNrMuon "%03d" "$3"
	runNr=$2
  echo 
	echo "=================================================================================="
	if [ $1 == "wave" ]; then 
      ./HGCROCStudy -d 1 -E 2 -f -w -i $3/calibratedHGCROC_Run_$runNr.root -o $3/calibratedHGCROC_wave_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsWave/$5 -r $runNrFile 
	elif [ $1 == "waveWOff" ]; then 
      ./HGCROCStudy -d 2 -E 2 -f -w -i $3/calibratedHGCROC_Run_$runNr.root -o $3/calibratedHGCROC_wave_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsWaveOffSetCorr/$5 -r $runNrFile -t $toAOffSetCalib
  elif [ $1 == "waveNToA" ]; then 
    ./HGCROCStudy -d 1 -E 2 -f -w -i $3/calibratedHGCROC_Run_$runNr.root -o $3/calibratedHGCROC_wave_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsWave/$5 -r $runNrFile -s $6
  elif [ $1 == "waveNToAWOff" ]; then 
    ./HGCROCStudy -d 1 -E 2 -f -w -i $3/calibratedHGCROC_Run_$runNr.root -o $3/calibratedHGCROC_wave_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsWaveOffSetCorr/$5 -r $runNrFile -s $6 -t $toAOffSetCalib
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


if [ $2 = "ElectronsA" ]; then 
  runs='166' #electrons
#   runs='166 167 168 169 170' #electrons
	for runNr in $runs; do 
		HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $4
	done
elif [ $2 = "MuonsA" ]; then 
  runs='FullSetA_2' 
#   runs='166 167 168 169 170' #electrons
	for runNr in $runs; do 
		HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $4
	done
elif [ $2 = "HadronsA" ]; then 
  runs='184' 
	for runNr in $runs; do 
		HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $4
	done	
fi
