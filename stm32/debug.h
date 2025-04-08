#ifndef DEBUG_H
#define DEBUG_H

#include "uart.h"

#define DPRINT(x) uart_write_buf(USART2, (x), sizeof((x)));
#define DPRINTLN(x) { uart_write_buf(USART2, (x), sizeof((x))); uart_write_buf(USART2, "\r\n", 2); }
#define DPRINTN(x) uart_write_number(USART2, (x), 10);
#define DPRINTNS(x) uart_write_number_signed(USART2, (x), 10);
#define DPRINTBUF(x,sz) uart_write_buf(USART2, (x), (sz));
#define DPRINTB(x) uart_write_number(USART2, (x), 2);
#define DPRINTH(x) uart_write_number(USART2, (x), 8);
#define DPRINTNL() uart_write_buf(USART2, "\r\n", 2);
#define DCLRSCRN() uart_write_buf(USART2, "\33c", 2);

#define ENABLE_DBG_TRACES() {\
    const UART_Config uart_cfg = {\
        USART2,\
        {'A', 2},\
        {'A', 3}\
    };\
    Result dbg_init_res = uart_init(&uart_cfg);\
    if (dbg_init_res) { for (;;) {} };\
    DCLRSCRN();\
    DPRINTLN("DEBUG TRACES ON");\
}

#endif