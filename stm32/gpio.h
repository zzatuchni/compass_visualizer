#ifndef GPIO_H
#define GPIO_H

#include "basic.h"

#define GPIOA_REGS_START_ADDRESS 0x48000000
#define GPIO_BANK_BOUNDARY_SIZE 0x400

typedef struct {
    volatile uint32_t 
        MODER, OTYPER, OSPEEDR, PUPDR, IDR, ODR, BSRR, LCKR, AFR[2], BRR, ASCR;
} GPIO_Bank;
#define GET_GPIO_BANK(bank) (GPIO_Bank *) ((GPIOA_REGS_START_ADDRESS) + (GPIO_BANK_BOUNDARY_SIZE) * ((bank) - 'A'))

typedef enum { 
    GPIO_MODE_INPUT, GPIO_MODE_OUTPUT, GPIO_MODE_AF, GPIO_MODE_ANALOG 
} GPIO_Mode;

typedef enum { 
    GPIO_SPEED_LOW, GPIO_SPEED_MED, GPIO_SPEED_FAST, GPIO_SPEED_HIGH 
} GPIO_Speed;

typedef enum { 
    GPIO_PP_PULL_DOWN_ONLY, GPIO_PP_PULL_UP, GPIO_PP_PULL_DOWN 
} GPIO_PushPull;

typedef uint8_t GPIO_AF;

void gpio_set_mode(GPIO_Mode mode, STM32_Pin pin);

void gpio_set_af(GPIO_AF af, STM32_Pin pin);

void gpio_set_speed(GPIO_Speed spd, STM32_Pin pin);

void gpio_set_otyper(bool is_open_drain, STM32_Pin pin);

void gpio_set_pupdr(GPIO_PushPull pp, STM32_Pin pin);

void gpio_write(bool value, STM32_Pin pin);

bool gpio_read(STM32_Pin pin);

#endif