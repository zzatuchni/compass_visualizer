#include "esp8266.h"

const volatile char esp_command_buf[128] = {'0', '1', '2', '3', '4'};

#define USART3_PERIPH_ADDR ((uint32_t)&(USART3->RDR))

#define esp8266_send_cmd(x) uart_write_buf(USART3, (x), sizeof((x))-1);

Result esp8266_dma_init() {
    USART3->CR3 |= BIT(6); // enable dma from usart

    RCC->AHB1ENR |= BIT(DMA1_AHB1ENR_BIT);

    DMA1->CPAR3 = USART3_PERIPH_ADDR;
    DMA1->CMAR3 = (uint32_t)&esp_command_buf;

    DMA1->CNDTR3 = 128;
    DMA1->CSELR |= BIT(9);  // channel mapping

    DMA1->CCR3 |= BIT(5);   // circular mode
    DMA1->CCR3 |= BIT(13);  // high prio
    DMA1->CCR3 |= BIT(7);   // memory increment

    // enable interrupts
    //DMA1->CCR3 |= BIT(1);
    //DMA1->CCR3 |= BIT(3);

    enable_interrupt(DMA1_CH3_INTERRUPT_NUM);

    DMA1->CCR3 |= BIT(0);

    //DPRINTB(DMA1_REGS_START_ADDRESS);
    //DPRINTNL();
    //DPRINTB(&DMA1->CCR3);
    //DPRINTNL();
    //DPRINTB(DMA1->CPAR3);
    //DPRINTNL();
    //DPRINTB(DMA1->CMAR3);
    //DPRINTNL();

    
    return RES_OK;
}

Result esp8266_init() {

    Result res;

    res = uart_init(&wifi_uart_config, false);
    if (res) return res;

    res = esp8266_dma_init();
    if (res) return res;

    //DPRINT("### SEND: ")
    //DPRINT(AT_RST_CMD)
    //DPRINTNL();

    //esp8266_send_cmd(AT_RST_CMD);
    //spin(200000);

    //uart_write_buf(USART2, esp_command_buf, 128);
    //DPRINTNL();
 
    DPRINT("### SEND: ")
    DPRINT(ATE1_CMD)
    DPRINTNL();

    esp8266_send_cmd(ATE1_CMD);
    spin(200);

    uart_write_buf(USART2, esp_command_buf, 128);
    DPRINTNL();

    DPRINT("### SEND: ")
    DPRINT(AT_TEST_CMD)
    DPRINTNL();

    esp8266_send_cmd(AT_TEST_CMD);
    spin(200);

    uart_write_buf(USART2, esp_command_buf, 128);
    DPRINTNL();

    DPRINT("### SEND: ")
    DPRINT(AT_WIFI_MODE_CMD)
    DPRINTNL();

    esp8266_send_cmd(AT_WIFI_MODE_CMD);
    spin(200);

    uart_write_buf(USART2, esp_command_buf, 128);
    DPRINTNL();

    DPRINT("### SEND: ")
    DPRINT(AT_CONF_GET_APS_CMD)
    DPRINTNL();

    esp8266_send_cmd(AT_CONF_GET_APS_CMD);
    spin(200);

    uart_write_buf(USART2, esp_command_buf, 128);
    DPRINTNL();

    DPRINT("### SEND: ")
    DPRINT(AT_GET_APS_CMD)
    DPRINTNL();

    esp8266_send_cmd(AT_GET_APS_CMD);
    spin(200000);
    spin(200000);
    spin(200000);
    spin(200000);
    spin(200000);
    spin(200000);
    spin(200000);

    uart_write_buf(USART2, esp_command_buf, 128);
    DPRINTNL();

    /*

    DPRINT("### SEND: ")
    DPRINT(AT_MULTIPLE_CONNECTIONS_CMD)
    DPRINTNL();

    esp8266_send_cmd(AT_MULTIPLE_CONNECTIONS_CMD);
    spin(200);

    uart_write_buf(USART2, esp_command_buf, 128);
    DPRINTNL();

    DPRINT("### SEND: ")
    DPRINT(AT_SET_SERVER_CMD)
    DPRINTNL();

    esp8266_send_cmd(AT_SET_SERVER_CMD);
    spin(200);

    uart_write_buf(USART2, esp_command_buf, 128);
    DPRINTNL();

    DPRINT("### SEND: ")
    DPRINT(AT_SEND_DATA_CMD)
    DPRINTNL();

    esp8266_send_cmd(AT_SEND_DATA_CMD);
    spin(200);

    uart_write_buf(USART2, esp_command_buf, 128);
    DPRINTNL();
    */


    return RES_OK;
}