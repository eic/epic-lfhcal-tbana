#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

PlotBaseDir=..
runList=../configs/FOCalTest2026/DataDB_LaserTestFocal_202606.csv

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
  dataDirRaw=/media/fbock/ALICE2-4TB/202607_FoCalTB/HGCROCData
  dataDirOut=/media/fbock/ALICE2-4TB/202607_FoCalTB/HGCROCData
  PlotBaseDir=/media/fbock/ALICE2-4TB/202607_FoCalTB/
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

# run pedest extraction for different run numbers
if [ $2 = "pedestal" ]; then
#   runs='154 155 156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222';
#   runs='160';
#   runs='164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179' ;
#   runs='180 181 182 183 184 185 186 187 188 189 190 191' ;
#   runs='192 205 206 207 208 209 210 211 212 213 214 215 216'
  runs='217 218 219 220 221 222'
  # different number of KCUs & asics
  for runNr in $runs; do 
    printf -v runNrPed "%03d" "$runNr"
    Pedestal $2 $runNrPed $dataDirRaw $dataDirOut Run$runNrPed pdf
#     ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNrPed.root -f -o $dataDirOut/rawHGCROC_wPed_$runNrPed.root -O $PlotBaseDir/PlotsPedestal/Run$runNrPed -r $runList
  done
fi

if [ $2 = "toaPhase" ]; then   
  if [ $3 = "FullSetB" ]; then
    runNrPed='071'
    if [ $4 = "Hadron" ]; then 
      runs=''
    elif [ $4 = "Muon" ]; then 
      runs='072' # 1st 0,0
    fi
  elif [ $3 = "HVScan" ]; then
    runNrPed='188'
    if [ $4 = "Muon" ]; then
      runs='202' # 1st 0,0
    fi
  fi
  if [ $4 = "Hadron" ]; then 
    for runNr in $runs; do 
      ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirOut/rawHGCROC_wPed_$runNrPed.root #-F png
    done
  elif [ $4 = "Muon" ]; then 
    for runNr in $runs; do 
      echo $runNr
      ./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_wPed_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirRaw/rawHGCROC_wPed_$runNrPed.root
      #./DataPrep -d 1 -f -i $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root -o $dataDirOut/rawHGCROC_toaPhase_$runNr.root -O $PlotBaseDir/ToAPhaseExtraction/Run$runNr -r $runList -g $dataDirRaw/rawHGCROC_miptrigg_wPedwMuon_wBC_$runNr.root #-F png
    done
  fi

fi

if [ $2 = "wave" ]; then
  echo "Add runnumbers before you try this & remove exit"
  exit
  runs=''
  for runNr in $runs; do
    WaveformHGCROC $3 $runNr $dataDirRaw Run_$runNr
  done
fi


if [ $2 == "calibMuon" ]; then
  toaPhaseOffset=''
  badChannelMap=../configs/FOCalTest2026/dummyFocalTB.txt
#   runs='154 155 156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179';
#   runs='156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222';
  runs='193';

#   runs='154 155 156 157 158 159 160 161 162 163 164 165 166 167 168 169 170 171 172 173 174 175 176 177 178 179 180 181 182 183 184 185 186 187 188 189 190 191 192 205 206 207 208 209 210 211 212 213 214 215 216 217 218 219 220 221 222';

  for runNr in $runs; do
    MuonCalibHGCROC $3 $runNr $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap $toaPhaseOffset 	
  done
fi
