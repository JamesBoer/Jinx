#!/bin/zsh

# Set to current working dir
DIR=$( cd "$( dirname "${(%):-%x}" )" && pwd )
cd "${DIR}"

cd ..

# Check if build folder exists, and if not, create it
if ! [ -e Build ]; then
    mkdir Build
fi

cd Build/

# Generate Xcode project using CMake
cmake ../ -G Xcode || exit 1

cd ..
cd Bin/
