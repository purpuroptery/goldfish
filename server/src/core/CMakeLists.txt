cmake_minimum_required(VERSION 3.10)

# Set the project name
project(GolfAtHome)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add the executable
add_executable(generator 
    generators/main.cpp 
    generators/AbstractPrecomputeGenerator.cpp
    lib/RP/RPGolWindSet.cpp
    lib/Sp2/Sp2GolConfig.cpp
    lib/RP/RPUtlRandom.cpp
    util_zlib.cpp
)

add_executable(finder
    finders/main.cpp
    WindArgParser.cpp
    lib/RP/RPUtlRandom.cpp 
    lib/RP/RPGolConfig.cpp 
    lib/RP/RPGolWindSet.cpp
    lib/Sp2/Sp2GolConfig.cpp
    util_zlib.cpp
)

# Include debug symbols
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Include directories (if any)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/lib)

# Find and link zlib
find_package(ZLIB REQUIRED)
if (ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    target_link_libraries(generator ${ZLIB_LIBRARIES})
    target_link_libraries(finder ${ZLIB_LIBRARIES})
endif()
