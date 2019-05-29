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

include(ExternalProject)

# Agent project.
ExternalProject_Add(microxrcedds_agent
    PREFIX
        ${PROJECT_BINARY_DIR}/microxrcedds_agent
    GIT_REPOSITORY
        https://github.com/eProsima/Micro-XRCE-DDS-Agent.git
    GIT_TAG
        origin/${AGENT_BRANCH}
    CMAKE_ARGS
        -DTHIRDPARTY=ON
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DEPROSIMA_BUILD_TESTS=ON
        -DBUILD_SHARED_LIBS=OFF
    )
ExternalProject_Get_Property(microxrcedds_agent INSTALL_DIR)
ExternalProject_Get_Property(microxrcedds_agent SOURCE_DIR)
set(AGENT_INSTALL_DIR ${INSTALL_DIR})
set(AGENT_SOURCE_DIR ${SOURCE_DIR})

# Client project.
ExternalProject_Add(microxrcedds_client
    PREFIX
        ${PROJECT_BINARY_DIR}/microxrcedds_client
    GIT_REPOSITORY
        https://github.com/eProsima/Micro-XRCE-DDS-Client.git
    GIT_TAG
        origin/${CLIENT_BRANCH}
    CMAKE_ARGS
        -DTHIRDPARTY=ON
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DEPROSIMA_BUILD_TESTS=ON
        -DBUILD_SHARED_LIBS=OFF
    )
ExternalProject_Get_Property(microxrcedds_client INSTALL_DIR)
ExternalProject_Get_Property(microxrcedds_client SOURCE_DIR)
set(CLIENT_INSTALL_DIR ${INSTALL_DIR})
set(CLIENT_SOURCE_DIR ${SOURCE_DIR})

# Integration test project.
ExternalProject_Add(itest
    SOURCE_DIR
        ${PROJECT_SOURCE_DIR}
    BINARY_DIR
        ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_ARGS
        -DSUPERBUILD=OFF
        -DAGENT_INSTALL_DIR=${AGENT_INSTALL_DIR}
        -DAGENT_SOURCE_DIR=${AGENT_SOURCE_DIR}
        -DCLIENT_INSTALL_DIR=${CLIENT_INSTALL_DIR}
        -DCLIENT_SOURCE_DIR=${CLIENT_SOURCE_DIR}
    DEPENDS
        microxrcedds_client
        microxrcedds_agent
    INSTALL_COMMAND
        ""
    )
