#! /bin/bash
echo "username $1"
echo "run option $2"

if [ $1 = "fbockTB" ]; then 
	dataRaw=/media/fbock/LFHCal2/Sep2024_PS_DataBKP/dump/hadron0830      # source directory for output files from DAQ system
	dataDir=/media/fbock/LFHCal2/202408_PST09/HGCROCData                 # base directory for root trees
elif [ $1 = "fbockCosm" ]; then 
  dataRaw=/media/fbock/LFHCal2/cosmics/TestSetupJune                  # source directory for output files from DAQ system
	dataDir=/media/fbock/LFHCal2/cosmics/TestSetupJune/converted        # base directory for root trees
elif [ $1 = "fbockCosmOct" ]; then 
  dataRaw=/media/fbock/LFHCal2/cosmics/TestSetupOct/raw                # source directory for output files from DAQ system
	dataDir=/media/fbock/LFHCal2/cosmics/TestSetupOct/converted          # base directory for root trees
elif [ $1 = "fbockSum" ]; then 
  dataRaw=/media/fbock/LFHCal2/SummingTest/rawHGCROC                   # source directory for output files from DAQ system
	dataDir=/media/fbock/LFHCal2/SummingTest/convertedHGCROC             # base directory for root trees
fi

mapA=../configs/mapping_HGCROC_PSTB2024_Run5-67_alternate.txt
mapB=../configs/mapping_HGCROC_PSTB2024_Run68-117_alternate.txt
mapC=../configs/mapping_HGCROC_PSTB2024_Run118-337_alternate.txt
runList=../configs/DataTakingDB_202409_HGCROC.csv

# default path for the HGCROC analysis
if [ $2 = "muons" ]; then 
	mkdir -p $dataRaw/muons
	runs='184 185'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/muons/rawHGCROC_$runNr.root -m $mapC -r $runList
	done
	runs='275 276 277 278 279 280 281 282 283 ' #299 300
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/muons/rawHGCROC_$runNr.root -m $mapC -r $runList
	done
	runs='302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/muons/rawHGCROC_$runNr.root -m $mapC -r $runList
	done

elif [ $2 = "MergeMuons" ]; then 	
	hadd -f $dataDir/muons/rawHGCROC_muonAll.root $dataDir/muons/rawHGCROC_1*.root $dataDir/muons/rawHGCROC_2*.root $dataDir/muons/rawHGCROC_3*.root 
	runs='303 305 307 308 309 310 313 314 320 321'
	echo "" > listMerge.txt
	for runNr in $runs; do 
		ls $dataDir/muons/rawHGCROC_$runNr.root  >> listMerge.txt
	done
	cat listMerge.txt
	fileList=`cat listMerge.txt`
	hadd -f $dataDir/muons/rawHGCROC_muonsNeg.root $fileList
	runs='302 304 306 311 312 315 316 317 318 319'
	echo "" > listMerge.txt
	for runNr in $runs; do 
		ls $dataDir/muons/rawHGCROC_$runNr.root  >> listMerge.txt
	done
	cat listMerge.txt
	fileList=`cat listMerge.txt`
	hadd -f $dataDir/muons/rawHGCROC_muonsPos.root $fileList

elif [ $2 = "electrons" ]; then 
	mkdir -p $dataRaw/electrons
	# 4GeV (0,1)
	runs='327 328 329 330 331 332'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataRaw/electrons/rawHGCROC_$runNr.root -m $mapC -r $runList
	done
	#scan 1-5 GeV, (0,0)
	runs='103 104 105 106 107'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataRaw/electrons/rawHGCROC_$runNr.root -m $mapB -r $runList
	done
elif [ $2 = "hadrons" ]; then 
	mkdir -p $dataRaw/hadrons
	runs='186 187 188 189 190 191 192 193 194 195 196 197 '
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataRaw/hadrons/rawHGCROC_$runNr.root -m $mapB -r $runList
	done
	

# switched on truncation for HGCROC data as alternate path	- EXPERIMENTAL
elif [ $2 = "muonsTruncated" ]; then 
	mkdir -p $dataRaw/muons
	runs='184 185'
	for runNr in $runs; do 
		./Convert -d 0 -t -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/muons/rawHGCROCtruncated_$runNr.root -m $mapC -r $runList
	done
	runs='275 276 277 278 279 280 281 282 283 ' #299 300
	for runNr in $runs; do 
		./Convert -d 0 -t -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/muons/rawHGCROCtruncated_$runNr.root -m $mapC -r $runList
	done
	runs='302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321'
	for runNr in $runs; do 
		./Convert -d 0 -t -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/muons/rawHGCROCtruncated_$runNr.root -m $mapC -r $runList
	done

elif [ $2 = "MergeMuonsTruncated" ]; then 	
	hadd -f $dataDir/muons/rawHGCROCtruncated_muonAll.root $dataDir/muons/rawHGCROCtruncated_1*.root $dataDir/muons/rawHGCROCtruncated_2*.root $dataDir/muons/rawHGCROCtruncated_3*.root 
	runs='303 305 307 308 309 310 313 314 320 321'
	echo "" > listMerge.txt
	for runNr in $runs; do 
		ls $dataDir/muons/rawHGCROCtruncated_$runNr.root  >> listMerge.txt
	done
	cat listMerge.txt
	fileList=`cat listMerge.txt`
	hadd -f $dataDir/muons/rawHGCROCtruncated_muonsNeg.root $fileList
	runs='302 304 306 311 312 315 316 317 318 319'
	echo "" > listMerge.txt
	for runNr in $runs; do 
		ls $dataDir/muons/rawHGCROCtruncated_$runNr.root  >> listMerge.txt
	done
	cat listMerge.txt
	fileList=`cat listMerge.txt`
	hadd -f $dataDir/muons/rawHGCROCtruncated_muonsPos.root $fileList
	
	
elif [ $2 = "cosmics" ]; then 
	# local cosmics
	# runs='006'
	runs='007' # 1700000 events
	for runNr in $runs; do 
# 		./Convert -d 2 -L 10000 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/mapping_HGCROC_Cosmics_20250606.txt -r ../configs/DataTakingDB_ORNL_HGCROC.txt
		./Convert -d 1 -L 1700000 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/mapping_HGCROC_Cosmics_20250606.txt -r ../configs/DataTakingDB_ORNL_HGCROC.txt		
	done
elif [ $2 = "skimCosmics" ]; then 
	runs='007' # 1700000 events
	for runNr in $runs; do 
    ./DataPrep -d 1 -f -X -i $dataDir/rawHGCROC_$runNr.root -o $dataDir/rawHGCROCskimmed_$runNr.root -r ../configs/DataTakingDB_ORNL_HGCROC.txt
	done
elif [ $2 = "cosmicsOct" ]; then 
	# local cosmics first stack
# 	runs='004 005 008 013 014' 
runs='017' 
	for runNr in $runs; do 
		./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/mapping_HGCROC_ORNL_Cosmics_20251009.txt -r ../configs/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt		
	done
elif [ $2 = "summing" ]; then 
	# local cosmics first stack
# 	runs='004 005 008 013 014' 
  mappingFile=../configs/mapping_HGCROC_ORNL_Summing1ch.txt
  runNr='100' 
  ./Convert -L 1539 -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='101'
  ./Convert -L 1540 -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='102'
  ./Convert -L 1541 -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='103'
  ./Convert -L 1541 -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='104'
  ./Convert -L 1540 -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='105'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='106'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='107'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='108'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='109'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='110'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='111'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='112'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='113'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='114'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  runNr='115'
  ./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mappingFile -r ../configs/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  

# 	local cosmics second stack
# 	runs='018' 
# 	
# 	for runNr in $runs; do 
# 		./Convert -d 1 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m ../configs/mapping_HGCROC_ORNL_Cosmics_20251014.txt -r ../configs/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt		
# 	done
	
fi
