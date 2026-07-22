#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

# base settings
PlotBaseDir=..
runList=../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv
dataDirRaw=""
dataDirOut=""

#set up your username and location for input & output
if [ $1 = "fbockTB" ]; then 
  dataDirRaw=/media/fbock/ALICE2-4TB/202605_SPSH2/HGCROCData
  dataDirOut=/media/fbock/ALICE2-4TB/202605_SPSH2/HGCROCData
  PlotBaseDir=/media/fbock/ALICE2-4TB/202605_SPSH2/
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

# real running starts here
# general options: 
#  - "wave": run wave form analysis w/ additional modifications to the data
#  - "waveWOff": run wave form analysis correcting for additional ToA offset in case this wasn't done
#  - "waveNToA": run wave form analysis w/ additional modifications to the data restricting ourselves to a fixed ToA sample. This needs another argument: the ToA sample you wanna restrict yourself to.
#  - "waveNToAWOff": run wave form analysis correcting for additional ToA offset in case this wasn't done. This path further restricts itself to a fixed ToA sample. This needs another argument: the ToA sample you wanna restrict yourself to.

if [ $2 = "ParamScan" ]; then 
  badChannelMap=../configs/TB2026/badChannel_HGCROC_SPSTB2026_OnlyCenter2x4.txt
  toAOffSetCalib=../configs/TB2026/ToAOffsets_TBSPS2026_ParamScan_1.csv
#   runs='295 298 300 302 304 306 308 310 329 331 333 335 337 339 341 343 345 347 349 351 353 355 357 359 361 363 366 369' 
  runs='329' 
#   runs='295 298 300 302 304 306 ' 
  cellsList=../configs/TB2026/activeCellsHGCROCParameterScan.txt
  for runNr in $runs; do 
    HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $cellsList $4 
  done
fi
