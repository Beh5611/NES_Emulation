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