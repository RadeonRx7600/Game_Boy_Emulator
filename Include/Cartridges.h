#ifndef CARTRIDGES_H
#define CARTRIDGES_H

#include<stdint.h>
#include<stdio.h>
#include<stdbool.h>

typedef struct {

    uint8_t *rom;
    size_t rom_size;
    uint8_t bank_register;

    uint8_t type;
    bool Ram_enable;
    uint8_t ram_size_code;
    uint8_t rom_size_code;

    uint8_t current_bank; 
    uint8_t rom_bank_high2; 
    uint8_t banking_mode;  

} Cartridge;

bool Cartridge_load(Cartridge *cart, const char *path);

void Header_Rom_Reader(Cartridge *cart);

uint32_t Cartridge_bus_write_bank_dispatcher(Cartridge *cart, uint8_t bank , uint16_t addr);

#endif
