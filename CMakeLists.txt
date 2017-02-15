cmake_minimum_required(VERSION 3.5)
project(Orbit)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Ofast -march=native")

find_package(Armadillo REQUIRED)

set(SOURCE_FILES main.cpp Satellite.h Planet.h Universe.h)

include_directories(${ARMADILLO_INCLUDE_DIRS})


add_executable(Orbit ${SOURCE_FILES})
target_link_libraries(Orbit ${ARMADILLO_LIBRARIES})