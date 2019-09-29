// ringbuff_dev.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include "string.h"
#include "ringbuff/ringbuff.h"

/* Create data array and buffer */
uint8_t ringbuff_data[8 + 1];
ringbuff_t buff;

static void debug_buff(uint8_t cmp, size_t r_w, size_t r_r, size_t r_f, size_t r_e);

uint8_t tmp[8];

int
main() {
    void* ptr;
    size_t len;
    
    /* Init buffer */
    ringbuff_init(&buff, ringbuff_data, sizeof(ringbuff_data));

    ringbuff_write(&buff, "abc", 3);
    ringbuff_write(&buff, "abc", 3);
    ringbuff_write(&buff, "abc", 3);
    len = ringbuff_read(&buff, tmp, 9);

    buff.r = 0;
    buff.w = 0;
    memset(ringbuff_get_linear_block_write_address(&buff), 'A', ringbuff_get_linear_block_write_length(&buff));
    ringbuff_advance(&buff, ringbuff_get_linear_block_write_length(&buff));

    buff.r = 2;
    buff.w = 0;
    memset(ringbuff_get_linear_block_write_address(&buff), 'B', ringbuff_get_linear_block_write_length(&buff));
    ringbuff_advance(&buff, ringbuff_get_linear_block_write_length(&buff));

    buff.r = 3;
    buff.w = 3;
    memset(ringbuff_get_linear_block_write_address(&buff), 'C', ringbuff_get_linear_block_write_length(&buff));
    ringbuff_advance(&buff, ringbuff_get_linear_block_write_length(&buff));

    //for (size_t r = 0; r < sizeof(ringbuff_data); r++) {
    //    for (size_t w = 0; w < sizeof(ringbuff_data); w++) {
    //        buff.r = r;
    //        buff.w = w;
    //        ptr = ringbuff_get_linear_block_write_address(&buff);
    //        len = ringbuff_get_linear_block_write_length(&buff);
    //        printf("W: %3d, R: %3d, LEN: %3d\r\n", (int)w, (int)r, (int)len);
    //    }
    //}

    return 0;
}

static void
debug_buff(uint8_t cmp, size_t r_w, size_t r_r, size_t r_f, size_t r_e) {
    /* Previous and current write, read pointers and full, empty values */
    static size_t p_r, p_w, p_f, p_e;
    size_t r, w, f, e;

    r = buff.r;
    w = buff.w;
    f = ringbuff_get_full(&buff);
    e = ringbuff_get_free(&buff);

    printf("R: %3d, W: %3d, F: %3d, E: %3d\r\n", (int)r, (int)w, (int)f, (int)e);
}
