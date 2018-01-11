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

agent_exec = os.environ.get("AGENT_EXEC")
assert agent_exec
client_exec = os.environ.get("CLIENT_EXEC")
assert client_exec
test_type = os.environ.get("TEST_TYPE")
assert test_type

option="--gtest_filter="

agent_args = option + "AgentTests." + test_type
client_args = option + "ClientTests." + test_type

agent_proc = subprocess.Popen([agent_exec, agent_args], stderr=subprocess.PIPE) 
sleep(1)
client_proc = subprocess.Popen([client_exec, client_args], stderr=subprocess.PIPE)

client_proc.communicate()
ret_value = client_proc.returncode

agent_proc.communicate()
if (ret_value == 0):
    ret_value = agent_proc.returncode
    
sys.exit(ret_value)

