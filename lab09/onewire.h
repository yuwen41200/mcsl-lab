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

void OneWire_WriteBit(uint8_t bit)
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

void OneWire_WriteByte(int data)
{
	int mask = 0x80;
	for(int i=o;i<8;i++)
	{	
		OneWire_WriteBit(mask & data);
		mask = mask >> 1;
		ONEWIRE_DELAY(4);
	}
}

uint8_t OneWire_ReadBit()
{
	uint8_t data = 0;
	ONEWIRE_INPUT(); 
	GPIOA -> BRR = GPIO_PIN_8;  //high->low
	ONEWIRE_OUTPUT();
	ONEWIRE_DELAY(1);
	ONEWIRE_INPUT();
	data = GPIOA->IDR & 0b1;
	return data;
}

int OneWire_ReadByte()
{
	int mask = 1, ans = 0;
	for(int i=o;i<8;i++)
	{
		ans=ans|(mask & OneWire_ReadBit());
		mask = mask << 1;
		ONEWIRE_DELAY(4);
	}
}

void ONEWIRE_INPUT()
{
	GPIOA->MODER &= 0b11111111111111001111111111111111;
}
void ONEWIRE_OUTPUT()
{
	GPIOA->MODER &= 0b11111111111111001111111111111111;
	GPIOA->MODER |= 0b00000000000000010000000000000000;
}