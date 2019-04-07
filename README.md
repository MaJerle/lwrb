# Generic ring buffer manager

Library provides generic FIFO ring buffer implementation.

## Features

- Written in ANSI C99, compatible with `size_t` for size data types
- Platform independent, no architecture specific code
- FIFO (First In First Out) buffer implementation
- No dynamic allocation
- Uses optimized memory copy instead of loops to read/write data from/to memory
- Thread safe when used as pipe with single write and single read entries
- Interrupt safe when used as pipe with single write and single read entries
- Suitable for DMA transfers from and to memory with zero-copy between buffer and application memory
- Supports data peek, skip for read and advance for write

# Examples and resources

For examples, please check second repository, available at https://github.com/MaJerle/ringbuff_res

# Documentation

Full API documentation with description and examples is available and is regulary updated with the source changes

http://majerle.eu/documentation/ringbuff/html/index.html

# Contribution

I invite you to give feature request or report a bug. Please use issues tracker.