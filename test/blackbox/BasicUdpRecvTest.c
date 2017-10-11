#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include <transport/ddsxrce_transport.h>


int main(int argc, char *argv[])
{
    printf("\nAt the very beginning everything was black\n\n");

    octet buffer[1024] = {};
    int len = 0;

    locator_id_t loc_id = add_udp_locator(2019, 2020);

    int loops = 10;
    while (loops--)
    {
        if (0 < (len = receive_data(buffer, sizeof(buffer), loc_id)))
        {
            printf(">> '%s'\n", buffer);
        }
        else
        {
            printf("# recv len %d\n", len);
        }
    }

    printf("exiting...\n");
    return 0;
}
