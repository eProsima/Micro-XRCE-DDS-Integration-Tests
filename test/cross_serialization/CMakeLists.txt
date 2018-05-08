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

set(SRCS CrossSerialization.cpp)
add_executable(cross_serialization_test ${SRCS})
add_gtest(cross_serialization_test
    SOURCES
        ${SRCS}
    DEPENDENCIES
        cross-serialization-intest-client
        cross-serialization-intest-agent
        micrortps_client
        micrortps_agent_static
        micrortps_transport
        microcdr
        fastrtps
        fastcdr
    )
target_include_directories(cross_serialization_test
    PRIVATE
        ${GTEST_INCLUDE_DIRS}
    )
target_link_libraries(cross_serialization_test
    ${GTEST_BOTH_LIBRARIES}
        cross-serialization-intest-client
        cross-serialization-intest-agent
    )
set_target_properties(cross_serialization_test PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    )
