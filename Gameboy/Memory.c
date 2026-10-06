#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include<stdbool.h>

#include"Cartridge.h"
#include"CPU.h"

/*
	General Memory Map :

	  0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
	  4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
	  8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
	  A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
	  C000-CFFF   4KB Work RAM Bank 0 (WRAM)
	  D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
	  E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
	  FE00-FE9F   Sprite Attribute Table (OAM)
	  FEA0-FEFF   Not Usable
	  FF00-FF7F   I/O Ports
	  FF80-FFFE   High RAM (HRAM)
	    FFFF      Interrupt Enable Register
*/

uint8_t Read_Rom_Bank(Cartridge *cart, uint16_t addr) {

	uint32_t offset = (uint32_t)cart->Current_ROM_bank * 0x4000 + (addr - 0x4000);
	return cart->Rom_buffer[offset];
}

uint8_t read_memory(Memory *ram, Cartridge *cart, uint16_t addr) {

	// Cartridge bank0
	if (addr <= 0x3FFF) {
		return cart->Rom_buffer[addr];
	}
	// Cartridge switchable bank
	else if (addr >= 0x4000 && addr <= 0x7FFF) {
		return Read_Rom_Bank(cart, addr);
	}
	// ROM ram embedded
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		return Read_SRam_Bank(ram); // to add
	}
	// Joyapad
	else if (addr == 0xFF00) {
		return Joypad_Status(ram); // to add
	}
	// Standard read
	else {
		return ram->Memory[addr];
	}
}

void write_memory(Memory *ram, Cartridge *cart, uint8_t value ,uint16_t addr) {
	
	// Switcable bank address
	if (addr >= 0x2000 && addr <= 0x3FFF) {
		uint8_t bank = value & 0x1F;
		if (bank == 0) {
			bank=1;
		}
		cart->Current_ROM_bank = bank;
	}
	// RAM address
	else if (addr >= 0xA000 && addr <= 0xBFFF) {
		//Write_SRam_Bank(ram, value, addr);
	}
	// ECHO RAM address
	else if (addr >= 0xE000 && addr < 0xFE00) {
		write_memory(ram, cart, value, addr - 0x2000);
	}
	// Restricted area
	else if (addr >= 0xFEA0 && addr < 0xFEFF) {
		// nothing writes
	}
	// LY register addr
	else if (addr == 0xFF44) {
		ram->Memory[0xFF44] = 0;
	}
	// DMA transfer
	else if (addr == 0xFF46) {
		DMA_transfer(ram, cart, value);
	}
	else {
		ram->Memory[addr] = value;
	}
}

uint16_t read_memory16(Memory *ram, Cartridge*cart, uint8_t addr) {
	
	uint8_t lo = read_memory(ram, cart, addr);
	uint8_t hi = read_memory(ram, cart, addr + 1);

	return (uint16_t)(lo | (hi << 8));
}

void write_memory16(Memory *ram, Cartridge *cart, uint16_t value ,uint16_t addr) {

	write_memory(ram, cart, (uint8_t)(value & 0xFF), addr);
	write_memory(ram, cart, (uint8_t)(value >> 8), addr + 1);
}


void Write_SRam_Bank(Memory *ram, uint8_t value, uint16_t addr) {} // to add

uint8_t Read_SRam_Bank(Memory *ram, uint16_t addr) {} // to add

uint8_t Joypad_Status(Memory *ram) {} // to add

void DMA_transfer(Memory *ram, Cartridge *cart, uint8_t value) {
    uint16_t src = (uint16_t)(value << 8);

    for (int i = 0; i < 0xA0; i++) {
        uint8_t byte = read_memory(ram, cart, src + i);
        ram->Memory[0xFE00 + i] = byte;
    }
}
