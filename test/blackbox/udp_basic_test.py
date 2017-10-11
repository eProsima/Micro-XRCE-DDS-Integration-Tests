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

udp_sender_command = os.environ.get("BASIC_UDP_SENDER_BIN")
assert udp_sender_command
udp_receiver_command = os.environ.get("BASIC_UDP_RECEIVER_BIN")
assert udp_receiver_command

sender_proc = subprocess.Popen([udp_sender_command])
receiver_proc = subprocess.Popen([udp_receiver_command])

sender_proc.communicate()
retvalue = receiver_proc.returncode

sender_proc.kill()

sys.exit(retvalue)
