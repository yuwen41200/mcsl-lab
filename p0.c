extern void gpio_init();
extern void max7219_init();
extern void max7219_send(unsigned char address, unsigned char data);
extern void fix(int input);
extern void display_arr();
int arr[10];

int display(int data , int num_digs)
{
	for(int i=1;i<=num_digs;i++)
	{
		int temp;
		temp=data%10;
		data=data/10;
		max7219_send(i,temp);
	}
}

int main()
{
	int student_id = 316323;
	gpio_init();
	max7219_init();
	display(student_id,7);
}
