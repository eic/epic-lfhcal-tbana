#ComparisonHGCROC SPS

runList=../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv
folder=../Trending
cfolder=configsComparison
#########################################################################################################################
#integrations ranges -t (-e extended plotting, -E expanded file list)
#########################################################################################################################
BaseInt=IntegrationVariation
# ./CompareCalib -d 0 -e 2 -H -f -t -I $cfolder/comparisonIntegrations.txt -o $folder/$BaseInt.root -O $folder/$BaseInt -r $runList

#########################################################################################################################
#iterations -T (-e extended plotting, -E expanded file list)
#########################################################################################################################
BaseIte=MuonIterations
# Muon set B_1
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetB_1.txt -o $folder/$BaseIte\_B1.root -O $folder/$BaseIte\_B1 -r $runList
# # Muon set B_2
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetB_2.txt -o $folder/$BaseIte\_B2.root -O $folder/$BaseIte\_B2 -r $runList
# Muon set C_1
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetC_1.txt -o $folder/$BaseIte\_C1.root -O $folder/$BaseIte\_C1 -r $runList
# Muon set C_2
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetC_2.txt -o $folder/$BaseIte\_C2.root -O $folder/$BaseIte\_C2 -r $runList
# Muon set C_3
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetC_3.txt -o $folder/$BaseIte\_C3.root -O $folder/$BaseIte\_C3 -r $runList
# # Muon set D_1
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetD_1.txt -o $folder/$BaseIte\_D1.root -O $folder/$BaseIte\_D1 -r $runList
# # Muon set D_2
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetD_2.txt -o $folder/$BaseIte\_D2.root -O $folder/$BaseIte\_D2 -r $runList
# # Muon set E_1
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetE_1.txt -o $folder/$BaseIte\_E1.root -O $folder/$BaseIte\_E1 -r $runList
# # Muon set E_2
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetE_2.txt -o $folder/$BaseIte\_E2.root -O $folder/$BaseIte\_E2 -r $runList
# # Muon set E_3
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetE_3.txt -o $folder/$BaseIte\_E3.root -O $folder/$BaseIte\_E3 -r $runList
# # Muon set F_1
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetF_1.txt -o $folder/$BaseIte\_F1.root -O $folder/$BaseIte\_F1 -r $runList
# # Muon set F_2
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetF_2.txt -o $folder/$BaseIte\_F2.root -O $folder/$BaseIte\_F2 -r $runList
# # Muon set G_1
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetG_1.txt -o $folder/$BaseIte\_G1.root -O $folder/$BaseIte\_G1 -r $runList
# # Muon set G_2
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_FullSetG_2.txt -o $folder/$BaseIte\_G2.root -O $folder/$BaseIte\_G2 -r $runList
# HV Scan 42 V
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_HVScan_42V.txt -o $folder/$BaseIte\_HVScan_42V.root -O $folder/$BaseIte\_HVScan_42V -r $runList
# HV Scan 42.5 V
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_HVScan_42_5V.txt -o $folder/$BaseIte\_HVScan_42_5V.root -O $folder/$BaseIte\_HVScan_42_5V -r $runList
# HV Scan 43 V
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_HVScan_43V.txt -o $folder/$BaseIte\_HVScan_43V.root -O $folder/$BaseIte\_HVScan_43V -r $runList
# HV Scan 43.5 V
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_HVScan_43_5V.txt -o $folder/$BaseIte\_HVScan_43_5V.root -O $folder/$BaseIte\_HVScan_43_5V -r $runList
# HV Scan 44 V
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_HVScan_44V.txt -o $folder/$BaseIte\_HVScan_44V.root -O $folder/$BaseIte\_HVScan_44V -r $runList
# HV Scan 44.5 V
./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_HVScan_44_5V.txt -o $folder/$BaseIte\_HVScan_44_5V.root -O $folder/$BaseIte\_HVScan_44_5V -r $runList
# HV Scan 45 V
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_HVScan_45V.txt -o $folder/$BaseIte\_HVScan_45V.root -O $folder/$BaseIte\_HVScan_45V -r $runList
# HV Scan 45.5 V
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_HVScan_45_5V.txt -o $folder/$BaseIte\_HVScan_45_5V.root -O $folder/$BaseIte\_HVScan_45_5V -r $runList
# HV Scan 45 V
# ./CompareCalib -d 0 -e 2 -H -f -T -I $cfolder/comparisonInterations_HVScan_46V.txt -o $folder/$BaseIte\_HVScan_46V.root -O $folder/$BaseIte\_HVScan_46V -r $runList


#########################################################################################################################
# Comparison same Campaign
#########################################################################################################################
BaseSet=MuonsSet
# #muon calibs final Ite Set B
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonFinal_FullSetB.txt -o $folder/$BaseSet\_B_Final.root -O $folder/$BaseSet\_B_Final -r $runList
#muon calibs final Ite Set C
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonFinal_FullSetC.txt -o $folder/$BaseSet\_C_Final.root -O $folder/$BaseSet\_C_Final -r $runList
#muon calibs final Ite Set D
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonFinal_FullSetD.txt -o $folder/$BaseSet\_D_Final.root -O $folder/$BaseSet\_D_Final -r $runList
# # #muon calibs final Ite Set E
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonFinal_FullSetE.txt -o $folder/$BaseSet\_E_Final.root -O $folder/$BaseSet\_E_Final -r $runList
# # #muon calibs final Ite Set F
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonFinal_FullSetF.txt -o $folder/$BaseSet\_F_Final.root -O $folder/$BaseSet\_F_Final -r $runList
# # #muon calibs final Ite Set G
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonFinal_FullSetG.txt -o $folder/$BaseSet\_G_Final.root -O $folder/$BaseSet\_G_Final -r $runList

#########################################################################################################################
# Comparison same Vov (might have different gain settings)
#########################################################################################################################
# muon calibs 43V comparison
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonFinal_43V.txt -o $folder/Muons43V.root -O $folder/Muons43V -r $runList
# muon calibs 44V comparison
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonFinal_44V.txt -o $folder/Muons43V.root -O $folder/Muons44V -r $runList
# muon calibs 45V comparison
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonFinal_45V.txt -o $folder/Muons43V.root -O $folder/Muons45V -r $runList
# 
#########################################################################################################################
# Comparison vs Voltage
#########################################################################################################################
# #muon calibs final Ite HV Scan
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonFinal_HVScan.txt -o $folder/MuonsHVScan_vs_Run.root -O $folder/MuonsHVScan_vs_Run -r $runList
# ./CompareCalib -d 0 -e 2 -H -f -V -I $cfolder/comparisonFinal_HVScan.txt -o $folder/MuonsHVScan_vs_Voltage.root -O $folder/MuonsHVScan_vs_Voltage -r $runList
# ./CompareCalib -d 0 -e 2 -H -f -V -I $cfolder/comparison1stIte_HVScan.txt -o $folder/MuonsHVScan_vs_Voltage.root -O $folder/MuonsHVScan_vs_Voltage -r $runList

#########################################################################################################################
# Parameter scan
#########################################################################################################################
# #muon calibs diff settings param scan
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonWaveformMips_All.txt -o $folder/MuonsDiffSettings.root -O $folder/MuonsDiffSettings -r $runList
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonWaveformMips_CCVariation.txt -o $folder/MuonsDiffSettings_CCVar.root -O $folder/MuonsDiffSettings_CCVar -r $runList
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonWaveformMips_CFVariation.txt -o $folder/MuonsDiffSettings_CFVar.root -O $folder/MuonsDiffSettings_CFVar -r $runList
# ./CompareCalib -d 0 -e 2 -H -f -R -I $cfolder/comparisonWaveformMips_RFVariations.txt -o $folder/MuonsDiffSettings_RFVar.root -O $folder/MuonsDiffSettings_RFVar -r $runList

# all runs transfer
#./CompareCalib -d 0 -E 3 -e 2 -f -R -I runConfigs/comparisonTransfer_41VAll.txt -o $folder/LGHGCompare41V.root -O $folder/LGHGCompare41V -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
#./CompareCalib -d 0 -E 3 -e 2 -f -R -I runConfigs/comparisonTransfer_42VAll.txt -o $folder/LGHGCompare42V.root -O $folder/LGHGCompare42V -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
#./CompareCalib -d 0 -E 3 -e 2 -f -R -I runConfigs/comparisonTransfer_43_5VAll.txt -o $folder/LGHGCompare43_5V.root -O $folder/LGHGCompare43_5V -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
#./CompareCalib -d 0 -E 3 -e 2 -f -R -I runConfigs/comparisonTransfer_45VSetA.txt -o $folder/LGHGCompare45VSetA.root -O $folder/LGHGCompare45VSetA -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
