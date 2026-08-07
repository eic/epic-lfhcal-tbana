function MergeMuonsFileList(){
  echo "========================================================================="
  echo "dataDir: $1"
  echo "run-list: $2"
  echo "Additional name: $3"
  echo "========================================================================="
  runs=`cat $2`
  if [ -f listMerge.txt ]; then
    rm listMerge.txt
  fi
  for runNr in $runs; do 
    ls $1/rawHGCROC_$runNr.root  >> listMerge.txt
  done
  cat listMerge.txt
  fileList=`cat listMerge.txt`
  hadd -f $1/rawHGCROC_$3.root $fileList
  rm listMerge.txt
}


#! /bin/bash
echo "username $1"
echo "run option $2"

if [ $1 = "fbockTB" ]; then 
  dataRaw=/media/fbock/LFHCal2/Sep2024_PS_DataBKP/dump/hadron0830      # source directory for output files from DAQ system
  dataDir=/media/fbock/LFHCal2/202408_PST09/HGCROCData                 # base directory for root trees
fi

mapA=../configs/TB2024/mapping_HGCROC_PSTB2024_Run5-67_alternate.txt
mapB=../configs/TB2024/mapping_HGCROC_PSTB2024_Run68-117_alternate.txt
mapC=../configs/TB2024/mapping_HGCROC_PSTB2024_Run118-337_alternate.txt
runList=../configs/TB2024/DataTakingDB_202409_HGCROC.csv

# default path for the HGCROC analysis
if [ $2 = "muons" ]; then 
  if [ $3 = "convert" ]; then 
    mkdir -p $dataRaw/
    runs='184 185'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapC -r $runList
    done
    runs='275 276 277 278 279 280 281 282 283 ' #299 300
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapC -r $runList
    done
    runs='302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapC -r $runList
    done
  elif [ $3 = "merge" ]; then 
    runs='184 185 275 276 277 278 279 280 281 282 283 302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321'          #all
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt muonAll

    runs='303 305 307 308 309 310 313 314 320 321'          #neg
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt muonsNeg

    runs='302 304 306 311 312 315 316 317 318 319'          #pos
    echo $runs > runList.txt
    MergeMuonsFileList $dataDir runList.txt muonsPos
  fi

elif [ $2 = "electrons" ]; then 
  if [ $3 = "convert" ]; then 
    mkdir -p $dataRaw/
    # 4GeV (0,1)
    runs='327 328 329 330 331 332'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataRaw/rawHGCROC_$runNr.root -m $mapC -r $runList
    done
    #scan 1-5 GeV, (0,0)
    runs='103 104 105 106 107'
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataRaw/rawHGCROC_$runNr.root -m $mapB -r $runList
    done
  fi
elif [ $2 = "hadrons" ]; then 
  if [ $3 = "convert" ]; then 
    mkdir -p $dataRaw/
    runs='186 187 188 189 190 191 192 193 194 195 196 197 '
    for runNr in $runs; do 
      ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataRaw/rawHGCROC_$runNr.root -m $mapB -r $runList
    done
  fi
  
# switched on truncation for HGCROC data as alternate path	- EXPERIMENTAL
elif [ $2 = "muonsTruncated" ]; then 
  if [ $3 = "convert" ]; then 
    mkdir -p $dataRaw/
    runs='184 185'
    for runNr in $runs; do 
      ./Convert -d 0 -t -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROCtruncated_$runNr.root -m $mapC -r $runList
    done
    runs='275 276 277 278 279 280 281 282 283 ' #299 300
    for runNr in $runs; do 
      ./Convert -d 0 -t -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROCtruncated_$runNr.root -m $mapC -r $runList
    done
    runs='302 303 304 305 306 307 308 309 310 311 312 313 314 315 316 317 318 319 320 321'
    for runNr in $runs; do 
      ./Convert -d 0 -t -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROCtruncated_$runNr.root -m $mapC -r $runList
    done
  elif [ $3 = "merge" ]; then 
    hadd -f $dataDir/rawHGCROCtruncated_muonAll.root $dataDir/rawHGCROCtruncated_1*.root $dataDir/rawHGCROCtruncated_2*.root $dataDir/rawHGCROCtruncated_3*.root 
    runs='303 305 307 308 309 310 313 314 320 321'
    echo "" > listMerge.txt
    for runNr in $runs; do 
      ls $dataDir/rawHGCROCtruncated_$runNr.root  >> listMerge.txt
    done
    cat listMerge.txt
    fileList=`cat listMerge.txt`
    hadd -f $dataDir/rawHGCROCtruncated_muonsNeg.root $fileList
    runs='302 304 306 311 312 315 316 317 318 319'
    echo "" > listMerge.txt
    for runNr in $runs; do 
      ls $dataDir/rawHGCROCtruncated_$runNr.root  >> listMerge.txt
    done
    cat listMerge.txt
    fileList=`cat listMerge.txt`
    hadd -f $dataDir/rawHGCROCtruncated_muonsPos.root $fileList
  fi
fi
