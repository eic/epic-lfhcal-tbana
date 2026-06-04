#! /bin/bash

#include common helper functions to make it easier across years
source helperCalibHGCROC.sh

#run list file
runList=../configs/TB2025/DataTakingDB_202511_HGCROC.csv

dataDirCal=""
if [ $1 = "fbockTB" ]; then 
  dataDirCal=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
  dataDirIn=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
  dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
  PlotBaseDir=/media/fbock/Lennard4TB/202511_PST09/

elif [ $1 = "yale" ]; then
  dataDirCal=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202511_PST09/calibrated_new
  dataDirIn=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202511_PST09/rawroot_new
  dataDirOut=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202511_PST09/rawroot_new
  PlotBaseDir=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202511_PST09/rawroot_new
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

# apply calibration
if [ $2 == "FullSetA" ]; then
  calibFile1=$dataDirCal/calib_FullSetA1.root
  calibFile2=$dataDirCal/calib_FullSetA2.root
  toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  echo "running calibrate for 44V runs, campaign A"
  badChannelMap="../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt"
  
  #muon runs
  Calib $3 $calibFile1 $dataDirIn $dataDirOut Muon_FullSetA_1 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
  Calib $3 $calibFile2 $dataDirIn $dataDirOut Muon_FullSetA_2 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

  # hadron runs
  runs='h-_3GeV_FullSetA h-_5GeV_FullSetA h-_8GeV_FullSetA h-_10GeV_FullSetA h-_12GeV_FullSetA h-_15GeV_FullSetA h+_3GeV_FullSetA h+_5GeV_FullSetA h+_8GeV_FullSetA h+_10GeV_FullSetA h+_12GeV_FullSetA h+_15GeV_FullSetA'
  for runNr in $runs; do
    Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
  done;

  # electron runs
  runs='e-_1GeV_FullSetA e-_2GeV_FullSetA e-_3GeV_FullSetA e-_4GeV_FullSetA e-_5GeV_FullSetA e+_1GeV_FullSetA e+_2GeV_FullSetA e+_3GeV_FullSetA e+_4GeV_FullSetA e+_5GeV_FullSetA'
  for runNr in $runs; do
    Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
  done;



# electron runs
#   runs='166'  
#   runs='169'
#   runs='165 166 167 168 169 170' 
#   runs='191 192 193 194 195' 
#   for runNr in $runs; do 
#     Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
#   done;
# 
#   # positron runs
# #   runs='171 172 173 174 196 200 199 198 197'
#   runs='171 172 173 174 175 196 200 199 198 197'
#   for runNr in $runs; do 
#     Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
#   done;
# 
#   #hadron runs
#   runs='176 178 179 180 181 182 187 188'
#  runs='184 188'
#   runs='176 177 178 179 180 181 182 183 184 185 186 187 188'
#   runs='176 177 178 179 180 181 182 183 185 186 187'


elif [ $2 == "FullSetB" ]; then
  calibFile1=$dataDirCal/calib_FullSetB1.root
  calibFile2=$dataDirCal/calib_FullSetB2.root
  toaPhaseOffset=../configs/TB2025/ToAOffsets_TB2025_HadRun.csv
  echo "running calibrate for 45V runs, campaign B"
  badChannelMap="../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt"
  
  #muon runs
  Calib $3 $calibFile1 $dataDirIn $dataDirOut FullSetB_1 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
  Calib $3 $calibFile2 $dataDirIn $dataDirOut FullSetB_2 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset

  badChannelMap="../configs/TB2025/badChannel_HGCROC_PSTB2025_layer0.txt"
  # electron runs
#   189 190 
  runs='218 219 220 221 222 223 224 225 226 227 228 229 230 231 232 233 234 235 236'
  for runNr in $runs; do 
    Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
  done;

#   #hadron runs
  runs='237 238 239 240 241 242 243 244 245 246 247 248 249 250 251'
#   runs='251'
  for runNr in $runs; do 
    Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap $toaPhaseOffset
  done;
# elif [ $2 == "MuonHVScan" ]; then
#   Calib $3 $dataDirCal/calib_305_44V.root $dataDirIn $dataDirOut 305 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_
#   Calib $3 $dataDirCal/calib_307_43V.root $dataDirIn $dataDirOut 307 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_
#   Calib $3 $dataDirCal/calib_309_42V.root $dataDirIn $dataDirOut 309 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_
#   Calib $3 $dataDirCal/calib_312_41V.root $dataDirIn $dataDirOut 312 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_
#   Calib $3 $dataDirCal/calib_316_40V.root $dataDirIn $dataDirOut 316 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_
fi

