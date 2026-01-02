#ifndef MEMORY_H
#define MEMORY_H
#include <cstdint>
#include <cstddef>


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




*/


class Cartridge { 

    public:
        uint8_t prg_rom[16 * 1024];
        uint8_t chr_rom[8 * 1024];        


}



#endif