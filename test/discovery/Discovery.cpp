#include <gtest/gtest.h>

#include <ClientInteraction.hpp>
#if defined(PLATFORM_NAME_LINUX)
#include <uxr/agent/transport/udp/UDPServerLinux.hpp>
#include <uxr/agent/transport/tcp/TCPServerLinux.hpp>
#endif

#include <thread>

class DiscoveryIntegration : public ::testing::TestWithParam<int>
{
public:
    const uint16_t AGENT_PORT = 2018;
    const uint16_t DISCOVERY_PORT = UXR_DEFAULT_DISCOVERY_PORT;
    const size_t AGENT_NUMBER = 4;

    DiscoveryIntegration()
        : transport_(GetParam())
    {
        for(size_t i = 0; i < AGENT_NUMBER; i++)
        {
            create_agent(AGENT_PORT + i, DISCOVERY_PORT + i);
        }
    }

    ~DiscoveryIntegration()
    {
        for(size_t i = 0; i < agents_.size(); ++i)
        {
            agents_[i]->stop();
        }
    }

    void create_agent(uint16_t port, uint16_t discovery_port)
    {
        std::shared_ptr<eprosima::uxr::Server> agent;
        switch(transport_)
        {
            case UDP_TRANSPORT:
                agent.reset(new eprosima::uxr::UDPServer(port, discovery_port));
                break;
            case TCP_TRANSPORT:
                agent.reset(new eprosima::uxr::TCPServer(port, discovery_port));
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

TEST_P(InteractionTest, DiscoveryUnicast)
{
    std::vector<uint16_t> agent_ports;
    agent_ports.push_back(AGENT_PORT);
    agent_ports.push_back(AGENT_PORT + 1);
    agent_ports.push_back(AGENT_PORT + 2);
    agent_ports.push_back(AGENT_PORT + 3);

    DiscoveryIntegration discovery(transport_, agent_ports);

    std::thread discovery_thread(&DiscoveryIntegration::unicast, &discovery, UXR_DEFAULT_DISCOVERY_PORT);

    discovery_thread.join();
}

TEST_P(InteractionTest, DiscoveryMulticast)
{
    std::vector<uint16_t> agent_ports;
    agent_ports.push_back(AGENT_PORT);

    DiscoveryIntegration discovery(transport_, agent_ports);

    std::thread discovery_thread(&DiscoveryIntegration::multicast, &discovery);

    discovery_thread.join();
}

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}
