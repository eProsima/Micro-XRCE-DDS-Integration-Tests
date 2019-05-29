###############################################################################
#
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
#
###############################################################################

#if(NOT PROFILE_UDP_TRANSPORT OR
#   NOT PROFILE_TCP_TRANSPORT OR
#   NOT PROFILE_SERIAL_TRANSPORT)
#    message(WARNING "Can not compile test: The PROFILE_UDP_TRANSPORT, PROFILE_TCP_PROFILE and PROFILE_SERIAL_TRANSPORT must be enabled.")
#else()
#    project(interaction-intest-client CXX)

    include(${PROJECT_SOURCE_DIR}/cmake/common/check_configuration.cmake)

    set(SRCS
        Discovery.cpp
        Client.cpp
        Gateway.cpp
        BigHelloWorld.c
        )

    add_library(interaction_client STATIC ${SRCS})
    set_common_compile_options(interaction_client)
    if(MSVC OR MSVC_IDE)
        target_compile_options(interaction_client PRIVATE /wd4996)
    endif()

    target_include_directories(interaction_client
        PUBLIC
            ${CMAKE_CURRENT_SOURCE_DIR}
        PRIVATE
            ${GTEST_INCLUDE_DIRS}
        )
    target_link_libraries(interaction_client microxrcedds_client ${GTEST_BOTH_LIBRARIES})
    target_include_directories(interaction_client
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
        )

    set_target_properties(interaction_client PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
        )
#endif()
