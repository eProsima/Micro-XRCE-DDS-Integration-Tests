#include <gtest/gtest.h>

#include "agent/MessageHeader.h"
#include "agent/SubMessageHeader.h"
#include "agent/Payloads.h"
#include "agent/XRCEParser.h"
#include "micrortps/client/xrce_spec.h"
#include <micrortps/client/message.h>
#include <micrortps/client/serialization.h>
#include <micrortps/client/debug/message_debugger.h>

#define BUFFER_SIZE 256

using namespace eprosima::micrortps;

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

int init_message_header(MessageHeaderSpec* header, void* data)
{
    header->client_key = 0xF1F2F3F4;
    header->session_id = 0x01;
    header->stream_id = 0x04;
    header->sequence_number = 0x0200;

    return 1;
}

typedef MessageHeader        agent_header;
typedef MessageHeaderSpec    client_header;
typedef SubmessageHeader     agent_subheader;
typedef SubmessageHeaderSpec client_subheader;
typedef CREATE_PAYLOAD       agent_create_payload;
typedef CreatePayloadSpec    client_create_payload;
typedef WRITE_DATA_PAYLOAD   agent_write_payload;
typedef WriteDataPayloadSpec client_write_payload;
typedef READ_DATA_PAYLOAD    agent_read_payload;
typedef ReadDataPayloadSpec  client_read_payload;
typedef DELETE_PAYLOAD       agent_delete_payload;
typedef DeletePayloadSpec    client_delete_payload;


client_header client_header_var = {
        .client_key = 0xF1F2F3F4,
        .session_id = 0x01,
        .stream_id = 0x04,
        .sequence_number = 0x0200,
};

client_subheader client_subheader_var = {
        .id = 0x00,
        .flags = 0x00,
        .length = 0x0000,
};

client_create_payload client_create_payload_var = {};
client_write_payload client_write_payload_var = {};
client_read_payload client_read_payload_var = {};
client_delete_payload client_delete_payload_var = {};


bool operator==(const std::array<uint8_t, 2>& left, const uint16_t right)
{
    return ((left[0] == (0xFF & (right >>  0))) &&
            (left[1] == (0xFF & (right >>  8))));
}

bool operator==(const uint16_t left, const std::array<uint8_t, 2>& right)
{
    return right == left;
}

bool operator==(const std::array<uint8_t, 3>& left, const uint_least24_t right)
{
    return ((left[0] == (0xFF & (right >>  0))) &&
            (left[1] == (0xFF & (right >>  8))) &&
            (left[2] == (0xFF & (right >> 16))));
}

bool operator==(const uint_least24_t left, const std::array<uint8_t, 3>& right)
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
            callback.on_initialize_message = init_message_header;
            callback.on_message_header = print_message_header;
            callback.on_submessage_header = print_submessage_header;
            callback.on_create_resource = print_create_submessage;
            callback.on_delete_resource = print_delete_submessage;
            callback.on_status = print_status_submessage;
            callback.on_write_data = print_write_data_submessage;
            callback.on_read_data = print_read_data_submessage;
            callback.on_data = print_data_submessage;
            callback.data = NULL;
            init_message_manager(&message_manager, buffer, BUFFER_SIZE, buffer, BUFFER_SIZE, callback);
        }

        virtual ~Client()
        {
            destroy_message_manager(&message_manager);
        }

        bool fill_client_create_payload(client_create_payload& payload)
        {
            payload.request_id = 0xAABBCCDD;
            payload.object_id = 0x778899;
            payload.object.kind = OBJECT_KIND_DATA_WRITER;

            // TODO: string
            //char string[] = "Hello world";
            //payload.object.string = string;
            //payload.object.string_size = strlen(string) + 1;

            switch(payload.object.kind)
            {
                case OBJECT_KIND_DATA_WRITER:
                    payload.object.variant.data_writer.participant_id = 0xAAAAAA;
                    payload.object.variant.data_writer.publisher_id = 0xBBBBBB;
                break;

                case OBJECT_KIND_DATA_READER:
                    payload.object.variant.data_reader.participant_id = 0xCCCCCC;
                    payload.object.variant.data_reader.subscriber_id = 0xDDDDDD;
                break;

                case OBJECT_KIND_SUBSCRIBER:
                    payload.object.variant.subscriber.participant_id = 0xEEEEEE;
                break;

                case OBJECT_KIND_PUBLISHER:
                    payload.object.variant.publisher.participant_id = 0xFFFFFF;
                break;
            }

            return true;
        }

        bool fill_client_write_payload(client_write_payload& payload)
        {
            uint8_t* data = new uint8_t[5];
            for (uint8_t i = 0; i < 5; ++i) data[i] = i + 1;

            payload.request_id = 0xAABBCCDD;
            payload.object_id = 0x778899;
            payload.data_writer.read_mode = READ_MODE_SAMPLE;

            SampleKindSpec* kind = &payload.data_writer.sample_kind;
            switch(payload.data_writer.read_mode)
            {
                case READ_MODE_DATA:
                    kind->data.serialized_data = data;
                    kind->data.serialized_data_size = 5;
                break;

                case READ_MODE_SAMPLE:
                    kind->sample.info.state = 0x01;
                    kind->sample.info.sequence_number = 0x01234567;
                    kind->sample.info.session_time_offset = 0xAAAABBBB;
                    kind->sample.data.serialized_data = data;
                    kind->sample.data.serialized_data_size = 5;
                break;
            }
            return true;
        }

        bool fill_client_read_payload(client_read_payload& payload)
        {
            //char expresion[] = "EXPRESION";

            payload.request_id = 0x11223344;
            payload.object_id = 0x778899;
            payload.max_messages = 12345;
            payload.read_mode = 0x02;
            payload.max_elapsed_time = 987654321;
            payload.max_rate = 123123123;
            //payload.content_filter_expression = expresion;
            //payload.expression_size = strlen(expresion) + 1;
            payload.max_samples = 55555;
            payload.include_sample_info = 1;
            return true;
        }


        MessageCallback callback = {};
        MessageManager message_manager = {};
    };

    class Agent
    {
    public:
        Agent()
        {
        }

        virtual ~Agent(){}

        class Listener : public XRCEListener
        {
        public:
            void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const CREATE_PAYLOAD& create_payload) override
            {
                content_match = (check_message_header(header, client_header_var) &&
                                 check_submessage_header(sub_header, client_subheader_var) &&
                                 check_create_payload(create_payload, client_create_payload_var));
                if (content_match) ++msg_counter;
            }

            void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const DELETE_PAYLOAD& delete_payload) override
            {
                content_match = (check_message_header(header, client_header_var) &&
                                 check_submessage_header(sub_header, client_subheader_var) &&
                                 check_delete_payload(delete_payload, client_delete_payload_var));
                if (content_match) ++msg_counter;
            }

            void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const WRITE_DATA_PAYLOAD&  write_payload) override
            {
                content_match = (check_message_header(header, client_header_var) &&
                                 check_submessage_header(sub_header, client_subheader_var) &&
                                 check_write_payload(write_payload, client_write_payload_var));
                if (content_match) ++msg_counter;
            }

            void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const READ_DATA_PAYLOAD&   read_payload) override
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
        return (EVALUATE(a_header.client_key(),  c_header.client_key) &&
                EVALUATE(a_header.session_id(),  c_header.session_id) &&
                EVALUATE(a_header.stream_id(),   c_header.stream_id)  &&
                EVALUATE(a_header.sequence_nr(), c_header.sequence_number));
    }

    static bool check_submessage_header(const agent_subheader& a_subheader,const client_subheader& c_subheader)
    {
        return (EVALUATE(a_subheader.submessage_id(),     c_subheader.id)    &&
                EVALUATE(a_subheader.flags(),             c_subheader.flags) &&
                EVALUATE(a_subheader.submessage_length(), c_subheader.length));
    }

    static bool check_create_payload(const agent_create_payload& a_payload, const client_create_payload& c_payload)
    {
        bool res = (EVALUATE(a_payload.request_id(),                            c_payload.request_id) &&
                    EVALUATE(a_payload.object_id(),                             c_payload.object_id)  &&
                    EVALUATE(a_payload.object_representation().discriminator(), c_payload.object.kind));


        // TODO: compare payload.object.string

        switch (a_payload.object_representation().discriminator())
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
                                       c_payload.object.variant.data_writer.participant_id)
                              &&
                              EVALUATE(a_payload.object_representation().data_writer().publisher_id(),
                                       c_payload.object.variant.data_writer.publisher_id));
            }
            break;
            case OBJK_DATAREADER:
            {
                res = res && (EVALUATE(a_payload.object_representation().data_reader().participant_id(),
                                       c_payload.object.variant.data_reader.participant_id)
                              &&
                              EVALUATE(a_payload.object_representation().data_reader().subscriber_id(),
                                       c_payload.object.variant.data_reader.subscriber_id));
            }
            break;
            case OBJK_SUBSCRIBER:
            {
                res = res && EVALUATE(a_payload.object_representation().subscriber().participant_id(),
                                      c_payload.object.variant.subscriber.participant_id);
            }
            break;
            case OBJK_PUBLISHER:
            {
                res = res && EVALUATE(a_payload.object_representation().publisher().participant_id(),
                                      c_payload.object.variant.publisher.participant_id);
            }
            break;
            case OBJK_TYPE:
                // TODO
                // OBJK_TYPE_Representation type_;
            break;
            case OBJK_QOSPROFILE:
                // TODO
                // OBJK_QOSPROFILE_Representation qos_profile_;
            break;
            case OBJK_APPLICATION:
                // TODO
                // OBJK_APPLICATION_Representation application_;
            break;
            case OBJK_CLIENT:
            {
                res = res && (EVALUATE(a_payload.object_representation().client().xrce_cookie(),
                                       c_payload.object.variant.client.xrce_cookie)
                              &&
                              EVALUATE(a_payload.object_representation().client().xrce_version(),
                                       c_payload.object.variant.client.xrce_version)
                              &&
                              EVALUATE(a_payload.object_representation().client().xrce_vendor_id(),
                                       c_payload.object.variant.client.xrce_vendor_id)
                              &&
                              EVALUATE(a_payload.object_representation().client().client_timestamp().seconds(),
                                       c_payload.object.variant.client.timestamp.seconds)
                              &&
                              EVALUATE(a_payload.object_representation().client().client_timestamp().nanoseconds(),
                                       c_payload.object.variant.client.timestamp.nanoseconds));

            }
            break;
            case OBJK_INVALID:
            default:
                res = false;
            break;
        }

        return res;
    }


    static bool check_write_payload(const agent_write_payload& a_payload, const client_write_payload& c_payload)
    {
        bool res = (EVALUATE(a_payload.request_id(), c_payload.request_id) &&
                    EVALUATE(a_payload.object_id(),  c_payload.object_id)  &&
                    EVALUATE(a_payload.data_writer()._d(), c_payload.data_writer.read_mode));

        switch (a_payload.data_writer()._d())
        {
            case READM_DATA:
            {
                res = res && are_the_same(a_payload.data_writer().data().serialized_data(),
                                          c_payload.data_writer.sample_kind.data.serialized_data,
                                          c_payload.data_writer.sample_kind.data.serialized_data_size);

            }
            break;
            case READM_DATA_SEQ:
                // TODO
                // SampleDataSeq data_seq_;
            break;
            case READM_SAMPLE:
            {
                res = res && (EVALUATE(a_payload.data_writer().sample().info().state(),
                                       c_payload.data_writer.sample_kind.sample.info.state)
                              &&
                              EVALUATE(a_payload.data_writer().sample().info().sequence_number(),
                                       c_payload.data_writer.sample_kind.sample.info.sequence_number)
                              &&
                              EVALUATE(a_payload.data_writer().sample().info().session_time_offset(),
                                       c_payload.data_writer.sample_kind.sample.info.session_time_offset)
                              &&
                              are_the_same(a_payload.data_writer().sample().data().serialized_data(),
                                           c_payload.data_writer.sample_kind.sample.data.serialized_data,
                                           c_payload.data_writer.sample_kind.sample.data.serialized_data_size));
            }
            break;
            case READM_SAMPLE_SEQ:
                // TODO
                // SampleSeq sample_seq_;
            break;
            case READM_PACKED_SAMPLE_SEQ:
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
        bool res = (EVALUATE(a_payload.request_id(),          c_payload.request_id) &&
                    EVALUATE(a_payload.object_id(),           c_payload.object_id)  &&
                    EVALUATE(a_payload.max_messages(),        c_payload.max_messages)  &&
                    EVALUATE(a_payload.read_mode(),           c_payload.read_mode)  &&
                    EVALUATE(a_payload.max_elapsed_time(),    c_payload.max_elapsed_time)  &&
                    EVALUATE(a_payload.max_rate(),            c_payload.max_rate)  &&
                    EVALUATE(a_payload.max_samples(),         c_payload.max_samples)  &&
                    EVALUATE(a_payload.include_sample_info(), c_payload.include_sample_info? true: false));
        return res;
    }

    static bool check_delete_payload(const agent_delete_payload& a_payload, const client_delete_payload& c_payload)
    {

        bool res = (EVALUATE(a_payload.request_id(),          c_payload.request_id) &&
                    EVALUATE(a_payload.object_id(),           c_payload.object_id));
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
    client.fill_client_create_payload(client_create_payload_var);
    add_create_submessage(&client.message_manager, &client_create_payload_var);
    // Set aux variable
    client_subheader_var.id = SUBMESSAGE_CREATE;
    client_subheader_var.flags = 0x07;
    client_subheader_var.length = size_of_create_payload(&client_create_payload_var);

    uint32_t seliarized_size = client.message_manager.writer.iterator - client.message_manager.writer.data;

    /// AGENT deserialization
    XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);

}

TEST_F(CrossSerializationTests, MultiCreateMessage)
{
    const int num_msg = 3;
    /// CLIENT serialization
    // [CREATE] SUBMESSAGE
    client.fill_client_create_payload(client_create_payload_var);
    add_create_submessage(&client.message_manager, &client_create_payload_var);
    add_create_submessage(&client.message_manager, &client_create_payload_var);
    add_create_submessage(&client.message_manager, &client_create_payload_var);
    // Set aux variable
    client_subheader_var.id = SUBMESSAGE_CREATE;
    client_subheader_var.flags = 0x07;
    client_subheader_var.length = size_of_create_payload(&client_create_payload_var);

    uint32_t seliarized_size = client.message_manager.writer.iterator - client.message_manager.writer.data;

    /// AGENT deserialization
    XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_EQ(agent.listener.msg_counter, num_msg);
}

TEST_F(CrossSerializationTests, WriteMessage)
{
    /// CLIENT serialization
    // [WRITE] SUBMESSAGE
    client.fill_client_write_payload(client_write_payload_var);
    add_write_data_submessage(&client.message_manager, &client_write_payload_var);
    // Set aux variable
    client_subheader_var.id = SUBMESSAGE_WRITE_DATA;
    client_subheader_var.flags = 0x07;
    client_subheader_var.length = size_of_write_data_payload(&client_write_payload_var);

    uint32_t seliarized_size = client.message_manager.writer.iterator - client.message_manager.writer.data;

    /// AGENT deserialization
    XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);
}

TEST_F(CrossSerializationTests, MultiWriteMessage)
{
    const int num_msg = 3;
    /// CLIENT serialization
    // [WRITE] SUBMESSAGE
    client.fill_client_write_payload(client_write_payload_var);
    add_write_data_submessage(&client.message_manager, &client_write_payload_var);
    add_write_data_submessage(&client.message_manager, &client_write_payload_var);
    add_write_data_submessage(&client.message_manager, &client_write_payload_var);
    // Set aux variable
    client_subheader_var.id = SUBMESSAGE_WRITE_DATA;
    client_subheader_var.flags = 0x07;
    client_subheader_var.length = size_of_write_data_payload(&client_write_payload_var);

    uint32_t seliarized_size = client.message_manager.writer.iterator - client.message_manager.writer.data;

    /// AGENT deserialization
    XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_EQ(agent.listener.msg_counter, num_msg);
}

TEST_F(CrossSerializationTests, ReadMessage)
{
    /// CLIENT serialization
    // [READ] SUBMESSAGE
    client.fill_client_read_payload(client_read_payload_var);
    add_read_data_submessage(&client.message_manager, &client_read_payload_var);
    // Set aux variable
    client_subheader_var.id = SUBMESSAGE_READ_DATA;
    client_subheader_var.flags = 0x07;
    client_subheader_var.length = size_of_read_data_payload(&client_read_payload_var);

    uint32_t seliarized_size = client.message_manager.writer.iterator - client.message_manager.writer.data;

    /// AGENT deserialization
    XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);
}

TEST_F(CrossSerializationTests, DeleteMessage)
{
    /// CLIENT serialization
    // [DELETE] SUBMESSAGE
    client_delete_payload_var.request_id = 0x12345678;
    client_delete_payload_var.object_id = 0xABCDEF;

    add_delete_submessage(&client.message_manager,  &client_delete_payload_var);

    // Set aux variable
    client_subheader_var.id = SUBMESSAGE_DELETE;
    client_subheader_var.flags = 0x07;
    client_subheader_var.length = size_of_delete_payload(&client_delete_payload_var);

    uint32_t seliarized_size = client.message_manager.writer.iterator - client.message_manager.writer.data;

    /// AGENT deserialization
    XRCEParser myParser{(char*)test_buffer, seliarized_size, &agent.listener};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(agent.listener.content_match);
}
