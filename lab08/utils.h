#ifndef UTILS_H_
#define UTILS_H_

#include "stm32l476xx.h"

/**
 * these functions are inside the assembly source file
 */
extern void gpio_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);
extern void delay_1s();
extern void fpu_enable();

/**
 * show data on 7-segment display via max7219_send
 *
 * input:
 *   data: decimal value
 *   num_digs: number of digits to show on 7-segment display
 *
 * return:
 *   0: success
 *   -1: illegal data range (out of 8 digits)
 */
int display(int data, int num_digs)
{
	int show_dec_pt = 0;
	if (num_digs <= -1000)
	{
		num_digs = -1000 - num_digs ;
		show_dec_pt = 1;
	}
	num_digs = num_digs > 8 ? 8 : num_digs;
	int data2 = data, i;
	for (i = 1; i <= num_digs; i++)
	{
		if (data2 < 0 && i == num_digs);
		else if (show_dec_pt && i == 3 && data % 10 < 0)
			max7219_send(i, -data % 10 | 0b10000000);
		else if (show_dec_pt && i == 3)
			max7219_send(i, data % 10 | 0b10000000);
		else if (data % 10 < 0)
			max7219_send(i, -data % 10);
		else
			max7219_send(i, data % 10);
		data /= 10;
	}
	if (data2 < 0)
		max7219_send(num_digs, 10);
	for ( ; i <= 8; i++)
		max7219_send(i, 15);
	return (data > 99999999 || data < -9999999) ? -1 : 0;
}

/**
 * GPIO pin macros
 */
#define GPIO_Pin_0  0b0000000000000001
#define GPIO_Pin_1  0b0000000000000010
#define GPIO_Pin_2  0b0000000000000100
#define GPIO_Pin_3  0b0000000000001000
#define GPIO_Pin_4  0b0000000000010000
#define GPIO_Pin_5  0b0000000000100000
#define GPIO_Pin_6  0b0000000001000000
#define GPIO_Pin_7  0b0000000010000000
#define GPIO_Pin_8  0b0000000100000000
#define GPIO_Pin_9  0b0000001000000000
#define GPIO_Pin_10 0b0000010000000000
#define GPIO_Pin_11 0b0000100000000000
#define GPIO_Pin_12 0b0001000000000000
#define GPIO_Pin_13 0b0010000000000000
#define GPIO_Pin_14 0b0100000000000000
#define GPIO_Pin_15 0b1000000000000000

/**
 * read GPIO data
 *
 * input:
 *   port: pointer to GPIO port structure
 *   pin: GPIO pin macro (see above)
 *
 * return:
 *   0: not set
 *   others: set
 */
int GPIO_ReadInputDataBit(GPIO_TypeDef *port, uint16_t pin) {
	return port->IDR & pin;
}

/**
 * floating point version of display
 */
int displayf(float data, int num_digs)
{
	if (num_digs > 8)
		return display(-1, 2);
	if ((int) (data * 100) % 100)
		return display(data * 100, -1002 - num_digs);
	else
		return display(data, num_digs);
}

/**
 * RCC PLL configuration structure definition
 */
typedef struct
{
	uint32_t PLLState;  // The new state of the PLL
	uint32_t PLLSource; // PLL entry clock source
	uint32_t PLLM;      // Division factor for PLL VCO input clock
	uint32_t PLLN;      // Multiplication factor for PLL VCO output clock
	uint32_t PLLP;      // Division factor for SAI clock
	uint32_t PLLQ;      // PLLQ: Division factor for SDMMC1, RNG and USB clocks
	uint32_t PLLR;      // Division for the main system clock
} RCC_PLLInitTypeDef;

/**
 * RCC Internal/External Oscillator configuration structure definition
 */
typedef struct
{
	uint32_t OscillatorType;      // The oscillators to be configured
	uint32_t HSEState;            // The new state of the HSE
	uint32_t LSEState;            // The new state of the LSE
	uint32_t HSIState;            // The new state of the HSI
	uint32_t HSICalibrationValue; // The calibration trimming value
	uint32_t LSIState;            // The new state of the LSI
	uint32_t MSIState;            // The new state of the MSI
	uint32_t MSICalibrationValue; // The calibration trimming value
	uint32_t MSIClockRange;       // The MSI frequency range
	uint32_t HSI48State;          // The new state of the HSI48
	RCC_PLLInitTypeDef PLL;       // Main PLL structure parameters
} RCC_OscInitTypeDef;

/**
 * RCC System, AHB and APB busses clock configuration structure definition
 */
typedef struct
{
	uint32_t ClockType;      // The clock to be configured
	uint32_t SYSCLKSource;   // The clock source used as system clock (SYSCLK)
	uint32_t AHBCLKDivider;  // The AHB clock (HCLK) divider
	uint32_t APB1CLKDivider; // The APB1 clock (PCLK1) divider
	uint32_t APB2CLKDivider; // The APB2 clock (PCLK2) divider
} RCC_ClkInitTypeDef;

/**
 * calculate length of number
 */
int cal_len(int n)
{
	int sum = 0;
	while (n > 0)
	{
		n /= 10;
		sum++;
	}
	return sum;
}

/**
 * keypad settings, used by keypad_scan
 */
#define XPORT GPIOC
#define YPORT GPIOB
#define X0 GPIO_Pin_0
#define X1 GPIO_Pin_1
#define X2 GPIO_Pin_2
#define X3 GPIO_Pin_3
#define Y0 GPIO_Pin_6
#define Y1 GPIO_Pin_5
#define Y2 GPIO_Pin_4
#define Y3 GPIO_Pin_7

/**
 * initialize keypad GPIO pin, X as output and Y as input
 */
void keypad_init()
{
	RCC->AHB2ENR   |= 0b00000000000000000000000000000110;

	GPIOC->MODER   &= 0b11111111111111111111111100000000;
	GPIOC->MODER   |= 0b00000000000000000000000001010101;
	GPIOC->PUPDR   &= 0b11111111111111111111111100000000;
	GPIOC->PUPDR   |= 0b00000000000000000000000001010101;
	GPIOC->OSPEEDR &= 0b11111111111111111111111100000000;
	GPIOC->OSPEEDR |= 0b00000000000000000000000001010101;
	GPIOC->ODR     |= 0b00000000000000000000000000001111;

	GPIOB->MODER   &= 0b11111111111111110000000011111111;
	GPIOB->PUPDR   &= 0b11111111111111110000000011111111;
	GPIOB->PUPDR   |= 0b00000000000000001010101000000000;
	GPIOB->OSPEEDR &= 0b11111111111111110000000011111111;
	GPIOB->OSPEEDR |= 0b00000000000000000101010100000000;
}

/**
 * scan keypad value
 *
 * return:
 *   >=0: key press value
 *   -1: no key press
 */
signed char keypad_scan()
{
	XPORT->BSRR = X0;
	XPORT->BRR  = X1;
	XPORT->BRR  = X2;
	XPORT->BRR  = X3;

	if (GPIO_ReadInputDataBit(YPORT, Y0))
		return 15;
	if (GPIO_ReadInputDataBit(YPORT, Y1))
		return 7;
	if (GPIO_ReadInputDataBit(YPORT, Y2))
		return 4;
	if (GPIO_ReadInputDataBit(YPORT, Y3))
		return 1;

	XPORT->BRR  = X0;
	XPORT->BSRR = X1;
	XPORT->BRR  = X2;
	XPORT->BRR  = X3;

	if (GPIO_ReadInputDataBit(YPORT, Y0))
		return 0;
	if (GPIO_ReadInputDataBit(YPORT, Y1))
		return 8;
	if (GPIO_ReadInputDataBit(YPORT, Y2))
		return 5;
	if (GPIO_ReadInputDataBit(YPORT, Y3))
		return 2;

	XPORT->BRR  = X0;
	XPORT->BRR  = X1;
	XPORT->BSRR = X2;
	XPORT->BRR  = X3;

	if (GPIO_ReadInputDataBit(YPORT, Y0))
		return 14;
	if (GPIO_ReadInputDataBit(YPORT, Y1))
		return 9;
	if (GPIO_ReadInputDataBit(YPORT, Y2))
		return 6;
	if (GPIO_ReadInputDataBit(YPORT, Y3))
		return 3;

	XPORT->BRR  = X0;
	XPORT->BRR  = X1;
	XPORT->BRR  = X2;
	XPORT->BSRR = X3;

	if (GPIO_ReadInputDataBit(YPORT, Y0))
		return 13;
	if (GPIO_ReadInputDataBit(YPORT, Y1))
		return 12;
	if (GPIO_ReadInputDataBit(YPORT, Y2))
		return 11;
	if (GPIO_ReadInputDataBit(YPORT, Y3))
		return 10;

	return -1;
}

/**
 * NVIC register addresses
 */
#define NVIC_ISER 0xE000E100 // interrupt set-enable registers
#define NVIC_ICPR 0xE000E280 // interrupt clear-pending registers
#define NVIC_IPR  0xE000E400 // interrupt priority registers

/**
 * initialize EXTI interrupts
 */
void exti_init()
{
	// setup SYSCFG
	SYSCFG->EXTICR[1] = SYSCFG_EXTICR2_EXTI4_PB | SYSCFG_EXTICR2_EXTI5_PB | SYSCFG_EXTICR2_EXTI6_PB | SYSCFG_EXTICR2_EXTI7_PB;

	// setup EXTI
	EXTI->IMR1 |= EXTI_IMR1_IM4 | EXTI_IMR1_IM5 | EXTI_IMR1_IM6 | EXTI_IMR1_IM7;
	EXTI->FTSR1 |= EXTI_FTSR1_FT4 | EXTI_FTSR1_FT5 | EXTI_FTSR1_FT6 | EXTI_FTSR1_FT7;
	EXTI->PR1 |= EXTI_PR1_PIF4 | EXTI_PR1_PIF5 | EXTI_PR1_PIF6 | EXTI_PR1_PIF7;

	// enable interrupts
	asm("cpsie i;");

	// setup NVIC
	// EXTI4_IRQn = 10
	// EXTI9_5_IRQn = 23
	uint32_t *ptr;
	ptr = (uint32_t *) NVIC_IPR;
	ptr[2] = 0x00100000;
	ptr[5] = 0x10000000;
	ptr = (uint32_t *) NVIC_ICPR;
	ptr[0] = 0x00000400;
	ptr[0] = 0x00800000;
	ptr = (uint32_t *) NVIC_ISER;
	ptr[0] = 0x00000400;
	ptr[0] = 0x00800000;
}

#endif /* UTILS_H_ */
