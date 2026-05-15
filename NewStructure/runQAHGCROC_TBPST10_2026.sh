#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv

PlotBaseDir=..
if [ $1 = "fbockTB" ]; then 
	dataDirOut=/media/fbock/Lennard4TB/202604_PST10/HGCROCData/
	PlotBaseDir=/media/fbock/Lennard4TB/202604_PST10/
elif [ $1 = "yale" ]; then
	dataDirOut=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/rawroot
	PlotBaseDir=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/plots
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

if [ $2 == "HVScan1" ]; then
	echo "running calibrate for muon HV scan 1"
	runs='Muon_HVScan_42V Muon_HVScan_42_5V Muon_HVScan_43V Muon_HVScan_43_5V Muon_HVScan_44V Muon_HVScan_44_5V Muon_HVScan_45V Muon_HVScan_45_5V Muon_HVScan_46V'
	for runNr in $runs; do
		QARun $3 $dataDirOut $runNr $PlotBaseDir
	done;

elif [ $2 == "FullSetC" ]; then
	echo "running calibrate for 44V runs, summing board V2, campaign C"
	runs='Muon_FullSetC_1 Muon_FullSetC_2 pi-_1GeV_FullSetC pi-_2GeV_FullSetC pi-_3GeV_FullSetC pi-_4GeV_FullSetC pi-_5GeV_FullSetC pi-_6GeV_FullSetC pi-_7GeV_FullSetC pi-_8GeV_FullSetC pi-_9GeV_FullSetC pi-_10GeV_FullSetC Had+_1GeV_FullSetC Had+_2GeV_FullSetC Had+_3GeV_FullSetC Had+_4GeV_FullSetC Had+_5GeV_FullSetC Had+_6GeV_FullSetC Had+_7GeV_FullSetC Had+_8GeV_FullSetC Had+_9GeV_FullSetC Had+_10GeV_FullSetC'
	for runNr in $runs; do
		QARun $3 $dataDirOut $runNr $PlotBaseDir
	done;

elif [ $2 == "FullSetD" ]; then
	echo "running calibrate for 44V runs, summing board V2, campaign D"
	runs='Muon_FullSetD_1 Muon_FullSetD_2 pi-_1GeV_FullSetD pi-_2GeV_FullSetD pi-_3GeV_FullSetD pi-_4GeV_FullSetD pi-_5GeV_FullSetD pi-_6GeV_FullSetD pi-_7GeV_FullSetD pi-_8GeV_FullSetD pi-_9GeV_FullSetD pi-_10GeV_FullSetD Had+_1GeV_FullSetD Had+_2GeV_FullSetD Had+_3GeV_FullSetD Had+_4GeV_FullSetD Had+_5GeV_FullSetD Had+_6GeV_FullSetD Had+_7GeV_FullSetD Had+_8GeV_FullSetD Had+_9GeV_FullSetD Had+_10GeV_FullSetD'
	for runNr in $runs; do
		QARun $3 $dataDirOut $runNr $PlotBaseDir
	done;

elif [ $2 == "FullSetE" ]; then
	echo "running calibrate for 44V runs, campaign A1"
  runs='Muon_FullSetE_1 Muon_FullSetE_2' 
	for runNr in $runs; do 
		QARun $3 $dataDirOut $runNr $PlotBaseDir
	done;
fi
