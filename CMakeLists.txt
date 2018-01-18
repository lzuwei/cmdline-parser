project(cmdline-parser)

cmake_minimum_required(VERSION 3.1)
set (CMAKE_CXX_STANDARD 11)

# ============================================================================ #
# Where to output binaries and libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")

# CXX Flags
if(CMAKE_BUILD_TYPE MATCHES "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE MATCHES "Release")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_RELEASE}")
else()
set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG}")
endif(CMAKE_BUILD_TYPE MATCHES "Debug")

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "${CMAKE_CXX_FLAGS}")

# Include dependency headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Enable Testing
enable_testing()

# Add Sub Directories
add_subdirectory(examples)
