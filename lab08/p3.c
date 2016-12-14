#include "stm32l476xx.h"
#include "utils.h"

#define XPORT GPIOC
#define YPORT GPIOB
#define X0 GPIO_Pin_0
#define X1 GPIO_Pin_1
#define X2 GPIO_Pin_2
#define X3 GPIO_Pin_3
#define Y0 GPIO_Pin_6
#define Y1 GPIO_Pin_5
#define Y2 GPIO_Pin_4
#define Y3 GPIO_Pin_3

#define DO 261.6
#define RE 293.7
#define MI 329.6
#define FA 349.2
#define SO 392.0
#define LA 440.0
#define SI 493.9
#define HI_DO 523.3

float freq = -1;
int curr = -2, prev = -3, check = -4;
int duty_cycle = 50;
unsigned int x_pin = {X0, X1, X2, X3};
unsigned int y_pin = {Y0, Y1, Y2, Y3};
unsigned int total, len;
char set[14];
int rem = 0;
int plln = 40, pllm = 7, prescaler = 0; // 10 MHz SYSCLK
int prev_btn = 1, curr_btn = 1;
int state = 0;

void timer_init()
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	// enable TIM2 timer clock
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL3_0;
	// select AF1 for PB3 (PB3 is TIM2_CH2)
	TIM2->CR1 |= TIM_CR1_DIR;
	// counter used as downcounter
	TIM2->CR1 |= TIM_CR1_ARPE;
	// enable auto-reload preload (buffer TIM2_ARR)
	TIM2->ARR = (uint32_t) 100;
	// auto-reload prescaler value
	TIM2->CCMR1 &= 0xFFFFFCFF;
	// select compare 2 (channel 2 is configured as output)
	TIM2->CCMR1 |= (TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);
	// set output compare 2 mode to PWM mode 1
	TIM2->CCMR1 |= TIM_CCMR1_OC2PE;
	// enable output compare 2 preload register on TIM2_CCR2
	TIM2->CCER |= TIM_CCER_CC2E;
	// enable compare 2 output
	TIM2->EGR = TIM_EGR_UG;
	// re-initialize the counter and generates an update of the registers
}

void timer_config()
{
	TIM2->PSC = (uint32_t) (4000000 / freq / 100);
	// prescaler value
	TIM2->CCR2 = duty_cycle;
	// compare 2 preload value
}

void start_systick_timer()
{
	SysTick->CTRL |= 0x00000001;
	state = 1;
}

void stop_systick_timer()
{
	SysTick->CTRL &= 0xFFFFFFFE;
	state = 0;
}

#define SHPR3 0xE000ED20

void SysTick_UserConfig()
{
	SysTick->CTRL |= 0x00000004;
	SysTick->LOAD = 10000000; // 1.0 second
	SysTick->VAL = 0;
	SysTick->CTRL |= 0x00000002;
	uint32_t *ptr;
	ptr = (uint32_t *) SHPR3;
	*ptr = *ptr | 0xF0000000;
}

void SysTick_Handler()
{
	total = total == 0 ? total : total - 1;
	int temp = total == 0 ? 1 : cal_len(total);
	display(total, temp);
	if (total == 0)
	{
		freq = DO;
		timer_config();
		TIM2->CR1 |= TIM_CR1_CEN;
		while (state == 1);
		TIM2->CR1 &= ~TIM_CR1_CEN;
		freq = -1;
		total = 0;
		len = 0;
		rem = 0;
	}
}

void set_clear()
{
	for (int i = 0; i < 14; i++)
		set[i] = 0;
}

void set_insert(int i)
{
	if (i >= 0 && i < 14)
		set[i] = 1;
}

int set_reduce()
{
	int sum = 0;
	for (int i = 0; i < 14; i++)
		if (set[i])
			sum += i;
	return sum;
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
	// f(VCO clock) = f(PLL clock input) กั (PLLN / PLLM)
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

void exti_init2()
{
	// setup SYSCFG
	SYSCFG->EXTICR[3] = SYSCFG_EXTICR4_EXTI13_PC;

	// setup EXTI
	EXTI->IMR1 |= EXTI_IMR1_IM13;
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;
	EXTI->PR1 |= EXTI_PR1_PIF13;

	// enable interrupts
	asm("cpsie i;");

	// setup NVIC
	// EXTI15_10_IRQn = 40
	uint32_t *ptr;
	ptr = (uint32_t *) NVIC_IPR;
	ptr[10] = 0x00000010;
	ptr = (uint32_t *) NVIC_ICPR;
	ptr[1] = 0x00000100;
	ptr = (uint32_t *) NVIC_ISER;
	ptr[1] = 0x00000100;
}

void EXTI15_10_IRQHandler()
{
	uint32_t *ptr;
	ptr = (uint32_t *) NVIC_ICPR;
	ptr[1] = 0x00000100;
	EXTI->PR1 |= EXTI_PR1_PIF13;
	while (1)
	{
		prev_btn = curr_btn;
		curr_btn = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13);
		if (state == 0 && !prev_btn && curr_btn)
		{
			if (total == 0)
				break;
			start_systick_timer();
			break;
		}
		else if (state == 1 && !prev_btn && curr_btn)
		{
			stop_systick_timer();
			break;
		}
	}
}

int main()
{
	fpu_enable();
	SystemClock_Config();
	SysTick_UserConfig();
	gpio_init();
	max7219_init();
	keypad_init();
	timer_init();
	exti_init2();
	TIM2->CR1 &= ~TIM_CR1_CEN;
	freq = -1;
	total = 0;
	len = 0;
	int cnt = 0;
	int input = -1, prev_input = -1;
	set_clear();
	while (1)
	{
		if (state == 0)
		{
			prev_input = input;
			input = keypad_scan();
			if (input == 0)
				rem = 1;
			if (input == prev_input)
			{
				cnt++;
				if (cnt > 12000)
				{
					cnt = 0;
					if (input < 14)
					goto A;
				}
			}
			else if (input >= 14)
			{
				total = 0;
				len = 0;
				set_clear();
				display(total, len);
			}
			else if (input != -1)
				set_insert(input);
			else
			{
				input = set_reduce();
				A:
				set_clear();
				if (input >= 10 && len + 2 <= 8)
				{
					total = total * 100 + input;
					len += 2;
				}
				else if (input < 10 && input >= 0 && len + 1 <= 8)
				{
					if (input == 0 && rem == 0);
					else
					{
						total = total * 10 + input;
						if (total == 0 && rem == 1);
						else
							len += 1;
						rem = 0;
					}
				}
				display(total, len);
			}
		}
	}
}
