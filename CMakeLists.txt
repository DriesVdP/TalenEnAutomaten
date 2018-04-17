cmake_minimum_required(VERSION 3.9)
project(ToepassingsOPdracht)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Werror")
set(CMAKE_EXE_LINKER_FLAGS -pthread)

set(RELEASE_SOURCE_FILES TOmain.cpp State.cpp State.h Automaat_parser.cc Automaat_parser.h)

set(DEBUG_SOURCE_FILES TOTests.cpp TestsStateElimination.cpp State.cpp State.h Automaat_parser.cc Automaat_parser.h)

include_directories(gtest/include)

link_directories(gtest/lib)

add_executable(ToepassingsOpdracht ${RELEASE_SOURCE_FILES})

add_executable(ToTests ${DEBUG_SOURCE_FILES})