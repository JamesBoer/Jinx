#!/bin/bash

# Generate makefiles
./GenerateMakefiles.sh || exit 1

# Navigate to build folder
cd ..
cd Build/

# Build Debug and Release configurations
cd Debug/
make || exit 1
cd ..
cd Release
make || exit 1

cd ..
cd ..
cd Bin/

