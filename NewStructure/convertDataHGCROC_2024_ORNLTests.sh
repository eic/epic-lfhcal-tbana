#! /bin/bash
echo "username $1"
echo "run option $2"

if [ $1 = "fbockCosm" ]; then 
  dataRaw=/media/fbock/LFHCal2/cosmics/TestSetupJune                  # source directory for output files from DAQ system
  dataDir=/media/fbock/LFHCal2/cosmics/TestSetupJune/converted        # base directory for root trees
elif [ $1 = "fbockCosmOct" ]; then 
  dataRaw=/media/fbock/LFHCal2/cosmics/TestSetupOct/raw                # source directory for output files from DAQ system
  dataDir=/media/fbock/LFHCal2/cosmics/TestSetupOct/converted          # base directory for root trees
elif [ $1 = "fbockSum" ]; then 
  dataRaw=/media/fbock/LFHCal2/SummingTest/rawHGCROC                   # source directory for output files from DAQ system
  dataDir=/media/fbock/LFHCal2/SummingTest/convertedHGCROC             # base directory for root trees
elif [ $1 = "eglimos_CosmOct" ]; then
  dataRaw=/home/ewa/EIC/DATA/HGCROCData/Cosmics_Oct25/raw
  dataDir=/home/ewa/EIC/DATA/HGCROCData/Cosmics_Oct25/converted
fi

if [ $2 = "cosmics" ]; then 
  # local cosmics
  # runs='006'
  runs='007' # 1700000 events
  for runNr in $runs; do 
# 		./Convert -d 2 -L 10000 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/LocalTesting/mapping_HGCROC_Cosmics_20250606.txt -r ../configs/LocalTesting/DataTakingDB_ORNL_HGCROC.txt
    ./Convert -d 1 -L 1700000 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/LocalTesting/mapping_HGCROC_Cosmics_20250606.txt -r ../configs/LocalTesting/DataTakingDB_ORNL_HGCROC.txt		
  done
elif [ $2 = "skimCosmics" ]; then 
  runs='007' # 1700000 events
  for runNr in $runs; do 
    ./DataPrep -d 1 -f -X -i $dataDir/rawHGCROC_$runNr.root -o $dataDir/rawHGCROCskimmed_$runNr.root -r ../configs/LocalTesting/DataTakingDB_ORNL_HGCROC.txt
  done
elif [ $2 = "cosmicsOct" ]; then 
  # local cosmics first stack
# 	runs='004 005 008 013 014' 
#   runs='017' 
# 	for runNr in $runs; do 
# 		./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_20251009.txt -r ../configs/LocalTesting/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt		
# 	done
  
# 	runs='018' # F-Stack,  ORNL-01
# 	for runNr in $runs; do 
# 		./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_20251014.txt -r ../configs/LocalTesting/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt		
# 	done
# 
# 	runs='020 021 022' # F-Stack,  UCR-01 asic 1
# 	for runNr in $runs; do 
# 		./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_20251014.txt -r ../configs/LocalTesting/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt		
# 	done
# 
# 	runs="028 029" # F-Stack,  UCR-01 asic 0
# 	for runNr in $runs; do 
# 		./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_20251017_FStack.txt -r ../configs/LocalTesting/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt		
# 	done
#   
  # both stacks 
# 	runs='030 031 032 033 034'  #UCR-01 30-32, UCR-02 33-41, starting 38 T0A lower DAC calib
# 	runs='030 031 032 033 034 036 037 038 039 040 041'  #UCR-01 30-32, UCR-02 33-41, starting 38 T0A lower DAC calib
# 	runs='036 037 038 039 040 041'  #UCR-01 30-32, UCR-02 33-41, starting 38 T0A lower DAC calib
  runs='051 052 053 054'  #UCR-02 51-52, UCR-01 53-54, low ToA
  for runNr in $runs; do 
    ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_20251017.txt -r ../configs/LocalTesting/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt		
  done
  
  
elif [ $2 = "summing" ]; then 
  # local cosmics first stack
# 	runs='004 005 008 013 014' 
  mappingFile=../configs/LocalTesting/mapping_HGCROC_ORNL_Summing1ch.txt
  runNrFile=../configs/LocalTesting/DataTakingDB_ORNL_Summing_HGCROC_202508.txt
  runNr='100' 
  ./Convert -L 1539 -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='101'
  ./Convert -L 1540 -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='102'
  ./Convert -L 1541 -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='103'
  ./Convert -L 1541 -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='104'
  ./Convert -L 1540 -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='105'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='106'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='107'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='108'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='109'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='110'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='111'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='112'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='113'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='114'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  runNr='115'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r $runNrFile		
  

# 	local cosmics second stack
# 	runs='018' 
# 	
# 	for runNr in $runs; do 
# 		./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/mapping_HGCROC_ORNL_Cosmics_20251014.txt -r ../configs/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt		
# 	done
  
fi
