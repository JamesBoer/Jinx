call GenerateMSVC.cmd
cd ..
cd Build

MSBuild Jinx.sln /p:Configuration=Debug && (
  echo Debug build succeeded
) || (
  echo Debug build failed
  cd ..
  cd Bin
  EXIT /B 1
)

MSBuild Jinx.sln /p:Configuration=Release && (
  echo Release build succeeded
) || (
  echo Release build failed
  cd ..
  cd Bin
  EXIT /B 1
)

cd ..
cd Bin
EXIT /B 0
