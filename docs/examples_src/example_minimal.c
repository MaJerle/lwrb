#include "ringbuff/ringbuff.h"

/* Buffer variables */
ringbuff_t buff;                            /* Declare ring buffer structure */
uint8_t buff_data[8];                       /* Declare raw buffer data array */

/* Application variables
uint8_t data[2];                            /* Application working data */

/* Application code ... */
ringbuff_init(&buff, buff_data, sizeof(buff_data)); /* Initialize buffer */

/* Write 4 bytes of data */
ringbuff_write(&buff, "0123", 4);

/* Print number of bytes in buffer */
printf("Bytes in buffer: %d\r\n", (int)ringbuff_get_full(&buff));

/* Will print "4" */