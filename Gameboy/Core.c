/*
    GameBoy Emulator Project

    29/07/2026 : Written in C (MAIN File) - a guided project
    License : MIT
    Warning : Not responsable of the usage !
    Github  : https://github.com/RadeonRx7600/GameBoyEmu

    sources : https://cturt.github.io/cinoop.html //
    		      https://gekkio.fi/files/gb-docs/gbctr.pdf //
        		  https://gbdev.io/pandocs/MBC1.html //
			        https://github.com/HFO4/gameboy.live // (very good)
			        http://www.codeslinger.co.uk/pages/projects/gameboy.html //
*/

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>

#define running true

void Cpu_core(Timer *timer, Cartridge *cart, RAM *ram , Registers *cpu) {

	init_GB(cpu, ram);
	
	while (running) {
    uint8_t cycles;
    
    if (cpu->HALTED == true) {
      cycles = 4;
    	} 	
		else {
        	cycles = Execute_Opcode(cpu, cart, ram);
    	}
    //update_timer(timer, ram, cycles); comming
    //update_ppu(ram, cycles); comming
    cycles = check_interrupts(cpu, ram);
	}
}

int main(void) {

	Cartridge cart;
	RAM ram;
	Timer timer;
	Registers cpu;

	printf("\nWelcome to Gameboy emulator (project by RadeonRx7600) / Github : https://github.com/RadeonRx7600/GameBoyEmu\n");

	char ROM[200];
	bool test = 0;

	while (test == 0) {
		printf("-to choose your ROM enter the ROM path : ");
		fgets(ROM, sizeof(ROM), stdin);
    ROM[strcspn(ROM, "\n")] = '\0';

		bool e =  Cartridge_load(&cart,ROM);
		test = e;
	}
	
	Cpu_core(&timer,&cart,&ram,&cpu);

	return 0;
}
