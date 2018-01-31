cmake_minimum_required(VERSION 3.5)
project(mazetool)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        mazetool.cpp
        sha1.cpp
        sha1.h
        )
add_executable(mazetool ${SOURCE_FILES})
