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

find_package(fastcdr REQUIRED)

if(NOT ((MSVC OR MSVC_IDE) AND EPROSIMA_INSTALLER))

    ###############################################################################
    # serialization
    ###############################################################################
    
    set(X_SER_DES_SRC CrossSerDesTest.cpp
                      ${CLIENT_DIR}/src/c/xrce_protocol_serialization.c
                      ${CLIENT_DIR}/src/c/input_message.c
                      ${CLIENT_DIR}/src/c/output_message.c
                      ${CLIENT_DIR}/src/c/log/message.c
                      ${CLIENT_DIR}/src/c/aux_memory.c
                      ${AGENT_DIR}/src/cpp/DDSXRCETypes.cxx
                      ${AGENT_DIR}/src/cpp/MessageHeader.cxx
                      ${AGENT_DIR}/src/cpp/Payloads.cxx
                      ${AGENT_DIR}/src/cpp/Serializer.cpp
                      ${AGENT_DIR}/src/cpp/SubMessageHeader.cxx
                      ${AGENT_DIR}/src/cpp/XRCEFactory.cpp
                      ${AGENT_DIR}/src/cpp/XRCEParser.cpp
                      )
    add_executable(cross_serialization_test ${X_SER_DES_SRC})
    add_gtest(cross_serialization_test ${X_SER_DES_SRC})
    
    target_include_directories(cross_serialization_test PRIVATE ${GTEST_INCLUDE_DIRS}
                                                      ${CLIENT_DIR}/include
                                                      ${CLIENT_DIR}/include/micrortps/client
                                                      ${CLIENT_DIR}/src/c
                                                      ${CLIENT_DIR}/thirdparty/micro-CDR/include
                                                      ${CLIENT_DIR}/thirdparty/micro-CDR/include/microcdr
                                                      ${PROJECT_BINARY_DIR}/external/install/include/microcdr
                                                      ${AGENT_DIR}/include
                                                      ${AGENT_DIR}/include/agent
                                                      ${AGENT_DIR}/src/cpp
                                                      ${FASTRTPS_INSTALL_DIR}/include)
    target_link_libraries(cross_serialization_test ${GTEST_BOTH_LIBRARIES} microcdr fastcdr)


endif()