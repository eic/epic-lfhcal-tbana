#! /bin/bash

function QARun()
{
	runNrFile='../configs/TB2024/DataTakingDB_202409_CAEN.csv'
	echo "============================ QA running =========================================="
	echo "option $1"
	echo "DataDir: $2"
	echo "input file: $5"
	echo "run Nr: $3"
	echo "PlotDir: $4"
	echo "=================================================================================="
	if [ $1 == "SimpleQA" ]; then 
		time ./DataAna -e 1 -d 1 -q -i $2/calibrated_Run_$3.root -O $2/SimpleQAHists_Run_$3.root -a -f -r $runNrFile -P $4/CAEN_SimpleQAPlots/Run_$3	
		elif [ $1 == "QA" ]; then 
		time ./DataAna -d 1 -Q -i $2/calibrated_Run_$3.root -O $2/QAHists_Run_$3.root -a -f -r $runNrFile -P $4/CAEN_QAPlots/Run_$3
	fi
}


dataDirOut=""
PlotBaseDir=..
if [ $1 = "fbock" ]; then 
	dataDirOut=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/MuonRuns
	dataDirOutE=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/ElectronRuns
	dataDirOutH=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/HadronRuns
elif [ $1 = "fbockExt" ]; then 
	dataDirOut=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/MuonRuns
	dataDirOutE=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/ElectronRuns
	dataDirOutH=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/HadronRuns
elif [ $1 = "fbockExt2" ]; then 
	dataDirOut=/media/fbock/T7/202408_PST09/CAENdata/MuonRuns
	dataDirOutE=/media/fbock/T7/202408_PST09/CAENdata/ElectronRuns
	dataDirOutH=/media/fbock/T7/202408_PST09/CAENdata/HadronRuns
	PlotBaseDir=/media/fbock/T7/202408_PST09
elif [ $1 = "eglimos" ]; then 
	dataDirOut=/home/ewa/EIC/test_beam2024/fullScanC/Output/MuonRuns
	dataDirOutE=/home/ewa/EIC/test_beam2024/fullScanC/Output/ElectronRuns
	dataDirOutH=/home/ewa/EIC/test_beam2024/fullScanC/Output/HadronRuns
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

runNrFile='../configs/TB2024/DataTakingDB_202409_CAEN.csv'
muonScanA_45V='244 250 282 283'
pedScanA_45V='271 277'
if [ $2 == "ScanA" ]; then
	# 192K events
	echo "running calibrate for 45V runs, campaing A1"
	runs='251 252 254 257 258 ' 
	for runNr in $runs; do 
		QARun $3 $dataDirOutE $runNr $PlotBaseDir $dataDirOutE/calibrated_Run_$runNr.root
	done;
	runs='261 264 265 269 270 272 274 275 ' 
	for runNr in $runs; do 
		QARun $3 $dataDirOutH $runNr $PlotBaseDir $dataDirOutH/calibrated_Run_$runNr.root
	done;
	QARun $3 $dataDirOut muonScanA1_45V $PlotBaseDir $dataDirOut/calibratedMuon_muonScanA1_45V.root
	QARun $3 $dataDirOut muonScanA2_45V $PlotBaseDir $dataDirOut/calibratedMuon_muonScanA2_45V.root	
fi

if [ $2 == "ScanC" ]; then
	echo "running calibrate for 43.5V runs, campaign C"
	
# 	#electron runs
	# runs='379 380 381 384 387' 
	runs='380' 
	for runNr in $runs; do 
		QARun $3 $dataDirOutE $runNr $PlotBaseDir $dataDirOutE/calibrated_Run_$runNr.root
	done;
# 	
# 	#hadron runs
	# runs='397 398 399 401' 
	# for runNr in $runs; do 
	# 	Calib $3 $calibFile1 $dataDirRawH $dataDirOutH $runNr $PlotBaseDir/PlotsHadronCalibrated_2024/Run_
	# done;
fi

