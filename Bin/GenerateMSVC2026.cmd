@echo off
cd ..
IF NOT EXIST Build (
echo Creating Build/ folder
mkdir Build 
)
cd Build/
cmake ../ -G "Visual Studio 18 2026" -A x64
cd ..
cd Bin
