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

add_executable(cross_serialization_test_exec
    CrossSerialization.cpp
    ClientSerialization.cpp
    AgentSerialization.cpp
    )
add_test(NAME cross_serialization_test COMMAND cross_serialization_test_exec)
target_include_directories(cross_serialization_test_exec
    PRIVATE
        ${GTEST_INCLUDE_DIRS}
        "../../thirdparty/client/src/c"
        )
target_link_libraries(cross_serialization_test_exec
    ${GTEST_BOTH_LIBRARIES}
    micrortps-client
    micrortps-agent
    )
set_target_properties(cross_serialization_test_exec PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    )
if(WIN32)
    set(DEPENDENCIES
        micrortps-client
        micrortps-agent
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
    set_tests_properties(cross_serialization_test PROPERTIES ENVIRONMENT "PATH=${WIN_PATH}")
endif()
