#include "lwrb/lwrb.h"

/* Declare rb instance & raw data */
lwrb_t buff;
uint8_t buff_data[8];

/* Application code ... */
lwrb_init(&buff, buff_data, sizeof(buff_data)); /* Initialize buffer */

/* Write 4 bytes of data */
lwrb_write(&buff, "0123", 4);

/* Print number of bytes in buffer */
printf("Bytes in buffer: %d\r\n", (int)lwrb_get_full(&buff));
/* Will print "4" */

/* Now let's read */
uint8_t data[8]; /* Application working data */
size_t len;

/* Read from buffer, will return number of bytes read */
len = lwrb_read(&buff, data, sizeof(data));
printf("Number of bytes read: %d\r\n", (int)len);

/* Data is now available in the "data" variable */
