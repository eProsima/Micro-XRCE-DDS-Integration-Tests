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

find_package(PythonInterp 3)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    FILES_MATCHING PATTERN "*.xml"
)

# Client executable
add_executable(client_itest client_integration_test.cpp)
target_include_directories(client_itest PRIVATE ${GTEST_INCLUDE_DIR})
target_link_libraries(client_itest micrortps-client ${GTEST_BOTH_LIBRARIES})
set_target_properties(client_itest PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    )

# Agent executable
add_executable(agent_itest agent_integration_test.cpp)
target_include_directories(agent_itest PRIVATE ${GTEST_INCLUDE_DIR})
target_link_libraries(agent_itest micrortps-agent ${GTEST_BOTH_LIBRARIES})
set_target_properties(agent_itest PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    )

# Client-Agent integration test
if(PYTHONINTERP_FOUND)
    set(ITESTS
    CreateDeleteClient
    CreateDeleteParticipant
    CreateDeleteTopic
    CreateDeletePublisher
    CreateDeleteSubscriber
    CreateDeleteDataWriter
    CreateDeleteDataReader
    )
    foreach(INTEGRATION_TEST ${ITESTS})
        add_test(NAME ${INTEGRATION_TEST}
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/integration_test.py
        )
        set_property(TEST ${INTEGRATION_TEST} PROPERTY ENVIRONMENT
            "AGENT_EXEC=$<TARGET_FILE:agent_itest>"
        )
        set_property(TEST ${INTEGRATION_TEST} APPEND PROPERTY ENVIRONMENT
            "CLIENT_EXEC=$<TARGET_FILE:client_itest>"
        )
        set_property(TEST ${INTEGRATION_TEST} APPEND PROPERTY ENVIRONMENT
            "TEST_TYPE=${INTEGRATION_TEST}"
        )
        if(WIN32)
            string(REPLACE ";" "\\;" WIN_PATH "$ENV{PATH}")
            set_property(TEST ${INTEGRATION_TEST} APPEND PROPERTY ENVIRONMENT
                "PATH=$<SHELL_PATH:$<TARGET_FILE_DIR:microcdr>\\;$<TARGET_FILE_DIR:transport>\\;
                $<TARGET_FILE_DIR:fastcdr>\\;$<TARGET_FILE_DIR:fastrtps>\\;${WIN_PATH}>"
            )
        endif()
    endforeach()
endif()
