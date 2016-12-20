#include "libtmd.h"
#include "onewire.h"

void set_resolution(unsigned resolution) {
	// Initialization
	OneWire_Reset();
	// ROM Command: Skip ROM [CCh]
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	// DS18B20 Function Command: Write Scratchpad [4Eh]
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	// Data Exchange: TH Register [40h]
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	// Data Exchange: TL Register [08h]
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	// Data Exchange: Configuration Register
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	switch (resolution) {
		case 9:
			OneWire_WriteBit(0);
			OneWire_WriteBit(0);
			break;
		case 10:
			OneWire_WriteBit(1);
			OneWire_WriteBit(0);
			break;
		case 11:
			OneWire_WriteBit(0);
			OneWire_WriteBit(1);
			break;
		case 12:
		default:
			OneWire_WriteBit(1);
			OneWire_WriteBit(1);
			break;
	}
	OneWire_WriteBit(0);

	// Initialization
	OneWire_Reset();
	// ROM Command: Skip ROM [CCh]
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	// DS18B20 Function Command: Copy Scratchpad [48h]
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
}

int16_t get_temperature() {
	// Initialization
	OneWire_Reset();
	// ROM Command: Skip ROM [CCh]
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	// DS18B20 Function Command: Convert T [44h]
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);

	// Fucking Wait
	usleep(750000);

	// Initialization
	OneWire_Reset();
	// ROM Command: Skip ROM [CCh]
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	// DS18B20 Function Command: Read Scratchpad [BEh]
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(1);
	OneWire_WriteBit(0);
	OneWire_WriteBit(1);
	// Data Exchange: Temperature LSB Register
	int16_t r = 0;
	r |= OneWire_ReadBit() << 0;
	r |= OneWire_ReadBit() << 1;
	r |= OneWire_ReadBit() << 2;
	r |= OneWire_ReadBit() << 3;
	r |= OneWire_ReadBit() << 4;
	r |= OneWire_ReadBit() << 5;
	r |= OneWire_ReadBit() << 6;
	r |= OneWire_ReadBit() << 7;
	// Data Exchange: Temperature MSB Register
	r |= OneWire_ReadBit() << 8;
	r |= OneWire_ReadBit() << 9;
	r |= OneWire_ReadBit() << 10;
	r |= OneWire_ReadBit() << 11;
	r |= OneWire_ReadBit() << 12;
	r |= OneWire_ReadBit() << 13;
	r |= OneWire_ReadBit() << 14;
	r |= OneWire_ReadBit() << 15;

	// Initialization
	OneWire_Reset();

	return r;
}
