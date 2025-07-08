#! /bin/bash
configdir=/home/Aidan/WorkStuff/eic/epic-lfhcal-tbana/configs
pwd=$PWD
dataDir=""
if [ $1 = "fbock" ]; then 
	dataDir=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/MuonRuns
	dataRaw=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/MuonRuns
elif [ $1 = "fbockExt" ]; then 
	dataDir=/media/fbock/T7/CAEN_Sept24_TB_PS
	dataRaw=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata
elif [ $1 = "fbockTB" ]; then 
	dataDir=/media/fbock/LFHCal2/CAEN_Sept24_TB_PS
	dataRaw=/media/fbock/LFHCal2/202408_PST09/CAENData
elif [ $1 = "eglimos" ]; then 
	dataDir=/home/ewa/EIC/test_beam2024/fullScanC
	dataRaw=/home/ewa/EIC/test_beam2024/fullScanC
elif [ $1 = "vandrieu" ]; then
    dataDir=/home/vandrieu/cernbox/ePIC_2024_PStestbeam/CAEN_RO
elif [ $1 = "kmaret" ]; then
    dataDir=/mnt/d/202408_PST9
    dataRaw=/mnt/d/202408_PST9_converted
elif [ $1 = "ahill" ]; then
    dataDir=/home/Aidan/WorkStuff/eic/data
    dataRaw=$PWD/data_converted
    mkdir -p $dataRaw
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

if [ $2 == "single" ]; then 
#   runs='244'
  runs='271'
#   runs='492 505'
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/MuonRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "mockUpSmallStack" ]; then 
#   runs='244'
#   runs='271 277 244 250 282 283'
  runs='271'
#   runs='244 250 282 283'
#   runs='492 505'
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/MuonRuns/rawRedStack_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN_reduced.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "calib45V" ]; then 
#   runs='271 277 244 250 282 283 460 456 457 454 526 527 528 554 559 552' 
  runs='277 244 250 282 283 460 456 457 454 526 527 528 554 559 552' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/MuonRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;  
elif [ $2 == "calibA" ]; then 
  runs='271 277 244 250 282 283' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/MuonRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
  
elif [ $2 == "electronsA" ]; then 
  runs='251 252 254 257 258 ' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/ElectronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "electronsB" ]; then 
  runs='333 334 336 337 338 ' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/ElectronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "electronsC" ]; then 
  runs='379 380 381 384 387 ' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/electronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "electronsD" ]; then 
  runs='421 422 429 430 432 ' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/ElectronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "electronsE" ]; then 
  runs='466 467 468 471 472 ' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/ElectronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "electronsF" ]; then 
  runs='494 495 497 502 504 ' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/ElectronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "electronsG" ]; then 
  runs='513 514 516 517 520 ' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/ElectronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "electronsH" ]; then 
  runs='529 530 533 535 538 541 '
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/ElectronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "hadronsA" ]; then 
  runs='261 264 265 269 270 272 274 275 ' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/HadronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "hadronsB" ]; then 
  runs='340 349 346 350 357 360 362 367 368' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/HadronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "hadronsC" ]; then 
  runs='390 392 393 394 397 398 399 401' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/HadronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "hadronsD" ]; then 
#   runs='434' 
  runs='434 437 439 441 444 445 449 452' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/HadronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "hadronsH" ]; then 
  runs='542 543 544 545 548 549 550 551' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/HadronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "pedandmuD1" ]; then 
  runs='412 417 420 '
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/MuonRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "fullScanC" ]; then
  runs='375 376 377 379 380 381 384 387 390 392 393 394 397 398 399 401 404 405 408 410'
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
elif [ $2 == "fullElectrons" ]; then  
#   runs='251 252 254 257 258 333 334 336 337 338 379 380 381 384 387 421 422 429 430 432 466 467 468 471 472 494 495 497 502 504 513 514 516 517 520 529 530 533 535 538 541 '
  runs='333 334 336 337 338 379 380 381 384 387 466 467 468 471 472 494 495 497 502 504 513 514 516 517 520 '
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/ElectronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
  
elif [ $2 == "fullHadrons" ]; then
#   runs='261 264 265 269 270 272 274 275 340 349 346 350 357 360 362 367 368 390 392 393 394 397 398 399 401 434 437 439 441 444 445 449 452 542 543 544 545 548 549 550 551' 
  runs='340 349 346 350 357 360 362 367 368 390 392 393 394 397 398 399 401 ' 
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/HadronRuns/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;

elif [ $2 == "ScanH" ]; then
  runs='526 527 528 529 530 533 535 538 541 542 543 544 545 548 549 550 551 552 554 559'
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;

else 
#   runs='244	250	271	277	282	283	303	305	306	307	308	309	311	312	315	316	322	331	332	369	370	371	374	375	376	377	404	405	408	410	412	417	420	454	456	457	460	463	464	465	476	478	481	486	489	492	505	506	507	508	510	511	521	524	525	526	527	528	552	553	554	559' 
  runs='303	305	306	307	308	309	311	312	315	316	322	331	332	369	370	371	374	375	376	377	404	405	408	410	412	417	420	463	464	465	476	478	481	486	489	492	505	506	507	508	510	511	521	524	525	553	' 

  #412
  for runNr in $runs; do 
    Convert -c $dataDir/Run$runNr\_list.txt -o $dataRaw/raw_$runNr.root -d 1 -f -m $configdir/mappingFile_202409_CAEN.txt -r $configdir/DataTakingDB_202409_CAEN.csv
  done;
fi
