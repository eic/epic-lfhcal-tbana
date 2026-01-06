#! /bin/bash
dataDirOut=""
runNr=""
if [ $1 = "fbockTB" ]; then 
    dataDirOut=/media/fbock/Lennard4TB/LongboardTesting/HGCROCData
    plotDir=/media/fbock/Lennard4TB/LongboardTesting/EventDisplay
else
	echo "Please select a known user name, otherwise I don't know where the data is"
	exit
fi

runNrFile='../configs/LocalTesting/DataTakingDB_ORNL_HGCROC_Cosmics_Longboard_202512.csv'

runs='171 172 175 176' #layers 24-41
# runs='167 165' #layers 16-23, 40-47 
# runs='162 163' #layers 8-15, 48-55
# runs='160' #layers 0-7, 56-59
for runNr in $runs; do 
    if [[ $2 = "tot" ]]; then
	    ./Display -i $dataDirOut/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp3_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/ToT -F pdf -N 1000 -d 1 -o
    elif [[ $2 = "adc" ]]; then
# 	    ./Display -i $dataDirOut/rawHGCROC_wPed_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/ADC -d 0 -F pdf -N 10 -d 3 -c
	    ./Display -i $dataDirOut/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp3_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/ADC -F pdf -N 50 -d 1 -c
    elif [[ $2 = "adcmip" ]]; then
# 	    ./Display -i $dataDirOut/rawHGCROC_wPed_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/ADC -d 0 -F pdf -N 10 -d 3 -c
	    ./Display -i $dataDirOut/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp3_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/ADCMIPCorr -F pdf -N 50 -d 1 -q
    else 
        ./Display -i $dataDirOut/rawHGCROC_mipTrigg_wPedwMuon_wBC_Imp3_$runNr.root -r $runNrFile -P $plotDir/Run_$runNr/PedestalSubtracted -d 0 -F pdf -N 10 -d 3 -p
    fi
done
