#! /bin/bash
runNrFile=""
PlotBaseDir=""

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
	if [ $1 == "BCTOA" ]; then 
		echo "badchannelMap:" $7
		echo "ToAPhase-Offset:" $8
	fi
	printf -v runNrPed "%03d" "$2"
	runNrMuon=$3
	
	echo "=================================================================================="
	if [ $1 == "BC" ]; then 
    if [ -f "$4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt" ]; then
      echo "overwriting original calib file with manually modified $4/rawHGCROC_wPed_$3_calib_mod.txt"
      ./DataPrep -d 1 -e -f -P $4/rawHGCROC_wPed_$runNrPed.root -i $4/rawHGCROC_$runNrMuon.root -o $4/rawHGCROC_wPed_wBC_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibTransfer/$6 -r $runNrFile -k $4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt -B $7     
    else 
      ./DataPrep -d 1 -e -f -P $4/rawHGCROC_wPed_$runNrPed.root -i $4/rawHGCROC_$runNrMuon.root -o $4/rawHGCROC_wPed_wBC_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibTransfer/$6 -r $runNrFile -B $7 
    fi
	elif [ $1 == "BCTOA" ]; then 
    if [ -f "$4/rawHGCROC_wPed_$3_calib_mod.txt" ]; then
      echo "overwriting original calib file with manually modified $4/rawHGCROC_wPed_$3_calib_mod.txt"
      ./DataPrep -d 1 -e -f -P $4/rawHGCROC_wPed_$runNrPed.root -i $4/rawHGCROC_$runNrMuon.root -o $4/rawHGCROC_wPed_wBC_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibTransfer/$6 -r $runNrFile -k $4/rawHGCROC_wPed_$runNrMuon_calib_mod.txt -B $7 -G $8
    else 
        ./DataPrep -d 2 -e -f -P $4/rawHGCROC_wPed_$runNrPed.root -i $4/rawHGCROC_$runNrMuon.root -o $4/rawHGCROC_wPed_wBC_$runNrMuon.root  -O $PlotBaseDir/HGCROC_PlotsCalibTransferWToA/$6 -r $runNrFile -B $7  -G $8  
    fi
	elif [ $1 == "default" ]; then 
		time ./DataPrep -f -d 1 -e  -s -i $4/rawHGCROC_wPed_wBC_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuon/$6 -r $runNrFile
	elif [ $1 == "imp1st" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved/$6 -r $runNrFile
	elif [ $1 == "imp2nd" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp2_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved2nd/$6 -r $runNrFile
	elif [ $1 == "imp3rd" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp2_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp3_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved3rd/$6 -r $runNrFile
	elif [ $1 == "imp4th" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp3_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp4_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved4th/$6 -r $runNrFile
	elif [ $1 == "saveNewMuon" ]; then 
		time ./DataPrep -f -d 1 -X -i $4/rawHGCROC_wPedwMuon_wBC_$runNrMuon.root -o $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_$runNrMuon.root 
	elif [ $1 == "imp1st_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_mipTrigg_wPedwMuon_wBC_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_ImpR_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved_Red/$6 -r $runNrFile
	elif [ $1 == "imp2nd_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_ImpR_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp2R_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved2nd_Red/$6 -r $runNrFile
	elif [ $1 == "imp3rd_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp2R_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp3R_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved3rd_Red/$6 -r $runNrFile
	elif [ $1 == "imp4th_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp3R_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp4R_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved4th_Red/$6 -r $runNrFile
	elif [ $1 == "imp5th_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp4R_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp5R_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved5th_Red/$6 -r $runNrFile
	elif [ $1 == "imp6th_red" ]; then 
		time ./DataPrep -f -d 1  -S -i $4/rawHGCROC_wPedwMuon_wBC_Imp5R_$runNrMuon.root -o $4/rawHGCROC_wPedwMuon_wBC_Imp6R_$runNrMuon.root -O $PlotBaseDir/HGCROC_PlotsCalibMuonImproved6th_Red/$6 -r $runNrFile	
	fi
}

function Pedestal()
{
  echo "option: $1" 
  echo "run Nr Pedestal: $2"
	echo "dataRawDir: $3"
	echo "dataOutDir: $4"
	echo "OutNameRun: $5" 
	echo "plot-ending: $6" 
	
  ./DataPrep -a -d 1 -p -i $3/rawHGCROC_$2.root -f -o $4/rawHGCROC_wPed_$2.root -O $PlotBaseDir/PlotsPedestal/$5 -r $runNrFile -F $6
}

function WaveformHGCROC()
{
	echo "=================================================================================="
	echo "option $1"
	echo "run Nr: $2"
	echo "dataDir: $3"
	echo "OutNameRun:" $4
  if [ $1 == "wavepdf" ]; then 
    ./DataPrep -d 1 -E 2 -f -w -i $3/calibratedHGCROC_Run_$2.root -o $3/calibratedHGCROC_Wave_Run_$2.root -O $PlotBaseDir/HGCROC_PlotsCalibWave/$4 -r $runNrFile 
  elif [ $1 == "wavepng" ]; then 
    ./DataPrep -d 1 -E 2 -f -w -i $3/calibratedHGCROC_Run_$2.root -o $3/calibratedHGCROC_Wave_Run_$2.root -O $PlotBaseDir/HGCROC_PlotsCalibWave/$4 -r $runNrFile -F png
	fi
}


function Calib()
{
	echo "===================================================================="
	echo "option:  $1"
	echo "calib File:  $2"
	echo "raw data path:  $3"
	echo "out data path:  $4"
	echo "additional Name/runNr:  $5"
	echo "Plots Directory-Base: $6"
	echo "Plots additional name: $7"
	echo "external bad channel:" $8
	echo "external TOA phase calib:" $9
	echo "===================================================================="
	if [ $1 == "transfer" ]; then
		time ./DataPrep -d 1 -e -a -f -P $2 -i $3/rawHGCROC_$5.root  -o $3/rawHGCROCWithCalib_$5.root -O $6/HGCROC_PlotsFullCalibTransferBC/Run_$5 -r $runNrFile
	elif [ $1 == "trigg" ]; then
		time ./DataPrep -f -d 1 -T $2 -i $3/rawHGCROC_$5.root -o $3/rawHGCROCWithLocTrigg_$5.root 
	elif [ $1 == "triggMuon" ]; then
		time ./DataPrep -f -d 1 -u -T $2 -i $3/rawHGCROC_miptrigg_wPedwMuon_wBC_$5.root -o $3/rawHGCROCWithLocTrigg_$5.root 
	elif [ $1 == "calibNoTrigg" ]; then
		time ./DataPrep -t -e -f -d 1 -a -C $2 -i $3/rawHGCROCWithLocTrigg_$5.root -o $4/calibratedHGCROC_Run_$5.root -O $6/$7$5 -r $runNrFile -B $8 -G $9 #-F png
	elif [ $1 == "calibNoTriggNLP" ]; then
    echo "no layer plotting" 
		time ./DataPrep -t -f -d 1 -a -C $2 -i $3/rawHGCROCWithLocTrigg_$5.root -o $4/calibratedHGCROC_Run_$5.root -O $6/$7$5 -r $runNrFile -B $8 -G $9 -F png
	elif [ $1 == "calibNoTriggZC" ]; then
		time ./DataPrep -t -e -f -d 1 -a -C $2 -i $3/rawHGCROCWithLocTrigg_$5.root -o $4/calibratedNoCutOffHGCROC_Run_$5.root -O $6NoCutOff$7$5 -r $runNrFile -B $8 -G $9 -c -10.
	elif [ $1 == "calibNoTriggZCMuon" ]; then
		time ./DataPrep -t -e -f -d 1 -a -C $2 -i $3/rawHGCROC_wPedwMuon_wBC_$5.root -o $4/calibratedNoCutOffHGCROC_Run_$5.root -O $6NoCutOff$7$5 -r $runNrFile -B $8 -G $9 -c -10.
	elif [ $1 == "full" ]; then
		time ./DataPrep -e -f -d 1 -a -C $2 -i $3/rawHGCROC_$5.root -o $4/calibratedHGCROC_Run_$5.root -O $6/$7$5 -r $runNrFile -B $8 -G $9
	fi
}
