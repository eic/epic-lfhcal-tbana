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
if [ $2 == "HVScan1" ]; then
	toaPhaseOffset='../configs/TB2026/ToAOffsets_TB2026_HVScan1.csv'
  badChannelMap="../configs/TB2026/badChannel_HGCROC_PSTB2026_dummy.txt"
	Calib $3 $dataDirCal/calib_HVScan1_42V.root $dataDirIn $dataDirOut Muon_HVScan_42V $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	Calib $3 $dataDirCal/calib_HVScan1_42_5V.root $dataDirIn $dataDirOut Muon_HVScan_42_5V $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	Calib $3 $dataDirCal/calib_HVScan1_43V.root $dataDirIn $dataDirOut Muon_HVScan_43V $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	Calib $3 $dataDirCal/calib_HVScan1_43_5V.root $dataDirIn $dataDirOut Muon_HVScan_43_5V $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	Calib $3 $dataDirCal/calib_HVScan1_44V.root $dataDirIn $dataDirOut Muon_HVScan_44V $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	Calib $3 $dataDirCal/calib_HVScan1_44_5V.root $dataDirIn $dataDirOut Muon_HVScan_44_5V $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	Calib $3 $dataDirCal/calib_HVScan1_45V.root $dataDirIn $dataDirOut Muon_HVScan_45V $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	Calib $3 $dataDirCal/calib_HVScan1_45_5V.root $dataDirIn $dataDirOut Muon_HVScan_45_5V $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	Calib $3 $dataDirCal/calib_HVScan1_46V.root $dataDirIn $dataDirOut Muon_HVScan_46V $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

elif [ $2 == "FullSetC" ]; then
	calibFile1=$dataDirCal/calib_FullSetC1.root
	calibFile2=$dataDirCal/calib_FullSetC2.root
	toaPhaseOffset='../configs/TB2026/ToAOffsets_TB2026_FullSetC.csv'
  badChannelMap="../configs/TB2026/badChannel_HGCROC_PSTB2026_dummy.txt"

	echo "running calibrate for FullSetC: 44V runs, summing board V2, original pre-amp settings"
	# muon runs
	Calib $3 $calibFile1 $dataDirIn $dataDirOut Muon_FullSetC_1 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	Calib $3 $calibFile2 $dataDirIn $dataDirOut Muon_FullSetC_2 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	# pi- runs
	runs='pi-_1GeV_FullSetC pi-_2GeV_FullSetC pi-_3GeV_FullSetC pi-_4GeV_FullSetC pi-_5GeV_FullSetC pi-_6GeV_FullSetC pi-_7GeV_FullSetC pi-_8GeV_FullSetC pi-_9GeV_FullSetC pi-_10GeV_FullSetC'
	for runNr in $runs; do
		Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	done;

	# had+ runs
	runs='Had+_1GeV_FullSetC Had+_2GeV_FullSetC Had+_3GeV_FullSetC Had+_4GeV_FullSetC Had+_5GeV_FullSetC Had+_6GeV_FullSetC Had+_7GeV_FullSetC Had+_8GeV_FullSetC Had+_9GeV_FullSetC Had+_10GeV_FullSetC'
	for runNr in $runs; do
		Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	done;

elif [ $2 == "FullSetD" ]; then
	calibFile1=$dataDirCal/calib_FullSetD1.root
	calibFile2=$dataDirCal/calib_FullSetD2.root
	toaPhaseOffset='../configs/TB2026/ToAOffsets_TB2026_FullSetD.csv'
  badChannelMap="../configs/TB2026/badChannel_HGCROC_PSTB2026_dummy.txt"

	echo "running calibrate for FullSetD: 44V runs, summing board V2, optimized pre-amp settings"
	# muon runs
	Calib $3 $calibFile1 $dataDirIn $dataDirOut Muon_FullSetD_1 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	Calib $3 $calibFile2 $dataDirIn $dataDirOut Muon_FullSetD_2 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	# pi- runs
	runs='pi-_1GeV_FullSetD pi-_2GeV_FullSetD pi-_3GeV_FullSetD pi-_4GeV_FullSetD pi-_5GeV_FullSetD pi-_6GeV_FullSetD pi-_7GeV_FullSetD pi-_8GeV_FullSetD pi-_9GeV_FullSetD pi-_10GeV_FullSetD'
	for runNr in $runs; do
		Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	done;

	# had+ runs
	runs='Had+_1GeV_FullSetD Had+_2GeV_FullSetD Had+_3GeV_FullSetD Had+_4GeV_FullSetD Had+_5GeV_FullSetD Had+_6GeV_FullSetD Had+_7GeV_FullSetD Had+_8GeV_FullSetD Had+_9GeV_FullSetD Had+_10GeV_FullSetD'
	for runNr in $runs; do
		Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	done;

elif [ $2 == "FullSetE" ]; then
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

