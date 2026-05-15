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
#     runs='004 005 006 007 008 009 010 011 012 013 014 015 016'
#       runs='046 047 017 018 019 020 021 022 '  #046 047
#       runs='023 024 025 026 027 028 029 030 031 032 033 034 035 036 037 038 039 '  
#       runs='017 018 019 020 021 022 '  
#       runs='048 049 '  
#       runs='050 051 '  
#       runs='052'  
      # fixed mapping starting with 52
#       runs='052 053 054 055 056 057 058 059 060 '  
#       runs='048 049 061 062 063 064 065 066 067 068 069 070 '  
        runs='030 031 032 033 034 035 036 037 038 039 040 041 042 043 044 045 046 047'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then  
    echo "not defined yet"
  fi
# 43 V, summing board V2, Preamp settings 9 7 10 1?
elif [ $2 = "FullSetB" ]; then 
  if [ $3 = "convert" ]; then 
#     runs='' #full list
#         runs='071 072 073 074 075 076 077 078 079 080 081 082 083 084' #full list
#       runs='071' #pedestals
#       runs='072 073 074 075 076 077 078 079 080 081 082 083 084' #muons set 1
      runs=' 085 086 067 088 089 089 090' #e-
    #   runs='091 092 093 094 095 096 097' #e+
    #   runs='' #pi-
    #   runs='' #h+
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDefV2 -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='072 073 074 075 076 077 078 079 080 081 082 083 084' # set 1
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt Muon_FullSetB_1  #ok
  fi
  
fi
