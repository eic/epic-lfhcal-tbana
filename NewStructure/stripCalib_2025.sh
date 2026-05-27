#! /bin/bash

if [ $1 == "fbockTB" ]; then 
	dataDirIn=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCData
	
elif [ $1 == "egpott" ]; then
		dataDirIn=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot
		dataDirOut=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/calibrated

elif [ $1 == "yale" ]; then	
	dataDirIn=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202511_PST09/HGCROCData
	dataDirOut=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202511_PST09/calibrated
fi

  
if [ $2 == "BaseCalibs" ]; then
	runs='028 029 030 031 032 033'
	for runNr in $runs; do
		./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_$runNr.root -A $dataDirOut/calib_$runNr.root
	done;
# 	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp3R_FullSetA_1.root -A $dataDirOut/calib_FullSetA1.root
# 	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp3R_FullSetA_2.root -A $dataDirOut/calib_FullSetA2.root
# 	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp3R_FullSetB_1.root -A $dataDirOut/calib_FullSetB1.root
#	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_FullSetB_2.root -A $dataDirOut/calib_FullSetB2.root

elif [ $2 == "FullSetA" ]; then
# after mip calibration
#	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetA_1.root -A $dataDirOut/calib_FullSetA1.root
#	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetA_2.root -A $dataDirOut/calib_FullSetA2.root

# comment out the muon runs above and run these after running applyCalibration
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_e-_1GeV_FullSetA.root -A $dataDirOut/calib_e-_1GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_e-_2GeV_FullSetA.root -A $dataDirOut/calib_e-_2GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_e-_3GeV_FullSetA.root -A $dataDirOut/calib_e-_3GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_e-_4GeV_FullSetA.root -A $dataDirOut/calib_e-_4GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_e-_5GeV_FullSetA.root -A $dataDirOut/calib_e-_5GeV_FullSetA.root

	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_e+_1GeV_FullSetA.root -A $dataDirOut/calib_e+_1GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_e+_2GeV_FullSetA.root -A $dataDirOut/calib_e+_2GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_e+_3GeV_FullSetA.root -A $dataDirOut/calib_e+_3GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_e+_4GeV_FullSetA.root -A $dataDirOut/calib_e+_4GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_e+_5GeV_FullSetA.root -A $dataDirOut/calib_e+_5GeV_FullSetA.root

	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h-_3GeV_FullSetA.root -A $dataDirOut/calib_h-_3GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h-_5GeV_FullSetA.root -A $dataDirOut/calib_h-_5GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h-_8GeV_FullSetA.root -A $dataDirOut/calib_h-_8GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h-_10GeV_FullSetA.root -A $dataDirOut/calib_h-_10GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h-_12GeV_FullSetA.root -A $dataDirOut/calib_h-_12GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h-_15GeV_FullSetA.root -A $dataDirOut/calib_h-_15GeV_FullSetA.root

	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h+_3GeV_FullSetA.root -A $dataDirOut/calib_h+_3GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h+_5GeV_FullSetA.root -A $dataDirOut/calib_h+_5GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h+_8GeV_FullSetA.root -A $dataDirOut/calib_h+_8GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h+_10GeV_FullSetA.root -A $dataDirOut/calib_h+_10GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h+_12GeV_FullSetA.root -A $dataDirOut/calib_h+_12GeV_FullSetA.root
	./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_h+_15GeV_FullSetA.root -A $dataDirOut/calib_h+_15GeV_FullSetA.root

# 	# 192K events
# 	echo "running calibrate for 45V runs, campaing A1"
# 	runs='251 252 254 257 258 ' 
# 	for runNr in $runs; do 
#     ./DataPrep -a -i $dataDirInE/rawWithLocTrigg_$runNr.root -A $dataDirOut/calibUsed_$runNr.root
# 	done;
# 	runs='261 264 265 269 270 272 274 275 ' 
# 	for runNr in $runs; do 
#     ./DataPrep -a -i $dataDirInH/rawWithLocTrigg_$runNr.root -A $dataDirOut/calibUsed_$runNr.root
# 	done;
# 	
# 	./DataPrep -a -i $dataDirInM/rawWithLocTrigg_muonScanA1_45V.root -A $dataDirOut/calibUsed_muonScanA1_45V.root
# 	./DataPrep -a -i $dataDirInM/rawWithLocTrigg_muonScanA2_45V.root -A $dataDirOut/calibUsed_muonScanA2_45V.root
fi

 
 
