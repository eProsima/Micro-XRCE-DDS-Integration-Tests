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

set(SRCS
    CrossSerialization.cpp
    AgentSerialization.cpp
    ClientSerialization.cpp
    )
add_executable(cross_serialization_test ${SRCS})
add_gtest(cross_serialization_test
    SOURCES
        ${SRCS}
    ENVIRONMENTS
        $<$<PLATFORM_ID:Linux>:LD_LIBRARY_PATH=${AGENT_INSTALL_DIR}/lib/>
        $<$<PLATFORM_ID:Windows>:"PATH=%PATH%;${AGENT_INSTALL_DIR}/lib">
    )
target_include_directories(cross_serialization_test
    PRIVATE
        ${CLIENT_INSTALL_DIR}/include
        ${CLIENT_SOURCE_DIR}/src/c
        ${AGENT_INSTALL_DIR}/include
        ${GTEST_INCLUDE_DIRS}
    )
target_link_libraries(cross_serialization_test
    PRIVATE
        ${CLIENT_INSTALL_DIR}/lib/${CLIENT_LIBRARY}
        ${CLIENT_INSTALL_DIR}/lib/libmicrocdr.so
        ${AGENT_INSTALL_DIR}/lib/${AGENT_LIBRARY}
        ${AGENT_INSTALL_DIR}/lib/libfastcdr.so
        ${GTEST_BOTH_LIBRARIES}
    )
set_target_properties(cross_serialization_test PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED YES
    )
#add_test(NAME cross_serialization
#    COMMAND
#        cross_serialization_test
#        "export LD_LIBRARY_PATH=${AGENT_INSTALL_DIR}/lib"
#    )
#set_tests_properties(cross_serialization PROPERTIES
#    ENVIRONMENT
#        LD_LIBRARY_PATH=${AGENT_INSTALL_DIR}/lib
#    )
#set_property(TEST cross_serialization APPEND PROPERTY ENVIRONMENT "PATH=${AGENT_INSTALL_DIR}/")
#get_property(TEST_PATH TEST cross_serialization PROPERTY ENVIRONMENT)
#message("${TEST_PATH}")
