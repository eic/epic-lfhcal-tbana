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
elif [ $1 = "eglimos" ]; then
  dataDirCal=/home/ewa/EIC/DATA/2026_05_SPS_TestBeam/calibrations
  dataDirIn=/home/ewa/EIC/DATA/2026_05_SPS_TestBeam/converted
  dataDirOut=/home/ewa/EIC/DATA/2026_05_SPS_TestBeam/converted
  PlotBaseDir=/home/ewa/EIC/DATA/2026_05_SPS_TestBeam
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

elif [ $2 == "FullSetB" ]; then
  calibFile1=$dataDirCal/calib_Final_Muon_FullSetB_1.root
  # calibFile2=$dataDirCal/calib_Final_Muon_FullSetB_2.root
  toaPhaseOffset='../configs/TB2026/ToAOffsets_TBSPS2026_FullSetB.csv'
  badChannelMap="../configs/TB2026/badChannel_HGCROC_PSTB2026_dummy.txt"

  echo "running calibrate for FullSetB"
  # runs='072 073 074 075 076 077 078 079 080 081 082 083 084' #muons set 1
  # runs="086 087 088 089 090" # e-
  # runs="098 099 100 101 102 103 104 105 106 107 108" # hadrons
  runs='091 092 093 094 095 096 097 ' #e+
  for runNr in $runs; do
    Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
  done

fi