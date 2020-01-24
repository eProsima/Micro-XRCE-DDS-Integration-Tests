#include "CLI.hpp"

int main(int argc, char** argv)
{
    CLI::App app("Micro XRCE-DDS Performance Tets");
    app.require_subcommand(1, 1);
    app.get_formatter()->column_width(42);

    UDPSubcommand udp_subcommand(app);
    TCPSubcommand tcp_subcommand(app);

    app.parse(argc, argv);

    return 0;
}
