call GenerateMSVC.cmd
cd ..
cd Build

MSBuild Jinx.sln /p:Configuration=Debug && (
  echo Debug build succeeded
) || (
  echo Debug build failed
  goto FINAL
)

MSBuild Jinx.sln /p:Configuration=Release && (
  echo Release build succeeded
) || (
  echo Release build failed
  goto FINAL
)

:FINAL
cd ..
cd Bin
EXIT /B %ERRORLEVEL%
