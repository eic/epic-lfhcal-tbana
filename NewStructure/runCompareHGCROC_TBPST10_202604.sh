#! /bin/bash

# bash runCompareHGCROC_TBPST10_202604.sh $USERNAME [calib/ana] [R/V] $OPTION
# R: trending plots vs run#. V: trending plots vs Vop.
# options: HVScan1, FullSetC, FullSetD

runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv

if [ $1 == "yale" ]; then
	mkdir -p ../Trending/$2/TB2026/PS/$4/$3
	mkdir -p ../Trending/$2/TB2026/PS/$4/$3/plots
	dirOut=../Trending/$2/TB2026/PS/$4/$3
	PlotBaseDir=../Trending/$2/TB2026/PS/$4/$3/plots
	calibFileList=calibFileList_202604_$4.txt
	anaFileList=anaFileList_202604_$4.txt
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

if [ $3 == "V" ]; then
	plotNameSuff=Volt_$4
elif [ $3 == "R" ]; then
	plotNameSuff=RunNr_$4
else
	echo "Useage: bash runCompareHGCROC_TBPST10_202604.sh $USERNAME [calib/ana] [R/V] [dataset]"
	exit
fi

if [ $2 == "calib" ]; then
#	if [ $3 == "V" ]; then
#		plotNameSuff=Volt_$4
#	elif [ $3 == "R" ]; then
#		plotNameSuff=RunNr_$4
#	else
#		echo "Useage: bash runCompareHGCROC_TBPST10_202604.sh $USERNAME [calib/ana] [R/V] [dataset]"
#		exit
#	fi	
	./CompareCalib -F pdf -e 1 -d 1 -f -$3 -H -I $calibFileList -o $dirOut/TrendingAllCalibs_$plotNameSuff.root -O $PlotBaseDir -r $runList
	
elif [ $2 == "ana" ]; then
	if [[ $4 == *"HVScan"* ]]; then
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -s -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	else
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	fi
else
	echo "Useage: bash runCompareHGCROC_TBPST10_202604.sh $USERNAME [calib/ana] [R/V] [dataset]"
	exit
fi
