
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.12)

# Set project name
project(Jinx)

# Set folders based on file structure
function(auto_set_folders source_list)
	foreach(source IN LISTS "${source_list}")
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "/" "\\" source_path_folder "${source_path}")
		source_group("${source_path_folder}" FILES "${source}")
	endforeach()
endfunction()

# Set folders manuallly
function(set_folders source_list folder_name)
	foreach(source IN LISTS source_list)
		source_group("\\${folder_name}" FILES "${source}")
	endforeach()
endfunction()

# Build a test or utility executable
function(build_executable project_name source_list is_linked is_startup)

	# Create executable as project name
	add_executable(${project_name} ${source_list})

	# Set C++ 17 compiler flags
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)
	
	# Set the executable as the default startup project
	if (is_startup)
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Features)
	endif()
	
	# Add Jinx library
	if (is_linked)
		target_link_libraries(${project_name} Jinx)
	endif()
	
	# Create IDE folder structure
	set_folders("${source_list}" Source)

endfunction()

# Enable IDE folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Check if reproc is included via add_subdirectory.
get_directory_property(JINX_IS_SUBPROJECT PARENT_DIRECTORY)

set(
    jinx_source_list
    "Source/Jinx.h"
    "Source/JxBuffer.cpp"
    "Source/JxBuffer.h"
    "Source/JxCollection.cpp"
    "Source/JxCollection.h"
    "Source/JxCommon.cpp"
    "Source/JxCommon.h"
    "Source/JxConversion.cpp"
    "Source/JxConversion.h"
    "Source/JxFunctionDefinition.h"
    "Source/JxFunctionSignature.cpp"
    "Source/JxFunctionSignature.h"
    "Source/JxGuid.h"
    "Source/JxHash.cpp"
    "Source/JxHash.h"
    "Source/JxInternal.h"
    "Source/JxLexer.cpp"
    "Source/JxLexer.h"
    "Source/JxLibCore.cpp"
    "Source/JxLibCore.h"
    "Source/JxLibrary.cpp"
    "Source/JxLibrary.h"
    "Source/JxLogging.cpp"
    "Source/JxLogging.h"
    "Source/JxMemory.cpp"
    "Source/JxMemory.h"
    "Source/JxParser.cpp"
    "Source/JxParser.h"
    "Source/JxPropertyName.cpp"
    "Source/JxPropertyName.h"
    "Source/JxRuntime.cpp"
    "Source/JxRuntime.h"
    "Source/JxScript.cpp"
    "Source/JxScript.h"
    "Source/JxSerialize.cpp"
    "Source/JxSerialize.h"
    "Source/JxUnicode.cpp"
    "Source/JxUnicode.h"
    "Source/JxUnicodeCaseFolding.cpp"
    "Source/JxUnicodeCaseFolding.h"
    "Source/JxVariableStackFrame.cpp"
    "Source/JxVariableStackFrame.h"
    "Source/JxVariant.cpp"
    "Source/JxVariant.h"
)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Create library as project name
add_library(${PROJECT_NAME} ${jinx_source_list})

# Set warning level
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
	target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Create IDE folder structure
auto_set_folders(jinx_source_list)

# Only build the test suite and utilities if this is not a subproject
if(NOT JINX_IS_SUBPROJECT)

	# Default to static linking for standalone build - otherwise defer to parent
	#set(BUILD_SHARED_LIBS OFF)

	set(
		headertest_source_list
		"Tests/HeaderTest/HeaderTest.cpp"
		"Tests/HeaderTest/HeaderTest.h"
		"Tests/HeaderTest/Main.cpp"
	)
	set(
		unittests_source_list
		"Tests/UnitTests/catch.hpp"
		"Tests/UnitTests/Main.cpp"
		"Tests/UnitTests/TestCasts.cpp"
		"Tests/UnitTests/TestCollections.cpp"
		"Tests/UnitTests/TestErrors.cpp"
		"Tests/UnitTests/TestExpressions.cpp"
		"Tests/UnitTests/TestFunctions.cpp"
		"Tests/UnitTests/TestIfElse.cpp"
		"Tests/UnitTests/TestLibCore.cpp"
		"Tests/UnitTests/TestLibraries.cpp"
		"Tests/UnitTests/TestLoops.cpp"
		"Tests/UnitTests/TestNative.cpp"
		"Tests/UnitTests/TestStatements.cpp"
		"Tests/UnitTests/TestUnicode.cpp"
		"Tests/UnitTests/UnitTest.cpp"
		"Tests/UnitTests/UnitTest.h"
	)
	build_executable(Features "Tests/Features/Main.cpp" TRUE FALSE)	
	build_executable(FuzzTests "Tests/FuzzTests/Main.cpp" TRUE FALSE)	
	build_executable(HeaderTest "${headertest_source_list}" FALSE FALSE)
	build_executable(PerfTest "Tests/PerfTest/Main.cpp" TRUE FALSE)
	build_executable(UnitTests "${unittests_source_list}" TRUE TRUE)
	build_executable(CaseFoldGen "Utils/CaseFoldGen/Main.cpp" FALSE FALSE)

endif()