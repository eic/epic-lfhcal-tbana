#! /bin/bash

source helperCalibHGCROC.sh

PlotBaseDir=..
runNrFile=../configs/LocalTesting/DataTakingDB_ORNL_HGCROC_Cosmics_Longboard_202512.csv


# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/Lennard4TB/LongboardTesting/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/LongboardTesting/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/LongboardTesting/
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

if [ $2 = "pedestal" ]; then
  #pure pedestal runs
  runs='158 161 165 166 170 173 178 180'
  for runNr in $runs; do 
    Pedestal ped $runNr $dataDirRaw $dataDirOut Run_$runNr pdf
  done
  #pedestal from cosmics
#   runs='160 162 163 164 167 171 172 175' # 176
#   runs='176'
#   for runNr in $runs; do 
#     Pedestal ped $runNr $dataDirRaw $dataDirOut Run_$runNr pdf
#   done
fi

if [ $2 = "toaPhase" ]; then 
  runs='160 162 163 164 167 171 172 175 176 179 181 182'
  for runNr in $runs; do 
    ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_mipTrigg_wPedwMuon_wBC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runNrFile -g $dataDirRaw/rawHGCROC_mipTrigg_wPedwMuon_wBC_$runNr.root #-F png
  done
fi


if [ $2 == "calibMuon" ]; then
# 	runPed='158'
# 	runNr='160'
# 	badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config1.txt"
#   MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config2.txt"
# 
# 	runPed='161'
# 	runs='162 163'
# #   runs='162' #OR
# #  	runs='163' #AND
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	done
# 	badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config3.txt"
# 	
# 	runPed='165'
# 	runs='164' #OR
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	done
#   badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config4.txt"
# 	
# 	runPed='166'
# 	runs='167' #AND
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	done
#   badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config5.txt"
# 	
# 	runPed='170'
# 	runs='171 172' #OR
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	done
#   badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config6.txt"
# 	
# 	runPed='173'
# 	runs='175 176' 
# # 	runs='175' #OR
# # 	runs='176' #AND
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	done
	
	runPed='178'
	runs='179' #AND
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
  badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config7.txt"
# 	
	runPed='180'
	runs='181 182' 
# 	runs='181' #OR
# 	runs='182' #AND
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi

if [ $2 == "calibMuonORs" ]; then	
	runs='160 162 164 171 172 175 182' #OR
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runNr $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi

if [ $2 == "calibMuonANDs" ]; then	
	runs='163 167 176 179 183' #AND
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runNr $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi
