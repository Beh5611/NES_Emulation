
#include <iostream>
#include "Bus.h"


int main() {
    Bus* bus = new Bus();
    P6502* cpu = bus->processor;
    cpu->pc = 0x8000; // Set the opcode value
    bus->write(0x8000, 0xE8);

    std::cout << "PC value: 0x" << std::hex << static_cast<int>(cpu->pc) << std::endl;
    std::cout << "X register value: 0x" << std::hex << static_cast<int>(cpu->x) << std::endl;
    
    cpu->cycle();
    std::cout << "X register value after first cycle: 0x" << std::hex << static_cast<int>(cpu->x) << std::endl;


    
    delete bus; // Don't forget to free dynamically allocated memory
    return 0;
}