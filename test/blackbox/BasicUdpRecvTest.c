#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <micrortps/transport/micrortps_transport.h>

int main(int argc, char *argv[])
{
    printf("\nAt the very beginning everything was black\n\n");

    octet_t buffer[1024] = {0};
    int len = 0;
    micrortps_locator_t locator;

    locator_id_t loc_id = add_udp_locator_agent(2019, &locator);

    int loops = 10;
    while (loops--)
    {
        if (0 < (len = receive_data(buffer, sizeof(buffer), loc_id)))
        {
            printf(">> '%s'\n", buffer);
            return 0;
        }
        else
        {
            printf("# recv len %d\n", len);
        }
    }

    printf("exiting...\n");
    return -1;
}
