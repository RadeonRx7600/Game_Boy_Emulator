#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>

/*
	General Memory Map
	  0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)      	 --done in cartridge.c--
	  4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number) 	 --done in cartridge.c--

	  8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
	  A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
	  C000-CFFF   4KB Work RAM Bank 0 (WRAM)
	  D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
	  E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
	  FE00-FE9F   Sprite Attribute Table (OAM)
	  FEA0-FEFF   Not Usable
	  FF00-FF7F   I/O Ports
	  FF80-FFFE   High RAM (HRAM)
	  FFFF        Interrupt Enable Register
*/

typedef struct {
	uint8_t Memory[0x2000];
    uint8_t VRAM[0x2000];

}RAM;

uint8_t read_memory8 (RAM *ram, uint16_t addr) {
	return ram->Memory[addr];
}

uint16_t read_memory16 (RAM *ram, uint16_t addr) {
	return ram->Memory[addr] | ((uint16_t)ram->Memory[addr + 1] << 8);
}

void write_memory8 (RAM *ram ,uint8_t value ,uint16_t addr) {
	ram->Memory[addr] = value;
}

void write_memory16(RAM *ram, uint16_t value, uint16_t addr)
{
    ram->Memory[addr] = value;
    ram->Memory[addr + 1] = value >> 8;
}

uint8_t read_byte(Cartridge *cart, uint16_t address)
{
    return cart->rom[address];
}

uint16_t read_word(Cartridge *cart, uint16_t address)
{
    uint8_t lo = read_byte(cart, address);
    uint8_t hi = read_byte(cart, address + 1);

    return (uint16_t)(lo | (hi << 8));
}
