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

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    FILES_MATCHING PATTERN "*.xml"
    )

add_executable(client-agent-test ClientAgentInteraction.cpp)
add_gtest(client-agent-test
    SOURCES
        ClientAgentInteraction.cpp
    ENVIRONMENTS
        $<$<PLATFORM_ID:Linux>:LD_LIBRARY_PATH=${AGENT_INSTALL_DIR}/lib;${CLIENT_INSTALL_DIR}/lib>
        $<$<PLATFORM_ID:Windows>:PATH=${AGENT_INSTALL_DIR}/bin;${CLIENT_INSTALL_DIR}/bin;${CMAKE_BINARY_DIR}>
    )
target_include_directories(client-agent-test
    PRIVATE
        ${GTEST_INCLUDE_DIR}
    )
target_link_libraries(client-agent-test
    PRIVATE
        interaction_client
        microxrcedds_agent
        ${GTEST_BOTH_LIBRARIES}
    )
set_target_properties(client-agent-test
    PROPERTIES
        CXX_STANDARD 11
        CXX_STANDARD_REQUIRED YES
    )

