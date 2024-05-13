#include "Bus.h"

Bus::Bus()
{
   
    processor = new P6502(this);
    for (size_t i = 0; i < sizeof(ram); i++) {
        ram[i] = 0x00;
    }

}


uint8_t Bus::read(uint16_t addr)
{
    return ram[addr];
}


void Bus::write(uint16_t addr, uint8_t data)
{
    ram[addr] = data;
}