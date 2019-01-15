# Generic ring buffer manager

Library provides generic FIFO ring buffer implementation.

## Features

- Written in ANSI C99, compatible with `size_t` type for size data types
- Platform independent
- FIFO (First In First Out) buffer implementation
- No dynamic allocation
- Fast read/write operations using memory copy functions
- Thread safe when used as pipe with single write and single read
- Interrupt safe when used as pipe with single write and single read
- Suitable for DMA transfers
- Supports data peek and data skip

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

/* Try to fully read buffer */
/* len holds number of bytes read */
while ((len = ringbuff_read(&buff, data, sizeof(data)))) {
	printf("Read successful of %d bytes\r\n", (int)len);
}
```

### DMA usage on microcontroller

Simple example showing how to use buffer in conjunction with DMA hardware
to efficiently transfer data from buffer to DMA with *zero copy*.

```c
/* Buffer */
ringbuff_t buff;
uint8_t buff_data[8];

/* Working data */
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
	len = ringbuff_get_linear_block_length(&buff);
	if (len) {
		/* Get pointer to read memory */
		uint8_t* data = ringbuff_get_linear_block_address(&buff);

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