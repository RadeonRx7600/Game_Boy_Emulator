#include<stdio.h>
#include<stdint.h>

/*
    This is my co-work with Ravioli my Master Sama on the CPUcore part, it will be
    difficult but a side kick always honor his master. YEHAW SIR 

*/

typedef union {
    struct {
        // 8-bit Registers
        uint8_t lo;
        uint8_t hi;
    };
    uint16_t value;
} Reg16;

typedef struct {
    // 8-bit Registers / 16-bit
    Reg16 af;
    Reg16 bc;
    Reg16 de;
    Reg16 hl;

    // 16-bit Registers
    uint16_t sp;
    uint16_t pc;
} Registers;
