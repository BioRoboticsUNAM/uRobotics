#PROJECT(uRobotics)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

#set the default path for built executables to the "bin" directory
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
#set the default path for built libraries to the "lib" directory
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

#set(Boost_INCLUDE_DIR /usr/local/src/boost_1_46_1)
#set(Boost_LIBRARY_DIR /usr/local/src/boost_1_46_1/stage/lib)
#find_package(Boost COMPONENTS system filesystem REQUIRED)
#include_directories(${Boost_INCLUDE_DIR})
#link_directories(${Boost_LIBRARY_DIR})

set(Boost_USE_STATIC_LIBS   ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
FIND_PACKAGE( Boost 1.40 COMPONENTS program_options system filesystem regex thread date_time REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})
file(GLOB_RECURSE headers "${CMAKE_CURRENT_SOURCE_DIR}/uRobotics/*.h*")
file(GLOB_RECURSE sources "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*")

ADD_LIBRARY( uRoboticsStatic STATIC ${sources} ${headers} )
TARGET_LINK_LIBRARIES( uRoboticsStatic ${Boost_LIBRARIES} )
SET_TARGET_PROPERTIES(uRoboticsStatic PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(uRoboticsStatic PROPERTIES OUTPUT_NAME "uRobotics")

ADD_LIBRARY( uRoboticsShared SHARED ${sources} ${headers} )
TARGET_LINK_LIBRARIES( uRoboticsShared ${Boost_LIBRARIES} )
SET_TARGET_PROPERTIES(uRoboticsShared PROPERTIES PREFIX "")
SET_TARGET_PROPERTIES(uRoboticsShared PROPERTIES OUTPUT_NAME "uRobotics")

file(GLOB_RECURSE test_headers "${CMAKE_CURRENT_SOURCE_DIR}/test/*.h*")
file(GLOB_RECURSE test_sources "${CMAKE_CURRENT_SOURCE_DIR}/test/*.c*")

ADD_EXECUTABLE( test ${test_sources} ${test_headers} )
ADD_DEPENDENCIES(test uRoboticsStatic)
TARGET_LINK_LIBRARIES( test ${Boost_LIBRARIES} ${Boost_REGEX_LIBRARY} ${LIBRARY_OUTPUT_PATH}/uRobotics.a )
