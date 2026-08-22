#ifndef MEMORY_H
#define MEMORY_H

typedef struct {
	uint8_t Memory[0x2000];
    uint8_t VRAM[0x2000];

}RAM;

uint8_t read_memory8 (RAM *ram, uint16_t addr);

uint16_t read_memory16 (RAM *ram, uint16_t addr);

void write_memory8 (RAM *ram ,uint8_t value ,uint16_t addr);

void write_memory16(RAM *ram, uint16_t value, uint16_t addr);

uint8_t read_byte(Cartridge *cart, uint16_t address);

uint16_t read_word(Cartridge *cart, uint16_t address);

#endif
