#! /bin/bash
# Mean cluster/cell energies vs beam energy across runs, from the histogram files of runShapeAnalyzer_TBSPSH2_2026.sh
# usage: ./runCompareRuns_TBSPSH2_2026.sh <user> <adc|full|tot_fit> ["<runs>"] [plot extension]
#   e.g. ./runCompareRuns_TBSPSH2_2026.sh atamis full "444 445 447 452"
#   without a run list the runs below are used; the runs must have been processed with the same mode first
#   output: $plotDir/RunComparison/<mode>/
plotDir=""
if [ "$1" = "eglimos" ]; then
  plotDir=/home/ewa/EIC/DATA/testBeam_Nov2025/2025_12_10_DataFromFredi/Output/EventDisplay
elif [ "$1" = "fbockTB" ]; then
  plotDir=/media/fbock/Lennard4TB/202604_PST10/EventDisplay/
elif [ "$1" = "yale" ]; then
  plotDir=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/EventDisplay
elif [ "$1" = "atamis" ]; then
  plotDir=/mnt/c/Users/tamis/Documents/eic/LFHCALDATA/Plots/SPS/EventDisplay
else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

mode=$2
if [[ "$mode" != "adc" && "$mode" != "full" && "$mode" != "tot_fit" ]]; then
  echo "Please select the energies to compare: adc, full or tot_fit"
  exit
fi

runs='444 445 446 447 448 449 450 451 452'
if [ -n "$3" ]; then
  runs="$3"
fi
ext=${4:-pdf}

histDir=$plotDir/HistFiles
outDir=$plotDir/RunComparison/$mode
root -l -b -q "CompareRuns_ShapeAnalyzer.C(\"$histDir\", \"$mode\", \"$runs\", \"$outDir\", \"$ext\")"
