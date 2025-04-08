#include "i2c.h"

const I2C_Config common_i2c_config = {
    I2C1,       // i2c
    {'B', 8},   // scl_pin
    {'B', 9},   // sda_pin
};

// Only doing master mode right now
Result i2c_init(const I2C_Config *config) {
    uint8_t af_num = I2C_AF_NUMBER;

    switch ((uint32_t)config->i2c) {
        case I2C1_REGS_START_ADDRESS:
            RCC->APB1ENR1 |= BIT(I2C1_APB1ENR1_BIT);
            break;
        case I2C2_REGS_START_ADDRESS:
            RCC->APB1ENR1 |= BIT(I2C2_APB1ENR1_BIT);
            break;
        case I2C3_REGS_START_ADDRESS:
            RCC->APB1ENR1 |= BIT(I2C3_APB1ENR1_BIT);
            break;
        default:
            return RES_BAD_PARAM;
    }

    gpio_set_mode(GPIO_MODE_AF, config->scl_pin);
    gpio_set_speed(GPIO_SPEED_HIGH, config->scl_pin);
    gpio_set_pupdr(GPIO_PP_PULL_UP, config->scl_pin);
    gpio_set_af(af_num, config->scl_pin);
    gpio_set_otyper(true, config->scl_pin);

    gpio_set_mode(GPIO_MODE_AF, config->sda_pin);
    gpio_set_speed(GPIO_SPEED_HIGH, config->sda_pin);
    gpio_set_pupdr(GPIO_PP_PULL_UP, config->sda_pin);
    gpio_set_af(af_num, config->sda_pin);
    gpio_set_otyper(true, config->sda_pin);

    config->i2c->CR1 &= ~BIT(0); // PE - peripheral enable
    WAIT_FOR_CONDITION(~(config->i2c->CR1 & BIT(0)), GENERIC_TIMEOUT_NUM);

    config->i2c->TIMOUTGR = I2C_TIMING; // set timing

    config->i2c->CR1 |= BIT(0); // PE - peripheral enable

    return RES_OK;
}

Result wait_for_nack_clear(I2C_Regs *i2c) {
    WAIT_FOR_CONDITION(~(i2c->CR2 & BIT(4)), GENERIC_TIMEOUT_NUM);
    return RES_OK;
}

Result i2c_write_buf(I2C_Regs *i2c, I2C_Address addr, char *buf, size_t len) {
    //DPRINTLN("WRITE");
    //DPRINTB(i2c->CR2);
    //DPRINTNL();
    //DPRINTB(i2c->ISR);
    //DPRINTNL();

    //reset CR2
    i2c->CR2 = 0x0;

    // set slave address
    i2c->CR2 |= (uint32_t)addr << 1;

    // set transfer direction to write
    i2c->CR2 &= ~BIT(10);

    // set nbytes to transfer
    i2c->CR2 |= (uint32_t)len << 16;

    // auto end
    i2c->CR2 |= BIT(25);

    //DPRINTLN("START");
    //DPRINTB(i2c->CR2);
    //DPRINTNL();
    //DPRINTB(i2c->ISR);
    //DPRINTNL();

    // enable START bit
    i2c->CR2 |= BIT(13);

    // begin transfer
    while(len-- > 0) {
        //DPRINTLN("BYTE");
        //DPRINTB(i2c->CR2);
        //DPRINTNL();
        //DPRINTB(i2c->ISR);
        //DPRINTNL();

        // if nackf received, return error
        if (wait_for_nack_clear(i2c)) {
            // set nackcf
            i2c->ICR |= BIT(4);
            return RES_WRITE_NACK;
        }

        // if txe not set, continue
        WAIT_FOR_CONDITION(i2c->ISR & BIT(1), GENERIC_TIMEOUT_NUM);

        // write byte to txdr
        i2c->TXDR = *(uint8_t *) buf++;
    }

    //DPRINTLN("DONE");
    //DPRINTB(i2c->CR2);
    //DPRINTNL();
    //DPRINTB(i2c->ISR);
    //DPRINTNL();

    // check tc bit
    WAIT_FOR_CONDITION(i2c->ISR & (BIT(5)), GENERIC_TIMEOUT_NUM);
    // stop bit
    //i2c->CR2 |= BIT(14);  
    return RES_OK;
}

Result i2c_read_buf(I2C_Regs *i2c, I2C_Address addr, char *buf, size_t len) {
    //DPRINTLN("READ");
    //DPRINTB(i2c->CR2);
    //DPRINTNL();
    //DPRINTB(i2c->ISR);
    //DPRINTNL();

    //reset CR2
    i2c->CR2 = 0x0;

    // set slave address
    i2c->CR2 |= (uint32_t)addr << 1;

    // set transfer direction to read
    i2c->CR2 |= BIT(10);

    // set nbytes to transfer
    i2c->CR2 |= (uint32_t)len << 16;

    // auto end
    i2c->CR2 |= BIT(25);

    //DPRINTLN("START");
    //DPRINTB(i2c->CR2);
    //DPRINTNL();
    //DPRINTB(i2c->ISR);
    //DPRINTNL();

    // enable START bit
    i2c->CR2 |= BIT(13);

    // begin transfer
    for (size_t i = 0; i < len; i++) {
        //DPRINTLN("BYTE");
        //DPRINTB(i2c->CR2);
        //DPRINTNL();
        //DPRINTB(i2c->ISR);
        //DPRINTNL();

        if (wait_for_nack_clear(i2c)) {
            // set nackcf
            i2c->ICR |= BIT(4);
            return RES_READ_NACK;
        }

        // if rxne not set, continue
        WAIT_FOR_CONDITION(i2c->ISR & BIT(2), GENERIC_TIMEOUT_NUM);

        // read byte from rxdr
        buf[i] = i2c->RXDR;
    }

    //DPRINTLN("DONE");
    //DPRINTB(i2c->CR2);
    //DPRINTNL();
    //DPRINTB(i2c->ISR);
    //DPRINTNL();

    // check tc bit
    WAIT_FOR_CONDITION(i2c->ISR & (BIT(5)), GENERIC_TIMEOUT_NUM);
    // stop bit
    //i2c->CR2 |= BIT(14);  
    return RES_OK;
}