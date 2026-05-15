#! /bin/bash

# bash runCompare_TBPST10_202604.sh $USERNAME [calib/ana] [R/V] $OPTION
# R: trending plots vs run#. V: trending plots vs Vop.
# options: HVScan1, FullSetC, FullSetD

runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv

if [ $1 == "yale" ]; then
	mkdir -p ../Trending/TB2026/PS/$4
	mkdir -p ../Trending/TB2026/PS/$4/plots
	dirOut=../Trending/TB2026/PS/$4
	PlotBaseDir=../Trending/TB2026/PS/$4/plots
	calibFileList=calibFileList_202604_$4.txt
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

if [ $2 == "calib" ]; then
	if [ $3 == "V" ]; then
		plotNameSuff=Volt_$4
	#	./CompareCalib -F pdf -e 1 -d 1 -f -V -H -I $calibFileList -o $dirOut/TrendingAllCalibs_$plotNameSuff.root -O $PlotBaseDir -r $runList
	elif [ $3 == "R" ]; then
		plotNameSuff=RunNr_$4
	else
		echo "Useage: bash runCompareHGCROC_TBPST10_202604.sh $USERNAME [calib/ana] [R/V] [dataset]"
		exit
	fi
	
	./CompareCalib -F pdf -e 1 -d 1 -f -$3 -H -I $calibFileList -o $dirOut/TrendingAllCalibs_$plotNameSuff.root -O $PlotBaseDir -r $runList
	
elif [ $2 == "ana" ]; then
	echo "CompareAna has not been implemented!"
	exit
fi

