#include "p6502.h"
#include "Bus.h"
P6502::P6502(Bus* bus)
{
    this->bus = bus;
    // Set the initial value for all the registers and variables
    acc = 0x00; 
    x = 0x00; 
    y = 0x00; 
    flag_status = U;
    stp = 0x0000; 
    pc = 0x0000; 
    mem_addr = 0x0000;
    brch_addr = 0x0000;
    cycles = 0;
    opcode = 0x00;
    operand = 0x00;

    
    
}

void P6502::SetFlag(uint8_t flag, bool b)
{
    if (b)
    {
        // If b then set the flag to the flag register
        flag_status = flag_status | flag;
    }
    else{
        // If not b remove the flag from the flag register
        flag_status = flag_status & ~flag;
    }
};

uint8_t P6502::GetFlag(uint8_t flag)
{
    // Return 1 if flag and flag register are the same, else return 0
    return (flag_status & flag) != 0;
}


void P6502::cycle()
{
    if (cycles == 0){

        opcode = bus->read(pc);

        SetFlag(U, true);
        
        pc++;
        
        INSTRUCTION instruction = instructions[opcode];

        cycles = instruction.cycles;

        uint8_t additional_cycle1 = (this->*instruction.operation)();
        uint8_t additional_cycle2 = (this->*instruction.address_mode)();

        cycles += (additional_cycle1 & additional_cycle2);

		
		SetFlag(U, true);
    }
    cycles--;
}  
void P6502::interrupt()
{

}
void P6502::nm_interrupt()
{

} 
void P6502::reset()
{

}  


// Addressing Modes

uint8_t P6502::IMP()
{
    operand = acc; // Set the operand to the accumilator 
    return 0;
}

uint8_t P6502::IMM()
{
    uint8_t next_byte = operand++;
    operand = next_byte; // Set the operand to the next bit after the instruction
    return 0;
}

uint8_t P6502::ZP0()
{
    
    return 0;
}

uint8_t P6502::ZPX()
{
    
    return 0;
}
uint8_t P6502::ZPY()
{
    
    return 0;
}
uint8_t P6502::REL()
{
    
    return 0;
}
uint8_t P6502::ABS()
{
    
    return 0;
}
uint8_t P6502::ABX()
{
    
    return 0;
}
uint8_t P6502::ABY()
{
    return 0;
}

uint8_t P6502::IND()
{
    return 0;
}

uint8_t P6502::IZX()
{
    
    return 0;
}

uint8_t P6502::IZY()
{
    
    return 0;
}








// Instructions

uint8_t P6502::INC()
{
    bus->write(mem_addr, bus->read(mem_addr) + 1); // Go into the absolute address and increment it by 1.
    return 0;
}



uint8_t P6502::INX()
{
    x++;
    return 0;
}

uint8_t P6502::INY()
{
    y++;
    return 0;
}

uint8_t P6502::ADC()
{
    return 0;
}
uint8_t P6502::AND()
{
    return 0;
}
uint8_t P6502::ASL()
{
    return 0;
}
uint8_t P6502::BCC()
{
    return 0;
}
uint8_t P6502::BCS()
{
    return 0;
}
uint8_t P6502::BEQ()
{
    return 0;
}
uint8_t P6502::BIT()
{
    return 0;
}
uint8_t P6502::BMI()
{
    return 0;
}
uint8_t P6502::BNE()
{
    return 0;
}
uint8_t P6502::BPL()
{
    return 0;
}
uint8_t P6502::BRK()
{
    return 0;
}
uint8_t P6502::BVC()
{
    return 0;
}
uint8_t P6502::BVS()
{
    return 0;
}
uint8_t P6502::CLC()
{
    return 0;
}
uint8_t P6502::CLD()
{
    return 0;
}
uint8_t P6502::CLI()
{
    return 0;
}
uint8_t P6502::CLV()
{
    return 0;
}
uint8_t P6502::CMP()
{
    return 0;
}


uint8_t P6502::CPX()
{
    return 0;
}

uint8_t P6502::CPY()
{
    return 0;
}

uint8_t P6502::DEC()
{
    return 0;
}
uint8_t P6502::DEX()
{
    return 0;
}
uint8_t P6502::DEY()
{
    return 0;
}
uint8_t P6502::EOR()
{
    return 0;
}


uint8_t P6502::JMP()
{
    return 0;
}
uint8_t P6502::JSR()
{
    return 0;
}
uint8_t P6502::LDA()
{
    return 0;
}
uint8_t P6502::LDX()
{
    return 0;
}

uint8_t P6502::LDY()
{
    return 0;
}

uint8_t P6502::LSR()
{
    return 0;
}

uint8_t P6502::NOP()
{
    return 0;
}


uint8_t P6502::ORA()
{
    return 0;
}
uint8_t P6502::PHA()
{
    return 0;
}
uint8_t P6502::PHP()
{
    return 0;
}
uint8_t P6502::PLA()
{
    return 0;
}
uint8_t P6502::PLP()
{
    return 0;
}
uint8_t P6502::ROL()
{
    return 0;
}
uint8_t P6502::ROR()
{
    return 0;
}


uint8_t P6502::RTI()
{
    return 0;
}
uint8_t P6502::RTS()
{
    return 0;
}
uint8_t P6502::SBC()
{
    return 0;
}
uint8_t P6502::SEC()
{
    return 0;
}
uint8_t P6502::SED()
{
    return 0;
}
uint8_t P6502::SEI()
{
    return 0;
}
uint8_t P6502::STA()
{
    return 0;
}
uint8_t P6502::STX()
{
    return 0;
}
uint8_t P6502::STY()
{
    return 0;
}
uint8_t P6502::TAX()
{
    return 0;
}
uint8_t P6502::TAY()
{
    return 0;
}


uint8_t P6502::TSX()
{
    return 0;
}
uint8_t P6502::TXA()
{
    return 0;
}

uint8_t P6502::TXS()
{
    return 0;
}

uint8_t P6502::TYA()
{
    return 0;
}

uint8_t P6502::XXX()
{
    return 0;
}

