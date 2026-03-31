#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

PlotBaseDir=..
runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/202511_PST09/
elif [ $1 = "ehagen" ]; then 
	dataDirRaw=/Users/hagen/Githubs/epic-lfhcal-tbana/TB_Data
	dataDirOut=/Users/hagen/Githubs/epic-lfhcal-tbana/TB_Data
	PlotBaseDir=/Users/hagen/Githubs/epic-lfhcal-tbana/plots
elif [ $1 = "egpott" ]; then
  dataDirRaw=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot
  dataDirOut=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot
  PlotBaseDir=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/plots
elif [ $1 = "rhamilton" ]; then
  dataDirRaw=/Users/ryanhamilton/Documents/Research/data.nosync/202511_PST08/CAENData/rawfiles
  dataDirOut=/Users/ryanhamilton/Documents/Research/data.nosync/202511_PST08/CAENData/processed_data
  PlotBaseDir=/Users/ryanhamilton/Documents/Research/data.nosync/202511_PST08/CAENData/plots
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

# run pedest extraction for different run numbers
if [ $2 = "pedestal" ]; then
  runs='';
  # different number of KCUs & asics
  if [ $3 = "Test" ]; then
    runs='122 123 124 125 126 128 129'  
  # reference pedestal runs for various campaigns
  elif [ $3 = "Ref" ]; then
#     runs='68 208 210 259 381'
    runs='208'
  # muon runs
  elif [ $3 = "Muon" ]; then
  #   runs='69 70 71 72 73 74 75 76 164' # Full Set A - muon set 1
  #   runs='201 202 203 204 205 206' # Full Set A - muon set 2
  #   runs='211 212 213 214 215 216 217' #Full set B - muon set 1
  #   runs='252 253 254 255 256 257 258 260' #Full set B - muon set 2
  #   runs='273 274 275 276 289'          #Hadron scan 1
  #   runs='296 297 298 299 300'          #Hadron scan 2
  #   runs='319 320 324 325 326 327 328 329 330'          #Hadron scan 3
  #     runs='352 353 354 355 356'          #Hadron scan 4

    runs='381 382 383 384 385 386 387 388 389 390 391 395'              #E scan 
  #   runs='33 28 29 30 31 32'                           #1st HV scan
  #     runs='382 383 384 385'              #E scan 
  	  runs='28 29 30 31 32 33 34 35'			# 1st HV scan
  #  runs='260 261 262 263 264 265 266 267 268 271 272' #2nd HV scan 
  # electron runs
  elif [ $3 = "Electron" ]; then
    runs='166 167 168 169 170'
  fi
  for runNr in $runs; do 
    printf -v runNrPed "%03d" "$runNr"
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNrPed.root -f -o $dataDirOut/rawHGCROC_wPed_$runNrPed.root -O $PlotBaseDir/PlotsPedestal/Run$runNrPed -r $runNrFile
  done
fi

if [ $2 = "toaPhase" ]; then 
  runNrPed='208'
  if [ $3 = "Hadron" ]; then 
    runs='184'
    for rn in $runs; do 
      printf -v runNr "%03d" "$rn"
      ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runNrFile -g $dataDirOut/rawHGCROC_wPed_$runNrPed.root #-F png
    done
  elif [ $3 = "Electron" ]; then 
    for runNr in $runs; do 
      ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runNrFile -g $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root #-F png
    done
  elif [ $3 = "Muon" ]; then 
    runs='028 029 030 031 032 033 034 035' # 1st HV scan
    for runNr in $runs; do 
			./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_wPed_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runNrFile -g $dataDirRaw/rawHGCROC_wPed_$runNr.root
      #./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runNrFile -g $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root #-F png
    done
  fi

fi

if [ $2 = "wave" ]; then  
#   runs='165 166 167 168 169 170'
#   runs='170'
#   runs='165'
#   runs='184'
  runs='FullSetA_2'
	runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
	for runNr in $runs; do 
		WaveformHGCROC $3 $runNr $dataDirRaw Run_$runNr
	done
fi


if [ $2 == "calibMuon" ]; then
#   runPed='68'
# 	runs='FullSetA_1'
#   runPed='208'
# 	runs='FullSetA_2'
#   runPed='210'
# 	runs='FullSetB_1'
#   runPed='259'
# 	runs='FullSetB_2'
  runPed='030'
	#runs='028 029 030 031 032 033 034 035'
	runs='030'
	badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
	runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
	toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_MuonScan1.csv
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runNr $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset # -EP 2026/2/15
		#MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset
	done
fi

if [ $2 == "calibMuon2" ]; then
#   runPed='68'
# 	runs='FullSetA_1'
#   runPed='208'
# 	runs='FullSetA_2'
#   runPed='210'
# 	runs='FullSetB_1'
  runPed='259'
	runs='FullSetB_2'
	badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
	runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
	toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset
	done
fi
