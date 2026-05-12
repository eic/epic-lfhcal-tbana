#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

#run list file
runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv



dataDirCal=""
if [ $1 = "fbockTB" ]; then 
	dataDirCal=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
	dataDirIn=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/202604_PST10
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

# apply calibration
if [ $2 == "FullSetE" ]; then
	calibFile1=$dataDirCal/calib_FullSetE1.root
	calibFile2=$dataDirCal/calib_FullSetE2.root
	toaPhaseOffset='../configs/TB2026/ToAOffsets_TB2026_FullSetE.csv'
  badChannelMap="../configs/TB2026/badChannel_HGCROC_PSTB2026_dummy.txt"

  echo "running calibrate for 43V runs, campaign E, summing board V2, improved pre-amp settings"
	
	#muon runs
# 	Calib $3 $calibFile1 $dataDirIn $dataDirOut Muon_FullSetE_1 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	Calib $3 $calibFile2 $dataDirIn $dataDirOut Muon_FullSetE_2 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

  #pi- runs
#   runs='294'
  runs='pi-_1GeV_FullSetE 295 296 297 300 301 302 303 304'
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	done;
# 	#h+ runs
  runs='305 306 307 308 309 310 311 312 313 314' #h+
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	done;
fi

