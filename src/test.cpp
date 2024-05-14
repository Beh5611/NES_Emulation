
#include <iostream>
#include "Bus.h"

//g++ -Wall -g -o test test.cpp Bus.cpp p6502.cpp

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
int main() {
    Bus* bus = new Bus();
    P6502* cpu = bus->processor;
    
    LDA_IMM(bus, cpu);

    delete bus; // Don't forget to free dynamically allocated memory
    return 0;
}