call GenerateMSVC.cmd
cd ..
cd Build
MSBuild Jinx.sln /p:Configuration=Debug
MSBuild Jinx.sln /p:Configuration=Release
cd ..
cd Bin
