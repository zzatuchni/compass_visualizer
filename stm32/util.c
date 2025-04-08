#include "util.h"

 __attribute__((optimize("O0"))) void spin(uint32_t count) {
    while (count--) asm("nop");
}

void int_to_str(uint32_t x, char *buf, size_t len, uint8_t radix) {
    for (size_t i = len; i > 0; i--) {
        uint32_t rem = x % radix;
        x /= radix;
        if (rem < 10) {
            buf[i-1] = '0' + rem;
        }
        else {
            buf[i-1] = 'a' + (rem - 10);
        }
    }
}

bool str_cmp(char *buf1, char *buf2, size_t len) {
    for (uint8_t i = 0; i < len; i++) {
        if (buf1[i] != buf2[i]) return false;
    }
    return true;
}

void str_copy(char *buf1, char *buf2, size_t len) {
    for (uint8_t i = 0; i < len; i++) {
        buf2[i] = buf1[i];
    }
}