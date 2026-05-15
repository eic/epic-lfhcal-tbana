#! /bin/bash

if [ $1 = "fbockTB" ]; then 
	dataDirIn=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
	
elif [ $1 = "egpott" ]; then
	dataDirIn=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot
	dataDirOut=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/calibrated
elif [ $1 = "yale" ]; then
	dataDirIn=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/rawroot
	dataDirOut=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/calibrated
fi

if [ $2 == "BaseCalibs" ]; then
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetC_1.root -A $dataDirOut/calib_FullSetC1.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetC_2.root -A $dataDirOut/calib_FullSetC2.root
	#./DataPrep -a -i $dataDirIn/rawHGCROC
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetD_1.root -A $dataDirOut/calib_FullSetD1.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetD_2.root -A $dataDirOut/calib_FullSetD2.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_42V.root -A $dataDirOut/calib_HVScan1_42V.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_42_5V.root -A $dataDirOut/calib_HVScan1_42_5V.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_43V.root -A $dataDirOut/calib_HVScan1_43V.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_43_5V.root -A $dataDirOut/calib_HVScan1_43_5V.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_44V.root -A $dataDirOut/calib_HVScan1_44V.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_44_5V.root -A $dataDirOut/calib_HVScan1_44_5V.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_45V.root -A $dataDirOut/calib_HVScan1_45V.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_45_5V.root -A $dataDirOut/calib_HVScan1_45_5V.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_46V.root -A $dataDirOut/calib_HVScan1_46V.root

#  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_Muon_FullSetE_1.root -A $dataDirOut/calib_FullSetE1.root
#  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_Muon_FullSetE_2.root -A $dataDirOut/calib_FullSetE2.root

fi

 
 
