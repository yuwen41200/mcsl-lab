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

void timer_init()
{
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->ARR = (uint32_t) 40000;
	TIM2->PSC = (uint32_t) (4000000 / freq / 2);
	TIM2->EGR = TIM_EGR_UG;
}

void timer_start()
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
			TIM2->CR1 &= ~TIM_CR1_CEN;
			freq = DO;
			timer_init();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 2:
			TIM2->CR1 &= ~TIM_CR1_CEN;
			freq = RE;
			timer_init();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 3:
			TIM2->CR1 &= ~TIM_CR1_CEN;
			freq = MI;
			timer_init();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 4:
			TIM2->CR1 &= ~TIM_CR1_CEN;
			freq = FA;
			timer_init();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 5:
			TIM2->CR1 &= ~TIM_CR1_CEN;
			freq = SO;
			timer_init();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 6:
			TIM2->CR1 &= ~TIM_CR1_CEN;
			freq = LA;
			timer_init();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 7:
			TIM2->CR1 &= ~TIM_CR1_CEN;
			freq = SI;
			timer_init();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 8:
			TIM2->CR1 &= ~TIM_CR1_CEN;
			freq = HI_DO;
			timer_init();
			TIM2->CR1 |= TIM_CR1_CEN;
			break;
		case 86400:
			break;
		default:
			TIM2->CR1 &= ~TIM_CR1_CEN;
			freq = -1;
			break;
		}
		if (freq > 0)
		{
			if (TIM2->CNT % 2)
				GPIOB->BSRR = (1 << 8);
			else
				GPIOB->BRR = (1 << 8);
		}
		else
			GPIOB->BRR = (1 << 8);
	}
}

int main()
{
	fpu_enable();
	gpio_init();
	keypad_init();
	timer_start();
}
