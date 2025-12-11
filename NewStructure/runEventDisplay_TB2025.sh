#! /bin/bash
dataDirOut=""
runNr=""
if [ $1 = "eglimos" ]; then 
    dataDirOut=/home/ewa/EIC/DATA/testBeam_Nov2025/2025_12_10_DataFromFredi/convertedData
    plotDir=/home/ewa/EIC/DATA/testBeam_Nov2025/2025_12_10_DataFromFredi/Output/EventDisplay
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

runNrFile='/home/ewa/EIC/epic-lfhcal-tbana/configs/TB2025/DataTakingDB_202511_HGCROC.csv'

runs='165'
for runNr in $runs; do 
    if [[ $2 = "tot" ]]; then
	    ./Display -i $dataDirOut/rawHGCROC_wPed_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/ToT -d 0 -F pdf -N 10 -d 3 -o
    elif [[ $2 = "adc" ]]; then
	    ./Display -i $dataDirOut/rawHGCROC_wPed_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/ADC -d 0 -F pdf -N 10 -d 3 -c
    else 
        ./Display -i $dataDirOut/rawHGCROC_wPed_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/PedestalSubtracted -d 0 -F pdf -N 10 -d 3 -p
    fi
done
