#!/bin/zsh

# Set to current working dir
DIR=$( cd "$( dirname "${(%):-%x}" )" && pwd )
cd "${DIR}"

# Generate Xcode project
./GenerateXcode.command || exit 1

cd ..
cd Build/

# Build Debug and Release configurations of the project
xcodebuild -project Jinx.xcodeproj -target ALL_BUILD -configuration Debug || exit 1
xcodebuild -project Jinx.xcodeproj -target ALL_BUILD -configuration Release || exit 1

cd ..
cd Bin/