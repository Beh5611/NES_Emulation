#include <iostream>
#include <fstream> 
#include "cartridge.h"


Cartridge::Cartridge(const std::string& file_name) {
    std::ifstream myFile(file_name, std::ios::binary);

    if (!myFile.is_open()) {
        std::cerr << "Failed to open " << file_name << "\n";
        return;
    }

    // Read 16-byte header
    myFile.read(reinterpret_cast<char*>(&nes_header), sizeof(NESHeader));

    // Check if trainer exists
    bool trainer_exists = nes_header.flags6 & (1 << 2);

    // Read 512-byte trainer if present
    if (trainer_exists) {
        myFile.read(reinterpret_cast<char*>(trainer), 512);
    }

    // Read PRG ROM
    myFile.read(reinterpret_cast<char*>(prg_rom), nes_header.prgRomSize);

    // Read CHR ROM if present
    if (nes_header.chrRomSize > 0) {
        myFile.read(reinterpret_cast<char*>(chr_rom), nes_header.chrRomSize);
    }

    myFile.close();
}

uint8_t Cartridge::cpu_read(uint16_t addr) {
    // PRG ROM is mapped at $8000–$FFFF
    if (addr >= 0x8000 && addr <= 0xFFFF) {

        // 16 KB PRG ROM → mirrored
        if (nes_header.prgRomSize == 16 * 1024) {
            return prg_rom[addr & 0x3FFF];
        }

        // 32 KB PRG ROM → direct mapping
        else {
            return prg_rom[addr & 0x7FFF];
        }
    }

    // Cartridge does not respond
    return 0x00;
}
uint8_t Cartridge::ppu_read(uint16_t addr) {
    // CHR is visible at $0000–$1FFF
    if (addr <= 0x1FFF) {

        // CHR ROM present
        if (nes_header.chrRomSize > 0) {
            return chr_rom[addr];
        }

        // CHR RAM (no CHR ROM in cartridge)
        // else {
        //     return chr_ram[addr];
        // }
    }

    return 0x00;
}
