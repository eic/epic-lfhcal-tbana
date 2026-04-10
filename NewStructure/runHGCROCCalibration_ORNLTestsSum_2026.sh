#!/bin/bash


#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

PlotBaseDir=""
runNrFile=../configs/LocalTesting/DataTakingDB_ORNL_HGCROC_Cosmics_Summing_202603.csv

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/Lennard4TB/SummingBoardTestFull/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/SummingBoardTestFull/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/SummingBoardTestFull/
elif [ $1 = "ehagen" ]; then
	dataDirRaw=/Users/hagen/Downloads/data/converted
	dataDirOut=/Users/hagen/Downloads/data/converted
	PlotBaseDir=/Users/hagen/Downloads/data
elif [ $1 = "dkapukch" ]; then
	dataDirRaw="/Users/davidkapukchyan/Documents/ePIC/LFHCal_TestBeam/DataCosmicsDir"
	dataDirOut="/Users/davidkapukchyan/Documents/ePIC/LFHCal_TestBeam/DataCosmicsDir"   #Stuff below needs out dir and data dir to be the dame
	PlotBaseDir="/Users/davidkapukchyan/Documents/ePIC/LFHCal_TestBeam/PlotsCosmicsDir"
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

if [ $2 = "pedestal" ]; then
  #pedestal from cosmics
#   runs='206 207'
#   runs='223'
#   runs='228 229'
  runs='231'
  for runNr in $runs; do 
    Pedestal ped $runNr $dataDirRaw $dataDirOut Run_$runNr pdf
  done
fi

if [ $2 = "toaPhase" ]; then 
#   runs='206 207'
  runs='206 207'
  for runNr in $runs; do 
    ./build/DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_mipTrigg_wPedwMuon_wBC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runNrFile -g $dataDirRaw/rawHGCROC_mipTrigg_wPedwMuon_wBC_$runNr.root #-F png
  done
fi


if [ $2 == "calibMuon" ]; then	
  #implemented options: BC, BCTOA, default, imp1st, imp2nd, imp3rd, imp4th, saveNewMuon, imp1st_red, imp2nd_red, imp3rd_red, imp4th_red 
  badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_ST.txt"
  toaPhaseOffset=../configs/LocalTesting/ToAOffsets_Cosmics_ST_ORNL02.csv
# 	runPed='206'
# 	runs='206' #OR
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset
# 	done
#   runPed='207'
# 	runs='207' 
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset
# 	done
  toaPhaseOffset=../configs/LocalTesting/ToAOffsets_Cosmics_ST_ORNL02_22x.csv
#   runPed='223'
# 	runs='224 225' 
# 	runs='225 226_227'
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset
# 	done

  runPed='231'
	#runs='232_234'
	runs='232'
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset
	done

fi

if [ $2 = "wave" ]; then  
  runs='207'
	for runNr in $runs; do 
		WaveformHGCROC $3 $runNr $dataDirRaw Run_$runNr
	done
fi

if [ $2 == "calib" ]; then
# 	calibFile1=/media/fbock/Lennard4TB/SummingBoardTestFull/HGCROCData/rawHGCROC_wPedwMuon_wBC_Imp2_207.root
#   toaPhaseOffset=../configs/LocalTesting/ToAOffsets_Cosmics_ST_ORNL02.csv
# 	badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_ST.txt"	
	#muon runs
# 	Calib $3 $calibFile1 $dataDirOut $dataDirOut 207 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
# 	calibFile1=/media/fbock/Lennard4TB/SummingBoardTestFull/HGCROCData/rawHGCROC_wPedwMuon_wBC_Imp2_225.root
# 	toaPhaseOffset=../configs/LocalTesting/ToAOffsets_Cosmics_ST_ORNL02_22x.csv
# 	badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_ST.txt"	
# 	#muon runs
# 	Calib $3 $calibFile1 $dataDirOut $dataDirOut 225 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	
# 	calibFile1=/media/fbock/Lennard4TB/SummingBoardTestFull/HGCROCData/rawHGCROC_wPedwMuon_wBC_Imp2_226_227.root
# 	toaPhaseOffset=../configs/LocalTesting/ToAOffsets_Cosmics_ST_ORNL02_22x.csv
# 	badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_ST.txt"	
# 	#muon runs
# 	Calib $3 $calibFile1 $dataDirOut $dataDirOut 226_227 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

	calibFile1=/media/fbock/Lennard4TB/SummingBoardTestFull/HGCROCData/rawHGCROC_wPedwMuon_wBC_Imp2_232_234.root
	toaPhaseOffset=../configs/LocalTesting/ToAOffsets_Cosmics_ST_ORNL02_22x.csv
	badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_ST.txt"	
	#muon runs
	Calib $3 $calibFile1 $dataDirOut $dataDirOut 232_234 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
	
	
fi

if [ $2 = "waveDet" ]; then  
  toaPhaseOffset=../configs/LocalTesting/ToAOffsets_Cosmics_ST_ORNL02_22x.csv
#   runs='225 226_227' 
  runs='232_234' 
	for runNr in $runs; do 
		HGCInv $3 $runNr $dataDirRaw $dataDirOut Run_$runNr $4
	done

fi
