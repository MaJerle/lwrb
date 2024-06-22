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
my_buff_evt_fn(lwrb_t* buff, lwrb_evt_type_t type, lwrb_sz_t len) {
    (void)buff;
    (void)len;
    switch (type) {
        case LWRB_EVT_RESET: printf("[EVT] Buffer reset event!\r\n"); break;
        case LWRB_EVT_READ: printf("[EVT] Buffer read event: %d byte(s)!\r\n", (int)len); break;
        case LWRB_EVT_WRITE: printf("[EVT] Buffer write event: %d byte(s)!\r\n", (int)len); break;
        default: break;
    }
}

int
main() {
    lwrb_sz_t len;

    /* Init buffer */
    lwrb_init(&buff, lwrb_data, sizeof(lwrb_data));

    printf("Read/Write test\r\n");
    {
        uint8_t rw_buff[8];

#define RW_TEST(_w_exp_, _r_exp_, _rw_len_, _rw_exp_len_)                                                              \
    do {                                                                                                               \
        printf("W ptr: %u, R ptr: %u, R/W len: %u, as_expected: %u\r\n", (unsigned)buff.w_ptr, (unsigned)buff.r_ptr,   \
               (unsigned)(_rw_len_),                                                                                   \
               (unsigned)(buff.w_ptr == (_w_exp_) && buff.r_ptr == (_r_exp_) && (_rw_len_) == (_rw_exp_len_)));        \
    } while (0)

        lwrb_reset(&buff);
        len = lwrb_write(&buff, "abc", 3); /* Write 3 bytes */
        RW_TEST(3, 0, len, 3);
        len = lwrb_write(&buff, "abc", 3); /* Write 3 bytes */
        RW_TEST(6, 0, len, 3);
        len = lwrb_read(&buff, rw_buff, 3); /* Read 3 bytes */
        RW_TEST(6, 3, len, 3);
        len = lwrb_read(&buff, rw_buff, 4); /* Read 4 bytes */
        RW_TEST(6, 6, len, 3);

        len = lwrb_write(&buff, "abc", 3); /* Write 3 bytes -> buffer should go over */
        RW_TEST(0, 6, len, 3);

#undef RW_TEST
    }

    printf("Read/Write extended test\r\n");
    {
        uint8_t rw_buff[8];
        lwrb_sz_t written, read;
        uint8_t success;

#define RW_TEST(_w_exp_, _r_exp_, _success_, _rw_len_, _rw_exp_len_)                                                   \
    do {                                                                                                               \
        printf("W ptr: %u, R ptr: %u, R/W success: %u, R/W len: %u, as_expected: %u\r\n", (unsigned)buff.w_ptr,        \
               (unsigned)buff.r_ptr, (unsigned)(_success_), (unsigned)(_rw_len_),                                      \
               (unsigned)(buff.w_ptr == (_w_exp_) && buff.r_ptr == (_r_exp_) && (_rw_len_) == (_rw_exp_len_)));        \
    } while (0)

        lwrb_reset(&buff);
        written = 0;
        success = lwrb_write_ex(&buff, "abcdefg", 7, &written, LWRB_FLAG_WRITE_ALL); /* Write all bytes */
        RW_TEST(7, 0, success, written, 7);
        success = lwrb_read_ex(&buff, rw_buff, 3, &read, LWRB_FLAG_READ_ALL); /* Read 3 bytes only */
        printf("RW FULL READ: %u, as_expected: %u\r\n", (unsigned)success, (unsigned)(success == 1));
        RW_TEST(7, 3, success, written, 7);

        /* This one shall failed, not enough memory available */
        success = lwrb_write_ex(&buff, "abcdefg", 7, &written, LWRB_FLAG_WRITE_ALL); /* Write all bytes */
        printf("RW FULL WRITE: %u, as_expected: %u\r\n", (unsigned)success, (unsigned)(success == 0));

        /* Read few more bytes to allow full write */
        success = lwrb_read_ex(&buff, rw_buff, 3, &read, LWRB_FLAG_READ_ALL); /* Read 3 bytes only */
        printf("RW FULL READ: %u, as_expected: %u\r\n", (unsigned)success, (unsigned)(success == 1));

        /* Now it should go through */
        success = lwrb_write_ex(&buff, "abcdefg", 7, &written, LWRB_FLAG_WRITE_ALL); /* Write all bytes */
        printf("RW FULL WRITE: %u, as_expected: %u\r\n", (unsigned)success, (unsigned)(success == 1));

#undef RW_TEST
    }

    printf("Overwrite test\r\n");
    {
#define OVERWRITE_TEST(_exp_content_, _exp_len_)                                                                       \
    do {                                                                                                               \
        len = lwrb_peek(&buff, 0, tmp, buff.size);                                                                     \
        printf("overwrite data read: %.*s, len: %u, as_expected: %u\r\n", (int)len, tmp, (unsigned)len,                \
               (unsigned)(strncmp((_exp_content_), (const void*)tmp, len) == 0 && len == (_exp_len_)));                \
    } while (0)

        /* Test overwrite */
        lwrb_reset(&buff);
        lwrb_write(&buff, "abcdef", 6); /* Initial data */
        OVERWRITE_TEST("abcdef", 6);

        lwrb_overwrite(&buff, "0", 1);
        OVERWRITE_TEST("abcdef0", 7);

        lwrb_overwrite(&buff, "1", 1);
        OVERWRITE_TEST("abcdef01", 8);

        lwrb_overwrite(&buff, "2", 1);
        OVERWRITE_TEST("bcdef012", 8);

        lwrb_overwrite(&buff, "3", 1);
        OVERWRITE_TEST("cdef0123", 8);

        lwrb_overwrite(&buff, "4", 1);
        OVERWRITE_TEST("def01234", 8);

        lwrb_overwrite(&buff, "5", 1);
        OVERWRITE_TEST("ef012345", 8);

        /* Bigger write which will completely change the buffer structure */
        lwrb_overwrite(&buff, "lwrb_new_test_structure", 23);
        OVERWRITE_TEST("tructure", 8);
#undef OVERWRITE_TEST
    }

    printf("Move test\r\n");
    {
#define MOVE_TEST(_exp_content_, _exp_move_len_, _exp_buff_len_)                                                       \
    do {                                                                                                               \
        lwrb_sz_t move_len;                                                                                            \
        move_len = lwrb_move(&dst, &src);                                                                              \
        len = lwrb_peek(&dst, 0, tmp, dst.size);                                                                       \
        printf("move data: len: %d, dest data: %.*s, as_expected: %u\r\n", (int)len, (int)len, tmp,                    \
               (unsigned)(strncmp((_exp_content_), (const void*)tmp, len) == 0 && move_len == (_exp_move_len_)         \
                          && len == (_exp_buff_len_)));                                                                \
    } while (0)

        lwrb_t src, dst;
        uint8_t src_data[16], dst_data[8];
        lwrb_init(&src, src_data, sizeof(src_data));
        lwrb_init(&dst, dst_data, sizeof(dst_data));

        lwrb_reset(&src);
        lwrb_reset(&dst);
        lwrb_write(&src, "012345", 6);
        MOVE_TEST("012345", 6, 6);

        lwrb_reset(&src);
        lwrb_reset(&dst);
        lwrb_write(&src, "0123456789ABCDEF", 16);
        MOVE_TEST("0123456", 7, 7);

        lwrb_reset(&src);
        lwrb_reset(&dst);
        lwrb_write(&src, "0123456789ABCDEF", 16);
        lwrb_write(&dst, "TT", 2);
        MOVE_TEST("TT01234", 5, 7);

#undef MOVE_TEST
    }

    (void)len;

    printf("Find test\r\n");
    {
#define FIND_TEST(_bts_, _bts_len_, _start_offset_, _exp_result_)                                                      \
    do {                                                                                                               \
        lwrb_sz_t found_idx;                                                                                           \
        uint8_t found;                                                                                                 \
        found = lwrb_find(&buff, (_bts_), (_bts_len_), (_start_offset_), &found_idx);                                  \
        printf("Find \"%s\" (len %d), start_offset: %d, found_index: %d; Found: %d; As expected: %d\r\n", (_bts_),     \
               (_bts_len_), (_start_offset_), (int)found_idx, (int)found, (int)(!!found == !!(_exp_result_)));         \
    } while (0)

        /* Prepare buffer and write data */
        lwrb_reset(&buff);
        lwrb_write(&buff, "12345678", 8);

        FIND_TEST("123", 3, 0, 1); /* Must find it */
        FIND_TEST("456", 3, 0, 1); /* Must find it */
        FIND_TEST("123", 3, 1, 0); /* Must not find it - start offset is later */
        FIND_TEST("678", 3, 0, 1);

        /* Restart by setting write and read as empty with offset */
        buff.w_ptr = 6;
        buff.r_ptr = 6;
        lwrb_write(&buff, "12345678", 8);

        FIND_TEST("123", 3, 0, 1); /* Must find it */
        FIND_TEST("456", 3, 0, 1); /* Must find it */
        FIND_TEST("123", 3, 1, 0); /* Must not find it - start offset is later */

        /* Restart by setting write and read as empty with offset */
        /* This should generate data for search in overflow mode */
        buff.w_ptr = 8;
        buff.r_ptr = 8;
        lwrb_write(&buff, "12345678", 8);

        FIND_TEST("1234", 3, 0, 1); /* Must find it */
        FIND_TEST("4567", 3, 0, 1); /* Must find it */
        FIND_TEST("1234", 3, 1, 0); /* Must not find it - start offset is later */

#undef FIND_TEST
    }
    return 0;
}
