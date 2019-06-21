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

#include <string.h>
#include <stdint.h>

/**
 * \defgroup        RINGBUFF Ring buffer
 * \brief           Generic ring buffer manager
 * \{
 */

/* --- Buffer unique part starts --- */
/**
 * \brief           Buffer function/typedef prefix string
 * 
 * It is used to change function names in zero time to easily re-use same library between applications.
 * Use `#define BUF_PREF(x)    my_prefix_ ## x` to change all function names to (for example) `my_prefix_buff_init` 
 *
 * \note            Modification of this macro must be done in header and source file aswell
 */
#define BUF_PREF(x)                     ring ## x
/* --- Buffer unique part ends --- */

/**
 * \brief           Buffer structure
 */
typedef struct {
    uint8_t* buff;                              /*!< Pointer to buffer data.
                                                    Buffer is considered initialized when `buff != NULL` and `size` */
    size_t size;                                /*!< Size of buffer data. Size of actual buffer is `1` byte less than value holds */
    size_t r;                                   /*!< Next read pointer. Buffer is considered empty when `r == w` and full when `w == r - 1` */
    size_t w;                                   /*!< Next write pointer. Buffer is considered empty when `r == w` and full when `w == r - 1` */
} BUF_PREF(buff_t);

uint8_t     BUF_PREF(buff_init)(BUF_PREF(buff_t)* buff, void* buffdata, size_t size);
void        BUF_PREF(buff_free)(BUF_PREF(buff_t)* buff);
void        BUF_PREF(buff_reset)(BUF_PREF(buff_t)* buff);

/* Read/Write functions */
size_t      BUF_PREF(buff_write)(BUF_PREF(buff_t)* buff, const void* data, size_t btw);
size_t      BUF_PREF(buff_read)(BUF_PREF(buff_t)* buff, void* data, size_t btr);
size_t      BUF_PREF(buff_peek)(BUF_PREF(buff_t)* buff, size_t skip_count, void* data, size_t btp);

/* Buffer size information */
size_t      BUF_PREF(buff_get_free)(BUF_PREF(buff_t)* buff);
size_t      BUF_PREF(buff_get_full)(BUF_PREF(buff_t)* buff);

/* Read data block management */
void *      BUF_PREF(buff_get_linear_block_read_address)(BUF_PREF(buff_t)* buff);
size_t      BUF_PREF(buff_get_linear_block_read_length)(BUF_PREF(buff_t)* buff);
size_t      BUF_PREF(buff_skip)(BUF_PREF(buff_t)* buff, size_t len);

/* Write data block management */
void *      BUF_PREF(buff_get_linear_block_write_address)(BUF_PREF(buff_t)* buff);
size_t      BUF_PREF(buff_get_linear_block_write_length)(BUF_PREF(buff_t)* buff);
size_t      BUF_PREF(buff_advance)(BUF_PREF(buff_t)* buff, size_t len);

#undef BUF_PREF         /* Prefix not needed anymore */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif

#endif /* RINGBUFF_HDR_H */
