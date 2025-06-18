#! /bin/bash

function Calib()
{
	runNrFile='../configs/DataTakingDB_202409_CAEN.csv'
	echo "===================================================================="
	echo "option:  $1"
	echo "calib File:  $2"
	echo "raw data path:  $3"
	echo "out data path:  $4"
	echo "additional Name/runNr:  $5"
	echo "Plots Directory-Base: $6"
	echo "Plots additional name: $7"
	echo "===================================================================="
	if [ $1 == "transfer" ]; then
		time build/DataPrep -d 1 -e -a -f -P $2 -i $3/raw_$5.root  -o $3/rawWithCalib_$5.root -O $6/PlotsFullCalibTransferBC_2024/Run_$5 -r $runNrFile
	elif [ $1 == "trigg" ]; then
		time build/DataPrep -f -d 1 -T $2 -i $3/raw_$5.root -o $3/rawWithLocTrigg_$5.root 
	elif [ $1 == "calibNoTrigg" ]; then
		time build/DataPrep -t -e -f -d 1 -a -C $2 -i $3/rawWithLocTrigg_$5.root -o $4/calibrated_Run_$5.root -O $6/$7$5 -r $runNrFile
	elif [ $1 == "full" ]; then
		time build/DataPrep -e -f -d 1 -a -C $2 -i $3/raw_$5.root -o $4/calibrated_Run_$5.root -O $6/$7$5 -r $runNrFile
	fi
}

dataDirCal=""
if [ $1 = "fbock" ]; then 
	dataDirCal=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata
	dataDirIn=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/MuonRuns
	dataDirInE=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/ElectronRuns
	dataDirInH=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/HadronRuns
	dataDirOut=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/MuonRuns
	dataDirOutE=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/ElectronRuns
	dataDirOutH=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/HadronRuns
	PlotBaseDir=..
elif [ $1 = "fbockExt" ]; then 
	dataDirCal=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata
	dataDirBase=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata
	dataDirIn=$dataDirBase/MuonRuns
	dataDirInE=$dataDirBase/ElectronRuns
	dataDirInH=$dataDirBase/HadronRuns
	dataDirOut=$dataDirIn
	dataDirOutE=$dataDirInE
	dataDirOutH=$dataDirInH
	PlotBaseDir=..
elif [ $1 = "fbockExt2" ]; then 
	dataDirCal=/media/fbock/T7/202408_PST09/CAENData
	dataDirBase=/media/fbock/T7/202408_PST09/CAENData
	dataDirIn=$dataDirBase/MuonRuns
	dataDirInE=$dataDirBase/ElectronRuns
	dataDirInH=$dataDirBase/HadronRuns
	dataDirOut=$dataDirIn
	dataDirOutE=$dataDirInE
	dataDirOutH=$dataDirInH
	PlotBaseDir=/media/fbock/T7/202408_PST09
elif [ $1 = "eglimos" ]; then 
	dataDirCal=/home/ewa/EIC/test_beam2024/fullScanC/Output/MuonRuns
	dataDirIn=/home/ewa/EIC/test_beam2024/fullScanC
	dataDirInE=/home/ewa/EIC/test_beam2024/fullScanC
	dataDirInH=/home/ewa/EIC/test_beam2024/fullScanC
	dataDirOut=/home/ewa/EIC/test_beam2024/fullScanC/Output/MuonRuns
	dataDirOutE=/home/ewa/EIC/test_beam2024/fullScanC/Output/ElectronRuns
	dataDirOutH=/home/ewa/EIC/test_beam2024/fullScanC/Output/HadronRuns
	PlotBaseDir=..
elif [ $1 = "kmaret" ]; then
	dataDirCal=/mnt/d/202408_PST9_converted/MuonRuns
	dataDirIn=/mnt/d/202408_PST9_converted/MuonRuns
	dataDirInE=/mnt/d/202408_PST9_converted
	dataDirInH=/mnt/d/202408_PST9_converted
	dataDirOut=/mnt/d/202408_PST9_converted/MuonRuns
	dataDirOutE=/mnt/d/202408_PST9_converted
	dataDirOutH=/mnt/d/202408_PST9_converted
	PlotBaseDir=/mnt/d/202408_PST9_converted/MuonRuns

else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

# apply calibration
if [ $2 == "ScanA" ]; then
	calibFile1=$dataDirCal/calib_muonScanA1.root
	calibFile2=$dataDirCal/calib_muonScanA1.root
	echo "running calibrate for 45V runs, campaing A1"
	
	#muon runs
	muonScanA_45V='244 250 282 283'
	pedScanA_45V='271 277'
	Calib $3 $calibFile1 $dataDirIn $dataDirOut muonScanA1_45V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	Calib $3 $calibFile2 $dataDirIn $dataDirOut muonScanA2_45V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	
	# electron runs
	runs='251 252 254 257 258 ' 
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirInE $dataDirOutE $runNr $PlotBaseDir PlotsElectronCalibrated_2024/Run_
	done;
	
	#hadron runs
	runs='261 264 265 269 270 272 274 275 ' 
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirInH $dataDirOutH $runNr $PlotBaseDir PlotsHadronCalibrated_2024/Run_
	done;
elif [ $2 == "ScanB" ]; then
	echo "running calibrate for 42V runs, campaign B"
	calibFile1=$dataDirCal/calib_muonScanB1_42V.root
	calibFile2=$dataDirCal/calib_muonScanB2_42V.root

	#muon runs
	muonScanB_42V='331 322 370 371 374'
	pedScanB_42V='332 369'
	Calib $3 $calibFile1 $dataDirIn $dataDirOut muonScanB1_42V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	Calib $3 $calibFile2 $dataDirIn $dataDirOut muonScanB2_42V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	
	#electron runs
	runs='333 334 336 337 338 ' 
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirInE $dataDirOutE $runNr $PlotBaseDir PlotsElectronCalibrated_2024/Run_
	done;
	
	#hadron runs
	runs='340 349 346 350 357 360 362 367 368' 
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirInH $dataDirOutH $runNr $PlotBaseDir PlotsHadronCalibrated_2024/Run_
	done;
elif [ $2 == "ScanC" ]; then
	echo "running calibrate for 43.5V runs, campaign C"
	if [ $1 == "fbock" ] || [ $1 == "fbockExt" ]; then 
		calibFile1=$dataDirCal/CalibFinal_muonScanC1_43_5V.root
		calibFile2=$dataDirCal/CalibFinal_muonScanC2_43_5V.root
	else 
		calibFile1=$dataDirCal/rawPedAndMuonWBCImp_muonScanC1_43_5V.root 
		calibFile2=$dataDirCal/rawPedAndMuonWBCImp_muonScanC2_43_5V.root
	fi
	#muon runs
# 	muonScanC_43V='376 375 405 410 408'
# 	pedScanC_43V='377 404'
# 	Calib $3 $calibFile1 $dataDirIn $dataDirOut muonScanC1_43_5V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
# 	Calib $3 $calibFile2 $dataDirIn $dataDirOut muonScanC2_43_5V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
# 	
# # 	#electron runs
	runs='379 380 381 384 387' 
# # 	runs='380' 
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirInE $dataDirOutE $runNr $PlotBaseDir PlotsElectronCalibrated_2024/Run_
	done;
# 	
# 	# hadron runs
# 	runs='390 392 393 394 397 398 399 401' 
# 	for runNr in $runs; do 
# 		Calib $3 $calibFile1 $dataDirInH $dataDirOutH $runNr $PlotBaseDir PlotsHadronCalibrated_2024/Run_
# 	done;
elif [ $2 == "ScanD" ]; then
	calibFile2=$dataDirCal/calib_muonScanD2.root
	echo "running calibrate for 45V runs, campaing A1"
	
	#muon runs
	# 29.3K events
	muonScanD2_45V='460 456 457'
	pedScanD2_45V='454'
	Calib $3 $calibFile2 $dataDirIn $dataDirOut muonScanD2_45V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	
	# electron runs
	runs='421 422 429 430 432 ' 
	for runNr in $runs; do 
		Calib $3 $calibFile2 $dataDirInE $dataDirOutE $runNr $PlotBaseDir PlotsElectronCalibrated_2024/Run_
	done;
	
	#hadron runs
	runs='434 437 439 441 444 445 449 452 ' 
	for runNr in $runs; do 
		Calib $3 $calibFile2 $dataDirInH $dataDirOutH $runNr $PlotBaseDir PlotsHadronCalibrated_2024/Run_
	done;
elif [ $2 == "ScanE" ]; then
	calibFile1=$dataDirCal/calib_muonScanE1_40V.root
	calibFile2=$dataDirCal/calib_muonScanE2_40V.root
	echo "running calibrate for 40V runs, campaing E"

	muonScanE_40V='463 464 481 478'
	pedScanE_40V='465 476'

	Calib $3 $calibFile1 $dataDirIn $dataDirOut muonScanE1_40V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	Calib $3 $calibFile2 $dataDirIn $dataDirOut muonScanE2_40V $PlotBaseDir PlotsMuonCalibrated_2024/Run_

	# electron runs
	runs='466 467 468 471 472 ' 
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirInE $dataDirOutE $runNr $PlotBaseDir PlotsElectronCalibrated_2024/Run_
	done;
elif [ $2 == "ScanF" ]; then
	calibFile1=$dataDirCal/calib_muonScanF1_41V.root
	calibFile2=$dataDirCal/calib_muonScanF2_41V.root
	echo "running calibrate for 41V runs, campaing F"

	muonScanE_41V='486 489 507 506'
	pedScanE_41V='492 505'

	Calib $3 $calibFile1 $dataDirIn $dataDirOut muonScanF1_41V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	Calib $3 $calibFile2 $dataDirIn $dataDirOut muonScanF2_41V $PlotBaseDir PlotsMuonCalibrated_2024/Run_

	# electron runs
	runs='494 495 497 502 504' 
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirInE $dataDirOutE $runNr $PlotBaseDir PlotsElectronCalibrated_2024/Run_
	done;
elif [ $2 == "ScanH" ]; then
	calibFile1=$dataDirCal/calib_muonScanH1.root
	calibFile2=$dataDirCal/calib_muonScanH2.root
	echo "running calibrate for 45V runs, campaing H"
	
	#muon runs
	muonScanH_45V='526 527 554 559'
	pedScanH_45V='528 552'
	Calib $3 $calibFile1 $dataDirIn $dataDirOut muonScanH1_45V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	Calib $3 $calibFile2 $dataDirIn $dataDirOut muonScanH2_45V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	
	# electron runs
	runs='529 530 533 535 538 541' 
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirInE $dataDirOutE $runNr $PlotBaseDir PlotsElectronCalibrated_2024/Run_
	done;
	
	#hadron runs
	runs='542 543 544 545 548 549 550 551' 
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirInH $dataDirOutH $runNr $PlotBaseDir PlotsHadronCalibrated_2024/Run_
	done;
elif [ $2 == "ScanG" ]; then
	calibFile1=$dataDirCal/calib_muonScanG_46V.root
	echo "running calibrate for 46V runs, campaing G"
	
	#muon runs
	muonScanG_46V='508 510 511 525'
	pedScanG_46V='521'
	Calib $3 $calibFile1 $dataDirIn $dataDirOut muonScanG_46V $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	
	# electron runs
	runs='513 514 516 517 520' 
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirInE $dataDirOutE $runNr $PlotBaseDir PlotsElectronCalibrated_2024/Run_
	done;
elif [ $2 == "MuonHVScan" ]; then
	Calib $3 $dataDirCal/calib_305_44V.root $dataDirIn $dataDirOut 305 $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	Calib $3 $dataDirCal/calib_307_43V.root $dataDirIn $dataDirOut 307 $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	Calib $3 $dataDirCal/calib_309_42V.root $dataDirIn $dataDirOut 309 $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	Calib $3 $dataDirCal/calib_312_41V.root $dataDirIn $dataDirOut 312 $PlotBaseDir PlotsMuonCalibrated_2024/Run_
	Calib $3 $dataDirCal/calib_316_40V.root $dataDirIn $dataDirOut 316 $PlotBaseDir PlotsMuonCalibrated_2024/Run_
fi

