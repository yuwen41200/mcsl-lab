#include "onewire.h"

void init(int resolution) {
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

int get_temperature() {
	// TODO
}
