# Copyright 2017-present Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

set(_test_name itest-discovery)

cmake_host_system_information(RESULT HOSTNAME_SUFFIX QUERY HOSTNAME)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DEFAULT_FASTRTPS_PROFILES.xml.in
    ${CMAKE_CURRENT_BINARY_DIR}/DEFAULT_FASTRTPS_PROFILES.xml
    @ONLY
    )

add_executable(${_test_name} DiscoveryIntegration.cpp)

add_gtest(${_test_name}
    SOURCES
        DiscoveryIntegration.cpp
    ENVIRONMENTS
        $<$<PLATFORM_ID:Linux>:LD_LIBRARY_PATH=${CMAKE_PREFIX_PATH}/lib>
        $<$<PLATFORM_ID:Windows>:PATH=${CMAKE_PREFIX_PATH}/bin>
    )

target_include_directories(${_test_name}
    PRIVATE
        ${GTEST_INCLUDE_DIR}
    )

target_link_libraries(${_test_name}
    PRIVATE
        interaction_client
        microxrcedds_agent
        ${GTEST_BOTH_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
    )

set_target_properties(${_test_name} PROPERTIES
    CXX_STANDARD
        11
    CXX_STANDARD_REQUIRED
        YES
    )

