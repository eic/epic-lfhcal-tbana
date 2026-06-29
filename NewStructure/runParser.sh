#/bin/bash

function ParseRun(){
  ./ParseCalibSamples -i $1 -d 0 -I -m $3 -o testingPlots/calibout_$2.root -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv -n $2 -p testingPlots/Run$2
}

function ParseDACRun(){
  ./ParseCalibSamples -i $1 -d 0 -D -m $3 -o testingPlots/caliboutDAC_$2.root -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv -n $2 -p testingPlots/DACRun$2
}

# mappingFile=../configs/FOCalTest2026/mapping_injectionTest.txt
# mainPath=/media/fbock/ALICE2-4TB/202604_PST10/Calibrations/FOCalTests
# while IFS=$' ' read -r runs pathPart2 ; do
#   echo "${runs}"
#   echo "${pathPart2}"
#   filePath=$mainPath/${pathPart2}
#   echo $filePath ${runs} 
#   ParseRun $filePath ${runs} $mappingFile
# done < "configs/injectionsList_FOCal_042026.txt"
# 
mappingFile76=../configs/FOCalTest2026/mapping_injectionTest_072026_full.txt
mappingFile16=../configs/FOCalTest2026/mapping_injectionTest_07202616ch.txt
mainPath=/media/fbock/ALICE2-4TB/202604_PST10/Calibrations/FoCalTests072026
# while IFS=$' ' read -r runs pathPart2 ; do
#   echo "${runs}"
#   echo "${pathPart2}"
#   filePath=$mainPath/${pathPart2}
#   echo $filePath ${runs} 
#   ParseRun $filePath ${runs} $mappingFile76
# done < "configs/injectionsList_FOCal_072026_76ch.txt"
# 
while IFS=$' ' read -r runs pathPart2 ; do
  echo "${runs}"
  echo "${pathPart2}"
  filePath=$mainPath/${pathPart2}
  echo $filePath ${runs} 
  ParseRun $filePath ${runs} $mappingFile16
done < "configs/injectionsList_FOCal_072026_16ch.txt"

# 
# while IFS=$' ' read -r runs pathPart2 ; do
#   echo "${runs}"
#   echo "${pathPart2}"
#   filePath=$mainPath/${pathPart2}
#   echo $filePath ${runs} 
#   ParseDACRun $filePath ${runs} $mappingFile76
# done < "configs/injectionsDACList_FOCal_072026_76ch.txt"
# 
# while IFS=$' ' read -r runs pathPart2 ; do
#   echo "${runs}"
#   echo "${pathPart2}"
#   filePath=$mainPath/${pathPart2}
#   echo $filePath ${runs} 
#   ParseDACRun $filePath ${runs} $mappingFile16
# done < "configs/injectionsDACList_FOCal_072026_16ch.txt"


# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionCF4_CFcomp1_red.txt -o testingPlots/injectionCF4_CFcomp1.root -O testingPlots/injectionCF4_CFcomp1/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# # ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionCF7_CFcomp1.txt -o testingPlots/injectionCF7_CFcomp1.root -O testingPlots/injectionCF7_CFcomp1/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionCF7_CFcomp1_low.txt -o testingPlots/injectionCF7_CFcomp1_low.root -O testingPlots/injectionCF7_CFcomp1_low/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionCF7_CFcomp1_high.txt -o testingPlots/injectionCF7_CFcomp1_high.root -O testingPlots/injectionCF7_CFcomp1_high/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionCF7_CFcomp1_new.txt -o testingPlots/injectionCF7_CFcomp1_new.root -O testingPlots/injectionCF7_CFcomp1_new/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# 
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionCF2_CFcomp1_red.txt -o testingPlots/injectionCF2_CFcomp1.root -O testingPlots/injectionCF2_CFcomp1/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionCF6_CFcomp1_red.txt -o testingPlots/injectionCF6_CFcomp1.root -O testingPlots/injectionCF6_CFcomp1/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionCF8_CFcomp1.txt -o testingPlots/injectionCF8_CFcomp1.root -O testingPlots/injectionCF8_CFcomp1/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionRF3_CFcomp1.txt -o testingPlots/injectionRF3_CFcomp1.root -O testingPlots/injectionRF3_CFcomp1/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionRF3_CFcomp1_low.txt -o testingPlots/injectionRF3_CFcomp1_low.root -O testingPlots/injectionRF3_CFcomp1_low/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionRF3_CFcomp1_new.txt -o testingPlots/injectionRF3_CFcomp1_new.root -O testingPlots/injectionRF3_CFcomp1_new/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv

# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionRF5_CFcomp1.txt -o testingPlots/injectionRF5_CFcomp1.root -O testingPlots/injectionRF5_CFcomp1/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionRF12_CFcomp1.txt -o testingPlots/injectionRF12_CFcomp1.root -O testingPlots/injectionRF12_CFcomp1/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configs/injectionRF4_CFcomp1.txt -o testingPlots/injectionRF4_CFcomp1.root -O testingPlots/injectionRF4_CFcomp1/ -r ../configs/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
