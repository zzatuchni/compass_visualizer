#include "gpio.h"

void gpio_set_mode(GPIO_Mode mode, STM32_Pin pin) {
    GPIO_Bank *gpio = GET_GPIO_BANK(pin.bank); 
    int n = pin.number;
    RCC->AHB2ENR |= (1UL << (pin.bank - 'A'));
    gpio->MODER &= ~(3U << (n * 2));            //reset mode
    gpio->MODER |= ((mode & 3U) << (n * 2));    //set to new mode
}

void gpio_set_af(GPIO_AF af, STM32_Pin pin) {
    GPIO_Bank *gpio = GET_GPIO_BANK(pin.bank); 
    int n = pin.number;
    gpio->AFR[n >> 3] &= ~(15UL << ((n & 7) * 4));              //reset af
    gpio->AFR[n >> 3] |= (((uint32_t) af) << ((n & 7) * 4));    //set to new mode
}

void gpio_set_speed(GPIO_Speed spd, STM32_Pin pin) {
    GPIO_Bank *gpio = GET_GPIO_BANK(pin.bank); 
    int n = pin.number;
    gpio->OSPEEDR &= ~(3U << (n * 2));           //reset speed
    gpio->OSPEEDR |= ((spd & 3U) << (n * 2));    //set to new speed
}

void gpio_set_otyper(bool is_open_drain, STM32_Pin pin) {
    GPIO_Bank *gpio = GET_GPIO_BANK(pin.bank); 
    int n = pin.number;
    gpio->OTYPER &= ~(1U << (n));                     //reset otyper
    gpio->OTYPER |= ((is_open_drain & 1U) << (n));    //set to new otyper
}

void gpio_set_pupdr(GPIO_PushPull pp, STM32_Pin pin) {
    GPIO_Bank *gpio = GET_GPIO_BANK(pin.bank); 
    int n = pin.number;
    gpio->PUPDR &= ~(3U << (n * 2));            //reset pupdr
    gpio->PUPDR |= ((pp & 3U) << (n * 2));      //set to new pupdr
}

void gpio_write(bool value, STM32_Pin pin) {
    GPIO_Bank *gpio = GET_GPIO_BANK(pin.bank); 
    int n = pin.number;
    gpio->BSRR = (1U << n) << (value ? 0 : 16);
}

bool gpio_read(STM32_Pin pin) {
    GPIO_Bank *gpio = GET_GPIO_BANK(pin.bank); 
    int n = pin.number;
    return (gpio->IDR & (1U << n));
}