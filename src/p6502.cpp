#include "p6502.h"
#include "Bus.h"


// Constructor
P6502::P6502(Bus* bus)
{
    this->bus = bus;
    // Set the initial value for all the registers and variables
    acc = 0x00; 
    x = 0x00; 
    y = 0x00; 
    flag_status = U;
    stp = 0xFD; 
    pc = 0x0000; 
    mem_addr = 0x0000;
    brch_addr = 0x0000;
    cycles = 0;
    opcode = 0x00;
    operand = 0x00;

    
    
}

// Read from the memory using the bus.
uint8_t P6502::read(uint16_t addr)
{
    return bus->read(addr);
}

// Write into the Bus, which writes into the Memory
void P6502::write(uint16_t addr, uint8_t data)
{
    bus->write(data, addr);
}

// Store the some value in the stack page.
void P6502::push(uint8_t data) 
{
    
    // The address is calculated by adding the base address of the stack page (0x0100)
    // to the current value of the stack pointer.
    write((0x0100 + stp), data);

    stp--;
}

// For implied instructions, sets the operand
void P6502::fetch_operand()
{
	if (!(instructions[opcode].address_mode == &P6502::IMP))
		operand = read(mem_addr);
	
}

// Sets the flag if true, removes the flag if false.
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

// Returns whether a particular flag in the bit field is used or not.
uint8_t P6502::GetFlag(uint8_t flag)
{
    // Return 1 if flag and flag register are the same, else return 0
    return (flag_status & flag) != 0;
}


void P6502::cycle()
{
    if (cycles == 0){

        opcode = read(pc);

        SetFlag(U, true);
        
        pc++;
        
        INSTRUCTION instruction = instructions[opcode];

        cycles = instruction.cycles;

        uint8_t additional_cycle1 = (this->*instruction.address_mode)();
        uint8_t additional_cycle2 = (this->*instruction.operation)();
        

        cycles += (additional_cycle1 & additional_cycle2);

		
		SetFlag(U, true);
    }
    cycles--;
}  
void P6502::interrupt()
{
    if(GetFlag(I) == 0){
        push((pc >> 8) & 0x00FF);  // Push high byte of PC onto stack
        push(pc & 0x00FF);         // Push low byte of PC onto stack
        
        // Set and push status register onto stack
        SetFlag(B, false);
		SetFlag(U, true);
		SetFlag(I, true);
        push(flag_status);   
        
        pc = (read(0xFFFF) << 8) | read(0xFFFE);
        cycles = 7;

    }

}
void P6502::nm_interrupt()
{
    push((pc >> 8) & 0x00FF);  // Push high byte of PC onto stack
    push(pc & 0x00FF);         // Push low byte of PC onto stack
    
    // Set and push status register onto stack
    SetFlag(B, false);
    SetFlag(U, true);
    SetFlag(I, true);
    push(flag_status);   
        
    pc = (read(0xFFFB) << 8) | read(0xFFFA);
    cycles = 8;

} 
void P6502::reset()
{
    // Reset Registers and variables
    acc = 0x00;
    x = 0x00;
    y = 0x00;
    mem_addr = 0x0000;
    brch_addr = 0x0000;
    stp = 0xFD;
    operand = 0x00;

    SetFlag(U, true); // Reset flag state

    // Read the reset vector to set the PC
    pc = (read(0xFFFD) << 8) | read(0xFFFC);

    cycles = 8; //Representative of how long reset takes.
}  

///////////////////////////////////////////////////////
//------------------Addressing Modes-----------------//
///////////////////////////////////////////////////////

// Implied Addressing
uint8_t P6502::IMP()
{
    return 0;
}

// Immidiate Addressing
uint8_t P6502::IMM()
{
    // Get the operand from the PC address
    mem_addr = pc;
    pc++;
    
    return 0;
}

// Zero-Page Addressing
uint8_t P6502::ZP0()
{
    uint16_t offset = read(pc); // Get the offset for the zero page

    mem_addr = offset & 0x00FF; // Mask the offset to get the operand 
    
    pc++;
    
    return 0;
}

// X-Indexed Zero-Page Addressing
uint8_t P6502::ZPX()
{
    
    uint16_t offset = read(pc); // Get the offset for the zero page

    // Mask the offset, and increment it by x, to get the operand 
    mem_addr = (offset + x) & 0x00FF; 
    pc++;
    
    return 0;
}

// Y-Indexed Zero-Page Addressing
uint8_t P6502::ZPY()
{
    uint16_t offset = read(pc); 

    // Mask the offset, and increment it by y, to get the operand 
    mem_addr = (offset + y) & 0x00FF; 
    pc++;
    
    return 0;
}

// Relative Addressing
uint8_t P6502::REL()
{
    // Fetch the offset
    uint16_t offset = read(pc);
	
	if (offset & 0x80)
		offset |= 0xFF00;
    
    pc++;

    // Calculate target address
    brch_addr = pc + offset; 

	return 0;
}

// Absolute Addressing
uint8_t P6502::ABS()
{
    // We will have to fetch the address in two iterations
    // since read() returns a uint8_t instead of a 2 Byte address.

    // Get the first 8 bits
    uint16_t low = read(pc);
    pc++;

    // Get the second 8 bits
    uint16_t high = read(pc);
    pc++;

    // Shift the second 8 bits up so that we can merge the two addresses into one
    mem_addr = (high << 8) | low;

    return 0;
}

// X-Indexed Absolute Addressing
uint8_t P6502::ABX()
{
    uint8_t lo = read(pc); 
    pc++;
    
    uint8_t hi = read(pc); 
    pc++;

    uint16_t base_addr = lo + (hi << 8); 

    // Index the memory address by x
    mem_addr = base_addr + x;

    // Check for page crossing
    if ((mem_addr & 0xFF00) != (base_addr & 0xFF00)) {
        return 1; // Indicate a page boundary crossing so a cycle will be added
    }

    return 0;
}

// Y-Indexed Absolute Addressing
uint8_t P6502::ABY()
{
    uint8_t lo = read(pc); 
    pc++;
    
    uint8_t hi = read(pc); 
    pc++;

    uint16_t base_addr = lo + (hi << 8); 
    
    // Index the memory address by y
    mem_addr = base_addr + y;

    
    if ((mem_addr & 0xFF00) != (base_addr & 0xFF00)) {
        return 1; 
    }

    return 0;
}

// Indirect Addressing
uint8_t P6502::IND() 
{
    // Get the low byte of the pointer address
    uint8_t ptr_low = read(pc); 
    pc++;
    
    // Get the high byte of the pointer address
    uint8_t ptr_high = read(pc); 
    pc++;

    // Combine the low and high bytes to form the 16-bit pointer address
    uint16_t ptr_addr = (ptr_high << 8) | ptr_low; // Shift the high byte left by 8 bits and add to the low byte

    // Get the low byte of the effective address from the pointer address
    uint8_t eff_lo = read(ptr_addr); 
    ptr_addr++;
    
    // Get the high byte of the effective address from the next memory location
    uint8_t eff_hi = read(ptr_addr); 

    // Combine the low and high bytes to form the 16-bit effective address
    mem_addr = (eff_hi << 8) | eff_lo; 

    return 0;
}

// X-Indexed Indirect Addressing
uint8_t P6502::IZX()
{
    uint16_t t = read(pc);
	pc++;

    uint16_t lo = read((uint16_t)(t + (uint16_t)x) & 0x00FF);
	uint16_t hi = read((uint16_t)(t + (uint16_t)x + 1) & 0x00FF);


    mem_addr = (hi << 8) | lo;

    return 0;
}

// Y-Indexed Indirect Addressing
uint8_t P6502::IZY()
{
    
    uint16_t t = read(pc);
	pc++;

	uint16_t lo = read(t & 0x00FF);
	uint16_t hi = read((t + 1) & 0x00FF);

	mem_addr = (hi << 8) | lo;
	mem_addr += y;
	
	if ((mem_addr & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
        
}







///////////////////////////////////////////////////
// -----------------Instructions-----------------//
///////////////////////////////////////////////////

// Increment the Value in the Memory Adress (Operand)
// Flags: N, Z
uint8_t P6502::INC()
{
    fetch_operand();
    operand += 1;
    write(mem_addr, operand);
    SetFlag(N, operand & 0x80);
    SetFlag(Z, operand == 0x00);

    return 0;
}


// Increments The x Register
// Flags: N, Z
uint8_t P6502::INX()
{
    x++;
    SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);
    return 0;
}

// Increments The y Register
// Flags: N, Z
uint8_t P6502::INY()
{
    y++;
    SetFlag(Z, y == 0x00);
	SetFlag(N, y & 0x80);
    return 0;
}

uint8_t P6502::ADC()
{
    return 0;   
}

// 'AND' Memory with Accumulator and Store it Back Into the Accumulator
// Flags: N, Z
uint8_t P6502::AND()
{
    fetch_operand();
    acc = acc & operand;
    SetFlag(N, acc & 0x80);
    SetFlag(Z, acc == 0x00);
    return 0;
}

uint8_t P6502::ASL()
{   
    if(instructions[opcode].address_mode == &P6502::IMP){
        uint16_t temp = (uint16_t)acc << 1;
        acc = (uint8_t)(temp & 0x00FF);
        SetFlag(Z, acc == 0x00);
        SetFlag(N, acc & 0x80);
        SetFlag(C, (temp & 0xFF00) > 0);
    }
    else{
        fetch_operand();
        uint16_t temp = (uint16_t)operand << 1;
        operand = (uint8_t)(temp & 0x00FF);
        write(mem_addr, operand);
        SetFlag(Z, operand == 0x00);
        SetFlag(N, operand & 0x80);
        SetFlag(C, (temp & 0xFF00) > 0);
    }
   
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
    SetFlag(C, false);
    return 0;
}
// Clear the Decimal Flag
uint8_t P6502::CLD()
{
    SetFlag(D, false);
    return 0;
}
// Clear the Interrupt Flag
uint8_t P6502::CLI()
{
    SetFlag(I, false);
    return 0;
}

uint8_t P6502::CLV()
{
    SetFlag(V, false);
    return 0;
}

// Compares the accumulator and the operand
// Flags: C, Z, N
uint8_t P6502::CMP()
{
    fetch_operand();
    uint8_t result = acc - operand;
    SetFlag(C, acc >= operand);
    SetFlag(Z, result == 0x00);
    SetFlag(N, result & 0x80);
    return 0;
}

// Compares the x and the operand
// Flags: C, Z, N
uint8_t P6502::CPX()
{
    fetch_operand();
    uint8_t result = x - operand;
    SetFlag(C, acc >= operand);
    SetFlag(Z, result == 0x00);
    SetFlag(N, result & 0x80);
    
    return 0;
}

// Compares the y and the operand
// Flags: C, Z, N
uint8_t P6502::CPY()
{
    fetch_operand();
    uint8_t result = y - operand;
    SetFlag(C, acc >= operand);
    SetFlag(Z, result == 0x00);
    SetFlag(N, result & 0x80);
    
    return 0;
}

uint8_t P6502::DEC()
{   
    fetch_operand();
    operand--;
    write(mem_addr, operand);
    SetFlag(N, operand == 0x00);
    SetFlag(Z, operand & 0x80);
    return 0;
}
uint8_t P6502::DEX()
{
    x--;
    SetFlag(N, x== 0x00);
    SetFlag(Z, x & 0x80);
    return 0;
}
uint8_t P6502::DEY()
{
    y--;
    SetFlag(N, y == 0x00);
    SetFlag(Z, y & 0x80);
    return 0;
}
uint8_t P6502::EOR()
{
    return 0;
}


uint8_t P6502::JMP()
{
    pc= mem_addr;
    return 0;
}
uint8_t P6502::JSR()
{
    return 0;
}

// Loads Accumulator With Value From Memory
// Flags: N, Z
uint8_t P6502::LDA()
{
    fetch_operand();
    acc = operand;
    SetFlag(Z, acc == 0x00);
	SetFlag(N, acc & 0x80);
    return 0;
}

// Loads X Register With Value From Memory
// Flags: N, Z
uint8_t P6502::LDX()
{
    fetch_operand();
    x = operand;
    SetFlag(Z, x == 0x00);
	SetFlag(N, x & 0x80);
    
    return 0;
}

// Loads y Register With Value From Memory
// Flags: N, Z
uint8_t P6502::LDY()
{
    fetch_operand();
    y = operand;
    SetFlag(Z, y == 0x00);
	SetFlag(N, y & 0x80);
    
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

// Push the Accumulator to Stack
uint8_t P6502::PHA()
{
    push(acc);

    return 0;
}
uint8_t P6502::PHP()
{
    push(flag_status);
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
// Set the Carry Flag
uint8_t P6502::SEC()
{
    SetFlag(C, true);
    return 0;
}
// Set the Decimal Flag
uint8_t P6502::SED()
{
    SetFlag(D, true);
    return 0;
}
// Enable Interrupts by Setting the Interrupt Flag
uint8_t P6502::SEI()
{
    SetFlag(I, true);
    return 0;
}
// Store Accumulator at Memory Address
uint8_t P6502::STA()
{
    write(mem_addr, acc);
    return 0;
}
// Store X Register at Address
uint8_t P6502::STX()
{
    write(mem_addr, x);
    return 0;
}
// Store Y Register at Address
uint8_t P6502::STY()
{
    write(mem_addr, y);
    return 0;
}
uint8_t P6502::TAX()
{
    x = acc;
    SetFlag(Z, x == 0x00);
    SetFlag(N, x & 0x80);
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

