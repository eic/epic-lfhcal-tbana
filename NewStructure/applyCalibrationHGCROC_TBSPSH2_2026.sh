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
elif [ $1 = "atamis" ]; then 
  dataDirCal=/mnt/c/Users/tamis/Documents/eic/LFHCALDATA/Converted/SPS
  dataDirIn=/mnt/c/Users/tamis/Documents/eic/LFHCALDATA/Converted/SPS
  dataDirOut=/mnt/c/Users/tamis/Documents/eic/LFHCALDATA/Converted/SPS
  PlotBaseDir=/mnt/c/Users/tamis/Documents/eic/LFHCALDATA/Plots/SPS
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

badChannelMap=../configs/TB2026/badChannel_HGCROC_SPSTB2026_OnlyCenter2x4.txt
  toaPhaseOffset=../configs/TB2026/ToAOffsets_TBSPS2026_ParamScan_1.csv
  runMuons='432 433 434 435 436 437 444 445 446 447 448 449 450 451 452 462 MuonFullSetF_1'

  # mip scale: max sample (default) or 3-sample sum for the *_3Int options (trigg_3Int, calibNoTrigg_3Int, full_3Int)
  calibFile=$dataDirCal/rawHGCROC_wPedwMuon_wBC_Imp3_MuonFullSetF_1.root
  if [[ $3 == *_3Int ]]; then
    calibFile=$dataDirCal/rawHGCROC_wPedwMuon_wBC_Imp2R_3Int_MuonFullSetF_1.root
  fi
  if [ ! -f "$calibFile" ]; then
    echo "calibration file $calibFile not found, run the corresponding calibMuon steps first"
    exit
  fi

  for runNr in $runMuons; do
    Calib $3 $calibFile $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
  done;
