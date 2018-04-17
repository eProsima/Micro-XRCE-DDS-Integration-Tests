# Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

###############################################################################
# CMake build rules for micro RTPS integration test
###############################################################################
cmake_minimum_required(VERSION 2.8.12)

set(IS_TOP_LEVEL TRUE)
if(PROJECT_SOURCE_DIR)
    set(IS_TOP_LEVEL FALSE)
endif()

# Set CMAKE_BUILD_TYPE to Release by default.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Release' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

project(micrortps_itest)

set(EPROSIMA_BUILD_TESTS ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSERIALIZATION_LOGS")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DMESSAGE_LOGS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSERIALIZATION_LOGS")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DMESSAGE_LOGS")

###############################################################################
# eProsima build options
###############################################################################
option(EPROSIMA_BUILD "Activate internal building" OFF)

###############################################################################
# Installation paths
###############################################################################
set(BIN_INSTALL_DIR bin/ CACHE PATH "Installation directory for binaries")
set(INCLUDE_INSTALL_DIR include/ CACHE PATH "Installation directory for C headers")
set(LIB_INSTALL_DIR lib/ CACHE PATH "Installation directory for libraries")
set(DATA_INSTALL_DIR share/ CACHE PATH "Installation directory for data")
if(WIN32)
    set(LICENSE_INSTALL_DIR . CACHE PATH "Installation directory for licenses")
else()
    set(LICENSE_INSTALL_DIR ${DATA_INSTALL_DIR}/${PROJECT_NAME} CACHE PATH "Installation directory for licenses")
endif()

###############################################################################
# Compile
###############################################################################
# Set C++11
include(CheckCXXCompilerFlag)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_CLANG OR
    CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    check_cxx_compiler_flag(--std=c++11 SUPPORTS_CXX11)
    if(SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS}")
    else()
        message(FATAL_ERROR "Compiler doesn't support C++11")
    endif()
endif()

###############################################################################
# Warning level
###############################################################################
if(MSVC OR MSVC_IDE)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4710 /wd4996")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4710 /wd4996 /wd4820 /wd4255 /wd4668")
    # Disabled warnings:
    # 4710 - Function printf not inlined.
    # 4996 - Function sprintf has undefined behavior.
    # 4820 - Padding in structures.
    # 4255 - Lack of void parameter in C functions.
    # 4668 - Not defined preprocessor macro in Windows libraries.

    if(EPROSIMA_EXTRA_CMAKE_CXX_FLAGS)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EPROSIMA_EXTRA_CMAKE_CXX_FLAGS}")
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -Wno-unknown-pragmas -std=c++11")
endif()

###############################################################################
# Load external eProsima projects.
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/dev/eprosima_libraries.cmake)
option(THIRDPARTY "Activate the build of thirdparties" OFF)
option(UPDATE_SUBMODULES "Update to the latest" OFF)

if(EPROSIMA_BUILD)
    set(THIRDPARTY ON)
endif()

set(AGENT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/agent)
set(CLIENT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/thirdparty/client)

find_eprosima_package(agent)
find_eprosima_package(client)

get_directory_property(CLIENT_BINARY_DIR DIRECTORY ${CLIENT_SOURCE_DIR} DEFINITION PROJECT_BINARY_DIR)

###############################################################################
# Testing
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/dev/gtest.cmake)
check_gtest()
if(GTEST_FOUND)
    enable_testing()
    include(CTest)
    add_subdirectory(test)
endif()
