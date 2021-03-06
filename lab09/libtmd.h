#ifndef LIBTMD_H
#define LIBTMD_H

extern void delay_ms(unsigned);
extern void fpu_enable();

#include "stm32l476xx.h"

#ifndef GPIO_PIN_0
#define GPIO_PIN_0   ((uint16_t) 0x0001)
#define GPIO_PIN_1   ((uint16_t) 0x0002)
#define GPIO_PIN_2   ((uint16_t) 0x0004)
#define GPIO_PIN_3   ((uint16_t) 0x0008)
#define GPIO_PIN_4   ((uint16_t) 0x0010)
#define GPIO_PIN_5   ((uint16_t) 0x0020)
#define GPIO_PIN_6   ((uint16_t) 0x0040)
#define GPIO_PIN_7   ((uint16_t) 0x0080)
#define GPIO_PIN_8   ((uint16_t) 0x0100)
#define GPIO_PIN_9   ((uint16_t) 0x0200)
#define GPIO_PIN_10  ((uint16_t) 0x0400)
#define GPIO_PIN_11  ((uint16_t) 0x0800)
#define GPIO_PIN_12  ((uint16_t) 0x1000)
#define GPIO_PIN_13  ((uint16_t) 0x2000)
#define GPIO_PIN_14  ((uint16_t) 0x4000)
#define GPIO_PIN_15  ((uint16_t) 0x8000)
#define GPIO_PIN_ALL ((uint16_t) 0xFFFF)
#endif

void TMD_GPIO_Init() {
	RCC->AHB2ENR   |= 0b00000000000000000000000000000111;

	GPIOA->MODER   &= 0b11111111111111110000001111111111;
	GPIOA->MODER   |= 0b00000000000000000101010000000000;
	GPIOA->PUPDR   &= 0b11111111111111110000001111111111;
	GPIOA->PUPDR   |= 0b00000000000000000101010000000000;
	GPIOA->OSPEEDR &= 0b11111111111111110000001111111111;
	GPIOA->OSPEEDR |= 0b00000000000000000101010000000000;
	GPIOA->OTYPER  &= 0b11111111111111111111111100011111;

	GPIOB->MODER   &= 0b11111111111111110000000000000000;
	GPIOB->MODER   |= 0b00000000000000000101010101010101;
	GPIOB->PUPDR   &= 0b11111111111111110000000000000000;
	GPIOB->PUPDR   |= 0b00000000000000000101010101010101;
	GPIOB->OSPEEDR &= 0b11111111111111110000000000000000;
	GPIOB->OSPEEDR |= 0b00000000000000000101010101010101;
	GPIOB->OTYPER  &= 0b11111111111111111111111100000000;

	GPIOC->MODER   &= 0b11110011111111111111111111111111;
}

void TMD_GPIO_SetPinLow(GPIO_TypeDef *GPIOX, uint16_t GPIO_PIN_Y) {
	GPIOX->BRR = GPIO_PIN_Y;
}

void TMD_GPIO_SetPinHigh(GPIO_TypeDef *GPIOX, uint16_t GPIO_PIN_Y) {
	GPIOX->BSRR = GPIO_PIN_Y;
}

void usleep(unsigned delay) {
	RCC->APB1ENR1 |= 0b1;
	if (delay == 0)
		TIM2->ARR = 2;
	else
		TIM2->ARR = delay;
	TIM2->PSC = (uint32_t) 3;
	TIM2->EGR = TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
	int pre_val = 0;
	while (1) {
		int now_val = TIM2->CNT;
		if (pre_val > now_val) {
			TIM2->CR1 &= ~TIM_CR1_CEN;
			return;
		}
		pre_val = now_val;
	}
}

#endif
