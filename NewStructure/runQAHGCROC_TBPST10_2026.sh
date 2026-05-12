#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv

PlotBaseDir=..
if [ $1 = "fbockTB" ]; then 
	dataDirOut=/media/fbock/Lennard4TB/202604_PST10/HGCROCData/
	PlotBaseDir=/media/fbock/Lennard4TB/202604_PST10/
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

if [ $2 == "FullSetE" ]; then
	echo "running calibrate for 44V runs, campaing A1"
  runs='Muon_FullSetE_1 Muon_FullSetE_2' 
	for runNr in $runs; do 
		QARun $3 $dataDirOut $runNr $PlotBaseDir
	done;
fi
