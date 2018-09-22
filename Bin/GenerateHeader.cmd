@echo off

rem Generate unified header file from all library source
Heady.exe --source "../Source" --output "../Include/Jinx.hpp"
