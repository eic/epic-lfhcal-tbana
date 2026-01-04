#! /bin/bash
echo "username $1"
echo "run option $2"

if [ $1 = "fbockTB" ]; then 
	dataRaw=/media/fbock/Lennard4TB/202511_PST09/raw/TBMain2025/      # source directory for output files from DAQ system
	dataDir=/media/fbock/Lennard4TB/202511_PST09/HGCROCData           # base directory for root trees
fi

# different mapping files for layering
mapConDef=../configs/TB2025/mapping_HGCROC_PSTB2025_default.txt   # layers 0-32 equipped
mapCon2=../configs/TB2025/mapping_HGCROC_PSTB2025_config2.txt     # layers 0-24, 33-40 equipped
mapCon3=../configs/TB2025/mapping_HGCROC_PSTB2025_config3.txt     # layers 0-16, 25-32, 41-49
mapCon4=../configs/TB2025/mapping_HGCROC_PSTB2025_config4.txt     # layers 0-16, 33-40, 50-58
  
# global run list for 2025 TB
runList=../configs/TB2025/DataTakingDB_202511_HGCROC.csv

# default path for the HGCROC analysis
if [ $2 = "pedestals" ]; then 
	mkdir -p $dataDir/
	# pedestals with different Nr of asics
# 	runs='122 123 124 125 126 128 129'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
# 	# pedestals with different Nr of asics
	runs='036 055 056 058 059 068 130 141 156 161 207 208 209 210 259 269 270 291 292 293 295 314 315 316 317 318 321 322 323 381 391'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
	

elif [ $2 = "muons" ]; then 
  # position scan
	mkdir -p $dataRaw
	runs='007 008 009 010 011 012 013 014 015 016 017 018 019 020 021 022 023 024 025 026'
# 	runs='007 008 009 010'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
	# HV Scan
	runs='027 028 029 030 031 032 033 034 035'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
# 	
	# default mapping 44 V
	runs='069 070 071 072 073 074 075 076 157 164 201 202 203 204 205 206'
# 	runs='164'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
# 	
	# default mapping 45 V
# 	runs='211 '
  runs='211 212 213 214 215 216 217 252 253 254 256 257 258 '
#   runs='256 257 258 '
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
# 	
	# HV Scan
	runs='260 261 262 263 264 265 266 267 268 271 272'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
# 	
	

elif [ $2 = "MergeMuons" ]; then 	
  echo "nothing to be done"
# 	hadd -f $dataDir/rawHGCROC_muonAll.root $dataDir/rawHGCROC_1*.root $dataDir/rawHGCROC_2*.root $dataDir/rawHGCROC_3*.root 
# 	runs='303 305 307 308 309 310 313 314 320 321'
# 	echo "" > listMerge.txt
# 	for runNr in $runs; do 
# 		ls $dataDir/rawHGCROC_$runNr.root  >> listMerge.txt
# 	done
# 	cat listMerge.txt
# 	fileList=`cat listMerge.txt`
# 	hadd -f $dataDir/rawHGCROC_muonsNeg.root $fileList
# 	runs='302 304 306 311 312 315 316 317 318 319'
# 	echo "" > listMerge.txt
# 	for runNr in $runs; do 
# 		ls $dataDir/rawHGCROC_$runNr.root  >> listMerge.txt
# 	done
# 	cat listMerge.txt
# 	fileList=`cat listMerge.txt`
# 	hadd -f $dataDir/rawHGCROC_muonsPos.root $fileList

elif [ $2 = "electrons" ]; then 
	mkdir -p $dataRaw
# default mapping 44 V
	runs='165 166 167 168 169 170'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
elif [ $2 = "positrons" ]; then 
	mkdir -p $dataRaw
# default mapping 44 V
	runs='171 172 173 174 175'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done
elif [ $2 = "hadrons" ]; then 
  # 15 GeV h+ beginning
	runs='037 038 039 040 041 042 043 046 047 048 049 050 051 052 053 054'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
	done

#   runs='188'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
fi
