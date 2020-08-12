cd ..
if ! [ -e Build ]; then
    mkdir Build
fi
cd Build/

if ! [ -e Debug ]; then
    mkdir Debug
fi
cd Debug/
cmake ../../ -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" 
cd ..

if ! [ -e Release ]; then
    mkdir Release
fi
cd Release/
cmake ../../ -DCMAKE_BUILD_TYPE=Release -G "CodeBlocks - Unix Makefiles" 
cd ..

if ! [ -e RelWithDebInfo ]; then
    mkdir RelWithDebInfo
fi
cd RelWithDebInfo/
cmake ../../ -DCMAKE_BUILD_TYPE=RelWithDebInfo -G "CodeBlocks - Unix Makefiles" 
cd ..

if ! [ -e MinSizeRel ]; then
    mkdir MinSizeRel
fi
cd MinSizeRel/
cmake ../../ -DCMAKE_BUILD_TYPE=MinSizeRel -G "CodeBlocks - Unix Makefiles" 
cd ..

cd ..
