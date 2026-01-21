cmake_minimum_required(VERSION 3.12)
project(LFHCAL VERSION 1.0 LANGUAGES CXX)

# Attempt to extract -std=c++XX from root-config --cflags, default to 20 if not found
execute_process(
    COMMAND root-config --cflags
    OUTPUT_VARIABLE ROOT_CFLAGS_STD
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string(REGEX MATCH "-std=c\\+\\+[0-9]+" STD_FLAG "${ROOT_CFLAGS_STD}")
if(STD_FLAG)
    string(REGEX REPLACE "-std=c\\+\\+" "" CXX_STANDARD "${STD_FLAG}")
    set(CMAKE_CXX_STANDARD ${CXX_STANDARD})
else()
    set(CMAKE_CXX_STANDARD 20)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "Using C++ standard: ${CMAKE_CXX_STANDARD}")

# Set C++17 standard
set(CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find ROOT package
find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Tree Gpad Graf)
include(${ROOT_USE_FILE})

# Add common compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC -ansi -Wno-overloaded-virtual -Wno-long-long -fno-common")

# Platform-specific settings
if(UNIX AND NOT APPLE)
  # Linux
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-rpath='${ORIGIN}'")
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Build h2g_decode subproject
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/h2g_decode/CMakeLists.txt)
  # Set USE_ROOT to OFF for h2g_decode subproject
  set(USE_ROOT OFF CACHE BOOL "Disable ROOT for h2g_decode" FORCE)
  add_subdirectory(h2g_decode)
  set(H2G_DECODE_FOUND TRUE)
  add_compile_definitions(DECODE_HGCROC)
  # Add h2g_decode source directory to include paths
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/h2g_decode/src)
  message(STATUS "Building h2g_decode from source")
else()
  # Fallback to prebuilt library
  set(H2G_DECODE_FOUND FALSE)
  if(APPLE)
    set(H2G_DECODE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/libh2g_decode.dylib")
  elseif(UNIX)
    set(H2G_DECODE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/libh2g_decode.so")
  elseif(WIN32)
    set(H2G_DECODE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/lib/h2g_decode.dll")
  endif()

  if(EXISTS ${H2G_DECODE_LIBRARY})
    message(STATUS "Found h2g_decode library: ${H2G_DECODE_LIBRARY}")
    set(H2G_DECODE_FOUND TRUE)
    add_compile_definitions(DECODE_HGCROC)
    # Create imported target for the library
    add_library(h2g_decode SHARED IMPORTED)
    set_target_properties(h2g_decode PROPERTIES IMPORTED_LOCATION ${H2G_DECODE_LIBRARY})
    target_include_directories(h2g_decode INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/lib)
  else()
    message(STATUS "h2g_decode library not found, HGCROC decoding will be disabled")
  endif()
endif()

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
  AnaSummary.h
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
  AnaSummary.cc
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
if(H2G_DECODE_FOUND)
  target_compile_definitions(Convert PRIVATE DECODE_HGCROC)
  target_link_libraries(Convert h2g_decode)
endif()

# DataPrep executable
add_executable(HGCROCStudy HGCROCStudy.cc HGCROC_Waveform_Analysis.cc)
target_link_libraries(HGCROCStudy LFHCAL WaveformFitting ${ROOT_LIBRARIES})

# DataPrep executable
add_executable(DataPrep DataPrep.cc)
target_link_libraries(DataPrep LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES})
if(H2G_DECODE_FOUND)
  target_compile_definitions(DataPrep PRIVATE DECODE_HGCROC)
  target_link_libraries(DataPrep h2g_decode)
endif()

# DataAna executable
add_executable(DataAna DataAna.cc DataAnalysis.cc)
target_link_libraries(DataAna LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES})
if(H2G_DECODE_FOUND)
  target_compile_definitions(DataAna PRIVATE DECODE_HGCROC)
  target_link_libraries(DataAna h2g_decode)
endif()

# Display executable
add_executable(Display Display.cc EventDisplay.cc)
target_link_libraries(Display LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES})
if(H2G_DECODE_FOUND)
  target_compile_definitions(Display PRIVATE DECODE_HGCROC)
  target_link_libraries(Display h2g_decode)
endif()

# CompareCalib executable
add_executable(CompareCalib CompareCalib.cc ComparisonCalib.cc)
target_link_libraries(CompareCalib LFHCAL ${ROOT_LIBRARIES})

# CompareAna executable
add_executable(CompareAna CompareAna.cc ComparisonAna.cc)
target_link_libraries(CompareAna LFHCAL ${ROOT_LIBRARIES})

# HGCROC_Conversion target (modified Convert with additional flags)
if(H2G_DECODE_FOUND)
  add_executable(HGCROC_Conversion Convert.cc)
  target_compile_definitions(HGCROC_Conversion PRIVATE DECODE_HGCROC)
  target_link_libraries(HGCROC_Conversion LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES} h2g_decode)
else()
  message(WARNING "HGCROC_Conversion target will not be built - h2g_decode library not found")
endif()

# Calib sample parser
if(H2G_DECODE_FOUND)
  add_executable(ParseCalibSamples ParseCalibSamples.cc CalibSampleParser.cc)
  target_compile_definitions(ParseCalibSamples PRIVATE DECODE_HGCROC)
  target_link_libraries(ParseCalibSamples LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES} h2g_decode)
else()
  message(WARNING "HGCROC calib sample parser will not be built - h2g_decode library not found")
endif()

# Compare HGCROC calib samples
if(H2G_DECODE_FOUND)
  add_executable(CompareHGCROCCalib CompareParsedHGCROC.cc CompareHGCROCCalib.cc)
  target_compile_definitions(CompareHGCROCCalib PRIVATE DECODE_HGCROC)
  target_link_libraries(CompareHGCROCCalib LFHCAL Analysis WaveformFitting ${ROOT_LIBRARIES} h2g_decode)
else()
  message(WARNING "HGCROC calib sample parser will not be built - h2g_decode library not found")
endif()


# Set default build target
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT DataPrep)

