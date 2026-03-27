#! /bin/bash
echo "username $1"
echo "run option $2"

if [ $1 = "fbockTB" ]; then 
	dataRaw=/media/fbock/Lennard4TB/SummingBoardTestFull/raw/                   # source directory for output files from DAQ system
	dataDir=/media/fbock/Lennard4TB/SummingBoardTestFull/HGCROCData            # base directory for root trees
fi


# global run list for 2025 TB
runList=""

# default path for the HGCROC analysis
if [ $2 = "SumTests" ]; then 

  runList=../configs/LocalTesting/DataTakingDB_ORNL_HGCROC_Cosmics_Summing_202603.csv
  # different mapping files for layering
  mapCon=../configs/LocalTesting/mapping_HGCROC_ORNL_SummingTest_2026_1.txt
  
  mkdir -p $dataDir/
	runs='206 207'
# 	runs='207'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon -r $runList
# 	done
  
  mapCon=../configs/LocalTesting/mapping_HGCROC_ORNL_SummingTest_2026_v1_2.txt
	runs='223 224 225 226 227'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon -r $runList
# 	done

  mapCon=../configs/LocalTesting/mapping_HGCROC_ORNL_SummingTest_2026_v2_2.txt
	runs='228 229'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon -r $runList
	done	
fi
