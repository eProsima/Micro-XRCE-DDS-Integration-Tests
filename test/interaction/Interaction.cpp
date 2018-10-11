#include <gtest/gtest.h>
#include <micrortps/agent/transport/UDPServerLinux.hpp>
#include <ClientInteraction.hpp>
#include <thread>

class InteractionTest : public ::testing::Test
{
public:
    const uint16_t AGENT_PORT = 2018;
    const float LOST = 0.1f;

    InteractionTest()
        : udpAgent_(AGENT_PORT)
    {
        udpAgent_.run();
    }

    ~InteractionTest()
    {
        udpAgent_.stop();
    }

private:
    eprosima::micrortps::UDPServer udpAgent_;
};

TEST_F(InteractionTest, NewEntitiesCreationBestEffort)
{
    Client client(AGENT_PORT, 0.0f, 8);
    client.create_entities(1, 0x01, MR_STATUS_OK, 0);
}

TEST_F(InteractionTest, NewEntitiesCreationReliable)
{
    Client client(AGENT_PORT, 0.0f, 8);
    client.create_entities(1, 0x80, MR_STATUS_OK, 0);
}

TEST_F(InteractionTest, NewEntitiesCreationReliableLost)
{
    Client client(AGENT_PORT, LOST, 8);
    client.create_entities(1, 0x80, MR_STATUS_OK, 0);
}

TEST_F(InteractionTest, ExistantEntitiesCreationReplaceReliable)
{
    Client client(AGENT_PORT, 0.0f, 8);
    client.create_entities(1, 0x80, MR_STATUS_OK, 0);
    client.create_entities(1, 0x80, MR_STATUS_OK, MR_REPLACE);
}

TEST_F(InteractionTest, ExistantEntitiesCreationReuseReliable)
{
    Client client(AGENT_PORT, 0.0f, 8);
    client.create_entities(1, 0x80, MR_STATUS_OK, 0);
    client.create_entities(1, 0x80, MR_STATUS_OK, MR_REUSE);
}

TEST_F(InteractionTest, ExistantEntitiesCreationNoReplaceReliable)
{
    Client client(AGENT_PORT, 0.0f, 8);
    client.create_entities(1, 0x80, MR_STATUS_OK, 0);
    client.create_entities(1, 0x80, MR_STATUS_ERR_ALREADY_EXISTS, 0);
}

TEST_F(InteractionTest, PubSub10TopicsBestEffort)
{
    Client publisher(AGENT_PORT, 0.0f, 8);
    publisher.create_entities(1, 0x80, MR_STATUS_OK, 0);

    Client subscriber(AGENT_PORT, 0.0f, 8);
    subscriber.create_entities(1, 0x80, MR_STATUS_OK, 0);

    std::thread publisher_thread(&Client::publish, &publisher, 1, 0x01, 10);
    std::thread subscriber_thread(&Client::subscribe, &subscriber, 1, 0x01, 10);

    publisher_thread.join();
    subscriber_thread.join();
}

TEST_F(InteractionTest, PubSub10TopicsReliable)
{
    Client publisher(AGENT_PORT, 0.0f, 8);
    publisher.create_entities(1, 0x80, MR_STATUS_OK, 0);

    Client subscriber(AGENT_PORT, 0.0f, 8);
    subscriber.create_entities(1, 0x80, MR_STATUS_OK, 0);

    std::thread publisher_thread(&Client::publish, &publisher, 1, 0x80, 10);
    std::thread subscriber_thread(&Client::subscribe, &subscriber, 1, 0x80, 10);

    publisher_thread.join();
    subscriber_thread.join();
}

TEST_F(InteractionTest, PubSub10TopicsReliableLost)
{
    Client publisher(AGENT_PORT, LOST, 8);
    publisher.create_entities(1, 0x80, MR_STATUS_OK, 0);

    Client subscriber(AGENT_PORT, LOST, 8);
    subscriber.create_entities(1, 0x80, MR_STATUS_OK, 0);

    std::thread publisher_thread(&Client::publish, &publisher, 1, 0x80, 10);
    std::thread subscriber_thread(&Client::subscribe, &subscriber, 1, 0x80, 10);

    publisher_thread.join();
    subscriber_thread.join();
}

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
