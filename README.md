# epic-lfhcal-tbana software repo

This software repo is meant to serve as basis for the test beam root based analysis of the LFHCal test beams and test beam setups.
The repo contains the initial code used in 2023 under OldStructure & the newer version of the code under NewStructure <br>

A detailed introduction to this package in particular the NewStructure can be found [here](https://friederikebock.gitbook.io/epiclfhcaltb-ana)
It is highly recommended to follow that guide.

For convenience a linker script is provided, which can be used to recreate the software structure with soft links. Execute it in the directory where you would like to work after having added your user name and path to the software repo

```console
bash $PATHTOSCRIPT/prepareAnalysisDirectory.sh $USERNAME [new/old]
```


## OldStructure

This part of the software was initially used for the analysis of the test beam from 2023. The main root macro for the test beam analysis can is

```c+
OldStructure/makeSimplePlotsFromJanusTree.C
```

How to start it can be found in ```runJanusExtraction.sh```, which serves as primary steering script for the data analysis. It also contains several examples on how to compare the output of different runs included in: <br>

```c+
OldStructure/CompareDifferentRuns.C
```


With this code also the single photon extraction can be done, this is steered by ```runSinglePhotonExtraction.sh``` for which the main root macro is 
```c+
OldStructure/makeSinglePhotonSpectraFitsFromJanusTree.C
```
The comparisons for the single photon spectra are steered by ```runCompareRunsForSinglePhotons.sh``` which have as main root macro:

```c+
OldStructure/CompareDifferentRunsForSinglePhotonSpectra.C.C
```

## NewStructure
This part of the software has been developed by Vincent Andrieux (@vandrie) primarily for the 2024 test beam analysis; it is being adapted to be usable also for the 2023 data.

### Running the analysis through scripts

#### September 2023 data

The data taken in September 2023 at the SPS-H4 beam line, was taken without absorber plates presents. The logbook can be found [here](https://docs.google.com/spreadsheets/d/1GBmztS66Cagwr1mpXuaDmIfSKAuhBm1gmhhpk7qSgbg/edit?usp=sharing), if you don't have access please ask Friederike for it. <br>
The raw data for this campaign was unfortunately partially lost and thus a separate routine had to be introduced reading the data from the initally converted root files. The conversion can be done with:

```console
bash convertData_Sept2023.sh $USERNAME [singe/all]
```

As for the ```prepareAnalysisDirectory.sh```, please add your username and the path to the data. The script is under construction and even under all might only have few runs commented in, feel free to uncomment the rest.<br>

The further analysis of this data is still under development and so far only the pedestal extraction is fully working as second step. The corresponding script can be run as follows:

```console
bash runCalibration_Sept2023.sh $USERNAME pedestal
```
Same principle applies as for the converter script. As we further develop the code more functionality will become available.

#### October 2023 data
The data taken in October 2023 at the PS-T09 beam line, was taken with only 14 layers, parasitic to the FOCal-E with various number of thin tungsten plates infront of the mini-LFHCal module. The logbook can be found [here](https://docs.google.com/spreadsheets/d/1oRI3KlPyHouo5P4J70wLrGlaFaeNuLCGMCwyNxyNMf4/edit?usp=sharing), if you don't have access please ask Friederike for it. <br>
This data was taken with a previous version of the Janus software, as such the reading works a little differently. So far only the data converter has been tested and adapted 

```console
bash convertData_Oct2023.sh $USERNAME [singe/all]
```
As for the ```prepareAnalysisDirectory.sh```, please add your username and the path to the data. The script is under construction and even under all might only have few runs commented in, feel free to uncomment the rest.<br>
Attention for a few runs (201,202,203,204,205), the converter will fail as a few channels were accidentally masked breaking the conversion. This data will have to be discarded. As we further develop the code more functionality will become available.

#### 2024 data
The data taken in August/September 2024 at the PS-T09 beam line is the first data available for a full module. The logbook can be found [here](https://docs.google.com/spreadsheets/d/1XaiSmG4jBaBOyjbjdiNuSeehjeZC03_2A7Ccoq0nIbI/edit?usp=sharing), if you don't have access please ask Friederike for it. A summary of the CAEN data taking campaign can be found on our [wiki](https://wiki.bnl.gov/EPIC/index.php?title=LFHCal_Fall_2024_Test_Beam).<br> 
The data conversion can be done using the script

```console
bash convertData_2024.sh $USERNAME [single/all]
```
As for the ```prepareAnalysisDirectory.sh```, please add your username and the path to the data.  This script so far contains all pedestal and muon runs under the flag ```all```. Further runs will be added in the future. <br>
The analysis of the 2024 muon runs is furthest developed. The pedestal extraction can be starting with the following command for all pedestal runs
```console
bash runCalibration_2024.sh $USERNAME pedestal
```
As for the ```prepareAnalysisDirectory.sh```, please add your username and the path to the data, as well as the calibration output directory. The further steps in the analysis , like merging of the raw data for different muon runs can be started with the argument ```mergemuons``` instead of ```pedestal```. In the script all combinations of muon runs and pedestal runs are defined with corresponding variables, i.e. 
```console
muonHVScan_44V='305'
pedHVScan_44V='303'
```
The corresponding analysis of the muon calib extraction can be started with 
```console
bash runCalibration_2024.sh $USERNAME muoncalibHV44 transfer
```
the last argument only needs to be used in case a new pedestal calibration has been created and needs to be transferred to the corresponding muon run file. Otherwise the following command is sufficient.
```console
bash runCalibration_2024.sh $USERNAME muoncalibHV44
```
The muon calibration can be started for all muon runs ```muoncalib``` or for specific parts of the campaign ```muoncalib[HV44|HV43|HV42|HV41|HV40|A|B|C|D|E|F|G|H]```. Please be careful if you start all runs, this will take a while and will produce quite some control plots.

### Individual calls of the analysis steps
Here an example of running the code from ASCII input to calibrated ROOT output format is provided:

1. Compile the code with 
  ```console
  make DataPrep Convert DataAna
  ```
  if you changed something in the base classes majorly first 
  ```console
    make clean
    make DataPrep Convert DataAna
  ```

2. As always please run the helper function first to make yourself familar with the options
  ```console
  ./DataPrep -h
  ```

3. Convert ASCII to root (step to be done for every single ASCII file):
  ```console
  ./Convert -c PATH_INPUT_FILE/RunXXX_list.txt -o PATH_OUTPUT_FILE/WhateverName.root -m ../configs/mappingFile_202409_CAEN.txt -r ../configs/DataTakingDB_202409_CAEN.csv
  ```

  ```console
  ./Convert -c Run375_list.txt -o RawMuonBeamPlus5.root -m ../configs/mappingFile_202409_CAEN.txt -r ../configs/DataTakingDB_202409_CAEN.csv
  ./Convert -c Run376_list.txt -o RawMuonBeamMinus5.root -m ../configs/mappingFile_202409_CAEN.txt -r ../configs/DataTakingDB_202409_CAEN.csv
  ./Convert -c Run377_list.txt -o RawPedestal.root -m ../configs/mappingFile_202409_CAEN.txt -r ../configs/DataTakingDB_202409_CAEN.csv
  ./Convert -c Run379_list.txt -o RawElectron1GeV.root -m ../configs/mappingFile_202409_CAEN.txt -r ../configs/DataTakingDB_202409_CAEN.csv
  ```

4. Extract pedestal value (currently simple gaussian fit):

  ```console
  ./DataPrep (-f) -p -i RawPedestal.root -o PedestalCalib.root
  ```



5. Extract MIP calibration:
    1. Conveniently hadd the input file to make sure we should see MIP signal on all tiles (included furthest Jura or Saleve)
      ```console
      hadd  RawMuonBeamFullAcceptance.root   RawMuonBeamPlus5.root  RawMuonBeamMinus5.root
      ```


    2. Transfer Calibration object from Pedestal file to MIP file
      ```console
      ./DataPrep (-f) -P PedestalCalib.root -i RawMuonBeamFullAcceptance.root -o RawMuonBeamFullAcceptancePedCalib.root
      ```


    3. Extract MIP signal based on Landau fit (request the Pedestal file to shift the ADC distribution to get rid of most of the noise, the tail is still taken into account from the values stored in the calibration object and due to potential deviation in the tail, a decreasing exponential is also added)

      ```console
      ./DataPrep (-f) -s -i RawMuonBeamFullAcceptancePedCalib.root -o RawMuonBeamFullAcceptancePedAndScaleCalib.root
      ```

  => Plenty of room for improvement in this step. Could also be more interesting to save only the histograms and deal with the fit outside<br>
  => Potentially interesting to add a method to load or overwite calib objets from information read in txt file with cellID and values (like in case of fit failure, which we would may want/need to address on a case by case basis) 




6. Apply Calibration to physics data
  ```console
  ./DataPrep (-f) -C RawMuonBeamFullAcceptancePedAndScaleCalib.root -i RawElectron1GeV.root -o CalibratedElectron1GeV.root
  ```


Status:
Not all functions that were foreseen are fully implemented and there are several location where improvement can be done.
At least the most dangerous ones which could prevent execution or meaningful output are:
The location of mapping file which is currently taken from hard coded and oblige the user to launch the executable from the location of the source code (although an option exist to redefine it but it is not yet taken into account)
The DB of runs is saved in a txt file and is also hard coded.


# HGCROC Conversion
To build for processing HGCROC data, I recommend using CMake.  
1) Make sure you have the latest version of the decoder `git pull --recurse-submodules`.  If it is your first time using the submodule you should run `git submodule update --init --recursive`
2) Set up the build directory.  From the `NewStructure` folder, run `CMake -B build .`
3) Compile the code.  Switch to the build directory with `cd build` and compile with `make`
4) You now have all the executables for the analysis.  To convert HGCROC data, use `HGCROC_Convert -w ...`