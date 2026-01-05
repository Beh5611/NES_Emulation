#ifndef CARTRIDGE_H
#define CARTRIDGE_H
#include <cstdint>
#include <cstddef>

#pragma once
#include <string>



struct NESHeader {
    char signature[4]; // "NES\x1A"
    uint8_t prgRomSize; // Number of 16KB PRG ROM pages
    uint8_t chrRomSize; // Number of 8KB CHR ROM pages
    uint8_t flags6; // Mapper, mirroring, etc.
    uint8_t flags7; // Mapper, battery, etc.
    uint8_t prgRamSize; // PRG RAM size in KB
    uint8_t flags9; // Flags 9
    uint8_t flags10; // Flags 10
    uint8_t zero[5]; // Zero padding
};

/* Cartridge <- contains Prg-rom and chr-rom
    prg-rom occupies the latter half of the cpu addresss space
    and the chr-rom occuppies 0x0000 - 0x1FFF of the ppu address
    space.

    things to do:
    1. read the ines file, fill the cartridge with the correct data required
    2. take the cartridge create pointer and then read then fill the address space
       of the cpu with the prg_rom and the address space of the ppu with chr_rom 



*/


class Cartridge { 

    public:
        Cartridge(const std::string& file_name);  
        NESHeader nes_header;  
        uint8_t trainer [512];
        uint8_t prg_rom[16 * 1024];
        uint8_t chr_rom[8 * 1024];        

};



#endif