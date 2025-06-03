#!/bin/bash

# Build script for LFHCAL project

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
echo "Configuring project..."
cmake ..

# Build
echo "Building project..."
cmake --build . -j $(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 2)

echo "Done. Executables are in the build directory."
