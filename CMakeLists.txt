
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.5)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable IDE folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set project name
project (Jinx)

set(
    source_list
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

# Create library as project name
add_library(${PROJECT_NAME} ${source_list})

# Set warning level
if(MSVC)
	target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
	target_compile_options(${TARGET_NAME} PRIVATE -Wall -Wextra -pedantic -Werror)
endif()

# Create filters to represent folder structure
foreach(source IN LISTS source_list)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_folder "${source_path}")
    source_group("${source_path_folder}" FILES "${source}")
endforeach()