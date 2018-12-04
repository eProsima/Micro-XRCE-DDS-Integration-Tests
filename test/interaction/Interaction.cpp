#include <gtest/gtest.h>

#include <ClientInteraction.hpp>
#if defined(PLATFORM_NAME_LINUX)
#include <uxr/agent/transport/udp/UDPServerLinux.hpp>
#include <uxr/agent/transport/tcp/TCPServerLinux.hpp>
#elif defined(PLATFORM_NAME_WINDOWS)
#include <uxr/agent/transport/udp/UDPServerWindows.hpp>
#include <uxr/agent/transport/tcp/TCPServerWindows.hpp>
#endif

#include <thread>

class InteractionTest : public ::testing::TestWithParam<int>
{
public:
    const uint16_t AGENT_PORT = 2018;
    const float LOST = 0.1f;

    InteractionTest()
        : transport_(GetParam())
    {
        create_agent(AGENT_PORT);
    }

    ~InteractionTest()
    {
        for(size_t i = 0; i < agents_.size(); ++i)
        {
            agents_[i]->stop();
        }
    }

    void create_agent(uint16_t port)
    {
        std::shared_ptr<eprosima::uxr::Server> agent;
        switch(transport_)
        {
            case UDP_TRANSPORT:
#if defined (PLATFORM_NAME_LINUX)
                agent.reset(new eprosima::uxr::UDPServer(port, UXR_DEFAULT_DISCOVERY_PORT + agents_.size()));
#else
                agent.reset(new eprosima::uxr::UDPServer(port));
#endif
                break;
            case TCP_TRANSPORT:
#if defined (PLATFORM_NAME_LINUX)
                agent.reset(new eprosima::uxr::TCPServer(port, UXR_DEFAULT_DISCOVERY_PORT + agents_.size()));
#else
                agent.reset(new eprosima::uxr::TCPServer(port));
#endif
                break;
        }
        agent->run();
        agents_.push_back(agent);
    }

protected:
    int transport_;

private:
    std::vector<std::shared_ptr<eprosima::uxr::Server>> agents_;
};

INSTANTIATE_TEST_CASE_P(Transport, InteractionTest, ::testing::Values(UDP_TRANSPORT, TCP_TRANSPORT), ::testing::PrintToStringParamName());

TEST_P(InteractionTest, InitCloseSession)
{
    Client client(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

TEST_P(InteractionTest, NewEntitiesCreationXMLBestEffort)
{
    Client client(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x01, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

TEST_P(InteractionTest, NewEntitiesCreationXMLReliable)
{
    Client client(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

TEST_P(InteractionTest, NewEntitiesCreationREFBestEffort)
{
    Client client(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_ref(1, 0x01, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

TEST_P(InteractionTest, NewEntitiesCreationREFReliable)
{
    Client client(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_ref(1, 0x80, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

TEST_P(InteractionTest, ExistantEntitiesCreationReuseXMLXMLReliable)
{
    Client client(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REUSE));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

//TEST_P(InteractionTest, ExistantEntitiesCreationReuseXMLREFReliable)
//{
//    Client client(0.0f, 8);
//    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
//    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));
//    ASSERT_NO_FATAL_FAILURE(client.create_entities_ref(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REUSE));
//    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
//}

TEST_P(InteractionTest, ExistantEntitiesCreationReuseREFREFReliable)
{
    Client client(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_ref(1, 0x80, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_ref(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REUSE));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

TEST_P(InteractionTest, NewEntitiesCreationReliableLost)
{
    Client client(LOST, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

TEST_P(InteractionTest, ExistantEntitiesCreationReplaceReliable)
{
    Client client(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_OK, UXR_REPLACE));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

TEST_P(InteractionTest, ExistantEntitiesCreationReplaceAndReuseReliable)
{
    Client client(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_OK_MATCHED, UXR_REPLACE | UXR_REUSE));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

TEST_P(InteractionTest, ExistantEntitiesCreationNoReplaceReliable)
{
    Client client(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(client.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));
    ASSERT_NO_FATAL_FAILURE(client.create_entities_xml(1, 0x80, UXR_STATUS_ERR_ALREADY_EXISTS, 0));
    ASSERT_NO_FATAL_FAILURE(client.close_transport(transport_));
}

TEST_P(InteractionTest, PubSub10TopicsBestEffort)
{
    Client publisher(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(publisher.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(publisher.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));

    Client subscriber(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(subscriber.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(subscriber.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));

    std::string message = "Hello DDS world!";
    std::thread publisher_thread(&Client::publish, &publisher, 1, 0x01, 10, message);
    std::thread subscriber_thread(&Client::subscribe, &subscriber, 1, 0x01, 10, message);

    publisher_thread.join();
    subscriber_thread.join();

    ASSERT_NO_FATAL_FAILURE(publisher.close_transport(transport_));
    ASSERT_NO_FATAL_FAILURE(subscriber.close_transport(transport_));
}

TEST_P(InteractionTest, PubSub10TopicsReliable)
{
    Client publisher(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(publisher.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(publisher.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));

    Client subscriber(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(subscriber.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(subscriber.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));

    std::string message = "Hello DDS world!";
    std::thread publisher_thread(&Client::publish, &publisher, 1, 0x80, 10, message);
    std::thread subscriber_thread(&Client::subscribe, &subscriber, 1, 0x80, 10, message);

    publisher_thread.join();
    subscriber_thread.join();

    ASSERT_NO_FATAL_FAILURE(publisher.close_transport(transport_));
    ASSERT_NO_FATAL_FAILURE(subscriber.close_transport(transport_));
}

TEST_P(InteractionTest, PubSub10TopicsReliableLost)
{
    Client publisher(LOST, 8);
    ASSERT_NO_FATAL_FAILURE(publisher.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(publisher.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));

    Client subscriber(LOST, 8);
    ASSERT_NO_FATAL_FAILURE(subscriber.init_transport(transport_,"127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(subscriber.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));

    std::string message = "Hello DDS world!";
    std::thread publisher_thread(&Client::publish, &publisher, 1, 0x80, 10, message);
    std::thread subscriber_thread(&Client::subscribe, &subscriber, 1, 0x80, 10, message);

    publisher_thread.join();
    subscriber_thread.join();

    ASSERT_NO_FATAL_FAILURE(publisher.close_transport(transport_));
    ASSERT_NO_FATAL_FAILURE(subscriber.close_transport(transport_));
}

TEST_P(InteractionTest, PubSubFragmentedTopic2Parts)
{
    Client publisher(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(publisher.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(publisher.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));

    Client subscriber(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(subscriber.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(subscriber.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));

    std::string message(size_t(publisher.get_mtu() * 1.5), 'A');
    std::thread publisher_thread(&Client::publish, &publisher, 1, 0x80, 1, message);
    std::thread subscriber_thread(&Client::subscribe, &subscriber, 1, 0x80, 1, message);

    publisher_thread.join();
    subscriber_thread.join();

    ASSERT_NO_FATAL_FAILURE(publisher.close_transport(transport_));
    ASSERT_NO_FATAL_FAILURE(subscriber.close_transport(transport_));
}

TEST_P(InteractionTest, PubSubFragmentedTopic5Parts)
{
    Client publisher(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(publisher.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(publisher.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));

    Client subscriber(0.0f, 8);
    ASSERT_NO_FATAL_FAILURE(subscriber.init_transport(transport_, "127.0.0.1", AGENT_PORT));
    ASSERT_NO_FATAL_FAILURE(subscriber.create_entities_xml(1, 0x80, UXR_STATUS_OK, 0));

    std::string message(size_t(publisher.get_mtu() * 4.5), 'A');
    std::thread publisher_thread(&Client::publish, &publisher, 1, 0x80, 1, message);
    std::thread subscriber_thread(&Client::subscribe, &subscriber, 1, 0x80, 1, message);

    publisher_thread.join();
    subscriber_thread.join();

    ASSERT_NO_FATAL_FAILURE(publisher.close_transport(transport_));
    ASSERT_NO_FATAL_FAILURE(subscriber.close_transport(transport_));
}

#if defined(PLATFORM_NAME_LINUX)
TEST_P(InteractionTest, DiscoveryUnicast)
{
    create_agent(AGENT_PORT + 1);
    create_agent(AGENT_PORT + 2);
    create_agent(AGENT_PORT + 3);

    std::vector<uint16_t> agent_ports;
    agent_ports.push_back(AGENT_PORT);
    agent_ports.push_back(AGENT_PORT + 1);
    agent_ports.push_back(AGENT_PORT + 2);
    agent_ports.push_back(AGENT_PORT + 3);

    Discovery discovery(transport_, agent_ports);

    std::thread discovery_thread(&Discovery::unicast, &discovery, UXR_DEFAULT_DISCOVERY_PORT);

    discovery_thread.join();
}

TEST_P(InteractionTest, DiscoveryMulticast)
{
    std::vector<uint16_t> agent_ports;
    agent_ports.push_back(AGENT_PORT);

    Discovery discovery(transport_, agent_ports);

    std::thread discovery_thread(&Discovery::multicast, &discovery);

    discovery_thread.join();
}
#endif

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
