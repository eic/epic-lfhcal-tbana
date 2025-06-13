UNAME := $(shell uname)
ifeq ($(UNAME), Linux)
FLAGS =  $(ARCH) -g -fPIC -ansi  -Wno-overloaded-virtual -Wno-long-long -fno-common -pedantic -Wall -std=c++17 -Wl,-rpath='${ORIGIN}' `root-config --libs --ldflags --cflags`
endif
ifeq ($(UNAME), Darwin)
FLAGS =  $(ARCH) -g -fPIC -ansi  -Wno-overloaded-virtual -Wno-long-long -fno-common -pedantic -Wall -std=c++17 `root-config --libs --ldflags --cflags`
endif


LINKER = g++ -g -shared $(OPTIMISATION)

Setup.o: Setup.cc Setup.h RootSetupWrapper.h
	$(CXX) $(FLAGS) -I. -c Setup.cc -o Setup.o

RootSetupWrapper.o: RootSetupWrapper.cc RootSetupWrapper.h Setup.h
	$(CXX) $(FLAGS) -I. -c RootSetupWrapper.cc -o RootSetupWrapper.o

Calib.o: Calib.cc Calib.h
	$(CXX) $(FLAGS) -I. -c Calib.cc -o Calib.o

#RootCalibWrapper.o: RootCalibWrapper.cc RootCalibWrapper.h Calib.h
#	$(CXX) $(FLAGS) -I. -c RootCalibWrapper.cc -o RootCalibWrapper.o

#Cint.cxx: Setup.h RootSetupWrapper.h Calib.h RootCalibWrapper.h Event.h Tile.h HGCROC.h Caen.h LinkDef.h
#	rootcint -v -f Cint.cxx -c RootSetupWrapper.h RootCalibWrapper.h Calib.h  Setup.h Event.h Tile.h HGCROC.h Caen.h LinkDef.h

Cint.cxx: Setup.h RootSetupWrapper.h Calib.h Event.h Tile.h HGCROC.h Caen.h TileSpectra.h TileTrend.h CalibSummary.h LinkDef.h
	rootcint -v -f Cint.cxx -c RootSetupWrapper.h Calib.h  Setup.h Event.h Tile.h HGCROC.h Caen.h TileSpectra.h TileTrend.h CalibSummary.h LinkDef.h

Cint.o:Cint.cxx
	$(CXX) $(FLAGS) -c Cint.cxx -o Cint.o

libLFHCAL.so: Cint.o Setup.o RootSetupWrapper.o Calib.o  Event.o Tile.o HGCROC.o Caen.o TileSpectra.o TileTrend.o CalibSummary.o
	$(LINKER) -o libLFHCAL.so Calib.o Setup.o RootSetupWrapper.o Event.o Tile.o HGCROC.o Caen.o TileSpectra.o TileTrend.o CalibSummary.o Cint.o `root-config --libs --cflags --ldflags` -I.

#libLFHCAL.so: Cint.o Setup.o RootSetupWrapper.o Calib.o RootCalibWrapper.o Event.o Tile.o HGCROC.o Caen.o 
#	$(LINKER) -o libLFHCAL.so Calib.o Setup.o RootSetupWrapper.o RootCalibWrapper.o Event.o Tile.o HGCROC.o Caen.o Cint.o `root-config --libs --cflags --ldflags` -I.

#libLFHCAL.so: Cint.o RootSetupWrapper.o RootCalibWrapper.o Event.o Tile.o HGCROC.o Caen.o 
#	$(LINKER) -o libLFHCAL.so RootSetupWrapper.o RootCalibWrapper.o Event.o Tile.o HGCROC.o Caen.o Cint.o `root-config --libs --cflags --ldflags` -I.

Caen.o: Caen.cc Caen.h
	$(CXX) $(FLAGS) -I. -c Caen.cc -o Caen.o

HGCROC.o: HGCROC.cc HGCROC.h
	$(CXX) $(FLAGS) -I. -c HGCROC.cc -o HGCROC.o

HGCROC_Convert.o: HGCROC_Convert.cc HGCROC_Convert.h
	$(CXX) $(FLAGS) -I. -c HGCROC_Convert.cc -o HGCROC_Convert.o

Tile.o: Tile.cc Tile.h Setup.h
	$(CXX) $(FLAGS) -I. -c Tile.cc -o Tile.o

TileSpectra.o: TileSpectra.cc TileSpectra.h Calib.h
	$(CXX) $(FLAGS) -I. -c TileSpectra.cc -o TileSpectra.o

TileTrend.o: TileTrend.cc TileTrend.h Calib.h
	$(CXX) $(FLAGS) -I. -c TileTrend.cc -o TileTrend.o

CalibSummary.o: CalibSummary.cc CalibSummary.h Calib.h
	$(CXX) $(FLAGS) -I. -c CalibSummary.cc -o CalibSummary.o

Event.o: Event.cc Event.h
	$(CXX) $(FLAGS) -I. -c Event.cc -o Event.o

DataPrep.o: DataPrep.cc
	$(CXX) $(FLAGS) -I. -c DataPrep.cc -o DataPrep.o

Convert.o: Convert.cc
	$(CXX) $(FLAGS) -I. -c Convert.cc -o Convert.o

Analyses.o: Analyses.cc Analyses.h PlottHelper.h CommonHelperFunctions.h
	$(CXX) $(FLAGS) -I. -c Analyses.cc -o Analyses.o

DataAna.o: DataAna.cc
	$(CXX) $(FLAGS) -I. -c DataAna.cc -o DataAna.o

DataAnalysis.o: DataAnalysis.cc DataAnalysis.h PlottHelper.h CommonHelperFunctions.h
	$(CXX) $(FLAGS) -I. -c DataAnalysis.cc -o DataAnalysis.o

Display.o: Display.cc
	$(CXX) $(FLAGS) -I. -c Display.cc -o Display.o

EventDisplay.o: EventDisplay.cc EventDisplay.h PlottHelper.h CommonHelperFunctions.h
	$(CXX) $(FLAGS) -I. -c EventDisplay.cc -o EventDisplay.o

Compare.o: Compare.cc
	$(CXX) $(FLAGS) -I. -c Compare.cc -o Compare.o

ComparisonCalib.o: ComparisonCalib.cc ComparisonCalib.h PlottHelper.h CommonHelperFunctions.h
	$(CXX) $(FLAGS) -I. -c ComparisonCalib.cc -o ComparisonCalib.o

waveform_fit_base.o: waveform_fitting/waveform_fit_base.cc waveform_fitting/waveform_fit_base.h
	$(CXX) $(FLAGS) -I. -c waveform_fitting/waveform_fit_base.cc -o waveform_fit_base.o

crystal_ball_fit.o: waveform_fitting/crystal_ball_fit.cc waveform_fitting/crystal_ball_fit.h
	$(CXX) $(FLAGS) -I. -c waveform_fitting/crystal_ball_fit.cc -o crystal_ball_fit.o

max_sample_fit.o: waveform_fitting/max_sample_fit.cc waveform_fitting/max_sample_fit.h
	$(CXX) $(FLAGS) -I. -c waveform_fitting/max_sample_fit.cc -o max_sample_fit.o

Convert: Event.o Setup.o RootSetupWrapper.o Calib.o Tile.o HGCROC.o Caen.o  TileSpectra.o libLFHCAL.so Convert.o Analyses.o HGCROC_Convert.o waveform_fit_base.o crystal_ball_fit.o max_sample_fit.o 
	$(CXX) Event.o Calib.o Setup.o RootSetupWrapper.o Tile.o HGCROC.o Caen.o TileSpectra.o Analyses.o Convert.o HGCROC_Convert.o waveform_fit_base.o crystal_ball_fit.o max_sample_fit.o $(FLAGS) -I. libLFHCAL.so -o Convert

DataPrep: Event.o Setup.o RootSetupWrapper.o Calib.o Tile.o HGCROC.o Caen.o  TileSpectra.o libLFHCAL.so DataPrep.o Analyses.o HGCROC_Convert.o waveform_fit_base.o crystal_ball_fit.o max_sample_fit.o 
	$(CXX) Event.o Calib.o Setup.o RootSetupWrapper.o Tile.o HGCROC.o Caen.o TileSpectra.o Analyses.o DataPrep.o HGCROC_Convert.o waveform_fit_base.o crystal_ball_fit.o max_sample_fit.o  $(FLAGS) -I. libLFHCAL.so -o DataPrep

HGCROC_Conversion: FLAGS += -DDECODE_HGCROC -Llib -lh2g_decode
HGCROC_Conversion: Convert

DataAna: Event.o Setup.o RootSetupWrapper.o Calib.o Tile.o HGCROC.o Caen.o  TileSpectra.o libLFHCAL.so DataAna.o DataAnalysis.o HGCROC_Convert.o waveform_fit_base.o crystal_ball_fit.o max_sample_fit.o 
	$(CXX) Event.o Calib.o Setup.o RootSetupWrapper.o Tile.o HGCROC.o Caen.o TileSpectra.o DataAna.o DataAnalysis.o HGCROC_Convert.o waveform_fit_base.o crystal_ball_fit.o max_sample_fit.o  $(FLAGS) -I. libLFHCAL.so -o DataAna

Display: Event.o Setup.o RootSetupWrapper.o Calib.o Tile.o HGCROC.o Caen.o  TileSpectra.o libLFHCAL.so Display.o EventDisplay.o HGCROC_Convert.o waveform_fit_base.o crystal_ball_fit.o max_sample_fit.o 
	$(CXX) Event.o Calib.o Setup.o RootSetupWrapper.o Tile.o HGCROC.o Caen.o TileSpectra.o Display.o EventDisplay.o HGCROC_Convert.o waveform_fit_base.o crystal_ball_fit.o max_sample_fit.o  $(FLAGS) -I. libLFHCAL.so -o Display

Compare: Event.o Setup.o RootSetupWrapper.o Calib.o Tile.o HGCROC.o Caen.o  TileSpectra.o TileTrend.o CalibSummary.o libLFHCAL.so Compare.o ComparisonCalib.o
	$(CXX) Event.o Calib.o Setup.o RootSetupWrapper.o Tile.o HGCROC.o Caen.o TileSpectra.o TileTrend.o CalibSummary.o ComparisonCalib.o Compare.o $(FLAGS) -I. libLFHCAL.so -o Compare

clean:
	rm -f *.o Cint.cxx Cint_* *.so ./DataPrep ./Compare ./Convert ./DataAna ./Display

cleanLib:
	rm -f *.so Cint*

default: DataPrep
