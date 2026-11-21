# Lightweight ring buffer manager

LwRB is a generic *FIFO* (First In, First Out) buffer library optimized for embedded systems.

[Open documentation](https://docs.majerle.eu/projects/lwrb/)

## Features

* Written in C (C11), compatible with `stdint.h` data types
* Platform independent, no architecture specific code
* FIFO (First In, First Out) buffer implementation
* No dynamic memory allocation, data is static array
* Uses optimized memory copy instead of loops to read/write data from/to memory
* Thread and interrupt safe when used as pipe with single write and single read entries
* Uses C11 atomic operations by default to keep reads and writes race-free, even on architectures whose native word size is smaller than `size_t` (AVR for instance); can be disabled with `LWRB_DISABLE_ATOMIC`
* Suitable for DMA transfers from and to memory with zero-copy overhead between buffer and application memory
* Supports data peek, skip for read and advance for write
* Byte-sequence search within buffered data
* Optional all-or-nothing read and write operations to avoid partial transfers
* Optional per-buffer custom user argument storage
* Implements support for event notifications
* User friendly MIT license

## Contribute

Fresh contributions are always welcome. Simple instructions to proceed:

1. Fork Github repository
2. Follow [C style & coding rules](https://github.com/MaJerle/c-code-style) and use `clang-format` to format the code
3. Create a pull request to `develop` branch with new features or bug fixes

Alternatively you may:

1. Report a bug
2. Ask for a feature request
