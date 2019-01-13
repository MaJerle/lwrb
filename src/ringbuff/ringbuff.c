/** 
 * \file            ringbuff.c
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
#include "ringbuff/ringbuff.h"

/**
 * \brief           Initialize buffer
 * \param[in]       buff: Pointer to buffer structure
 * \param[in]       size: Size of buffer. This parameter must match length of memory used on memory param
 * \return          `1` on success, `0` otherwise
 */
uint8_t
ringbuff_init(ringbuff_t* buff, void* buffdata, size_t size) {
    if (buff == NULL || size == 0 || buffdata == NULL) {/* Check buffer structure */
        return 0;
    }
    memset(buff, 0, sizeof(*buff));             /* Set buffer values to all zeros */

    buff->size = size;                          /* Set default values */
    buff->buff = buffdata;                      /* Save buffer working array */
    return 1;                                   /* Initialized OK */
}

/**
 * \brief           Write data to buffer
 * \param[in]       buff: Pointer to buffer structure
 * \param[in]       data: Pointer to data to copy memory from
 * \param[in]       count: Number of bytes we want to write
 * \return          Number of bytes actually written to buffer
 */
size_t
ringbuff_write(ringbuff_t* buff, const void* data, size_t count) {
    size_t tocopy, free;
    const uint8_t* d = data;

    if (buff == NULL || buff->buff == NULL || count == 0) {
        return 0;
    }

    if (buff->in >= buff->size) {               /* Check input pointer */
        buff->in = 0;
    }
    free = ringbuff_get_free(buff);             /* Get free memory */
    if (free < count) {                         /* Check available memory */
        if (free == 0) {                        /* If no memory, stop execution */
            return 0;
        }
        count = free;                           /* Set values for write */
    }

    /* We have calculated memory for write */
    tocopy = buff->size - buff->in;             /* Calculate number of elements we can put at the end of buffer */
    if (tocopy > count) {                       /* Check for copy count */
        tocopy = count;
    }
    memcpy(&buff->buff[buff->in], d, tocopy);   /* Copy content to buffer */
    buff->in += tocopy; 
    count -= tocopy;
    if (count > 0) {                            /* Check if anything to write */
        memcpy(buff->buff, (void *)&d[tocopy], count);  /* Copy content */
        buff->in = count;                       /* Set input pointer */
    }
    if (buff->in >= buff->size) {               /* Check input overflow */
        buff->in = 0;
    }
    return tocopy + count;                      /* Return number of elements stored in memory */
}

/**
 * \brief           Read data from buffer
 * \param[in]       buff: Pointer to buffer structure
 * \param[out]      data: Pointer to data to copy memory to
 * \param[in]       count: Number of bytes we want to read
 * \return          Number of bytes actually read and saved to data variable
 */
size_t
ringbuff_read(ringbuff_t* buff, void* data, size_t count) {
    size_t tocopy, full;
    uint8_t *d = data;

    if (buff == NULL || buff->buff == NULL || count == 0) {
        return 0;
    }

    if (buff->out >= buff->size) {              /* Check output pointer */
        buff->out = 0;
    }
    full = ringbuff_get_full(buff);             /* Get free memory */
    if (full < count) {                         /* Check available memory */
        if (full == 0) {                        /* If no memory, stop execution */
            return 0;
        }
        count = full;                           /* Set values for write */
    }

    tocopy = buff->size - buff->out;            /* Calculate number of elements we can read from end of buffer */
    if (tocopy > count) {                       /* Check for copy count */
        tocopy = count;
    }
    memcpy(d, &buff->buff[buff->out], tocopy);  /* Copy content from buffer */
    buff->out += tocopy;
    count -= tocopy;
    if (count > 0) {                            /* Check if anything to read */
        memcpy(&d[tocopy], buff->buff, count);  /* Copy content */
        buff->out = count;                      /* Set input pointer */
    }
    if (buff->out >= buff->size) {              /* Check output overflow */
        buff->out = 0;
    }
    return tocopy + count;                      /* Return number of elements stored in memory */
}

/**
 * \brief           Read from buffer but do not change read and write pointers
 * \param[in]       buff: Pointer to buffer structure
 * \param[in]       skip_count: Number of bytes to skip before reading peek data
 * \param[out]      data: Pointer to data to save read memory
 * \param[in]       count: Number of bytes to peek
 * \return          Number of bytes written to data array
 */
size_t
ringbuff_peek(ringbuff_t* buff, size_t skip_count, void* data, size_t count) {
    size_t full, tocopy, out;
    uint8_t *d = data;

    if (buff == NULL || buff->buff == NULL || count == 0) {
        return 0;
    }

    out = buff->out;
    if (buff->out >= buff->size) {              /* Check output pointer */
        buff->out = 0;
    }
    full = ringbuff_get_full(buff);             /* Get free memory */
    if (skip_count >= full) {                   /* We cannot skip for more than we have in buffer */
        return 0;
    }
    out += skip_count;                          /* Skip buffer data */
    full -= skip_count;                         /* Effective full is less than before */
    if (out >= buff->size) {                    /* Check overflow */
        out -= buff->size;                      /* Go to beginning */
    }
    if (full < count) {                         /* Check available memory */
        if (full == 0) {                        /* If no memory, stop execution */
            return 0;
        }
        count = full;                           /* Set values for write */
    }

    tocopy = buff->size - out;                  /* Calculate number of elements we can read from end of buffer */
    if (tocopy > count) {                       /* Check for copy count */
        tocopy = count;
    }
    memcpy(d, &buff->buff[out], tocopy);        /* Copy content from buffer */
    count -= tocopy;
    if (count > 0) {                            /* Check if anything to read */
        memcpy(&d[tocopy], buff->buff, count);  /* Copy content */
    }
    return tocopy + count;                      /* Return number of elements stored in memory */
}

/**
 * \brief           Get length of free space
 * \param[in]       buff: Pointer to buffer structure
 * \return          Number of free bytes in memory
 */
size_t
ringbuff_get_free(ringbuff_t* buff) {
    size_t size, in, out;

    if (buff == NULL || buff->buff == NULL) {
        return 0;
    }

    in = buff->in;
    out = buff->out;
    if (in == out) {                            /* Check if the same */
        size = buff->size;
    } else if (out > in) {                      /* Check normal mode */
        size = out - in;
    } else {                                    /* Check if overflow mode */
        size = buff->size - (in - out);
    }
    return size - 1;                            /* Return free memory */
}

/**
 * \brief           Get length of buffer currently being used
 * \param[in]       buff: Pointer to buffer structure
 * \return          Number of bytes ready to be read
 */
size_t
ringbuff_get_full(ringbuff_t* buff) {
    size_t in, out, size;

    if (buff == NULL || buff->buff == NULL) {
        return 0;
    }
    
    in = buff->in;
    out = buff->out;
    if (in == out) {                            /* Pointer are same? */
        size = 0;
    } else if (in > out) {                      /* buff is not in overflow mode */
        size = in - out;
    } else {                                    /* buff is in overflow mode */
        size = buff->size - (out - in);
    }
    return size;                                /* Return number of elements in buffer */
}

/**
 * \brief           Resets and clears buffer
 * \param[in]       buff: Pointer to buffer structure
 */
void
ringbuff_reset(ringbuff_t* buff) {
    if (buff == NULL) {                         /* Check buffer structure */
        return;
    }
    buff->in = 0;                               /* Reset values */
    buff->out = 0;
}

/**
 * \brief           Get linear address for buffer for fast read
 * \param[in]       buff: Pointer to buffer
 * \return          Pointer to start of linear address
 */
void *
ringbuff_get_linear_block_address(ringbuff_t* buff) {
    if (buff == NULL || buff->buff == NULL) {
        return NULL
    }
    return &buff->buff[buff->out];              /* Return read address */
}

/**
 * \brief           Get length of linear block address before it overflows
 * \param[in]       buff: Pointer to buffer
 * \return          Length of linear address
 */
size_t
ringbuff_get_linear_block_length(ringbuff_t* buff) {
    size_t len;
    
    if (buff->in > buff->out) {
        len = buff->in - buff->out;
    } else if (buff->out > buff->in) {
        len = buff->size - buff->out;
    } else {
        len = 0;
    }
    return len;
}

/**
 * \brief           Skip (ignore) buffer data.
 * \note            Useful at the end of streaming transfer such as DMA
 * \param[in]       buff: Pointer to buffer structure
 * \param[in]       len: Length of bytes we want to skip
 * \return          Number of bytes skipped
 */
size_t
ringbuff_skip(ringbuff_t* buff, size_t len) {
    size_t full;

    full = ringbuff_get_full(buff);             /* Get buffer used length */
    if (len > full) {
        len = full;
    }
    buff->out += len;                           /* Advance buffer */
    if (buff->out >= buff->size) {              /* Subtract possible overflow */
        buff->out -= buff->size;                /* Do subtract */
    }
    return len;
}
