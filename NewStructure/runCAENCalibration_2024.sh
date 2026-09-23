#! /bin/bash

PlotBaseDir=..
runNrFile='../configs/TB2024/DataTakingDB_202409_CAEN.csv'

function MuonCalib()
{
  echo "=================================================================================="
  echo "option $1"
  echo "run Nr Pedestal: $2"
  echo "run Nr Muon: $3"
  echo "dataRawDir: $4"
  echo "dataOutDir: $5"
  echo "OutNameRun:" $6
  if [ $1 == "transfer" ]; then 
    echo "badchannelMap:" $7
  else 
    echo "layer skip:" $8
  fi
  echo "=================================================================================="
  if [ $1 == "transferWithBC" ]; then 
    time ./DataPrep -d 1 -a -e -f -P $5/PedestalCalib_$2.root -i $4/raw_$3.root -o $4/rawPedWBC_$3.root -B $7 -O $PlotBaseDir/CAEN_PlotsCalibTransferBC_2024/$6 -r $runNrFile -l $8
  elif [ $1 == "defaultWithBC" ]; then 
    time ./DataPrep -e -a -f -d 1  -s -i $4/rawPedWBC_$3.root -o $5/rawPedAndMuonWBC_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonBC_2024/$6 -r $runNrFile -l $8
  elif [ $1 == "impWBC1st" ]; then 
    time ./DataPrep -f -d 1 -a -S -i $5/rawPedAndMuonWBC_$3.root -o $5/rawPedAndMuonWBCImp_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImprovedBC_2024/$6 -r $runNrFile -l $8
  elif [ $1 == "impWBC2nd" ]; then 
    time ./DataPrep -f -d 1 -a -S -i $5/rawPedAndMuonWBCImp_$3.root -o $5/rawPedAndMuonWBCImp2nd_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImprovedBC_2024/$6_2ndIte -r $runNrFile -l $8
  elif [ $1 == "impWBC3rd" ]; then 
    time ./DataPrep -f -d 1 -a -S -i $5/rawPedAndMuonWBCImp2nd_$3.root -o $5/rawPedAndMuonWBCImp3rd_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImprovedBC_2024/$6_3rdIte -r $runNrFile -l $8
  elif [ $1 == "impWBC4th" ]; then 
    time ./DataPrep -f -e -d 1 -a -S -i $5/rawPedAndMuonWBCImp3rd_$3.root -o $5/rawPedAndMuonWBCImp4th_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImprovedBC_2024/$6_4thIte -r $runNrFile -l $8
  elif [ $1 == "impWBC5th" ]; then 
    time ./DataPrep -f -e -d 1 -a -S -i $5/rawPedAndMuonWBCImp4th_$3.root -o $5/rawPedAndMuonWBCImp5th_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImprovedBC_2024/$6_5thIte -r $runNrFile -l $8
  elif [ $1 == "impWBC6th" ]; then 
    time ./DataPrep -f -d 1 -a -S -i $5/rawPedAndMuonWBCImp5th_$3.root -o $5/rawPedAndMuonWBCImp6th_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImprovedBC_2024/$6_6thIte -r $runNrFile -l $8
  elif [ $1 == "noise" ]; then 
    ./DataPrep -f -d 1  -n -i $5/rawPedAndMuon_$3.root -o $5/rawPedAndMuonNoise_$2.root -O $PlotBaseDir/CAEN_PlotsCalibNoiseRe_2024/$6 -r $runNrFile
  elif [ $1 == "transferAlt" ]; then 
    ./DataPrep -d 1  -f -P $5/rawPedAndMuonNoise_$3.root -i $4/raw_$3.root -o $4/rawPedImp_$3.root -r $runNrFile
  elif [ $1 == "defaultImpPed" ]; then 
    ./DataPrep -f -d 1  -s -i $4/rawPedImp_$3.root -o $5/rawPedImpAndMuon_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonPedImp_2024/$6 -r $runNrFile
  elif [ $1 == "saveNewPed" ]; then 
    ./DataPrep -f -d 1 -N -i $5/rawPedAndMuonNoise_$3.root -o $4/raw_pedonly_$3.root 
  elif [ $1 == "saveNewMuon" ]; then 
    time ./DataPrep -f -d 1 -M -i $5/rawPedAndMuonWBC_$3.root -o $4/raw_muononly_$3.root 
  elif [ $1 == "imp1st_red" ]; then 
    time ./DataPrep -f -d 1  -S -i $4/raw_muononly_$3.root -o $5/rawPedAndMuonImp1st_red_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImproved_2024/$6_Red1st -r $runNrFile -l $8
  elif [ $1 == "imp2nd_red" ]; then 
    time ./DataPrep -f -d 1  -S -i $5/rawPedAndMuonImp1st_red_$3.root -o $5/rawPedAndMuonImp2nd_red_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImproved_2024/$6_Red2nd -r $runNrFile -l $8
  elif [ $1 == "imp3rd_red" ]; then 
    time ./DataPrep -f -d 1  -S -i $5/rawPedAndMuonImp2nd_red_$3.root -o $5/rawPedAndMuonImp3rd_red_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImproved_2024/$6_Red3rd -r $runNrFile -l $8
  elif [ $1 == "imp4th_red" ]; then 
    time ./DataPrep -f -d 1  -S -i $5/rawPedAndMuonImp3rd_red_$3.root -o $5/rawPedAndMuonImp4th_red_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImproved_2024/$6_Red4th -r $runNrFile -l $8
  elif [ $1 == "imp5th_red" ]; then 
    time ./DataPrep -f -d 1  -S -i $5/rawPedAndMuonImp4th_red_$3.root -o $5/rawPedAndMuonImp5th_red_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImproved_2024/$6_Red5th -r $runNrFile -l $8
  elif [ $1 == "imp6th_red" ]; then 
    time ./DataPrep -f -d 1  -S -i $5/rawPedAndMuonImp5th_red_$3.root -o $5/rawPedAndMuonImp6th_red_$3.root -O $PlotBaseDir/CAEN_PlotsCalibMuonImproved_2024/$6_Red6th -r $runNrFile -l $8
  fi
}

# running example:
# bash runCAENCalibration_2024.sh fbockExt2 muoncalib improvedWBC4th FullSetA_1


dataDirRaw=""
dataDirOut=""
if [ $1 = "fbock" ]; then 
  dataDirRaw=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/MuonRuns
  dataDirRawE=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/ElectronRuns
  dataDirRawH=/media/fbock/Samsung_T5/LFHCAL_TB/202408_PST09/CAENdata/HadronRuns
  dataDirOut=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/MuonRuns
  dataDirOutE=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/ElectronRuns
  dataDirOutH=/home/fbock/EIC/Analysis/LFHCalTB2024/CAENdata/HadronRuns
  PlotBaseDir=..
elif [ $1 = "fbockExt" ]; then 
  dataDirRaw=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENData
  dataDirRawE=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENData
  dataDirRawH=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENData
  dataDirOut=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENData
  dataDirOutE=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENData
  dataDirOutH=/media/fbock/T7/LFHCalTBData/202408_PST09/CAENData
  PlotBaseDir=/media/fbock/T7/LFHCalTBData/202408_PST09/ReanlysisCAEN
elif [ $1 = "eglimos" ]; then
  dataDirRaw=/home/ewa/EIC/test_beam2024/fullScanC
  dataDirRawE=/home/ewa/EIC/test_beam2024/fullScanC
  dataDirRawH=/home/ewa/EIC/test_beam2024/fullScanC
  dataDirOut=/home/ewa/EIC/test_beam2024/fullScanC/Output/MuonRuns
  dataDirOutE=/home/ewa/EIC/test_beam2024/fullScanC/Output/ElectronRuns
  dataDirOutH=/home/ewa/EIC/test_beam2024/fullScanC/Output/HadronRuns
  PlotBaseDir=..
elif [ $1 = "rjh78" ]; then
  dataDirRaw=/Users/ryanhamilton/Documents/Research/data.nosync/202408_PST09/CAENData/outfiles/MuonRuns
  dataDirRawE=/Users/ryanhamilton/Documents/Research/data.nosync/202408_PST09/CAENData/outfiles/ElectronRuns
  dataDirRawH=/Users/ryanhamilton/Documents/Research/data.nosync/202408_PST09/CAENData/outfiles/HadronRuns
  dataDirOut=/Users/ryanhamilton/Documents/Research/data.nosync/202408_PST09/CAENData/outfiles/MuonRuns
  dataDirOutE=/Users/ryanhamilton/Documents/Research/data.nosync/202408_PST09/CAENData/outfiles/ElectronRuns
  dataDirOutH=/Users/ryanhamilton/Documents/Research/data.nosync/202408_PST09/CAENData/outfiles/HadronRuns
  PlotBaseDir=/Users/ryanhamilton/Documents/Research/data.nosync/202408_PST09/CAENData/outfiles/plots

elif [ $1 = "egpott" ]; then
  dataDirRaw=/Users/egpott/rhig/lfhcal/data/outfiles/MuonRuns
  dataDirRawE=/Users/egpott/rhig/lfhcal/data/outfiles/ElectronRuns
  dataDirRawH=/Users/egpott/rhig/lfhcal/data/outfiles/HadronRuns
  dataDirOut=/Users/egpott/rhig/lfhcal/data/outfiles/MuonRuns
  dataDirOutE=/Users/egpott/rhig/lfhcal/data/outfiles/ElectronRuns
  dataDirOutH=/Users/egpott/rhig/lfhcal/data/outfiles/ElectronRuns
  PlotBaseDir=/Users/egpott/rhig/lfhcal/data/outfiles/plots

elif [ $1 = "yale" ]; then
  dataDirRaw=/media/lfhcal/LFHCal_Backup_11/Test_Beams/2024_PST9/rawroot
  dataDirRawE=/media/lfhcal/LFHCal_Backup_11/Test_Beams/2024_PST9/rawroot
  dataDirRawH=/media/lfhcal/LFHCal_Backup_11/Test_Beams/2024_PST9/rawroot
  dataDirOut=/media/lfhcal/LFHCal_Backup_11/Test_Beams/2024_PST9/rawroot
  dataDirOutE=/media/lfhcal/LFHCal_Backup_11/Test_Beams/2024_PST9/rawroot
  dataDirOutH=/media/lfhcal/LFHCal_Backup_11/Test_Beams/2024_PST9/rawroot
  PlotBaseDir=/media/lfhcal/LFHCal_Backup_11/Test_Beams/2024_PST9/plots

else
  echo "Please select a known user name, otherwise I don't know where the data is"
  exit
fi

skipLayer=0
if [ $# -eq 5 ]; then
  skipLayer=$5
fi

runList=../configs/TB2024/DataTakingDB_202409_CAEN.csv

###############################################################
## pedestal running
###############################################################
if [ $2 = "pedestal" ]; then

  if [ $3 = "FullSetA" ]; then
    pedestalRuns='277 271'
  elif [ $3 = "FullSetB" ]; then
    pedestalRuns='332 369'
  elif [ $3 = "FullSetC" ]; then
    pedestalRuns='377 404'
  elif [ $3 = "FullSetD" ]; then
    pedestalRuns='454'
  elif [ $3 = "FullSetH" ]; then
    pedestalRuns='528 552 553'
  elif [ $3 = "MiniSetE" ]; then
    pedestalRuns=' 465 476'
  elif [ $3 = "MiniSetF" ]; then
    pedestalRuns='492 505'
  elif [ $3 = "MiniSetG" ]; then
    pedestalRuns='521'
  elif [ $3 = "HVScan" ]; then
    pedestalRuns='311 315 308 306 404 303 300 521'  
  fi

  for runNr in $pedestalRuns; do
    ./DataPrep -d 1 -p -i $dataDirRaw/raw_$runNr.root -f -o $dataDirOut/PedestalCalib_$runNr.root -O $PlotBaseDir/CAEN_PlotsPedestal_2024/Run$runNr -r $runList
  done;
fi

###############################################################
## muon calib
###############################################################
if [ $2 == "calibMuon" ]; then 
  badChannelMap='../configs/TB2024/badChannelMap_TBSetup_CAEN_202408.txt'
  # muon runs different scans in groups with separate pedestal
  
  #####################################################################
  # HV Scan
  #####################################################################
  if [ $4 == "MuonHV_44V" ] ; then 
    echo "running muon calib for 44V runs"
    #50.3 events
    runPed='303'
    runMuon='305'
    runMuonName='muonHVScan_44V'
    badChannelMap='../configs/TB2024/badChannelMap_TBSetup_CAEN_202408_HVScan.txt'
  elif [ $4 == "MuonHV_43V" ] ; then 
    echo "running muon calib for 43V runs"
    #50.1K events
    runPed='306'
    runMuon='307'
    runMuonName='muonHVScan_43V'  
    badChannelMap='../configs/TB2024/badChannelMap_TBSetup_CAEN_202408_HVScan.txt'
  elif [ $4 == "MuonHV_42V" ] ; then 
    echo "running muon calib for 42V runs"
    #50.6K events
    runPed='308'
    runMuon='309'
    runMuonName='muonHVScan_42V'  
    badChannelMap='../configs/TB2024/badChannelMap_TBSetup_CAEN_202408_HVScan.txt'
  elif [ $4 == "MuonHV_41V" ] ; then 
    echo "running muon calib for 41V runs"
    #51K events
    runPed='311'
    runMuon='312'
    runMuonName='muonHVScan_40V'  
    badChannelMap='../configs/TB2024/badChannelMap_TBSetup_CAEN_202408_HVScan.txt'
  elif [ $4 == "MuonHV_40V" ] ; then 
    echo "running muon calib for 40V runs"
    #50.8K events
    runPed='315'
    runMuon='316'
    runMuonName='muonHVScan_40V'  
    badChannelMap='../configs/TB2024/badChannelMap_TBSetup_CAEN_202408_HVScan.txt'
  #####################################################################  
  # Set A  - 45 V
  # muon runs: '244 250 282 283'
  # pdestal runs: '271 277'
  #####################################################################
  elif [ $4 == "FullSetA_1" ] ; then 
    echo "running muon calib for 45V runs, campaing A1"
    # 192K events
    runPed='271'
    runMuon='muonScanA1_45V'
    runMuonName='muonScanA1_45V'  
  elif [ $4 == "FullSetA_2" ] ; then 
    echo "running muon calib for 45V runs, campaing A2"
    # 201.6K events
    runPed='277'
    runMuon='muonScanA2_45V'
    runMuonName='muonScanA2_45V'  
  #####################################################################
  # Set D  - 45 V
  # first part of campaign messed up runs '412 417 420' don't use
  # muon runs: '460 456 457'
  # pedestal runs: '454'
  #####################################################################
  elif [ $4 == "FullSetD_2" ] ; then 
    echo "running muon calib for 45V runs, campaing D2"
    # 29.3K events
    runPed='454'
    runMuon='muonScanD2_45V'
    runMuonName='muonScanD2_45V'  
  #####################################################################
  # Set H  - 45 V
  # muon runs: '526 527 554 559'
  # pedestal runs: '528 552 553'
  #####################################################################
  elif [ $4 == "FullSetH_1" ] ; then 
    echo "running muon calib for 45V runs, campaing H1"
    # 50.6K events 2nd column underrespresented
    runPed='528'
    runMuon='muonScanH1_45V'
    runMuonName='muonScanH1_45V'  
  elif [ $4 == "FullSetH_2" ] ; then 
    echo "running muon calib for 45V runs, campaing H2"
    #33.5K events 1st-2nd column only
    runPed='552'
    runMuon='muonScanH2_45V'
    runMuonName='muonScanH2_45V'  
  #####################################################################
  # Set B - 42 V
  # muon runs '331 322 370 371 374'
  # pedestal runs '332 369'
  #####################################################################
  elif [ $4 == "FullSetB_1" ] ; then 
    echo "running muon calib for 42V runs, campaing B1"
    # 202.6K events
    runPed='332'
    runMuon='muonScanB1_42V'
    runMuonName='muonScanB1_42V'  
  elif [ $4 == "FullSetB_1_1" ] ; then 
    echo "running muon calib for 42V runs, campaing B1 - (-5,0)"
    runPed='332'
    runMuon='331'
    runMuonName='331'  
  elif [ $4 == "FullSetB_1_2" ] ; then 
    echo "running muon calib for 42V runs, campaing B1 - (5,0)"
    runPed='332'
    runMuon='322'
    runMuonName='322'  
  elif [ $4 == "FullSetB_2" ] ; then 
    echo "running muon calib for 42V runs, campaing B2"
    # 214.8k events
    runPed='369'
    runMuon='muonScanB2_42V'
    runMuonName='muonScanB2_42V'  
  #####################################################################
  # Set C - 43.5 V
  # muon runs '376 375 405 410 408'
  # pedestal runs '377 404'
  # 240.2K events
  #####################################################################
  elif [ $4 == "FullSetC_1" ] ; then 
    echo "running muon calib for 43.5V runs, campaing C1"
    runPed='377'
    runMuon='muonScanC1_43_5V'
    runMuonName='muonScanC1_43_5V'  
  elif [ $4 == "FullSetC_2" ] ; then 
    echo "running muon calib for 43.5V runs, campaing C2"
    runPed='404'
    runMuon='muonScanC2_43_5V'
    runMuonName='muonScanC2_43_5V'  
  #####################################################################
  # Set E - 40 V
  # muon runs '463 464 481 478'
  # pedestal runs '465 476'
  # 40.7K events
  #####################################################################
  elif [ $4 == "FullSetE_1" ] ; then 
    echo "running muon calib for 40V runs, campaing E1"
    runPed='465'
    runMuon='muonScanE1_40V'
    runMuonName='muonScanE1_40V'  
  elif [ $4 == "FullSetE_2" ] ; then 
    echo "running muon calib for 40V runs, campaing E2"
    runPed='476'
    runMuon='muonScanE2_40V'
    runMuonName='muonScanE2_40V'  
  elif [ $4 == "FullSetE" ] ; then 
    echo "running muon calib for 40V runs, campaing E all muon runs merged"
    runPed='465'
    runMuon='muonScanE_40V'
    runMuonName='muonScanE_40V'  
  #####################################################################
  # Set F - 41 V
  # muon runs '486 489 507 506'
  # pedestal runs '492 505'
  #####################################################################
  elif [ $4 == "FullSetF_1" ] ; then 
    # 22.6K events
    echo "running muon calib for 41V runs, campaing F1"
    runPed='492'
    runMuon='muonScanF1_41V'
    runMuonName='muonScanF1_41V'  
  elif [ $4 == "FullSetF_2" ] ; then 
    # 42.8K events
    echo "running muon calib for 41V runs, campaing F2"
    runPed='505'
    runMuon='muonScanF2_41V'
    runMuonName='muonScanF2_41V'  
  elif [ $4 == "FullSetF" ] ; then 
    echo "running muon calib for 41V runs, campaing F all muon runs merged"
    runPed='492'
    runMuon='muonScanF_41V'
    runMuonName='muonScanF_41V'  
  #####################################################################
  # Set G - 46 V
  # muon runs '508 510 511 525'
  # pedestal runs '521'
  # 101.7K events
  #####################################################################
  elif [ $4 == "FullSetG" ] ; then 
    echo "running muon calib for 46V runs, campaing G"
    runPed='492'
    runMuon='muonScanG_46V'
    runMuonName='muonScanG_46V'  
  fi
  MuonCalib $3 $runPed $runMuon $dataDirRaw $dataDirOut $runMuonName $badChannelMap $skipLayer  
fi

###############################################################
## reextraction of LG-HG correlation
###############################################################
if [ $2 == "ReextractLGHG" ]; then 
  runNr=''
  calibFile=''
  if [ $3 = "FullSetA" ]; then
    runNr='269'
    calibFile=$dataDirOut/calib_muonScanA1_45V_V2.root
  elif [ $3 = "FullSetA_2" ]; then
    runNr='275'
    calibFile=$dataDirOut/calib_muonScanA1_45V_V2.root
  elif [ $3 = "FullSetB" ]; then
    runNr='357'
    calibFile=$dataDirOut/calib_muonScanB1_42V_V2.root
  elif [ $3 = "FullSetC" ]; then
    runNr='394'
    calibFile=$dataDirOut/calib_muonScanC2_43_5V_V2.root
  elif [ $3 = "FullSetD" ]; then
    runNr='441'
    calibFile=''
  elif [ $3 = "FullSetH" ]; then
    runNr='545'
    calibFile=''
  elif [ $3 = "MiniSetE" ]; then
    runNr='472'
    calibFile=$dataDirOut/calib_muonScanE1_40V_V2.root
  elif [ $3 = "MiniSetF" ]; then
    runNr='504'
    calibFile=$dataDirOut/calib_muonScanF_41V_V2.root
  elif [ $3 = "MiniSetG" ]; then
    runNr='520'
    calibFile=''
  fi
  ####################################################
  # additional switches
  # -D swith on data rejection due to data corruption
  # -K KKK enable reextraction of LG/HG correlation values from using calib-file "KKK" and write to output
  ####################################################
  time ./DataPrep -d 1 -a -e -D -f -K $calibFile -i $dataDirRaw/raw_$runNr.root -o $dataDirOut/rawCalibReExtractedLGHG_$runNr.root -B $badChannelMap -O $PlotBaseDir/CAEN_ReextractLGHG_2024/Run_$runNr -r $runNrFile -l $skipLayer
fi

if [ $2 == "reducemuons" ]; then
  runs='261 264 265 269 270 272 274 275 ' 
  for runNr in $runs; do 
    time ./DataPrep -f -d 1 -M -i $dataDirOutH/calibrated_Run_$runNr.root -o $dataDirOutH/calibrated_localmuononly_Run_$runNr.root 
  done
  runs='251 252 254 257 258 ' 
  for runNr in $runs; do 
    time ./DataPrep -f -d 1 -M -i $dataDirOutE/calibrated_Run_$runNr.root -o $dataDirOutE/calibrated_localmuononly_Run_$runNr.root 
  done	
  time ./DataPrep -f -d 1 -M -i $dataDirOut/calibratedMuon_muonScanA1_45V.root -o $dataDirOut/calibratedMuon_localmuononly_muonScanA1_45V.root 
  time ./DataPrep -f -d 1 -M -i $dataDirOut/calibratedMuon_muonScanA2_45V.root -o $dataDirOut/calibratedMuon_localmuononly_muonScanA2_45V.root 
fi
