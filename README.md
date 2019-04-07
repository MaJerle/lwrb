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

## How it works

![Ring buffer use cases](https://raw.githubusercontent.com/MaJerle/ringbuff_res/master/docs/src/buff_cases.svg?sanitize=true)

Image shows different corner cases. As for the reference, `R` stands for ***R**ead pointer* and `W` stands for ***W**rite pointer*. Pointer are being used when *read* or *write* operations are used respectively. Numbers `0 - 7` represent `S = 8` byte long data array. **S** represents buffer size.
`R` and `W` pointers always point to *next read/write* operation. When `W == R`, buffer is considered empty. When `W == R - 1`, buffer is considered empty. Please note that `W == R - 1` is valid only if `W` and `R` overflow at buffer size `S`.

- ***a***: Buffer is empty as `W == R = 0 == 0`
- ***b***: Buffer holds `W - R = 4 - 0 = 4` bytes as `W > R`
- ***c***: Buffer is full as `W == R - 1 or 7 == 0 - 1`
    - `R` and `W` can hold `S` different values, from `0` to `S - 1`, that is modulus of `S`
    - Buffer holds `W - R = 7 - 0 = 7` bytes as `W > R`
* ***d***: Buffer holds `S - (R - W) = 8 - (5 - 3) = 6` bytes as `R > W`
* ***e***: Buffer is full as `W == R - 1` (`4 = 5 - 1`) and holds `S - (R - W) = 8 - (5 - 4) ) = 7` bytes

> Effective number of bytes to read/write from/to buffer is always `1` less than buffer size. In case of example, maximal number of bytes buffer can hold is `8 - 1 = 7`.

> Consider `W` and `R` pointers to overflow at buffer size `S`. If `S = 4`, `W/R` values are: `0, 1, 2, 3, 0, 1, 2, 3, 0, ...`. Numbers *overflow* at modulus of `S`.
    - Example 1, addition: `3 + 2 = (3 + 2 + S) % S = (5 + 4) % 4 = 1`.
    - Example 2, subtraction: `1 - 2 = (1 - 2 + S) % S = (1 - 2 + 4) % 4 = 3`.
> If `S = 5`, `W/R` values can be: `0, 1, 2, 3, 4, 0, 1, 2, 3, 4, ...`.
    - Example: `1 - 2 (1 - 2 + S) % S = (1 - 2 + 5) % 5 = 4`

### Linear read block and data skipping

Instead of using read function and copying data from working buffer to application buffer, it is possible to use original working buffer and process/read data directly from memory.
This feature is very useful when implementing Direct Memory Access (DMA) feature on microcontroller or any other platform as it allows data transfer with zero-copy between memories.

![Linear read and data skipping](https://raw.githubusercontent.com/MaJerle/ringbuff_res/master/docs/src/buff_lin_read_skip.svg?sanitize=true)

Following code is explaining image above.

```c
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
```

Code may be rewritten to something like:

```c
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

```

Refer to DMA example below for more real-life use case.

### Linear write block and write pointer advancing

Similar to read block, it is possible to get linear block and its length to directly copy data to buffer.
This feature is very useful when hardware writes to memory on a microcontroller (DMA) without interrupting CPU for each received byte.
Once transfer is completed, application must manually increase write pointer to consider buffer is containing data.

![Linear write and pointer advancing](https://raw.githubusercontent.com/MaJerle/ringbuff_res/master/docs/src/buff_lin_write_advance.svg?sanitize=true)

Following code is explaining image above. Buffer write will continue where buffer read ended in previous image.

```c
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
```

## Examples

Few examples to show how to use the library

### Basic example

Example shows how to use basic buffer implementation for writing and reading data from buffer

```c
/* Buffer */
ringbuff_t buff;
uint8_t buff_data[8];

/* Working data */
uint8_t data[2];
size_t len;

/* Initialize buffer */
ringbuff_init(&buff, buff_data, sizeof(buff_data));

/* Write 4 bytes of data */
ringbuff_write(&buff, "0123", 4);

/* Try to read buffer */
/* len holds number of bytes read */
/* Read until len == 0, when buffer is empty */
while ((len = ringbuff_read(&buff, data, sizeof(data))) > 0) {
    printf("Successfully read %d bytes\r\n", (int)len);
}
```

### DMA usage on microcontroller

Simple example showing how to use buffer in conjunction with DMA hardware
to efficiently transfer data from buffer to DMA with *zero copy*.

```c
/* Buffer */
ringbuff_t buff;
uint8_t buff_data[8];

/* Working data length */
size_t len;

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
}

/* Called on DMA transfer finish */
void
DMA_Interrupt_handler(void) {
    /* Transfer finished */
    if (len) {
        /* Now skip the data as they were successfully transferred over DMA */
        ringbuff_skip(&buff, len);

        /* Reset length = DMA is not active */
        len = 0;

        /* Try to send more */
        send_data();
    }
}

```
