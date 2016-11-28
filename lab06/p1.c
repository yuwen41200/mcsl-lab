// these functions are inside the asm file
extern void gpio_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);

/**
 * show data on 7-seg via max7219_send
 *
 * input:
 *   data: decimal value
 *   num_digs: number of digits to show on 7-seg
 *
 * return:
 *   0: success
 *   -1: illegal data range (out of 8 digits)
 */
int display(int data, int num_digs)
{
	for (int i = 1; i <= num_digs; i++)
	{
		max7219_send(i, data % 10);
		data /= 10;
	}
	return data > 99999999 ? -1 : 0;
}

int main()
{
	int student_id = 316323;
	gpio_init();
	max7219_init();
	display(student_id, 7);
}
