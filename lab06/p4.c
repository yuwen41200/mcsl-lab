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
int total, len;
char set[14];
int rem = 0;
// int error_flag=1;

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

signed int stack_num[100];
signed int stack_ope[100];
int top_num = -1;
int top_ope = -1;

int main()
{
	gpio_init();
	max7219_init();
	keypad_init();
	total = 0;
	len = 0;
	int input = -1, prev_input = -1;
	set_clear();
	display(0, 0);
	while (1)
	{
		prev_input = input;
		input = keypad_scan();
		if(input == 0)
			rem = 1;
		if (input == prev_input);
		else if (input == 14) // Clear
		{
			for(int i = 0; i <= 99; i++)
			{
				stack_num[i] = 0;
				stack_ope[i] = 0;
			}
			top_num = -1;
			top_ope = -1;
			total = 0;
			len = 0;
			set_clear();
			display(total, len);
		}
		else if(input == 15) // =
		{
			top_num++;
			stack_num[top_num] = total;
			while (top_ope != -1)
			{
				int operator;
				operator = stack_ope[top_ope];
				top_ope--;
				if (operator == 10) // +
				{
					int temp1, temp2;
					temp2 = stack_num[top_num];
					top_num--;
					temp1 = stack_num[top_num];
					// top_num--;
					// top_num++;
					stack_num[top_num] = temp1 + temp2;
				}
				else if (operator == 11) // -
				{
					int temp1, temp2;
					temp2 = stack_num[top_num];
					top_num--;
					temp1 = stack_num[top_num];
					// top_num--;
					// top_num++;
					stack_num[top_num] = temp1 - temp2;
				}
				else if (operator == 12) // *
				{
					int temp1, temp2;
					temp2 = stack_num[top_num];
					top_num--;
					temp1 = stack_num[top_num];
					// top_num--;
					// top_num++;
					stack_num[top_num] = temp1 * temp2;
				}
				else if (operator == 13) // /
				{
					int temp1, temp2;
					temp2 = stack_num[top_num];
					top_num--;
					temp1 = stack_num[top_num];
					// top_num--;
					// top_num++;
					stack_num[top_num] = temp1 / temp2;
				}
			}
			total = stack_num[top_num];
			// if (error_flag == 1)
			// {
			//     total=-1;
			// }
		}
		else if (input == 10) // +
		{
			top_num++;
			stack_num[top_num] = total;
			total = 0;
			len = 0;
			while (stack_ope[top_ope] == 12) // *
			{
				int temp1, temp2;
				temp2 = stack_num[top_num];
				top_num--;
				temp1 = stack_num[top_num];
				// top_num--;
				// top_num++;
				top_ope--;
				stack_num[top_num] = temp1 * temp2;
			}
			while (stack_ope[top_ope] == 13) // /
			{
				int temp1, temp2;
				temp2 = stack_num[top_num];
				top_num--;
				temp1 = stack_num[top_num];
				// top_num--;
				// top_num++;
				top_ope--;
				stack_num[top_num] = temp1 / temp2;
			}
			top_ope++;
			stack_ope[top_ope] = 10; // +
		}
		else if (input == 11) // -
		{
			top_num++;
			stack_num[top_num] = total;
			total = 0;
			len = 0;
			while (stack_ope[top_ope] == 12) // *
			{
				int temp1, temp2;
				temp2 = stack_num[top_num];
				top_num--;
				temp1 = stack_num[top_num];
				// top_num--;
				// top_num++;
				top_ope--;
				stack_num[top_num] = temp1 * temp2;
			}
			while (stack_ope[top_ope] == 13) // /
			{
				int temp1, temp2;
				temp2 = stack_num[top_num];
				top_num--;
				temp1 = stack_num[top_num];
				// top_num--;
				// top_num++;
				top_ope--;
				stack_num[top_num] = temp1 / temp2;
			}
			top_ope++;
			stack_ope[top_ope] = 11; // -
		}
		else if (input == 12) // *
		{
			top_num++;
			top_ope++;
			stack_num[top_num] = total;
			total = 0;
			len = 0;
			stack_ope[top_ope] = 12; // *
		}
		else if (input == 13) // /
		{
			top_num++;
			top_ope++;
			stack_num[top_num] = total;
			total = 0;
			len = 0;
			stack_ope[top_ope] = 13; // /
		}
		else if (input != -1)
		{
			set_insert(input);
		}
		else
		{
			input = set_reduce();
			set_clear();
			if (input >= 10 && len + 2 <= 3)
			{
				total = total * 100 + input;
				len += 2;
			}
			else if (input < 10 && input >= 0 && len + 1 <= 3)
			{
				if(input == 0 && rem == 0);
				else
				{
					total = total * 10 + input;
					len += 1;
					rem = 0;
				}
			}
			if (total > 0)
				len = cal_len(total);
			else if (total < 0)
				len = cal_len(-total) + 1;
			display(total, len);
		}
	}
}
