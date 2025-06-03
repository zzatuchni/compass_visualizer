#ifndef BASIC_H
#define BASIC_H

#include "util.h"

////////////////////////////////////////////////////////////
//  Constants
////////////////////////////////////////////////////////////

#define CPU_CLOCK_FREQ 16000000
#define DEFAULT_SYSCLK_FREQ 4000000

////////////////////////////////////////////////////////////
//  Pin
////////////////////////////////////////////////////////////

typedef struct {
    uint8_t bank;
    uint8_t number;
} STM32_Pin;


////////////////////////////////////////////////////////////
//  Syscfg
////////////////////////////////////////////////////////////

#define SYSCFG_REGS_START_ADDRESS 0x40010000

typedef struct {
    volatile uint32_t MEMRMP, CFGR1, EXTICR[4], SCSR, CFGR2, SWPR, SKR, SWPR2;
} Syscfg_Regs;

////////////////////////////////////////////////////////////
//  Interrupts
////////////////////////////////////////////////////////////

#define NVIC_ISER(x) (0xE000E100 + (0x04 * (x)))    // set enable
#define NVIC_ICER(x) (0xE000E180 + (0x04 * (x)))    // clear enable
#define NVIC_ISPR(x) (0xE000E200 + (0x04 * (x)))    // set pending
#define NVIC_ICPR(x) (0xE000E280 + (0x04 * (x)))    // clear pending
#define NVIC_IABR(x) (0xE000E300 + (0x04 * (x)))    // active
#define NVIC_IPR(x) (0xE000E400  + (0x04 * (x)))    // priority

void enable_interrupt(uint8_t interrupt_num);

void disable_interrupt(uint8_t interrupt_num);

////////////////////////////////////////////////////////////
//  EXTI
////////////////////////////////////////////////////////////

#define EXTI_REGS1_START_ADDRESS 0x40010400
#define EXTI_REGS2_START_ADDRESS 0x40010420

#define EXTI0_INTERRUPT_NUM 6
#define EXTI1_INTERRUPT_NUM 7
#define EXTI2_INTERRUPT_NUM 8
#define EXTI3_INTERRUPT_NUM 9
#define EXTI4_INTERRUPT_NUM 10
#define EXTI9_5_INTERRUPT_NUM 23
#define EXTI15_10_INTERRUPT_NUM 40

typedef struct {
    volatile uint32_t IMR, EMR, RTSR, FTSR, SWIER, PR;
} EXTI_Regs;

typedef enum { 
    EXTI_NO_TRIGGER, EXTI_TRIGGER_ON_RISE, EXTI_TRIGGER_ON_FALL, EXTI_TRIGGER_ON_BOTH 
} EXTI_Mode;

uint8_t exti_get_interrupt_num(uint8_t line_number);

void exti_enable_on_pin(STM32_Pin pin, EXTI_Mode trigger_mode);

void exti_enable(uint8_t line_number, EXTI_Mode trigger_mode);

////////////////////////////////////////////////////////////
//  Flash
////////////////////////////////////////////////////////////

#define FLASH_REGS_START_ADDRESS 0x40022000
typedef struct {
    volatile uint32_t ACR, PDKEYR, KEYR, OPTKEYR, SR, CR, ECCR, OPTR, 
        PCROP1SR, PCROP1ER, WRP1AR, WRP1BR, 
        PCROP2SR, PCROP2ER, WRP2AR, WRP2BR;
} Flash_Regs;

#define FLASH ((Flash_Regs *) FLASH_REGS_START_ADDRESS)

////////////////////////////////////////////////////////////
//  RCC
////////////////////////////////////////////////////////////

#define AHB2_START_ADDRESS  0x40024400
#define AHB1_START_ADDRESS  0x40020000
#define APB2_START_ADDRESS  0x40010000
#define APB1_START_ADDRESS  0x40000000

#define RCC_REGS_START_ADDRESS   0x40021000
typedef struct {
    volatile uint32_t 
        CR, ICSCR, CFGR, 
        PLLCFGR, PLLSAI1CFGR, PLLSAI2CFGR,
        CIER, CIFR, CICR, RESERVED0,
        AHB1RSTR, AHB2RSTR, AHB3RSTR, RESERVED1, APB1RSTR1, APB1RSTR2, APB2RSTR, RESERVED2,
        AHB1ENR, AHB2ENR, AHB3ENR, RESERVED3, APB1ENR1, APB1ENR2, APB2ENR, RESERVED4,
        AHB1SMENR, AHB2SMENR, AHB3SMENR, RESERVED5, APB1SMENR1, APB1SMENR2, APB2SMENR, RESERVED6,
        CCIPR, RESERVED7, BDCR, CSR, CRRCR, CCIPR2;
} RCC_Regs;
#define RCC ((RCC_Regs *) RCC_REGS_START_ADDRESS)

// Later: typedef struct ClockSettings, sysclk_config(ClockSettings *settings)

////////////////////////////////////////////////////////////
//  Systick
////////////////////////////////////////////////////////////

#define SYSTICK_REGS_START_ADDRESS 0xE000E010
#define SYSTICK_APB2ENR_BIT 0

typedef struct {
    volatile uint32_t CSR, RVR, CVR, CALIB;
} Systick_Regs;

void systick_init(uint32_t ticks);

////////////////////////////////////////////////////////////
//  PWR
////////////////////////////////////////////////////////////

#define PWR_REGS_START_ADDRESS 0x40007000
#define PWR_APB1ENR1_BIT 28

typedef struct {
    volatile uint32_t CR[4], SR[2], SCR, RESERVED;
    volatile uint64_t PUCRA_PDCRA[9];
} PWR_Regs;

#define PWR ((PWR_Regs *)(PWR_REGS_START_ADDRESS))

////////////////////////////////////////////////////////////
//  DMA
////////////////////////////////////////////////////////////

#define DMA1_REGS_START_ADDRESS 0x40020000
#define DMA2_REGS_START_ADDRESS 0x40020400

#define DMA1_AHB1ENR_BIT 0
#define DMA2_AHB1ENR_BIT 1
typedef struct {
    volatile uint32_t ISR, IFCR, 
        CCR1, CNDTR1, CPAR1, CMAR1, RESERVED1,
        CCR2, CNDTR2, CPAR2, CMAR2, RESERVED2,
        CCR3, CNDTR3, CPAR3, CMAR3, RESERVED3,
        CCR4, CNDTR4, CPAR4, CMAR4, RESERVED4,
        CCR5, CNDTR5, CPAR5, CMAR5, RESERVED5,
        CCR6, CNDTR6, CPAR6, CMAR6, RESERVED6,
        CCR7, CNDTR7, CPAR7, CMAR7, 
        RESERVED7[6],
        CSELR;

} DMA_Regs;

#define DMA1 ((DMA_Regs *)(DMA1_REGS_START_ADDRESS))
#define DMA2 ((DMA_Regs *)(DMA2_REGS_START_ADDRESS))

#endif