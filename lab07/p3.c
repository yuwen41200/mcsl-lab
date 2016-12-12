#include "stm32l476xx.h"
#include "utils.h"

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

void keypad_ctrl()
{
	while (1)
	{
		prev = curr;
		curr = keypad_scan();
		if (curr == prev)
			check = 86400;
		else
			check = curr;
		switch (check)
		{
		case 1:
			freq = DO;
			timer_config();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 2:
			freq = RE;
			timer_config();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 3:
			freq = MI;
			timer_config();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 4:
			freq = FA;
			timer_config();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 5:
			freq = SO;
			timer_config();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 6:
			freq = LA;
			timer_config();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 7:
			freq = SI;
			timer_config();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 8:
			freq = HI_DO;
			timer_config();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 10:
			duty_cycle = duty_cycle == 90 ? duty_cycle : duty_cycle + 5;
			break;
		case 11:
			duty_cycle = duty_cycle == 10 ? duty_cycle : duty_cycle - 5;
			break;
		case 86400:
			break;
		default:
			TIM2->CR1 &= ~TIM_CR1_CEN;
			freq = -1;
			break;
		}
	}
}

int main()
{
	fpu_enable();
	gpio_init();
	keypad_init();
	timer_init();
	keypad_ctrl();
}
