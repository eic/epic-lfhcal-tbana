#! /bin/bash

if [ $1 = "fbockTB" ]; then 
  dataDirIn=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
  dataDirOut=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
  
elif [ $1 = "egpott" ]; then
  dataDirIn=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot
  dataDirOut=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/calibrated
elif [ $1 = "yale" ]; then
  dataDirIn=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/rawroot_new
  dataDirOut=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/calibrated_new
fi

# Just the muons from each set
if [ $2 == "BaseCalibs" ]; then
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetC_1.root -A $dataDirOut/calib_FullSetC1.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetC_2.root -A $dataDirOut/calib_FullSetC2.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetD_1.root -A $dataDirOut/calib_FullSetD1.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetD_2.root -A $dataDirOut/calib_FullSetD2.root
#  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_Muon_FullSetE_1.root -A $dataDirOut/calib_FullSetE1.root
#  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_Muon_FullSetE_2.root -A $dataDirOut/calib_FullSetE2.root

elif [ $2 == "HVScan1" ]; then
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_42V.root -A $dataDirOut/calib_HVScan1_42V.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_42_5V.root -A $dataDirOut/calib_HVScan1_42_5V.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_43V.root -A $dataDirOut/calib_HVScan1_43V.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_43_5V.root -A $dataDirOut/calib_HVScan1_43_5V.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_44V.root -A $dataDirOut/calib_HVScan1_44V.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_44_5V.root -A $dataDirOut/calib_HVScan1_44_5V.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_45V.root -A $dataDirOut/calib_HVScan1_45V.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_45_5V.root -A $dataDirOut/calib_HVScan1_45_5V.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_HVScan_46V.root -A $dataDirOut/calib_HVScan1_46V.root


elif [ $2 == "FullSetC" ]; then
  #./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetC_1.root -A $dataDirOut/calib_FullSetC1.root
  #./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetC_2.root -A $dataDirOut/calib_FullSetC2.root

# Make sure you've already run applyCalibrations!
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_1GeV_FullSetC.root -A $dataDirOut/calib_pi-_1GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_2GeV_FullSetC.root -A $dataDirOut/calib_pi-_2GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_3GeV_FullSetC.root -A $dataDirOut/calib_pi-_3GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_4GeV_FullSetC.root -A $dataDirOut/calib_pi-_4GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_5GeV_FullSetC.root -A $dataDirOut/calib_pi-_5GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_6GeV_FullSetC.root -A $dataDirOut/calib_pi-_6GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_7GeV_FullSetC.root -A $dataDirOut/calib_pi-_7GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_8GeV_FullSetC.root -A $dataDirOut/calib_pi-_8GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_9GeV_FullSetC.root -A $dataDirOut/calib_pi-_9GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_10GeV_FullSetC.root -A $dataDirOut/calib_pi-_10GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_1GeV_FullSetC.root -A $dataDirOut/calib_Had+_1GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_2GeV_FullSetC.root -A $dataDirOut/calib_Had+_2GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_3GeV_FullSetC.root -A $dataDirOut/calib_Had+_3GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_4GeV_FullSetC.root -A $dataDirOut/calib_Had+_4GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_5GeV_FullSetC.root -A $dataDirOut/calib_Had+_5GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_6GeV_FullSetC.root -A $dataDirOut/calib_Had+_6GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_7GeV_FullSetC.root -A $dataDirOut/calib_Had+_7GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_8GeV_FullSetC.root -A $dataDirOut/calib_Had+_8GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_9GeV_FullSetC.root -A $dataDirOut/calib_Had+_9GeV_FullSetC.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_10GeV_FullSetC.root -A $dataDirOut/calib_Had+_10GeV_FullSetC.root

elif [ $2 == "FullSetD" ]; then
  #./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetD_1.root -A $dataDirOut/calib_FullSetD1.root
  #./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_Muon_FullSetD_2.root -A $dataDirOut/calib_FullSetD2.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_1GeV_FullSetD.root -A $dataDirOut/calib_pi-_1GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_2GeV_FullSetD.root -A $dataDirOut/calib_pi-_2GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_3GeV_FullSetD.root -A $dataDirOut/calib_pi-_3GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_4GeV_FullSetD.root -A $dataDirOut/calib_pi-_4GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_5GeV_FullSetD.root -A $dataDirOut/calib_pi-_5GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_6GeV_FullSetD.root -A $dataDirOut/calib_pi-_6GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_7GeV_FullSetD.root -A $dataDirOut/calib_pi-_7GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_8GeV_FullSetD.root -A $dataDirOut/calib_pi-_8GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_9GeV_FullSetD.root -A $dataDirOut/calib_pi-_9GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_pi-_10GeV_FullSetD.root -A $dataDirOut/calib_pi-_10GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_1GeV_FullSetD.root -A $dataDirOut/calib_Had+_1GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_2GeV_FullSetD.root -A $dataDirOut/calib_Had+_2GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_3GeV_FullSetD.root -A $dataDirOut/calib_Had+_3GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_4GeV_FullSetD.root -A $dataDirOut/calib_Had+_4GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_5GeV_FullSetD.root -A $dataDirOut/calib_Had+_5GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_6GeV_FullSetD.root -A $dataDirOut/calib_Had+_6GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_7GeV_FullSetD.root -A $dataDirOut/calib_Had+_7GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_8GeV_FullSetD.root -A $dataDirOut/calib_Had+_8GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_9GeV_FullSetD.root -A $dataDirOut/calib_Had+_9GeV_FullSetD.root
  ./DataPrep -a -i $dataDirIn/calibratedHGCROC_Run_Had+_10GeV_FullSetD.root -A $dataDirOut/calib_Had+_10GeV_FullSetD.root

fi



