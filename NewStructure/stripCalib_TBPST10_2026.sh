#! /bin/bash

if [ $1 = "fbockTB" ]; then 
	dataDirIn=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
	dataDirOut=/media/fbock/Lennard4TB/202604_PST10/HGCROCData
	
	elif [ $1 = "egpott" ]; then
		dataDirIn=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/rawroot
		dataDirOut=/Users/egpott/rhig/lfhcal/data/TB2025_HVscan1/calibrated
fi

if [ $2 == "BaseCalibs" ]; then
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_Muon_FullSetE_1.root -A $dataDirOut/calib_FullSetE1.root
  ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_Muon_FullSetE_2.root -A $dataDirOut/calib_FullSetE2.root

fi

 
 
