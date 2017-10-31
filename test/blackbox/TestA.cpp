#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

#include "MessageOutput.h"
#include "MessageHeader.h"

using namespace eprosima::micrortps;
using namespace eprosima::micrortps::debug;

int main(int /*argc*/, char** /*argv*/)
{
    printf("\nAt the very beginning everything was black\n\n");


    MessageHeader header;
    ClientKey key = {0x0A, 0x0B, 0x0C, 0x0D};
    header.client_key(key);
    header.session_id(0x13);
    header.stream_id(0x22);
    header.sequence_nr(0xABCD);
    print_message_header(header);


    printf("exiting...\n");
    return -1;
}
