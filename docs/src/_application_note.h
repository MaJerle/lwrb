/**
 * \page            page_appnote Application note
 * \tableofcontents
 *
 * \section         sect_getting_started Getting started
 *
 * Library development is fully hosted on Github and there is no future plans to move to any other platform.
 *
 * There are `2` repositories
 *
 *  - <a href="https://github.com/MaJerle/ringbuff"><b>ringbuff</b></a>: Source code of library itself.
 *      - Repository is required when developing final project
 *
 *  - <a href="https://github.com/MaJerle/ringbuff_res"><b>ringbuff_res</b></a>: Resources, development code,
 *      documentation sources, examples, code snippets, etc.
 *      - This repository uses `ringbuff` repository as `submodule`
 *      - Repository is used to evaluate library using prepared examples
 *
 * \subsection      sect_clone_res Clone resources with examples
 *
 * Easiest way to test the library is to clone resources repository.
 *
 * \par             First-time clone
 *
 *  - Download and install `git` if not already
 *  - Open console and navigate to path in the system to clone repository to. Use command `cd your_path`
 *  - Run `git clone --recurse-submodules https://github.com/MaJerle/ringbuff_res` command to clone repository including submodules
 *  - Navigate to `examples` directory and run favourite example
 *
 * \par             Already cloned, update to latest version
 *
 *  - Open console and navigate to path in the system where your resources repository is. Use command `cd your_path`
 *  - Run `git pull origin master --recurse-submodules` command to pull latest changes and to fetch latest changes from submodules
 *  - Run `git submodule foreach git pull origin master` to update & merge all submodules
 *
 * \subsection      sect_clone_lib Clone library only
 *
 * If you are already familiar with library and you wish to include it in existing project, easiest way to do so is to clone library repository only.
 
 * \par             First-time clone
 *
 *  - Download and install `git` if not already
 *  - Open console and navigate to path in the system to clone repository to. Use command `cd your_path`
 *  - Run `git clone --recurse-submodules https://github.com/MaJerle/ringbuff` command to clone repository
 *
 * \par             Already cloned, update to latest version
 *
 *  - Open console and navigate to path in the system where your repository is. Use command `cd your_path`
 *  - Run `git pull origin master --recurse-submodules` to update & merge latest repository changes
 *
 * \section         sect_how_it_works How ring buffer works

 * Reference first:
 *  - `R` represents `R`ead pointer. Read on read/write operations. Modified on read operation only
 *  - `W` represents `W`rite pointer. Read on read/write operations. Modified on write operation only
 *  - `S` represents `S`ize of buffer. Used on all operations, never modified (atomic value)
 *      - Valid number of `W` and `R` pointers are between `0` and `S - 1`
 *
 *  - Buffer size is `S = 8`, thus valid number range for `W` and `R` pointers is `0 - 7`.
 *      - `R` and `W` numbers overflow at `S`, thus valid range is always `0, 1, 2, 3, ..., S - 2, S - 1, 0, 1, 2, 3, ..., S - 2, S - 1, 0, ...`
 *      - Example `S = 4`: `0, 1, 2, 3, 0, 1, 2, 3, 0, 1, ...`
 *  - Maximal number of bytes buffer can hold is always `S - 1`, thus example buffer can hold up to `7` bytes
 *  - `R` and `W` pointers always point to \e next read/write operation.
 *  - When `W == R`, buffer is considered empty. When `W == R - 1`, buffer is considered full.
 *      - `W == R - 1` is valid only if `W` and `R` overflow at buffer size `S`.
 *      - Always add `S` to calculated number and then use modulus `S` to get final value
 *          - Example 1, addition of `2` numbers: `3 + 2 = (3 + 2 + S) % S = (3 + 2 + 4) % 4 = (5 + 4) % 4 = 1`.
 *          - Example 2, subtraction of `2` numbers: `1 - 2 = (1 - 2 + S) % S = (1 - 2 + 4) % 4 = (-1 + 4) % 4 = 3`.
 *
 * \image html buff_cases.svg Different buffer corner cases
 * 
 * Different image cases:
 *
 *  - Case ***A***: Buffer is empty as `W == R = 0 == 0`
 *  - Case ***B***: Buffer holds `W - R = 4 - 0 = 4` bytes as `W > R`
 *  - Case ***C***: Buffer is full as `W == R - 1` or `7 == 0 - 1` or `7 = (0 - 1 + S) % S = (0 - 1 + 8) % 8 = (-1 + 8) % 8 = 7`
 *      - `R` and `W` can hold `S` different values, from `0` to `S - 1`, that is modulus of `S`
 *      - Buffer holds `W - R = 7 - 0 = 7` bytes as `W > R`
 *  * Case ***D***: Buffer holds `S - (R - W) = 8 - (5 - 3) = 6` bytes as `R > W`
 *  * Case ***E***: Buffer is full as `W == R - 1` (`4 = 5 - 1`) and holds `S - (R - W) = 8 - (5 - 4) ) = 7` bytes
 *
 * \section         sect_linear_blocks Linear memory blocks
 * \subsection      sect_linear_blocks_read Read block and data skipping
 *
 * Instead of using read function and copying data from working buffer to application buffer,
 * it is possible to use original working buffer and process/read data directly from memory with zero-copy.
 * This is handy with Direct Memory Access (DMA) feature on microcontroller or any other platform,
 * as it allows data transfer with zero-copy between memory and hardware IP.
 * 
 * \image html buff_lin_read_skip.svg Linear memory read operation and data skipping
 
 *  - Case ***A***: Initial state, buffer is full and holds `7` bytes
 *  - Case ***B***: State after skipping `R` pointer for `3` bytes. Buffer now holds `4` remaining bytes
 *  - Case ***C***: Buffer is empty, no more memory available for read operation
 *
 * Example code with comments
 *
 * \include         buff_block_read.c
 *
 * Application may use loop when trying to send out all bytes
 *
 * \include         buff_block_read_loop.c
 *
 * \subsection      sect_linear_blocks_write Write block and data advancing
 * 
 * Similar to read block, it is possible to get linear block and its length to directly copy data to buffer.
 * This is handy when hardware writes to memory on a microcontroller (DMA) without interrupting CPU for each received byte.
 * Once transfer is completed, application has to manually increase write pointer to consider buffer is holding data.
 *
 * \image html buff_lin_write_advance.svg Linear memory write operation and pointer advancing
 *
 * Example code with comments
 *
 *  - Case ***A***: Initial state, buffer is empty
 *  - Case ***B***: State after writing advancing `W` pointer for `4` bytes. Buffer now holds `4` new bytes
 *  - Case ***C***: Buffer is full, no more free memory available for write operation
 *
 * \include         buff_block_write.c
 *
 */