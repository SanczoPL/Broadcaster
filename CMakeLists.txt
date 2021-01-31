cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
project(Broadcaster
  VERSION 0.0.1
  DESCRIPTION "Broadcaster"
  LANGUAGES C CXX)

set(TARGET_NAME "Broadcaster")
set(DEFAULT_BUILD_TYPE "Release")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_COMMON}")

set(CMAKE_AUTOMOC ON)

if (WIN32)
	list(APPEND CMAKE_PREFIX_PATH "E:\\Qt\\5.12.8\\msvc2017_64")
endif (WIN32)

find_package(IncludeSpdlog)
add_subdirectory(QtCommunicatorPlugin)

find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Threads REQUIRED)


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified")
    set(CMAKE_BUILD_TYPE ${DEFAULT_BUILD_TYPE} CACHE STRING "Choose the type of build." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(SRC
  src/broadcaster.cc

)

set(HEADERS
  include/broadcaster.h
)

add_library(${PROJECT_NAME} STATIC
    ${SRC}
    ${HEADERS}
)

target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    PRIVATE src)

target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Threads::Threads)
target_link_libraries(${PROJECT_NAME} IncludeSpdlog QtCommunicatorPlugin)
