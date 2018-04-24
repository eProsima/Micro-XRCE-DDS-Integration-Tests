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

#include_directories("/usr/include")
#include_directories("../../thirdparty/client/include")
#include_directories("../../thirdparty/client/src/c")
#include_directories("../../thirdparty/agent/include")
find_package(fastcdr REQUIRED)

if(NOT ((MSVC OR MSVC_IDE) AND EPROSIMA_INSTALLER))

    include(${PROJECT_SOURCE_DIR}/cmake/dev/gtest.cmake)

    set(CROSS_SERIALIZATION_SOURCE CrossSerialization.cpp ClientSerialization.cpp AgentSerialization.cpp)
    add_executable(CrossSerializationTest ${CROSS_SERIALIZATION_SOURCE})
    add_gtest(CrossSerializationTest ${CROSS_SERIALIZATION_SOURCE})
    target_include_directories(CrossSerializationTest PRIVATE ${GTEST_INCLUDE_DIRS})
    target_link_libraries(CrossSerializationTest ${GTEST_BOTH_LIBRARIES}
        micrortps-client
        agent
        pthread)

endif()

