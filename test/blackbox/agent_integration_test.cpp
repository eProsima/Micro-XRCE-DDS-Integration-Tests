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

#include <agent/Root.h>
#include <agent/XRCEObject.h>
#include <agent/datareader/DataReader.h>
#include <agent/datawriter/DataWriter.h>
#include <agent/participant/Participant.h>
#include <agent/publisher/Publisher.h>
#include <agent/subscriber/Subscriber.h>
#include <agent/topic/Topic.hpp>

#include <gtest/gtest.h>

#include <chrono>
#include <thread>

#define MAX_NUM_ATTEMPTS 100
#define MAX_TIME_WAIT    100

namespace eprosima {
namespace micrortps {
namespace testing {

class AgentTests : public ::testing::Test
{
  protected:
    AgentTests()
    {
        agent_.init(2019);
    }

    virtual ~AgentTests() = default;

    ProxyClient* wait_client(dds::xrce::ClientKey client_k)
    {
        ProxyClient* client = nullptr;
        int count           = 0;
        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIME_WAIT));
            client = agent_.get_client(client_k);
        } while (client == nullptr && ++count < MAX_NUM_ATTEMPTS);
        EXPECT_LT(count, MAX_NUM_ATTEMPTS);
        return client;
    }

    ProxyClient* wait_delete_client(dds::xrce::ClientKey client_k)
    {
        ProxyClient* client = nullptr;
        int count           = 0;
        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIME_WAIT));
            client = agent_.get_client(client_k);
        } while (client != nullptr && ++count < MAX_NUM_ATTEMPTS);
        EXPECT_LT(count, MAX_NUM_ATTEMPTS);
        return client;
    }

    XRCEObject* wait_object(ProxyClient* client, const dds::xrce::ObjectId& object_id)
    {

        XRCEObject* object = nullptr;
        if (client == nullptr)
        {
            return object;
        }
        int count = 0;
        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIME_WAIT));
            object = client->get_object(object_id);
        } while (object == nullptr && ++count < MAX_NUM_ATTEMPTS);
        EXPECT_LT(count, MAX_NUM_ATTEMPTS);
        return object;
    }

    XRCEObject* wait_delete_object(ProxyClient* client, const dds::xrce::ObjectId& object_id)
    {
        XRCEObject* object = nullptr;
        if (client == nullptr)
        {
            return object;
        }
        int count = 0;
        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIME_WAIT));
            object = client->get_object(object_id);
        } while (object != nullptr && ++count < MAX_NUM_ATTEMPTS);
        EXPECT_LT(count, MAX_NUM_ATTEMPTS);
        return object;
    }

    void wait_action()
    {
        int count = 0;
        do
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(MAX_TIME_WAIT));
        } while (++count < MAX_NUM_ATTEMPTS);
    }

    eprosima::micrortps::Agent& agent_ = root();
    const dds::xrce::ClientKey client_key         = {{0xAA, 0xBB, 0xCC, 0xDD}};
    std::thread agent_thread;
};

TEST_F(AgentTests, CreateDeleteClient)
{
    ASSERT_EQ(agent_.get_client(client_key), nullptr);
    agent_thread               = std::thread(&Agent::run, &agent_);
    ProxyClient* client        = wait_client(client_key);
    ProxyClient* delete_client = wait_delete_client(client_key);

    agent_.stop();
    agent_thread.join();
    agent_.abort_execution();
    ASSERT_NE(client, nullptr);
    ASSERT_EQ(delete_client, nullptr);
}

TEST_F(AgentTests, CreateDeleteParticipant)
{
    const dds::xrce::ObjectId participant_id = {{0x00, 0x01}};
    ASSERT_EQ(agent_.get_client(client_key), nullptr);
    agent_thread        = std::thread(&Agent::run, &agent_);
    ProxyClient* client = wait_client(client_key);
    XRCEParticipant* participant =
        dynamic_cast<XRCEParticipant*>(wait_object(client, participant_id));
    XRCEParticipant* delete_participant =
        dynamic_cast<XRCEParticipant*>(wait_delete_object(client, participant_id));
    ProxyClient* delete_client = wait_delete_client(client_key);

    agent_.stop();
    agent_thread.join();
    agent_.abort_execution();
    ASSERT_NE(client, nullptr);
    ASSERT_NE(participant, nullptr);
    ASSERT_EQ(delete_participant, nullptr);
    ASSERT_EQ(delete_client, nullptr);
}

TEST_F(AgentTests, CreateDeleteTopic)
{
    const dds::xrce::ObjectId participant_id = {{0x00, 0x01}};
    const dds::xrce::ObjectId topic_id       = {{0x00, 0x02}};

    ASSERT_EQ(agent_.get_client(client_key), nullptr);
    agent_thread        = std::thread(&Agent::run, &agent_);
    ProxyClient* client = wait_client(client_key);
    XRCEParticipant* participant =
        dynamic_cast<XRCEParticipant*>(wait_object(client, participant_id));
    Topic* topic = dynamic_cast<Topic*>(wait_object(client, topic_id));

    Topic* delete_topic = dynamic_cast<Topic*>(wait_delete_object(client, topic_id));
    XRCEParticipant* delete_participant =
        dynamic_cast<XRCEParticipant*>(wait_delete_object(client, participant_id));
    ProxyClient* delete_client = wait_delete_client(client_key);

    agent_.stop();
    agent_thread.join();
    agent_.abort_execution();
    ASSERT_NE(client, nullptr);
    ASSERT_NE(participant, nullptr);
    ASSERT_NE(topic, nullptr);
    ASSERT_EQ(delete_topic, nullptr);
    ASSERT_EQ(delete_participant, nullptr);
    ASSERT_EQ(delete_client, nullptr);
}

TEST_F(AgentTests, CreateDeletePublisher)
{
    const dds::xrce::ObjectId participant_id = {{0x00, 0x01}};
    const dds::xrce::ObjectId publisher_id   = {{0x00, 0x03}};
    ASSERT_EQ(agent_.get_client(client_key), nullptr);
    agent_thread        = std::thread(&Agent::run, &agent_);
    ProxyClient* client = wait_client(client_key);
    XRCEParticipant* participant =
        dynamic_cast<XRCEParticipant*>(wait_object(client, participant_id));
    Publisher* publisher = dynamic_cast<Publisher*>(wait_object(client, publisher_id));
    Publisher* delete_publisher =
        dynamic_cast<Publisher*>(wait_delete_object(client, publisher_id));
    XRCEParticipant* delete_participant =
        dynamic_cast<XRCEParticipant*>(wait_delete_object(client, participant_id));
    ProxyClient* delete_client = wait_delete_client(client_key);

    agent_.stop();
    agent_thread.join();
    agent_.abort_execution();
    ASSERT_NE(client, nullptr);
    ASSERT_NE(participant, nullptr);
    ASSERT_NE(publisher, nullptr);
    ASSERT_EQ(delete_publisher, nullptr);
    ASSERT_EQ(delete_participant, nullptr);
    ASSERT_EQ(delete_client, nullptr);
}

TEST_F(AgentTests, CreateDeleteSubscriber)
{
    const dds::xrce::ObjectId participant_id = {{0x00, 0x01}};
    const dds::xrce::ObjectId subscriber_id  = {{0x00, 0x04}};
    ASSERT_EQ(agent_.get_client(client_key), nullptr);
    agent_thread        = std::thread(&Agent::run, &agent_);
    ProxyClient* client = wait_client(client_key);
    XRCEParticipant* participant =
        dynamic_cast<XRCEParticipant*>(wait_object(client, participant_id));
    Subscriber* subscriber = dynamic_cast<Subscriber*>(wait_object(client, subscriber_id));

    Subscriber* delete_subscriber =
        dynamic_cast<Subscriber*>(wait_delete_object(client, subscriber_id));
    XRCEParticipant* delete_participant =
        dynamic_cast<XRCEParticipant*>(wait_delete_object(client, participant_id));
    ProxyClient* delete_client = wait_delete_client(client_key);

    agent_.stop();
    agent_thread.join();
    agent_.abort_execution();
    ASSERT_NE(client, nullptr);
    ASSERT_NE(participant, nullptr);
    ASSERT_NE(subscriber, nullptr);
    ASSERT_EQ(delete_subscriber, nullptr);
    ASSERT_EQ(delete_participant, nullptr);
    ASSERT_EQ(delete_client, nullptr);
}

TEST_F(AgentTests, CreateDeleteDataWriter)
{
    const dds::xrce::ObjectId participant_id = {{0x00, 0x01}};
    const dds::xrce::ObjectId topic_id       = {{0x00, 0x02}};
    const dds::xrce::ObjectId publisher_id   = {{0x00, 0x03}};
    const dds::xrce::ObjectId datawriter_id  = {{0x00, 0x05}};
    ASSERT_EQ(agent_.get_client(client_key), nullptr);
    agent_thread        = std::thread(&Agent::run, &agent_);
    ProxyClient* client = wait_client(client_key);
    XRCEParticipant* participant =
        dynamic_cast<XRCEParticipant*>(wait_object(client, participant_id));
    Topic* topic = dynamic_cast<Topic*>(wait_object(client, topic_id));
    Publisher* publisher    = dynamic_cast<Publisher*>(wait_object(client, publisher_id));
    DataWriter* data_writer = dynamic_cast<DataWriter*>(wait_object(client, datawriter_id));

    DataWriter* delete_data_writer =
        dynamic_cast<DataWriter*>(wait_delete_object(client, datawriter_id));
    Publisher* delete_publisher =
        dynamic_cast<Publisher*>(wait_delete_object(client, publisher_id));
    Topic* delete_topic = dynamic_cast<Topic*>(wait_delete_object(client, topic_id));
    XRCEParticipant* delete_participant =
        dynamic_cast<XRCEParticipant*>(wait_delete_object(client, participant_id));
    ProxyClient* delete_client = wait_delete_client(client_key);

    agent_.stop();
    agent_thread.join();
    agent_.abort_execution();
    ASSERT_NE(client, nullptr);
    ASSERT_NE(participant, nullptr);
    ASSERT_NE(topic, nullptr);
    ASSERT_NE(publisher, nullptr);
    ASSERT_NE(data_writer, nullptr);
    ASSERT_EQ(delete_data_writer, nullptr);
    ASSERT_EQ(delete_publisher, nullptr);
    ASSERT_EQ(delete_topic, nullptr);
    ASSERT_EQ(delete_participant, nullptr);
    ASSERT_EQ(delete_client, nullptr);
}

TEST_F(AgentTests, CreateDeleteDataReader)
{
    const dds::xrce::ObjectId participant_id = {{0x00, 0x01}};
    const dds::xrce::ObjectId topic_id       = {{0x00, 0x02}};
    const dds::xrce::ObjectId subscriber_id  = {{0x00, 0x04}};
    const dds::xrce::ObjectId datareader_id  = {{0x00, 0x06}};
    ASSERT_EQ(agent_.get_client(client_key), nullptr);
    agent_thread        = std::thread(&Agent::run, &agent_);
    ProxyClient* client = wait_client(client_key);
    XRCEParticipant* participant =
        dynamic_cast<XRCEParticipant*>(wait_object(client, participant_id));
    Topic* topic = dynamic_cast<Topic*>(wait_object(client, topic_id));
    Subscriber* subscriber  = dynamic_cast<Subscriber*>(wait_object(client, subscriber_id));
    DataReader* data_reader = dynamic_cast<DataReader*>(wait_object(client, datareader_id));

    DataReader* delete_data_reader =
        dynamic_cast<DataReader*>(wait_delete_object(client, datareader_id));
    Subscriber* delete_subscriber =
        dynamic_cast<Subscriber*>(wait_delete_object(client, subscriber_id));
    Topic* delete_topic = dynamic_cast<Topic*>(wait_delete_object(client, topic_id));
    XRCEParticipant* delete_participant =
        dynamic_cast<XRCEParticipant*>(wait_delete_object(client, participant_id));
    ProxyClient* delete_client = wait_delete_client(client_key);

    agent_.stop();
    agent_thread.join();
    agent_.abort_execution();
    ASSERT_NE(client, nullptr);
    ASSERT_NE(participant, nullptr);
    ASSERT_NE(topic, nullptr);
    ASSERT_NE(subscriber, nullptr);
    ASSERT_NE(data_reader, nullptr);
    ASSERT_EQ(delete_data_reader, nullptr);
    ASSERT_EQ(delete_subscriber, nullptr);
    ASSERT_EQ(delete_topic, nullptr);
    ASSERT_EQ(delete_participant, nullptr);
    ASSERT_EQ(delete_client, nullptr);
}

TEST_F(AgentTests, WriteData)
{
    const dds::xrce::ObjectId participant_id = {{0x00, 0x01}};
    const dds::xrce::ObjectId topic_id       = {{0x00, 0x02}};
    const dds::xrce::ObjectId publisher_id   = {{0x00, 0x03}};
    const dds::xrce::ObjectId datawriter_id  = {{0x00, 0x04}};
    ASSERT_EQ(agent_.get_client(client_key), nullptr);
    agent_thread        = std::thread(&Agent::run, &agent_);
    ProxyClient* client = wait_client(client_key);
    XRCEParticipant* participant =
        dynamic_cast<XRCEParticipant*>(wait_object(client, participant_id));
    Topic* topic = dynamic_cast<Topic*>(wait_object(client, topic_id));
    Publisher* publisher    = dynamic_cast<Publisher*>(wait_object(client, publisher_id));
    DataWriter* data_writer = dynamic_cast<DataWriter*>(wait_object(client, datawriter_id));

    DataWriter* delete_data_writer =
        dynamic_cast<DataWriter*>(wait_delete_object(client, datawriter_id));
    Publisher* delete_publisher =
        dynamic_cast<Publisher*>(wait_delete_object(client, publisher_id));
    Topic* delete_topic = dynamic_cast<Topic*>(wait_delete_object(client, topic_id));
    XRCEParticipant* delete_participant =
        dynamic_cast<XRCEParticipant*>(wait_delete_object(client, participant_id));
    ProxyClient* delete_client = wait_delete_client(client_key);

    agent_.stop();
    agent_thread.join();
    agent_.abort_execution();
    ASSERT_NE(client, nullptr);
    ASSERT_NE(participant, nullptr);
    ASSERT_NE(topic, nullptr);
    ASSERT_NE(publisher, nullptr);
    ASSERT_NE(data_writer, nullptr);
    ASSERT_EQ(delete_data_writer, nullptr);
    ASSERT_EQ(delete_publisher, nullptr);
    ASSERT_EQ(delete_topic, nullptr);
    ASSERT_EQ(delete_participant, nullptr);
    ASSERT_EQ(delete_client, nullptr);
}

TEST_F(AgentTests, ReadData)
{
    const dds::xrce::ObjectId participant_id = {{0x00, 0x01}};
    const dds::xrce::ObjectId topic_id       = {{0x00, 0x02}};
    const dds::xrce::ObjectId subscriber_id  = {{0x00, 0x03}};
    const dds::xrce::ObjectId datareader_id  = {{0x00, 0x04}};
    ASSERT_EQ(agent_.get_client(client_key), nullptr);
    agent_thread        = std::thread(&Agent::run, &agent_);
    ProxyClient* client = wait_client(client_key);
    XRCEParticipant* participant =
        dynamic_cast<XRCEParticipant*>(wait_object(client, participant_id));
    Topic* topic = dynamic_cast<Topic*>(wait_object(client, topic_id));
    Subscriber* subscriber  = dynamic_cast<Subscriber*>(wait_object(client, subscriber_id));
    DataReader* data_reader = dynamic_cast<DataReader*>(wait_object(client, datareader_id));
    wait_action();
    DataReader* delete_data_reader =
        dynamic_cast<DataReader*>(wait_delete_object(client, datareader_id));
    Subscriber* delete_reader =
        dynamic_cast<Subscriber*>(wait_delete_object(client, subscriber_id));
    Topic* delete_topic = dynamic_cast<Topic*>(wait_delete_object(client, topic_id));
    XRCEParticipant* delete_participant =
        dynamic_cast<XRCEParticipant*>(wait_delete_object(client, participant_id));
    ProxyClient* delete_client = wait_delete_client(client_key);

    agent_.stop();
    agent_.abort_execution();
    agent_thread.join();
    ASSERT_NE(client, nullptr);
    ASSERT_NE(participant, nullptr);
    ASSERT_NE(topic, nullptr);
    ASSERT_NE(subscriber, nullptr);
    ASSERT_NE(data_reader, nullptr);
    ASSERT_EQ(delete_data_reader, nullptr);
    ASSERT_EQ(delete_reader, nullptr);
    ASSERT_EQ(delete_topic, nullptr);
    ASSERT_EQ(delete_participant, nullptr);
    ASSERT_EQ(delete_client, nullptr);
}

TEST_F(AgentTests, ReadMultiData)
{
    const dds::xrce::ObjectId participant_id = {{0x00, 0x01}};
    const dds::xrce::ObjectId subscriber_id  = {{0x00, 0x02}};
    const dds::xrce::ObjectId datareader_id  = {{0x00, 0x03}};
    ASSERT_EQ(agent_.get_client(client_key), nullptr);
    agent_thread        = std::thread(&Agent::run, &agent_);
    ProxyClient* client = wait_client(client_key);
    XRCEParticipant* participant =
        dynamic_cast<XRCEParticipant*>(wait_object(client, participant_id));
    Subscriber* subscriber  = dynamic_cast<Subscriber*>(wait_object(client, subscriber_id));
    DataReader* data_reader = dynamic_cast<DataReader*>(wait_object(client, datareader_id));
    wait_action();
    DataReader* delete_data_reader =
        dynamic_cast<DataReader*>(wait_delete_object(client, datareader_id));
    Subscriber* delete_reader =
        dynamic_cast<Subscriber*>(wait_delete_object(client, subscriber_id));
    XRCEParticipant* delete_participant =
        dynamic_cast<XRCEParticipant*>(wait_delete_object(client, participant_id));
    ProxyClient* delete_client = wait_delete_client(client_key);

    agent_.stop();
    agent_.abort_execution();
    agent_thread.join();
    ASSERT_NE(client, nullptr);
    ASSERT_NE(participant, nullptr);
    ASSERT_NE(subscriber, nullptr);
    ASSERT_NE(data_reader, nullptr);
    ASSERT_EQ(delete_data_reader, nullptr);
    ASSERT_EQ(delete_reader, nullptr);
    ASSERT_EQ(delete_participant, nullptr);
    ASSERT_EQ(delete_client, nullptr);
}

} // namespace testing
} // namespace micrortps
} // namespace eprosima

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
