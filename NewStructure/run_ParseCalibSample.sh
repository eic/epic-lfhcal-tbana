#! /bin/bash

if [ $1 = "eglimos" ]; then 
    InputFile=/home/ewa/EIC/epic-lfhcal-tbana/configs/config_HGCROCPedestalCalibParser.txt
    InputMapping=/home/ewa/EIC/epic-lfhcal-tbana/configs/mapping_HGCROC_ORNL_Cosmics_20251017.txt
    InputCalibFile=/home/ewa/EIC/DATA/HGCROCData/Cosmics_Oct25/converted/rawHGCROC_wPed_051.root
    OutputPath=/home/ewa/EIC/DATA/HGCROCData/TestOut/rawHGCROC_overwrittenPed_051.root
    RunListFile=/home/ewa/EIC/epic-lfhcal-tbana/configs/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt
    RunNumber=1051

    ./ParseCalibSamples -i $InputFile -c $InputCalibFile -m $InputMapping -o $OutputPath -r $RunListFile -n $RunNumber
elif [ $1 = "eglimos_csv" ]; then
    InputFile=/home/ewa/EIC/DATA/CALIB_SAMPLES/inj_adc_samples_208_24_300.csv
    OutputPath=/home/ewa/EIC/DATA/CALIB_SAMPLES/CalibOutput.root
    OutputPlots=/home/ewa/EIC/DATA/CALIB_SAMPLES/Plots
    RunListFile=/home/ewa/EIC/epic-lfhcal-tbana/configs/DataTakingDB_ORNL_Cosmics_HGCROC_202510.txt
    InputMapping=/home/ewa/EIC/epic-lfhcal-tbana/configs/mapping_HGCROC_ORNL_Cosmics_20251017.txt
    RunNumber=1020

    ./ParseCalibSamples -i $InputFile -m $InputMapping -o $OutputPath -p $OutputPlots -r $RunListFile -n $RunNumber
else 
    echo "Please select a known user name, otherwise I don't know where the data is."
    exit
fi 
