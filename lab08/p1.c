#include "stm32l476xx.h"
#include "utils.h"

int plln = 40, pllm = 7, prescaler = 0; // 10 MHz SYSCLK
int prev_btn = 1, curr_btn = 1;

void SystemClock_Config();
void SysTick_Config();
void SysTick_Handler();

int main()
{
	SystemClock_Config();
	SysTick_Config();
	gpio_init();
	while (1)
	{
		if (!prev_btn && curr_btn)
			SysTick->CTRL = (SysTick->CTRL & 0xFFFFFFFE) | ~(SysTick->CTRL & 0x00000001);
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

void SysTick_Config()
{
	SysTick->CTRL |= 0x00000004;
	SysTick->LOAD = 5000000; // 0.5 second
	SysTick->VAL = 0;
	SysTick->CTRL |= 0x00000003;
}

void SysTick_Handler()
{
	GPIOA->ODR = (GPIOA->ODR & 0xFFFFFFDF) | ~(GPIOA->ODR & 0x00000020);
}
