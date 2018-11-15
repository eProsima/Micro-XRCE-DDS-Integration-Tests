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

#if defined(PLATFORM_NAME_LINUX)
#include <uxr/agent/transport/udp/UDPServerLinux.hpp>
#include <uxr/agent/transport/tcp/TCPServerLinux.hpp>
#elif defined(PLATFORM_NAME_WINDOWS)
#include <uxr/agent/transport/udp/UDPServerWindows.hpp>
#include <uxr/agent/transport/tcp/TCPServerWindows.hpp>
#endif

#include <gtest/gtest.h>
#include <cstdlib>

#define UDP_TRANSPORT 1
#define TCP_TRANSPORT 2

class ShapesDemoTest : public ::testing::TestWithParam<int>
{
public:
    const uint16_t AGENT_PORT = 2018;
    ShapesDemoTest()
        : transport_(GetParam())
        , agent_(init_agent(AGENT_PORT))
    {
        agent_->run();
    }

    void TearDown() override
    {
        std::string echo = "echo '";
        std::string executable = "' | build/client_clone/examples/ShapesDemo/ShapeDemoClient ";
        std::string args = ((UDP_TRANSPORT == transport_) ? "--udp" : "--tcp") + std::string(" 127.0.0.1 ") + std::to_string(AGENT_PORT);

        std::string commands = "";
        for(std::vector<std::string>::iterator it = commands_.begin() ; it != commands_.end(); ++it)
        {
            commands.append(*it + "\n ");
        }

        std::string execution = echo + commands + executable + args;
        std::cout << execution << std::endl;
        int shape_demo_app_result = std::system(execution.c_str());

        agent_->stop();
        ASSERT_EQ(0, shape_demo_app_result);
    }

protected:
    eprosima::uxr::Server* init_agent(uint16_t port)
    {
        eprosima::uxr::Server* agent;
        switch(transport_)
        {
            case UDP_TRANSPORT:
                agent = new eprosima::uxr::UDPServer(port);
                break;
            case TCP_TRANSPORT:
                agent = new eprosima::uxr::TCPServer(port);
                break;
        }

        return agent;
    }

    int transport_;
    std::unique_ptr<eprosima::uxr::Server> agent_;
    std::vector<std::string> commands_;
};

/* ############################################## TESTS ##################################################### */

INSTANTIATE_TEST_CASE_P(Transport, ShapesDemoTest, ::testing::Values(UDP_TRANSPORT, TCP_TRANSPORT), ::testing::PrintToStringParamName());

TEST_P(ShapesDemoTest, CreateSession)
{
    commands_.push_back("create_session");
    commands_.push_back("exit");
}

TEST_P(ShapesDemoTest, CreateDeleteSession)
{
    commands_.push_back("create_session");
    commands_.push_back("delete_session");
    commands_.push_back("exit");
}

