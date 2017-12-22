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
if(NOT ((MSVC OR MSVC_IDE) AND EPROSIMA_INSTALLER))
    find_package(PythonInterp 3)
    find_package(fastrtps REQUIRED)
    find_package(fastcdr REQUIRED)

    ###############################################################################
    # blackbox
    ###############################################################################

    # Hellow world test
    set(BLACKBOXTESTS_SOURCE FirstTest.cpp)
    add_executable(first_test ${BLACKBOXTESTS_SOURCE})
    add_gtest(first_test ${BLACKBOXTESTS_SOURCE})
    target_include_directories(first_test PRIVATE ${GTEST_INCLUDE_DIRS})
    target_link_libraries(first_test ${GTEST_BOTH_LIBRARIES})

    # Basic test udp - C code
    set(BASIC_UDP_SEND_SRC BasicUdpSendTest.c)
    add_executable(udp_send_test ${BASIC_UDP_SEND_SRC})
    target_link_libraries(udp_send_test transport)

    set(BASIC_UDP_RECV_SRC BasicUdpRecvTest.c)
    add_executable(udp_recv_test ${BASIC_UDP_RECV_SRC})
    target_link_libraries(udp_recv_test transport)

    # libdev.so
    # include_directories(${AGENT_DIR}/src/cpp/
    #                     ${AGENT_DIR}/src/cpp/libdev/)
    #file(GLOB LIBDEV_SRCS "${AGENT_DIR}/src/cpp/libdev/*.cpp")
    #add_library(dev SHARED ${LIBDEV_SRCS})
    #install(TARGETS dev DESTINATION ${CMAKE_SOURCE_DIR}/install)

    # TestA TestB - C code
    # TODO Agent cant be linked against
    # set(TEST_A_SRC TestA.cpp)
    # add_executable(test_a ${TEST_A_SRC})
    # target_link_libraries(test_a ${GTEST_BOTH_LIBRARIES} agent fastcdr fastrtps)

    set(TEST_B_SRC TestB.cpp)
    add_executable(test_b ${TEST_B_SRC})
    target_link_libraries(test_b ${GTEST_BOTH_LIBRARIES})

    if(PYTHONINTERP_FOUND)
        # Basic test udp - python
        add_test(NAME basic_udp_test
            COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/udp_basic_test.py)

        set_property(TEST basic_udp_test PROPERTY ENVIRONMENT
            "BASIC_UDP_SENDER_BIN=$<TARGET_FILE:udp_send_test>")
        set_property(TEST basic_udp_test APPEND PROPERTY ENVIRONMENT
            "BASIC_UDP_RECEIVER_BIN=$<TARGET_FILE:udp_recv_test>")
        if(WIN32)
            string(REPLACE ";" "\\;" WIN_PATH "$ENV{PATH}")
            set_property(TEST basic_udp_test APPEND PROPERTY ENVIRONMENT
                "PATH=$<TARGET_FILE_DIR:${PROJECT_NAME}>\\;$<TARGET_FILE_DIR:transport>\\;${WIN_PATH}")
        endif()

        # TestA TestB - python
        add_test(NAME testA_testB COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test.py)
        # set_property(TEST testA_testB        PROPERTY ENVIRONMENT "TEST_A_BIN=$<TARGET_FILE:test_a>")
        set_property(TEST testA_testB APPEND PROPERTY ENVIRONMENT "TEST_B_BIN=$<TARGET_FILE:test_b>")
        if(WIN32)
            string(REPLACE ";" "\\;" WIN_PATH "$ENV{PATH}")
            set_property(TEST testA_testB APPEND PROPERTY ENVIRONMENT
                "PATH=$<TARGET_FILE_DIR:${PROJECT_NAME}>\\;$<TARGET_FILE_DIR:agent>\\;${WIN_PATH}")
        endif()

    endif()

    if(PYTHONINTERP_FOUND)

        # Integration - python
        add_test(NAME integration_test COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_integration.py)
        set_property(TEST integration_test        PROPERTY ENVIRONMENT "TEST_INTEGRATION_AGENT=$<TARGET_FILE:AgentIntegrationTest>")
        set_property(TEST integration_test APPEND PROPERTY ENVIRONMENT "TEST_INTEGRATION_CLIENT=$<TARGET_FILE:ClientUnitTests>")
        if(WIN32)
            string(REPLACE ";" "\\;" WIN_PATH "$ENV{PATH}")
            set_property(TEST integration_test APPEND PROPERTY ENVIRONMENT
                "PATH=$<TARGET_FILE_DIR:${PROJECT_NAME}>\\;$<TARGET_FILE_DIR:agent>\\;${WIN_PATH}")
        endif()
    endif()

endif()