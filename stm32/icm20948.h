#ifndef ICM20948_H
#define ICM20948_H

#include "basic.h"
#include "uart.h"
#include "gpio.h"
#include "i2c.h"

#define ICM_I2C_ADDRESS 0b110100

#define ICM_BANK_SELECT_REG_ADDRESS 0x7F


//##########

#define ICM_ACCEL_OUTX_L_REG_ADDRESS 0x11
#define ICM_ACCEL_OUTX_H_REG_ADDRESS 0x12

#define ICM_ACCEL_OUTY_L_REG_ADDRESS 0x13
#define ICM_ACCEL_OUTY_H_REG_ADDRESS 0x14

#define ICM_ACCEL_OUTZ_L_REG_ADDRESS 0x15
#define ICM_ACCEL_OUTZ_H_REG_ADDRESS 0x16

//##########

#define ICM_GYRO_OUTX_L_REG_ADDRESS 0x11
#define ICM_GYRO_OUTX_H_REG_ADDRESS 0x12

#define ICM_GYRO_OUTY_L_REG_ADDRESS 0x13
#define ICM_GYRO_OUTY_H_REG_ADDRESS 0x14

#define ICM_GYRO_OUTZ_L_REG_ADDRESS 0x15
#define ICM_GYRO_OUTZ_H_REG_ADDRESS 0x16

//##########

#define ICM_MAG_OUTX_L_REG_ADDRESS 0x11
#define ICM_MAG_OUTX_H_REG_ADDRESS 0x12

#define ICM_MAG_OUTY_L_REG_ADDRESS 0x13
#define ICM_MAG_OUTY_H_REG_ADDRESS 0x14

#define ICM_MAG_OUTZ_L_REG_ADDRESS 0x15
#define ICM_MAG_OUTZ_H_REG_ADDRESS 0x16

#define ICM_MAG_OFFSETX_L_REG_ADDRESS 0x45
#define ICM_MAG_OFFSETX_H_REG_ADDRESS 0x46
#define ICM_MAG_OFFSETX_VALUE (int16_t)(0)

#define ICM_MAG_OFFSETY_L_REG_ADDRESS 0x47
#define ICM_MAG_OFFSETY_H_REG_ADDRESS 0x48
#define ICM_MAG_OFFSETY_VALUE (int16_t)(0)

#define ICM_MAG_OFFSETZ_L_REG_ADDRESS 0x49
#define ICM_MAG_OFFSETZ_H_REG_ADDRESS 0x4A
#define ICM_MAG_OFFSETZ_VALUE (int16_t)(0)

//##########

typedef struct {
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;

    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;

    int16_t mag_x;
    int16_t mag_y;
    int16_t mag_z;
} ICM20948_Raw_Data;

#endif