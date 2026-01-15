#! /bin/bash

function Calib()
{
	runNrFile='../configs/TB2025/DataTakingDB_202511_HGCROC.csv'
	echo "===================================================================="
	echo "option:  $1"
	echo "calib File:  $2"
	echo "raw data path:  $3"
	echo "out data path:  $4"
	echo "additional Name/runNr:  $5"
	echo "Plots Directory-Base: $6"
	echo "Plots additional name: $7"
	echo "external bad channel:" $8
	echo "===================================================================="
	if [ $1 == "transfer" ]; then
		time ./DataPrep -d 1 -e -a -f -P $2 -i $3/rawHGCROC_$5.root  -o $3/rawHGCROCWithCalib_$5.root -O $6/HGCROC_PlotsFullCalibTransferBC/Run_$5 -r $runNrFile
	elif [ $1 == "trigg" ]; then
		time ./DataPrep -f -d 1 -T $2 -i $3/rawHGCROC_$5.root -o $3/rawHGCROCWithLocTrigg_$5.root 
	elif [ $1 == "triggMuon" ]; then
		time ./DataPrep -f -d 1 -u -T $2 -i $3/rawHGCROC_miptrigg_wPedwMuon_wBC_$5.root -o $3/rawHGCROCWithLocTrigg_$5.root 
	elif [ $1 == "calibNoTrigg" ]; then
		time ./DataPrep -t -e -f -d 1 -a -C $2 -i $3/rawHGCROCWithLocTrigg_$5.root -o $4/calibratedHGCROC_Run_$5.root -O $6/$7$5 -r $runNrFile -B $8
	elif [ $1 == "full" ]; then
		time ./DataPrep -e -f -d 1 -a -C $2 -i $3/rawHGCROC_$5.root -o $4/calibratedHGCROC_Run_$5.root -O $6/$7$5 -r $runNrFile -B $8
	fi
}

dataDirCal=""
if [ $1 = "fbockTB" ]; then 
	dataDirCal=/media/fbock/Lennard4TB/202511_PST09/HGCROCDataAlt
	dataDirIn=/media/fbock/Lennard4TB/202511_PST09/HGCROCDataAlt
	dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCDataAlt
	PlotBaseDir=/media/fbock/Lennard4TB/202511_PST09/AltPlots
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

# apply calibration
if [ $2 == "FullScanA" ]; then
	calibFile1=$dataDirCal/calib_FullSetA1.root
	calibFile2=$dataDirCal/calib_FullSetA2.root
	echo "running calibrate for 44V runs, campaign A"
  badChannelMap="../configs/TB2025/badChannel_HGCROC_PSTB2025_default.txt"
	
	#muon runs
# 	Calib $3 $calibFile1 $dataDirIn $dataDirOut FullSetA_1 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap
# 	Calib $3 $calibFile2 $dataDirIn $dataDirOut FullSetA_2 $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap

  badChannelMap="../configs/TB2025/badChannel_HGCROC_PSTB2025_layer0.txt"
	# electron runs
# 	runs='170' 
# 	runs='165 166 167 168 169 170' 
# 	runs='191 192 193 194 195' 
# 	for runNr in $runs; do 
# 		Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap
# 	done;

	# positron runs
# runs='171 172 173 174 175 196 200 199 198 197'
#   runs='175'
# 	for runNr in $runs; do 
# 		Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap
# 	done;
# 
# 	#hadron runs
# #   runs='176 177 178 179 180 181 182 183 184 184 185 186 187 188'
#   runs='177 184'
  runs='177'
	for runNr in $runs; do 
		Calib $3 $calibFile1 $dataDirIn $dataDirOut $runNr $PlotBaseDir HGCROC_PlotsCalibrated/Run_ $badChannelMap
	done;
# elif [ $2 == "MuonHVScan" ]; then
# 	Calib $3 $dataDirCal/calib_305_44V.root $dataDirIn $dataDirOut 305 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_
# 	Calib $3 $dataDirCal/calib_307_43V.root $dataDirIn $dataDirOut 307 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_
# 	Calib $3 $dataDirCal/calib_309_42V.root $dataDirIn $dataDirOut 309 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_
# 	Calib $3 $dataDirCal/calib_312_41V.root $dataDirIn $dataDirOut 312 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_
# 	Calib $3 $dataDirCal/calib_316_40V.root $dataDirIn $dataDirOut 316 $PlotBaseDir CAEN_PlotsMuonCalibrated_2024/Run_
fi

