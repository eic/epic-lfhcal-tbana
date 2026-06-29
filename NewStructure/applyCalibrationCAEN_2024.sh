#! /bin/bash

function Calib()
{
  runNrFile='../configs/TB2024/DataTakingDB_202409_CAEN.csv'
  echo "===================================================================="
  echo "option:  $1"
  echo "calib File:  $2"
  echo "raw data path:  $3"
  echo "out data path:  $4"
  echo "additional Name/runNr:  $5"
  echo "Plots Directory-Base: $6"
  echo "Plots additional name: $7"
  echo "runNr file" $runNrFile
  echo "skipDetailed plots layer" $8
  echo "===================================================================="
  if [ $1 == "transfer" ]; then
    time ./DataPrep -d 1 -e -a -f -P $2 -i $3/raw_$5.root  -o $3/rawWithCalib_$5.root -O $6/CAEN_PlotsFullCalibTransferBC_2024/Run_$5 -r $runNrFile -l $8
  elif [ $1 == "transferWER" ]; then
    time ./DataPrep -d 1 -e -a -f -D -P $2 -i $3/raw_$5.root  -o $3/rawWithCalibWER_$5.root -O $6/CAEN_PlotsFullCalibTransferBCWER_2024/Run_$5 -r $runNrFile -l $8
  elif [ $1 == "trigg" ]; then
    time ./DataPrep -f -d 1 -T $2 -i $3/raw_$5.root -o $3/rawWithLocTrigg_$5.root -r $runNrFile -O $6/CAEN_Transfer/Run_$5 -l $8
  elif [ $1 == "calibNoTrigg" ]; then
    time ./DataPrep -t -e -f -d 1 -a -C $2 -i $3/rawWithLocTrigg_$5.root -o $4/calibrated_Run_$5.root -O $6/$7$5 -r $runNrFile -l $8
  elif [ $1 == "calibAlter" ]; then
    time ./DataPrep -t -e -f -d 1 -a -C $2 -i $3/rawWithLocTrigg_$5.root -o $4/calibratedAlter_Run_$5.root -O $6/CAEN_PlotsCalibratedAlter_2024/Run_$5 -r $runNrFile -l $8
  elif [ $1 == "calibAlterWER" ]; then
    time ./DataPrep -t -e -f -d 1 -D -a -C $2 -i $3/rawWithLocTrigg_$5.root -o $4/calibratedAlter_Run_$5.root -O $6/CAEN_PlotsCalibratedAlterWER_2024/Run_$5 -r $runNrFile -l $8
  elif [ $1 == "calibNoTriggMuon" ]; then
    time ./DataPrep -t -e -f -d 1 -a -C $2 -i $3/raw_muononly_$5.root -o $4/calibrated_Run_$5.root -O $6/$7$5 -r $runNrFile -l $8
  elif [ $1 == "full" ]; then
    time ./DataPrep -e -f -d 1 -a -C $2 -i $3/raw_$5.root -o $4/calibrated_Run_$5.root -O $6/$7$5 -r $runNrFile -l $8
  fi
}

userName=$1
optionMain=$2
optionSub=$3
skipLayer=0
if [ $# -eq 4 ]; then
  skipLayer=$4
fi


dataDirCal=""
if [ $userName = "fbock" ]; then 
  dataDirCal=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata
  dataDirIn=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/MuonRuns
  dataDirInE=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/ElectronRuns
  dataDirInH=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/HadronRuns
  dataDirOut=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/MuonRuns
  dataDirOutE=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/ElectronRuns
  dataDirOutH=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/HadronRuns
  PlotBaseDir=..
elif [ $userName = "fbockExt" ]; then 
  dataDirCal=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENdata
  dataDirBase=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENdata
  dataDirIn=$dataDirBase
  dataDirInE=$dataDirBase
  dataDirInH=$dataDirBase
  dataDirOut=$dataDirIn
  dataDirOutE=$dataDirInE
  dataDirOutH=$dataDirInH
  PlotBaseDir=/media/fbock/T7/LFHCalTBData/202408_PST09/ReanlysisCAEN
elif [ $userName = "eglimos" ]; then 
  dataDirCal=/home/ewa/EIC/test_beam2024/fullScanC/Output/MuonRuns
  dataDirIn=/home/ewa/EIC/test_beam2024/fullScanC
  dataDirInE=/home/ewa/EIC/test_beam2024/fullScanC
  dataDirInH=/home/ewa/EIC/test_beam2024/fullScanC
  dataDirOut=/home/ewa/EIC/test_beam2024/fullScanC/Output/MuonRuns
  dataDirOutE=/home/ewa/EIC/test_beam2024/fullScanC/Output/ElectronRuns
  dataDirOutH=/home/ewa/EIC/test_beam2024/fullScanC/Output/HadronRuns
  PlotBaseDir=..
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

# apply calibration
if [ $optionMain == "ScanA" ]; then
  if [ $optionSub == "calibAlter" ] ||  [ $optionSub == "calibAlterWER" ] ; then
    calibFile1=$dataDirCal/calib_muonScanA1_45V_V2hadCorr.root
    calibFile2=$dataDirCal/calib_muonScanA1_45V_V2.root
  else
    calibFile1=$dataDirCal/calib_muonScanA1_45V_V2.root
    calibFile2=$dataDirCal/calib_muonScanA1_45V_V2.root  
  fi
  echo "running calibrate for 45V runs, campaing A1"
  
  #muon runs
  muonScanA_45V='244 250 282 283'
  pedScanA_45V='271 277'
  Calib $optionSub $calibFile1 $dataDirIn $dataDirOut muonScanA1_45V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer 
  Calib $optionSub $calibFile2 $dataDirIn $dataDirOut muonScanA2_45V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer 
  
#   # electron runs
  runs='251 252 254 257 258 ' 
  for runNr in $runs; do 
    Calib $optionSub $calibFile1 $dataDirInE $dataDirOutE $runNr $PlotBaseDir  CAEN_PlotsElectronCalibrated_2024/Run_ $skipLayer
  done;
#   
  #hadron runs
  runs='261 264 265 269 270 272 274 275 ' 
  for runNr in $runs; do 
    Calib $optionSub $calibFile1 $dataDirInH $dataDirOutH $runNr $PlotBaseDir CAEN_PlotsHadronCalibrated_2024/Run_ $skipLayer
  done;
elif [ $optionMain == "ScanB" ]; then
  echo "running calibrate for 42V runs, campaign B"
  if [ $optionSub == "calibAlter" ] ||  [ $optionSub == "calibAlterWER" ] ; then
    calibFile1had=$dataDirCal/calib_muonScanB1_42V_V2hadCorr.root
  else 
    calibFile1had=$dataDirCal/calib_muonScanB1_42V_V2.root
  fi
  calibFile1=$dataDirCal/calib_muonScanB1_42V_V2.root
  calibFile2=$dataDirCal/calib_muonScanB2_42V_V2.root
  
  #muon runs
  muonScanB_42V='331 322 370 371 374'
  pedScanB_42V='332 369'
  if [ $optionSub != "calibAlter" ] && [ $optionSub != "calibAlterWER" ]; then
    Calib $optionSub $calibFile1 $dataDirIn $dataDirOut muonScanB1_42V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
    Calib $optionSub $calibFile2 $dataDirIn $dataDirOut muonScanB2_42V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  fi
  
  #electron runs
  if [ $optionSub == "calibAlter" ] ||  [ $optionSub == "calibAlterWER" ] ; then
    runs='338' 
  else 
    runs='333 334 336 337 338 '   
  fi
  for runNr in $runs; do 
    Calib $optionSub $calibFile1had $dataDirInE $dataDirOutE $runNr $PlotBaseDir CAEN_PlotsElectronCalibrated_2024/Run_ $skipLayer
  done;
#   
  #hadron runs
  if [ $optionSub == "calibAlter" ] ||  [ $optionSub == "calibAlterWER" ] ; then
    runs='350 367' 
#     runs='350 368' 
  else 
    runs='368' 
#     runs='340 349 346 350 357 360 362 367 368' 
  fi
  
  for runNr in $runs; do 
    Calib $optionSub $calibFile1had $dataDirInH $dataDirOutH $runNr $PlotBaseDir CAEN_PlotsHadronCalibrated_2024/Run_ $skipLayer
  done;
elif [ $optionMain == "ScanC" ]; then
  echo "running calibrate for 43.5V runs, campaign C"
  if [ $userName == "fbock" ] || [ $userName == "fbockExt" ]; then 
    calibFile1=$dataDirCal/calib_muonScanC1_43_5V_V2.root
    calibFile2=$dataDirCal/calib_muonScanC2_43_5V_V2.root
  else 
    calibFile1=$dataDirCal/rawPedAndMuonWBCImp_muonScanC1_43_5V.root 
    calibFile2=$dataDirCal/rawPedAndMuonWBCImp_muonScanC2_43_5V.root
  fi
  #muon runs
#   muonScanC_43V='376 375 405 410 408'
#   pedScanC_43V='377 404'
  Calib $optionSub $calibFile1 $dataDirIn $dataDirOut muonScanC1_43_5V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
#   Calib $optionSub $calibFile2 $dataDirIn $dataDirOut muonScanC2_43_5V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
#   
#   #electron runs
#   runs='379 380 381 384 387' 
# # #   runs='380' 
#   for runNr in $runs; do 
#     Calib $optionSub $calibFile2 $dataDirInE $dataDirOutE $runNr $PlotBaseDir CAEN_PlotsElectronCalibrated_2024/Run_ $skipLayer
#   done;
# #   
#   # hadron runs
#   runs='390 392 393 394 397 398 399 401' 
#   for runNr in $runs; do 
#     Calib $optionSub $calibFile2 $dataDirInH $dataDirOutH $runNr $PlotBaseDir CAEN_PlotsHadronCalibrated_2024/Run_ $skipLayer
#   done;
elif [ $optionMain == "ScanD" ]; then
  calibFile2=$dataDirCal/calib_muonScanD2.root
  echo "running calibrate for 45V runs, campaing A1"
  
  #muon runs
  # 29.3K events
  muonScanD2_45V='460 456 457'
  pedScanD2_45V='454'
  Calib $optionSub $calibFile2 $dataDirIn $dataDirOut muonScanD2_45V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  
  # electron runs
  runs='421 422 429 430 432 ' 
  for runNr in $runs; do 
    Calib $optionSub $calibFile2 $dataDirInE $dataDirOutE $runNr $PlotBaseDir CAEN_PlotsElectronCalibrated_2024/Run_ $skipLayer
  done;
  
  #hadron runs
  runs='434 437 439 441 444 445 449 452 ' 
  for runNr in $runs; do 
    Calib $optionSub $calibFile2 $dataDirInH $dataDirOutH $runNr $PlotBaseDir CAEN_PlotsHadronCalibrated_2024/Run_ $skipLayer
  done;
elif [ $optionMain == "ScanE" ]; then
  calibFile0=$dataDirCal/calib_muonScanE_40V.root
  calibFile1=$dataDirCal/calib_muonScanE1_40V.root
  calibFile2=$dataDirCal/calib_muonScanE2_40V.root
  echo "running calibrate for 40V runs, campaing E"

  muonScanE_40V='463 464 481 478'
  pedScanE_40V='465 476'

  Calib $optionSub $calibFile0 $dataDirIn $dataDirOut muonScanE_40V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  Calib $optionSub $calibFile1 $dataDirIn $dataDirOut muonScanE1_40V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  Calib $optionSub $calibFile2 $dataDirIn $dataDirOut muonScanE2_40V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer

  # electron runs
  runs='466 467 468 471 472 ' 
  for runNr in $runs; do 
    Calib $optionSub $calibFile0 $dataDirInE $dataDirOutE $runNr $PlotBaseDir CAEN_PlotsElectronCalibrated_2024/Run_ $skipLayer
  done;
elif [ $optionMain == "ScanF" ]; then
  calibFile0=$dataDirCal/calib_muonScanF_41V_V2.root
  calibFile1=$dataDirCal/calib_muonScanF1_41V_V2.root
  calibFile2=$dataDirCal/calib_muonScanF2_41V_V2.root
  echo "running calibrate for 41V runs, campaing F"

  muonScanE_41V='486 489 507 506'
  pedScanE_41V='492 505'

  Calib $optionSub $calibFile0 $dataDirIn $dataDirOut muonScanF_41V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  Calib $optionSub $calibFile1 $dataDirIn $dataDirOut muonScanF1_41V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  Calib $optionSub $calibFile2 $dataDirIn $dataDirOut muonScanF2_41V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer

  # electron runs
  runs='494 495 497 502 504' 
  for runNr in $runs; do 
    Calib $optionSub $calibFile0 $dataDirInE $dataDirOutE $runNr $PlotBaseDir CAEN_PlotsElectronCalibrated_2024/Run_ $skipLayer
  done;
elif [ $optionMain == "ScanH" ]; then
  calibFile1=$dataDirCal/calib_muonScanH1.root
  calibFile2=$dataDirCal/calib_muonScanH2.root
  echo "running calibrate for 45V runs, campaing H"
  
  #muon runs
  muonScanH_45V='526 527 554 559'
  pedScanH_45V='528 552'
  Calib $optionSub $calibFile1 $dataDirIn $dataDirOut muonScanH1_45V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  Calib $optionSub $calibFile2 $dataDirIn $dataDirOut muonScanH2_45V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  
  # electron runs
  runs='529 530 533 535 538 541' 
  for runNr in $runs; do 
    Calib $optionSub $calibFile1 $dataDirInE $dataDirOutE $runNr $PlotBaseDir CAEN_PlotsElectronCalibrated_2024/Run_ $skipLayer
  done;
  
  #hadron runs
  runs='542 543 544 545 548 549 550 551' 
  for runNr in $runs; do 
    Calib $optionSub $calibFile1 $dataDirInH $dataDirOutH $runNr $PlotBaseDir CAEN_PlotsHadronCalibrated_2024/Run_ $skipLayer
  done;
elif [ $optionMain == "ScanG" ]; then
  calibFile1=$dataDirCal/calib_muonScanG_46V.root
  echo "running calibrate for 46V runs, campaing G"
  
  #muon runs
  muonScanG_46V='508 510 511 525'
  pedScanG_46V='521'
  Calib $optionSub $calibFile1 $dataDirIn $dataDirOut muonScanG_46V $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  
  # electron runs
  runs='513 514 516 517 520' 
  for runNr in $runs; do 
    Calib $optionSub $calibFile1 $dataDirInE $dataDirOutE $runNr $PlotBaseDir CAEN_PlotsElectronCalibrated_2024/Run_ $skipLayer
  done;
elif [ $optionMain == "MuonHVScan" ]; then
  Calib $optionSub $dataDirCal/calib_305_44V.root $dataDirIn $dataDirOut 305 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  Calib $optionSub $dataDirCal/calib_307_43V.root $dataDirIn $dataDirOut 307 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  Calib $optionSub $dataDirCal/calib_309_42V.root $dataDirIn $dataDirOut 309 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  Calib $optionSub $dataDirCal/calib_312_41V.root $dataDirIn $dataDirOut 312 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
  Calib $optionSub $dataDirCal/calib_316_40V.root $dataDirIn $dataDirOut 316 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_ $skipLayer
fi

