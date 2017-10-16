#include <gtest/gtest.h>

#include "agent/MessageHeader.h"
#include "agent/SubMessageHeader.h"
#include "agent/Payloads.h"
#include "agent/XRCEParser.h"
#include "micrortps/client/xrce_spec.h"
#include <micrortps/client/message.h>
#include <micrortps/client/debug/message_debugger.h>

#define BUFFER_SIZE 256

using namespace eprosima::micrortps;

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


class CrossSerializationTests : public testing::Test
{
public:

    class Client
    {
    public:
        Client()
        {
        }

        virtual ~Client(){}
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
                //TODO write tests on this

            }

            virtual void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const DELETE_PAYLOAD& create_payload)
            {
                //TODO write tests on this
            }

            virtual void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const WRITE_DATA_PAYLOAD&  write_payload)
            {
                //TODO write tests on this
            }

            virtual void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const READ_DATA_PAYLOAD&   read_payload)
            {
                //TODO write tests on this
            }

        };
    };

    class BoolListener : public XRCEListener
    {
    public:
        void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const CREATE_PAYLOAD& create_payload) override
        {
            create_called = true;

        }

        virtual void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const DELETE_PAYLOAD& create_payload)
        {
            //TODO write tests on this
        }

        virtual void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const WRITE_DATA_PAYLOAD&  write_payload)
        {
            write_called = true;
        }

        virtual void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const READ_DATA_PAYLOAD&   read_payload)
        {
            read_called = true;
        }

        bool create_called = false;
        bool write_called = false;
        bool read_called = false;
    };

    class CountListener : public XRCEListener
    {
    public:
        void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const CREATE_PAYLOAD& create_payload) override
        {
            ++creates;
        }

        virtual void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const DELETE_PAYLOAD& create_payload)
        {
            //TODO write tests on this
        }

        virtual void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const WRITE_DATA_PAYLOAD&  write_payload)
        {
            ++writes;
        }

        virtual void on_message(const MessageHeader& header, const SubmessageHeader& sub_header, const READ_DATA_PAYLOAD&   read_payload)
        {
            ++reads;
        }

        int creates = 0;
        int writes = 0;
        int reads = 0;
    };

protected:
    CrossSerializationTests()
    {
    }

    virtual ~CrossSerializationTests()
    {
    }

    char test_buffer[BUFFER_SIZE];

    BoolListener bool_listener_;
    CountListener count_listener_;


    // Header
    const uint32_t client_key = 0xF1F2F3F4;
    const uint8_t session_id = 0x01;
    const uint8_t stream_id = 0x04;
    const uint16_t sequence_number = 0x0200;
};

TEST_F(CrossSerializationTests, MessageCreate)
{
    // Client side creation
    uint8_t buffer[BUFFER_SIZE] = {};
    MessageCallback callback = {};
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
    MessageManager message_manager;
    init_message_manager(&message_manager, buffer, BUFFER_SIZE, buffer, BUFFER_SIZE, callback);
    //create_message(&message_manager);
    // [CREATE] SUBMESSAGE
    {
        char string[] = "Hello world";

        CreatePayloadSpec payload = {};
        payload.request_id = 0xAABBCCDD;
        payload.object_id = 0x778899;
        payload.object.kind = OBJECT_KIND_DATA_WRITER;

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

        add_create_submessage(&message_manager, &payload);
    }
    uint32_t seliarized_size = message_manager.writer.iterator - message_manager.writer.data;
    //*/

    /*Serializer serializer_(test_buffer, BUFFER_SIZE);
    MessageHeader message_header;
    serializer_.serialize(message_header);
    CREATE_PAYLOAD createData;
    SubmessageHeader submessage_header;
    submessage_header.submessage_id(CREATE);
    submessage_header.submessage_length(createData.getCdrSerializedSize(createData));
    serializer_.serialize(submessage_header);
    serializer_.serialize(createData);
    uint32_t seliarized_size = serializer_.get_serialized_size();//*/

    printf("################ %u serialized bytes. \n", seliarized_size);

    XRCEParser myParser{(char*)buffer, seliarized_size, &bool_listener_};
    ASSERT_TRUE(myParser.parse());
    ASSERT_TRUE(bool_listener_.create_called);

}
