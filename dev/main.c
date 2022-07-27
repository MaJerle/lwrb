// lwrb_dev.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string.h>
#include "lwrb/lwrb.h"

/* Create data array and buffer */
uint8_t lwrb_data[8 + 1];
lwrb_t buff;

uint8_t tmp[8];

void
my_buff_evt_fn(lwrb_t* buff, lwrb_evt_type_t type, size_t len) {
    (void)buff;
    switch (type) {
        case LWRB_EVT_RESET:
            printf("[EVT] Buffer reset event!\r\n");
            break;
        case LWRB_EVT_READ:
            printf("[EVT] Buffer read event: %d byte(s)!\r\n", (int)len);
            break;
        case LWRB_EVT_WRITE:
            printf("[EVT] Buffer write event: %d byte(s)!\r\n", (int)len);
            break;
        default: break;
    }
}

int
main() {
    size_t len;

    /* Init buffer */
    lwrb_init(&buff, lwrb_data, sizeof(lwrb_data));
    lwrb_set_evt_fn(&buff, my_buff_evt_fn);

    lwrb_write(&buff, "abc", 3);
    lwrb_write(&buff, "abc", 3);
    lwrb_write(&buff, "abc", 3);
    len = lwrb_read(&buff, tmp, 9);

    buff.r = 0;
    buff.w = 0;
    memset(lwrb_get_linear_block_write_address(&buff), 'A', lwrb_get_linear_block_write_length(&buff));
    lwrb_advance(&buff, lwrb_get_linear_block_write_length(&buff));

    buff.r = 2;
    buff.w = 0;
    memset(lwrb_get_linear_block_write_address(&buff), 'B', lwrb_get_linear_block_write_length(&buff));
    lwrb_advance(&buff, lwrb_get_linear_block_write_length(&buff));

    buff.r = 3;
    buff.w = 3;
    memset(lwrb_get_linear_block_write_address(&buff), 'C', lwrb_get_linear_block_write_length(&buff));
    lwrb_advance(&buff, lwrb_get_linear_block_write_length(&buff));

    lwrb_reset(&buff);

    (void)len;

    return 0;
}
