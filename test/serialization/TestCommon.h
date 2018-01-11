/****************************************************************************
 *
 * Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

#ifndef _TEST_COMMON_H_
#define _TEST_COMMON_H_

#include <gtest/gtest.h>

#include <Payloads.h>
#include <agent/XRCEParser.h>
#include <log/message.h>
#include <micrortps/client/xrce_protocol_spec.h>

#define BUFFER_SIZE 1024

using namespace eprosima;

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define BROWN   "\033[33m"      /* Brown */
#define YELLOW  "\033[1;33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define RESET   "\033[0m"

#define EVALUATE(exp, val) ((exp == val)? true: (printf(RED "line %d: %s != %s\n" RESET, __LINE__, #exp, #val), false))

typedef micrortps::MessageHeader           agent_header;
typedef MessageHeader                      client_header;
typedef micrortps::SubmessageHeader        agent_subheader;
typedef SubmessageHeader                   client_subheader;
typedef micrortps::CREATE_Payload          agent_create_payload;
typedef CreateResourcePayload              client_create_payload;
typedef micrortps::WRITE_DATA_Payload      agent_write_payload;
typedef WriteDataPayload                   client_write_payload;
typedef micrortps::READ_DATA_Payload       agent_read_payload;
typedef ReadDataPayload                    client_read_payload;
typedef micrortps::DELETE_RESOURCE_Payload agent_delete_payload;
typedef DeleteResourcePayload              client_delete_payload;
typedef micrortps::RESOURCE_STATUS_Payload agent_status_payload;
typedef StatusPayload                      client_status_payload;

void on_initialize_message(client_header* header, ClientKey* key, void* vstate);
void on_initialize_submessage(const client_subheader* header, void* vstate);
bool operator==(const micrortps::OBJECTKIND& left, const uint8_t right);
bool operator==(const uint8_t left, const micrortps::OBJECTKIND& right);
bool operator==(const std::array<uint8_t, 2>& left, const uint16_t right);
bool operator==(const uint16_t left, const std::array<uint8_t, 2>& right);
bool operator==(const std::array<uint8_t, 3>& left, const uint32_t right);
bool operator==(const uint32_t left, const std::array<uint8_t, 3>& right);
bool operator==(const std::array<uint8_t, 4>& left, const uint32_t& right);
bool operator==(const micrortps::ClientKey& left, const ClientKey& right);
bool operator==(const uint32_t& left, const std::array<uint8_t, 4>& right);
bool are_the_same(const std::vector<uint8_t>& _vector, const uint8_t* _array, const uint32_t& array_length);

#endif
