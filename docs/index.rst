Ringbuffer documentation!
=========================

Ring buffer is a generic *FIFO* (First In; First Out) buffer library optimized for embedded systems.

.. image:: static/images/logo.svg
	:align: center

.. class::center

:ref:`download_library` · `Github <https://github.com/MaJerle/ringbuff>`_

Features
^^^^^^^^

* Written in ANSI C99, compatible with ``size_t`` for size data types
* Platform independent, no architecture specific code
* FIFO (First In First Out) buffer implementation
* No dynamic memory allocation, data is static array
* Uses optimized memory copy instead of loops to read/write data from/to memory
* Thread safe when used as pipe with single write and single read entries
* Interrupt safe when used as pipe with single write and single read entries
* Suitable for DMA transfers from and to memory with zero-copy overhead between buffer and application memory
* Supports data peek, skip for read and advance for write
* User friendly MIT license

Requirements
^^^^^^^^^^^^

* C compiler
* Less than ``1kB`` of memory

Contribute
^^^^^^^^^^

We always welcome new contributors. To be as efficient as possible, we recommend:

#. Fork Github repository
#. Respect `C style & coding rules <https://github.com/MaJerle/c-code-style>`_ used by the library
#. Make a pull request to ``develop`` branch with new features or bug fixes

Alternatively you may:

#. Report a bug
#. Ask for a feature request

Example code
^^^^^^^^^^^^

Minimalistic example code to read and write data to buffer

.. code-block:: c

    /* Buffer variables */
    ringbuff_t buff;                            /* Declare ring buffer structure */
    uint8_t buff_data[8];                       /* Declare raw buffer data array */

    /* Application variables */
    uint8_t data[2];                            /* Application working data */
    size_t len;

    /* Application code ... */
    ringbuff_init(&buff, buff_data, sizeof(buff_data)); /* Initialize buffer */

    /* Write 4 bytes of data */
    ringbuff_write(&buff, "0123", 4);

    /* Try to read buffer */
    /* len holds number of bytes read */
    /* Read until len == 0, when buffer is empty */
    while ((len = ringbuff_read(&buff, data, sizeof(data))) > 0) {
        printf("Successfully read %d bytes\r\n", (int)len);
    }

License
^^^^^^^

.. literalinclude:: license.txt

Table of contents
^^^^^^^^^^^^^^^^^

.. toctree::
    :maxdepth: 2

    get-started/index
    user-manual/index
    tips-tricks/index
    api-reference/index
