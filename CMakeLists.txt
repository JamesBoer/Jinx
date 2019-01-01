
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.10)

# Add C# utilities for building tools
include(CSharpUtilities)

# Set project name
project(Jinx)

# Set folders based on file structure
function(jinx_auto_set_folders source_list)
	foreach(source IN LISTS ${source_list})
		get_filename_component(source_path "${source}" PATH)
		string(REPLACE "/" "\\" source_path_folder "${source_path}")
		source_group("${source_path_folder}" FILES "${source}")
	endforeach()
endfunction()

# Set folders manuallly
function(jinx_set_folders source_list folder_name)
	foreach(source IN LISTS source_list)
		source_group("\\${folder_name}" FILES "${source}")
	endforeach()
endfunction()

# Build a test or utility executable
function(jinx_build_executable project_name source_list is_linked is_startup requires_pthreads)

	# Create executable as project name
	add_executable(${project_name} ${source_list})

	# Set C++ 17 compiler flags
	set(CMAKE_CXX_STANDARD 17)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)
	
	# Set the executable as the default startup project
	if(is_startup)
		set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${project_name})
	endif()
	
	# Add Jinx library
	if(is_linked)
		target_link_libraries(${project_name} Jinx)
	endif()
	
	# Add pthreads
	if(requires_pthreads AND UNIX AND NOT APPLE)
		set(THREADS_PREFER_PTHREAD_FLAG ON)
		find_package(Threads REQUIRED)
		target_link_libraries(${project_name} Threads::Threads)
	endif()

	# Create IDE folder structure
	jinx_set_folders("${source_list}" Source)

endfunction()

# Enable IDE folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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
jinx_auto_set_folders(jinx_source_list)

# Check if Jinx is included via add_subdirectory.
get_directory_property(jinx_is_subproject PARENT_DIRECTORY)

# Only build the test suite and utilities if this is not a subproject
if(NOT jinx_is_subproject)

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
	jinx_build_executable(Features "Tests/Features/Main.cpp" TRUE FALSE FALSE)	
	jinx_build_executable(FuzzTests "Tests/FuzzTests/Main.cpp" TRUE FALSE TRUE)	
	jinx_build_executable(HeaderTest "${headertest_source_list}" FALSE FALSE FALSE)
	jinx_build_executable(PerfTest "Tests/PerfTest/Main.cpp" TRUE FALSE TRUE)
	jinx_build_executable(UnitTests "${unittests_source_list}" TRUE TRUE FALSE)
	if(MSVC)
		jinx_build_executable(CaseFoldGen "Utils/CaseFoldGen/Main.cpp" FALSE FALSE FALSE)
		add_subdirectory(Utils/JinxTools)		
		add_subdirectory(Utils/JinxPad)	
	endif()

endif()
