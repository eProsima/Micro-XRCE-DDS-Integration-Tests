#include <gtest/gtest.h>

#include <Discovery.hpp>
#include <uxr/agent/transport/udp/UDPServerLinux.hpp>
#include <uxr/agent/transport/tcp/TCPServerLinux.hpp>

#include <thread>

class DiscoveryIntegration : public ::testing::TestWithParam<int>
{
public:
    const uint16_t AGENT_PORT = 2018;
    const uint16_t DISCOVERY_PORT = UXR_DEFAULT_DISCOVERY_PORT;

    DiscoveryIntegration()
    : transport_(GetParam())
    {
    }

    ~DiscoveryIntegration()
    {
    }

    void TearDown() override
    {
        for(size_t i = 0; i < agents_.size(); ++i)
        {
            agents_[i]->stop();
        }
    }

    std::vector<uint16_t> init_scenario(size_t number)
    {
        std::vector<uint16_t> agent_ports;
        std::vector<uint16_t> discovery_ports;
        for(size_t i = 0; i < number; i++)
        {
            uint16_t agent_port = AGENT_PORT + i;
            uint16_t discovery_port = DISCOVERY_PORT + i;
            create_agent(agent_port, discovery_port);
            agent_ports.push_back(agent_port);
            discovery_ports.push_back(discovery_port);
        }

        discovery_.reset(new Discovery(transport_, agent_ports));

        return discovery_ports;
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
    std::unique_ptr<Discovery> discovery_;

private:
    std::vector<std::shared_ptr<eprosima::uxr::Server>> agents_;
};

INSTANTIATE_TEST_CASE_P(Transport, DiscoveryIntegration, ::testing::Values(UDP_TRANSPORT, TCP_TRANSPORT), ::testing::PrintToStringParamName());

TEST_P(DiscoveryIntegration, DiscoveryUnicast)
{
    std::vector<uint16_t> discovery_ports = init_scenario(4);
    discovery_->unicast(discovery_ports);
}

TEST_P(DiscoveryIntegration, DiscoveryMulticast)
{
    init_scenario(1);
    discovery_->multicast();
}

int main(int args, char** argv)
{
    ::testing::InitGoogleTest(&args, argv);
    return RUN_ALL_TESTS();
}