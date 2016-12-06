#include "stm32l476xx.h"
#include "utils.h"

int plln = 16, pllm = 7, prescaler = 9;
enum {S1MHZ, S6MHz, S10MHZ, S16MHZ, S40MHZ} state = S1MHZ;
int prev_btn = 1, curr_btn = 1;

void SystemClock_Config();

int main()
{
	SystemClock_Config();
	gpio_init();
	while (1)
	{
		if (!prev_btn && curr_btn)
		{
			switch (state)
			{
			case S1MHZ:
				plln = 16;
				pllm = 7;
				prescaler = 9;
				break;
			case S6MHz:
				plln = 24;
				pllm = 7;
				prescaler = 0;
				break;
			case S10MHZ:
				plln = 40;
				pllm = 7;
				prescaler = 0;
				break;
			case S16MHZ:
				plln = 64;
				pllm = 7;
				prescaler = 0;
				break;
			case S40MHZ:
				plln = 20;
				pllm = 0;
				prescaler = 0;
				break;
			default:
				break;
			}
			SystemClock_Config();
			state = state == S40MHZ ? S1MHZ : state + 1;
		}
		GPIOA->BSRR = (1 << 5);
		delay_1s();
		GPIOA->BRR = (1 << 5);
		delay_1s();
		prev_btn = curr_btn;
		curr_btn = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
	}
}

void SystemClock_Config()
{
	RCC->CFGR = 0x00000000;
	// CFGR reset value
	RCC->CR &= 0xFEFFFFFF;
	// main PLL enable: PLL off
	while (RCC->CR & 0x02000000);
	// main PLL clock ready flag: PLL locked
	RCC->PLLCFGR = 0x01000001;
	// main PLL PLLCLK output enable: PLLCLK output enable
	// main PLL entry clock source: MSI clock selected as PLL clock entry
	RCC->PLLCFGR |= plln << 8;
	// main PLL multiplication factor for VCO
	RCC->PLLCFGR |= pllm << 4;
	// division factor for the main PLL input clock
	// f(VCO clock) = f(PLL clock input) × (PLLN / PLLM)
	// f(PLL_R) = f(VCO clock) / PLLR
	RCC->CR |= 0x01000000;
	// main PLL enable: PLL on
	while (!(RCC->CR & 0x02000000));
	// main PLL clock ready flag: PLL locked
	RCC->CFGR = 0x00000003;
	// system clock switch: PLL selected as system clock
	RCC->CFGR |= prescaler << 4;
	// AHB prescaler: SYSCLK divided by N
}
