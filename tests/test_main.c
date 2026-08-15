#include <stdio.h>
#include "lwrb/lwrb.h"
#include "test.h"

int
main(void) {
    int ret = 0;
    printf("Application running\r\n");
    ret = test_run();
    printf("Done\r\n");
    return ret;
}
