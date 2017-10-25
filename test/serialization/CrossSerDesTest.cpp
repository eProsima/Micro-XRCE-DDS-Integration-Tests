#include <gtest/gtest.h>

#include "MessageHeader.h"
#include "SubMessageHeader.h"
#include "Payloads.h"
#include "XRCEParser.h"
#include <micrortps/client/output_message.h>
#include <micrortps/client/xrce_protocol_spec.h>

#define BUFFER_SIZE 256

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

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}

typedef micrortps::MessageHeader agent_header;
typedef MessageHeader            client_header;
typedef micrortps::SubmessageHeader     agent_subheader;
typedef SubmessageHeader client_subheader;
typedef micrortps::CREATE_Payload       agent_create_payload;
typedef CreateResourcePayload    client_create_payload;
typedef micrortps::WRITE_DATA_Payload   agent_write_payload;
typedef WriteDataPayload client_write_payload;
typedef micrortps::READ_DATA_Payload    agent_read_payload;
typedef ReadDataPayload  client_read_payload;
typedef micrortps::DELETE_RESOURCE_Payload       agent_delete_payload;
typedef DeleteResourcePayload    client_delete_payload;

ClientKey client_key = {0xF1, 0xF2, 0xF3, 0xF4};

client_header client_header_var = {
        .session_id = 0xFF,
        .stream_id = 0x04,
        .sequence_nr = 0x0200,
};

client_subheader      client_subheader_var = {};
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

void on_initialize_submessage(client_subheader* header, void* vstate)
{
    client_subheader_var = *header;
}

/*RequestId& set_request_id(RequestId& r_id, const uint16_t val)
{
    r_id.data[0] = (0xFF & (val >>  0));
    r_id.data[1] = (0xFF & (val >>  8));
    return r_id;
}

ObjectId& set_object_id(ObjectId& o_id, const uint16_t val)
{
    o_id.data[0] = (0xFF & (val >>  0));
    o_id.data[1] = (0xFF & (val >>  8));
    return o_id;
}*/

bool operator==(const std::array<uint8_t, 2>& left, const uint16_t right)
{
    return ((left[0] == (0xFF & (right >>  0))) &&
            (left[1] == (0xFF & (right >>  8))));
}

bool operator==(const uint16_t left, const std::array<uint8_t, 2>& right)
{
    return right == left;
}

bool operator==(const std::array<uint8_t, 3>& left, const uint32_t right)
{
    return ((left[0] == (0xFF & (right >>  0))) &&
            (left[1] == (0xFF & (right >>  8))) &&
            (left[2] == (0xFF & (right >> 16))));
}

bool operator==(const uint32_t left, const std::array<uint8_t, 3>& right)
{
    return right == left;
}

bool operator==(const std::array<uint8_t, 4>& left, const uint32_t& right)
{
    return ((left[0] == (0xFF & (right >>  0))) &&
            (left[1] == (0xFF & (right >>  8))) &&
            (left[2] == (0xFF & (right >> 16))) &&
            (left[3] == (0xFF & (right >> 24))));
}

bool operator==(const micrortps::ClientKey& left, const ClientKey& right)
{
    return ((left[0] == right.data[0]) &&
            (left[1] == right.data[1]) &&
            (left[2] == right.data[2]) &&
            (left[3] == right.data[3]));
}

bool operator==(const uint32_t& left, const std::array<uint8_t, 4>& right)
{
    return right == left;
}


bool are_the_same(const std::vector<uint8_t>& _vector, const uint8_t* _array, const uint32_t& array_length)
{
    return array_length == _vector.size() &&
           std::equal(_vector.begin(), _vector.end(), _array, [](const uint8_t& a, const uint8_t& b){return a == b;});
}


class CrossSerializationTests : public testing::Test
{
public:

    virtual void SetUp()
    {
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
            output_callback.on_initialize_submessage = on_initialize_submessage;
            init_output_message(&output_message, output_callback, buffer, BUFFER_SIZE);
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
            payload.read_specification.optional_delivery_config = FORMAT_SAMPLE;
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
                                 check_submessage_header(sub_header, client_subheader_var) &&
                                 check_create_payload(create_payload, client_create_payload_var));
                if (content_match) ++msg_counter;
            }

            void on_message(const micrortps::MessageHeader& header, const micrortps::SubmessageHeader& sub_header, const micrortps::DELETE_RESOURCE_Payload& delete_payload) override
            {
                content_match = (check_message_header(header, client_header_var) &&
                                 check_submessage_header(sub_header, client_subheader_var) &&
                                 check_delete_payload(delete_payload, client_delete_payload_var));
                if (content_match) ++msg_counter;
            }

            void on_message(const micrortps::MessageHeader& header, const micrortps::SubmessageHeader& sub_header, const micrortps::WRITE_DATA_Payload&  write_payload) override
            {
                content_match = (check_message_header(header, client_header_var) &&
                                 check_submessage_header(sub_header, client_subheader_var) &&
                                 check_write_payload(write_payload, client_write_payload_var));
                if (content_match) ++msg_counter;
            }

            void on_message(const micrortps::MessageHeader& header, const micrortps::SubmessageHeader& sub_header, const micrortps::READ_DATA_Payload&   read_payload) override
            {
                content_match = (check_message_header(header, client_header_var) &&
                                 check_submessage_header(sub_header, client_subheader_var) &&
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
        return (EVALUATE(a_subheader.submessage_id(),     c_subheader.id)    &&
                EVALUATE(a_subheader.flags(),             c_subheader.flags) &&
                EVALUATE(a_subheader.submessage_length(), c_subheader.length));
    }

    static bool check_create_payload(const agent_create_payload& a_payload, const client_create_payload& c_payload)
    {
        bool res = (EVALUATE(a_payload.request_id(),                 c_payload.request.base.request_id) &&
                    EVALUATE(a_payload.object_id(),                  c_payload.request.object_id)  &&
                    EVALUATE(a_payload.object_representation()._d(), c_payload.representation.kind));

        switch (a_payload.object_representation()._d())
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
                             (uint8_t)c_payload.read_specification.optional_delivery_config) // TODO: must be different type, now bool from uint8_t
                    &&
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
                            : true)
                    );
        return res;
    }

    static bool check_delete_payload(const agent_delete_payload& a_payload, const client_delete_payload& c_payload)
    {

        bool res = (EVALUATE(a_payload.request_id(), c_payload.request.base.request_id) &&
                    EVALUATE(a_payload.object_id(),  c_payload.request.object_id));
        return res;
    }

protected:
    CrossSerializationTests(): client((uint8_t*)test_buffer)
    {
    }

    virtual ~CrossSerializationTests()
    {
    }


    Client client;
    Agent agent;
    char test_buffer[BUFFER_SIZE];

};

/* ############################################## TESTS ##################################################### */


TEST_F(CrossSerializationTests, CreateMessage)
{
    /// CLIENT serialization
    // [CREATE] SUBMESSAGE
    // Writing a message
    client.fill_client_create_payload(client_create_payload_var);
    add_create_resource_submessage(&client.output_message, &client_create_payload_var, CREATION_MODE_REPLACE); //This function calls the OutputMessageCallbacks

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);

}

TEST_F(CrossSerializationTests, MultiCreateMessage)
{
    const int num_msg = 3;
    /// CLIENT serialization
    // [CREATE] SUBMESSAGE
    client.fill_client_create_payload(client_create_payload_var);
    add_create_resource_submessage(&client.output_message, &client_create_payload_var, CREATION_MODE_REPLACE); //This function calls the OutputMessageCallbacks
    add_create_resource_submessage(&client.output_message, &client_create_payload_var, CREATION_MODE_REPLACE); //This function calls the OutputMessageCallbacks
    add_create_resource_submessage(&client.output_message, &client_create_payload_var, CREATION_MODE_REPLACE); //This function calls the OutputMessageCallbacks

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_EQ(agent.listener.msg_counter, num_msg);
}

TEST_F(CrossSerializationTests, WriteMessage)
{
    /// CLIENT serialization
    // [WRITE] SUBMESSAGE
    client.fill_client_write_payload(client_write_payload_var);
    add_write_data_submessage(&client.output_message, &client_write_payload_var);

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);
}

TEST_F(CrossSerializationTests, MultiWriteMessage)
{
    const int num_msg = 3;
    /// CLIENT serialization
    // [WRITE] SUBMESSAGE
    client.fill_client_write_payload(client_write_payload_var);
    add_write_data_submessage(&client.output_message, &client_write_payload_var);
    add_write_data_submessage(&client.output_message, &client_write_payload_var);
    add_write_data_submessage(&client.output_message, &client_write_payload_var);

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_EQ(agent.listener.msg_counter, num_msg);
}

TEST_F(CrossSerializationTests, ReadMessage)
{
    /// CLIENT serialization
    // [READ] SUBMESSAGE
    client.fill_client_read_payload(client_read_payload_var);
    add_read_data_submessage(&client.output_message, &client_read_payload_var);

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);
}

TEST_F(CrossSerializationTests, DeleteMessage)
{
    /// CLIENT serialization
    // [DELETE] SUBMESSAGE
    client_delete_payload_var.request.base.request_id = 0x12345678;
    client_delete_payload_var.request.object_id = 0xABCDEF;

    add_delete_resource_submessage(&client.output_message,  &client_delete_payload_var);

    uint32_t seliarized_size = client.output_message.writer.iterator - client.output_message.writer.init;

    /// AGENT deserialization
    micrortps::XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);
}
