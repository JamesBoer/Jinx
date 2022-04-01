@echo off
cd ..
IF NOT EXIST Build (
echo Creating Build/ folder
mkdir Build 
)
cd Build/
cmake ../ -G "Visual Studio 17 2022" -A x64
cd ..
cd Bin
