#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

# base settings
PlotBaseDir=..
runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv
dataDirRaw=""
dataDirOut=""

#set up your username and location for input & output
if [ $1 = "fbockTB" ]; then 
  dataDirRaw=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
  dataDirOut=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
  PlotBaseDir=/media/fbock/Lennard4TB/202604_PST10/
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

# real running starts here
if [ $2 = "FullSetE" ]; then 
  toAOffSetCalib=../configs/TB2026/ToAOffsets_TB2026_FullSetE.csv
#   runs='Muon_FullSetE_1' 
  runs='Muon_FullSetE_1 Muon_FullSetE_2 pi-_1GeV_FullSetE 295 296 297 300 301 302 303 304 305 306 307 308 309 310 311 312 313 314' 
  for runNr in $runs; do 
    HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $4
  done
elif [ $2 = "FullSetEAll" ]; then 
  toAOffSetCalib=../configs/TB2026/ToAOffsets_TB2026_FullSetE.csv
  runs='Muon_FullSetE_1' 
#   runs='Muon_FullSetE_2 pi-_1GeV_FullSetE 295 296 297 300 301 302 303 304 305 306 307 308 309 310 311 312 313 314' 
  for runNr in $runs; do 
    chIDs='0 1 2 3 4 5 6 7 9 10 11 12 13 14 15 16 18 19 20 21 22 23 24 25 27 28 29 30 31 32 33 34 36 37 38 39 40 41 42 43 45 46 47 48 49 50 51 52 54 55 56 57 58 59 60 61 63 64 65 66 67 68 69 70'
    for chID in $chIDs; do
      HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $chID
    done
  done 
fi
