#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "ddsxrce_transport.h"


int main(int argc, char *argv[])
{
    printf("\nAt the very beginning everything was black\n\n");

    octet buffer[256] = {"Mensaje_del_senderA"};
    size_t buffer_len = 256;
    int len = 0;

    locator_id_t loc_id = add_udp_locator(2020, 2019);

    int loops = 10;
    while (loops--)
    {
        ++buffer[18];
        if (0 < (len = send_data(buffer, strlen("Mensaje_del_sender_") + 1, loc_id)))
        {
            printf("<< '%s'\n", buffer);
            break;
        }
        else
        {
            printf("# send len %d\n", len);
        }

        usleep(1000000);
    }

    printf("exiting...\n");
    return 0;
}
