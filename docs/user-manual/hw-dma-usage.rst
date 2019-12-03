DMA on embedded systems
=======================

One of the key features of ringbuffer library is that it can be seamlessly integrated with DMA controllers on embedded systems.

.. note::
    
    DMA stands for *Direct Memory Access* controller and is usually used to off-load CPU.
    More about DMA is available on `Wikipedia <https://en.wikipedia.org/wiki/Direct_memory_access>`_.

DMA controllers normally use source and destination memory addresses to transfer data in-between.
This features, together with ringbuffer, allows seamless integration and zero-copy of application data at interrupts after DMA transfer has been completed.
Some manual work is necessary to be handled, but this is very minor in comparison of writing byte-by-byte to buffer at (for example) each received character.

Below are ``2`` common use cases:

* DMA transfers data from ringbuffer memory to (usually) some hardware IP
* DMA transfers data from hardware IP to memory

Zero-copy data from memory
^^^^^^^^^^^^^^^^^^^^^^^^^^

This describes how to pass ringbuffer output memory address as pointer to DMA (or any other processing function).
After all the data are successfully processed, application can skip processed data and free ringbuff for new data being written to it.

.. figure:: ../static/images/buff_lin_read_skip.svg
    :align: center
    :alt: Data transfer from memory to hardware IP

* Case **A**: Initial state, buffer is full and holds ``7`` bytes
* Case **B**: State after skipping ``R`` pointer for ``3`` bytes. Buffer now holds ``4`` remaining bytes
* Case **C**: Buffer is empty, no more memory available for read operation

Code example:

.. code-block:: c

    /* Declare buffer variables */
    ringbuff_t buff;
    uint8_t buff_data[8];

    size_t len;
    uint8_t* data;

    /* Initialize buffer, use buff_data as data array */
    ringbuff_init(&buff, buff_data, sizeof(buff_data));

    /* Use write, read operations, process data */
    /* ... */

    /* IMAGE PART A */

    /* At this stage, we have buffer as on image above */
    /* R = 5, W = 4, buffer is considered full */

    /* Get length of linear memory at read pointer */
    /* Function returns 3 as we can read 3 bytes from buffer in sequence */
    /* When function returns 0, there is no memory available in the buffer for read anymore */
    if ((len = ringbuff_get_linear_block_read_length(&buff)) > 0) {
        /* Get pointer to first element in linear block at read address */
        /* Function returns &buff_data[5] */
        data = ringbuff_get_linear_block_read_address(&buff);

        /* Send data via DMA and wait to finish (for sake of example) */
        send_data(data, len);

        /* Now skip sent bytes from buffer = move read pointer */
        ringbuff_skip(&buff, len);

        /* Now R points to top of buffer, R = 0 */
        /* At this point, we are at image part B */
    }

    /* IMAGE PART B */

    /* Get length of linear memory at read pointer */
    /* Function returns 4 as we can read 4 bytes from buffer in sequence */
    /* When function returns 0, there is no memory available in the buffer for read anymore */
    if ((len = ringbuff_get_linear_block_read_length(&buff)) > 0) {
        /* Get pointer to first element in linear block at read address */
        /* Function returns &buff_data[0] */
        data = ringbuff_get_linear_block_read_address(&buff);

        /* Send data via DMA and wait to finish (for sake of example) */
        send_data(data, len);

        /* Now skip sent bytes from buffer = move read pointer */
        /* Read pointer is moved for len bytes */
        ringbuff_skip(&buff, len);

        /* Now R points to 4, that is R == W and buffer is now empty */
        /* At this point, we are at image part C */
    }

    /* IMAGE PART C */

    /* Buffer is considered empty as R == W */


Part **A** on image clearly shows that not all data bytes are linked in single contiguous block of memory. 
To send all bytes from ringbuff, it might be necessary to repeat procedure multiple times

.. code-block:: c

    /* Initialization part skipped */

    /* Get length of linear memory at read pointer */
    /* When function returns 0, there is no memory available in the buffer for read anymore */
    while ((len = ringbuff_get_linear_block_read_length(&buff)) > 0) {
        /* Get pointer to first element in linear block at read address */
        data = ringbuff_get_linear_block_read_address(&buff);

        /* If max length needs to be considered */
        /* simply decrease it and use smaller len on skip function */
        if (len > max_len) {
            len = max_len;
        }

        /* Send data via DMA and wait to finish (for sake of example) */
        send_data(data, len);

        /* Now skip sent bytes from buffer = move read pointer */
        ringbuff_skip(&buff, len);
    }

Zero-copy data to memory
^^^^^^^^^^^^^^^^^^^^^^^^

Similar to reading data from buffer with zero-copy overhead, it is possible to write to ringbuff with zero-copy overhead too.
Only difference is that application now needs pointer to write memory address and length of maximal number of bytes to directly copy in buffer.
After processing is successful, buffer advance operation is necessary to manually increase write pointer and to increase number of bytes in buffer.

.. figure:: ../static/images/buff_lin_write_advance.svg
    :align: center
    :alt: Data transfer from memory to hardware IP

* Case **A**: Initial state, buffer is empty as ``R == W``
    
    * Based on ``W`` pointer position, application could write ``4`` bytes to contiguous block of memory
* Case **B**: State after advancing `W` pointer for `4` bytes. Buffer now holds `4` bytes and has ``3`` remaining available
* Case **C**: Buffer is full, no more free memory available for write operation

Code example:

.. code-block:: c

    /* Declare buffer variables */
    ringbuff_t buff;
    uint8_t buff_data[8];

    size_t len;
    uint8_t* data;

    /* Initialize buffer, use buff_data as data array */
    ringbuff_init(&buff, buff_data, sizeof(buff_data));

    /* Use write, read operations, process data */
    /* ... */

    /* IMAGE PART A */

    /* At this stage, we have buffer as on image above */
    /* R = 4, W = 4, buffer is considered empty */

    /* Get length of linear memory at write pointer */
    /* Function returns 4 as we can write 4 bytes to buffer in sequence */
    /* When function returns 0, there is no memory available in the buffer for write anymore */
    if ((len = ringbuff_get_linear_block_write_length(&buff)) > 0) {
        /* Get pointer to first element in linear block at write address */
        /* Function returns &buff_data[4] */
        data = ringbuff_get_linear_block_write_address(&buff);

        /* Receive data via DMA and wait to finish (for sake of example) */
        /* Any other hardware may directly write to data array */
        /* Data array has len bytes length */
        /* Or use memcpy(data, my_array, len); */
        receive_data(data, len);

        /* Now advance buffer for written bytes to buffer = move write pointer */
        /* Write pointer is moved for len bytes */
        ringbuff_advance(&buff, len);

        /* Now W points to top of buffer, W = 0 */
        /* At this point, we are at image part B */
    }

    /* IMAGE PART B */

    /* Get length of linear memory at write pointer */
    /* Function returns 3 as we can write 3 bytes to buffer in sequence */
    /* When function returns 0, there is no memory available in the buffer for write anymore */
    if ((len = ringbuff_get_linear_block_read_length(&buff)) > 0) {
        /* Get pointer to first element in linear block at write address */
        /* Function returns &buff_data[0] */
        data = ringbuff_get_linear_block_read_address(&buff);

        /* Receive data via DMA and wait to finish (for sake of example) */
        /* Any other hardware may directly write to data array */
        /* Data array has len bytes length */
        /* Or use memcpy(data, my_array, len); */
        receive_data(data, len);

        /* Now advance buffer for written bytes to buffer = move write pointer */
        /* Write pointer is moved for len bytes */
        ringbuff_advance(&buff, len);

        /* Now W points to 3, R points to 4, that is R == W + 1 and buffer is now full */
        /* At this point, we are at image part C */
    }

    /* IMAGE PART C */

    /* Buffer is considered full as R == W + 1 */

Example for DMA transfer from memory
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is an example showing pseudo code for implementing data transfer using DMA with zero-copy overhead.
For read operation purposes, application gets direct access to ringbuffer read pointer and length of contigouos memory.

It is assumed that after DMA transfer completes, interrupt is generated (embedded system) and buffer is skipped in the interrupt.

.. note::

    Buffer skip operation is used to mark sent data as processed and to free memory for new writes to buffer

.. code-block:: c

    /* Buffer */
    ringbuff_t buff;
    uint8_t buff_data[8];

    /* Working data length */
    size_t len;

    /* Send data function */
    void send_data(void);

    int
    main(void) {
        /* Initialize buffer */
        ringbuff_init(&buff, buff_data, sizeof(buff_data));

        /* Write 4 bytes of data */
        ringbuff_write(&buff, "0123", 4);

        /* Send data over DMA */
        send_data();

        while (1);
    }

    /* Send data over DMA */
    void
    send_data(void) {
        /* If len > 0, DMA transfer is on-going */
        if (len) {
            return;
        }

        /* Get maximal length of buffer to read data as linear memory */
        len = ringbuff_get_linear_block_read_length(&buff);
        if (len) {
            /* Get pointer to read memory */
            uint8_t* data = ringbuff_get_linear_block_read_address(&buff);

            /* Start DMA transfer */
            start_dma_transfer(data, len);
        }

        /* Function does not wait for transfer to finish */
    }

    /* Interrupt handler */
    /* Called on DMA transfer finish */
    void
    DMA_Interrupt_handler(void) {
        /* Transfer finished */
        if (len) {
            /* Now skip the data (move read pointer) as they were successfully transferred over DMA */
            ringbuff_skip(&buff, len);

            /* Reset length = DMA is not active */
            len = 0;

            /* Try to send more */
            send_data();
        }
    }

.. toctree::
    :maxdepth: 2