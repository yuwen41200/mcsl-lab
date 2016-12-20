#include "libtmd.h"
#include "ds18b20.h"

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

const char *test_string = "Test: E=m*c^2";
const unsigned resolution = 11;

void SysTick_UserConfig(float);
void SysTick_Handler();
void init();
void init_lcd();
void write_to_lcd(int, int);
void create_font(int, const int *);
void write_str_to_lcd(char *);
void write_int_to_lcd(int16_t);

int counter = 0, mode = 0, position = 0;

int main() {
	int prev_btn = 1, curr_btn = 1;
	fpu_enable();
	init();
	set_resolution(resolution);
	SysTick_UserConfig(0.3);
	while (1) {
		if (!prev_btn && curr_btn) {
			mode ^= 1;
			position = 0;
			counter = 0;
			SysTick->CTRL &= 0xFFFFFFF8;
			init();
			if (mode == 0)
				SysTick_UserConfig(0.3);
			else
				SysTick_UserConfig(1);
		}
		prev_btn = curr_btn;
		curr_btn = GPIOC->IDR & GPIO_PIN_13;
	}
	return 0;
}

void SysTick_UserConfig(float n) {
	SysTick->CTRL |= 0x00000004;
	SysTick->LOAD = (uint32_t) (n * 4000000.0);
	SysTick->VAL = 0;
	SysTick->CTRL |= 0x00000003;
}

void SysTick_Handler() {
	if (mode == 0) {
		counter = counter + 1;
		if (counter == 18) {
			write_to_lcd(0x80 + 0x0F, 1);
			write_to_lcd(0x20, 0); // print ' '
			write_to_lcd(0x20, 0); // print ' '
			write_to_lcd(0x80 + 0x41, 1);
		}
		if (counter == 34) {
			write_to_lcd(0x80 + 0x4F, 1);
			write_to_lcd(0x20, 0); // print ' '
			write_to_lcd(0x20, 0); // print ' '
			write_to_lcd(0x80 + 0x1, 1);
			counter = 2;
		}
		write_to_lcd(0x10, 1); // shift cursor
		write_to_lcd(0x10, 1); // shift cursor
		write_to_lcd(0x20, 0); // print ' '
		write_to_lcd(0x00, 0); // print '4'
		write_to_lcd(0x01, 0); // print '5'
		if (counter == 17) {
			write_to_lcd(0x80 + 0x40, 1);
			write_to_lcd(0x01, 0); // print '5'
			write_to_lcd(0x80 + 0x0F, 1);
		}
		if (counter == 33) {
			write_to_lcd(0x80 + 0x0, 1);
			write_to_lcd(0x01, 0); // print '5'
			write_to_lcd(0x80 + 0x4F, 1);
		}
	}
	else {
		SysTick->CTRL &= 0xFFFFFFFE;
		write_int_to_lcd(get_temperature());
		SysTick->CTRL |= 0x00000001;
	}
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

void write_str_to_lcd(char *str) {
	if (str[position] == 0) {
		position = 0;
		counter = 0;
		SysTick->CTRL &= 0xFFFFFFFE;
		init();
		SysTick->CTRL |= 0x00000001;
	}
	write_to_lcd(str[position], 0);
	position++;
}

void write_int_to_lcd(int16_t in) {
	switch (resolution) {
		case 12:
			in &= 0xFFFF;
			break;
		case 11:
			in &= 0xFFFE;
			break;
		case 10:
			in &= 0xFFFC;
			break;
		case 9:
			in &= 0xFFF8;
			break;
		default:
			break;
	}
	int16_t in1 = in >> 4;
	int16_t in2 = ((in & 0x0001) * 0.0625 + (in & 0x0002) * 0.125 + \
	               (in & 0x0004) * 0.25 + (in & 0x0008) * 0.5) * 1000;
	init();
	write_to_lcd(0x30, 0);
	write_to_lcd(0x30, 0);
	write_to_lcd(0x2E, 0);
	write_to_lcd(0x30, 0);
	write_to_lcd(0x30, 0);
	write_to_lcd(0x30, 0);
	write_to_lcd(0x30, 0);
	write_to_lcd(0x10, 1);
	write_to_lcd(0x30 + in2 % 10, 0);
	in2 /= 10;
	write_to_lcd(0x10, 1);
	write_to_lcd(0x30 + in2 % 10, 0);
	in2 /= 10;
	write_to_lcd(0x10, 1);
	write_to_lcd(0x30 + in2 % 10, 0);
	in2 /= 10;
	write_to_lcd(0x10, 1);
	write_to_lcd(0x30 + in2 % 10, 0);
	in2 /= 10;
	write_to_lcd(0x10, 1);
	write_to_lcd(0x10, 1);
	write_to_lcd(0x30 + in1 % 10, 0);
	in1 /= 10;
	write_to_lcd(0x10, 1);
	write_to_lcd(0x30 + in1 % 10, 0);
	in1 /= 10;
}
