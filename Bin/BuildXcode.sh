cd ..
if ! [ -e Build ]; then
    mkdir Build
fi
cd Build/
cmake ../ -G Xcode
cd ..
