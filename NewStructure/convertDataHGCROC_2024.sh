#! /bin/bash
echo "username $1"
echo "run option $2"

if [ $1 = "fbockTB" ]; then 
	dataDir=/media/fbock/LFHCal2/Sep2024_PS_DataBKP/dump/hadron0830
	dataRaw=/media/fbock/LFHCal2/202408_PST09/HGCROCData
elif [ $1 = "fbockCosm" ]; then 
  dataDir=/media/fbock/LFHCal2/cosmics
	dataRaw=/media/fbock/LFHCal2/cosmics/converted
fi

mapA=../configs/mapping_HGCROC_PSTB2024_Run5-67.txt
mapB=../configs/mapping_HGCROC_PSTB2024_Run68-117_alternate.txt
mapC=../configs/mapping_HGCROC_PSTB2024_Run118-337_alternate.txt
runList=../configs/DataTakingDB_202409_HGCROC.csv

if [ $2 = "muons" ]; then 
	mkdir -p $dataRaw/muons
	runs='184 185'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/muons/rawHGCROC_$runNr.root -m $mapC -r $runList
	done
	runs='275 276 277 278 279 280 281 282 283 ' #299 300
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/muons/rawHGCROC_$runNr.root -m $mapC -r $runList
	done
	runs='302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/muons/rawHGCROC_$runNr.root -m $mapC -r $runList
	done

elif [ $2 = "MergeMuons" ]; then 	
	hadd -f $dataRaw/muons/rawHGCROC_muonAll.root $dataRaw/muons/rawHGCROC_1*.root $dataRaw/muons/rawHGCROC_2*.root $dataRaw/muons/rawHGCROC_3*.root 
	runs='303 305 307 308 309 310 313 314 320 321'
	echo "" > listMerge.txt
	for runNr in $runs; do 
		ls $dataRaw/muons/rawHGCROC_$runNr.root  >> listMerge.txt
	done
	cat listMerge.txt
	fileList=`cat listMerge.txt`
	hadd -f $dataRaw/muons/rawHGCROC_muonsNeg.root $fileList
	runs='302 304 306 311 312 315 316 317 318 319'
	echo "" > listMerge.txt
	for runNr in $runs; do 
		ls $dataRaw/muons/rawHGCROC_$runNr.root  >> listMerge.txt
	done
	cat listMerge.txt
	fileList=`cat listMerge.txt`
	hadd -f $dataRaw/muons/rawHGCROC_muonsPos.root $fileList

	
	
elif [ $2 = "electrons" ]; then 
	mkdir -p $dataRaw/electrons
	# 4GeV (0,1)
	runs='327 328 329 330 331 332'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/electrons/rawHGCROC_$runNr.root -m $mapC -r $runList
	done
	#scan 1-5 GeV, (0,0)
	runs='103 104 105 106 107'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/electrons/rawHGCROC_$runNr.root -m $mapB -r $runList
	done
elif [ $2 = "hadrons" ]; then 
	mkdir -p $dataRaw/hadrons
	runs='186 187 188 189 190 191 192 193 194 195 196 197 '
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/hadrons/rawHGCROC_$runNr.root -m $mapB -r $runList
	done
	
elif [ $2 = "cosmics" ]; then 
	# local cosmics
	# runs='006'
	runs='007' # 1700000 events
	for runNr in $runs; do 
# 		./Convert -d 2 -L 10000 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/rawHGCROC_$runNr.root -m ../configs/mapping_HGCROC_Cosmics_20250606.txt -r ../configs/DataTakingDB_ORNL_HGCROC.txt
		./Convert -d 1 -L 1700000 -f -w -c $dataDir/Run$runNr.h2g -o $dataRaw/rawHGCROC_$runNr.root -m ../configs/mapping_HGCROC_Cosmics_20250606.txt -r ../configs/DataTakingDB_ORNL_HGCROC.txt		
	done
fi
