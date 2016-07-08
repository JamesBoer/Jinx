@echo off

rem Generate API documents from source using Doxygen
doxygen Jinx.cfg

rem Add Courier as a backup font to Consolas, which may not be available on all systems
powershell -Command "(gc ../Overview.htm) -replace 'Consolas;', 'Consolas,Courier;' | Out-File ../Overview.htm"
powershell -Command "(gc ../LibCore.htm) -replace 'Consolas;', 'Consolas,Courier;' | Out-File ../LibCore.htm"
powershell -Command "(gc ../Tutorial.htm) -replace 'Consolas;', 'Consolas,Courier;' | Out-File ../Tutorial.htm"