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

#define DATA_TRANSMISSION_FEQ_HZ 100

//////////////////////////////////////
// INTERRUPTS
//////////////////////////////////////

void _on_uart2_interrupt(void) {
    uint8_t discard_byte = uart_read_byte(USART2);
}

void _on_hard_fault(void) {
    DPRINT("*HF*\r\n");
    DPRINTH(*(uint32_t *) 0xE000ED28);
    DPRINTNL();
    DPRINTB(*(uint32_t *) 0xE000ED28);
    DPRINTNL();
    DPRINTH(*(uint16_t *)0xE000ED2A);
    DPRINTNL();
    DPRINTB(*(uint16_t *)0xE000ED2A);
    DPRINTNL();;
    DPRINTH(*(uint8_t *)0xE000ED29);
    DPRINTNL();
    DPRINTB(*(uint8_t *)0xE000ED29);
    DPRINTNL();
    DPRINTH(*(uint8_t *)0xE000ED28);
    DPRINTNL();
    DPRINTB(*(uint8_t *)0xE000ED28);
    DPRINTNL();
    DPRINTB(USART2->ISR);
    for (;;) {}
}

void _on_mem_fault(void) {
    DPRINT("*MF*\r\n");
    DPRINTH(*(uint32_t *) 0xE000ED28);
    DPRINT("\r\n");
    DPRINTB(*(uint32_t *) 0xE000ED28);
    for (;;) {}
}

void _on_bus_fault(void) {
    DPRINT("*BF*\r\n");
    DPRINTH(*(uint32_t *) 0xE000ED28);
    DPRINT("\r\n");
    DPRINTB(*(uint32_t *) 0xE000ED28);
    for (;;) {}
}

void _on_usage_fault(void) {
    DPRINT("*UF*\r\n");
    DPRINTH(*(uint32_t *) 0xE000ED28);
    DPRINT("\r\n");
    DPRINTB(*(uint32_t *) 0xE000ED28);
    for (;;) {}
}

volatile Magnetometer_Raw_Data mgmtr_data = {0xffff, 0xffff};
void _on_systick(void) {
    Result res = lis2mdl_get_raw_data(&mgmtr_data);
    if (res) { DPRINT("GET MGMT DATA "); DPRINTN(res); for (;;) { spin(50000); } };

    //DPRINTNS(mgmtr_data.x);
    //DPRINTNL();
    //DPRINTNS(mgmtr_data.y);
    //DPRINTNL();

    uart_write_buf(USART2, (char *)&(mgmtr_data.x), 2);
    uart_write_buf(USART2, (char *)&(mgmtr_data.y), 2);
}

//////////////////////////////////////
// APPLICATION
//////////////////////////////////////

int main(void) {

    Result res = uart_init(&uart_config);
    if (res) { for (;;) {} };
    DCLRSCRN();

    res = i2c_init(&common_i2c_config);
    if (res) { DPRINT("I2C INIT ERR "); DPRINTN(res); for (;;) {} };

    res = lis2mdl_init(&common_i2c_config);
    if (res) { DPRINT("LIS2 INIT ERR "); DPRINTN(res); for (;;) {} };

    systick_init(DEFAULT_SYSCLK_FREQ / DATA_TRANSMISSION_FEQ_HZ);
    for (;;) {}

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
    _estack, _reset, 0, _on_hard_fault, _on_mem_fault, _on_bus_fault, _on_usage_fault, 0, 0, 0, 0, 0, 0, 0, 0, _on_systick,  //ARM core interrupts
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 0 - 15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 16 - 31
    0, 0, 0, 0, 0, 0, _on_uart2_interrupt, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 32 - 47
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 48 - 63
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 64 - 79
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 80 - 96
    };