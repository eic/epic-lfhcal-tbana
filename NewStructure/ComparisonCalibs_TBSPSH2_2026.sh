#ComparisonHGCROC SPS

#integrations ranges -t (-e extended plotting, -E expanded file list)
# ./CompareCalib -d 0 -e 2 -H -f -t -I configsComparison/comparisonIntegrations.txt -o ../Trending/IntegrationVariation.root -O ../Trending/IntegrationVariation -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv

#iterations -T (-e extended plotting, -E expanded file list)
# ./CompareCalib -d 0 -e 2 -H -f -T -I configsComparison/comparisonInterations_FullSetB_2.txt -o ../Trending/MuonIterationsB2.root -O ../Trending/MuonIterationsB2 -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv
# ./CompareCalib -d 0 -e 2 -H -f -T -I configsComparison/comparisonInterations_FullSetB_1.txt -o ../Trending/MuonIterationsB1.root -O ../Trending/MuonIterationsB1 -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv
# ./CompareCalib -d 0 -e 2 -H -f -T -I configsComparison/comparisonInterations_FullSetE_1.txt -o ../Trending/MuonIterationsE1.root -O ../Trending/MuonIterationsE1 -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv

#muon calibs 2ndIte Set B
# ./CompareCalib -d 0 -e 2 -H -f -R -I configsComparison/comparison2ndIte_FullSetB.txt -o ../Trending/MuonsSetB_2nd.root -O ../Trending/MuonsSetB_2nd -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv
#muon calibs 4th Ite Set B
./CompareCalib -d 0 -e 2 -H -f -R -I configsComparison/comparison4thIte_FullSetB.txt -o ../Trending/MuonsSetB_4th.root -O ../Trending/MuonsSetB_4th -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv

#muon calibs 2ndIte Set B
# ./CompareCalib -d 0 -e 2 -H -f -R -I configsComparison/comparison2ndIte_HVScan.txt -o ../Trending/MuonsHVScan_vs_Run_2nd.root -O ../Trending/MuonsHVScan_vs_Run_2nd -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv
# ./CompareCalib -d 0 -e 2 -H -f -V -I configsComparison/comparison2ndIte_HVScan.txt -o ../Trending/MuonsHVScan_vs_Voltage_2nd.root -O ../Trending/MuonsHVScan_vs_Voltage_2nd -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv



#muon calibs diff settings param scan
# ./CompareCalib -d 0 -e 2 -H -f -R -I configsComparison/comparisonWaveformMips_All.txt -o ../Trending/MuonsDiffSettings.root -O ../Trending/MuonsDiffSettings -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv
# ./CompareCalib -d 0 -e 2 -H -f -R -I configsComparison/comparisonWaveformMips_CCVariation.txt -o ../Trending/MuonsDiffSettings_CCVar.root -O ../Trending/MuonsDiffSettings_CCVar -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv
# ./CompareCalib -d 0 -e 2 -H -f -R -I configsComparison/comparisonWaveformMips_CFVariation.txt -o ../Trending/MuonsDiffSettings_CFVar.root -O ../Trending/MuonsDiffSettings_CFVar -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv
# ./CompareCalib -d 0 -e 2 -H -f -R -I configsComparison/comparisonWaveformMips_RFVariations.txt -o ../Trending/MuonsDiffSettings_RFVar.root -O ../Trending/MuonsDiffSettings_RFVar -r ../configs/TB2026/DataTakingDB_TBSPSH2_202605_HGCROC.csv

#./CompareCalib -d 0 -e 2 -f -R -I runConfigs/comparisonMuons41V.txt -o ../Trending/Muons41V.root -O ../Trending/MuonCalib41V -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
#./CompareCalib -d 0 -e 2 -f -R -I runConfigs/comparisonMuons41VAll.txt -o ../Trending/Muons41VAll.root -O ../Trending/MuonCalib41VAll -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv

#./CompareCalib -d 0 -e 2 -f -R -I runConfigs/comparisonMuons42V.txt -o ../Trending/Muons42V.root -O ../Trending/MuonCalib42V -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
#./CompareCalib -d 0 -e 2 -f -R -I runConfigs/comparisonMuons43_5V.txt -o ../Trending/Muons43_5V.root -O ../Trending/MuonCalib43_5V -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
#./CompareCalib -d 0 -e 2 -f -R -I runConfigs/comparisonMuons45VSetA.txt -o ../Trending/Muons45VSetA.root -O ../Trending/MuonCalib45VSetA -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv

# all runs transfer
#./CompareCalib -d 0 -E 3 -e 2 -f -R -I runConfigs/comparisonTransfer_41VAll.txt -o ../Trending/LGHGCompare41V.root -O ../Trending/LGHGCompare41V -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
#./CompareCalib -d 0 -E 3 -e 2 -f -R -I runConfigs/comparisonTransfer_42VAll.txt -o ../Trending/LGHGCompare42V.root -O ../Trending/LGHGCompare42V -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
#./CompareCalib -d 0 -E 3 -e 2 -f -R -I runConfigs/comparisonTransfer_43_5VAll.txt -o ../Trending/LGHGCompare43_5V.root -O ../Trending/LGHGCompare43_5V -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
#./CompareCalib -d 0 -E 3 -e 2 -f -R -I runConfigs/comparisonTransfer_45VSetA.txt -o ../Trending/LGHGCompare45VSetA.root -O ../Trending/LGHGCompare45VSetA -r ../configs/TB2024/DataTakingDB_202409_CAEN.csv
