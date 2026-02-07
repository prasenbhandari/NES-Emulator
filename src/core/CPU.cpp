#include "core/CPU.h"
#include "core/Bus.h"
#include <cstdint>

CPU::CPU() {
    // Initialize all instructions to be illegal (XXX)
    for (auto& i : lookup) {
        i = { "XXX", &CPU::XXX, &CPU::IMP, 2 };
    }

    // --- ADD WITH CARRY (ADC) ---
    lookup[0x69] = { "ADC", &CPU::ADC, &CPU::IMM, 2 };
    lookup[0x65] = { "ADC", &CPU::ADC, &CPU::ZP0, 3 };
    lookup[0x75] = { "ADC", &CPU::ADC, &CPU::ZPX, 4 };
    lookup[0x6D] = { "ADC", &CPU::ADC, &CPU::ABS, 4 };
    lookup[0x7D] = { "ADC", &CPU::ADC, &CPU::ABX, 4 };
    lookup[0x79] = { "ADC", &CPU::ADC, &CPU::ABY, 4 };
    lookup[0x61] = { "ADC", &CPU::ADC, &CPU::IZX, 6 };
    lookup[0x71] = { "ADC", &CPU::ADC, &CPU::IZY, 5 };

    // --- LOGICAL AND (AND) ---
    lookup[0x29] = { "AND", &CPU::AND, &CPU::IMM, 2 };
    lookup[0x25] = { "AND", &CPU::AND, &CPU::ZP0, 3 };
    lookup[0x35] = { "AND", &CPU::AND, &CPU::ZPX, 4 };
    lookup[0x2D] = { "AND", &CPU::AND, &CPU::ABS, 4 };
    lookup[0x3D] = { "AND", &CPU::AND, &CPU::ABX, 4 };
    lookup[0x39] = { "AND", &CPU::AND, &CPU::ABY, 4 };
    lookup[0x21] = { "AND", &CPU::AND, &CPU::IZX, 6 };
    lookup[0x31] = { "AND", &CPU::AND, &CPU::IZY, 5 };

    // --- ARITHMETIC SHIFT LEFT (ASL) ---
    lookup[0x0A] = { "ASL", &CPU::ASL, &CPU::IMP, 2 };
    lookup[0x06] = { "ASL", &CPU::ASL, &CPU::ZP0, 5 };
    lookup[0x16] = { "ASL", &CPU::ASL, &CPU::ZPX, 6 };
    lookup[0x0E] = { "ASL", &CPU::ASL, &CPU::ABS, 6 };
    lookup[0x1E] = { "ASL", &CPU::ASL, &CPU::ABX, 7 };

    // --- BRANCH ON CARRY CLEAR (BCC) ---
    lookup[0x90] = { "BCC", &CPU::BCC, &CPU::REL, 2 };

    // --- BRANCH ON CARRY SET (BCS) ---
    lookup[0xB0] = { "BCS", &CPU::BCS, &CPU::REL, 2 };

    // --- BRANCH ON EQUAL (BEQ) ---
    lookup[0xF0] = { "BEQ", &CPU::BEQ, &CPU::REL, 2 };

    // --- BIT TEST (BIT) ---
    lookup[0x24] = { "BIT", &CPU::BIT, &CPU::ZP0, 3 };
    lookup[0x2C] = { "BIT", &CPU::BIT, &CPU::ABS, 4 };

    // --- BRANCH ON MINUS (BMI) ---
    lookup[0x30] = { "BMI", &CPU::BMI, &CPU::REL, 2 };

    // --- BRANCH ON NOT EQUAL (BNE) ---
    lookup[0xD0] = { "BNE", &CPU::BNE, &CPU::REL, 2 };

    // --- BRANCH ON PLUS (BPL) ---
    lookup[0x10] = { "BPL", &CPU::BPL, &CPU::REL, 2 };

    // --- BREAK (BRK) ---
    lookup[0x00] = { "BRK", &CPU::BRK, &CPU::IMP, 7 };

    // --- BRANCH ON OVERFLOW CLEAR (BVC) ---
    lookup[0x50] = { "BVC", &CPU::BVC, &CPU::REL, 2 };

    // --- BRANCH ON OVERFLOW SET (BVS) ---
    lookup[0x70] = { "BVS", &CPU::BVS, &CPU::REL, 2 };

    // --- CLEAR CARRY (CLC) ---
    lookup[0x18] = { "CLC", &CPU::CLC, &CPU::IMP, 2 };

    // --- CLEAR DECIMAL (CLD) ---
    lookup[0xD8] = { "CLD", &CPU::CLD, &CPU::IMP, 2 };

    // --- CLEAR INTERRUPT DISABLE (CLI) ---
    lookup[0x58] = { "CLI", &CPU::CLI, &CPU::IMP, 2 };

    // --- CLEAR OVERFLOW (CLV) ---
    lookup[0xB8] = { "CLV", &CPU::CLV, &CPU::IMP, 2 };

    // --- COMPARE ACCUMULATOR (CMP) ---
    lookup[0xC9] = { "CMP", &CPU::CMP, &CPU::IMM, 2 };
    lookup[0xC5] = { "CMP", &CPU::CMP, &CPU::ZP0, 3 };
    lookup[0xD5] = { "CMP", &CPU::CMP, &CPU::ZPX, 4 };
    lookup[0xCD] = { "CMP", &CPU::CMP, &CPU::ABS, 4 };
    lookup[0xDD] = { "CMP", &CPU::CMP, &CPU::ABX, 4 };
    lookup[0xD9] = { "CMP", &CPU::CMP, &CPU::ABY, 4 };
    lookup[0xC1] = { "CMP", &CPU::CMP, &CPU::IZX, 6 };
    lookup[0xD1] = { "CMP", &CPU::CMP, &CPU::IZY, 5 };

    // --- COMPARE X (CPX) ---
    lookup[0xE0] = { "CPX", &CPU::CPX, &CPU::IMM, 2 };
    lookup[0xE4] = { "CPX", &CPU::CPX, &CPU::ZP0, 3 };
    lookup[0xEC] = { "CPX", &CPU::CPX, &CPU::ABS, 4 };

    // --- COMPARE Y (CPY) ---
    lookup[0xC0] = { "CPY", &CPU::CPY, &CPU::IMM, 2 };
    lookup[0xC4] = { "CPY", &CPU::CPY, &CPU::ZP0, 3 };
    lookup[0xCC] = { "CPY", &CPU::CPY, &CPU::ABS, 4 };

    // --- DECREMENT MEMORY (DEC) ---
    lookup[0xC6] = { "DEC", &CPU::DEC, &CPU::ZP0, 5 };
    lookup[0xD6] = { "DEC", &CPU::DEC, &CPU::ZPX, 6 };
    lookup[0xCE] = { "DEC", &CPU::DEC, &CPU::ABS, 6 };
    lookup[0xDE] = { "DEC", &CPU::DEC, &CPU::ABX, 7 };

    // --- DECREMENT X (DEX) ---
    lookup[0xCA] = { "DEX", &CPU::DEX, &CPU::IMP, 2 };

    // --- DECREMENT Y (DEY) ---
    lookup[0x88] = { "DEY", &CPU::DEY, &CPU::IMP, 2 };

    // --- EXCLUSIVE OR (EOR) ---
    lookup[0x49] = { "EOR", &CPU::EOR, &CPU::IMM, 2 };
    lookup[0x45] = { "EOR", &CPU::EOR, &CPU::ZP0, 3 };
    lookup[0x55] = { "EOR", &CPU::EOR, &CPU::ZPX, 4 };
    lookup[0x4D] = { "EOR", &CPU::EOR, &CPU::ABS, 4 };
    lookup[0x5D] = { "EOR", &CPU::EOR, &CPU::ABX, 4 };
    lookup[0x59] = { "EOR", &CPU::EOR, &CPU::ABY, 4 };
    lookup[0x41] = { "EOR", &CPU::EOR, &CPU::IZX, 6 };
    lookup[0x51] = { "EOR", &CPU::EOR, &CPU::IZY, 5 };

    // --- INCREMENT MEMORY (INC) ---
    lookup[0xE6] = { "INC", &CPU::INC, &CPU::ZP0, 5 };
    lookup[0xF6] = { "INC", &CPU::INC, &CPU::ZPX, 6 };
    lookup[0xEE] = { "INC", &CPU::INC, &CPU::ABS, 6 };
    lookup[0xFE] = { "INC", &CPU::INC, &CPU::ABX, 7 };

    // --- INCREMENT X (INX) ---
    lookup[0xE8] = { "INX", &CPU::INX, &CPU::IMP, 2 };

    // --- INCREMENT Y (INY) ---
    lookup[0xC8] = { "INY", &CPU::INY, &CPU::IMP, 2 };

    // --- JUMP (JMP) ---
    lookup[0x4C] = { "JMP", &CPU::JMP, &CPU::ABS, 3 };
    lookup[0x6C] = { "JMP", &CPU::JMP, &CPU::IND, 5 };

    // --- JUMP SUBROUTINE (JSR) ---
    lookup[0x20] = { "JSR", &CPU::JSR, &CPU::ABS, 6 };

    // --- LOAD ACCUMULATOR (LDA) ---
    lookup[0xA9] = { "LDA", &CPU::LDA, &CPU::IMM, 2 };
    lookup[0xA5] = { "LDA", &CPU::LDA, &CPU::ZP0, 3 };
    lookup[0xB5] = { "LDA", &CPU::LDA, &CPU::ZPX, 4 };
    lookup[0xAD] = { "LDA", &CPU::LDA, &CPU::ABS, 4 };
    lookup[0xBD] = { "LDA", &CPU::LDA, &CPU::ABX, 4 };
    lookup[0xB9] = { "LDA", &CPU::LDA, &CPU::ABY, 4 };
    lookup[0xA1] = { "LDA", &CPU::LDA, &CPU::IZX, 6 };
    lookup[0xB1] = { "LDA", &CPU::LDA, &CPU::IZY, 5 };

    // --- LOAD X (LDX) ---
    lookup[0xA2] = { "LDX", &CPU::LDX, &CPU::IMM, 2 };
    lookup[0xA6] = { "LDX", &CPU::LDX, &CPU::ZP0, 3 };
    lookup[0xB6] = { "LDX", &CPU::LDX, &CPU::ZPY, 4 };
    lookup[0xAE] = { "LDX", &CPU::LDX, &CPU::ABS, 4 };
    lookup[0xBE] = { "LDX", &CPU::LDX, &CPU::ABY, 4 };

    // --- LOAD Y (LDY) ---
    lookup[0xA0] = { "LDY", &CPU::LDY, &CPU::IMM, 2 };
    lookup[0xA4] = { "LDY", &CPU::LDY, &CPU::ZP0, 3 };
    lookup[0xB4] = { "LDY", &CPU::LDY, &CPU::ZPX, 4 };
    lookup[0xAC] = { "LDY", &CPU::LDY, &CPU::ABS, 4 };
    lookup[0xBC] = { "LDY", &CPU::LDY, &CPU::ABX, 4 };

    // --- LOGICAL SHIFT RIGHT (LSR) ---
    lookup[0x4A] = { "LSR", &CPU::LSR, &CPU::IMP, 2 };
    lookup[0x46] = { "LSR", &CPU::LSR, &CPU::ZP0, 5 };
    lookup[0x56] = { "LSR", &CPU::LSR, &CPU::ZPX, 6 };
    lookup[0x4E] = { "LSR", &CPU::LSR, &CPU::ABS, 6 };
    lookup[0x5E] = { "LSR", &CPU::LSR, &CPU::ABX, 7 };

    // --- NO OPERATION (NOP) ---
    lookup[0xEA] = { "NOP", &CPU::NOP, &CPU::IMP, 2 };

    // --- LOGICAL OR (ORA) ---
    lookup[0x09] = { "ORA", &CPU::ORA, &CPU::IMM, 2 };
    lookup[0x05] = { "ORA", &CPU::ORA, &CPU::ZP0, 3 };
    lookup[0x15] = { "ORA", &CPU::ORA, &CPU::ZPX, 4 };
    lookup[0x0D] = { "ORA", &CPU::ORA, &CPU::ABS, 4 };
    lookup[0x1D] = { "ORA", &CPU::ORA, &CPU::ABX, 4 };
    lookup[0x19] = { "ORA", &CPU::ORA, &CPU::ABY, 4 };
    lookup[0x01] = { "ORA", &CPU::ORA, &CPU::IZX, 6 };
    lookup[0x11] = { "ORA", &CPU::ORA, &CPU::IZY, 5 };

    // --- PUSH ACCUMULATOR (PHA) ---
    lookup[0x48] = { "PHA", &CPU::PHA, &CPU::IMP, 3 };

    // --- PUSH PROCESSOR STATUS (PHP) ---
    lookup[0x08] = { "PHP", &CPU::PHP, &CPU::IMP, 3 };

    // --- PULL ACCUMULATOR (PLA) ---
    lookup[0x68] = { "PLA", &CPU::PLA, &CPU::IMP, 4 };

    // --- PULL PROCESSOR STATUS (PLP) ---
    lookup[0x28] = { "PLP", &CPU::PLP, &CPU::IMP, 4 };

    // --- ROTATE LEFT (ROL) ---
    lookup[0x2A] = { "ROL", &CPU::ROL, &CPU::IMP, 2 };
    lookup[0x26] = { "ROL", &CPU::ROL, &CPU::ZP0, 5 };
    lookup[0x36] = { "ROL", &CPU::ROL, &CPU::ZPX, 6 };
    lookup[0x2E] = { "ROL", &CPU::ROL, &CPU::ABS, 6 };
    lookup[0x3E] = { "ROL", &CPU::ROL, &CPU::ABX, 7 };

    // --- ROTATE RIGHT (ROR) ---
    lookup[0x6A] = { "ROR", &CPU::ROR, &CPU::IMP, 2 };
    lookup[0x66] = { "ROR", &CPU::ROR, &CPU::ZP0, 5 };
    lookup[0x76] = { "ROR", &CPU::ROR, &CPU::ZPX, 6 };
    lookup[0x6E] = { "ROR", &CPU::ROR, &CPU::ABS, 6 };
    lookup[0x7E] = { "ROR", &CPU::ROR, &CPU::ABX, 7 };

    // --- RETURN FROM INTERRUPT (RTI) ---
    lookup[0x40] = { "RTI", &CPU::RTI, &CPU::IMP, 6 };

    // --- RETURN FROM SUBROUTINE (RTS) ---
    lookup[0x60] = { "RTS", &CPU::RTS, &CPU::IMP, 6 };

    // --- SUBTRACT WITH CARRY (SBC) ---
    lookup[0xE9] = { "SBC", &CPU::SBC, &CPU::IMM, 2 };
    lookup[0xE5] = { "SBC", &CPU::SBC, &CPU::ZP0, 3 };
    lookup[0xF5] = { "SBC", &CPU::SBC, &CPU::ZPX, 4 };
    lookup[0xED] = { "SBC", &CPU::SBC, &CPU::ABS, 4 };
    lookup[0xFD] = { "SBC", &CPU::SBC, &CPU::ABX, 4 };
    lookup[0xF9] = { "SBC", &CPU::SBC, &CPU::ABY, 4 };
    lookup[0xE1] = { "SBC", &CPU::SBC, &CPU::IZX, 6 };
    lookup[0xF1] = { "SBC", &CPU::SBC, &CPU::IZY, 5 };

    // --- SET CARRY (SEC) ---
    lookup[0x38] = { "SEC", &CPU::SEC, &CPU::IMP, 2 };

    // --- SET DECIMAL MODE (SED) ---
    lookup[0xF8] = { "SED", &CPU::SED, &CPU::IMP, 2 };

    // --- SET INTERRUPT DISABLE (SEI) ---
    lookup[0x78] = { "SEI", &CPU::SEI, &CPU::IMP, 2 };

    // --- STORE ACCUMULATOR (STA) ---
    lookup[0x85] = { "STA", &CPU::STA, &CPU::ZP0, 3 };
    lookup[0x95] = { "STA", &CPU::STA, &CPU::ZPX, 4 };
    lookup[0x8D] = { "STA", &CPU::STA, &CPU::ABS, 4 };
    lookup[0x9D] = { "STA", &CPU::STA, &CPU::ABX, 5 };
    lookup[0x99] = { "STA", &CPU::STA, &CPU::ABY, 5 };
    lookup[0x81] = { "STA", &CPU::STA, &CPU::IZX, 6 };
    lookup[0x91] = { "STA", &CPU::STA, &CPU::IZY, 6 };

    // --- STORE X (STX) ---
    lookup[0x86] = { "STX", &CPU::STX, &CPU::ZP0, 3 };
    lookup[0x96] = { "STX", &CPU::STX, &CPU::ZPY, 4 };
    lookup[0x8E] = { "STX", &CPU::STX, &CPU::ABS, 4 };

    // --- STORE Y (STY) ---
    lookup[0x84] = { "STY", &CPU::STY, &CPU::ZP0, 3 };
    lookup[0x94] = { "STY", &CPU::STY, &CPU::ZPX, 4 };
    lookup[0x8C] = { "STY", &CPU::STY, &CPU::ABS, 4 };

    // --- TRANSFER ACCUMULATOR TO X (TAX) ---
    lookup[0xAA] = { "TAX", &CPU::TAX, &CPU::IMP, 2 };

    // --- TRANSFER ACCUMULATOR TO Y (TAY) ---
    lookup[0xA8] = { "TAY", &CPU::TAY, &CPU::IMP, 2 };

    // --- TRANSFER STACK POINTER TO X (TSX) ---
    lookup[0xBA] = { "TSX", &CPU::TSX, &CPU::IMP, 2 };

    // --- TRANSFER X TO ACCUMULATOR (TXA) ---
    lookup[0x8A] = { "TXA", &CPU::TXA, &CPU::IMP, 2 };

    // --- TRANSFER X TO STACK POINTER (TXS) ---
    lookup[0x9A] = { "TXS", &CPU::TXS, &CPU::IMP, 2 };

    // --- TRANSFER Y TO ACCUMULATOR (TYA) ---
    lookup[0x98] = { "TYA", &CPU::TYA, &CPU::IMP, 2 };
}

uint8_t CPU::read(uint16_t addr) {
    return bus->read(addr, false);
}

void CPU::write(uint16_t addr, uint8_t data) {
    bus->write(addr, data);
}

uint8_t CPU::fetch() {
    if (lookup[opcode].addr_mode != &CPU::IMP){
        fetched = read(addr_abs);
    }

    return fetched;
}

uint8_t CPU::get_flag(StatusFlag f){
    return ((status & f) > 0) ? 1 : 0;
}

void CPU::set_flag(StatusFlag f, bool v) {
    if (v) {
        status |= f;
    } else {
        status &= ~f;
    }
}

// Interrupts
// Reset 
void CPU::reset(){
    uint16_t lo = read(0xFFFC);
    uint16_t hi = read(0xFFFD);
    pc = (hi << 8) | lo;

    a = 0x00;
    x = 0x00;
    y = 0x00;
    sp = 0xFD;

    set_flag(I, true);
    set_flag(U, true);

    cycles = 7;
}

// IRQ
void CPU::irq() {
    if (get_flag(I))
        return;
    stack_push((pc >> 8) & 0x00FF);
    stack_push(pc & 0x00FF);
    set_flag(B, false);
    set_flag(I, true);
    stack_push(status);

    uint16_t lo = read(0xFFFE);
    uint16_t hi = read(0xFFFF);
    pc = (hi << 8) | lo;

    cycles = 7;
}

// NMI
void CPU::nmi() {
    stack_push((pc >> 8) & 0x00FF);
    stack_push(pc & 0x00FF);
    set_flag(B, false);
    set_flag(I, true);
    stack_push(status);

    uint16_t lo = read(0xFFFA);
    uint16_t hi = read(0xFFFB);
    pc = (hi << 8) | lo;
    
    cycles = 7;   
}


// Stack operations
void CPU::stack_push(uint8_t data) {
    write(0x0100 + sp, data);
    sp--;
}

uint8_t CPU::stack_pop() {
    sp++;
    return read(0x0100 + sp);
}

void CPU::clock() {
    if (cycles == 0) {
        opcode = read(pc++);

        cycles = lookup[opcode].cycles;

        uint8_t additional_cycle1 = (this->*lookup[opcode].addr_mode)();
        uint8_t additional_cycle2 = (this->*lookup[opcode].operate)();

        cycles += (additional_cycle1 & additional_cycle2);
    }
    cycles--;
    total_cycles++;
}


// Addressing Modes
uint8_t CPU::IMP() {
    fetched = a;
    return 0;
}

uint8_t CPU::IMM() {
    addr_abs = pc++;
    return 0;
}

uint8_t CPU::ZP0() {
    addr_abs = read(pc++);
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t CPU::ZPX() {
    addr_abs = read(pc++) + x;
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t CPU::ZPY() {
    addr_abs = read(pc++) + y;
    addr_abs &= 0x00FF;
    return 0;
}

uint8_t CPU::ABS() {
    uint8_t low = read(pc++);
    uint8_t high = read(pc++);
    addr_abs = (high << 8) | low;
    return 0;
}

uint8_t CPU::ABX() {
    uint8_t low = read(pc++);
    uint8_t high = read(pc++);
    addr_abs = (high << 8) | low;
    addr_abs += x;

    if ((addr_abs & 0xFF00) != (high << 8)) {
        return 1;
    } else {
        return 0;
    }
}

uint8_t CPU::ABY() {
    uint8_t low = read(pc++);
    uint8_t high = read(pc++);
    addr_abs = (high << 8) | low;
    addr_abs += y;

    if ((addr_abs & 0xFF00) != (high << 8)) {
        return 1;
    } else {
        return 0;
    }
}

uint8_t CPU::REL() {
    addr_rel = read(pc++);
    if (addr_rel & 0x80) {
        addr_rel |= 0xFF00;
    }
    return 0;
}

uint8_t CPU::IND() {
    uint16_t ptr_lo = read(pc++);
    uint16_t ptr_hi = read(pc++);
    uint16_t ptr = (ptr_hi << 8) | ptr_lo;

    if (ptr_lo == 0x00FF) {
        addr_abs = (read(ptr & 0xFF00) << 8) | read(ptr);
    } else {
        addr_abs = (read(ptr + 1) << 8) | read(ptr);
    }
    return 0;
}

uint8_t CPU::IZX() {
    uint16_t t = read(pc++);
    uint16_t lo = read((static_cast<uint16_t>(t) + static_cast<uint16_t>(x)) & 0x00FF);
    uint16_t hi = read((static_cast<uint16_t>(t) + static_cast<uint16_t>(x) + 1) & 0x00FF);
    addr_abs = (hi << 8) | lo;
    return 0;
}

uint8_t CPU::IZY() {
    uint16_t t = read(pc++);
    uint16_t lo = read(t & 0x00FF);
    uint16_t hi = read((t + 1) & 0x00FF);
    addr_abs = (hi << 8) | lo;
    addr_abs += y;

    if ((addr_abs & 0xFF00) != (hi << 8))
        return 1;
    else
    return 0;
}


// opcodes

uint8_t CPU::ADC(){
    fetch();
    uint16_t temp = static_cast<uint16_t>(a) + static_cast<uint16_t>(fetched) + static_cast<uint16_t>(get_flag(C));
    set_flag(C, temp > 0x00FF);
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(V, (~(static_cast<uint16_t>(a) ^ static_cast<uint16_t>(fetched)) & (static_cast<uint16_t>(a) ^ static_cast<uint16_t>(temp))) & 0x0080);
    set_flag(N, temp & 0x80);
    a = temp & 0x00FF;
    return 1;
}

uint8_t CPU::AND(){
    fetch();
    a = a & fetched;
    set_flag(Z, (a & 0x00FF) == 0x0000);
    set_flag(N, a & 0x80);
    return 1;
}

uint8_t CPU::ASL() {
    fetch();
    uint16_t temp = static_cast<uint16_t>(fetched) << 1;
    set_flag(C, temp & 0xFF00);
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(N, temp & 0x80);

    if (lookup[opcode].addr_mode == &CPU::IMP) {
        a = temp & 0x00FF;
    } else {
        write(addr_abs, temp & 0x00FF);
    }

    return 0;
}

void CPU::branch() {
    cycles++;
    addr_abs = pc + addr_rel;
    if ((addr_abs & 0xFF00) != (pc & 0xFF00)) {
        cycles++;
    }
    pc = addr_abs;
}

uint8_t CPU::BCC(){
    if(get_flag(C) == 0) branch();
    return 0;
}

uint8_t CPU::BCS(){
    if(get_flag(C) == 1) branch();
    return 0;
}

uint8_t CPU::BEQ(){
    if(get_flag(Z) == 1) branch();
    return 0;
}

uint8_t CPU::BIT() {
    fetch();
    set_flag(Z, (a & fetched) == 0x00);
    set_flag(V, fetched & 0x40);
    set_flag(N, fetched & 0x80);
    return 0;

}

uint8_t CPU::BMI() {
    if(get_flag(N) == 1) branch();
    return 0;
}

uint8_t CPU::BNE() {
    if(get_flag(Z) == 0) branch();
    return 0;
}

uint8_t CPU::BPL() {
    if(get_flag(N) == 0) branch();
    return 0;
}

uint8_t CPU::BRK() {
    pc++;

    stack_push((pc >> 8) & 0x00FF);
    stack_push(pc & 0x00FF);

    set_flag(B, true);
    stack_push(status);
    set_flag(I, true);

    uint16_t lo = read(0xFFFE);
    uint16_t hi = read(0xFFFF);
    pc = (hi << 8) | lo;
    return 0;
}

uint8_t CPU::BVC() {
    if(get_flag(V) == 0) branch();
    return 0;
}

uint8_t CPU::BVS() {
    if(get_flag(V) == 1) branch();
    return 0;
}

uint8_t CPU::CLC() {
    set_flag(C, false);
    return 0;
}

uint8_t CPU::CLD() {
    set_flag(D, false);
    return 0;
}

uint8_t CPU::CLI() {
    set_flag(I, false);
    return 0;
}

uint8_t CPU::CLV(){
    set_flag(V, false);
    return 0;
}

uint8_t CPU::CMP() {
    fetch();
    uint16_t temp = static_cast<uint16_t>(a) - static_cast<uint16_t>(fetched);
    set_flag(C, a >= fetched);
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(N, temp & 0x80);
    return 1;
}

uint8_t CPU::CPX() {
    fetch();
    uint16_t temp = static_cast<uint16_t>(x) - static_cast<uint16_t>(fetched);
    set_flag(C, x >= fetched);
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(N, temp & 0x80);
    return 0;
}

uint8_t CPU::CPY() {
    fetch();
    uint16_t temp = static_cast<uint16_t>(y) - static_cast<uint16_t>(fetched);
    set_flag(C, y >= fetched);
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(N, temp & 0x80);
    return 0;
}

uint8_t CPU::DEC(){
    fetch();
    uint16_t temp = static_cast<uint16_t>(fetched) - 1;
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(N, temp & 0x80);
    return 0;
}

uint8_t CPU::DEX() {
    x--;
    set_flag(Z, x == 0x00);
    set_flag(N, x & 0x80);
    return 0;
}

uint8_t CPU::DEY() {
    y--;
    set_flag(Z, y == 0x00);
    set_flag(N, y & 0x80);
    return 0;
}

uint8_t CPU::EOR() {
    fetch();
    a = a ^ fetched;
    set_flag(Z, (a & 0x00FF) == 0x0000);
    set_flag(N, a & 0x80);
     return 1;
}

uint8_t CPU::INC() {
    fetch();
    uint16_t temp = static_cast<uint16_t>(fetched) + 1;
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(N, temp & 0x80);
    return 0;
}

uint8_t CPU::INX() {
    x++;
    set_flag(Z, x == 0x00);
    set_flag(N, x & 0x80);
    return 0;
}

uint8_t CPU::INY() {
    y++;
    set_flag(Z, y == 0x00);
    set_flag(N, y & 0x80);
    return 0;
}

uint8_t CPU::JMP() {
    pc = addr_abs;
    return 0;
}

uint8_t CPU::JSR() {
    pc--;

    stack_push((pc >> 8) & 0x00FF);
    stack_push(pc & 0x00FF);

    pc = addr_abs;
    return 0;
}

// --- LOAD & STORE ---
uint8_t CPU::LDA() {
    fetch();
    a = fetched;
    set_flag(Z, a == 0x00);
    set_flag(N, a & 0x80);
    return 1;
}

uint8_t CPU::LDX() {
    fetch();
    x = fetched;
    set_flag(Z, x == 0x00);
    set_flag(N, x & 0x80);
    return 1;
}

uint8_t CPU::LDY() {
    fetch();
    y = fetched;
    set_flag(Z, y == 0x00);
    set_flag(N, y & 0x80);
    return 1;
}

uint8_t CPU::LSR() {
    fetch();
    set_flag(C, fetched & 0x01);
    uint16_t temp = fetched >> 1;
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(N, temp & 0x80);

    if (lookup[opcode].addr_mode == &CPU::IMP) {
        a = temp & 0x00FF;
    } else {
        write(addr_abs, temp & 0x00FF);
    }

    return 0;
}

uint8_t CPU::NOP() {
    return 0;
}

uint8_t CPU::ORA() {
    fetch();
    a = a | fetched;
    set_flag(Z, (a & 0x00FF) == 0x0000);
    set_flag(N, a & 0x80);
    return 1;
}

uint8_t CPU::PHA() {
    stack_push(a);
    return 0;
}

uint8_t CPU::PHP() {
    stack_push(status | B);
    return 0;
}

uint8_t CPU::PLA() {
    a = stack_pop();
    set_flag(Z, a == 0x00);
    set_flag(N, a & 0x80);
    return 0;
}

uint8_t CPU::PLP() {
    // ~B
    status = stack_pop();
    set_flag(U, true);
    return 0;
}

uint8_t CPU::ROL() {
    fetch();
    uint16_t temp = (static_cast<uint16_t>(fetched) << 1) | get_flag(C);
    set_flag(C, temp & 0xFF00);
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(N, temp & 0x80);

    if (lookup[opcode].addr_mode == &CPU::IMP) {
        a = temp & 0x00FF;
    } else {
        write(addr_abs, temp & 0x00FF);
    }

    return 0;
}

uint8_t CPU::ROR() {
    fetch();
    uint16_t temp = (static_cast<uint16_t>(get_flag(C)) << 7) | (fetched >> 1);
    set_flag(C, fetched & 0x01);
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(N, temp & 0x80);

    if (lookup[opcode].addr_mode == &CPU::IMP) {
        a = temp & 0x00FF;
    } else {
        write(addr_abs, temp & 0x00FF);
    }

    return 0;
}

uint8_t CPU::RTI() {
    status = stack_pop();
    set_flag(U, true);
    uint16_t lo = stack_pop();
    uint16_t hi = stack_pop();
    pc = (hi << 8) | lo;
    return 0;
}

uint8_t CPU::RTS() {
    uint16_t lo = stack_pop();
    uint16_t hi = stack_pop();
    pc = ((hi << 8) | lo) + 1;
    return 0;
}

uint8_t CPU::SBC() {
    fetch();
    uint16_t temp = static_cast<uint16_t>(a) - static_cast<uint16_t>(fetched) - static_cast<uint16_t>(1 - get_flag(C));
    set_flag(C, temp > 0x00FF);
    set_flag(Z, (temp & 0x00FF) == 0x0000);
    set_flag(V, ((static_cast<uint16_t>(a) ^ static_cast<uint16_t>(fetched)) & (static_cast<uint16_t>(a) ^ static_cast<uint16_t>(temp))) & 0x0080);
    set_flag(N, temp & 0x80);
    a = temp & 0x00FF;
    return 1;
}

uint8_t CPU::SEC() {
    set_flag(C, true);
    return 0;
}

uint8_t CPU::SED() {
    set_flag(D, true);
    return 0;
}

uint8_t CPU::SEI() {
    set_flag(I, true);
    return 0;
}

uint8_t CPU::STA() {
    write(addr_abs, a);
    return 0;
}

uint8_t CPU::STX() {
    write(addr_abs, x);
    return 0;
}

uint8_t CPU::STY() {
    write(addr_abs, y);
    return 0;
}

uint8_t CPU::TAX() {
    x = a;
    set_flag(Z, x == 0x00);
    set_flag(N, x & 0x80);
    return 0;
}

uint8_t CPU::TAY() {
    y = a;
    set_flag(Z, y == 0x00);
    set_flag(N, y & 0x80);
    return 0;
}

uint8_t CPU::TSX() {
    x = sp;
    set_flag(Z, x == 0x00);
    set_flag(N, x & 0x80);
    return 0;
}

uint8_t CPU::TXA() {
    a = x;
    set_flag(Z, a == 0x00);
    set_flag(N, a & 0x80);
    return 0;
}

uint8_t CPU::TXS() {
    sp = x;
    return 0;
}

uint8_t CPU::TYA() {
    a = y;
    set_flag(Z, a == 0x00);
    set_flag(N, a & 0x80);
    return 0;
}

// --- ILLEGAL OPCODE ---
uint8_t CPU::XXX() {
    return 0;
}
