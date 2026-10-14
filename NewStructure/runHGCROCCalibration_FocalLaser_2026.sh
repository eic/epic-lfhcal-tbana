#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

PlotBaseDir=..
runList=../configs/FOCalTest2026/DataDB_LaserTestFocal_202609.csv

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbock" ]; then 
  dataDirRaw=/media/fbock/ALICE2-4TB/202609_FocalLaser/HGCROCData
  dataDirOut=/media/fbock/ALICE2-4TB/202609_FocalLaser/HGCROCData
  PlotBaseDir=/media/fbock/ALICE2-4TB/202609_FocalLaser/
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

# run pedest extraction for different run numbers
if [ $2 = "pedestal" ]; then
  runs='166 167 168 178 188 198 208 210 212 213 216 218 221 258 268 279 309 334 359 372 386 401 416 431'
  for runNr in $runs; do 
    printf -v runNrPed "%03d" "$runNr"
    Pedestal $2 $runNrPed $dataDirRaw $dataDirOut Run$runNrPed pdf
  done
fi

if [ $2 = "toaPhase" ]; then   
  if [ $3 = "SetA" ]; then
    runNrPed='168'    
    runs='173'
#     runs='169 170 171 172 173 174 175 176 177'
  elif [ $3 = "SetB" ]; then
    runNrPed='178'
    runs='179 180 181 182 183 184 185 186 187'
  fi
  for runNr in $runs; do 
    ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirOut/rawHGCROC_wPed_$runNrPed.root #-F png
  done
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
  toaPhaseOffset='/home/fbock/EIC/Software/epic-LFHCalTB/configs/FOCalTest2026/toAOffset_SetA.txt'
  badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt
  
  if [ $4 == "SetA" ]; then 
    runNrPed='168'    
    runs='169 170 171 172 173 174 175 176 177'
  else 
    runNrPed=''    
    runs=''
  fi
  
  for runNr in $runs; do
    MuonCalibHGCROC $3 $runNrPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset 	
  done
fi
