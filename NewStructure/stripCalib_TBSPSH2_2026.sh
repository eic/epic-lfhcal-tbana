#! /bin/bash

if [ $1 = "fbockTB" ]; then 
  dataDirIn=/media/fbock/ALICE2-4TB/202605_SPSH2/HGCROCData
  dataDirOut=/media/fbock/ALICE2-4TB/202605_SPSH2/HGCROCData  
fi

# Just the muons from each set
if [ $2 == "FullSetB" ]; then
  echo "Stripping FullSet B"
  runs='Muon_FullSetB_1 Muon_FullSetB_2'
  for runNr in $runs; do
#     ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_ImpR_$runNr.root -A $dataDirOut/calib_ImpR_$runNr.root
#     ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp2R_$runNr.root -A $dataDirOut/calib_Imp2R_$runNr.root
#     ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp3R_$runNr.root -A $dataDirOut/calib_Imp3R_$runNr.root
#     ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_$runNr.root -A $dataDirOut/calib_Imp4R_$runNr.root
    cp $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_$runNr.root $dataDirOut/calib_Final_$runNr.root
    cp $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_$runNr\_calib.txt $dataDirOut/calib_Final_$runNr\_calib.txt
  done

  runNrSp='Muon_FullSetB_2'
#   ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_ImpR_3Int_$runNrSp.root -A $dataDirOut/calib_ImpR_3Int_$runNrSp.root
#   ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp2R_3Int_$runNrSp.root -A $dataDirOut/calib_Imp2R_3Int_$runNrSp.root
#   ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_ImpR_5Int_$runNrSp.root -A $dataDirOut/calib_ImpR_5Int_$runNrSp.root
#   ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp2R_5Int_$runNrSp.root -A $dataDirOut/calib_Imp2R_5Int_$runNrSp.root
  
# Just the muons from each set
elif [ $2 == "FullSetC" ]; then
  echo "Stripping FullSet C"
  runs='Muon_FullSetC_1 Muon_FullSetC_2 Muon_FullSetC_3'
  for runNr in $runs; do
#     ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_$runNr.root -A $dataDirOut/calib_Imp4R_$runNr.root
    cp $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_$runNr.root $dataDirOut/calib_Final_$runNr.root
    cp $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_$runNr\_calib.txt $dataDirOut/calib_Final_$runNr\_calib.txt
  done
elif [ $2 == "FullSetD" ]; then
  echo "Stripping FullSet D"
  runs='Muon_FullSetD_1 Muon_FullSetD_2'
  for runNr in $runs; do
#     ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_$runNr.root -A $dataDirOut/calib_Imp4R_$runNr.root
    cp $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_$runNr.root $dataDirOut/calib_Final_$runNr.root
    cp $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp5R_$runNr\_calib.txt $dataDirOut/calib_Final_$runNr\_calib.txt
  done
  
elif [ $2 == "HVScan" ]; then
  echo "Stripping HVScan"
  runs='194 195 196 197 198 199 200 201 202'
  for runNr in $runs; do
#     ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_ImpR_$runNr.root -A $dataDirOut/calib_ImpR_$runNr.root
#     ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp2R_$runNr.root -A $dataDirOut/calib_Imp2R_$runNr.root
    ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp3R_$runNr.root -A $dataDirOut/calib_Imp3R_$runNr.root
    ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_$runNr.root -A $dataDirOut/calib_Imp4R_$runNr.root
  done

elif [ $2 == "ParamScan" ]; then
  echo "Stripping ParamScan"
  runs='295 298 300 302 304 306 308 310 329 331 333 335 337 339 341 343 345 347 349 351 353 355 357 359 361 363 366 369'
  for runNr in $runs; do
    ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_ImpR_$runNr.root -A $dataDirOut/calib_ImpR_$runNr.root
    ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp2R_$runNr.root -A $dataDirOut/calib_Imp2R_$runNr.root
    ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp3R_$runNr.root -A $dataDirOut/calib_Imp3R_$runNr.root
    ./DataPrep -a -i $dataDirIn/rawHGCROC_wPedwMuon_wBC_Imp4R_$runNr.root -A $dataDirOut/calib_Imp4R_$runNr.root
  done


elif [ $2 == "FullSetC" ]; then
  echo "Stripping FullSet C"

elif [ $2 == "FullSetD" ]; then
  echo "Stripping FullSet C"

fi



