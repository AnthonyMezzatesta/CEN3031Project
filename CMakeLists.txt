cmake_minimum_required(VERSION 3.15)
project(CyclePlanner VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SQLITE3 REQUIRED sqlite3)
find_package(SFML 2.5 COMPONENTS system window graphics network audio REQUIRED)

# Include directories
include_directories(include)
include_directories(${SQLITE3_INCLUDE_DIRS})
include_directories(${SFML_INCLUDE_DIRS})

# Source files
set(SOURCES
        src/TaskManager/Task.cpp
        src/TaskManager/TaskManager.cpp
)

# Executables
add_executable(TaskManager ${SOURCES} src/TaskManager/main.cpp)
add_executable(Test_SFML src/Kanban/main.cpp)

# Link libraries
target_link_libraries(TaskManager ${SQLITE3_LIBRARIES})
target_compile_options(TaskManager PRIVATE ${SQLITE3_CFLAGS_OTHER})
target_link_libraries(Test_SFML sfml-system sfml-window sfml-graphics sfml-audio sfml-network)

# Set output directory
set_target_properties(TaskManager PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
set_target_properties(Test_SFML PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Create build type options
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Print build information
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ standard: ${CMAKE_CXX_STANDARD}")
