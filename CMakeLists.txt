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

cmake_minimum_required(VERSION 3.5)

project(microxrcedds_itest LANGUAGES C CXX)

set(CLIENT_BRANCH "develop" CACHE STRING "uClient's branch to be tested.")
set(AGENT_BRANCH "develop" CACHE STRING "uAgent's branch to be tested.")

include(ExternalProject)

ExternalProject_Add(microxrcedds_agent
    PREFIX
        ${PROJECT_BINARY_DIR}/microxrcedds_agent
    GIT_REPOSITORY
        https://github.com/eProsima/Micro-XRCE-DDS-Agent.git
    GIT_TAG
        origin/${AGENT_BRANCH}
    CMAKE_ARGS
        -DTHIRDPARTY=ON
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DEPROSIMA_BUILD_TESTS=ON
        -DBUILD_SHARED_LIBS=ON
    )

ExternalProject_Add(microxrcedds_client
    PREFIX
        ${PROJECT_BINARY_DIR}/microxrcedds_client
    GIT_REPOSITORY
        https://github.com/eProsima/Micro-XRCE-DDS-Client.git
    GIT_TAG
        origin/${CLIENT_BRANCH}
    CMAKE_ARGS
        -DTHIRDPARTY=ON
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DEPROSIMA_BUILD_TESTS=ON
        -DBUILD_SHARED_LIBS=ON
    )

ExternalProject_Get_Property(microxrcedds_client INSTALL_DIR)
set(CLIENT_INSTALL_DIR ${INSTALL_DIR})
set(CLIENT_LIBRARY ${CMAKE_SHARED_LIBRARY_PREFIX}microxrcedds_client${CMAKE_SHARED_LIBRARY_SUFFIX})

ExternalProject_Get_Property(microxrcedds_agent INSTALL_DIR)
set(AGENT_INSTALL_DIR ${INSTALL_DIR})
set(AGENT_LIBRARY ${CMAKE_SHARED_LIBRARY_PREFIX}microxrcedds_agent${CMAKE_SHARED_LIBRARY_SUFFIX})

add_executable(${PROJECT_NAME} main.cpp)
add_dependencies(${PROJECT_NAME} microxrcedds_client microxrcedds_agent)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${CLIENT_INSTALL_DIR}/lib/${CLIENT_LIBRARY}
        ${AGENT_INSTALL_DIR}/lib/${AGENT_LIBRARY}
    )
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${CLIENT_INSTALL_DIR}/include
        ${AGENT_INSTALL_DIR}/include
    )

################################################################################
## Testing
################################################################################
#include(${PROJECT_SOURCE_DIR}/cmake/common/gtest.cmake)
#check_gtest()
#if(GTEST_FOUND)
#    enable_testing()
#    include(CTest)
#
#    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#        add_subdirectory(${PROJECT_BINARY_DIR}/client_clone/test/integration/cross_serialization)
#        add_subdirectory(${PROJECT_BINARY_DIR}/agent_clone/test/integration/cross_serialization)
#        add_subdirectory(test/cross_serialization)
#    endif()
#
#    add_subdirectory(${PROJECT_BINARY_DIR}/client_clone/test/integration/interaction)
#    add_subdirectory(test/client_agent)
#    add_subdirectory(test/publisher_subscriber)
#
#    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#        add_subdirectory(test/discovery)
#    endif()
#
#    if(EPROSIMA_BUILD)
#        add_subdirectory(test/shapes_demo)
#    endif()
#else()
#    message(FATAL_ERROR "This meta-project needs gtest")
#endif()


#ExternalProject_Add(microxrcedds_agent
#    PREFIX ${PROJECT_BINARY_DIR}/microxrcedds_agent
#    GIT_REPOSITORY https://github.com/eProsima/Micro-XRCE-DDS-Agent.git
#    GIT_TAG master
#    CMAKE_ARGS -DTHIRDPARTY=ON
#    INSTALL_DIR ${PROJECT_BINARY_DIR}/install
#    )
#
#ExternalProject_Get_Property(microxrcedds_agent INSTALL_DIR)
#include_directories(${INSTALL_DIR}/include)
#
##message(FATAL_ERROR "${INSTALL_DIR}")
#
#add_executable(${PROJECT_NAME} main.cpp)
#target_link_libraries(${PROJECT_NAME} ${INSTALL_DIR}/lib/microxrcedds_agent.so)


################################################################################
## CMake build rules for micro XRCE-DDS integration test
################################################################################
#cmake_minimum_required(VERSION 2.8.12)
#
#set(IS_TOP_LEVEL TRUE)
#if(PROJECT_SOURCE_DIR)
#    set(IS_TOP_LEVEL FALSE)
#endif()
#
## Set CMAKE_BUILD_TYPE to Release by default.
#if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
#    message(STATUS "Setting build type to 'Release' as none was specified.")
#    set(CMAKE_BUILD_TYPE Release CACHE STRING
#        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
#        FORCE)
#    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
#endif()
#
################################################################################
## Product information
################################################################################
#if(CMAKE_VERSION VERSION_LESS 3.0)
#    project(microxrcedds_itest C CXX)
#else()
#    cmake_policy(SET CMP0048 NEW)
#    project(microxrcedds_itest LANGUAGES C CXX)
#endif()
#
################################################################################
## Check MSVC architecture
################################################################################
#include(${PROJECT_SOURCE_DIR}/cmake/common/check_configuration.cmake)
#if(MSVC OR MSVC_IDE)
#    check_msvc_arch()
#endif()
#
################################################################################
## Load external eProsima projects.
################################################################################
#include(${PROJECT_SOURCE_DIR}/cmake/dev/eprosima_libraries.cmake)
#set(CLIENT_URL "git@github.com:eProsima/Micro-XRCE-DDS-Client.git" CACHE STRING "URL of micrortps-client repository which will be tested.")
#set(CLIENT_BRANCH "develop" CACHE STRING "micrortps-client's branch which will be tested.")
#set(AGENT_URL "git@github.com:eProsima/Micro-XRCE-DDS-Agent.git" CACHE STRING "url of micrortps-agent repository which will be tested.")
#set(AGENT_BRANCH "develop" CACHE STRING "micrortps-agent's branch which will be tested.")
#
#set(CLIENT_URL_ "REPO_URL" "${CLIENT_URL}")
#set(CLIENT_BRANCH_ "BRANCH" "${CLIENT_BRANCH}")
#set(AGENT_BRANCH_ "BRANCH" "${AGENT_BRANCH}")
#set(AGENT_URL_ "REPO_URL" "${AGENT_URL}")
#
#option(THIRDPARTY "thirdparty" ON)
#option(PACKAGE_UPDATE "update" OFF)
#
#eprosima_clone_package(agent ${AGENT_URL_} ${AGENT_BRANCH_} REQUIRED)
#eprosima_clone_package(client ${CLIENT_URL_} ${CLIENT_BRANCH_} REQUIRED)
#
################################################################################
## Set flags
################################################################################
#if(MSVC OR MSVC_IDE)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /wd4700 /wd4996 /wd4820 /wd4255 /wd4668")
#else()
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -fstrict-aliasing -Wall -Wextra -Wcast-align")
#endif()
#
################################################################################
## Testing
################################################################################
#include(${PROJECT_SOURCE_DIR}/cmake/common/gtest.cmake)
#check_gtest()
#if(GTEST_FOUND)
#    enable_testing()
#    include(CTest)
#
#    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#        add_subdirectory(${PROJECT_BINARY_DIR}/client_clone/test/integration/cross_serialization)
#        add_subdirectory(${PROJECT_BINARY_DIR}/agent_clone/test/integration/cross_serialization)
#        add_subdirectory(test/cross_serialization)
#    endif()
#
#    add_subdirectory(${PROJECT_BINARY_DIR}/client_clone/test/integration/interaction)
#    add_subdirectory(test/client_agent)
#    add_subdirectory(test/publisher_subscriber)
#
#    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
#        add_subdirectory(test/discovery)
#    endif()
#
#    if(EPROSIMA_BUILD)
#        add_subdirectory(test/shapes_demo)
#    endif()
#else()
#    message(FATAL_ERROR "This meta-project needs gtest")
#endif()
#
