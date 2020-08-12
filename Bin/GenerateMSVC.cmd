@echo off
cd ..
IF NOT EXIST Build (
echo Creating Build/ folder
mkdir Build 
)
cd Build/
cmake ../ -G "Visual Studio 16 2019" -A x64
cd ..
cd Bin
