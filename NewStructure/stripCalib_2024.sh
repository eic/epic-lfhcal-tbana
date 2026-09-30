#! /bin/bash

if [ $1 = "fbockExt" ]; then 
  dataDirIn=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENData
  dataDirOut=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENData
fi  
  

if [ $2 == "BaseCalibsCAEN" ]; then

  # redone calibs
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanA1_45V.root -A $dataDirOut/calib_muonScanA1_45V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanA2_45V.root -A $dataDirOut/calib_muonScanA2_45V_V3.root
# 
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanB1_42V.root -A $dataDirOut/calib_muonScanB1_42V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanB2_42V.root -A $dataDirOut/calib_muonScanB2_42V_V3.root
# 
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanC2_43_5V.root  -A $dataDirOut/calib_muonScanC2_43_5V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanC1_43_5V.root  -A $dataDirOut/calib_muonScanC1_43_5V_V3.root
#   
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanD2_45V.root -A $dataDirOut/calib_muonScanD2_45V_V3.root
# 
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanH1_45V.root -A $dataDirOut/calib_muonScanH1_45V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanH2_45V.root -A $dataDirOut/calib_muonScanH2_45V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanH_45V.root -A $dataDirOut/calib_muonScanH_45V_V3.root
#   
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp6th_red_muonScanF_41V.root -A $dataDirOut/calib_muonScanF_41V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp6th_red_muonScanF1_41V.root -A $dataDirOut/calib_muonScanF1_41V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp6th_red_muonScanF2_41V.root -A $dataDirOut/calib_muonScanF2_41V_V3.root
# 
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp6th_red_muonScanE_40V.root -A $dataDirOut/calib_muonScanE_40V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp6th_red_muonScanE1_40V.root -A $dataDirOut/calib_muonScanE1_40V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp6th_red_muonScanE2_40V.root -A $dataDirOut/calib_muonScanE2_40V_V3.root
# 
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanG_46V.root -A $dataDirOut/calib_muonScanG_46V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanG1_46V.root -A $dataDirOut/calib_muonScanG1_46V_V3.root
#   ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_muonScanG2_46V.root -A $dataDirOut/calib_muonScanG2_46V_V3.root

 ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_305.root -A $dataDirOut/calib_HVScan_44V_V3.root
 ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_307.root -A $dataDirOut/calib_HVScan_43V_V3.root
 ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_309.root -A $dataDirOut/calib_HVScan_42V_V3.root
 ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp6th_red_312.root -A $dataDirOut/calib_HVScan_41V_V3.root
 ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp6th_red_316.root -A $dataDirOut/calib_HVScan_40V_V3.root

elif [ $2 == "CleanCalibsCAEN" ]; then
  mkdir $dataDirOut/tempCalibs
#   runs='muonScanD2_45V'
  runs='316'
#   runs='muonScanA1_45V muonScanA2_45V muonScanB1_42V muonScanB2_42V muonScanC1_43_5V muonScanC2_43_5V muonScanH1_45V muonScanH2_45V muonScanH_45V muonScanF_41V muonScanF1_41V muonScanF2_41V muonScanE_40V muonScanE1_40V muonScanE2_40V muonScanG_46V muonScanG1_46V muonScanG2_46V 305 307 309 312 316'
  for runNr in $runs; do
    ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp1st_red_$runNr.root -A $dataDirOut/tempCalibs/calib_ImpR_$runNr.root
    mv $dataDirOut/tempCalibs/calib_ImpR_$runNr.root $dataDirIn/rawPedAndMuonImp1st_red_$runNr.root
    mv $dataDirOut/tempCalibs/calib_ImpR_$runNr\_calib.txt $dataDirIn/rawPedAndMuonImp1st_red_$runNr\_calib.txt
    ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp2nd_red_$runNr.root -A $dataDirOut/tempCalibs/calib_Imp2R_$runNr.root
    mv $dataDirOut/tempCalibs/calib_Imp2R_$runNr.root $dataDirIn/rawPedAndMuonImp2nd_red_$runNr.root
    mv $dataDirOut/tempCalibs/calib_Imp2R_$runNr\_calib.txt $dataDirIn/rawPedAndMuonImp2nd_red_$runNr\_calib.txt
    ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp3rd_red_$runNr.root -A $dataDirOut/tempCalibs/calib_Imp3R_$runNr.root
    mv $dataDirOut/tempCalibs/calib_Imp3R_$runNr.root $dataDirIn/rawPedAndMuonImp3rd_red_$runNr.root
    mv $dataDirOut/tempCalibs/calib_Imp3R_$runNr\_calib.txt $dataDirIn/rawPedAndMuonImp3rd_red_$runNr\_calib.txt
#     ./DataPrep -a -i $dataDirIn/rawPedAndMuonImp4th_red_$runNr.root -A $dataDirOut/tempCalibs/calib_Imp4R_$runNr.root
#     mv $dataDirOut/tempCalibs/calib_Imp4R_$runNr.root $dataDirIn/rawPedAndMuonImp4th_red_$runNr.root
#     mv $dataDirOut/tempCalibs/calib_Imp4R_$runNr\_calib.txt $dataDirIn/rawPedAndMuonImp4th_red_$runNr\_calib.txt
  done

fi

 
 
