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

import sys, os, subprocess
from time import sleep

test_agent_command = os.environ.get("TEST_INTEGRATION_AGENT")
assert test_agent_command
test_client_command = os.environ.get("TEST_INTEGRATION_CLIENT")
assert test_client_command

option="--gtest_filter="

integration_test_list = set([
    ("AgentTests.CreateDeleteClient",       "ClientTests.CreateDeleteClient"),
    ("AgentTests.CreateDeleteParticipant",  "ClientTests.CreateDeleteParticipant"),
    ("AgentTests.CreateDeletePublisher",    "ClientTests.CreateDeletePublisher"),
    ("AgentTests.CreateDeleteSubscriber",   "ClientTests.CreateDeleteSubscriber"),
    ("AgentTests.CreateDeleteDataWriter",   "ClientTests.CreateDeleteDataWriter"),
    ("AgentTests.CreateDeleteDataReader",   "ClientTests.CreateDeleteDataReader"),
    ("AgentTests.WriteData",                "ClientTests.WriteData"),
    ])

for a_test, c_test in integration_test_list:
    a_args = option + a_test
    c_args = option + c_test
    
    test_agent_proc = subprocess.Popen([test_agent_command, a_args], stderr=subprocess.PIPE) 
    sleep(1)
    test_client_proc = subprocess.Popen([test_client_command, c_args], stderr=subprocess.PIPE)
    print("###################################")

    #return rc

    
    # Todo de golpe
    #for line in test_agent_proc.stdout:
          #print(line)
    
    #output = test_agent_proc.stdout.read()
    #print("###" + output)
    retvalue = test_agent_proc.returncode


sys.exit(retvalue)
