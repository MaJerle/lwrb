/**
 * \file            lwrb.c
 * \brief           Lightweight ring buffer
 */

/*
 * Copyright (c) 2024 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwRB - Lightweight ring buffer library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v3.1.0
 */
#include "lwrb/lwrb.h"

/* Memory set and copy functions */
#define BUF_MEMSET      memset
#define BUF_MEMCPY      memcpy

#define BUF_IS_VALID(b) ((b) != NULL && (b)->buff != NULL && (b)->size > 0)
#define BUF_MIN(x, y)   ((x) < (y) ? (x) : (y))
#define BUF_MAX(x, y)   ((x) > (y) ? (x) : (y))
#define BUF_SEND_EVT(b, type, bp)                                                                                      \
    do {                                                                                                               \
        if ((b)->evt_fn != NULL) {                                                                                     \
            (b)->evt_fn((void*)(b), (type), (bp));                                                                     \
        }                                                                                                              \
    } while (0)

/* Optional atomic opeartions */
#ifdef LWRB_DISABLE_ATOMIC
#define LWRB_INIT(var, val)        (var) = (val)
#define LWRB_LOAD(var, type)       (var)
#define LWRB_STORE(var, val, type) (var) = (val)
#else
#define LWRB_INIT(var, val)        atomic_init(&(var), (val))
#define LWRB_LOAD(var, type)       atomic_load_explicit(&(var), (type))
#define LWRB_STORE(var, val, type) atomic_store_explicit(&(var), (val), (type))
#endif

/**
 * \brief           Initialize buffer handle to default values with size and buffer data array
 * \param[in]       buff: Ring buffer instance
 * \param[in]       buffdata: Pointer to memory to use as buffer data
 * \param[in]       size: Size of `buffdata` in units of bytes
 *                      Maximum number of bytes buffer can hold is `size - 1`
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwrb_init(lwrb_t* buff, void* buffdata, lwrb_sz_t size) {
    if (buff == NULL || buffdata == NULL || size == 0) {
        return 0;
    }

    buff->evt_fn = NULL;
    buff->size = size;
    buff->buff = buffdata;
    LWRB_INIT(buff->w, 0);
    LWRB_INIT(buff->r, 0);
    return 1;
}

/**
 * \brief           Check if buff is initialized and ready to use
 * \param[in]       buff: Ring buffer instance
 * \return          `1` if ready, `0` otherwise
 */
uint8_t
lwrb_is_ready(lwrb_t* buff) {
    return BUF_IS_VALID(buff);
}

/**
 * \brief           Free buffer memory
 * \note            Since implementation does not use dynamic allocation,
 *                  it just sets buffer handle to `NULL`
 * \param[in]       buff: Ring buffer instance
 */
void
lwrb_free(lwrb_t* buff) {
    if (BUF_IS_VALID(buff)) {
        buff->buff = NULL;
    }
}

/**
 * \brief           Set event function callback for different buffer operations
 * \param[in]       buff: Ring buffer instance
 * \param[in]       evt_fn: Callback function
 */
void
lwrb_set_evt_fn(lwrb_t* buff, lwrb_evt_fn evt_fn) {
    if (BUF_IS_VALID(buff)) {
        buff->evt_fn = evt_fn;
    }
}

/**
 * \brief           Set custom buffer argument, that can be retrieved in the event function
 * \param[in]       buff: Ring buffer instance
 * \param[in]       arg: Custom user argument
 */
void
lwrb_set_arg(lwrb_t* buff, void* arg) {
    if (BUF_IS_VALID(buff)) {
        buff->arg = arg;
    }
}

/**
 * \brief           Get custom buffer argument, previously set with \ref lwrb_set_arg
 * \param[in]       buff: Ring buffer instance
 * \return          User argument, previously set with \ref lwrb_set_arg
 */
void*
lwrb_get_arg(lwrb_t* buff) {
    return buff != NULL ? buff->arg : NULL;
}

/**
 * \brief           Write data to buffer.
 * Copies data from `data` array to buffer and marks buffer as full for maximum `btw` number of bytes
 *
 * \param[in]       buff: Ring buffer instance
 * \param[in]       data: Pointer to data to write into buffer
 * \param[in]       btw: Number of bytes to write
 * \return          Number of bytes written to buffer.
 *                      When returned value is less than `btw`, there was no enough memory available
 *                      to copy full data array.
 */
lwrb_sz_t
lwrb_write(lwrb_t* buff, const void* data, lwrb_sz_t btw) {
    lwrb_sz_t written = 0;

    if (lwrb_write_ex(buff, data, btw, &written, 0)) {
        return written;
    }
    return 0;
}

/**
 * \brief           Write extended functionality
 * 
 * \param           buff: Ring buffer instance
 * \param           data: Pointer to data to write into buffer
 * \param           btw: Number of bytes to write
 * \param           bw: Output pointer to write number of bytes written
 * \param           flags: Optional flags.
 *                      \ref LWRB_FLAG_WRITE_ALL: Request to write all data (up to btw).
 *                          Will early return if no memory available
 * \return          `1` if write operation OK, `0` otherwise
 */
uint8_t
lwrb_write_ex(lwrb_t* buff, const void* data, lwrb_sz_t btw, lwrb_sz_t* bw, uint16_t flags) {
    lwrb_sz_t tocopy, free, buff_w_ptr;
    const uint8_t* d = data;

    if (!BUF_IS_VALID(buff) || data == NULL || btw == 0) {
        return 0;
    }

    /* Calculate maximum number of bytes available to write */
    free = lwrb_get_free(buff);
    /* If no memory, or if user wants to write ALL data but no enough space, exit early */
    if (free == 0 || (free < btw && flags & LWRB_FLAG_WRITE_ALL)) {
        return 0;
    }
    btw = BUF_MIN(free, btw);
    buff_w_ptr = LWRB_LOAD(buff->w, memory_order_acquire);

    /* Step 1: Write data to linear part of buffer */
    tocopy = BUF_MIN(buff->size - buff_w_ptr, btw);
    BUF_MEMCPY(&buff->buff[buff_w_ptr], d, tocopy);
    buff_w_ptr += tocopy;
    btw -= tocopy;

    /* Step 2: Write data to beginning of buffer (overflow part) */
    if (btw > 0) {
        BUF_MEMCPY(buff->buff, &d[tocopy], btw);
        buff_w_ptr = btw;
    }

    /* Step 3: Check end of buffer */
    if (buff_w_ptr >= buff->size) {
        buff_w_ptr = 0;
    }

    /*
     * Write final value to the actual running variable.
     * This is to ensure no read operation can access intermediate data
     */
    LWRB_STORE(buff->w, buff_w_ptr, memory_order_release);

    BUF_SEND_EVT(buff, LWRB_EVT_WRITE, tocopy + btw);
    if (bw != NULL) {
        *bw = tocopy + btw;
    }
    return 1;
}

/**
 * \brief           Read data from buffer.
 * Copies data from buffer to `data` array and marks buffer as free for maximum `btr` number of bytes
 *
 * \param[in]       buff: Ring buffer instance
 * \param[out]      data: Pointer to output memory to copy buffer data to
 * \param[in]       btr: Number of bytes to read
 * \return          Number of bytes read and copied to data array
 */
lwrb_sz_t
lwrb_read(lwrb_t* buff, void* data, lwrb_sz_t btr) {
    lwrb_sz_t read = 0;

    if (lwrb_read_ex(buff, data, btr, &read, 0)) {
        return read;
    }
    return 0;
}

/**
 * \brief           Write extended functionality
 * 
 * \param           buff: Ring buffer instance
 * \param           data: Pointer to memory to write read data from buffer 
 * \param           btr: Number of bytes to read
 * \param           br: Output pointer to write number of bytes read
 * \param           flags: Optional flags
 *                      \ref LWRB_FLAG_READ_ALL: Request to read all data (up to btr).
 *                          Will early return if no enough bytes in the buffer
 * \return          `1` if read operation OK, `0` otherwise
 */
uint8_t
lwrb_read_ex(lwrb_t* buff, void* data, lwrb_sz_t btr, lwrb_sz_t* br, uint16_t flags) {
    lwrb_sz_t tocopy, full, buff_r_ptr;
    uint8_t* d = data;

    if (!BUF_IS_VALID(buff) || data == NULL || btr == 0) {
        return 0;
    }

    /* Calculate maximum number of bytes available to read */
    full = lwrb_get_full(buff);
    if (full == 0 || (full < btr && (flags & LWRB_FLAG_READ_ALL))) {
        return 0;
    }
    btr = BUF_MIN(full, btr);
    buff_r_ptr = LWRB_LOAD(buff->r, memory_order_acquire);

    /* Step 1: Read data from linear part of buffer */
    tocopy = BUF_MIN(buff->size - buff_r_ptr, btr);
    BUF_MEMCPY(d, &buff->buff[buff_r_ptr], tocopy);
    buff_r_ptr += tocopy;
    btr -= tocopy;

    /* Step 2: Read data from beginning of buffer (overflow part) */
    if (btr > 0) {
        BUF_MEMCPY(&d[tocopy], buff->buff, btr);
        buff_r_ptr = btr;
    }

    /* Step 3: Check end of buffer */
    if (buff_r_ptr >= buff->size) {
        buff_r_ptr = 0;
    }

    /*
     * Write final value to the actual running variable.
     * This is to ensure no write operation can access intermediate data
     */
    LWRB_STORE(buff->r, buff_r_ptr, memory_order_release);

    BUF_SEND_EVT(buff, LWRB_EVT_READ, tocopy + btr);
    if (br != NULL) {
        *br = tocopy + btr;
    }
    return 1;
}

/**
 * \brief           Read from buffer without changing read pointer (peek only)
 * \param[in]       buff: Ring buffer instance
 * \param[in]       skip_count: Number of bytes to skip before reading data
 * \param[out]      data: Pointer to output memory to copy buffer data to
 * \param[in]       btp: Number of bytes to peek
 * \return          Number of bytes peeked and written to output array
 */
lwrb_sz_t
lwrb_peek(const lwrb_t* buff, lwrb_sz_t skip_count, void* data, lwrb_sz_t btp) {
    lwrb_sz_t full, tocopy, r;
    uint8_t* d = data;

    if (!BUF_IS_VALID(buff) || data == NULL || btp == 0) {
        return 0;
    }

    /*
     * Calculate maximum number of bytes available to read
     * and check if we can even fit to it
     */
    full = lwrb_get_full(buff);
    if (skip_count >= full) {
        return 0;
    }
    r = LWRB_LOAD(buff->r, memory_order_relaxed);
    r += skip_count;
    full -= skip_count;
    if (r >= buff->size) {
        r -= buff->size;
    }

    /* Check maximum number of bytes available to read after skip */
    btp = BUF_MIN(full, btp);
    if (btp == 0) {
        return 0;
    }

    /* Step 1: Read data from linear part of buffer */
    tocopy = BUF_MIN(buff->size - r, btp);
    BUF_MEMCPY(d, &buff->buff[r], tocopy);
    btp -= tocopy;

    /* Step 2: Read data from beginning of buffer (overflow part) */
    if (btp > 0) {
        BUF_MEMCPY(&d[tocopy], buff->buff, btp);
    }
    return tocopy + btp;
}

/**
 * \brief           Get available size in buffer for write operation
 * \param[in]       buff: Ring buffer instance
 * \return          Number of free bytes in memory
 */
lwrb_sz_t
lwrb_get_free(const lwrb_t* buff) {
    lwrb_sz_t size, w, r;

    if (!BUF_IS_VALID(buff)) {
        return 0;
    }

    /*
     * Copy buffer pointers to local variables with atomic access.
     *
     * To ensure thread safety (only when in single-entry, single-exit FIFO mode use case),
     * it is important to write buffer r and w values to local w and r variables.
     *
     * Local variables will ensure below if statements will always use the same value,
     * even if buff->w or buff->r get changed during interrupt processing.
     *
     * They may change during load operation, important is that
     * they do not change during if-else operations following these assignments.
     *
     * lwrb_get_free is only called for write purpose, and when in FIFO mode, then:
     * - buff->w pointer will not change by another process/interrupt because we are in write mode just now
     * - buff->r pointer may change by another process. If it gets changed after buff->r has been loaded to local variable,
     *    buffer will see "free size" less than it actually is. This is not a problem, application can
     *    always try again to write more data to remaining free memory that was read just during copy operation
     */
    w = LWRB_LOAD(buff->w, memory_order_relaxed);
    r = LWRB_LOAD(buff->r, memory_order_relaxed);

    if (w >= r) {
        size = buff->size - (w - r);
    } else {
        size = r - w;
    }

    /* Buffer free size is always 1 less than actual size */
    return size - 1;
}

/**
 * \brief           Get number of bytes currently available in buffer
 * \param[in]       buff: Ring buffer instance
 * \return          Number of bytes ready to be read
 */
lwrb_sz_t
lwrb_get_full(const lwrb_t* buff) {
    lwrb_sz_t size, w, r;

    if (!BUF_IS_VALID(buff)) {
        return 0;
    }

    /*
     * Copy buffer pointers to local variables.
     *
     * To ensure thread safety (only when in single-entry, single-exit FIFO mode use case),
     * it is important to write buffer r and w values to local w and r variables.
     *
     * Local variables will ensure below if statements will always use the same value,
     * even if buff->w or buff->r get changed during interrupt processing.
     *
     * They may change during load operation, important is that
     * they do not change during if-else operations following these assignments.
     *
     * lwrb_get_full is only called for read purpose, and when in FIFO mode, then:
     * - buff->r pointer will not change by another process/interrupt because we are in read mode just now
     * - buff->w pointer may change by another process. If it gets changed after buff->w has been loaded to local variable,
     *    buffer will see "full size" less than it really is. This is not a problem, application can
     *    always try again to read more data from remaining full memory that was written just during copy operation
     */
    w = LWRB_LOAD(buff->w, memory_order_relaxed);
    r = LWRB_LOAD(buff->r, memory_order_relaxed);

    if (w >= r) {
        size = w - r;
    } else {
        size = buff->size - (r - w);
    }
    return size;
}

/**
 * \brief           Resets buffer to default values. Buffer size is not modified
 * \note            This function is not thread safe.
 *                      When used, application must ensure there is no active read/write operation
 * \param[in]       buff: Ring buffer instance
 */
void
lwrb_reset(lwrb_t* buff) {
    if (BUF_IS_VALID(buff)) {
        LWRB_STORE(buff->w, 0, memory_order_release);
        LWRB_STORE(buff->r, 0, memory_order_release);
        BUF_SEND_EVT(buff, LWRB_EVT_RESET, 0);
    }
}

/**
 * \brief           Get linear address for buffer for fast read
 * \param[in]       buff: Ring buffer instance
 * \return          Linear buffer start address
 */
void*
lwrb_get_linear_block_read_address(const lwrb_t* buff) {
    if (!BUF_IS_VALID(buff)) {
        return NULL;
    }
    return &buff->buff[buff->r];
}

/**
 * \brief           Get length of linear block address before it overflows for read operation
 * \param[in]       buff: Ring buffer instance
 * \return          Linear buffer size in units of bytes for read operation
 */
lwrb_sz_t
lwrb_get_linear_block_read_length(const lwrb_t* buff) {
    lwrb_sz_t len, w, r;

    if (!BUF_IS_VALID(buff)) {
        return 0;
    }

    /*
     * Use temporary values in case they are changed during operations.
     * See lwrb_buff_free or lwrb_buff_full functions for more information why this is OK.
     */
    w = LWRB_LOAD(buff->w, memory_order_relaxed);
    r = LWRB_LOAD(buff->r, memory_order_relaxed);

    if (w > r) {
        len = w - r;
    } else if (r > w) {
        len = buff->size - r;
    } else {
        len = 0;
    }
    return len;
}

/**
 * \brief           Skip (ignore; advance read pointer) buffer data
 * Marks data as read in the buffer and increases free memory for up to `len` bytes
 *
 * \note            Useful at the end of streaming transfer such as DMA
 * \param[in]       buff: Ring buffer instance
 * \param[in]       len: Number of bytes to skip and mark as read
 * \return          Number of bytes skipped
 */
lwrb_sz_t
lwrb_skip(lwrb_t* buff, lwrb_sz_t len) {
    lwrb_sz_t full, r;

    if (!BUF_IS_VALID(buff) || len == 0) {
        return 0;
    }

    full = lwrb_get_full(buff);
    len = BUF_MIN(len, full);
    r = LWRB_LOAD(buff->r, memory_order_acquire);
    r += len;
    if (r >= buff->size) {
        r -= buff->size;
    }
    LWRB_STORE(buff->r, r, memory_order_release);
    BUF_SEND_EVT(buff, LWRB_EVT_READ, len);
    return len;
}

/**
 * \brief           Get linear address for buffer for fast read
 * \param[in]       buff: Ring buffer instance
 * \return          Linear buffer start address
 */
void*
lwrb_get_linear_block_write_address(const lwrb_t* buff) {
    if (!BUF_IS_VALID(buff)) {
        return NULL;
    }
    return &buff->buff[buff->w];
}

/**
 * \brief           Get length of linear block address before it overflows for write operation
 * \param[in]       buff: Ring buffer instance
 * \return          Linear buffer size in units of bytes for write operation
 */
lwrb_sz_t
lwrb_get_linear_block_write_length(const lwrb_t* buff) {
    lwrb_sz_t len, w, r;

    if (!BUF_IS_VALID(buff)) {
        return 0;
    }

    /*
     * Use temporary values in case they are changed during operations.
     * See lwrb_buff_free or lwrb_buff_full functions for more information why this is OK.
     */
    w = LWRB_LOAD(buff->w, memory_order_relaxed);
    r = LWRB_LOAD(buff->r, memory_order_relaxed);

    if (w >= r) {
        len = buff->size - w;
        /*
         * When read pointer is 0,
         * maximal length is one less as if too many bytes
         * are written, buffer would be considered empty again (r == w)
         */
        if (r == 0) {
            /*
             * Cannot overflow:
             * - If r is not 0, statement does not get called
             * - buff->size cannot be 0 and if r is 0, len is greater 0
             */
            --len;
        }
    } else {
        len = r - w - 1;
    }
    return len;
}

/**
 * \brief           Advance write pointer in the buffer.
 * Similar to skip function but modifies write pointer instead of read
 *
 * \note            Useful when hardware is writing to buffer and application needs to increase number
 *                      of bytes written to buffer by hardware
 * \param[in]       buff: Ring buffer instance
 * \param[in]       len: Number of bytes to advance
 * \return          Number of bytes advanced for write operation
 */
lwrb_sz_t
lwrb_advance(lwrb_t* buff, lwrb_sz_t len) {
    lwrb_sz_t free, w;

    if (!BUF_IS_VALID(buff) || len == 0) {
        return 0;
    }

    /* Use local variables before writing back to main structure */
    free = lwrb_get_free(buff);
    len = BUF_MIN(len, free);
    w = LWRB_LOAD(buff->w, memory_order_acquire);
    w += len;
    if (w >= buff->size) {
        w -= buff->size;
    }
    LWRB_STORE(buff->w, w, memory_order_release);
    BUF_SEND_EVT(buff, LWRB_EVT_WRITE, len);
    return len;
}

/**
 * \brief           Searches for a *needle* in an array, starting from given offset.
 * 
 * \note            This function is not thread-safe. 
 * 
 * \param           buff: Ring buffer to search for needle in
 * \param           bts: Constant byte array sequence to search for in a buffer
 * \param           len: Length of the \arg bts array 
 * \param           start_offset: Start offset in the buffer
 * \param           found_idx: Pointer to variable to write index in array where bts has been found
 *                      Must not be set to `NULL`
 * \return          `1` if \arg bts found, `0` otherwise
 */
uint8_t
lwrb_find(const lwrb_t* buff, const void* bts, lwrb_sz_t len, lwrb_sz_t start_offset, lwrb_sz_t* found_idx) {
    lwrb_sz_t full, r, max_x;
    uint8_t found = 0;
    const uint8_t* needle = bts;

    if (!BUF_IS_VALID(buff) || needle == NULL || len == 0 || found_idx == NULL) {
        return 0;
    }
    *found_idx = 0;

    full = lwrb_get_full(buff);
    /* Verify initial conditions */
    if (full < (len + start_offset)) {
        return 0;
    }

    /* Max number of for loops is buff_full - input_len - start_offset of buffer length */
    max_x = full - len;
    for (lwrb_sz_t skip_x = start_offset; !found && skip_x <= max_x; ++skip_x) {
        found = 1; /* Found by default */

        /* Prepare the starting point for reading */
        r = buff->r + skip_x;
        if (r >= buff->size) {
            r -= buff->size;
        }

        /* Search in the buffer */
        for (lwrb_sz_t i = 0; i < len; ++i) {
            if (buff->buff[r] != needle[i]) {
                found = 0;
                break;
            }
            if (++r >= buff->size) {
                r = 0;
            }
        }
        if (found) {
            *found_idx = skip_x;
        }
    }
    return found;
}
