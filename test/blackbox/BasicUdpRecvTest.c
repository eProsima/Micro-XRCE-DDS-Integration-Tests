#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include <transport/micrortps_transport.h>

int main(int argc, char *argv[])
{
    printf("\nAt the very beginning everything was black\n\n");

    octet_t buffer[1024] = {""};
    int len = 0;

    locator_id_t loc_id = add_udp_locator(2021, 2019, 2020, "127.0.0.1");

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
