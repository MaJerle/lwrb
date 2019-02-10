/**
 * \file            ringbuff.h
 * \brief           Ring buffer manager
 */

/*
 * Copyright (c) 2019 Tilen Majerle
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
 * This file is part of ring buffer library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 */
#ifndef RINGBUFF_HDR_H
#define RINGBUFF_HDR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "string.h"
#include "stdint.h"

/**
 * \defgroup        RINGBUFF Ring buffer
 * \brief           Generic FIFO ring buffer manager
 * \{
 */

/**
 * \brief           Buffer structure
 */
typedef struct {
    uint8_t* buff;                              /*!< Pointer to buffer data.
                                                    Buffer is considered initialized when `buff != NULL` */
    size_t size;                                /*!< Size of buffer data. Size of actual buffer is `1` byte less than written here */
    size_t r;                                   /*!< Next read pointer. Buffer is considered empty when `r == w` and full when `w == r - 1` */
    size_t w;                                   /*!< Next write pointer. Buffer is considered empty when `r == w` and full when `w == r - 1` */
} ringbuff_t;

uint8_t     ringbuff_init(ringbuff_t* buff, void* buffdata, size_t size);
void        ringbuff_reset(ringbuff_t* buff);

size_t      ringbuff_write(ringbuff_t* buff, const void* data, size_t count);
size_t      ringbuff_read(ringbuff_t* buff, void* data, size_t count);
size_t      ringbuff_peek(ringbuff_t* buff, size_t skip_count, void* data, size_t count);

size_t      ringbuff_get_free(ringbuff_t* buff);
size_t      ringbuff_get_full(ringbuff_t* buff);

void *      ringbuff_get_linear_block_address(ringbuff_t* buff);
size_t      ringbuff_get_linear_block_length(ringbuff_t* buff);
size_t      ringbuff_skip(ringbuff_t* buff, size_t len);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* RINGBUFF_HDR_H */
