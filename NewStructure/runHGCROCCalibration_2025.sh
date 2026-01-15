#! /bin/bash

PlotBaseDir=..
runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv

function MuonCalibHGCROC()
{
	
	echo "=================================================================================="
	echo "option $1"
	echo "run Nr Pedestal: $2"
	echo "run Nr Muon: $3"
	echo "dataRawDir: $4"
	echo "dataOutDir: $5"
	echo "OutNameRun:" $6
	if [ $1 == "BC" ]; then 
		echo "badchannelMap:" $7
	fi
	printf -v runNrPed "%03d" "$2"
# 	printf -v runNrMuon "%03d" "$3"
	runNrMuon=$3
	echo "=================================================================================="
	if [ $1 == "BC" ]; then 
    if [ -f "$4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt" ]; then
      echo "overwriting original calib file with manually modified $4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt"
      ./DataPrep -d 1 -e -f -P $4/rawHGCROC_wPed_$runNrPed.root -i $4/rawHGCROC_$runNrMuon.root -o $4/rawHGCROC_wPed_wBC_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibTransfer/$6 -r $runNrFile -k $4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt -B $7    
    else 
      ./DataPrep -d 2 -e -f -P $4/rawHGCROC_wPed_$runNrPed.root -i $4/rawHGCROC_$runNrMuon.root -o $4/rawHGCROC_wPed_wBC_$runNrMuon.root  -O $PlotBaseDir/HGCROC_PlotsCalibTransfer/$6 -r $runNrFile -B $7
    fi
	elif [ $1 == "wave" ]; then 
    if [ -f "$4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt" ]; then
      echo "overwriting original calib file with manually modified $4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt"
      ./DataPrep -d 1 -E 2 -f -w $4/rawHGCROC_wPed_$runNrPed.root -i $4/rawHGCROC_$runNrMuon.root -o $4/rawHGCROC_wave_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsWave/$6 -r $runNrFile -k $4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt     
    else 
      ./DataPrep -d 1 -E 2 -f -w $4/rawHGCROC_wPed_$runNrPed.root -i $4/rawHGCROC_$runNrMuon.root -o $4/rawHGCROC_wave_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibWave/$6 -r $runNrFile 
    fi
	elif [ $1 == "default" ]; then 
		time ./DataPrep -f -d 1 -e  -s -i $4/rawHGCROC_wPed_wBC_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuon/$6 -r $runNrFile
	elif [ $1 == "imp" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved/$6 -r $runNrFile
	elif [ $1 == "imp2nd" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp2_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved2nd/$6 -r $runNrFile
	elif [ $1 == "imp3rd" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp2_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp3_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved3rd/$6 -r $runNrFile
	elif [ $1 == "imp4th" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp3_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp4_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved4th/$6 -r $runNrFile
	elif [ $1 == "saveNewMuon" ]; then 
		time ./DataPrep -f -d 1 -M -i $5/rawHGCROC_wPedwMuon_wBC_$runNrMuon.root -o $4/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNrMuon.root 
	elif [ $1 == "imp_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_ImpR_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImp_Red/$6 -r $runNrFile
	elif [ $1 == "imp2nd_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_ImpR_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp2R_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved2nd_Red/$6 -r $runNrFile
	elif [ $1 == "imp3rd_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp2R_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp3R_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved3rd_Red/$6 -r $runNrFile
  elif [ $1 == "wave_red" ]; then 
     ./DataPrep -d 1 -E 2 -f -w $4/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNrMuon.root -i $4/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNrMuon.root -o $4/rawHGCROC_miptrigg_wPedwMuon_wBC_wave_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibWaveMuons/$6 -r $runNrFile 
	fi
}

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/Lennard4TB/202511_PST09/HGCROCDataAlt
	dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCDataAlt
	PlotBaseDir=/media/fbock/Lennard4TB/202511_PST09/AltPlots
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi


if [ $2 = "pedestalTestRuns" ]; then
  runs='122 123 124 125 126 128 129'
  for runNr in $runs; do 
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNr.root -f -o $dataDirOut/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/PlotsPedestal/Run$runNr -r $runNrFile
  done
fi

if [ $2 = "pedestalRef" ]; then
#   runs='68' #ped muons FullSet A, 1st
#   runs='208' #ped muons FullSet A, 2nd
#   runs='210' #ped muons FullSet B, 1st
#   runs='259' #ped muons FullSet B, 2nd
  runs='68 208 210 259' #ped muons
  for runNr in $runs; do 
    printf -v runNrPed "%03d" "$runNr"
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNrPed.root -f -o $dataDirOut/rawHGCROC_wPed_$runNrPed.root -O $PlotBaseDir/PlotsPedestal/Run$runNrPed -r $runNrFile
  done
fi

if [ $2 = "pedestalMuon" ]; then
#   runs='69 70 71 72 73 74 75 76 164' # Full Set A - muon set 1
  #   runs='201 202 203 204 205 206' # Full Set A - muon set 2
  #   runs='211 212 213 214 215 216 217' #Full set B - muon set 1
  #   runs='252 253 254 255 256 257 258 260' #Full set B - muon set 2
  #   runs='273 274 275 276 289'          #Hadron scan 1
  #   runs='296 297 298 299 300'          #Hadron scan 2
  #   runs='319 320 324 325 326 327 328 329 330'          #Hadron scan 3
#     runs='352 353 354 355 356'          #Hadron scan 4
#     runs='382 383 384 385'              #E scan 
#   runs='33 28 29 30 31 32'                           #1st HV scan
  runs='260 261 262 263 264 265 266 267 268 271 272' #2nd HV scan 
  for runNr in $runs; do 
    printf -v runNrPed "%03d" "$runNr"
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNrPed.root -f -o $dataDirOut/rawHGCROC_wPed_$runNrPed.root -O $PlotBaseDir/PlotsPedestal/Run$runNrPed -r $runNrFile
  done
fi


if [ $2 = "pedestalE" ]; then
#   runs='165'
  runs='166 167 168 169 170'
  for runNr in $runs; do 
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNr.root -f -o $dataDirOut/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/PlotsPedestal/Run$runNr -r $runNrFile
  done
fi
if [ $2 = "pedestalM" ]; then
  runs='211'
  for runNr in $runs; do 
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNr.root -f -o $dataDirOut/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/PlotsPedestal/Run$runNr -r $runNrFile
  done
fi


if [ $2 = "elewave" ]; then
  runs='165 166 167 168 169 170'
#   runs='165'
  runPed='68'
	badChannelMap="../configs/TB2025/badChannel_HGCROC_PSTB2025_layer0.txt"
	runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi


if [ $2 == "calibMuon" ]; then
  runPed='68'
	runs='FullSetA_1'
#   runPed='208'
# 	runs='FullSetA_2'
#   runPed='210'
# 	runs='FullSetB_1'
#   runPed='259'
# 	runs='FullSetB_2'
	badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
	runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi

if [ $2 == "calibMuon2" ]; then
#   runPed='68'
# 	runs='FullSetA_1'
  runPed='208'
	runs='FullSetA_2'
#   runPed='210'
# 	runs='FullSetB_1'
#   runPed='259'
# 	runs='FullSetB_2'
	badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
	runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi
