#! /bin/bash

PlotBaseDir=..
runNrFile=../configs/LocalTesting/DataTakingDB_ORNL_HGCROC_Cosmics_Longboard_202512.csv

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
      ./DataPrep -d 1 -e -f -P $4/rawHGCROC_wPed_$2.root -i $4/rawHGCROC_$3.root -o $4/rawHGCROC_wPed_wBC_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibTransfer/$6 -r $runNrFile -k $4/rawHGCROC_wPed_$3_calib_mod.txt -B $7     
    else 
      ./DataPrep -d 1 -e -f -P $4/rawHGCROC_wPed_$2.root -i $4/rawHGCROC_$3.root -o $4/rawHGCROC_wPed_wBC_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibTransfer/$6 -r $runNrFile -B $7 
    fi
	elif [ $1 == "wave" ]; then 
    if [ -f "$4/rawHGCROC_wPed_$3_calib_mod.txt" ]; then
      echo "overwriting original calib file with manually modified $4/rawHGCROC_wPed_$3_calib_mod.txt"
      ./DataPrep -d 1 -E -f -w $4/rawHGCROC_wPed_$2.root -i $4/rawHGCROC_$3.root -o $4/rawHGCROC_wave_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibWave/$6 -r $runNrFile -k $4/rawHGCROC_wPed_$3_calib_mod.txt     
    else 
      ./DataPrep -d 1 -E -f -w $4/rawHGCROC_wPed_$2.root -i $4/rawHGCROC_$3.root -o $4/rawHGCROC_wave_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibWave/$6 -r $runNrFile 
    fi
	elif [ $1 == "default" ]; then 
		time ./DataPrep -f -d 1 -e  -s -i $4/rawHGCROC_wPed_wBC_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuon/$6 -r $runNrFile
	elif [ $1 == "imp1st" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved/$6 -r $runNrFile
	elif [ $1 == "imp2nd" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp2_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved2nd/$6 -r $runNrFile
	elif [ $1 == "imp3rd" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp2_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp3_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved3rd/$6 -r $runNrFile
	elif [ $1 == "imp4th" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp3_$3.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp4_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved4th/$6 -r $runNrFile
	elif [ $1 == "saveNewMuon" ]; then 
		time ./DataPrep -f -d 1 -X -i $4/rawHGCROC_wPedwMuon_wBC_$3.root -o $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_$3.root 
	elif [ $1 == "imp1st_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_$3.root -o $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved_Red/$6 -r $runNrFile
	elif [ $1 == "imp2nd_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp_$3.root -o $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp2_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved2nd_Red/$6 -r $runNrFile
	elif [ $1 == "imp3rd_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp2_$3.root -o $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp3_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved3rd_Red/$6 -r $runNrFile
	elif [ $1 == "imp4th_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp3_$3.root -o $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp4_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved4th_Red/$6 -r $runNrFile
  elif [ $1 == "wave_red" ]; then 
    if [ -f "$4/rawHGCROC_wPed_$3_calib_mod.txt" ]; then
      echo "overwriting original calib file with manually modified $4/rawHGCROC_wPed_$3_calib_mod.txt"
      ./DataPrep -d 1 -E -f -w $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_$3.root -i $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_$3.root -o $4/rawHGCROC_wave_red_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibWave/$6 -r $runNrFile -k $4/rawHGCROC_wPed_$3_calib_mod.txt     
    else 
      ./DataPrep -d 1 -E -f -w $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_$3.root -i $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_$3.root -o $4/rawHGCROC_wave_red_$3.root -O $PlotBaseDir/HGCROC_PlotsCalibWave_Red/$6 -r $runNrFile 
    fi

	fi
}

# running example:
# bash runCalibration_2024.sh fbockExt2 muoncalibA1 improvedWBC4th


dataDirRaw=""
dataDirOut=""

if [ $1 = "fbockTB" ]; then 
	dataDirRaw=/media/fbock/Lennard4TB/LongboardTesting/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/LongboardTesting/HGCROCData
	PlotBaseDir=/media/fbock/Lennard4TB/LongboardTesting/
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

if [ $2 = "pedestal" ]; then
  #pure pedestal runs
#   runs='158 161 165 166 170 173'
#   for runNr in $runs; do 
#     ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNr.root -f -o $dataDirOut/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/PlotsPedestal/Run$runNr -r $runNrFile
#   done
  #pedestal from cosmics
#   runs='160 162 163 164 167 171 172 175' # 176
  runs='176'
  for runNr in $runs; do 
    ./DataPrep -a -d 1 -p -i $dataDirRaw/rawHGCROC_$runNr.root -f -o $dataDirOut/rawHGCROC_wPed_$runNr.root -O $PlotBaseDir/PlotsPedestal/Run$runNr -r $runNrFile
  done
fi

if [ $2 = "muonWave" ]; then
  runs='160 162 163 164 167 171 172 175' # 176
	badChannelMap=""
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runNr $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi


if [ $2 == "calibMuon" ]; then
# 	runPed='158'
# 	runs='160'
# 	badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config1.txt"
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	done
# 	badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config2.txt"
# 	runPed='161'
# # 	runs='162 163'
#   runs='162' #OR
# # 	runs='163' #AND
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	done
# 	badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config3.txt"
# 	runPed='165'
# 	runs='164' #OR
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	done
#   badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config4.txt"
# 	runPed='166'
# 	runs='167' #AND
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	done
  badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config5.txt"
	runPed='170'
	runs='171 172' #OR
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
#   badChannelMap="../configs/LocalTesting/badChannelMap_HGCROC_Cosmics_LB_Config6.txt"
# 	runPed='173'
# 	runs='175' #OR
# # 	runs='176' #AND
# 	for runNr in $runs; do 
# 		MuonCalibHGCROC $3 $runPed $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
# 	done
fi

if [ $2 == "calibMuonORs" ]; then	
# 	runs='160 162 164 171 172 175' #OR
	runs='160 162 164 171 172 175' #OR
# 	 runs='175'
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runNr $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi

if [ $2 == "calibMuonANDs" ]; then	
	runs='163 167 176' #OR
	for runNr in $runs; do 
		MuonCalibHGCROC $3 $runNr $runNr $dataDirRaw $dataDirOut Run_$runNr $badChannelMap
	done
fi
