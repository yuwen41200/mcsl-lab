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

unsigned int x_pin = {X0, X1, X2, X3};
unsigned int y_pin = {Y0, Y1, Y2, Y3};
unsigned int total, len;
char set[14];

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

// initial keypad gpio pin, X as output and Y as input
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

	GPIOB->MODER   &= 0b11111111111111111100000000111111;
	GPIOB->PUPDR   &= 0b11111111111111111100000000111111;
	GPIOB->PUPDR   |= 0b00000000000000000010101010000000;
	GPIOB->OSPEEDR &= 0b11111111111111111111111100000000;
	GPIOB->OSPEEDR |= 0b00000000000000000000000001010101;
}

/**
 * scan keypad value
 *
 * return:
 *   >=0: key pressed value
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

int main()
{
	gpio_init();
	max7219_init();
	keypad_init();

	total = 0;
	len = 0;
	int input = -1, prev_input = -1;
	set_clear();

	while (1)
	{
		prev_input = input;
		input = keypad_scan();
		if (input == prev_input);
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
			set_clear();
			if (input >= 10 && len + 2 <= 8)
			{
				total = total * 100 + input;
				len += 2;
			}
			else if (input < 10 && input >= 0 && len + 1 <= 8)
			{
				total = total * 10 + input;
				len += 1;
			}
			display(total, len);
		}
	}
}
