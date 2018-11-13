
# Require minimum standard version of CMake
cmake_minimum_required (VERSION 3.5)

# Set C++ 17 compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable IDE folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set project name
project (Features)

set(
    source_list
    "Main.cpp"
)

# Create executable as project name
add_executable(${PROJECT_NAME} ${source_list})

# Set the executable as the default startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# Add Jinx library
add_subdirectory(../.. Jinx)
target_link_libraries(${PROJECT_NAME} Jinx)

# Create filters to represent folder structure
foreach(source IN LISTS source_list)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_folder "${source_path}")
    source_group("Source\\${source_path_folder}" FILES "${source}")
endforeach()