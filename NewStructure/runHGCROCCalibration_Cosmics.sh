#! /bin/bash

PlotBaseDir=..
runNrFile=../configs/LocalTesting/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt

function MuonCalibHGCROC()
{
	
	echo "=================================================================================="
	echo "option $1"
	echo "run Nr Pedestal: $2"
	echo "run Nr Muon: $3"
	echo "dataRawDir: $4"
	echo "dataOutDir: $5"
	echo "OutNameRun:" $6
	if [ $1 == "BC" ]; then 
		echo "badchannelMap:" $7
	fi
	echo "=================================================================================="
	if [ $1 == "BC" ]; then 
    if [ -f "$4/rawHGCROC_wPed_$3_calib_mod.txt" ]; then
      echo "overwriting original calib file with manually modified $4/rawHGCROC_wPed_$3_calib_mod.txt"
      ./DataPrep -d 1 -e -f -P $4/rawHGCROC_wPed_$3.root -i $4/rawHGCROC_wPed_$3.root -o $4/rawHGCROC_wPed_wBC_$3.root -B $7 -O $PlotBaseDir/HGCROC_PlotsCalibTransfer/$6 -r $runNrFile -k $4/rawHGCROC_wPed_$3_calib_mod.txt     
    else 
      ./DataPrep -d 2 -e -f -P $4/rawHGCROC_wPed_$3.root -i $4/rawHGCROC_wPed_$3.root -o $4/rawHGCROC_wPed_wBC_$3.root -B $7 -O $PlotBaseDir/HGCROC_PlotsCalibTransfer/$6 -r $runNrFile 
    fi
	elif [ $1 == "wave" ]; then 
    if [ -f "$4/rawHGCROC_wPed_$3_calib_mod.txt" ]; then
      echo "overwriting original calib file with manually modified $4/rawHGCROC_wPed_$3_calib_mod.txt"
      ./DataPrep -d 1 -E -f -w $4/rawHGCROC_wPed_$3.root -i $4/rawHGCROC_wPed_$3.root -o $4/rawHGCROC_wave_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibWave/$6 -r $runNrFile -k $4/rawHGCROC_wPed_$3_calib_mod.txt     
    else 
      ./DataPrep -d 1 -E -f -w $4/rawHGCROC_wPed_$3.root -i $4/rawHGCROC_wPed_$3.root -o $4/rawHGCROC_wave_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibWave/$6 -r $runNrFile 
    fi
	elif [ $1 == "default" ]; then 
		time ./DataPrep -f -d 1 -e  -s -i $4/rawHGCROC_wPed_wBC_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuon/$6 -r $runNrFile
	elif [ $1 == "improved" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved/$6 -r $runNrFile
	elif [ $1 == "imp2nd" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp2_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved2nd/$6 -r $runNrFile
	elif [ $1 == "imp3rd" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp2_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp3_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved3rd/$6 -r $runNrFile
	elif [ $1 == "imp4th" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp3_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp4_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved4th/$6 -r $runNrFile
	elif [ $1 == "saveNewMuon" ]; then 
		time ./DataPrep -f -d 1 -M -i $5/rawPedAndMuon_$3.root -o $4/raw_muononly_$3.root 
	fi
}

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockCosm" ]; then 
	dataDirRaw=/media/fbock/LFHCal2/cosmics/TestSetupJune/converted
	dataDirOut=/media/fbock/LFHCal2/cosmics/TestSetupJune/converted
	PlotBaseDir=/media/fbock/LFHCal2/cosmics/TestSetupJune
elif [ $1 = "fbockCosmOct" ]; then 
	dataDirRaw=/media/fbock/LFHCal2/cosmics/TestSetupOct/converted
	dataDirOut=/media/fbock/LFHCal2/cosmics/TestSetupOct/converted
	PlotBaseDir=/media/fbock/LFHCal2/cosmics/TestSetupOct
elif [ $1 = "fbockSum" ]; then 
  dataDirRaw=/media/fbock/LFHCal2/SummingTest/convertedHGCROC
  dataDirOut=/media/fbock/LFHCal2/SummingTest/convertedHGCROC
  PlotBaseDir=/media/fbock/LFHCal2/SummingTest
elif [ $1 = "eglimos_CosmOct" ]; then
	dataDirRaw=/home/ewa/EIC/DATA/HGCROCData/Cosmics_Oct25/converted
	dataDirOut=/home/ewa/EIC/DATA/HGCROCData/Cosmics_Oct25/converted
	PlotBaseDir=/home/ewa/EIC/DATA/HGCROCData/Cosmics_Oct25
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

# pedestal runs 
if [ $2 = "pedestalSkimmed" ]; then
  runNr="007"
  ./DataPrep -d 1 -p -i $dataDirRaw/rawHGCROCskimmed_$runNr.root -f -o $dataDirOut/rawHGCROCskimmed_wPed_$runNr.root -O $PlotBaseDir/PlotsPedestal/Run$runNr -r ../configs/LocalTesting/DataTakingDB_ORNL_HGCROC.txt
fi

if [ $2 = "pedestalJune" ]; then
  runNr="007"
  ./DataPrep -d 1 -p -i $dataDirRaw/rawHGCROC_$runNr.root -f -o $dataDirOut/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/PlotsPedestalUnSkimmed/Run$runNr -r ../configs/LocalTesting/DataTakingDB_ORNL_HGCROC.txt
fi

if [ $2 = "pedestalOct" ]; then
#   runs="008 004 005 013 014 017 018"
#   runs="018"
#   for runNr in $runs; do 
#     ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNr.root -f -o $dataDirOut/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/PlotsPedestalUnSkimmed/Run$runNr -r ../configs/LocalTesting/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt		
#   done
  
#   runs='018' # F-Stack,  ORNL-01
# 	runs='020 021 022' # F-Stack,  UCR-01 asic 1
# 	runs="028 029" # F-Stack,  UCR-01 asic 0
# 	runs='030 031 032 033 034'  #UCR-01 30-32, UCR-02 33-41, starting 38 T0A lower DAC calib
#   runs='018 020 021 022 028 029 030 031 032 033 034'  #UCR-01 30-32, UCR-02 33-41, starting 38 T0A lower DAC calib
#   runs='036 037 038 039 040 041'  #UCR-01 30-32, UCR-02 33-41, starting 38 T0A lower DAC calib
  runs='051 052 053 054'  #UCR-02 51-52, UCR-01 53-54, low ToA
  for runNr in $runs; do 
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNr.root -f -o $dataDirOut/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/PlotsPedestalUnSkimmed/Run$runNr -r ../configs/LocalTesting/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt		
  done
fi

if [ $2 == "calibMuonOct" ]; then
# 	runs='018' 
# 	runs='018 020 021 022 028 029 030 031 032 033 034'  #UCR-01 30-32, UCR-02 33-41, starting 38 T0A lower DAC calib
# 	runs='036 037 038 039 040 041'  #UCR-01 30-32, UCR-02 33-41, starting 38 T0A lower DAC calib
	
# 	runs='018 021 022 029 032 034 036 038 041'
	badChannelMap=../configs/LocalTesting/badChannelMap_TBSetup_HGCROC_cosmics_202510_Fstack.txt
	runNrFile=../configs/LocalTesting/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt
	pedestalRun='051' 
	runs='052' 
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $pedestalRun $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi

if [ $2 = "pedestalSumming" ]; then
#   runs="100 101"
  runs="100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115"
  for runNr in $runs; do 
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNr.root -f -o $dataDirOut/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/PlotsPedestalUnSkimmed/Run$runNr -r ../configs/LocalTesting/DataTakingDB_ORNL_Summing_HGCROC_202508.txt		
  done
fi

if [ $2 == "calibSum" ]; then
#   runs="100 101 102 103 104 105 106 107 108 109 110 111 112 113 114 115"
  runs="107"
# 	runs="102"
	badChannelMap=../configs/LocalTesting/badChannelMap_TBSetup_HGCROC_cosmics_202510_Fstack.txt
	runNrFile=../configs/LocalTesting/DataTakingDB_ORNL_Summing_HGCROC_202508.txt
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runNr $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi


