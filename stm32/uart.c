#include "uart.h"

char temp_number_buffer[32];

const UART_Config debug_uart_config = {
    USART2,
    {'A', 2},
    {'A', 3},
    USART1_BAUD_RATE
};

const UART_Config wifi_uart_config = {
    USART3,
    {'C', 4},
    {'C', 5},
    USART3_BAUD_RATE
};

Result uart_init(const UART_Config *config, bool interrupt_en) {

    // enable peripheral in RCC and get af_num and interrupt_num

    uint8_t af_num = 0;
    uint8_t interrupt_num = 0;

    switch ((uint32_t)config->uart) {
        case USART1_REGS_START_ADDRESS:
            RCC->APB2ENR |= BIT(USART1_APB2ENR_BIT);
            af_num = USART1_3_AF_NUMBER;
            interrupt_num = USART1_INTERRUPT_NUM;
            break;
        case USART2_REGS_START_ADDRESS:
            RCC->APB1ENR1 |= BIT(USART2_APB1ENR1_BIT);
            af_num = USART1_3_AF_NUMBER;
            interrupt_num = USART2_INTERRUPT_NUM;
            break;
        case USART3_REGS_START_ADDRESS:
            RCC->APB1ENR1 |= BIT(USART3_APB1ENR1_BIT);
            af_num = USART1_3_AF_NUMBER;
            interrupt_num = USART3_INTERRUPT_NUM;
            break;
        case UART4_REGS_START_ADDRESS:
            RCC->APB1ENR1 |= BIT(UART4_APB1ENR1_BIT);
            af_num = UART4_5_AF_NUMBER;
            interrupt_num = UART4_INTERRUPT_NUM;
            break;
        case UART5_REGS_START_ADDRESS:
            RCC->APB1ENR1 |= BIT(UART5_APB1ENR1_BIT);
            af_num = UART4_5_AF_NUMBER;
            interrupt_num = UART5_INTERRUPT_NUM;
            break;
        default:
            return RES_BAD_PARAM;
    }

    // configure the pin gpio

    gpio_set_mode(GPIO_MODE_AF, config->tx_pin);
    gpio_set_mode(GPIO_MODE_AF, config->rx_pin);

    gpio_set_af(af_num, config->tx_pin);
    gpio_set_af(af_num, config->rx_pin);

    // configure the uart regs

    config->uart->CR1 = 0;                                      // Disable this UART
    config->uart->BRR = DEFAULT_SYSCLK_FREQ / config->baud;   // DEFAULT_SYSCLK_FREQ is the UART bus frequency
    config->uart->CR1 |= BIT(0) | BIT(2) | BIT(3);     // Set UE, RE, TE, RXNEIE (uart enable, rx enable, tx enable, rx interrupt enable)
    if (interrupt_en) {
        config->uart->CR1 |= BIT(5);
        enable_interrupt(interrupt_num);
    }

    return RES_OK;
}

Result lpuart_init(const LPUART_Config *config) {

    // enable peripheral in RCC
    RCC->APB1ENR2 |= BIT(LPUART1_APB1ENR2_BIT);
    uint8_t af_num = UART4_5_AF_NUMBER;
    uint8_t interrupt_num = LPUART1_INTERRUPT_NUM;

    gpio_set_mode(GPIO_MODE_AF, config->tx_pin);
    gpio_set_mode(GPIO_MODE_AF, config->rx_pin);

    gpio_set_af(af_num, config->tx_pin);
    gpio_set_af(af_num, config->rx_pin);

    LPUART_Regs *uart = LPUART1;

    uart->CR1 = 0;                                      // Disable this UART
    uart->BRR = DEFAULT_SYSCLK_FREQ / config->baud;   // FREQ is a UART bus frequency
    uart->CR1 |= BIT(0) | BIT(2) | BIT(3) | BIT(5);     // Set UE, RE, TE, RXNEIE

    enable_interrupt(interrupt_num);

    return RES_OK;
}

uint8_t uart_read_byte(UART_Regs *uart) {
    return (uint8_t) (uart->RDR & 255);
}

int uart_read_ready(UART_Regs *uart) {
    return (uart->ISR & BIT(5)); // RXNE
}

void uart_write_byte(UART_Regs *uart, uint8_t byte) {
    uart->TDR = byte;
    while ((uart->ISR & BIT(6)) == 0) spin(1); // TC - transmission complete
}

Result uart_read_until(UART_Regs *uart, char *buf, char *look_for, size_t len, size_t max_wait) {
    
    /**/
    char *search_end = buf;
    char *search_start = buf;
    for (size_t i = 0; i < len; i++) {
        WAIT_FOR_CONDITION(uart_read_ready(uart), GENERIC_TIMEOUT_NUM);
        char byte = (char)uart_read_byte(uart);
        uart_write_byte(USART2, byte);
        uart_write_byte(USART2, 'X');
        *search_end = byte;
        search_end++;
    }
    while (true) {
        if (str_cmp(search_start, look_for, len)) return RES_OK;
        while (!uart_read_ready(uart)) {};
        *search_end = (char)uart_read_byte(uart);
        search_end++;
        search_start++;
        if (search_end - buf >= max_wait) return RES_FAILURE;
    }
    return RES_OK;
}

void uart_write_buf(UART_Regs *uart, char *buf, size_t len) {
    while (len-- > 0) uart_write_byte(uart, *(uint8_t *) buf++);
}

void uart_write_number(UART_Regs *uart, uint32_t x, uint8_t radix) {
    bool leading_zero = true;
    if (radix == 8) uart_write_buf(uart, "0x", 2);
    else if (radix == 2) uart_write_buf(uart, "0b", 2);
    for (size_t i = 32; i > 0; i--) {
        uint32_t rem = x % radix;
        x /= radix;
        if (rem < 10) {
            temp_number_buffer[i-1] = '0' + rem;
        }
        else {
            temp_number_buffer[i-1] = 'a' + (rem - 10);
        }
    }
    for (size_t i = 0; i < 32; i++) {
        if (temp_number_buffer[i] != '0' || !leading_zero || (i == 31)) {
            if (leading_zero) leading_zero = false;
            uart_write_byte(uart, temp_number_buffer[i]);
        }
    }
}

void uart_write_number_signed(UART_Regs *uart, int32_t x, uint8_t radix) {
    if (x < 0) {
        uart_write_byte(uart, '-');
        x = x * -1;
    }

    bool leading_zero = true;
    if (radix == 8) uart_write_buf(uart, "0x", 2);
    else if (radix == 2) uart_write_buf(uart, "0b", 2);
    for (size_t i = 32; i > 0; i--) {
        uint32_t rem = x % radix;
        x /= radix;
        if (rem < 10) {
            temp_number_buffer[i-1] = '0' + rem;
        }
        else {
            temp_number_buffer[i-1] = 'a' + (rem - 10);
        }
    }
    for (size_t i = 0; i < 32; i++) {
        if (temp_number_buffer[i] != '0' || !leading_zero || (i == 31)) {
            if (leading_zero) leading_zero = false;
            uart_write_byte(uart, temp_number_buffer[i]);
        }
    }
}