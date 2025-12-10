#! /bin/bash
dataDirOut=""
runNr=""
if [ $1 = "fbock" ]; then 
	if [ $2 = "muon" ]; then
		dataDirOut=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/MuonRuns
		runNr=$6
	elif [ $2 = "electron" ]; then
		dataDirOut=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/ElectronRuns
		runNr=$6
	elif [ $2 = "hadron" ]; then
		dataDirOut=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/HadronRuns
		runNr=$6
	else 
		echo "Please select a beam type (muon, electron, or hadron), otherwise I don't know where the data is"
		exit
	fi
elif [ $1 = "fbockExt" ]; then 
	if [ $2 = "muon" ]; then
		dataDirOut=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/MuonRuns
		runNr=$6
	elif [ $2 = "electron" ]; then
		dataDirOut=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/ElectronRuns
		runNr=$6
	elif [ $2 = "hadron" ]; then
		dataDirOut=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/HadronRuns
		runNr=$6
	else 
		echo "Please select a beam type (muon, electron, or hadron), otherwise I don't know where the data is"
		exit
	fi
elif [ $1 = "eglimos" ]; then 
	if [ $2 = "muon" ]; then
		dataDirOut=/home/ewa/EIC/test_beam2024/fullScanC/Output/MuonRuns
		runNr=376
	elif [ $2 = "electron" ]; then
		dataDirOut=/home/ewa/EIC/test_beam2024/fullScanC/Output/ElectronRuns
		runNr=380
	elif [ $2 = "hadron" ]; then
		dataDirOut=/home/ewa/EIC/test_beam2024/fullScanC/Output/HadronRuns
		runNr=397
	else 
		echo "Please select a beam type (muon, electron, or hadron), otherwise I don't know where the data is"
		exit
	fi
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

runNrFile='../configs/TB2024/DataTakingDB_202409_CAEN.csv'

if [ $5 == "muonTrig" ]; then
	./Display -i $dataDirOut/calibrated_Run_$runNr.root -r $runNrFile -P ../EventDisplay/Run_$runNr/MuonTrig -e $3 -N $4 -M
else
# 	./Display -i $dataDirOut/calibrated_Run_$runNr.root -r $runNrFile -P ../EventDisplay/Run_$runNr -e $3 -N $4 -d 0 -F png
	./Display -i $dataDirOut/calibrated_Run_$runNr.root -r $runNrFile -P ../EventDisplay/Run_$runNr -e $3 -N $4 -d 0 -F pdf
fi 

