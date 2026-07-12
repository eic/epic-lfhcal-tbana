#/bin/bash

function ParseRun(){
  ./ParseCalibSamples -i $1 -d 0 -I -m $3 -o InjectionOutputs/calibout_$2.root -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv -n $2 -p InjectionOutputs/Run$2
}

function ParseDACRun(){
  ./ParseCalibSamples -i $1 -d 0 -D -m $3 -o InjectionOutputs/caliboutDAC_$2.root -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv -n $2 -p InjectionOutputs/DACRun$2
}

# mappingFile=../configsComparisons/FOCalTest2026/mapping_injectionTest.txt
# mainPath=/media/fbock/ALICE2-4TB/202604_PST10/Calibrations/FOCalTests
# while IFS=$' ' read -r runs pathPart2 ; do
#   echo "${runs}"
#   echo "${pathPart2}"
#   filePath=$mainPath/${pathPart2}
#   echo $filePath ${runs} 
#   ParseRun $filePath ${runs} $mappingFile
# done < "configsComparisons/injectionsList_FOCal_042026.txt"
# 
mappingFile76=../configsComparisons/FOCalTest2026/mapping_injectionTest_072026_full.txt
mappingFile16=../configsComparisons/FOCalTest2026/mapping_injectionTest_07202616ch.txt
mainPath=/media/fbock/ALICE2-4TB/202604_PST10/Calibrations/FoCalTests072026
# while IFS=$' ' read -r runs pathPart2 ; do
#   echo "${runs}"
#   echo "${pathPart2}"
#   filePath=$mainPath/${pathPart2}
#   echo $filePath ${runs} 
#   ParseRun $filePath ${runs} $mappingFile76
# done < "configsComparisons/injectionsList_FOCal_072026_76ch.txt"
# 
while IFS=$' ' read -r runs pathPart2 ; do
  echo "${runs}"
  echo "${pathPart2}"
  filePath=$mainPath/${pathPart2}
  echo $filePath ${runs} 
  ParseRun $filePath ${runs} $mappingFile16
done < "configsComparisons/injectionsList_FOCal_072026_16ch.txt"

# 
# while IFS=$' ' read -r runs pathPart2 ; do
#   echo "${runs}"
#   echo "${pathPart2}"
#   filePath=$mainPath/${pathPart2}
#   echo $filePath ${runs} 
#   ParseDACRun $filePath ${runs} $mappingFile76
# done < "configsComparisons/injectionsDACList_FOCal_072026_76ch.txt"
# 
# while IFS=$' ' read -r runs pathPart2 ; do
#   echo "${runs}"
#   echo "${pathPart2}"
#   filePath=$mainPath/${pathPart2}
#   echo $filePath ${runs} 
#   ParseDACRun $filePath ${runs} $mappingFile16
# done < "configsComparisons/injectionsDACList_FOCal_072026_16ch.txt"


# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionCF4_CFcomp1_red.txt -o InjectionOutputs/injectionCF4_CFcomp1.root -O InjectionOutputs/injectionCF4_CFcomp1/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# # ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionCF7_CFcomp1.txt -o InjectionOutputs/injectionCF7_CFcomp1.root -O InjectionOutputs/injectionCF7_CFcomp1/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionCF7_CFcomp1_low.txt -o InjectionOutputs/injectionCF7_CFcomp1_low.root -O InjectionOutputs/injectionCF7_CFcomp1_low/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionCF7_CFcomp1_high.txt -o InjectionOutputs/injectionCF7_CFcomp1_high.root -O InjectionOutputs/injectionCF7_CFcomp1_high/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionCF7_CFcomp1_new.txt -o InjectionOutputs/injectionCF7_CFcomp1_new.root -O InjectionOutputs/injectionCF7_CFcomp1_new/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# 
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionCF2_CFcomp1_red.txt -o InjectionOutputs/injectionCF2_CFcomp1.root -O InjectionOutputs/injectionCF2_CFcomp1/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionCF6_CFcomp1_red.txt -o InjectionOutputs/injectionCF6_CFcomp1.root -O InjectionOutputs/injectionCF6_CFcomp1/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionCF8_CFcomp1.txt -o InjectionOutputs/injectionCF8_CFcomp1.root -O InjectionOutputs/injectionCF8_CFcomp1/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionRF3_CFcomp1.txt -o InjectionOutputs/injectionRF3_CFcomp1.root -O InjectionOutputs/injectionRF3_CFcomp1/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionRF3_CFcomp1_low.txt -o InjectionOutputs/injectionRF3_CFcomp1_low.root -O InjectionOutputs/injectionRF3_CFcomp1_low/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionRF3_CFcomp1_new.txt -o InjectionOutputs/injectionRF3_CFcomp1_new.root -O InjectionOutputs/injectionRF3_CFcomp1_new/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv

# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionRF5_CFcomp1.txt -o InjectionOutputs/injectionRF5_CFcomp1.root -O InjectionOutputs/injectionRF5_CFcomp1/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionRF12_CFcomp1.txt -o InjectionOutputs/injectionRF12_CFcomp1.root -O InjectionOutputs/injectionRF12_CFcomp1/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
# ./CompareInjection -d 1 -e 1 -E 1 -f -H -I configsComparisons/injectionRF4_CFcomp1.txt -o InjectionOutputs/injectionRF4_CFcomp1.root -O InjectionOutputs/injectionRF4_CFcomp1/ -r ../configsComparisons/FOCalTest2026/DataDB_InjectionTestFocal_202604.csv
