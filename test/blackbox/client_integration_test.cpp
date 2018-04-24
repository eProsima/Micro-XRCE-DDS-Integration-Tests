// Copyright 2017 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <micrortps/client/client.h>
#include <micrortps/client/xrce_client.h>
#include <micrortps/client/xrce_protocol_spec.h>

#include <microcdr/microcdr.h>

#include <gtest/gtest.h>
#include <cstdint>

#ifdef WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include <thread>

#include <fstream>

#define MAX_NUM_ATTEMPTS  100
#define MAX_TIME_WAIT	  100

typedef struct HelloWorld
{
    uint32_t index;
    char* message;
} HelloTopic;

typedef struct ShapeTopic
{
    char*    color;
    uint32_t x;
    uint32_t y;
    uint32_t shapesize;
} ShapeTopic;

void on_topic(ObjectId id, MicroBuffer *serialized_topic, void* args);

void printl_shape_topic(const ShapeTopic* shape_topic);
void printl_hello_topic(const HelloTopic* hellow_topic);

class ClientTests : public ::testing::Test
{
    public:
        ClientTests()
        {
            uint8_t ip[4] = {127, 0, 0, 1};
            new_udp_session(&session, 0x01, key, ip, 2019, on_topic, NULL);

            statusObjectId = 0x0000;
            statusRequestId = 0x0000;
            statusOperation = 0xFF;
            statusImplementation = 0xFF;

            lastObject = 0x0000;
            lastRequest = 0x0000;

            topicCount = 0;
        }

        ~ClientTests()
        {
        }

        void checkStatus()
        {
            ASSERT_EQ(statusObjectId, lastObject);
            ASSERT_EQ(statusRequestId, lastRequest);
            ASSERT_EQ(statusOperation, STATUS_OK);
        }

        void checkDataTopic(int expectedNumTopic)
        {
            ASSERT_EQ(expectedNumTopic, topicCount);
        }

        bool createClient()
        {
            return init_session_sync(&session);
        }

        bool closeSession()
        {
            return close_session_sync(&session);
        }

        bool createParticipant()
        {
            return create_participant_sync_by_ref(&session, participant_id_, "default_participant", false, false);
        }

        bool createTopic()
        {
            return create_topic_sync_by_xml(&session, topic_id_, topic_xml_, participant_id_, false, false);
        }

        bool createPublisher()
        {
            return create_publisher_sync_by_xml(&session, publisher_id_, publisher_xml_, participant_id_, false, false);
        }

        bool createSubscriber()
        {
            return create_subscriber_sync_by_xml(&session, subscriber_id_, subscriber_xml_, participant_id_, false, false);
        }

        bool createDataWriter()
        {
            return create_datawriter_sync_by_xml(&session, datawriter_id_, datawriter_xml_, publisher_id_, false, false);
        }

        bool createDataReader()
        {
            return create_datareader_sync_by_xml(&session, datareader_id_, datareader_xml_, subscriber_id_, false, false);
        }

        bool delete_object(ObjectId id)
        {
            return delete_object_sync(&session, id);
        }

        /* Session config. */
        Session session;
        ClientKey key = {{0xAA, 0xBB, 0xCC, 0xDD}};

        /* Object IDs. */
        ObjectId participant_id_ = {{0x00, 0x01}};
        ObjectId topic_id_       = {{0x00, 0x02}};
        ObjectId publisher_id_   = {{0x00, 0x03}};
        ObjectId datawriter_id_  = {{0x00, 0x05}};
        ObjectId subscriber_id_  = {{0x00, 0x04}};
        ObjectId datareader_id_  = {{0x00, 0x06}};

        /* XMLs. */
        const char* topic_xml_ = {"<dds><topic><name>HelloWorldTopic</name><dataType>HelloWorld</dataType></topic></dds>"};
        const char* publisher_xml_ = {"<publisher name=\"MyPublisher\""};
        const char* datawriter_xml_ = {"<profiles><publisher profile_name=\"default_xrce_publisher_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></publisher></profiles>"};
        const char* subscriber_xml_ = {"<publisher name=\"MySubscriber\""};
        const char* datareader_xml_ = {"<profiles><subscriber profile_name=\"default_xrce_subscriber_profile\"><topic><kind>NO_KEY</kind><name>HelloWorldTopic</name><dataType>HelloWorld</dataType><historyQos><kind>KEEP_LAST</kind><depth>5</depth></historyQos><durability><kind>TRANSIENT_LOCAL</kind></durability></topic></subscriber></profiles>"};

        uint16_t statusObjectId;
        uint16_t statusRequestId;
        uint8_t statusOperation;
        uint8_t statusImplementation;

        uint16_t lastRequest;
        uint16_t lastObject;

        int topicCount;
};

void on_topic(ObjectId /*id*/, MicroBuffer* /*serialized_topic*/, void* /*args*/)
{
}

void printl_shape_topic(const ShapeTopic* shape_topic)
{
    printf("        %s[%s | x: %u | y: %u | shapesize: %u]%s\n",
            "\x1B[1;34m",
            shape_topic->color,
            shape_topic->x,
            shape_topic->y,
            shape_topic->shapesize,
            "\x1B[0m");
}

void printl_hello_topic(const HelloTopic* hello_topic)
{
    printf("        %s[%s | index: %u]%s\n",
            "\x1B[1;34m",
            hello_topic->message,
            hello_topic->index,
            "\x1B[0m");
}

TEST_F(ClientTests, CreateDeleteClient)
{
    ASSERT_EQ(createClient(), true);
    ms_sleep(1000);
    ASSERT_EQ(closeSession(), true);
}

TEST_F(ClientTests, CreateDeleteParticipant)
{
    ASSERT_EQ(createClient(), true);
    ASSERT_EQ(createParticipant(), true);
    ms_sleep(1000);
    ASSERT_EQ(delete_object(participant_id_), true);
    ms_sleep(1000);
    ASSERT_EQ(closeSession(), true);
}

TEST_F(ClientTests, CreateDeleteTopic)
{
    ASSERT_EQ(createClient(), true);
    ASSERT_EQ(createParticipant(), true);
    ASSERT_EQ(createTopic(), true);
    ms_sleep(1000);
    ASSERT_EQ(delete_object(topic_id_), true);
    ASSERT_EQ(delete_object(participant_id_), true);
    ms_sleep(1000);
    ASSERT_EQ(closeSession(), true);
}

TEST_F(ClientTests, CreateDeletePublisher)
{
    ASSERT_EQ(createClient(), true);
    ASSERT_EQ(createParticipant(), true);
    ASSERT_EQ(createTopic(), true);
    ASSERT_EQ(createPublisher(), true);
    ms_sleep(1000);
    ASSERT_EQ(delete_object(publisher_id_), true);
    ASSERT_EQ(delete_object(topic_id_), true);
    ASSERT_EQ(delete_object(participant_id_), true);
    ms_sleep(1000);
    ASSERT_EQ(closeSession(), true);
}

TEST_F(ClientTests, CreateDeleteSubscriber)
{
    ASSERT_EQ(createClient(), true);
    ASSERT_EQ(createParticipant(), true);
    ASSERT_EQ(createTopic(), true);
    ASSERT_EQ(createSubscriber(), true);
    ms_sleep(1000);
    ASSERT_EQ(delete_object(subscriber_id_), true);
    ASSERT_EQ(delete_object(topic_id_), true);
    ASSERT_EQ(delete_object(participant_id_), true);
    ms_sleep(1000);
    ASSERT_EQ(closeSession(), true);
}

TEST_F(ClientTests, CreateDeleteDataWriter)
{
    ASSERT_EQ(createClient(), true);
    ASSERT_EQ(createParticipant(), true);
    ASSERT_EQ(createTopic(), true);
    ASSERT_EQ(createPublisher(), true);
    ASSERT_EQ(createDataWriter(), true);
    ms_sleep(1000);
    ASSERT_EQ(delete_object(datawriter_id_), true);
    ASSERT_EQ(delete_object(publisher_id_), true);
    ASSERT_EQ(delete_object(topic_id_), true);
    ASSERT_EQ(delete_object(participant_id_), true);
    ms_sleep(1000);
    ASSERT_EQ(closeSession(), true);
}

TEST_F(ClientTests, CreateDeleteDataReader)
{
    ASSERT_EQ(createClient(), true);
    ASSERT_EQ(createParticipant(), true);
    ASSERT_EQ(createTopic(), true);
    ASSERT_EQ(createSubscriber(), true);
    ASSERT_EQ(createDataReader(), true);
    ms_sleep(1000);
    ASSERT_EQ(delete_object(datareader_id_), true);
    ASSERT_EQ(delete_object(subscriber_id_), true);
    ASSERT_EQ(delete_object(topic_id_), true);
    ASSERT_EQ(delete_object(participant_id_), true);
    ms_sleep(1000);
    ASSERT_EQ(closeSession(), true);
}
