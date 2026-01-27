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
    inj='20 30 40 50 60 70 80 100 125 150 175 200 225 250 275 300 400 500 600 700 800 900 1000 1200'
    # inj='20 30 40 50 60 70 80'
    # inj='20'
    RunListFile=/home/ewa/EIC/epic-lfhcal-tbana/configs/InjectionTest_ORNL_Dec25.csv
    InputMapping=/home/ewa/EIC/epic-lfhcal-tbana/configs/mapping_HGCROC_ORNL_Cosmics_202512.txt

    for nr in $inj; do
        nr_full=$(printf %04d $nr)
        # echo $nr_full
        InputFile=/home/ewa/EIC/DATA/INJECTION/InjectionTests_ANL-01/ANL-01_$nr_full/inj_adc_samples_208_2_$nr.csv
        OutputPath=/home/ewa/EIC/DATA/INJECTION/Output/InjectionTest_$nr.root
        OutputPlots=/home/ewa/EIC/DATA/INJECTION/Output/Plots/$nr_full

        mkdir -p $OutputPlots

        ./ParseCalibSamples -i $InputFile -m $InputMapping -o $OutputPath -p $OutputPlots -r $RunListFile -n $nr -d 5
        # ./ParseCalibSamples -i $InputFile -m $InputMapping -o $OutputPath -r $RunListFile -n $nr -d 0
    done

    ./CompareHGCROCCalib -i /home/ewa/EIC/DATA/INJECTION/compareFileList.txt -p /home/ewa/EIC/DATA/INJECTION/Output/Plots/ComparePlots -d 5
elif [ $1 = "eglimos_injJan26" ]; then
    inj='10 25 40 55 70 85 100 115 130 145 160 175 190 205 220 235 250 265 280 295 310 325 340 355 370 385 400 415 430 445 460 475 490 505 520 535 550 565 580 595 610 625 640 655 670 685 700 715 730 745 760 775 790 815 830 845 860 875 890 905 920 935 950 965 980 995'
    RunListFile=/home/ewa/EIC/epic-lfhcal-tbana/configs/InjectionTest_ORNL_Jan26.csv
    InputMapping=/home/ewa/EIC/epic-lfhcal-tbana/configs/LocalTesting/mapping_HGCROC_ORNL_Cosmics_LB_Config7_Injection.txt

    # for nr in $inj; do
    #     nr_full=$(printf %04d $nr)
    #     # echo $nr_full
    #     InputFile=/home/ewa/EIC/DATA/InjectionTests_January/Input/inj_adc_samples_208_8_$nr.csv
    #     OutputPath=/home/ewa/EIC/DATA/InjectionTests_January/Output/InjectionTest_$nr.root
    #     OutputPlots=/home/ewa/EIC/DATA/InjectionTests_January/Output/Plots/$nr_full

    #     mkdir -p $OutputPlots

    #     ./ParseCalibSamples -i $InputFile -m $InputMapping -o $OutputPath -p $OutputPlots -r $RunListFile -n $nr -d 0
    #     # ./ParseCalibSamples -i $InputFile -m $InputMapping -o $OutputPath -r $RunListFile -n $nr -d 0
    # done

    mkdir -p /home/ewa/EIC/DATA/InjectionTests_January/Output/Plots/ComparePlots

    ./CompareHGCROCCalib -i /home/ewa/EIC/DATA/InjectionTests_January/compareFileList.txt -p /home/ewa/EIC/DATA/InjectionTests_January/Output/Plots/ComparePlots -d 5
else 
    echo "Please select a known user name, otherwise I don't know where the data is."
    exit
fi 
