#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "basic.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "debug.h"
#include "lis2mdl.h"

//////////////////////////////////////
// INTERRUPTS
//////////////////////////////////////

void _on_hard_fault(void) {
    DPRINT("*HF*");
    spin(50000);
}

void _on_mem_fault(void) {
    DPRINT("*MF*");
    spin(50000);
}

void _on_bus_fault(void) {
    DPRINT("*BF*");
    spin(50000);
}

void _on_usage_fault(void) {
    DPRINT("*UF*");
    spin(50000);
}

//////////////////////////////////////
// APPLICATION
//////////////////////////////////////

Result send_magnetometer_data(Magnetometer_Raw_Data *mgmtr_data, UART_Regs *uart) {

    uart_write_buf(uart, (char *)&mgmtr_data->x, 2);
    uart_write_buf(uart, (char *)&mgmtr_data->y, 2);
}

int main(void) {

    Result res = uart_init(&uart_config);
    if (res) { for (;;) {} };
    //DCLRSCRN();
    //DPRINTLN("DEBUG TRACES ON");

    res = i2c_init(&common_i2c_config);
    //if (res) { DPRINT("I2C INIT ERR "); DPRINTN(res); for (;;) {} };

    res = lis2mdl_init(&common_i2c_config);
    //if (res) { DPRINT("LIS2 INIT ERR "); DPRINTN(res); for (;;) {} };

    Magnetometer_Raw_Data mgmtr_data;

    for (;;) {
        res = lis2mdl_get_raw_data(&mgmtr_data);
        //if (res) { DPRINT("GET MGMT ERR "); DPRINTN(res); for (;;) {} };

        //DPRINT("X ");
        //DPRINTNS((int32_t)mgmtr_data.x);
        //DPRINT("\r\n");
        //DPRINT("Y ");
        //DPRINTNS((int32_t)mgmtr_data.y);
        //DPRINT("\r\n");
        //DPRINT("Z ");
        //DPRINTNS((int32_t)mgmtr_data.z);
        //DPRINT("\r\n");

        send_magnetometer_data(&mgmtr_data, USART2);

        spin(500000);
    }

}

//////////////////////////////////////
// BOARD SETUP
//////////////////////////////////////

__attribute__((naked, noreturn)) void _reset(void) {
    // memset .bss to zero, and copy .data section to RAM region
    extern long _sbss, _ebss, _sdata, _edata, _sidata;
    for (long *dst = &_sbss; dst < &_ebss; dst++) *dst = 0;
    for (long *dst = &_sdata, *src = &_sidata; dst < &_edata;) *dst++ = *src++;
  
    main();             // Call main()
    for (;;) (void) 0;  // Infinite loop in the case if main() returns
  }
  
  extern void _estack(void);  // Defined in link.ld

// 16 standard and 91 STM32-specific handlers
__attribute__((section(".vectors"))) void (*const volatile tab[16 + 99])(void) = {
    _estack, _reset, 0, _on_hard_fault, _on_mem_fault, _on_bus_fault, _on_usage_fault, 0, 0, 0, 0, 0, 0, 0, 0, 0,  //ARM core interrupts
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - 15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16 - 31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 32 - 47
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48 - 63
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64 - 79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80 - 96
    };