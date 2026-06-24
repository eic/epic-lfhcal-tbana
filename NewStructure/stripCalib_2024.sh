#! /bin/bash

if [ $1 = "fbockExt2" ]; then 
  dataDirIn=/media/fbock/T7/202408_PST09/CAENdata/MuonRuns
  dataDirOut=/media/fbock/T7/202408_PST09/CAENdata
elif [ $1 = "fbockExtCAEN" ]; then 
  dataDirIn=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENData
  dataDirOut=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENData
fi  
  

if [ $2 == "BaseCalibsCAEN" ]; then

# redone calibs
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp3rd_red_muonScanA1_45V.root -A $dataDirOut/calib_muonScanA1_45V_V2.root
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp3rd_red_muonScanA2_45V.root -A $dataDirOut/calib_muonScanA2_45V_V2.root
  ./DataPrep -a -i $dataDirIn/rawCalibReExtractedLGHG_269.root -A $dataDirOut/calib_muonScanA1_45V_V2hadCorr.root

#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanB1_42V.root -A $dataDirOut/calib_muonScanB1_42V_V2.root
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp3rd_red_muonScanB2_42V.root -A $dataDirOut/calib_muonScanB2_42V_V2.root

  ./DataPrep -a -i $dataDirIn/rawCalibReExtractedLGHG_357.root -A $dataDirOut/calib_muonScanB1_42V_V2hadCorr.root

#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp3rd_red_muonScanC2_43_5V.root  -A $dataDirOut/calib_muonScanC2_43_5V_V2.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp3rd_red_muonScanC1_43_5V.root  -A $dataDirOut/calib_muonScanC1_43_5V_V2.root
  ./DataPrep -a -i $dataDirIn/rawCalibReExtractedLGHG_394.root -A $dataDirOut/calib_muonScanC2_43_5V_V2hadCorr.root
  
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp5th_red_muonScanF_41V.root -A $dataDirOut/calib_muonScanF_41V_V2.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp5th_red_muonScanF1_41V.root -A $dataDirOut/calib_muonScanF1_41V_V2.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp5th_red_muonScanF2_41V.root -A $dataDirOut/calib_muonScanF2_41V_V2.root

  ./DataPrep -a -i $dataDirIn/rawCalibReExtractedLGHG_504.root -A $dataDirOut/calib_muonScanF_41V_V2eCorr.root

#old calibs
 #  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp4th_muonScanD2_45V.root -A $dataDirOut/calib_muonScanD2_45V.root
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp4th_muonScanH1_45V.root -A $dataDirOut/calib_muonScanH1_45V.root
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp4th_muonScanH2_45V.root -A $dataDirOut/calib_muonScanH2_45V.root

#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp4th_305.root -A $dataDirOut/calib_305_44V.root
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp4th_307.root -A $dataDirOut/calib_307_43V.root
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp4th_309.root -A $dataDirOut/calib_309_42V.root
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp4th_312.root -A $dataDirOut/calib_312_41V.root
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp6th_316.root -A $dataDirOut/calib_316_40V.root
# 
# 
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp6th_muonScanE1_40V.root -A $dataDirOut/calib_muonScanE1_40V.root
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp6th_muonScanE2_40V.root -A $dataDirOut/calib_muonScanE2_40V.root
#  ./DataPrep -a -i $dataDirIn/rawPedAndMuonWBCImp4th_muonScanG_46V.root -A $dataDirOut/calib_muonScanG_46V.root
elif [ $2 == "ScanA" ]; then
  # 192K events
  echo "running calibrate for 45V runs, campaing A1"  
  ./DataPrep -a -i $dataDirIn/rawWithLocTrigg_muonScanA1_45V.root -A $dataDirOut/calibUsed_muonScanA1_45V.root
  ./DataPrep -a -i $dataDirIn/rawWithLocTrigg_muonScanA2_45V.root -A $dataDirOut/calibUsed_muonScanA2_45V.root
fi

 
 
