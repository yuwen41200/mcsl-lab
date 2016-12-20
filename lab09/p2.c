#include "stm32l476xx.h"
// I cannot use TM Libraries! TMD!
#include "libtmd.h"

const GPIO_TypeDef *LCD_DATA_PORT[8] = {
	GPIOB,
	GPIOB,
	GPIOB,
	GPIOB,
	GPIOB,
	GPIOB,
	GPIOB,
	GPIOB
};
const GPIO_TypeDef *LCD_RS_PORT = GPIOA;
const GPIO_TypeDef *LCD_RW_PORT = GPIOA;
const GPIO_TypeDef *LCD_EN_PORT = GPIOA;

const uint16_t LCD_DATA_PIN[8] = {
	GPIO_PIN_0,
	GPIO_PIN_1,
	GPIO_PIN_2,
	GPIO_PIN_3,
	GPIO_PIN_4,
	GPIO_PIN_5,
	GPIO_PIN_6,
	GPIO_PIN_7
};
const uint16_t LCD_RS_PIN = GPIO_PIN_5;
const uint16_t LCD_RW_PIN = GPIO_PIN_6;
const uint16_t LCD_EN_PIN = GPIO_PIN_7;

const int map_four[8] = {
	0x11,
	0x11,
	0x11,
	0x1F,
	0x01,
	0x01,
	0x01,
	0x00
};

const int map_five[8] = {
	0x1F,
	0x10,
	0x10,
	0x1F,
	0x01,
	0x01,
	0x1F,
	0x00
};

void SysTick_UserConfig(float);
void SysTick_Handler();
void init();
void init_lcd();
void write_to_lcd(int, int);
void create_font(int, const int *);

int main() {
	fpu_enable();
	init();
	SysTick_UserConfig(0.3);
	while (1);
	return 0;
}

void SysTick_UserConfig(float n) {
	SysTick->CTRL |= 0x00000004;
	SysTick->LOAD = (uint32_t) (n * 4000000.0);
	SysTick->VAL = 0;
	SysTick->CTRL |= 0x00000003;
}

void SysTick_Handler() {
	write_to_lcd(0x10, 1); // shift cursor
	write_to_lcd(0x10, 1); // shift cursor
	write_to_lcd(0x20, 0); // print ' '
	write_to_lcd(0x34, 0); // print '4'
	write_to_lcd(0x35, 0); // print '5'
}

void init() {
	TMD_GPIO_Init();
	init_lcd();
	create_font(0, map_four);
	create_font(8, map_five);
	write_to_lcd(0x80, 1); // move to top left
}

void init_lcd() {
	write_to_lcd(0x38, 1); // function setting
	write_to_lcd(0x06, 1); // entry mode
	write_to_lcd(0x0C, 1); // display on
	write_to_lcd(0x01, 1); // clear screen
	write_to_lcd(0x80, 1); // move to top left
}

void write_to_lcd(int input, int is_cmd) {
	if (is_cmd)
		TMD_GPIO_SetPinLow(LCD_RS_PORT, LCD_RS_PIN);
	else
		TMD_GPIO_SetPinHigh(LCD_RS_PORT, LCD_RS_PIN);

	TMD_GPIO_SetPinLow(LCD_RW_PORT, LCD_RW_PIN);

	for (int i = 0; i < 8; ++i) {
		if (input & (1 << i))
			TMD_GPIO_SetPinHigh(LCD_DATA_PORT[i], LCD_DATA_PIN[i]);
		else
			TMD_GPIO_SetPinLow(LCD_DATA_PORT[i], LCD_DATA_PIN[i]);
	}

	TMD_GPIO_SetPinHigh(LCD_EN_PORT, LCD_EN_PIN);
	delay_ms(10);
	TMD_GPIO_SetPinLow(LCD_EN_PORT, LCD_EN_PIN);
	delay_ms(10);
}

void create_font(int location, const int *font_array) {
	write_to_lcd(location & 0x3F | 0x40, 1);
	for (int i = 0; i < 8; ++i)
		write_to_lcd(font_array[i] & 0x1F, 0);
}
