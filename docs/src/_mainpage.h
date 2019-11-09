/**
 * \mainpage
 * \tableofcontents
 * 
 * Generic ring buffer implemented in C language. It is also known as FIFO (First-In; First-Out) buffer.
 *
 * \section         sect_features Features
 *
 *  - Written in ANSI C99, compatible with `size_t` for size data types
 *  - Platform independent, no architecture specific code
 *  - FIFO (First In First Out) buffer implementation
 *  - No dynamic memory allocation, data is static array
 *  - Uses optimized memory copy instead of loops to read/write data from/to memory
 *  - Thread safe when used as pipe with single write and single read entries
 *  - Interrupt safe when used as pipe with single write and single read entries
 *  - Suitable for DMA transfers from and to memory with zero-copy between buffer and application memory
 *  - Supports data peek, skip for read and advance for write
 *  - User friendly MIT license
 *
 * \section         sect_resources Download & Resources
 *
 *  - <a class="download_url" href="https://github.com/MaJerle/ringbuff/releases">Download library at Github releases</a>
 *  - <a href="https://github.com/MaJerle/ringbuff">Resources and examples repository</a>
 *  - Read \ref page_appnote before you start development
 *  - <a href="https://github.com/MaJerle/ringbuff">Official development repository on Github</a>
 *
 * \section         sect_contribute How to contribute
 * 
 *  - Official development repository is hosted on Github
 *  - <a href="https://github.com/MaJerle/c_code_style">Respect C style and coding rules</a>
 *
 * \section         sect_license License
 *
 * \verbatim        
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
 * OTHER DEALINGS IN THE SOFTWARE. \endverbatim
 *
 */