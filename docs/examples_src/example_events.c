/**
 * \brief           Buffer event function
 */
void
my_buff_evt_fn(ringbuff_t* buff, ringbuff_evt_type_t type, size_t len) {
    switch (type) {
        case RINGBUFF_EVT_RESET:
            printf("[EVT] Buffer reset event!\r\n");
            break;
        case RINGBUFF_EVT_READ:
            printf("[EVT] Buffer read event: %d byte(s)!\r\n", (int)len);
            break;
        case RINGBUFF_EVT_WRITE:
            printf("[EVT] Buffer write event: %d byte(s)!\r\n", (int)len);
            break;
        default: break;
    }
}

/* Later in the code... */
ringbuff_t buff;
uint8_t buff_data[8];

/* Init buffer and set event function */
ringbuff_init(&buff, buff_data, sizeof(buff_data));
ringbuff_set_evt_fn(&buff, my_buff_evt_fn);
