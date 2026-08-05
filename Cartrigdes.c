#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>


/*  - v0.1.0 :

        I am using https://gbdev.io/pandocs/MBCs.html which tells everthing about the Cartridge.
        Its the best documentation I could find. For this section we're emulating the whole Cartridge process.
        The current version of this project is v0.1.0 for this version I did the minimun so I mean,
        the code is done to run (Super Mario Land) only, No RAM/ no cartridge bigger than 64KiB and only MBC1.

        Disclamer -> : no Cartridge or any ROM types will be provided.
        Caution -> Becarefull on the net when downloading ROMs, check their integrities first with cheksum header or sha256.

*/


typedef struct {

    uint8_t *rom;
    size_t rom_size;
    uint8_t bank_register;

    uint8_t type;
    bool Ram_enable;
    uint8_t ram_size_code;
    uint8_t rom_size_code;

    uint8_t current_bank;    // 2000–3FFF — ROM Bank Number (Write Only) (called the 5-bits register);
    uint8_t rom_bank_high2; // 4000–5FFF — RAM Bank Number or Upper Bits of ROM Bank Number (Write Only);
    uint8_t banking_mode;  // later ...;

} Cartridge;

bool Cartridge_load(Cartridge *cart, const char *path) // Return True if the file has been loaded into *ROM_struct;
{
    printf("\n-File name is : %s\n",path); //
    FILE *file = fopen(path, "rb");
    
    if (file == NULL) {
        perror("Error with fopen ");
        return false;
    } 
    else {
    fseek(file, 0, SEEK_END); // fseek(FILE * fptr, long int offset, int origin);
    cart->rom_size = ftell(file); // tell where the file indicator is (elements [.] in this case it gives the total lenght of file because seekend aim at EOF;
    printf("-Rom_size is %zu long.\n",cart->rom_size); //

    rewind(file);
    cart->rom = malloc(cart->rom_size);
    printf("-Malloc is at %p checked.\n",(void *)cart->rom);

    fread(cart->rom, 1, cart->rom_size , file); //fread(void * destination, size_t size, size_t amount, FILE * fptr);
    fclose(file);

    return true;
    }
}

void Header_Rom_Reader(Cartridge *cart) {
    
    /*  
        0x0100 - 0x014F is the region where the header file belong (header file is in Bank0),
        in the header file you can find a lot of importants informations about the cartridge.
        https://gbdev.io/pandocs/The_Cartridge_Header.html gives every parts and their regions.
        Most used to debug and verify, but I will develop it further in the v1.
    */

    uint8_t checksum = 0; // this section tries a cheksum located at [0x14D]
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

    cart->rom_size_code = cart->rom[0x148]; //tells the lenght on one byte ; ex : 0x1 is 64KiB  / 0x2 is 128 KiB
    printf("-The Rom_size_code is : %u\n",cart->rom_size_code);

};

uint32_t Cartridge_bus_write_bank_dispatcher(Cartridge *cart, uint8_t bank , uint16_t addr) {

    /*
        The MBC1 chip includes four registers that affect the behaviour of the chip.
        All registers are effectively mapped to address ranges instead of single addresses. 
        All registers are smaller than 8 bits, and unused bits are simply ignored during writes. 

        The registers are not directly readable.
        The explanation is easy, there is no inside registers. The CPU send a write instruction to the ROM but cannot write (read only memory) so instead the mbc chip will intercept the bus and the value, that's how he create his regs.

        This 5-bit register (can reach(bank : $01-$1F) in decimal its bank1 to bank31) selects the ROM bank number for the 4000–7FFF region (The switchable one). 
        Higher bits are discarded, exemple : writing $E1 (binary 11100001) - take only 5-bits (LSB one) to this register and would select bank $01.

        Important : the bank n°0 cannot change place or dumped into another bank so anytime the cpu addresses to the bank0, the zero is immediatly replaced by a 1.
    */
    
    if (addr >= 0x2000 && addr <= 0x3FFF) // region bank1
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
            File name is : ...
            Rom_size is 65536 long.
            Malloc is 0x60f98cb088a0 checked.
            checksum verified;
            The Cart_type is : 1
            Bool status Ram Enabled = 0
            the Ram_size is : 0
            the Rom_size_code is : 1

            ROM = 64KiB;
            TypeRom = MCB1;
            Rom_Size 64KiB with 4 banks;
            No RAM code : 0;
    */
    Cartridge cart;

    char *Game_Rom_File = "ur file with the gameRom"; // for test (Rom not include)
    bool debug = Cartridge_load(&cart, Game_Rom_File);
    if (debug == 0) { printf("Cartridge Error loaded bool = 0");};
    Header_Rom_Reader(&cart);

    // main is a test not the final one, just for me :D

    return 0;
};
