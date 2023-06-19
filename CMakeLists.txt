cmake_minimum_required(VERSION 3.25)
project(AmplifierUDPDoS)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB_RECURSE SOURCE_FILES ${SOURCE_DIR}/*.cpp)

add_executable(AmplifierUDPDoS ${SOURCE_FILES})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -Weffc++")
