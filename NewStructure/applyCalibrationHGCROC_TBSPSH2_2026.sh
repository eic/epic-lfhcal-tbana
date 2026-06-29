#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

#run list file
runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv
dataDirCal=""

if [ $1 = "fbockTB" ]; then 
  dataDirCal=/media/fbock/ALICE2-4TB/202605_SPSH2/HGCROCData
  dataDirIn=/media/fbock/ALICE2-4TB/202605_SPSH2/HGCROCData
  dataDirOut=/media/fbock/ALICE2-4TB/202605_SPSH2/HGCROCData
  PlotBaseDir=/media/fbock/ALICE2-4TB/202605_SPSH2/
  elif [ $1 = "yale" ]; then
  dataDirCal=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/calibrated
  dataDirIn=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/rawroot
  dataDirOut=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/rawroot
  PlotBaseDir=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/rawroot
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

# apply calibration
if [ $2 == "ParamScan" ]; then

  badChannelMap=../configs/TB2026/badChannel_HGCROC_SPSTB2026_OnlyCenter2x4.txt
  if [ $4 = "Set1" ]; then
    toaPhaseOffset=../configs/TB2026/ToAOffsets_TBSPS2026_ParamScan_1.csv
    runMuons='295 298 300 302 304 306 308 310'
  elif [ $4 = "Set3" ]; then
    toaPhaseOffset=../configs/TB2026/ToAOffsets_TBSPS2026_ParamScan_2.csv
    runMuons='329 331 333 335 337 339 341 343 345 347 349 351 353 355 357 359 361 363 366 369 '
  fi
  
  for runNr in $runMuons; do
    Calib $3 $dataDirCal/rawHGCROC_wPedwMuon_wBC_Imp3R_$runNr.root $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
  done;
fi

