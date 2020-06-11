# $Id: CMakeLists.txt Baranyai Dávid | 2018.06.05 18:52:01 $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(root2SQL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#----------------------------------------------------------------------------
# Find ROOT (required package)
#
find_package(ROOT REQUIRED)
#----------------------------------------------------------------------------
# Setup ROOT include directories and compile definitions
#
include(${ROOT_USE_FILE})

# Setup include directory for this project
#
include_directories(${PROJECT_SOURCE_DIR}/include ${ROOT_INCLUDE_DIRS})

set(ROOT_LIBRARIES -L${ROOT_LIBRARY_DIR} -lCore -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/source/*.cpp)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hpp)

#----------------------------------------------------------------------------
# Add the executable
#
add_executable(root2SQL root2SQL.cpp ${sources} ${headers})
target_link_libraries(root2SQL ${ROOT_LIBRARIES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # using Clang
    target_link_libraries(root2SQL -lc++)
else()
    #any other compiler
    target_link_libraries(root2SQL stdc++fs)
endif()
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS root2SQL DESTINATION bin)