#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>

/*  
        I am using https://gbdev.io/pandocs/The_Cartridge_Header.html which tells everthing about the Cartridge,
        its the best documentation I could find. For this section we're emulating the whole Cartridge process, 
        the segmentation to the output Bus. Disclamer -> : no Cartridge or any ROM types will be provided - .
        Caution -> Becarefull on the net when downloading ROMs, check their integrity first with cheksum header or sha256.

*/

typedef struct {
    uint8_t *rom;
    size_t rom_size;
    uint8_t bank_register;

    uint8_t type;
    bool Ram_enable;
    uint8_t ram_size_code;
    uint8_t rom_size_code;

    uint8_t current_bank;    // 2000–3FFF — ROM Bank Number (Write Only)
    uint8_t rom_bank_high2; // 4000–5FFF — RAM Bank Number or Upper Bits of ROM Bank Number (Write Only)
    uint8_t banking_mode;  // later ...

} Cartridge;

bool Cartridge_load(Cartridge *cart, const char *path) // return True if the file have been loaded into *ROM_struct
{   
    printf("\n --------------Debug-----------------");
    printf("\n-File name is : %s\n",path);
    FILE *file = fopen(path, "rb");
    
    if (file == NULL) {
        perror("Error with fopen ");
        return false;
    } 
    else {
    fseek(file, 0, SEEK_END); // fseek(FILE * fptr, long int offset, int origin);
    cart->rom_size = ftell(file); // tell where the file indicator is (elements ... in this case it gives the total lenght of file bc seekend aim at EOF
    printf("-Rom_size is %zu long.\n",cart->rom_size);

    rewind(file); // put file indicator at first element

    cart->rom = malloc(cart->rom_size);
    printf("-Malloc is at %p checked.\n",(void *)cart->rom);

    fread(cart->rom, 1, cart->rom_size , file); //fread(void * destination, size_t size, size_t amount, FILE * fptr);
    fclose(file);

    return true;
    }
}

void Header_Rom_Reader(Cartridge *cart) {
    
    //0x0100 - 0x014F header file in a Cartridge (header is in the Bank1)

    uint8_t checksum = 0;
    for (uint16_t address = 0x0134; address <= 0x014C; address++) {
    checksum = checksum - cart->rom[address] - 1;
    }
    if (checksum == cart->rom[0x014D]) {
        printf("-Checksum verified;\n");
    }
    else {
        printf("-Header not correct wrong checksum verify the integrity;\n"); 
    }

   
    cart->type = cart->rom[0x147]; 
    printf("-The Cart_type is : %i\n",cart->type);
    
    cart->ram_size_code = cart->rom[0x149];
    if (cart->ram_size_code == 0) {
        cart->Ram_enable = false;
    } else {
        cart->Ram_enable = true;
    }
    printf("-Bool status Ram Enabled = %b\n",cart->Ram_enable);

    printf("-The Ram_size is : %u\n",cart->ram_size_code);

    cart->rom_size_code = cart->rom[0x148]; //tells the lenght in one byte ; ex : 0x1 is 64KiB
    printf("-The Rom_size_code is : %u\n",cart->rom_size_code);

};

uint32_t Cartridge_bus_write_bank_dispatcher(Cartridge *cart, uint8_t bank , uint16_t addr) {

    /*

        The MBC1 chip includes four registers that affect the behaviour of the chip.
        All registers are effectively mapped to address ranges instead of single addresses. 
        All registers are smaller than 8 bits, and unused bits are simply ignored during writes. 
        The registers are not directly readable.

        This 5-bit register (range $01-$1F) selects the ROM bank number for the 4000–7FFF region (The switchable one). 
        Higher bits are discarded — writing $E1 (binary 11100001 - take only five bits) to this register would select bank $01.

    */
    
    if (addr >= 0x2000 && addr <= 0x3FFF) 
    {
        cart->current_bank = bank & 0x03;

        if (cart->current_bank == 0) {
            cart->current_bank = 1;
        }
    }

    int32_t offset = cart->current_bank * 0x4000 + (addr - 0x4000); // bank number * one bank size so 0x4000(16KiB) + the address the bus sent for example [0x5209] minus 0x4000 bc the cpu can only address at the switchable bank [0x4000 - 0x7FFF] so it starts at 0x4000 (see pdf or pan doc)

    return offset;
}

int main(void) {

    /*
        For the moment : 
            File name is : /home/juan/Bureau/eco-system/Code/C,C++/Game_Boy_Emu/ROMS/Super Mario Land (World) (Rev 1).gb
            Rom_size is 65536 long.
            Malloc is 0x60f98cb088a0 checked.
            checksum verified;
            The Cart_type is : 1
            Bool status Ram Enabled = 0
            the Ram_size is : 0
            the Rom_size_code is : 1
            ---------------------------------------------------------DEBUG::
            ROM = 64KiB;
            TypeRom = MCB1;
            Rom_Size 64KiB with 4 banks;
            No RAM code 0;
    */

    char *MBC1_ROM = "/home/juan/Bureau/eco-system/Code/C,C++/Game_Boy_Emu/ROMS/Super Mario Land (World) (Rev 1).gb"; // for test (Rom not include)
    Cartridge cart;
    bool debug = Cartridge_load(&cart, MBC1_ROM);
    if (debug == 0) { printf("Cartridge Error loaded bool = 0");};
    Header_Rom_Reader(&cart);

    return 0;
};