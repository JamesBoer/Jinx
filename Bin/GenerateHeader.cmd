@echo off

rem Generate unified header file from all library source
Heady.exe --define JINX_HEADER_ONLY --source "../Source" --output "../Include/Jinx.hpp"
