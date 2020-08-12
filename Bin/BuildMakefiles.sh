#!/bin/bash

# Generate makefiles
./GenerateMakefiles.sh

# Navigate to build folder
cd ..
cd Build/

# Build Debug and Release configurations
cd Debug/
make
cd ..
cd Release
make

cd ..
cd ..
cd Bin/

