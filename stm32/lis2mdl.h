#ifndef LIS2MDL_H
#define LIS2MDL_H

#include "basic.h"
#include "uart.h"
#include "gpio.h"
#include "i2c.h"

#define LIS2MDL_I2C_ADDRESS 0x1E

#define LIS2MDL_CFG_REG_A_ADDRESS 0x60
#define LIS2MDL_CFG_REG_B_ADDRESS 0x61
#define LIS2MDL_CFG_REG_C_ADDRESS 0x62

#define LIS2MDL_STATUS_REG_ADDRESS 0x67

#define LIS2MDL_OUTX_L_REG_ADDRESS 0x68
#define LIS2MDL_OUTX_H_REG_ADDRESS 0x69

#define LIS2MDL_OUTY_L_REG_ADDRESS 0x6A
#define LIS2MDL_OUTY_H_REG_ADDRESS 0x6B

#define LIS2MDL_OUTZ_L_REG_ADDRESS 0x6C
#define LIS2MDL_OUTZ_H_REG_ADDRESS 0x6D

#define LIS2MDL_OFFSETX_L_REG_ADDRESS 0x45
#define LIS2MDL_OFFSETX_H_REG_ADDRESS 0x46
#define LIS2MDL_OFFSETX_VALUE (int16_t)(-455)

#define LIS2MDL_OFFSETY_L_REG_ADDRESS 0x47
#define LIS2MDL_OFFSETY_H_REG_ADDRESS 0x48
#define LIS2MDL_OFFSETY_VALUE (int16_t)(-500)

#define LIS2MDL_OFFSETZ_L_REG_ADDRESS 0x49
#define LIS2MDL_OFFSETZ_H_REG_ADDRESS 0x4A
#define LIS2MDL_OFFSETZ_VALUE (int16_t)(0)

#define LIS2MDL_CFG_REG_A_STARTUP_VALUE 0x8C
#define LIS2MDL_CFG_REG_B_STARTUP_VALUE 0x01
#define LIS2MDL_CFG_REG_C_STARTUP_VALUE 0x01

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} Magnetometer_Raw_Data;

Result lis2mdl_init();

Result lis2mdl_get_raw_data(Magnetometer_Raw_Data *mgnmtr_data);

#endif