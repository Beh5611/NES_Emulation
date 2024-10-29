
#include <iostream>
#include <fstream>
#include <vector>
#include "Bus.h"
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <conio.h> 
#include <sstream>

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

void INX(Bus * bus, P6502* cpu)
{
    cpu->pc = 0x8000; // Set the opcode value
    bus->write(0x8000, 0xE8);

    std::cout << "PC value: 0x" << std::hex << static_cast<int>(cpu->pc) << std::endl;
    std::cout << "X register value: 0x" << std::hex << static_cast<int>(cpu->x) << std::endl;
    
    cpu->cycle();
    std::cout << "X register value after first cycle: 0x" << std::hex << static_cast<int>(cpu->x) << std::endl;


}

void LDA_IMM(Bus * bus, P6502* cpu)
{
    cpu->pc = 0x8000; // Set the opcode value
    bus->write(0x8000, 0xA9);
    bus->write(0x8001, 0x05);
    std::cout << "PC value: 0x" << std::hex << static_cast<int>(cpu->pc) << std::endl;
    std::cout << "Acc register value: 0x" << std::hex << static_cast<int>(cpu->acc) << std::endl;

    cpu->cycle();
    std::cout << "Acc register value after first cycle: 0x" << std::hex << static_cast<int>(cpu->acc) << std::endl;

    

}



void draw_pg(Bus* bus) {
    // Helper function to print a memory block
    auto print_block = [&](uint16_t startAddr, uint16_t endAddr) {
        std::cout << "\nMemory Block: $" 
                  << std::hex << std::uppercase << startAddr 
                  << " to $" << std::hex << std::uppercase << (endAddr - 1) << "\n";

        for (uint16_t addr = startAddr; addr < endAddr; addr++) {
            // Print the row header at the beginning of each 16-byte row
            if ((addr & 0xF) == 0) {
                printf("$%04X ", addr);  // Address in 4-digit hex with a dollar sign
            }

            // Read and print the byte in 2-digit hex
            uint8_t byte = bus->read(addr);
            printf("%02X ", byte);

            // Print a newline after every 16 bytes
            if ((addr & 0xF) == 0xF) {
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
    };

    // Print the first block (0000 - 0040)
    print_block(0x0000, 0x0040);

    // Print the second block (8000 - 8040)
    print_block(0x8000, 0x8040);
}




void load_test_program(Bus* bus) {
    uint16_t offset = 0x8000;
    std::string byteStr;
    std::stringstream ss;

    ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA";

    while(ss >> byteStr){
        uint8_t byte = static_cast <uint8_t> (std::stoul(byteStr, nullptr, 16));

        bus-> ram[offset] = byte;
        offset ++;

    }
}

void display_cpu_memory(Bus* bus, P6502* cpu) {
    // Display the Flags as required
    std::cout << "N  V  -  B  D  I  Z  C" << std::endl;
    std::cout << std::hex 
          << static_cast<int>((cpu->flag_status & 0x80) != 0) << "  "  // N
          << static_cast<int>((cpu->flag_status & 0x40) != 0) << "  "  // V
          << static_cast<int>((cpu->flag_status & 0x20) != 0) << "  "  // - (Unused)
          << static_cast<int>((cpu->flag_status & 0x10) != 0) << "  "  // B
          << static_cast<int>((cpu->flag_status & 0x08) != 0) << "  "  // D
          << static_cast<int>((cpu->flag_status & 0x04) != 0) << "  "  // I
          << static_cast<int>((cpu->flag_status & 0x02) != 0) << "  "  // Z
          << static_cast<int>((cpu->flag_status & 0x01) != 0) << "  "  // C
    << std::endl; 


    std::cout << "X: " << std::hex << static_cast<int>(cpu->x) << std::endl;
    std::cout << "Y: " << std::hex << static_cast<int>(cpu->y) << std::endl;
    std::cout << "ACC: " << std::hex << static_cast<int>(cpu->acc) << std::endl;
    std::cout << "PC: " << std::hex << static_cast<int>(cpu->pc) << std::endl;
    std::cout << "Opcode: " << std::hex << static_cast<int>(cpu->opcode) << std::endl;

    // Display the first 5 rows of RAM starting at 0x8000
    draw_pg(bus);
}

// Step through a single CPU cycle
void step_cpu(Bus* bus, P6502* cpu) {
    // execute instruction


    cpu->cycle();
    display_cpu_memory(bus, cpu);
}

// Main interactive loop
void run_interactive(Bus* bus, P6502* cpu) {
    char input;
    bool running = true;

    std::cout << "Interactive Mode:\n"
              << "[s] Step | [r] Reset | [q] Quit\n";

    while (running) {
        input = _getch();  // Wait for key press

        switch (input) {
            case 's':  // Step through one CPU cycle
                step_cpu(bus, cpu);
                break;
            case 'r':  // Reset the program
                cpu->reset();
                cpu->pc = 0x8000;  // Reset program counter
                std::cout << "CPU Reset!\n";
                display_cpu_memory(bus, cpu);
                break;
            case 'q':  // Quit the program
                running = false;
                std::cout << "Quitting...\n";
                break;
            default:
                std::cout << "Invalid input. Use [s] Step | [r] Reset | [q] Quit\n";
        }
    }
}

int main() {
    Bus* bus = new Bus();
    P6502* cpu = bus->processor;

    // Load a small test program into memory
    load_test_program(bus);
    cpu->pc = 0x8000;  // Set PC to the start of the program

    // Run the interactive loop
    display_cpu_memory(bus, cpu);
    run_interactive(bus, cpu);

    delete bus;  // Clean up
    return 0;
}