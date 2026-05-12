#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

# base settings
runList=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
toAOffSetCalib=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
PlotBaseDir=..
dataDirRaw=""
dataDirOut=""

#set up your username and location for input & output
if [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/202511_PST09/
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

# real running starts here
if [ $2 = "ElectronsA" ]; then 
  runs='166' #electrons
#   runs='166 167 168 169 170' #electrons
	for runNr in $runs; do 
		HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $4
	done
elif [ $2 = "ElectronsAAll" ]; then 
  runs='166' #electrons
	for runNr in $runs; do 
    chIDs='0 1 2 3 4 5 6 7 9 10 11 12 13 14 15 16 18 19 20 21 22 23 24 25 27 28 29 30 31 32 33 34 36 37 38 39 40 41 42 43 45 46 47 48 49 50 51 52 54 55 56 57 58 59 60 61 63 64 65 66 67 68 69 70'
    for chID in $chIDs; do
      HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $chID
    done
	done
elif [ $2 = "MuonsA" ]; then 
  runs='FullSetA_2' 
	for runNr in $runs; do 
		HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $4
	done
elif [ $2 = "MuonsAAll" ]; then 
  runs='FullSetA_2' 
	for runNr in $runs; do 
    chIDs='0 1 2 3 4 5 6 7 9 10 11 12 13 14 15 16 18 19 20 21 22 23 24 25 27 28 29 30 31 32 33 34 36 37 38 39 40 41 42 43 45 46 47 48 49 50 51 52 54 55 56 57 58 59 60 61 63 64 65 66 67 68 69 70'
    for chID in $chIDs; do
      HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $chID
    done
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
