#ifndef BUS_H
#define BUS_H
#include <cstdint>
#include <iostream>
#include <vector>
#include "p6502.h"
#include "memory.h"
#include "ppu.h"
class Cartridge;

class Bus
{
    private:
        RAM ram;
        PPU ppu;
        Cartridge* cartridge;

    public:        
        uint8_t prg_rom[16* 1024];
        
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);

        
};
#endif
