cmake_minimum_required(VERSION 3.5)
project(Orbit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Ofast -march=native")

find_package(Armadillo REQUIRED)

set(SOURCE_FILES_gphysics
        gphysics/Satellite.h
        gphysics/Planet.h
        gphysics/Universe.h
        gphysics/Mission.h)
include_directories(${ARMADILLO_INCLUDE_DIRS})
add_library(gphysics ${SOURCE_FILES_gphysics})
set_target_properties(gphysics PROPERTIES LINKER_LANGUAGE CXX)

set(SOURCE_FILES_Optimization
        Optimization/Cmaes.h
        Optimization/ParallelDriver.h)

include_directories(${ARMADILLO_INCLUDE_DIRS})
add_library(Optimization ${SOURCE_FILES_Optimization})
set_target_properties(Optimization PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(Optimization -larmadillo -lpthread)

add_executable(Orbit main.cpp)
target_link_libraries(Orbit gphysics Optimization)

add_executable(OptimizationTesting Testing/OptimizationTesting.cpp)
target_link_libraries(OptimizationTesting Optimization)

