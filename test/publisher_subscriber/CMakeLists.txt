# Copyright 2018 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    FILES_MATCHING PATTERN "*.xml"
    )

add_executable(publisher_subscriber_interaction_test PublisherSubscriberInteraction.cpp)
add_gtest(publisher_subscriber_interaction_test
    SOURCES
        TwoClientAgentInteraction.cpp
    DEPENDENCIES
        interaction-intest-client
        microxrcedds_agent
        microxrcedds_client
        microcdr
        fastrtps
        fastcdr
    )

target_include_directories(publisher_subscriber_interaction_test PRIVATE ${GTEST_INCLUDE_DIR})
target_link_libraries(publisher_subscriber_interaction_test
    interaction-intest-client
    microxrcedds_agent
    ${GTEST_BOTH_LIBRARIES}
    )

set_target_properties(publisher_subscriber_interaction_test PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    )

