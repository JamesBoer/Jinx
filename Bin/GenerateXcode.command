#!/bin/zsh

# Set to current working dir
DIR=$( cd "$( dirname "${(%):-%x}" )" && pwd )
cd "${DIR}"

cd ..
if ! [ -e Build ]; then
    mkdir Build
fi
cd Build/
cmake ../ -G Xcode
cd ..
cd Bin/
