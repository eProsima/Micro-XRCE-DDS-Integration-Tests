#ifndef IN_TEST_PERFORMANCE_CLI_HPP_
#define IN_TEST_PERFORMANCE_CLI_HPP_

#include "PerformanceTest.hpp"

#include <EntitiesInfo.hpp>
#include <TransportInfo.hpp>

#include <CLI/CLI.hpp>

/*************************************************************************************************
 * Middleware CLI Option
 *************************************************************************************************/
class MiddlewareOpt
{
public:
    MiddlewareOpt(CLI::App& subcommand)
        : kind_{"dds"}
        , set_{}
        , cli_opt_{}
    {
        set_.insert("dds");
        set_.insert("ced");
        cli_opt_ = subcommand.add_set("-m,--middleware", kind_, set_, "Select the kind of Middleware", true);
    }

    MiddlewareKind get_kind() const
    {
        if ("dds" == kind_)
        {
            return MiddlewareKind::FAST;
        }
        else if ("ced" == kind_)
        {
            return MiddlewareKind::CED;
        }
        else
        {
            exit(EXIT_FAILURE);
        }
    }

protected:
    std::string kind_;
    std::set<std::string> set_;
    CLI::Option* cli_opt_;
};

/*************************************************************************************************
 * Common CLI Opts
 *************************************************************************************************/
class CommonOpts
{
public:
    CommonOpts(CLI::App& subcommand)
        : middleware_opt_{subcommand}
    {}

    MiddlewareOpt middleware_opt_;
};

/*************************************************************************************************
 * Server Subcommand
 *************************************************************************************************/
class ServerSubcommand
{
public:
    ServerSubcommand(
            CLI::App& app,
            const std::string& name,
            const std::string& description,
            const CommonOpts& common_opts)
        : cli_subcommand_{app.add_subcommand(name, description)}
        , opts_ref_{common_opts}
    {
        cli_subcommand_->callback(std::bind(&ServerSubcommand::test_callback, this));
    }

    virtual ~ServerSubcommand() = default;

private:
    void test_callback()
    {
        launch_test();
    }

    virtual bool launch_test() = 0;

protected:
    CLI::App* cli_subcommand_;
    const CommonOpts& opts_ref_;
};


/*************************************************************************************************
 * UDP Subcommand
 *************************************************************************************************/
class UDPSubcommand : public ServerSubcommand
{
public:
    UDPSubcommand(CLI::App& app)
        : ServerSubcommand{app, "udp", "Launch a UDP server", common_opts_}
        , cli_ip_opt_{ cli_subcommand_->add_option("-i,--ip", ip_, "Select Agent IP")}
        , cli_port_opt_{cli_subcommand_->add_option("-p,--port", port_, "Select Agent port")}
        , common_opts_{*cli_subcommand_}
    {
        cli_ip_opt_->required(true);
        cli_port_opt_->required(true);
    }

    ~UDPSubcommand() = default;

private:
    bool launch_test() final
    {
        UDPTransportInfo transport_info;
        transport_info.ip = ip_.c_str();
        transport_info.port = port_;

        switch (common_opts_.middleware_opt_.get_kind())
        {
            case MiddlewareKind::FAST:
            {
                run_test_middleware<MiddlewareKind::FAST>(transport_info, std::chrono::seconds(1));
                break;
            }
            case MiddlewareKind::CED:
            {
                run_test_middleware<MiddlewareKind::CED>(transport_info, std::chrono::seconds(1));
                break;
            }
        }
    }

private:
    std::string ip_;
    uint16_t port_;
    CLI::Option* cli_ip_opt_;
    CLI::Option* cli_port_opt_;
    CommonOpts common_opts_;
};

/*************************************************************************************************
 * TCP Subcommand
 *************************************************************************************************/
class TCPSubcommand : public ServerSubcommand
{
public:
    TCPSubcommand(CLI::App& app)
        : ServerSubcommand{app, "tcp", "Launch a TCP server", common_opts_}
        , cli_ip_opt_{ cli_subcommand_->add_option("-i,--ip", ip_, "Select Agent IP")}
        , cli_port_opt_{cli_subcommand_->add_option("-p,--port", port_, "Select Agent port")}
        , common_opts_{*cli_subcommand_}
    {
        cli_ip_opt_->required(true);
        cli_port_opt_->required(true);
    }

    ~TCPSubcommand() = default;

private:
    bool launch_test() final
    {
        TCPTransportInfo transport_info;
        transport_info.ip = ip_.c_str();
        transport_info.port = port_;

        switch (common_opts_.middleware_opt_.get_kind())
        {
            case MiddlewareKind::FAST:
            {
                run_test_middleware<MiddlewareKind::FAST>(transport_info, std::chrono::seconds(1));
                break;
            }
            case MiddlewareKind::CED:
            {
                run_test_middleware<MiddlewareKind::CED>(transport_info, std::chrono::seconds(1));
                break;
            }
        }
    }

private:
    std::string ip_;
    uint16_t port_;
    CLI::Option* cli_ip_opt_;
    CLI::Option* cli_port_opt_;
    CommonOpts common_opts_;
};


#endif // IN_TEST_PERFORMANCE_CLI_HPP_
