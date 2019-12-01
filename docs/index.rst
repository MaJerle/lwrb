Welcome to Ringbuffer's documentation!
======================================

.. image:: images/logo.svg
	:align: center

Features
^^^^^^^^

- Written in ANSI C99, compatible with size_t for size data types
- Platform independent, no architecture specific code
- FIFO (First In First Out) buffer implementation
- No dynamic memory allocation, data is static array
- Uses optimized memory copy instead of loops to read/write data from/to memory
- Thread safe when used as pipe with single write and single read entries
- Interrupt safe when used as pipe with single write and single read entries
- Suitable for DMA transfers from and to memory with zero-copy overhead between buffer and application memory
- Supports data peek, skip for read and advance for write
- User friendly MIT license

Requirements
^^^^^^^^^^^^

- C compiler

.. toctree::
   :maxdepth: 2

   get-started/index
   api/ringbuff