#! /bin/bash

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

echo "username $1"

# different mapping files for layering
mapping=../configs/FOCalTest2026/mapping_LaserTestFocal_202607.csv
# mapping=../configs/FOCalTest2026/mapping_injectionTest_072026_full.txt   # v2 summing board


if [ $1 = "fbockTB" ]; then 
  dataRaw=/media/fbock/ALICE2-4TB/202607_FoCalTB/rawTesting/fixed/    # source directory for output files from DAQ system
  dataDir=/media/fbock/ALICE2-4TB/202607_FoCalTB/HGCROCData           # base directory for root trees  fi
fi

# global run list for 2026 SPS TB
runList=../configs/FOCalTest2026/DataDB_LaserTestFocal_202606.csv

# runs='154 155 156 157 158 159 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192'
# runs='205 206 207 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222'
runs='193'
for runNr in $runs; do 
  ./Convert -d 0 -f -w -c $dataRaw/Run$runNr.h2g -o $dataDir/rawHGCROC_$runNr.root -m $mapping -r $runList
done
