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
ExternalProject_Add(uagent
    PREFIX
        ${PROJECT_BINARY_DIR}/uagent
    GIT_REPOSITORY
        https://github.com/eProsima/Micro-XRCE-DDS-Agent.git
    GIT_TAG
        origin/${AGENT_BRANCH}
    GIT_SHALLOW
        TRUE
    GIT_SUBMODULES
        "thirdparty/uxrclient"
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DBUILD_SHARED_LIBS:BOOL=ON
    )
ExternalProject_Get_Property(uagent INSTALL_DIR)
ExternalProject_Get_Property(uagent SOURCE_DIR)
set(AGENT_INSTALL_DIR ${INSTALL_DIR})
set(AGENT_SOURCE_DIR ${SOURCE_DIR})

# Client project.
ExternalProject_Add(uclient
    PREFIX
        ${PROJECT_BINARY_DIR}/uclient
    GIT_REPOSITORY
        https://github.com/eProsima/Micro-XRCE-DDS-Client.git
    GIT_TAG
        origin/${CLIENT_BRANCH}
    GIT_SHALLOW
        TRUE
    GIT_SUBMODULES
        "thirdparty/microcdr" 
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
        -DUCLIENT_BUILD_EXAMPLES:BOOL=ON
        -DBUILD_SHARED_LIBS:BOOL=OFF
    )
ExternalProject_Get_Property(uclient INSTALL_DIR)
ExternalProject_Get_Property(uclient SOURCE_DIR)
set(CLIENT_INSTALL_DIR ${INSTALL_DIR})
set(CLIENT_SOURCE_DIR ${SOURCE_DIR})

# Integration test project.
ExternalProject_Add(itest
    SOURCE_DIR
        ${PROJECT_SOURCE_DIR}
    BINARY_DIR
        ${CMAKE_CURRENT_BINARY_DIR}
    CMAKE_ARGS
        -DAGENT_INSTALL_DIR=${AGENT_INSTALL_DIR}
        -DAGENT_SOURCE_DIR=${AGENT_SOURCE_DIR}
        -DCLIENT_INSTALL_DIR=${CLIENT_INSTALL_DIR}
        -DCLIENT_SOURCE_DIR=${CLIENT_SOURCE_DIR}
    CMAKE_CACHE_ARGS
        -DUTEST_SUPERBUILD:BOOL=OFF
    DEPENDS
        uagent
        uclient
    INSTALL_COMMAND
        ""
    )
