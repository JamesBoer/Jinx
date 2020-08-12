#!/bin/zsh

# Set to current working dir
DIR=$( cd "$( dirname "${(%):-%x}" )" && pwd )
cd "${DIR}"

./GenerateXcode.command

cd ..
if ! [ -e Build ]; then
    mkdir Build
fi
cd Build/
xcodebuild -project Jinx.xcodeproj -target ALL_BUILD -configuration Debug
xcodebuild -project Jinx.xcodeproj -target ALL_BUILD -configuration Release
cd ..
cd Bin/