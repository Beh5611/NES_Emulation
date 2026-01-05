#include "Bus.h"
#include "ppu.h"
#include "cartridge.h"
// improve the bus:read with bitwise & (x % n == x & (n-1))
// example 
// 0x0FFF & 0x07FF

// 0000 1111 1111 1111 
// 0000 0111 1111 1111   & 
// =====================
// 0000 0111 1111 1111

// 0010 0000 0000 0111
// 0000 0000 0000 0111    &
// ===================
// 0000 0000 0000 0111
uint8_t Bus::read(uint16_t addr)
{
    if(addr <= 0x1FFF){
        return ram.read(addr & 0x07FF);
    }
    else if(0x2000 <= addr && addr <= 0x3FFF){
        
        return ppu.read(addr & 0x0007);
    }

    return 0;

}

void Bus::write(uint16_t addr, uint8_t data)
{   
    if(addr <= 0x2000){
        ram.write(addr & 0x07FF, data);
    }


    return;
}