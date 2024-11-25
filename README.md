# epiclfhcal

This software repo is meant to serve as basis for the test beam root based analysis of the LFHCal test beams and test beam setups.
The repo contains the initial code used in 2023 under OldStructure & the newer version of the code under NewStructure



## OldStructure


## NewStructure
Here an example of running the code from ASCII input to calibrated ROOT output format is provided:

0/Compile the code with 
```console
make Analyse
```
if you changed something in the base classes majorly first 
```console
  make clean
  make Analyse
```

1/Convert ASCII to root (step to be done for every single ASCII file):
```console
./Analyse -c PATH_INPUT_FILE/RunXXX_list.txt -o PATH_OUTPUT_FILE/WhateverName.root -m ../configs/mappingFile_202409_CAEN.txt -r ../configs/DataTakingDB_202409_CAEN.csv
```

```console
./Analyse -c Run375_list.txt -o RawMuonBeamPlus5.root -m ../configs/mappingFile_202409_CAEN.txt -r ../configs/DataTakingDB_202409_CAEN.csv
./Analyse -c Run376_list.txt -o RawMuonBeamMinus5.root -m ../configs/mappingFile_202409_CAEN.txt -r ../configs/DataTakingDB_202409_CAEN.csv
./Analyse -c Run377_list.txt -o RawPedestal.root -m ../configs/mappingFile_202409_CAEN.txt -r ../configs/DataTakingDB_202409_CAEN.csv
./Analyse -c Run379_list.txt -o RawElectron1GeV.root -m ../configs/mappingFile_202409_CAEN.txt -r ../configs/DataTakingDB_202409_CAEN.csv
```

2/Extract pedestal value (currently simple gaussian fit):

```console
./Analyse (-f) -p -i RawPedestal.root -o PedestalCalib.root
```



3/Extract MIP calibration:
3.1 Conveniently hadd the input file to make sure we should see MIP signal on all tiles (included furthest Jura or Saleve)
```console
hadd  RawMuonBeamFullAcceptance.root   RawMuonBeamPlus5.root  RawMuonBeamMinus5.root
```


3.2/ Transfer Calibration object from Pedestal file to MIP file
```console
./Analyse (-f) -P PedestalCalib.root -i RawMuonBeamFullAcceptance.root -o RawMuonBeamFullAcceptancePedCalib.root
```


3.3/ Extract MIP signal based on Landau fit (request the Pedestal file to shift the ADC distribution to get rid of most of the noise, the tail is still taken into account from the values stored in the calibration object and due to potential deviation in the tail, a decreasing exponential is also added)

```console
./Analyse (-f) -s -i RawMuonBeamFullAcceptancePedCalib.root -o RawMuonBeamFullAcceptancePedAndScaleCalib.root
```

=> Plenty of room for improvement in this step. Could also be more interesting to save only the histograms and deal with the fit outside
=> Potentially interesting to add a method to load or overwite calib objets from information read in txt file with cellID and values (like in case of fit failure, which we would may want/need to address on a case by case basis) 




4/Apply Calibration to physics data
```console
./Analyse (-f) -C RawMuonBeamFullAcceptancePedAndScaleCalib.root -i RawElectron1GeV.root -o CalibratedElectron1GeV.root
```


Status:
Not all functions that were foreseen are fully implemented and there are several location where improvement can be done.
At least the most dangerous ones which could prevent execution or meaningful output are:
The location of mapping file which is currently taken from hard coded and oblige the user to launch the executable from the location of the source code (although an option exist to redefine it but it is not yet taken into account)
The DB of runs is saved in a txt file and is also hard coded.

