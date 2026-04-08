#! /bin/bash

function MergeMuonsFileList(){
  echo "========================================================================="
  echo "dataDir: $1"
  echo "run-list: $2"
  echo "Additional name: $3"
  echo "========================================================================="
  runs=`cat $2`
  if [ -f listMerge.txt ]; then
    rm listMerge.txt
  fi
	for runNr in $runs; do 
		ls $1/rawHGCROC_$runNr.root  >> listMerge.txt
	done
	cat listMerge.txt
	fileList=`cat listMerge.txt`
	hadd -f $1/rawHGCROC_$3.root $fileList
  rm listMerge.txt
}

echo "username $1"
echo "run option $2"

# different mapping files for layering
mapConDefV2=../configs/TB2026/mapping_HGCROC_PST10TB_sumV2_default.csv   # v2 summing board
mapConDefV1=../configs/TB2026/mapping_HGCROC_PST10TB_sumV1_default.csv   # v1 summing board


if [ $1 = "fbockTB" ]; then 
	dataRaw=/media/fbock/Lennard4TB/202604_PST10/raw/      # source directory for output files from DAQ system
	dataDir=/media/fbock/Lennard4TB/202604_PST10/HGCROCData           # base directory for root trees

elif [ $1 = "egpott" ]; then
	dataRaw=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/raw
	dataDir=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot

elif [ $1 = "ehagen" ]; then 
	dataRaw=/Volumes/UWU/25_TB_Data
	dataDir=/Volumes/UWU/25_TB_Data

fi
  
# global run list for 2026 PS TB
runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv

# default path for the HGCROC analysis
if [ $2 = "pedestals" ]; then 
	mkdir -p $dataDir/
	# pedestals with different Nr of asics
	echo "insert runnumbers first in this option, then remove exit below" 
	exit
  runs=''
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
	done
elif [ $2 = "muons" ]; then 
	mkdir -p $dataDir/
	echo "insert runnumbers first in this option, then remove exit below" 
	exit
	runs=''
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
	done
	
elif [ $2 = "MergeMuons" ]; then 	
  echo "Merging muon files, fix your runnumbers, example below in code"
  exit
  runs='069 070 071 072 073 074 075 076 164' # Full Set A - muon set 1
  echo $runs > runList.txt
  MergeMuonsFileList $dataDir runList.txt FullSetA_1 
  
elif [ $2 = "electrons" ]; then 
	mkdir -p $dataDir/
	echo "insert runnumbers first in this option, then remove exit below" 
  exit

  runs=''
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
	done

elif [ $2 = "positrons" ]; then 
	mkdir -p $dataRaw
	echo "insert runnumbers first in this option, then remove exit below" 
  exit
	runs=''
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
	done
	
elif [ $2 = "hadrons" ]; then 
	mkdir -p $dataRaw
	echo "insert runnumbers first in this option, then remove exit below" 
  exit

  runs=''
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
	done
	
fi
