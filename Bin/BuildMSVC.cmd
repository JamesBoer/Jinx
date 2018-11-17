cd ..
mkdir Build
cd Build/

rem build main Jinx library
mkdir Jinx
cd Jinx/
cmake ../../ -G "Visual Studio 15 2017" -A x64
cd ..

rem build features test app
mkdir Features
cd Features/
cmake ../../Tests/Features/ -G "Visual Studio 15 2017" -A x64
cd ..

rem build fuzz test app
mkdir FuzzTests
cd FuzzTests/
cmake ../../Tests/FuzzTests/ -G "Visual Studio 15 2017" -A x64
cd ..

rem build header test app
mkdir HeaderTest
cd HeaderTest/
cmake ../../Tests/HeaderTest/ -G "Visual Studio 15 2017" -A x64
cd ..

rem build perf test app
mkdir PerfTest
cd PerfTest/
cmake ../../Tests/PerfTest/ -G "Visual Studio 15 2017" -A x64
cd ..

rem build unit tests app
mkdir UnitTests
cd UnitTests/
cmake ../../Tests/UnitTests/ -G "Visual Studio 15 2017" -A x64
cd ..

cd ..
pause
