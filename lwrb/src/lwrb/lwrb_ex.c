/**
 * \file            lwrb_ex.c
 * \brief           Lightweight ring buffer extended functions
 */


#include "lwrb.h"


#define BUF_IS_VALID(b) ((b) != NULL && (b)->buff != NULL && (b)->size > 0)
#define BUF_MIN(x, y)   ((x) < (y) ? (x) : (y))


/**
 * \brief           Similar to write(), writes data to buffer, will overwrite existing values.
 * \param[in]       buff: Buffer handle
 * \param[in]       data: Data to write to ring buffer
 * \param[in]       btw: Bytes To Write, length
 * \return          Number of bytes written to buffer, will always return btw
 * \note            Functionality is primary two parts, always writes some linear region, then
 * writes the wrap region if there is more data to write. The r indicator is advanced if w overtakes
 * it. This operation is a read op as well as a write op. For thread-safety mutexes may be desired,
 * see documentation.
 */
size_t lwrb_overwrite(lwrb_t* buff, const void* data, size_t btw)
{
    size_t w, r, linear, wrap;
    size_t size   = buff->size;
    size_t offset = 0;

    if (!BUF_IS_VALID(buff) || data == NULL || btw == 0)
    {
        return 0;
    }

    w = atomic_load_explicit(&buff->w, memory_order_relaxed);
    r = atomic_load_explicit(&buff->r, memory_order_relaxed);



    /* Step 1: Special handling if asked to write more bytes than size. */
    if (btw >= size)
    {
        offset = (btw - size) + 1;
        btw    = size - 1;
    }



    /* Step 2: Always write the linear region. There will be at least one byte written here. */

    /* Handle special case of linear being off by one when zero */   // TODO: Test this
    if (w == 0)
        linear = size - 1;
    else
        linear = size - w;

    /* We might not need the whole linear length available, depends on amount to write*/
    if (btw < linear)
        linear = btw;

    /* If linear write will exceed r it must be updated, this may also happen in wrap region later
     */
    if ((w < r) && (w + linear) > r)
    {
        r = w + btw + 1;
        r %= size;
    }

    /* Copy the linear region data, and prepare for the wrap region */
    memcpy(buff->buff + w, data + offset, linear);
    w += linear;
    w %= size;
    wrap = btw - linear;



    /* Step 3: Wrap region are the bytes that rolled back to the start of the storage array */
    if (wrap)
    {

        /* If r will be overrun by new write data, skip r ahead */
        if ((w <= r) && (w + wrap) >= r)
        {
            r = w + wrap + 1;
            // r %= size;   // TODO: Can this happen now?
        }

        /* Copy the wrap region data */
        memcpy(buff->buff, data + offset + linear, wrap);
        w += wrap;
        w %= size;
    }



    /* Step 4: The ring buffer is complete, this is a write op as well as possibly a read op */
    atomic_store_explicit(&buff->w, w, memory_order_release);
    atomic_store_explicit(&buff->r, r, memory_order_release);

    // TODO: It might be desirable to store r_old and only update if r has changed

    // TODO: Missing event handler

    return (btw);
}



/**
 * \brief           Copy one ring buffer to another, upto the amount of data in the source, or amount 
 * of data free in the destination.
 * \param[in]       dest: Buffer handle that the copied data will be written to
 * \param[in]       src:  Buffer handle that the copied data will come from
 * \return          Number of bytes written to buffer
 * \note            This operation is a read op to the source, on success it will update the r index. 
 * As well as a write op to the destination, and may update the w index. For thread-safety mutexes may 
 * be desired, see documentation.
 */
size_t lwrb_copy(lwrb_t* dest, lwrb_t* src)
{
    size_t dest_w, src_r;
    size_t len_to_copy = BUF_MIN(lwrb_get_full(src), lwrb_get_free(dest));
    size_t i;

    if (!BUF_IS_VALID(dest) || !BUF_IS_VALID(src))
    {
        return 0;
    }

    dest_w = atomic_load_explicit(&dest->w, memory_order_relaxed);
    src_r  = atomic_load_explicit(&src->r, memory_order_relaxed);

    /* For the lesser amount in source or free in destination, copy byte by byte */
    for (i = 0; i < len_to_copy; i++)
    {

        dest->buff[dest_w++] = src->buff[src_r++];
        
        /* Handle roll-over / wrap for both source and destination indexes */
        if (dest_w >= dest->size)
            dest_w = 0;
        if (src_r >= src->size)
            src_r = 0;
   }

    atomic_store_explicit(&dest->w, dest_w, memory_order_release);
    atomic_store_explicit(&src->r, src_r, memory_order_release);

    return (len_to_copy);
}
