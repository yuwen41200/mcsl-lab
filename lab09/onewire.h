uint8_t OneWire_Reset()
{
	ONEWIRE_INPUT(); 
	GPIOA -> BRR = GPIO_PIN_8;  //high->low
	ONEWIRE_OUTPUT();
	ONEWIRE_DELAY(480);
	ONEWIRE_INPUT();

	ONEWIRE_DELAY(70);

	ONEWIRE_DELAY(410);
}

void OneWire_WriteBit(OneWire_t* , uint8_t bit)
{
	ONEWIRE_INPUT();
	if (bit) //1
	{
		/* Set line low */
		GPIOA -> BRR = GPIO_PIN_8;
		ONEWIRE_OUTPUT();
		/* Bit high */
		ONEWIRE_INPUT();
	} 
	else //0
	{
		/* Set line low */
		GPIOA -> BRR = GPIO_PIN_8;
		ONEWIRE_OUTPUT();
		ONEWIRE_DELAY(70);
		
	}
	ONEWIRE_INPUT();
}

int OneWire_ReadBit(OneWire_t* , uint8_t bit)
{
	int data = 0;
	ONEWIRE_INPUT(); 
	GPIOA -> BRR = GPIO_PIN_8;  //high->low
	ONEWIRE_OUTPUT();
	ONEWIRE_DELAY(1);
	ONEWIRE_INPUT();
	data = GPIOA->IDR & 0b1;
	return data;
}
