#! /bin/bash

if [ $1 = "fbockTB" ]; then 
	dataDirIn=/media/fbock/Lennard4TB/202511_PST09/HGCROCDataAlt
	dataDirOut=/media/fbock/Lennard4TB/202511_PST09/HGCROCDataAlt
fi  
  

if [ $2 == "BaseCalibs" ]; then
# 	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp3R_FullSetA_1.root -A $dataDirOut/calib_FullSetA1.root
	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp3R_FullSetA_2.root -A $dataDirOut/calib_FullSetA2.root
# 	./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp3R_FullSetB_1.root -A $dataDirOut/calib_FullSetB1.root
# elif [ $2 == "ScanA" ]; then
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

 
 
