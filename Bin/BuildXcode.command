#!/bin/bash

# Set to current working dir
DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
cd "${DIR}"

cd ..
if ! [ -e Build ]; then
    mkdir Build
fi
cd Build/
cmake ../ -G Xcode
cd ..
