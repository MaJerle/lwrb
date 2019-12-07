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