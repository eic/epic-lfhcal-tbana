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
mapConDefV2=../configs/TB2026/mapping_HGCROC_PST10TB_sumV2_default_inv.csv   # v2 summing board
mapConDefV1=../configs/TB2026/mapping_HGCROC_PST10TB_sumV1_default_inv.csv   # v1 summing board


if [ $1 = "fbockTB" ]; then 
	dataRaw=/media/fbock/Lennard4TB/202604_PST10/raw/      # source directory for output files from DAQ system
	dataDir=/media/fbock/Lennard4TB/202604_PST10/HGCROCData           # base directory for root trees

elif [ $1 = "egpott" ]; then
	dataRaw=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/raw
	dataDir=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot

elif [ $1 = "ehagen" ]; then 
	dataRaw=/Users/hagen/Githubs/TB_data
	dataDir=/Users/hagen/Githubs/TB_data
elif [ $1 = "dkapukch" ]; then
  dataRaw=/Users/davidkapukchyan/Documents/ePIC/LFHCal_TestBeam/HvScan42VFromDaq
  dataDir=/Users/davidkapukchyan/Documents/ePIC/LFHCal_TestBeam/HvScan42VFromDaq/ConvertedData
fi
  
# global run list for 2026 PS TB
runList=../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv

# default path for the HGCROC analysis
if [ $2 = "pedestals" ]; then 
	mkdir -p $dataDir/
	# pedestals with different Nr of asics
  #runs='001 039 066 070 079 080 082 085'
  runs='070'
	for runNr in $runs; do 
		./build/Convert -d 3 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
	done
elif [ $2 = "muons" ]; then 
	mkdir -p $dataDir/
  # runs='002 005 006 007 040 041 042 043 044 045 046 047 048 049 050 051 052 053 054 055 056 057 058 086 087 088 089 090 091 092'
#   
  runs='071 072 073 074'

	for runNr in $runs; do 
		./build/Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
	done
	
elif [ $2 = "MergeMuons" ]; then 	
  #runs='040 041 042' # firstMuons
  runs='071 072 073 074'
  echo $runs > runList.txt
  MergeMuonsFileList $dataDir runList.txt FirstMuons 

  # position scan
  #runs='040 041 042 043 044 045 046 047 048 049' # firstMuons 0,0
  #echo $runs > runList.txt
  #MergeMuonsFileList $dataDir runList.txt FMuonCent
  #runs='050 051 052' # firstMuons 0,5
  #echo $runs > runList.txt
  #MergeMuonsFileList $dataDir runList.txt FMuon_0_5
  #runs='053 054 055' # firstMuons 5,-5
  #echo $runs > runList.txt
  #MergeMuonsFileList $dataDir runList.txt FMuon_5_-5
  #runs='056 057' # firstMuons 0,-5
  #echo $runs > runList.txt
  #MergeMuonsFileList $dataDir runList.txt FMuon_0_-5
  # firstMuons -5,-5
  #cp $dataDir/rawHGCROC_058.root $dataDir/rawHGCROC_FMuon_-5_-5.root 
  # firstMuons -5,0
  #cp $dataDir/rawHGCROC_059.root $dataDir/rawHGCROC_FMuon_-5_0.root 
  # firstMuons 0,5
  #cp $dataDir/rawHGCROC_062.root $dataDir/rawHGCROC_FMuon_0_5.root 
  # firstMuons 5,5
  #cp $dataDir/rawHGCROC_063.root $dataDir/rawHGCROC_FMuon_5_5.root 
  #runs='060 061' # firstMuons -5,0
  #echo $runs > runList.txt
  #MergeMuonsFileList $dataDir runList.txt FMuon_-5_0

elif [ $2 = "PositionMuons" ]; then 	
  # position scan
  runs='091' # Muons 5,5
  echo $runs > runList.txt
  MergeMuonsFileList $dataDir runList.txt Muon_5_5
  runs='092' # Muons -5,5
  echo $runs > runList.txt
  MergeMuonsFileList $dataDir runList.txt Muon_-5_5
  runs='089' # Muons -5,-5
  echo $runs > runList.txt
  MergeMuonsFileList $dataDir runList.txt Muon_-5_-5
  runs='090' # Muons 5,-5
  echo $runs > runList.txt
  MergeMuonsFileList $dataDir runList.txt Muon_5_-5
  # firstMuons 5,5
  cp $dataDir/rawHGCROC_091.root $dataDir/rawHGCROC_Muon_-5_-5.root 
  # firstMuons -5,5
  cp $dataDir/rawHGCROC_092.root $dataDir/rawHGCROC_Muon_-5_0.root 
  # firstMuons -5,-5
  cp $dataDir/rawHGCROC_089.root $dataDir/rawHGCROC_Muon_0_5.root 
  # firstMuons 5,-5
  cp $dataDir/rawHGCROC_090.root $dataDir/rawHGCROC_Muon_5_5.root 


elif [ $2 = "electrons" ]; then 
	mkdir -p $dataDir/
	echo "insert runnumbers first in this option, then remove exit below" 
  exit

  runs=''
	for runNr in $runs; do 
		./build/Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
	done

elif [ $2 = "positrons" ]; then 
	mkdir -p $dataRaw
	echo "insert runnumbers first in this option, then remove exit below" 
  exit
	runs=''
	for runNr in $runs; do 
		./build/Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
	done
	
elif [ $2 = "hadrons" ]; then 
	mkdir -p $dataRaw
	echo "insert runnumbers first in this option, then remove exit below" 
  exit

  runs=''
	for runNr in $runs; do 
		./build/Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
	done
	
fi
