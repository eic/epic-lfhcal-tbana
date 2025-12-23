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
elif [ $1 = "eglimos_inj" ]; then
    # inj='20 30 40 50 60 70 80 100 125 150 175 200 225 250 275 300 400 500 600 700 800 900 1000 1200'
    # inj_full='0020 0030 0040 0050 0060 0070 0080 0100 0125 0150 0175 0200 0225 0250 0275 0300 0400 0500 0600 0700 0800 0900 1000 1200'
    # inj='20 30 40 50 60 70 80'
    inj='20'
    RunListFile=/home/ewa/EIC/epic-lfhcal-tbana/configs/InjectionTest_ORNL_Dec25.csv
    InputMapping=/home/ewa/EIC/epic-lfhcal-tbana/configs/mapping_HGCROC_ORNL_Cosmics_202512.txt

    for nr in $inj; do
        nr_full=$(printf %04d $nr)
        # echo $nr_full
        InputFile=/home/ewa/EIC/DATA/INJECTION/InjectionTests_ANL-01/ANL-01_$nr_full/inj_adc_samples_208_2_$nr.csv
        OutputPath=/home/ewa/EIC/DATA/INJECTION/Output/InjectionTest_$nr.root
        OutputPlots=/home/ewa/EIC/DATA/INJECTION/Output/Plots/$nr_full

        mkdir -p $OutputPlots

        # ./ParseCalibSamples -i $InputFile -m $InputMapping -o $OutputPath -p $OutputPlots -r $RunListFile -n $nr -d 5
        ./ParseCalibSamples -i $InputFile -m $InputMapping -o $OutputPath -r $RunListFile -n $nr -d 0
    done

    # ./CompareHGCROCCalib -i /home/ewa/EIC/DATA/INJECTION/compareFileList.txt -p /home/ewa/EIC/DATA/INJECTION/Output/Plots/ComparePlots -d 5
else 
    echo "Please select a known user name, otherwise I don't know where the data is."
    exit
fi 
