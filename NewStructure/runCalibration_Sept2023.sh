#! /bin/bash

outDirPlot=".."
function PedestalCalibSept2023()
{
  echo "=================================================================================="
  echo "run Nr: $1"
  echo "dataRawDir: $2"
  echo "dataOutDir: $3"
  echo "=================================================================================="
  ./DataPrep -a -d 1 -y 2023 -p -i $2/raw_$1.root -f -o $3/PedestalCalib_$1.root -O $outDirPlot/PlotsCalib_Sept2023/Run$1 -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv  
}

function PedestalCalibSeptReext2023()
{
  echo "=================================================================================="
  echo "run Nr: $1"
  echo "dataRawDir: $2"
  echo "dataOutDir: $3"
  echo "=================================================================================="
  ./DataPrep -d 1 -y 2023 -p -i $2/raw_pedonly_$1.root -f -o $3/PedestalCalib2nd_$1.root -O $outDirPlot/PlotsCalib_Sept2023/PedRe_Run$1 -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
  
}

function MuonCalibSept2023()
{
    echo "=================================================================================="
    echo "option $1"
    echo "run Nr: $2"
    echo "dataRawDir: $3"
    echo "dataOutDir: $4"
    echo "transfer option: $5"
    echo "=================================================================================="
    if [ $1 == "transfer" ]; then 
      if [ $5 == "overwrite" ]; then 
        ./DataPrep -d 1 -y 2023 -f -P $4/PedestalCalib2nd_$2.root -i $3/raw_$2.root -o $3/rawPed_$2.root -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
      else 
        ./DataPrep -d 1 -y 2023 -f -P $4/PedestalCalib_$2.root -i $3/raw_$2.root -o $3/rawPed_$2.root -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
      fi
    elif [ $1 == "default" ]; then 
      ./DataPrep -f -d 1 -y 2023 -s -i $3/rawPed_$2.root -o $4/rawPedAndMuon_$2.root -O $outDirPlot/PlotsCalibMuon_2023/Run$2 -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
    elif [ $1 == "improved" ]; then 
      ./DataPrep -f -d 1 -y 2023 -S -i $4/rawPedAndMuon_$2.root -o $4/rawPedAndMuonImp_$2.root -O $outDirPlot/PlotsCalibMuonImproved_2023/Run$2 -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
    elif [ $1 == "improved2nd" ]; then 
      ./DataPrep -f -d 1 -y 2023 -S -i $4/rawPedAndMuonImp_$2.root -o $4/rawPedAndMuonImp2nd_$2.root -O $outDirPlot/PlotsCalibMuonImproved_2023/Run$2_2ndIte -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
    elif [ $1 == "noise" ]; then 
      ./DataPrep -f -d 1 -y 2023 -n -i $4/rawPedAndMuon_$2.root -o $4/rawPedAndMuonNoise_$2.root -O $outDirPlot/PlotsCalibNoiseRe_2023/Run$2 -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
    elif [ $1 == "transferAlt" ]; then 
      ./DataPrep -d 1 -y 2023 -f -P $4/rawPedAndMuonNoise_$2.root -i $3/raw_$2.root -o $3/rawPedImp_$2.root -r $outDirPlot/configs/DataTakingDB_TBSept2023_SPSH4.csv
    elif [ $1 == "defaultImpPed" ]; then 
      ./DataPrep -f -d 1 -y 2023 -s -i $3/rawPedImp_$2.root -o $4/rawPedImpAndMuon_$2.root -O $outDirPlot/PlotsCalibMuonPedImp_2023/Run$2 -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
    elif [ $1 == "saveNewPed" ]; then 
      ./DataPrep -f -d 1 -N -i $4/rawPedAndMuonNoise_$2.root -o $3/raw_pedonly_$2.root 
    elif [ $1 == "saveNewMuon" ]; then 
      ./DataPrep -f -d 1 -M -i $4/rawPedAndMuon_$2.root -o $3/raw_muononly_$2.root 
    elif [ $1 == "improvedMinimal" ]; then 
      ./DataPrep -f -d 1 -y 2023 -S -i $3/raw_muononly_$2.root -o $4/rawPedAndMuonImpMinimal_$2.root -O $outDirPlot/PlotsCalibMuonImproved_2023/Run$2_MinimalSet -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
    elif [ $1 == "improvedMinimal2nd" ]; then 
      ./DataPrep -f -d 1 -y 2023 -S -i $4/rawPedAndMuonImpMinimal_$2.root -o $4/rawPedAndMuonImpMinimal2nd_$2.root -O $outDirPlot/PlotsCalibMuonImproved_2023/Run$2_MinimalSet2nd -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
    elif [ $1 == "improvedMinimal3rd" ]; then 
      ./DataPrep -f -d 1 -y 2023 -S -i $4/rawPedAndMuonImpMinimal2nd_$2.root -o $4/rawPedAndMuonImpMinimal3rd_$2.root -O $outDirPlot/PlotsCalibMuonImproved_2023/Run$2_MinimalSet3rd -r ../configs/DataTakingDB_TBSept2023_SPSH4.csv
    fi
}


dataDirRaw=""
dataDirOut=""
baseDir=""


if [ $1 = "fbock" ]; then 
  baseDir=/media/fbock/Samsung_T5/LFHCAL_TB/202309_SPSH4
elif [ $1 = "fbockLFHCal2" ]; then 
  baseDir=/media/fbock/LFHCal2/202309_SPSH4
  outDirPlot=/media/fbock/LFHCal2/202309_SPSH4
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

#mapping file ../configs/mappingTBSetupNewStructure_Sept2023_SPSfirstdays.txt 
#run list file  ../configs/DataTakingDB_TBSept2023_SPSH4.csv

# pedestal runs 
if [ $2 = "pedestal" ]; then

  dataDirOut=$baseDir/Calibrations
  pedestalRuns='47 48 50 51 52 53 54 74 75 76 77 78 79 80 81 82 142 143 144 145 146 147 148 149'   
  dataDirRaw=$baseDir/pionsEnergyScan
  for runNr in $pedestalRuns; do
      PedestalCalibSept2023 $runNr $dataDirRaw $dataDirOut
  done;
  
#   pedestalRuns='120 121' 
#   dataDirRaw=$baseDir/pions300GeV_positionScan
#   for runNr in $pedestalRuns; do
#       PedestalCalibSept2023 $runNr $dataDirRaw $dataDirOut
#   done;
#   pedestalRuns='30 31 32 33 34 35 36 ' 
#   dataDirRaw=$baseDir/pions360GeV_PositionScan
#   for runNr in $pedestalRuns; do
#       PedestalCalibSept2023 $runNr $dataDirRaw $dataDirOut
#   done;
# 
#   pedestalRuns='99 100 101 102 104 124 125' 
#   dataDirRaw=$baseDir/electron60GeV-VoltageScan
#   for runNr in $pedestalRuns; do
#       PedestalCalibSept2023 $runNr $dataDirRaw $dataDirOut
#   done;
# 
#   pedestalRuns='105 106 107 108' 
#   dataDirRaw=$baseDir/electron60GeV_GainScan
#   for runNr in $pedestalRuns; do
#       PedestalCalibSept2023 $runNr $dataDirRaw $dataDirOut
#   done;
# 
#   pedestalRuns='55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 83 85 86 87 88 89 90 91 140 141' 
#   dataDirRaw=$baseDir/electron_EnergyScan
#   for runNr in $pedestalRuns; do
#       PedestalCalibSept2023 $runNr $dataDirRaw $dataDirOut
#   done;
# 
#   pedestalRuns='3 20 21 22 23 24 150 152 153'
#   dataDirRaw=$baseDir/electrons100GeV_positionScan
#   for runNr in $pedestalRuns; do
#     PedestalCalibSept2023 $runNr $dataDirRaw $dataDirOut
#   done;

elif [ $2 = "pedestalRe" ]; then

  dataDirOut=$baseDir/Calibrations
#   pedestalRuns='47 48 50 51 52 53 54 74 75 76 77 78 79 80 81 82 142 143 144 145 146 147 148 149' 
#   pedestalRuns='47' 
#   dataDirRaw=$baseDir/pionsEnergyScan
#   for runNr in $pedestalRuns; do
#       PedestalCalibSeptReext2023 $runNr $dataDirRaw $dataDirOut
#   done;

#   pedestalRuns='120 121' 
  pedestalRuns='120' 
  dataDirRaw=$baseDir/pions300GeV_positionScan
  for runNr in $pedestalRuns; do
      PedestalCalibSeptReext2023 $runNr $dataDirRaw $dataDirOut
  done;
#     
  
elif [ $2 = "muoncalib" ]; then

  dataDirOut=$baseDir/Calibrations
  dataDirRaw=$baseDir/pionsEnergyScan
  runs='47 48 50 51 52 53 54 74 75 76 77 78 79 80 81 82 142 143 144 145 146 147 148 149' 
#   runs='47' 
#   runs='48' 
  for runNr in $runs; do
    MuonCalibSept2023 $3 $runNr $dataDirRaw $dataDirOut $4
  done

#   runs='120 121' 
#   runs='120' 
#   dataDirRaw=$baseDir/pions300GeV_positionScan
#   for runNr in $runs; do
#     MuonCalibSept2023 $3 $runNr $dataDirRaw $dataDirOut $4
#   done;
  
  
fi


