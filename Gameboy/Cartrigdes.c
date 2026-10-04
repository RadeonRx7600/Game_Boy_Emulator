#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include"/home/juan/Bureau/eco-system/Code/C,C#/Game_Boy_Emu.REV1/Include/Cartridge.h"

void Header_Rom_Setter(Cartridge *cart) { 

    //https://gbdev.io/pandocs/The_Cartridge_Header.html

    // CHECKSUM [0x14D]
    uint8_t checksum = 0;
    for (uint16_t addr = 0x134; addr <= 0x14C; addr++) {
        checksum = checksum - cart->Rom_buffer[addr] - 1;
    }
    if (checksum == cart->Rom_buffer[0x14D]) {
        printf("Checksum verified !\n");
    } else {
        printf("Checksum not correct !\n");
        exit(1);
    }

    // Cartridge controller chip [0x147]
    cart->Rom_type = cart->Rom_buffer[0x147]; 
    printf("The Cart_type is : %i\n",cart->Rom_type);

    // RAM [0x149]
    uint8_t ram = cart->Rom_buffer[0x149];
    if (ram == 0) {
        cart->Ram_enable = false;
        printf("Cartridge does not embed RAM\n");

    } else {
        cart->Ram_enable = true;
        
        switch (ram) {
            case 0x1: // no RAM or unused;
            printf("Ram is unused\n");
            break;

            case 0x2: // RAM 8KiB
            cart->SRam_alloc = calloc(8192, sizeof(uint8_t));
            if (cart->SRam_alloc == NULL) {
                printf("Error: SRAM allocation failed\n");
                exit(1);
            }
            break;

            case 0X3: // RAM 16KiB
            break;
        
            default:
            break;
        }
    }
    
    // ROM size [0x148]
    uint8_t size = cart->Rom_buffer[0x148];
    printf("the length of the ROM is %i\n",size);
}

bool Cartridge_Load(Cartridge *cart, const char *path) {

    // Need Check if allocation didn't work propely

    printf("\n-File name is : %s\n",path);
    FILE *file = fopen(path, "rb");
    
    if (file == NULL) {
        perror("Error with the ROM file : ");
        return false;
    }

    fseek(file, 0, SEEK_END);
    cart->Rom_size = ftell(file);
    printf("Rom_size is %zu long.\n",cart->Rom_size);

    rewind(file);
    cart->Rom_buffer = calloc(cart->Rom_size,sizeof(uint8_t));
    printf("Malloc is at %p checked.\n",(void *)cart->Rom_buffer);

    fread(cart->Rom_buffer, 1, cart->Rom_size , file);
    fclose(file);

    return true;
}
