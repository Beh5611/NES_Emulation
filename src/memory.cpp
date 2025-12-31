#include "memory.h"

RAM::RAM(){
    for (size_t i = 0; i < sizeof(ram); i++) {
        ram[i] = 0x00;
    }
}

uint8_t RAM::read(uint16_t addr){  
    uint8_t data = ram[addr];
    return data;

}

void RAM::write(uint16_t addr, uint8_t data){
    ram[addr] = data;
}


