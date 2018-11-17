cd ..
mkdir Build
cd Build/

# build main Jinx library
mkdir Jinx
cd Jinx/
cmake ../../ -G Xcode
cd ..

# build features test app
mkdir Features
cd Features/
cmake ../../Tests/Features/ -G "CodeBlocks - Unix Makefiles"
cd ..

# build fuzz test app
mkdir FuzzTests
cd FuzzTests/
cmake ../../Tests/FuzzTests/ -G "CodeBlocks - Unix Makefiles"
cd ..

# build header test app
mkdir HeaderTest
cd HeaderTest/
cmake ../../Tests/HeaderTest/ -G "CodeBlocks - Unix Makefiles"
cd ..

# build perf test app
mkdir PerfTest
cd PerfTest/
cmake ../../Tests/PerfTest/ -G "CodeBlocks - Unix Makefiles"
cd ..

# build unit tests app
mkdir UnitTests
cd UnitTests/
cmake ../../Tests/UnitTests/ -G "CodeBlocks - Unix Makefiles"
cd ..

cd ..
