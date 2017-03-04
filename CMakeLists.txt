cmake_minimum_required (VERSION 2.6)

#Use C++11
set(CMAKE_CXX_STANDARD 11)

set(PROJ_NAME metronome)
project(${PROJ_NAME})

set(SOURCE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/metronome.cpp
	)

set(HEADERS
	)

#include_directories(${ZMQ_INCLUDE_DIRS})


add_executable(${PROJ_NAME} ${SOURCE} ${HEADERS})


