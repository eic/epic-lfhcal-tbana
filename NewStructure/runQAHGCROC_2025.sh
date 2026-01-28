#! /bin/bash

function QARun()
{
	runNrFile='../configs/TB2025/DataTakingDB_202511_HGCROC.csv'
	echo "============================ QA running =========================================="
	echo "option $1"
	echo "DataDir: $2"
	echo "run Nr: $3"
	echo "PlotDir: $4"
	echo "=================================================================================="
	if [ $1 == "SimpleQA" ]; then 
		time ./DataAna -e 1 -d 1 -q -i $2/calibratedHGCROC_Run_$3.root -O $2/SimpleQAHists_Run_$3.root -a -f -r $runNrFile -P $4/HGCROC_SimpleQAPlots/Run_$3	
  elif [ $1 == "QA" ]; then 
		time ./DataAna -d 1 -Q -i $2/calibratedHGCROC_Run_$3.root -O $2/QAHists_Run_$3.root -a -f -r $runNrFile -P $4/HGCROC_QAPlots/Run_$3
	fi
}


dataDirOut=""
PlotBaseDir=..
if [ $1 = "fbockTB" ]; then 
	dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/202511_PST09/
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

if [ $2 == "FullScanA" ]; then
	echo "running calibrate for 44V runs, campaing A1"
		runs='FullSetA_1' 
# 	runs='165 166 167 168 169 170 FullSetA_1' 
# 	runs='175 FullSetA_2 168_169'
# 	runs='183'
	for runNr in $runs; do 
		QARun $3 $dataDirOut $runNr $PlotBaseDir
	done;
# 	runs='261 264 265 269 270 272 274 275 ' 
# 	for runNr in $runs; do 
# 		QARun $3 $dataDirOutH $runNr $PlotBaseDir $dataDirOutH/calibrated_Run_$runNr.root
# 	done;
# 	QARun $3 $dataDirOut muonScanA1_45V $PlotBaseDir $dataDirOut/calibratedMuon_muonScanA1_45V.root
# 	QARun $3 $dataDirOut muonScanA2_45V $PlotBaseDir $dataDirOut/calibratedMuon_muonScanA2_45V.root	
elif [ $2 == "FullScanB" ]; then
	echo "running calibrate for 45V runs, campaing B"
		runs='FullSetB_1' 
# 	runs='165 166 167 168 169 170 FullSetA_1' 
# 	runs='175 FullSetA_2 168_169'
# 	runs='183'
	for runNr in $runs; do 
		QARun $3 $dataDirOut $runNr $PlotBaseDir
	done;
# 	runs='261 264 265 269 270 272 274 275 ' 
# 	for runNr in $runs; do 
# 		QARun $3 $dataDirOutH $runNr $PlotBaseDir $dataDirOutH/calibrated_Run_$runNr.root
# 	done;
# 	QARun $3 $dataDirOut muonScanA1_45V $PlotBaseDir $dataDirOut/calibratedMuon_muonScanA1_45V.root
# 	QARun $3 $dataDirOut muonScanA2_45V $PlotBaseDir $dataDirOut/calibratedMuon_muonScanA2_45V.root	
fi
