#include "stm32l476xx.h"
#include "utils.h"
#define TIME_SEC 11.99

int cal_len(int a)
{
	int sum = 0;
	while (a > 0)
	{
		a /= 10;
		sum++;
	}
	return sum;
}

void timer_init()
{
	RCC->APB1ENR1 |= 0b1;
	TIM2->ARR = (uint32_t) (TIME_SEC * (4000000 / 40000)); // reload value
	TIM2->PSC = (uint32_t) 39999; // prescaler
	TIM2->EGR = TIM_EGR_UG; // reinitialize the counter
}

void timer_start()
{
	TIM2->CR1 |= TIM_CR1_CEN;
	display(0, -1003);
	if (TIME_SEC <= 0 || TIME_SEC > 10000)
	{
		TIM2->CR1 &= ~TIM_CR1_CEN;
		return;
	}
	int pre_val = 0;
	while (1)
	{
		int now_val = TIM2->CNT;
		if (pre_val > now_val)
		{
			TIM2->CR1 &= ~TIM_CR1_CEN;
			return;
		}
		pre_val = now_val;
		int len = cal_len(now_val);
		if (now_val < 100)
			len = 3;
		display(now_val, -1000 - len);
	}
}

int main()
{
	gpio_init();
	max7219_init();
	timer_init();
	timer_start();
}
