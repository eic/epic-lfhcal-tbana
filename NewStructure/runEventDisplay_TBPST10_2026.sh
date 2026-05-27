#! /bin/bash
dataDirOut=""
runNr=""
if [ $1 = "eglimos" ]; then 
    dataDirOut=/home/ewa/EIC/DATA/testBeam_Nov2025/2025_12_10_DataFromFredi/convertedData
    plotDir=/home/ewa/EIC/DATA/testBeam_Nov2025/2025_12_10_DataFromFredi/Output/EventDisplay
elif [ $1 = "fbockTB" ]; then 
    dataDirOut=/media/fbock/Lennard4TB/202604_PST10/HGCROCData/
    plotDir=/media/fbock/Lennard4TB/202604_PST10/EventDisplay/
elif [ $1 = "yale" ]; then
	dataDirOut=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/rawroot_new
	plotDir=/media/lfhcal/LFHCal_Backup_11/Test_Beams/202604_PST10/EventDisplay
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

runNrFile='../configs/TB2026/DataTakingDB_TBPST10_202604_HGCROC.csv'

runs='Had+_10GeV_FullSetC'

for runNr in $runs; do 
    if [[ $2 = "tot" ]]; then
	    ./Display -i $dataDirOut/rawHGCROC_wPed_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/ToT -d 0 -F pdf -N $3 -o
    elif [[ $2 = "adc" ]]; then
	    ./Display -i $dataDirOut/rawHGCROC_wPed_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/ADC -d 1 -F pdf -N $3 -c
    elif [[ $2 = "full" ]]; then
	    ./Display -i $dataDirOut/calibratedHGCROC_Run_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/Calibrated -d 1 -F pdf -N $3 -c -s
    else 
      ./Display -i $dataDirOut/rawHGCROC_wPed_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/PedestalSubtracted -d 0 -F pdf -N $3 -p
    fi
done
