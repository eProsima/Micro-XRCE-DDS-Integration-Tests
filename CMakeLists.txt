# Copyright 2019 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

cmake_minimum_required(VERSION 3.5)

set(CLIENT_BRANCH "develop" CACHE STRING "uClient's branch to be tested.")
set(AGENT_BRANCH "develop" CACHE STRING "uAgent's branch to be tested.")

option(UTEST_SUPERBUILD "Active super build" ON)

if(UTEST_SUPERBUILD)
    project(utest_superbuild NONE)
    include(cmake/SuperBuild.cmake)
    return()
else()
    project(microxrcedds_itest LANGUAGES C CXX)
endif()

set(CMAKE_PREFIX_PATH "${CLIENT_INSTALL_DIR}")
find_package(microcdr REQUIRED)
find_package(microxrcedds_client REQUIRED)

set(CMAKE_PREFIX_PATH "${AGENT_INSTALL_DIR}")
find_package(fastcdr REQUIRED)
find_package(microxrcedds_agent REQUIRED)

###############################################################################
# Testing
###############################################################################
include(${PROJECT_SOURCE_DIR}/cmake/common/gtest.cmake)
check_gtest()
if(GTEST_FOUND)
    enable_testing()
    include(CTest)

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_subdirectory(test/cross_serialization)
    endif()

    add_subdirectory(test/interaction_client)
    add_subdirectory(test/client_agent)
    add_subdirectory(test/publisher_subscriber)

    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        add_subdirectory(test/discovery)
    endif()

    if(EPROSIMA_BUILD)
        add_subdirectory(test/shapes_demo)
    endif()
else()
    message(FATAL_ERROR "This meta-project needs gtest")
endif()
