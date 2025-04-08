#include "basic.h"

////////////////////////////////////////////////////////////
//  Interrupts
////////////////////////////////////////////////////////////

void enable_interrupt(uint8_t interrupt_num) {
    uint32_t *nvic_iser = (uint32_t *)NVIC_ISER(interrupt_num / 32);
    (*nvic_iser) |= BIT(interrupt_num % 32);
}

void disable_interrupt(uint8_t interrupt_num) {
    uint32_t *nvic_icer = (uint32_t *)NVIC_ICER(interrupt_num / 32);
    (*nvic_icer) |= BIT(interrupt_num % 32);
}

////////////////////////////////////////////////////////////
//  EXTI
////////////////////////////////////////////////////////////

uint8_t exti_get_interrupt_num(uint8_t line_number) {
    if (line_number <= 4) return line_number + EXTI0_INTERRUPT_NUM;
    else if (line_number <= 9) return EXTI9_5_INTERRUPT_NUM;
    else return EXTI15_10_INTERRUPT_NUM;
}

void exti_enable_on_pin(STM32_Pin pin, EXTI_Mode trigger_mode) {
    EXTI_Regs *exti = (EXTI_Regs *)EXTI_REGS1_START_ADDRESS;
    Syscfg_Regs *syscfg = (Syscfg_Regs *)SYSCFG_REGS_START_ADDRESS;

    syscfg->EXTICR[pin.number / 4] &= ~(0b1111UL << ((uint32_t)(pin.number % 4) * 4));
    syscfg->EXTICR[pin.number / 4] |= ((uint32_t)(pin.bank - 'A') << ((uint32_t)(pin.number % 4) * 4));

    exti->IMR |= (1UL << pin.number);

    exti->RTSR &= ~(1UL << pin.number);
    exti->RTSR |= ((uint32_t)(trigger_mode & 0b01) << pin.number);
    exti->FTSR &= ~(1UL << pin.number);
    exti->FTSR |= ((uint32_t)((trigger_mode & 0b10) / 2) << pin.number);

    uint8_t interrupt_num = exti_get_interrupt_num(pin.number);
    enable_interrupt(interrupt_num);
}

void exti_enable(uint8_t line_number, EXTI_Mode trigger_mode) {
    EXTI_Regs *exti;
    uint8_t ln;
    
    if (line_number < 32) {
        exti = (EXTI_Regs *)EXTI_REGS1_START_ADDRESS;
        ln = line_number;
    } else {
        exti = (EXTI_Regs *)EXTI_REGS2_START_ADDRESS;
        ln = line_number - 32;
    }

    Syscfg_Regs *syscfg = (Syscfg_Regs *)SYSCFG_REGS_START_ADDRESS;

    exti->IMR |= (1UL << ln);

    exti->RTSR &= ~(1UL << ln);
    exti->RTSR |= ((uint32_t)(trigger_mode & 0b01) << ln);
    exti->FTSR &= ~(1UL << ln);
    exti->FTSR |= ((uint32_t)((trigger_mode & 0b10) / 2) << ln);

    uint8_t interrupt_num = exti_get_interrupt_num(ln);
    enable_interrupt(interrupt_num);
}

////////////////////////////////////////////////////////////
//  Systick
////////////////////////////////////////////////////////////

void systick_init(uint32_t ticks) {
    Systick_Regs *systick = (Systick_Regs *)SYSTICK_REGS_START_ADDRESS;
    if ((ticks - 1) > 0xffffff) return;
    systick->RVR = ticks - 1;
    systick->CVR = 0;
    systick->CSR |= 7UL;
    RCC->APB2ENR |= BIT(0);
}