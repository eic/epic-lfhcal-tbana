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


if [ $2 == "mockUpSmallStack" ]; then 
#   runs='244'
#   runs='271 277 244 250 282 283'
  runs='271'
#   runs='244 250 282 283'
#   runs='492 505'
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/rawRedStack_$runNr.root -d 1 -f -m $configdir/TB2024/mappingFile_202409_CAEN_reduced.txt -r $runList
  done;
elif [ $2 == "FullSetA" ]; then 
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
elif [ $2 == "calibC" ]; then
  runs='376 375 377 404 405 410 408'
  for runNr in $runs; do
    ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;

elif [ $2 == "electronsA" ]; then 
  runs='251 252 254 257 258 ' 
  for runNr in $runs; do 
    ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "electronsC" ]; then 
  runs='379 380 381 384 387 ' 
  for runNr in $runs; do 
    ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "electronsD" ]; then 
  runs='421 422 429 430 432 ' 
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "electronsE" ]; then 
  runs='466 467 468 471 472 ' 
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "electronsF" ]; then 
  runs='494 495 497 502 504 ' 
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "electronsG" ]; then 
  runs='513 514 516 517 520 ' 
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "electronsH" ]; then 
  runs='529 530 533 535 538 541 '
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "hadronsA" ]; then 
  runs='261 264 265 269 270 272 274 275 ' 
  for runNr in $runs; do 
    ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "hadronsC" ]; then 
  runs='390 392 393 394 397 398 399 401' 
  for runNr in $runs; do 
    ./Convert -c $dataRaw/Run$runNr\_list.txt -o $dataDir/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "hadronsD" ]; then 
#   runs='434' 
  runs='434 437 439 441 444 445 449 452' 
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "hadronsH" ]; then 
  runs='542 543 544 545 548 549 550 551' 
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "pedandmuD1" ]; then 
  runs='412 417 420 '
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "fullScanC" ]; then
  runs='375 376 377 379 380 381 384 387 390 392 393 394 397 398 399 401 404 405 408 410'
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
elif [ $2 == "fullElectrons" ]; then  
#   runs='251 252 254 257 258 333 334 336 337 338 379 380 381 384 387 421 422 429 430 432 466 467 468 471 472 494 495 497 502 504 513 514 516 517 520 529 530 533 535 538 541 '
  runs='333 334 336 337 338 379 380 381 384 387 466 467 468 471 472 494 495 497 502 504 513 514 516 517 520 '
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
  
elif [ $2 == "fullHadrons" ]; then
#   runs='261 264 265 269 270 272 274 275 340 349 346 350 357 360 362 367 368 390 392 393 394 397 398 399 401 434 437 439 441 444 445 449 452 542 543 544 545 548 549 550 551' 
  runs='340 349 346 350 357 360 362 367 368 390 392 393 394 397 398 399 401 ' 
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;

elif [ $2 == "ScanH" ]; then
  runs='526 527 528 529 530 533 535 538 541 542 543 544 545 548 549 550 551 552 554 559'
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;

else 
#   runs='244	250	271	277	282	283	303	305	306	307	308	309	311	312	315	316	322	331	332	369	370	371	374	375	376	377	404	405	408	410	412	417	420	454	456	457	460	463	464	465	476	478	481	486	489	492	505	506	507	508	510	511	521	524	525	526	527	528	552	553	554	559' 
  runs='303	305	306	307	308	309	311	312	315	316	322	331	332	369	370	371	374	375	376	377	404	405	408	410	412	417	420	463	464	465	476	478	481	486	489	492	505	506	507	508	510	511	521	524	525	553	' 

  #412
  for runNr in $runs; do 
    ./Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $mappingFile -r $runList
  done;
fi
