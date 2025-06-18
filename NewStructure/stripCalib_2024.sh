#! /bin/bash

if [ $1 = "fbockExt2" ]; then 
	dataDirInM=/media/fbock/T7/202408_PST09/CAENdata/MuonRuns
	dataDirInE=/media/fbock/T7/202408_PST09/CAENdata/ElectronRuns
	dataDirInH=/media/fbock/T7/202408_PST09/CAENdata/HadronRuns
	dataDirOut=/media/fbock/T7/202408_PST09/CAENdata
fi  
  

if [ $2 == "BaseCalibs" ]; then

# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp3rd_muonScanA1_45V.root -A $dataDirOut/calib_muonScanA1_45V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp3rd_muonScanA2_45V.root -A $dataDirOut/calib_muonScanA2_45V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp4th_muonScanD2_45V.root -A $dataDirOut/calib_muonScanD2_45V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp4th_muonScanH1_45V.root -A $dataDirOut/calib_muonScanH1_45V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp4th_muonScanH2_45V.root -A $dataDirOut/calib_muonScanH2_45V.root

# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp4th_305.root -A $dataDirOut/calib_305_44V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp4th_307.root -A $dataDirOut/calib_307_43V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp4th_309.root -A $dataDirOut/calib_309_42V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp4th_312.root -A $dataDirOut/calib_312_41V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp6th_316.root -A $dataDirOut/calib_316_40V.root
# 
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp2nd_muonScanB1_42V.root -A $dataDirOut/calib_muonScanB1_42V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp2nd_muonScanB2_42V.root -A $dataDirOut/calib_muonScanB2_42V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp6th_muonScanE1_40V.root -A $dataDirOut/calib_muonScanE1_40V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp6th_muonScanE2_40V.root -A $dataDirOut/calib_muonScanE2_40V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp6th_muonScanF1_41V.root -A $dataDirOut/calib_muonScanF1_41V.root
# 	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp6th_muonScanF2_41V.root -A $dataDirOut/calib_muonScanF2_41V.root
	build/DataPrep -a -i $dataDirInM/rawPedAndMuonWBCImp4th_muonScanG_46V.root -A $dataDirOut/calib_muonScanG_46V.root
elif [ $2 == "ScanA" ]; then
	# 192K events
	echo "running calibrate for 45V runs, campaing A1"
	runs='251 252 254 257 258 ' 
	for runNr in $runs; do 
    build/DataPrep -a -i $dataDirInE/rawWithLocTrigg_$runNr.root -A $dataDirOut/calibUsed_$runNr.root
	done;
	runs='261 264 265 269 270 272 274 275 ' 
	for runNr in $runs; do 
    build/DataPrep -a -i $dataDirInH/rawWithLocTrigg_$runNr.root -A $dataDirOut/calibUsed_$runNr.root
	done;
	
	build/DataPrep -a -i $dataDirInM/rawWithLocTrigg_muonScanA1_45V.root -A $dataDirOut/calibUsed_muonScanA1_45V.root
	build/DataPrep -a -i $dataDirInM/rawWithLocTrigg_muonScanA2_45V.root -A $dataDirOut/calibUsed_muonScanA2_45V.root
fi

 
 
