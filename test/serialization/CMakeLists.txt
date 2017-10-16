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
                      ${CLIENT_DIR}/src/c/serialization.c
                      ${CLIENT_DIR}/src/c/message.c
                      ${CLIENT_DIR}/src/c/mini_cdr.c
                      ${CLIENT_DIR}/src/c/memory_cache.c
                      ${AGENT_DIR}/src/cpp/ObjectVariant.cpp
                      ${AGENT_DIR}/src/cpp/Serializer.cpp
                      ${AGENT_DIR}/src/cpp/XRCEFactory.cpp
                      ${AGENT_DIR}/src/cpp/MessageHeader.cpp
                      ${AGENT_DIR}/src/cpp/SubMessageHeader.cpp
                      ${AGENT_DIR}/src/cpp/Payloads.cpp
                      ${AGENT_DIR}/src/cpp/XRCEParser.cpp
                      )
    add_executable(x_ser_des_test ${X_SER_DES_SRC})
    add_gtest(x_ser_des_test ${X_SER_DES_SRC})
    
    target_include_directories(x_ser_des_test PRIVATE ${GTEST_INCLUDE_DIRS}
                                                      ${CLIENT_DIR}/include
                                                      ${AGENT_DIR}/include
                                                      ${FASTRTPS_INSTALL_DIR}/include
                                                      )
    target_link_libraries(x_ser_des_test ${GTEST_BOTH_LIBRARIES} fastcdr)


endif()