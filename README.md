# Lightweight ring buffer manager

Library provides generic FIFO ring buffer implementation.

<h3>Read first: <a href="http://docs.majerle.eu/projects/lwrb/">Documentation</a></h3>

## Features

* Written in C (C11), compatible with ``size_t`` for size data types
* Platform independent default code - with restrictions for smaller CPU architectures (`< sizeof(size_t)`)
* FIFO (First In First Out) buffer implementation
* No dynamic memory allocation, data is static array
* Uses optimized memory copy instead of loops to read/write data from/to memory
* Thread safe when used as pipe with single write and single read entries - when CPU read/write operation for `size_t` are single instruction (ARM Cortex-M for instance)
* Interrupt safe when used as pipe with single write and single read entries - when CPU read/write operation for `size_t` are single instruction (ARM Cortex-M for instance)
* For CPU systems with smaller architecture than `sizeof(size_t)` (AVR for instance), atomic protection is required for read-write operation of buffer writes
* Suitable for DMA transfers from and to memory with zero-copy overhead between buffer and application memory
* Supports data peek, skip for read and advance for write
* Implements support for event notifications
* User friendly MIT license

## Contribute

Fresh contributions are always welcome. Simple instructions to proceed:

1. Fork Github repository
2. Follow [C style & coding rules](https://github.com/MaJerle/c-code-style) already used in the project
3. Create a pull request to develop branch with new features or bug fixes

Alternatively you may:

1. Report a bug
2. Ask for a feature request
