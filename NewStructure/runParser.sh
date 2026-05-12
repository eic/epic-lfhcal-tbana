#/bin/bash

function ParseRun(){
  ./ParseCalibSamples -i $1 -d 1 -I -m ../configs/FOCalTest2026/mapping_injectionTest.txt -o testingPlots/calibout_$2.root -r /home/fbock/EIC/Software/epic-LFHCalTB/configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv -n $2 #-p testingPlots/Run$2

}

mainPath=/media/fbock/Lennard4TB/202604_PST10/Calibrations

run=1
filePath=$mainPath/FOCALTests-DefSettingsShihai-ORNL04-05/test_wo_detector/302_ToTCalib_Target400/205_Injection_Low/205_Injection_20260424_165941/205_Injection_asic4_injdac250_mg7_pack8_chn76
ParseRun $filePath $run

run=2
filePath=$mainPath/FOCALTests-DefSettingsShihai-ORNL04-05/test_wo_detector/302_ToTCalib_Target400/205_Injection_Low/205_Injection_20260424_153821/205_Injection_asic4_injdac100_mg7_pack8_chn76
ParseRun $filePath $run


run=3
filePath=$mainPath/FOCALTests-DefSettingsShihai-ORNL04-05/test_wo_detector/302_ToTCalib_Target400/205_Injection_Low/205_Injection_20260424_153054/205_Injection_asic4_injdac400_mg7_pack8_chn76
ParseRun $filePath $run

# mainPath=/media/fbock/Lennard4TB/202604_PST10/Calibrations/modifiedInjections
# run=4
# filePath=$mainPath/FOCALTests-settings_10_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_210504/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=5
# filePath=$mainPath/FOCALTests-settings_10_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_205436/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=6
# filePath=$mainPath/FOCALTests-settings_11_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260426_120805/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=7
# filePath=$mainPath/FOCALTests-settings_11_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260426_115343/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=8
# filePath=$mainPath/FOCALTests-settings_3_7_5_12_ORNL03-Valpo01/205_Injection_High/205_Injection_20260427_045126/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=9
# filePath=$mainPath/FOCALTests-settings_3_7_5_12_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260427_044500/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=10
# filePath=$mainPath/FOCALTests-settings_3_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_204143/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=11
# filePath=$mainPath/FOCALTests-settings_3_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_203243/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# 
# run=12
# filePath=$mainPath/FOCALTests-settings_3_7_5_8_ORNL03-Valpo01/205_Injection_High/205_Injection_20260427_021617/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=13
# filePath=$mainPath/FOCALTests-settings_3_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_203243/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=14
# filePath=$mainPath/FOCALTests-settings_4_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_201610/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=15
# filePath=$mainPath/FOCALTests-settings_4_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_200726/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=16
# filePath=$mainPath/FOCALTests-settings_5_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_174555/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=17
# filePath=$mainPath/FOCALTests-settings_5_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_173712/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=18
# filePath=$mainPath/FOCALTests-settings_8_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_215731/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=19
# filePath=$mainPath/FOCALTests-settings_8_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_214335/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=20
# filePath=$mainPath/FOCALTests-settings_9_7_9_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_151010/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=21
# filePath=$mainPath/FOCALTests-settings_9_7_9_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_150316/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=22
# filePath=$mainPath/FOCALTests-settings_9_7_9_4_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_161639/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=23
# filePath=$mainPath/FOCALTests-settings_9_7_9_4_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_153858/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=24
# filePath=$mainPath/FOCALTests-settings_9_7_9_4_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_160124/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=25
# filePath=$mainPath/FOCALTests-settings_12_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_181246/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=26
# filePath=$mainPath/FOCALTests-settings_12_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_180511/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=27
# filePath=$mainPath/FOCALTests-settings_1_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_184447/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=28
# filePath=$mainPath/FOCALTests-settings_1_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_183227/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=29
# filePath=$mainPath/FOCALTests-settings_2_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_194740/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=30
# filePath=$mainPath/FOCALTests-settings_2_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_194049/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=31
# filePath=$mainPath/FOCALTests-settings_7_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260426_113424/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=32
# filePath=$mainPath/FOCALTests-settings_7_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260426_112639/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
# run=33
# filePath=$mainPath/FOCALTests-settings_9_7_5_1_ORNL03-Valpo01/205_Injection_High/205_Injection_20260425_154622/205_Injection_asic4_injdac400_mg7_pack8_chn76
# ParseRun $filePath $run
# run=34
# filePath=$mainPath/FOCALTests-settings_9_7_5_1_ORNL03-Valpo01/205_Injection_Low/205_Injection_20260425_153858/205_Injection_asic4_injdac250_mg7_pack8_chn76
# ParseRun $filePath $run
