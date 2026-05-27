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
	calibFileList=./file_lists/TB2026/calibFileList_202604_$4.txt
	anaFileList=./file_lists/TB2026/anaFileList_202604_$4.txt
else
	echo "Useage: bash runCompareHGCROC_TBPST10_202604.sh [username] [calib/ana] [R/V] [dataset]"
	exit
fi

if [ $3 == "V" ]; then
	plotNameSuff=Volt_$4
elif [ $3 == "R" ]; then
	plotNameSuff=RunNr_$4
else
	echo "Useage: bash runCompareHGCROC_TBPST10_202604.sh [username] [calib/ana] [R/V] [dataset]"
	exit
fi

if [ $2 == "calib" ]; then
	if [ $4 == "HVScan1_OG" ]; then
		./CompareCalib -F pdf -e 1 -d 1 -f -$3 -H -s -I $calibFileList -o $dirOut/TrendingAllCalibs_$plotNameSuff.root -O $PlotBaseDir -r $runList
	else
	./CompareCalib -F pdf -e 1 -d 1 -f -$3 -H -I $calibFileList -o $dirOut/TrendingAllCalibs_$plotNameSuff.root -O $PlotBaseDir -r $runList
	fi

elif [ $2 == "ana" ]; then
	if [[ $4 == *"HVScan"* ]]; then
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -x 600 -s -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	elif [ $4 == "FullSetC_OG" || $4 == "FullSetC_OG_pions" || $4 == "FullSetC_OG_hadrons"]; then
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -x 150 -c -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	elif [ $4 == "FullSetC" ]; then
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -x 700 -C 3 -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	elif [ $4 == "FullSetC_pions" ]; then
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -x 700 -c -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	elif [ $4 == "FullSetC_hadrons" ]; then
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -x 700 -c -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	elif [ $4 == "FullSetC_OG_piHad" ]; then
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -C 6 -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	elif [ $4 == "FullSetD_piHad" ]; then
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -C 6 -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	elif [[ $4 == *"CD"* ]]; then
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -C 6 -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	else
		./CompareAna -F pdf -e 1 -d 0 -f -$3 -I $anaFileList -o $dirOut/TrendingAllAna_$plotNameSuff.root -O $PlotBaseDir -r $runList
	fi
else
	echo "Useage: bash runCompareHGCROC_TBPST10_202604.sh [username] [calib/ana] [R/V] [dataset]"
	exit
fi
