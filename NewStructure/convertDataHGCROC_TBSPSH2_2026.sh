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
mapConDefV2=../configs/TB2026/mapping_HGCROC_SPSH2TB_sumV2_default.csv   # v2 summing board
mapConDefV1=../configs/TB2026/mapping_HGCROC_PST10TB_sumV1_default_inv.csv   # v1 summing board


if [ $1 = "fbockTB" ]; then 
	dataRaw=/media/fbock/Lennard4TB/202605_SPSH2/raw/      # source directory for output files from DAQ system
	dataDir=/media/fbock/Lennard4TB/202605_SPSH2/HGCROCData           # base directory for root trees

elif [ $1 = "egpott" ]; then # bla bla bla test test test
	dataRaw=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/raw
	dataDir=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot

elif [ $1 = "ehagen" ]; then 
	dataRaw=/Users/hagen/Githubs/TB_data
	dataDir=/Users/hagen/Githubs/TB_data
	
elif [ $1 = "atamis" ]; then 
	dataRaw=/home/drewtam20/Documents/eic/LFHCALDATA/Runs
	dataDir=/home/drewtam20/Documents/eic/LFHCALDATA/Converted
fi
  
# global run list for 2026 SPS TB
runList=../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv

# default path for the HGCROC analysis	
#V2 summing board, initial preamp settings 
if [ $2 = "InitMuon" ]; then 	
  if [ $3 = "convert" ]; then 
#     runs='001 002 003'
    runs='004 005 006 007 008 009 010 011 012 013 014 015 016'
#     runs='001 002 003 004 005 006 007 008 009 010 011 012 013 014 015 016 017'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then  
    echo "not defined yet"
  fi
fi
