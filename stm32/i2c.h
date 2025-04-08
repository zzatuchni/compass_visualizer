#ifndef I2C_H
#define I2C_H

#include "basic.h"
#include "gpio.h"

#define I2C1_REGS_START_ADDRESS 0x40005400
#define I2C2_REGS_START_ADDRESS 0x40005800
#define I2C3_REGS_START_ADDRESS 0x40005C00

#define I2C1_APB1ENR1_BIT 21
#define I2C2_APB1ENR1_BIT 22
#define I2C3_APB1ENR1_BIT 23

#define I2C_AF_NUMBER 4

typedef uint8_t I2C_Address;

typedef struct {
    volatile uint32_t CR1, CR2, OAR1, OAR2, TIMINGR, TIMOUTGR, 
        ISR, ICR, PECR, RXDR, TXDR;
} I2C_Regs;

#define I2C1 ((I2C_Regs *)I2C1_REGS_START_ADDRESS)
#define I2C2 ((I2C_Regs *)I2C2_REGS_START_ADDRESS)
#define I2C3 ((I2C_Regs *)I2C3_REGS_START_ADDRESS)

// Standard mode, 4khz clock
#define I2C_TIMING 0x004091F3

typedef struct {
    I2C_Regs *i2c;
    STM32_Pin scl_pin;
    STM32_Pin sda_pin;
} I2C_Config;

extern const I2C_Config common_i2c_config;

// Only doing master mode right now
Result i2c_init(const I2C_Config *config);

Result i2c_write_buf(I2C_Regs *i2c, I2C_Address addr, char *buf, size_t len);

Result i2c_read_buf(I2C_Regs *i2c, I2C_Address addr, char *buf, size_t len);

#endif