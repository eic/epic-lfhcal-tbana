cmake_minimum_required(VERSION 3.12)
project(LFHCAL VERSION 1.0 LANGUAGES CXX)

# Set C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find ROOT package
find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Tree Gpad Graf)
include(${ROOT_USE_FILE})

# Add common compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC -ansi -Wno-overloaded-virtual -Wno-long-long -fno-common -pedantic -Wall")

# Platform-specific settings
if(UNIX AND NOT APPLE)
  # Linux
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath='${ORIGIN}'")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Generate ROOT dictionary
set(LINKDEF_FILE ${CMAKE_CURRENT_SOURCE_DIR}/LinkDef.h)
set(ROOT_HEADERS
  Setup.h
  RootSetupWrapper.h
  Calib.h
  Event.h
  Tile.h
  HGCROC.h
  Caen.h
  TileSpectra.h
  TileTrend.h
  CalibSummary.h
)

# Generate ROOT dictionary
root_generate_dictionary(Cint ${ROOT_HEADERS} LINKDEF ${LINKDEF_FILE})

# Common library source files
set(LIB_SOURCES
  Setup.cc
  RootSetupWrapper.cc
  Calib.cc
  Event.cc
  Tile.cc
  HGCROC.cc
  Caen.cc
  TileSpectra.cc
  TileTrend.cc
  CalibSummary.cc
)

# Create the LFHCAL shared library
add_library(LFHCAL SHARED 
  ${LIB_SOURCES}
  Cint.cxx
)

# Link against ROOT libraries
target_link_libraries(LFHCAL ${ROOT_LIBRARIES})

# Waveform fitting library
add_library(WaveformFitting STATIC
  waveform_fitting/waveform_fit_base.cc
  waveform_fitting/crystal_ball_fit.cc
  waveform_fitting/max_sample_fit.cc
)
target_include_directories(WaveformFitting PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(WaveformFitting ${ROOT_LIBRARIES})

# Analysis library
add_library(Analysis STATIC
  Analyses.cc
  HGCROC_Convert.cc
)
target_include_directories(Analysis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Analysis ${ROOT_LIBRARIES})

# Convert executable
add_executable(Convert Convert.cc)
target_link_libraries(Convert LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES})

# DataPrep executable
add_executable(DataPrep DataPrep.cc)
target_link_libraries(DataPrep LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES})

# DataAna executable
add_executable(DataAna DataAna.cc DataAnalysis.cc)
target_link_libraries(DataAna LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES})

# Display executable
add_executable(Display Display.cc EventDisplay.cc)
target_link_libraries(Display LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES})

# Compare executable
add_executable(Compare Compare.cc ComparisonCalib.cc)
target_link_libraries(Compare LFHCAL ${ROOT_LIBRARIES})

# HGCROC_Conversion target (modified Convert with additional flags)
add_executable(HGCROC_Conversion Convert.cc)
target_compile_definitions(HGCROC_Conversion PRIVATE DECODE_HGCROC)
target_link_directories(HGCROC_Conversion PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/lib)
target_link_libraries(HGCROC_Conversion LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES} h2g_decode)

# Set default build target
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT DataPrep)

# Install targets
install(TARGETS LFHCAL Convert DataPrep DataAna Display Compare HGCROC_Conversion
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)
