#include "stm32l476xx.h"
#include "utils.h"

int scan_state = 0, key_value = 0;

void SysTick_UserConfig();
void SysTick_Handler();
void EXTI4_IRQHandler();
void EXTI9_5_IRQHandler();

int main()
{
	SysTick_UserConfig();
	gpio_init();
	exti_init();
	while (1)
		display(key_value, 2);
}

void SysTick_UserConfig()
{
	SysTick->CTRL |= 0x00000004;
	SysTick->LOAD = 100000; // 0.1 second
	SysTick->VAL = 0;
	SysTick->CTRL |= 0x00000003;
}

void SysTick_Handler()
{
	EXTI->IMR1 ^= EXTI_IMR1_IM4 | EXTI_IMR1_IM5 | EXTI_IMR1_IM6 | EXTI_IMR1_IM7;
	scan_state = scan_state == 3 ? 0 : scan_state + 1;
	switch (scan_state)
	{
	case 0:
		XPORT->BSRR = X0;
		XPORT->BRR  = X1;
		XPORT->BRR  = X2;
		XPORT->BRR  = X3;
		break;
	case 1:
		XPORT->BRR  = X0;
		XPORT->BSRR = X1;
		XPORT->BRR  = X2;
		XPORT->BRR  = X3;
		break;
	case 2:
		XPORT->BRR  = X0;
		XPORT->BRR  = X1;
		XPORT->BSRR = X2;
		XPORT->BRR  = X3;
		break;
	case 3:
		XPORT->BRR  = X0;
		XPORT->BRR  = X1;
		XPORT->BRR  = X2;
		XPORT->BSRR = X3;
		break;
	}
	EXTI->IMR1 |= EXTI_IMR1_IM4 | EXTI_IMR1_IM5 | EXTI_IMR1_IM6 | EXTI_IMR1_IM7;
}

void EXTI4_IRQHandler()
{
	switch (scan_state)
	{
	case 0:
		if (GPIO_ReadInputDataBit(YPORT, Y0))
			key_value = 15;
		if (GPIO_ReadInputDataBit(YPORT, Y1))
			key_value = 7;
		if (GPIO_ReadInputDataBit(YPORT, Y2))
			key_value = 4;
		if (GPIO_ReadInputDataBit(YPORT, Y3))
			key_value = 1;
		break;
	case 1:
		if (GPIO_ReadInputDataBit(YPORT, Y0))
			key_value = 0;
		if (GPIO_ReadInputDataBit(YPORT, Y1))
			key_value = 8;
		if (GPIO_ReadInputDataBit(YPORT, Y2))
			key_value = 5;
		if (GPIO_ReadInputDataBit(YPORT, Y3))
			key_value = 2;
		break;
	case 2:
		if (GPIO_ReadInputDataBit(YPORT, Y0))
			key_value = 14;
		if (GPIO_ReadInputDataBit(YPORT, Y1))
			key_value = 9;
		if (GPIO_ReadInputDataBit(YPORT, Y2))
			key_value = 6;
		if (GPIO_ReadInputDataBit(YPORT, Y3))
			key_value = 3;
		break;
	case 3:
		if (GPIO_ReadInputDataBit(YPORT, Y0))
			key_value = 13;
		if (GPIO_ReadInputDataBit(YPORT, Y1))
			key_value = 12;
		if (GPIO_ReadInputDataBit(YPORT, Y2))
			key_value = 11;
		if (GPIO_ReadInputDataBit(YPORT, Y3))
			key_value = 10;
		break;
	}
}

void EXTI9_5_IRQHandler()
{
	switch (scan_state)
	{
	case 0:
		if (GPIO_ReadInputDataBit(YPORT, Y0))
			key_value = 15;
		if (GPIO_ReadInputDataBit(YPORT, Y1))
			key_value = 7;
		if (GPIO_ReadInputDataBit(YPORT, Y2))
			key_value = 4;
		if (GPIO_ReadInputDataBit(YPORT, Y3))
			key_value = 1;
		break;
	case 1:
		if (GPIO_ReadInputDataBit(YPORT, Y0))
			key_value = 0;
		if (GPIO_ReadInputDataBit(YPORT, Y1))
			key_value = 8;
		if (GPIO_ReadInputDataBit(YPORT, Y2))
			key_value = 5;
		if (GPIO_ReadInputDataBit(YPORT, Y3))
			key_value = 2;
		break;
	case 2:
		if (GPIO_ReadInputDataBit(YPORT, Y0))
			key_value = 14;
		if (GPIO_ReadInputDataBit(YPORT, Y1))
			key_value = 9;
		if (GPIO_ReadInputDataBit(YPORT, Y2))
			key_value = 6;
		if (GPIO_ReadInputDataBit(YPORT, Y3))
			key_value = 3;
		break;
	case 3:
		if (GPIO_ReadInputDataBit(YPORT, Y0))
			key_value = 13;
		if (GPIO_ReadInputDataBit(YPORT, Y1))
			key_value = 12;
		if (GPIO_ReadInputDataBit(YPORT, Y2))
			key_value = 11;
		if (GPIO_ReadInputDataBit(YPORT, Y3))
			key_value = 10;
		break;
	}
}
