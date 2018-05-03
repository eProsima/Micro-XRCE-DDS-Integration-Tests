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

find_package(PythonInterp 3 REQUIRED)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
    FILES_MATCHING PATTERN "*.xml"
    )

## Client executable
#add_executable(client_itest client_integration_test.cpp)
#target_include_directories(client_itest PRIVATE ${GTEST_INCLUDE_DIR})
#target_link_libraries(client_itest micrortps-client ${GTEST_BOTH_LIBRARIES})
#set_target_properties(client_itest PROPERTIES
#    CXX_STANDARD 11
#    CXX_STANDARD_REQUIRED YES
#    )
#
## Agent executable
#add_executable(agent_itest agent_integration_test.cpp)
#target_include_directories(agent_itest PRIVATE ${GTEST_INCLUDE_DIR})
#target_link_libraries(agent_itest micrortps-agent ${GTEST_BOTH_LIBRARIES})
#set_target_properties(agent_itest PROPERTIES
#    CXX_STANDARD 11
#    CXX_STANDARD_REQUIRED YES
#    )

# Interaction test
add_executable(interaction_test_exec interaction_test.cpp)
add_test(NAME interaction_test COMMAND interaction_test_exec)
target_include_directories(interaction_test_exec PRIVATE ${GTEST_INCLUDE_DIR})
target_link_libraries(interaction_test_exec micrortps-agent micrortps-client ${GTEST_BOTH_LIBRARIES})
set_target_properties(interaction_test_exec PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    )
if(WIN32)
    set(DEPENDENCIES
        micrortps-agent
        micrortps-client
        transport
        microcdr
        fastrtps
        fastcdr
        )
    set(WIN_PATH "$ENV{PATH}")
    foreach(P ${CMAKE_PREFIX_PATH})
        set(WIN_PATH "${CMAKE_PREFIX_PATH}/bin;${WIN_PATH}")
    endforeach()
    foreach(D ${DEPENDENCIES})
        if(TARGET ${D})
            set(WIN_PATH "$<TARGET_FILE_DIR:${D}>;${WIN_PATH}")
        endif()
    endforeach()
    string(REPLACE ";" "\\;" WIN_PATH "${WIN_PATH}")
    set_tests_properties(interaction_test PROPERTIES ENVIRONMENT "PATH=${WIN_PATH}")
endif()

## Client-Agent integration test
#if(PYTHONINTERP_FOUND)
#    set(ITESTS
#        CreateDeleteClient
#        CreateDeleteParticipant
#        CreateDeleteTopic
#        CreateDeletePublisher
#        CreateDeleteSubscriber
#        CreateDeleteDataWriter
#        CreateDeleteDataReader
#        )
#    foreach(INTEGRATION_TEST ${ITESTS})
#        add_test(NAME ${INTEGRATION_TEST}
#            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/integration_test.py
#        )
#        set_property(TEST ${INTEGRATION_TEST} PROPERTY ENVIRONMENT
#            "AGENT_EXEC=$<TARGET_FILE:agent_itest>"
#        )
#        set_property(TEST ${INTEGRATION_TEST} APPEND PROPERTY ENVIRONMENT
#            "CLIENT_EXEC=$<TARGET_FILE:client_itest>"
#        )
#        set_property(TEST ${INTEGRATION_TEST} APPEND PROPERTY ENVIRONMENT
#            "TEST_TYPE=${INTEGRATION_TEST}"
#        )
#        if(WIN32)
#            set(DEPENDENCIES
#                micrortps-agent
#                transport
#                microcdr
#                fastrtps
#                fastcdr
#                )
#            set(WIN_PATH "$ENV{PATH}")
#            foreach(P ${CMAKE_PREFIX_PATH})
#                set(WIN_PATH "${CMAKE_PREFIX_PATH}/bin;${WIN_PATH}")
#            endforeach()
#            foreach(D ${DEPENDENCIES})
#                if(TARGET ${D})
#                    set(WIN_PATH "$<TARGET_FILE_DIR:${D}>;${WIN_PATH}")
#                endif()
#            endforeach()
#            string(REPLACE ";" "\\;" WIN_PATH "${WIN_PATH}")
#            set_property(TEST ${INTEGRATION_TEST} APPEND PROPERTY ENVIRONMENT "PATH=${WIN_PATH}")
#        endif()
#    endforeach()
#endif()
