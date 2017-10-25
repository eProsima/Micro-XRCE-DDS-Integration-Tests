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

#include "TestCommon.h"

ClientKey client_key = {0xF1, 0xF2, 0xF3, 0xF4};

client_header client_header_var = {
        .session_id = 0xFF,
        .stream_id = 0x04,
        .sequence_nr = 0x0200,
};

int submsg_idx = 0;
client_subheader      client_subheader_var[16] = {};
client_create_payload client_create_payload_var = {};
client_write_payload  client_write_payload_var = {};
client_read_payload   client_read_payload_var = {};
client_delete_payload client_delete_payload_var = {};

char client_name[] = "Roy Batty";
uint8_t client_write_data[] = "Tengo unos datos magnificos";

void on_initialize_message(client_header* header, ClientKey* key, void* vstate)
{
    *header = client_header_var;
    *key = client_key;
}

void on_initialize_submessage(const client_subheader* header, void* vstate)
{
    client_subheader_var[submsg_idx++] = *header;
}

class ClientToAgent_CrossSerializationTests : public testing::Test
{
public:

    virtual void SetUp()
    {
        submsg_idx = 0;
    }

    virtual void TearDown()
    {
    }

    class Client
    {
    public:

        Client(uint8_t* buffer)
        {
            output_callback.object = nullptr; // +V+ User data
            output_callback.on_initialize_message = on_initialize_message;
            output_callback.on_submessage_header = on_initialize_submessage;
            init_output_message(&output_message, output_callback, buffer, BUFFER_SIZE);
            output_message.writer.endianness = LITTLE_ENDIANNESS;
        }

        virtual ~Client()
        {
        }

        bool fill_client_create_payload(client_create_payload& payload)
        {
            payload.request.base.request_id = 0xAABB;
            payload.request.object_id = 0x7799;
            payload.representation.kind = OBJK_PARTICIPANT;
            payload.representation._.participant.base2.format = REPRESENTATION_BY_REFERENCE;
            payload.representation._.participant.base2._.object_name.data = client_name;
            payload.representation._.participant.base2._.object_name.size = sizeof(client_name);

            return true;
        }

        bool fill_client_write_payload(client_write_payload& payload)
        {
            payload.request.base.request_id = 0xBBCC;
            payload.request.object_id = 0x7889;
            payload.data_to_write.format = FORMAT_SAMPLE;
            payload.data_to_write._.sample.info.state = 0x11223344;
            payload.data_to_write._.sample.info.sequence_number = 0x22334455;
            payload.data_to_write._.sample.info.session_time_offset = 0x33445566;
            payload.data_to_write._.sample.data.data = client_write_data;
            payload.data_to_write._.sample.data.size = sizeof(client_write_data);
            return true;
        }

        bool fill_client_read_payload(client_read_payload& payload)
        {
            payload.request.base.request_id = 0xCCDD;
            payload.request.object_id = 0x8899;
            payload.read_specification.delivery_config.max_elapsed_time = 987654321;
            payload.read_specification.delivery_config.max_rate = 123123123;
            payload.read_specification.delivery_config.max_samples = 12345;
            //payload.read_specification.content_filter_expression;
            payload.read_specification.optional_content_filter_expression = false;
            payload.read_specification.optional_delivery_config = FORMAT_DATA;
            return true;
        }


        OutputMessageCallback output_callback = {};
        OutputMessage output_message;
    };

    class Agent
    {
    public:

        Agent()
        {
        }

        virtual ~Agent(){}

        class Listener : public micrortps::XRCEListener
        {
        public:
            void on_message(const micrortps::MessageHeader& header, const micrortps::SubmessageHeader& sub_header, const micrortps::CREATE_Payload& create_payload) override
            {
                content_match = (check_message_header(header, client_header_var) &&
                                 check_submessage_header(sub_header, client_subheader_var[msg_counter]) &&
                                 check_create_payload(create_payload, client_create_payload_var));
                if (content_match) ++msg_counter;
            }

            void on_message(const micrortps::MessageHeader& header, const micrortps::SubmessageHeader& sub_header, const micrortps::DELETE_RESOURCE_Payload& delete_payload) override
            {
                content_match = (check_message_header(header, client_header_var) &&
                                 check_submessage_header(sub_header, client_subheader_var[msg_counter]) &&
                                 check_delete_payload(delete_payload, client_delete_payload_var));
                if (content_match) ++msg_counter;
            }

            void on_message(const micrortps::MessageHeader& header, const micrortps::SubmessageHeader& sub_header, const micrortps::WRITE_DATA_Payload&  write_payload) override
            {
                content_match = (check_message_header(header, client_header_var) &&
                                 check_submessage_header(sub_header, client_subheader_var[msg_counter]) &&
                                 check_write_payload(write_payload, client_write_payload_var));
                if (content_match) ++msg_counter;
            }

            void on_message(const micrortps::MessageHeader& header, const micrortps::SubmessageHeader& sub_header, const micrortps::READ_DATA_Payload&   read_payload) override
            {
                content_match = (check_message_header(header, client_header_var) &&
                                 check_submessage_header(sub_header, client_subheader_var[msg_counter]) &&
                                 check_read_payload(read_payload, client_read_payload_var));
                if (content_match) ++msg_counter;
            }

            bool content_match = false;
            uint8_t msg_counter = 0;
        };

        Listener listener;

    };

    static bool check_message_header(const agent_header& a_header,const client_header& c_header)
    {
        bool res = true;
        if (c_header.session_id > 127)
        {
            res = EVALUATE(a_header.client_key(),  client_key);
        }

        EXPECT_EQ(a_header.sequence_nr(), c_header.sequence_nr);

        return res &&
               (EVALUATE(a_header.session_id(),  c_header.session_id) &&
                EVALUATE(a_header.stream_id(),   c_header.stream_id)  &&
                EVALUATE(a_header.sequence_nr(), c_header.sequence_nr));
    }

    static bool check_submessage_header(const agent_subheader& a_subheader,const client_subheader& c_subheader)
    {
        EXPECT_EQ(a_subheader.submessage_length(), c_subheader.length);
        return (EVALUATE(a_subheader.submessage_id(),     c_subheader.id)    &&
                EVALUATE(a_subheader.flags(),             c_subheader.flags) &&
                EVALUATE(a_subheader.submessage_length(), c_subheader.length));
    }

    static bool check_create_payload(const agent_create_payload& a_payload, const client_create_payload& c_payload)
    {
        //uint16_t uint = (0xFF00 & (a_payload.object_id()[1] << 8)) | (0x00FF & a_payload.object_id()[0]);
        //printf("cliente %u, agente %u %hhu %hhu\n", c_payload.request.object_id, uint, a_payload.object_id()[1], a_payload.object_id()[0]);

        bool res = (EVALUATE(a_payload.request_id(),                 c_payload.request.base.request_id) &&
                    EVALUATE(a_payload.object_id(),                  c_payload.request.object_id)  &&
                    EVALUATE(a_payload.object_representation()._d(), c_payload.representation.kind));

        switch (c_payload.representation.kind)
        {
            case OBJK_PARTICIPANT:
                // TODO
                // OBJK_PARTICIPANT_Representation participant_;
            break;
            case OBJK_TOPIC:
                // TODO
                // OBJK_TOPIC_Representation topic_;
            break;
            case OBJK_DATAWRITER:
            {
                res = res && (EVALUATE(a_payload.object_representation().data_writer().participant_id(),
                                       c_payload.representation._.data_writer.participant_id)
                              &&
                              EVALUATE(a_payload.object_representation().data_writer().publisher_id(),
                                       c_payload.representation._.data_writer.publisher_id)
                              &&
                              EVALUATE(a_payload.object_representation().data_writer().representation()._d(),
                                       c_payload.representation._.data_writer.base3.format)
                              );

                const micrortps::OBJK_Representation3Formats& a_rep = a_payload.object_representation().data_writer().representation();
                const OBJK_Representation3_Base& c_rep = c_payload.representation._.data_writer.base3;

                switch (a_rep._d())
                {
                    case REPRESENTATION_BY_REFERENCE:
                    {
                        res = res && EVALUATE(a_rep.object_reference().size(), (uint32_t)c_rep._.object_name.size)
                                  && EVALUATE(a_rep.object_reference(), c_rep._.object_name.data);

                    }
                    break;
                    case REPRESENTATION_AS_XML_STRING:
                    {
                        res = res && EVALUATE(a_rep.xml_string_representation().size(), (uint32_t)c_rep._.string_representation.size)
                                  && EVALUATE(a_rep.xml_string_representation(), c_rep._.string_representation.data);
                    }
                    break;
                    case REPRESENTATION_IN_BINARY:
                    {
                        res = res && EVALUATE(a_rep.binary_representation().size(), (uint32_t)c_rep._.binary_representation.size)
                                  && EVALUATE(are_the_same(a_rep.binary_representation(),
                                                           c_rep._.binary_representation.data,
                                                           (uint32_t)c_rep._.binary_representation.size),
                                              true);
                    }
                    break;
                }

            }
            break;
            case OBJK_DATAREADER:
            {
                /*res = res && (EVALUATE(a_payload.object_representation().data_reader().participant_id(),
                                       c_payload.object.variant.data_reader.participant_id)
                              &&
                              EVALUATE(a_payload.object_representation().data_reader().subscriber_id(),
                                       c_payload.object.variant.data_reader.subscriber_id));*/
            }
            break;
            case OBJK_SUBSCRIBER:
            {
                /*res = res && EVALUATE(a_payload.object_representation().subscriber().participant_id(),
                                      c_payload.object.variant.subscriber.participant_id);*/
            }
            break;
            case OBJK_PUBLISHER:
            {
                /*res = res && EVALUATE(a_payload.object_representation().publisher().participant_id(),
                                      c_payload.object.variant.publisher.participant_id);*/
            }
            break;
            case OBJK_TYPE:
            {
                // TODO
                // OBJK_TYPE_Representation type_;
            }
            break;
            case OBJK_QOSPROFILE:
            {
                // TODO
                // OBJK_QOSPROFILE_Representation qos_profile_;
            }
            break;
            case OBJK_APPLICATION:
            {
                // TODO
                // OBJK_APPLICATION_Representation application_;
            }
            break;
            case OBJK_INVALID:
            default:
            {
                res = false;
            }
            break;
        }

        return res;
    }


    static bool check_write_payload(const agent_write_payload& a_payload, const client_write_payload& c_payload)
    {
        bool res = (EVALUATE(a_payload.request_id(),         c_payload.request.base.request_id) &&
                    EVALUATE(a_payload.object_id(),          c_payload.request.object_id)  &&
                    EVALUATE(a_payload.data_to_write()._d(), c_payload.data_to_write.format));

        switch (a_payload.data_to_write()._d())
        {
            case FORMAT_DATA:
            {
                res = res && EVALUATE(are_the_same(a_payload.data_to_write().data().serialized_data(),
                                                   c_payload.data_to_write._.data.data,
                                                   c_payload.data_to_write._.data.size), true);

            }
            break;
            case FORMAT_DATA_SEQ:
                // TODO
                // SampleDataSeq data_seq_;
            break;
            case FORMAT_SAMPLE:
            {
                res = res && (EVALUATE(a_payload.data_to_write().sample().info().state(),
                                       c_payload.data_to_write._.sample.info.state)
                              &&
                              EVALUATE(a_payload.data_to_write().sample().info().sequence_number(),
                                       c_payload.data_to_write._.sample.info.sequence_number)
                              &&
                              EVALUATE(a_payload.data_to_write().sample().info().session_time_offset(),
                                       c_payload.data_to_write._.sample.info.session_time_offset)
                              &&
                              EVALUATE(are_the_same(a_payload.data_to_write().sample().data().serialized_data(),
                                                    c_payload.data_to_write._.sample.data.data,
                                                    c_payload.data_to_write._.sample.data.size), true));
            }
            break;
            case FORMAT_SAMPLE_SEQ:
                // TODO
                // SampleSeq sample_seq_;
            break;
            case FORMAT_PACKED_SAMPLES:
                // TODO
                // SamplePackedSeq sample_packed_seq_;
            break;
            default:
                res = false;
            break;
        }

        return res;
    }

    static bool check_read_payload(const agent_read_payload& a_payload, const client_read_payload& c_payload)
    {
        bool res = (EVALUATE(a_payload.request_id(), c_payload.request.base.request_id)
                    &&
                    EVALUATE(a_payload.object_id(), c_payload.request.object_id)
                    &&
                    EVALUATE(a_payload.read_specification().delivery_config()._d(),
                             (uint8_t)c_payload.read_specification.optional_delivery_config)
                    );

        switch(a_payload.read_specification().delivery_config()._d())
        {
            case FORMAT_DATA_SEQ:
            case FORMAT_SAMPLE_SEQ:
            case FORMAT_PACKED_SAMPLES:
            {
                res = res &&
                        EVALUATE(a_payload.read_specification().delivery_config().delivery_control().max_elapsed_time(),
                                 c_payload.read_specification.delivery_config.max_elapsed_time)
                        &&
                        EVALUATE(a_payload.read_specification().delivery_config().delivery_control().max_rate(),
                                 c_payload.read_specification.delivery_config.max_samples)
                        &&
                        EVALUATE(a_payload.read_specification().delivery_config().delivery_control().max_samples(),
                                 c_payload.read_specification.delivery_config.max_samples)
                        &&
                        EVALUATE(a_payload.read_specification().has_content_filter_expresion(),
                                 c_payload.read_specification.optional_content_filter_expression)
                        &&
                        (a_payload.read_specification().has_content_filter_expresion()?
                                EVALUATE(a_payload.read_specification().content_filter_expression().compare(
                                                      c_payload.read_specification.content_filter_expression.data),
                                         0)
                                : true);
            }
            break;
            default:
            break;
        }

        return res;
    }

    static bool check_delete_payload(const agent_delete_payload& a_payload, const client_delete_payload& c_payload)
    {

        bool res = (EVALUATE(a_payload.request_id(), c_payload.request.base.request_id) &&
                    EVALUATE(a_payload.object_id(),  c_payload.request.object_id));
        return res;
    }

protected:
    ClientToAgent_CrossSerializationTests(): client((uint8_t*)test_buffer)
    {
    }

    virtual ~ClientToAgent_CrossSerializationTests()
    {
    }


    Client client;
    Agent agent;
    char test_buffer[BUFFER_SIZE];

};

/* ############################################## TESTS ##################################################### */


TEST_F(ClientToAgent_CrossSerializationTests, CreateMessage)
{
    /// CLIENT serialization
    // [CREATE] SUBMESSAGE
    // Writing a message
    client.fill_client_create_payload(client_create_payload_var);
    client_create_payload payload = client_create_payload_var;
    add_create_resource_submessage(&client.output_message, &payload, CREATION_MODE_REPLACE); //This function calls the OutputMessageCallbacks

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    PRINTL_SERIALIZATION("", client.output_message.writer.init, seliarized_size);

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);

}

TEST_F(ClientToAgent_CrossSerializationTests, MultiCreateMessage)
{
    const int num_msg = 3;
    /// CLIENT serialization
    // [CREATE] SUBMESSAGE
    client.fill_client_create_payload(client_create_payload_var);
    add_create_resource_submessage(&client.output_message, &client_create_payload_var, CREATION_MODE_REPLACE); //This function calls the OutputMessageCallbacks
    add_create_resource_submessage(&client.output_message, &client_create_payload_var, CREATION_MODE_REPLACE); //This function calls the OutputMessageCallbacks
    add_create_resource_submessage(&client.output_message, &client_create_payload_var, CREATION_MODE_REPLACE); //This function calls the OutputMessageCallbacks

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    PRINTL_SERIALIZATION("", client.output_message.writer.init, seliarized_size);

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_EQ(agent.listener.msg_counter, num_msg);
}

TEST_F(ClientToAgent_CrossSerializationTests, WriteMessage)
{
    /// CLIENT serialization
    // [WRITE] SUBMESSAGE
    client.fill_client_write_payload(client_write_payload_var);
    add_write_data_submessage(&client.output_message, &client_write_payload_var);

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    PRINTL_SERIALIZATION("", client.output_message.writer.init, seliarized_size);

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);
}

TEST_F(ClientToAgent_CrossSerializationTests, MultiWriteMessage)
{
    const int num_msg = 3;
    /// CLIENT serialization
    // [WRITE] SUBMESSAGE
    client.fill_client_write_payload(client_write_payload_var);
    add_write_data_submessage(&client.output_message, &client_write_payload_var);
    add_write_data_submessage(&client.output_message, &client_write_payload_var);
    add_write_data_submessage(&client.output_message, &client_write_payload_var);

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    PRINTL_SERIALIZATION("", client.output_message.writer.init, seliarized_size);

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_EQ(agent.listener.msg_counter, num_msg);
}

TEST_F(ClientToAgent_CrossSerializationTests, ReadMessage)
{
    /// CLIENT serialization
    // [READ] SUBMESSAGE
    client.fill_client_read_payload(client_read_payload_var);
    add_read_data_submessage(&client.output_message, &client_read_payload_var);

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    PRINTL_SERIALIZATION("", client.output_message.writer.init, seliarized_size);

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);
}

TEST_F(ClientToAgent_CrossSerializationTests, DeleteMessage)
{
    /// CLIENT serialization
    // [DELETE] SUBMESSAGE
    client_delete_payload_var.request.base.request_id = 0x1234;
    client_delete_payload_var.request.object_id = 0xABCD;

    add_delete_resource_submessage(&client.output_message,  &client_delete_payload_var);

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    PRINTL_SERIALIZATION("", client.output_message.writer.init, seliarized_size);

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);
}

/*TEST_F(ClientToAgent_CrossSerializationTests, StatusMessage)
{
    micrortps::XRCEFactory newMessage{test_buffer_, BUFFER_LENGTH};

    MessageHeader message_header = generate_message_header();
    newMessage.header(message_header);
    micrortps::RESOURCE_STATUS_Payload resource_status = generate_resource_status_payload(STATUS_LAST_OP_NONE, STATUS_OK);
    newMessage.status(resource_status);
    SubmessageHeader submessage_header =
        generate_submessage_header(STATUS, static_cast<uint16_t>(resource_status.getCdrSerializedSize()));

    micrortps::MessageHeader deserialized_header;
    micrortps::SubmessageHeader deserialized_submessage_header;
    micrortps::RESOURCE_STATUS_Payload deserialized_status;
    deserializer_.deserialize(deserialized_header);
    deserializer_.deserialize(deserialized_submessage_header);
    deserializer_.deserialize(deserialized_status);

    ASSERT_EQ(client_key, deserialized_header.client_key());
    ASSERT_EQ(session_id, deserialized_header.session_id());
    ASSERT_EQ(stream_id, deserialized_header.stream_id());
    ASSERT_EQ(sequence_nr, deserialized_header.sequence_nr());

    ASSERT_EQ(submessage_header.submessage_id(), deserialized_submessage_header.submessage_id());
    ASSERT_EQ(submessage_header.flags(), deserialized_submessage_header.flags());
    ASSERT_EQ(submessage_header.submessage_length(), deserialized_submessage_header.submessage_length());

    ASSERT_EQ(resource_status.object_id(), deserialized_status.object_id());
    ASSERT_EQ(resource_status.request_id(), deserialized_status.request_id());
    ASSERT_EQ(resource_status.result().status(), deserialized_status.result().status());
    ASSERT_EQ(resource_status.result().implementation_status(), deserialized_status.result().implementation_status());

}*/

