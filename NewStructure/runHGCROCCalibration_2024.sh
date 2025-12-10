#! /bin/bash

PlotBaseDir=..

function MuonCalibHGCROC()
{
	runNrFile='../configs/TB2024/DataTakingDB_202409_HGCROC.csv'
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
	echo "=================================================================================="
	if [ $1 == "BC" ]; then 
		./DataPrep -d 1 -e -f -P $4/rawHGCROC_wPed_$3.root -i $4/rawHGCROC_wPed_$3.root -o $4/rawHGCROC_wPed_wBC_$3.root -B $7 -O $PlotBaseDir/HGCROC_PlotsCalibTransfer_2024/$6 -r $runNrFile
# 		./DataPrep -d 1 -e -f -P $4/rawHGCROC_wPed_$3.root -i $4/rawHGCROC_wPed_$3.root -o $4/rawHGCROC_wPed_wBC_$3.root -B $7 -O $PlotBaseDir/HGCROC_PlotsCalibTransfer_2024/$6 -r $runNrFile
	elif [ $1 == "defaultLowStat" ]; then 
		time ./DataPrep -f -L 5000 -d 1 -e  -s -i $4/rawHGCROC_wPed_wBC_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_lowStat_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuon_2024/LowStat$6 -r $runNrFile
	elif [ $1 == "default" ]; then 
		time ./DataPrep -f -d 1 -e  -s -i $4/rawHGCROC_wPed_wBC_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuon_2024/$6 -r $runNrFile
	elif [ $1 == "improved" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved_2024/$6 -r $runNrFile
	elif [ $1 == "noise" ]; then 
		./DataPrep -f -d 1  -n -i $5/rawPedAndMuon_$3.root -o $5/rawPedAndMuonNoise_$2.root -O $PlotBaseDir/HGCROC_PlotsCalibNoiseRe_2024/$6 -r $runNrFile
	elif [ $1 == "transferAlt" ]; then 
		./DataPrep -d 1  -f -P $5/rawPedAndMuonNoise_$3.root -i $4/raw_$3.root -o $4/rawPedImp_$3.root -r $runNrFile
	elif [ $1 == "defaultImpPed" ]; then 
		./DataPrep -f -d 1  -s -i $4/rawPedImp_$3.root -o $5/rawPedImpAndMuon_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonPedImp_2024/$6 -r $runNrFile
	elif [ $1 == "saveNewPed" ]; then 
		./DataPrep -f -d 1 -N -i $5/rawPedAndMuonNoise_$3.root -o $4/raw_pedonly_$3.root 
	elif [ $1 == "saveNewMuon" ]; then 
		time ./DataPrep -f -d 1 -M -i $5/rawPedAndMuon_$3.root -o $4/raw_muononly_$3.root 
	elif [ $1 == "improvedMinimal" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/raw_muononly_$3.root -o $5/rawPedAndMuonImpMinimal_$2.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved_2024/$6_MinimalSet -r $runNrFile
	elif [ $1 == "improvedMinimal2nd" ]; then 
		time ./DataPrep -f -d 1  -S -i $5/rawPedAndMuonImpMinimal_$3.root -o $5/rawPedAndMuonImpMinimal2nd_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved_2024/$6_MinimalSet2nd -r $runNrFile
	elif [ $1 == "improvedMinimal3rd" ]; then 
		time ./DataPrep -f -d 1  -S -i $5/rawPedAndMuonImpMinimal2nd_$3.root -o $5/rawPedAndMuonImpMinimal3rd_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved_2024/$6_MinimalSet3rd -r $runNrFile
	fi
}

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbock" ]; then 
	PlotBaseDir=..
elif [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/LFHCal2/202408_PST09/HGCROCData
	dataDirOut=/media/fbock/LFHCal2/202408_PST09/HGCROCData
	PlotBaseDir=/media/fbock/LFHCal2/202408_PST09
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

# pedestal runs 
if [ $2 = "pedestalM" ]; then
  ./DataPrep -d 1 -p -i $dataDirRaw/muons/rawHGCROC_muonAll.root -f -o $dataDirOut/muons/rawHGCROC_wPed_muonAll.root -O $PlotBaseDir/HGCROC_PlotsPedestal_2024/RunMuon -r ../configs/TB2024/DataTakingDB_202409_HGCROC.csv
# 	runs='184 185'
# 	for runNr in $runs; do 
#     ./DataPrep -d 1 -p -i $dataDirRaw/muons/rawHGCROC_$runNr.root -f -o $dataDirOut/muons/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/HGCROC_PlotsPedestal_2024/Run_$runNr -r ../configs/TB2024/DataTakingDB_202409_HGCROC.csv
# 	done
# 	runs='275 276 277 278 279 280 281 282 283 ' #299 300
# 	for runNr in $runs; do 
#     ./DataPrep -d 1 -p -i $dataDirRaw/muons/rawHGCROC_$runNr.root -f -o $dataDirOut/muons/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/HGCROC_PlotsPedestal_2024/Run_$runNr -r ../configs/TB2024/DataTakingDB_202409_HGCROC.csv
# 	done
# 	runs='302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321'
# 	for runNr in $runs; do 
#     ./DataPrep -d 1 -p -i $dataDirRaw/muons/rawHGCROC_$runNr.root -f -o $dataDirOut/muons/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/HGCROC_PlotsPedestal_2024/Run_$runNr -r ../configs/TB2024/DataTakingDB_202409_HGCROC.csv
# 	done
# 	runs='muonsPos muonsNeg'
# 	for runNr in $runs; do 
#     ./DataPrep -d 1 -p -i $dataDirRaw/muons/rawHGCROC_$runNr.root -f -o $dataDirOut/muons/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/HGCROC_PlotsPedestal_2024/Run_$runNr -r ../configs/TB2024/DataTakingDB_202409_HGCROC.csv
# 	done
fi

if [ $2 = "pedestalMtrunc" ]; then
  ./DataPrep -d 1 -p -i $dataDirRaw/muons/rawHGCROCtruncated_muonAll.root -f -o $dataDirOut/muons/rawHGCROCtruncated_wPed_muonAll.root -O $PlotBaseDir/HGCROC_PlotsPedestalTruncated_2024/RunMuon -r ../configs/TB2024/DataTakingDB_202409_HGCROC.csv -F png
fi

if [ $2 = "pedestalE" ]; then
  runs='103 104 105 106 107'
  for runNr in $runs; do 
    ./DataPrep -d 1 -p -i $dataDirRaw/electrons/rawHGCROC_$runNr.root -f -o $dataDirOut/electrons/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/HGCROC_PlotsPedestal_2024/Run_$runNr -r ../configs/TB2024/DataTakingDB_202409_HGCROC.csv
  done
fi

badChannelMap=../configs/TB2024/badChannelMap_TBSetup_HGCROC_202408_118-337.txt
if [ $2 == "calibMuon" ]; then
	echo "running muon calib for 43V runs"
	MuonCalibHGCROC $3 muonAll muonAll $dataDirRaw/muons/ $dataDirOut/muons/ Muons $badChannelMap
fi

if [ $2 == "calibMuonT" ]; then
	echo "running muon calib for 43V runs"
	runs='275 276 277 278 279 280 281 282 283 muonsPos muonsNeg' #299 300
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runNr $runNr $dataDirRaw/muons/ $dataDirOut/muons/ Run_$runNr $badChannelMap
	done
fi


if [ $2 == "calibElectron" ]; then
	echo "running muon calib for 43V runs"
	MuonCalibHGCROC $3 muonAll 103 electrons/$dataDirRaw muon/$dataDirOut 103 $badChannelMap
fi

