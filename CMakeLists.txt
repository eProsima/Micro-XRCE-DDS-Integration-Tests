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

###############################################################################
# Product information
###############################################################################
if(CMAKE_VERSION VERSION_LESS 3.0)
    project(micrortps_itest C CXX)
else()
    cmake_policy(SET CMP0048 NEW)
    project(micrortps_itest LANGUAGES C CXX)
endif()

###############################################################################
# eProsima build options
###############################################################################
option(EPROSIMA_BUILD "Activate internal building" OFF)
option(EPROSIMA_BUILD_TESTS "Activate internal building" OFF)
option(THIRDPARTY "Activate the build of thirdparties" OFF)
option(VERBOSE "Use verbose output" OFF)

if(EPROSIMA_BUILD)
    set(THIRDPARTY ON)
    set(EPROSIMA_BUILD_TESTS ON)
endif()

###############################################################################
# Load external eProsima projects.
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/dev/eprosima_libraries.cmake)
set(CLIENT_BRANCH_ENV_ $ENV{CLIENT_BRANCH})
if(CLIENT_BRANCH_ENV_)
    set(CLIENT_BRANCH "BRANCH" "${CLIENT_BRANCH_ENV_}")
else()
    set(CLIENT_BRANCH "BRANCH" "develop")
endif()
set(CLIENT_URL_ENV_ $ENV{CLIENT_URL})
if(CLIENT_URL_ENV_)
    set(CLIENT_URL "REPO_URL" "${CLIENT_URL_ENV_}")
else()
    set(CLIENT_URL "REPO_URL" "git@gitlab.sambaserver.eprosima.com:eProsima/micrortps-client.git")
endif()
set(AGENT_BRANCH_ENV_ $ENV{AGENT_BRANCH})
if(AGENT_BRANCH_ENV_)
    set(AGENT_BRANCH "BRANCH" "${AGENT_BRANCH_ENV_}")
else()
    set(AGENT_BRANCH "BRANCH" "develop")
endif()
set(AGENT_URL_ENV_ $ENV{AGENT_URL})
if(AGENT_URL_ENV_)
    set(AGENT_URL "REPO_URL" "${AGENT_URL_ENV_}")
else()
    set(AGENT_URL "REPO_URL" "git@gitlab.sambaserver.eprosima.com:eProsima/micrortps-agent.git")
endif()
eprosima_clone_package(agent ${AGENT_URL} ${AGENT_BRANCH} REQUIRED)
eprosima_clone_package(client ${CLIENT_URL} ${CLIENT_BRANCH} REQUIRED)

###############################################################################
# Set flags
###############################################################################
if(MSVC OR MSVC_IDE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4700 /wd4996 /wd4820 /wd4255 /wd4668")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fstrict-aliasing -Wall -Wextra -Wcast-align")
endif()

###############################################################################
# Testing
###############################################################################
if(EPROSIMA_BUILD_TESTS)
    include(${PROJECT_SOURCE_DIR}/cmake/common/gtest.cmake)
    check_gtest()
    if(GTEST_FOUND)
        enable_testing()
        include(CTest)

        add_subdirectory(${PROJECT_BINARY_DIR}/client_clone/test/integration/interaction)
        add_subdirectory(${PROJECT_BINARY_DIR}/client_clone/test/integration/cross_serialization)
        add_subdirectory(${PROJECT_BINARY_DIR}/agent_clone/test/integration/cross_serialization)
        add_subdirectory(test/interaction)
        add_subdirectory(test/cross_serialization)

    endif()
endif()
