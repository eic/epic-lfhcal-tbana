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
  elif [ $1 == "timeWalk" ]; then 
    ./HGCROCStudy -d 1 -E 2 -f -T -i $3/calibratedHGCROC_Run_$runNr.root -o $3/calibratedHGCROC_timewalk_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsTimeWalk/$5 -r $runNrFile 
  elif [ $1 == "xTalk" ]; then 
    ./HGCROCStudy -d 1 -E 2 -f -x -i $3/calibratedHGCROC_Run_$runNr.root -o $3/calibratedHGCROC_xTalk_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsXTalk/$5 -r $runNrFile 
  elif [ $1 == "xTalkFCh" ]; then 
    ./HGCROCStudy -d 1 -E 1 -f -x -i $3/calibratedHGCROC_Run_$runNr.root -o $3/calibratedHGCROC_xTalk$6_Run_$runNr.root -O $PlotBaseDir/HGCROC_PlotsXTalk/$5 -r $runNrFile -c $6 
	fi
	
}

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
		HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $4
	done
elif [ $2 = "MuonsA" ]; then 
  runs='FullSetA_2' 
	for runNr in $runs; do 
		HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $4
	done
elif [ $2 = "HadronsA" ]; then 
  runs='184 188' 
	for runNr in $runs; do 
		HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $4
	done	
elif [ $2 = "HadronsAAll" ]; then 
  runs='184 188' 
	for runNr in $runs; do
    chIDs='0 1 2 3 4 5 6 7 9 10 11 12 13 14 15 16 18 19 20 21 22 23 24 25 27 28 29 30 31 32 33 34 36 37 38 39 40 41 42 43 45 46 47 48 49 50 51 52 54 55 56 57 58 59 60 61 63 64 65 66 67 68 69 70'
    for chID in $chIDs; do
      HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $chID
    done
	done	
fi
