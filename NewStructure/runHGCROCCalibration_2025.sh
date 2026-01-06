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
      ./DataPrep -d 1 -E -f -w $4/rawHGCROC_wPed_$runNrPed.root -i $4/rawHGCROC_$runNrMuon.root -o $4/rawHGCROC_wave_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibWave/$6 -r $runNrFile -k $4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt     
    else 
      ./DataPrep -d 1 -E -f -w $4/rawHGCROC_wPed_$runNrPed.root -i $4/rawHGCROC_$runNrMuon.root -o $4/rawHGCROC_wave_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibWave/$6 -r $runNrFile 
    fi
	elif [ $1 == "default" ]; then 
		time ./DataPrep -f -d 1 -e  -s -i $4/rawHGCROC_wPed_wBC_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuon/$6 -r $runNrFile
	elif [ $1 == "improved" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved/$6 -r $runNrFile
	elif [ $1 == "imp2nd" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp2_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved2nd/$6 -r $runNrFile
	elif [ $1 == "imp3rd" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp2_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp3_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved3rd/$6 -r $runNrFile
	elif [ $1 == "imp4th" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp3_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp4_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved4th/$6 -r $runNrFile
	elif [ $1 == "saveNewMuon" ]; then 
		time ./DataPrep -f -d 1 -M -i $5/rawPedAndMuon_$runNrMuon.root -o $4/raw_muononly_$runNrMuon.root 
	fi
}

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/202511_PST09/
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

if [ $2 = "pedestalMuon" ]; then
  runs='68' #ped muons FullSet A, 1st
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
  runs='165'
	badChannelMap=""
	runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runNr $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi


if [ $2 == "calibMuon" ]; then
  runPed='68'
	runs='FullSetA_1'
	badChannelMap=../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt
	runNrFile=../configs/TB2025/DataTakingDB_202511_HGCROC.csv
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi
