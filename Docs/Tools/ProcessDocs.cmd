@echo off

rem Add Courier as a backup font to Consolas, which may not be available on all systems
powershell -Command "(gc ../LibCore.htm) -replace 'Consolas;', 'Consolas, Hack, Inconsolata, Monaco, monospace;' | Out-File ../LibCore.htm"
powershell -Command "(gc ../Tutorial.htm) -replace 'Consolas;', 'Consolas, Hack, Inconsolata, Monaco, monospace;' | Out-File ../Tutorial.htm"
powershell -Command "(gc ../Performance.htm) -replace 'Consolas;', 'Consolas, Hack, Inconsolata, Monaco, monospace;' | Out-File ../Performance.htm"