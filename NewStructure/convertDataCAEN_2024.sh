#! /bin/bash
configdir=../configs
pwd=$PWD
dataDir=""
if [ $1 = "fbock" ]; then 
  dataRaw=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/MuonRuns
  dataDir=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/MuonRuns
elif [ $1 = "fbockExt" ]; then 
  dataRaw=/media/fbock/T7/LFHCalTBData/202408_PST09/rawCAEN
  dataDir=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENdata
elif [ $1 = "fbockTB" ]; then 
  dataRaw=/media/fbock/LFHCal2/CAEN_Sept24_TB_PS
  dataDir=/media/fbock/LFHCal2/202408_PST09/CAENData
elif [ $1 = "eglimos" ]; then 
  dataRaw=/home/ewa/EIC/test_beam2024/fullScanC
  dataDir=/home/ewa/EIC/test_beam2024/fullScanC
elif [ $1 = "vandrieu" ]; then
  dataDir=/home/vandrieu/cernbox/ePIC_2024_PStestbeam/CAEN_RO
elif [ $1 = "ahill" ]; then
  configdir=/home/Aidan/WorkStuff/eic/epic-lfhcal-tbana/configs
  dataRaw=/home/Aidan/WorkStuff/eic/data
  dataDir=$PWD/data_converted
  mkdir -p $dataRaw
elif [ $1 = "rjh78" ]; then
  dataDir=/Users/ryanhamilton/Documents/Research/data.nosync/202408_PST09/CAENData/outfiles 
  dataRaw=/Users/ryanhamilton/Documents/Research/data.nosync/202408_PST09/CAENData/rawfiles
elif [ $1 = "egpott" ]; then
  dataDir=/Users/egpott/rhig/lfhcal/data/outfiles
  dataRaw=/Users/egpott/rhig/lfhcal/data/rawfiles/aug2024_fullScanC
elif [ $1 = "yale" ]; then
  dataRaw=/media/lfhcal/LFHCal_Backup_11/Test_Beams/2024_PST9/rawroot
  dataDir=/media/lfhcal/LFHCal_Backup_11/Test_Beams/2024_PST9/CAEN_Sept24_TB_PS
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

# mappingFile=$configdir/TB2024/$configdir/TB2024/mappingFile_202409_CAEN.txt
mappingFile=$configdir/TB2024/mapping_CAEN_PST09_202409_CAEN_withSeg.txt
runList=$configdir/TB2024/DataTakingDB_202409_CAEN.csv

if [ $2 == "FullSetA" ]; then 
  if [ $3 == "convert" ]; then
    runs='271 277 244 250 282 283 251 252 254 257 258 261 264 265 269 270 272 274 275' #mu
  #   runs='271 277 244 250 282 283' #mu
    # runs='251 252 254 257 258 ' #e-
    # runs='261 264 265 269 270 272 274 275 ' #had
    for runNr in $runs; do 
      echo "to convert: " $dataRaw/Run$runNr\_list.txt
      ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
    done;
  elif [ $3 == "merge" ]; then
    echo "mergeing muon runs for Full Set A"
    hadd -f $dataDir/raw_muonScanA1_45V.root $dataDir/raw_244.root $dataDir/raw_250.root
    hadd -f $dataDir/raw_muonScanA2_45V.root $dataDir/raw_283.root $dataDir/raw_282.root
  fi
elif [ $2 == "FullSetB" ]; then 
  if [ $3 == "convert" ]; then
    runs='331 322 332 370 371 374 369 333 334 336 337 338 340 349 346 350 357 360 362 367 368'
  #   runs='331 322 332 370 371 374 369' #mu
  #   runs='333 334 336 337 338 ' #e-
  #   runs='340 349 346 350 357 360 362 367 368' #had
    for runNr in $runs; do 
      echo "to convert: " $dataRaw/Run$runNr\_list.txt
      ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
    done;
  elif [ $3 == "merge" ]; then
    echo "mergeing muon runs for Full Set B"
    hadd -f $dataDir/raw_muonScanB1_42V.root $dataDir/raw_331.root $dataDir/raw_322.root
    hadd -f $dataDir/raw_muonScanB2_42V.root $dataDir/raw_370.root $dataDir/raw_371.root $dataDir/raw_374.root
  fi
elif [ $2 == "FullSetC" ]; then 
  if [ $3 == "convert" ]; then
#     runs='377 404 405 410 408 379 380 381 384 387 390 392 393 394 397 398 399 401'
#   #   runs='376 375 377 404 405 410 408' #mu
#   #   runs='379 380 381 384 387' #e-
#   #   runs='390 392 393 394 397 398 399 401' #had
#     for runNr in $runs; do 
#       echo "to convert: " $dataRaw/Run$runNr\_list.txt
#       ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
#     done;
#       
    # First muon runs have partially broken files and need to be dealt with separately
    runNrMuonC1_1=376
    # event 90974 broken stopping there with the conversion
    ./Convert -c $dataRaw/Run$runNrMuonC1_1\_list.txt -o $dataDir/raw_$runNrMuonC1_1.root -d 1 -f -m $mappingFile -r $runList -L 90970
    runNrMuonC1_2=375
    # event 93811 broken stopping there with the conversion
    ./Convert -c $dataRaw/Run$runNrMuonC1_2\_list.txt -o $dataDir/raw_$runNrMuonC1_2.root -d 1 -f -m $mappingFile -r $runList -L 93800
  
  elif [ $3 == "merge" ]; then
    echo "mergeing muon runs for Full Set C"
    hadd -f $dataDir/raw_muonScanC1_43_5V.root $dataDir/raw_375.root $dataDir/raw_376.root
    hadd -f $dataDir/raw_muonScanC2_43_5V.root $dataDir/raw_405.root $dataDir/raw_410.root $dataDir/raw_408.root
  fi
elif [ $2 == "FullSetD" ]; then 
  if [ $3 == "convert" ]; then
    runs='460 456 457 454 421 422 429 430 432 434 437 439 441 444 445 449 452'
  #   runs='460 456 457 454' #mu
  #   runs='421 422 429 430 432' #e-
  #   runs='434 437 439 441 444 445 449 452' #had
    for runNr in $runs; do 
      echo "to convert: " $dataRaw/Run$runNr\_list.txt
      ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
    done;
  elif [ $3 == "merge" ]; then
    echo "mergeing muon runs for Full Set D"
    hadd -f $dataDir/raw_muonScanD2_45V.root $dataDir/raw_460.root $dataDir/raw_456.root $dataDir/raw_457.root
  fi
elif [ $2 == "FullSetH" ]; then 
  if [ $3 == "convert" ]; then
    runs='528 526 527 554 559 552 553 529 530 533 535 538 541 542 543 544 545 548 549 550 551'
  #   runs='528 526 527 554 559 552 553' #mu
  #   runs='529 530 533 535 538 541' #e-
  #   runs='542 543 544 545 548 549 550 551' #had
    for runNr in $runs; do 
      echo "to convert: " $dataRaw/Run$runNr\_list.txt
      ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
    done;
  elif [ $3 == "merge" ]; then
    echo "mergeing muon runs for Full Set H"
    hadd -f $dataDir/raw_muonScanH1_45V.root $dataDir/raw_526.root $dataDir/raw_527.root 
    hadd -f $dataDir/raw_muonScanH2_45V.root $dataDir/raw_554.root $dataDir/raw_559.root
  fi
elif [ $2 == "MiniSetE" ]; then 
  if [ $3 == "convert" ]; then
    runs='316 463 464 315 465 481 478 476 466 467 468 471 472'
  #   runs='316 463 464 315 465 481 478 476' #mu
  #   runs='466 467 468 471 472' #e-
    for runNr in $runs; do 
      echo "to convert: " $dataRaw/Run$runNr\_list.txt
      ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
    done;
  elif [ $3 == "merge" ]; then
    echo "mergeing muon runs for Mini Set E" 
    cp  $dataDir/raw_316.root $dataDir/raw_muonHVScan_40V.root
    hadd -f $dataDir/raw_muonScanE1_40V.root $dataDir/raw_463.root $dataDir/raw_464.root  
    hadd -f $dataDir/raw_muonScanE2_40V.root $dataDir/raw_481.root $dataDir/raw_478.root
    hadd -f $dataDir/raw_muonScanE_40V.root $dataDir/raw_muonScanE1_40V.root $dataDir/raw_muonScanE2_40V.root
  fi
elif [ $2 == "MiniSetF" ]; then 
  if [ $3 == "convert" ]; then
    runs='312 486 489 311 492 507 506 505 494 495 497 502 504'
  #   runs='312 486 489 311 492 507 506 505' #mu
  #   runs='494 495 497 502 504' #e-
    for runNr in $runs; do 
      echo "to convert: " $dataRaw/Run$runNr\_list.txt
      ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
    done;
  elif [ $3 == "merge" ]; then
    echo "mergeing muon runs for Mini Set F" 
    cp  $dataDir/raw_312.root $dataDir/raw_muonHVScan_41V.root
    hadd -f $dataDir/raw_muonScanF1_41V.root $dataDir/raw_486.root $dataDir/raw_489.root  
    hadd -f $dataDir/raw_muonScanF2_41V.root $dataDir/raw_507.root $dataDir/raw_506.root
    hadd -f $dataDir/raw_muonScanF_41V.root $dataDir/raw_muonScanF1_41V.root $dataDir/raw_muonScanF2_41V.root
  fi
elif [ $2 == "MiniSetG" ]; then 
  if [ $3 == "convert" ]; then
    runs='511 508 510 521 525 524 521 513 514 516 517 520'
  #   runs='511 508 510 521 525 524 521' #mu
  #   runs='513 514 516 517 520' #e-
    for runNr in $runs; do 
      echo "to convert: " $dataRaw/Run$runNr\_list.txt
      ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
    done;
  elif [ $3 == "merge" ]; then
    echo "mergeing muon runs for Mini Set G" 
    cp  $dataDir/raw_511.root $dataDir/raw_muonHVScan_46V.root
    hadd -f $dataDir/raw_muonScanG1_46V.root $dataDir/raw_511.root $dataDir/raw_508.root  $dataDir/raw_510.root  
    hadd -f $dataDir/raw_muonScanG2_46V.root $dataDir/raw_507.root $dataDir/raw_506.root
  fi  
elif [ $2 == "HVScan" ]; then 
  if [ $3 == "convert" ]; then
    runs='316 312 309 307 405 305 302 511 315 311 308 306 404 303 300 521'
  #   runs='316 312 309 307 405 305 302 511 ' #mu
  #   runs='315 311 308 306 404 303 300 521 ' #ped
    for runNr in $runs; do 
      echo "to convert: " $dataRaw/Run$runNr\_list.txt
      ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
    done;
  elif [ $3 == "merge" ]; then
    echo "moving muon runs for HV Scan" 
    cp  $dataDir/raw_316.root $dataDir/raw_muonHVScan_40V.root
    cp  $dataDir/raw_312.root $dataDir/raw_muonHVScan_41V.root
    cp  $dataDir/raw_309.root $dataDir/raw_muonHVScan_42V.root
    cp  $dataDir/raw_307.root $dataDir/raw_muonHVScan_43V.root
    cp  $dataDir/raw_405.root $dataDir/raw_muonHVScan_43_5V.root
    cp  $dataDir/raw_305.root $dataDir/raw_muonHVScan_44V.root
    cp  $dataDir/raw_302.root $dataDir/raw_muonHVScan_45V.root
    cp  $dataDir/raw_511.root $dataDir/raw_muonHVScan_46V.root
  fi  
else 
  echo "Set name undefined"
fi
