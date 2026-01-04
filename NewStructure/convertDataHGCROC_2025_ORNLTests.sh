#! /bin/bash
echo "username $1"
echo "run option $2"

if [ $1 = "fbockTB" ]; then 
	dataRaw=/media/fbock/Lennard4TB/LongboardTesting/raw                   # source directory for output files from DAQ system
	dataDir=/media/fbock/Lennard4TB/LongboardTesting/HGCROCData            # base directory for root trees
fi


# global run list for 2025 TB
runList=""

# default path for the HGCROC analysis
if [ $2 = "LBTests" ]; then 

  runList=../configs/LocalTesting/DataTakingDB_ORNL_HGCROC_Cosmics_Longboard_202512.csv
  # different mapping files for layering
  mapCon1=../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_LB_Config1.txt   # runs 158-160
  mapCon2=../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_LB_Config2.txt   # runs 161-163
  mapCon3=../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_LB_Config3.txt   # runs 164-165
  mapCon4=../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_LB_Config4.txt   # runs 166-167
  mapCon5=../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_LB_Config5.txt   # runs 170-172
  mapCon6=../configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_LB_Config6.txt   # runs 173-176

  mkdir -p $dataDir/
# 	runs='158 159 160'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon1 -r $runList
# 	done
# 	runs='161 162 163'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon2 -r $runList
# 	done
# 	runs='164 165'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon3 -r $runList
# 	done
# 	runs='166 167'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon4 -r $runList
# 	done
# 	runs='170 171 172'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon5 -r $runList
# 	done
# 	runs='173 175'
	runs='176'
	for runNr in $runs; do 
		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapCon6 -r $runList
	done
	

# elif [ $2 = "muons" ]; then 
#   # position scan
# 	mkdir -p $dataRaw
# # 	runs='007 008 009 010 011 012 013 014 015 016 017 018 019 020 021 022 023 024 025 026'
# # 	runs='007 008 009 010'
# # 	for runNr in $runs; do 
# # 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# # 	done
# 	# HV Scan
# # 	runs='027 028 029 030 031 032 033 034 035'
# # 	for runNr in $runs; do 
# # 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# # 	done
# # 	
# 	# default mapping 44 V
# 	runs='069 070 071 072 073 074 075 076 157 164 201 202 203 204 205 206'
# 	runs='164'
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
# # 	
# # 	# default mapping 45 V
# # 	runs='211 '
# # 		runs='211 212 213 214 215 216 217 252 253 254 256 257 258 '
#   runs='256 257 258 '
# 	for runNr in $runs; do 
# 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# 	done
# # 	
# # 	# HV Scan
# # 	runs='260 261 262 263 264 265 266 267 268 271 272'
# # 	for runNr in $runs; do 
# # 		./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapConDef -r $runList
# # 	done
# 	
fi
