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

set(_test_name performance-test)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../common/EntitiesInfo.hpp.in
    ${CMAKE_CURRENT_BINARY_DIR}/EntitiesInfo.hpp
    @ONLY
    )

add_executable(${_test_name} performance-test.cpp)

target_link_libraries(${_test_name}
    PRIVATE
        microxrcedds_agent
        microxrcedds_client
    )

target_include_directories(${_test_name}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/../common
    )

set_target_properties(${_test_name} PROPERTIES
    CXX_STANDARD
        11
    CXX_STANDARD_REQUIRES
        YES
    )
