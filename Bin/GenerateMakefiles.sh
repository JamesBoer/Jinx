#!/bin/bash

# Generate Build folder if it doesn't exist and navigate there
cd ..
if ! [ -e Build ]; then
    mkdir Build
fi
cd Build/

# Build Debug makefiles
if ! [ -e Debug ]; then
    mkdir Debug
fi
cd Debug/
cmake ../../ -DCMAKE_BUILD_TYPE=Debug -G "Unix Makefiles" || exit 1 
cd ..

# Build Release makefiles
if ! [ -e Release ]; then
    mkdir Release
fi
cd Release/
cmake ../../ -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" || exit 1 
cd ..

# Build RelWithDebInfo makefiles
if ! [ -e RelWithDebInfo ]; then
    mkdir RelWithDebInfo
fi
cd RelWithDebInfo/
cmake ../../ -DCMAKE_BUILD_TYPE=RelWithDebInfo -G "Unix Makefiles" || exit 1 
cd ..

# Build MinSizeRel makefiles
if ! [ -e MinSizeRel ]; then
    mkdir MinSizeRel
fi
cd MinSizeRel/
cmake ../../ -DCMAKE_BUILD_TYPE=MinSizeRel -G "Unix Makefiles" || exit 1 
cd ..

cd ..
cd Bin/
