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

#include <gtest/gtest.h>
#include <ClientSerialization.hpp>
#include <AgentSerialization.hpp>
#include <cstdint>

class CrossSerializationTests : public testing::Test
{

};

/* ############################################## TESTS ##################################################### */

TEST_F(CrossSerializationTests, CreateClientPayload)
{
    std::vector<uint8_t> client_ser = ClientSerialization::create_client_payload();
    std::vector<uint8_t> agent_ser = AgentSerialization::create_client_payload();

    EXPECT_EQ(client_ser, agent_ser);
}

TEST_F(CrossSerializationTests, CreatePayload)
{
    std::vector<uint8_t> client_ser = ClientSerialization::create_payload();
    std::vector<uint8_t> agent_ser = AgentSerialization::create_payload();

    EXPECT_EQ(client_ser, agent_ser);
}

TEST_F(CrossSerializationTests, DeletePayload)
{
    std::vector<uint8_t> client_ser = ClientSerialization::delete_payload();
    std::vector<uint8_t> agent_ser = AgentSerialization::delete_payload();

    EXPECT_EQ(client_ser, agent_ser);
}

TEST_F(CrossSerializationTests, StatusPayload)
{
    std::vector<uint8_t> client_ser = ClientSerialization::status_payload();
    std::vector<uint8_t> agent_ser = AgentSerialization::status_payload();

    EXPECT_EQ(client_ser, agent_ser);
}

TEST_F(CrossSerializationTests, ReadDataPayload)
{
    std::vector<uint8_t> client_ser = ClientSerialization::read_data_payload();
    std::vector<uint8_t> agent_ser = AgentSerialization::read_data_payload();

    EXPECT_EQ(client_ser, agent_ser);
}

TEST_F(CrossSerializationTests, WriteDataPayloadData)
{
    std::vector<uint8_t> client_ser = ClientSerialization::write_data_payload_data();
    std::vector<uint8_t> agent_ser = AgentSerialization::write_data_payload_data();

    EXPECT_EQ(client_ser, agent_ser);
}

TEST_F(CrossSerializationTests, DataPayloadData)
{
    std::vector<uint8_t> client_ser = ClientSerialization::data_payload_data();
    std::vector<uint8_t> agent_ser = AgentSerialization::data_payload_data();

    EXPECT_EQ(client_ser, agent_ser);
}

TEST_F(CrossSerializationTests, AcknackPayload)
{
    std::vector<uint8_t> client_ser = ClientSerialization::acknack_payload();
    std::vector<uint8_t> agent_ser = AgentSerialization::acknack_payload();

    EXPECT_EQ(client_ser, agent_ser);
}

TEST_F(CrossSerializationTests, HeartbeatPayload)
{
    std::vector<uint8_t> client_ser = ClientSerialization::heartbeat_payload();
    std::vector<uint8_t> agent_ser = AgentSerialization::heartbeat_payload();

    EXPECT_EQ(client_ser, agent_ser);
}
