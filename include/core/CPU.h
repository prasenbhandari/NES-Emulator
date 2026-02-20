#pragma once

#include <cstdint>
#include <string>
#include <array>

class Bus;

class CPU {
    public:
        CPU();

        void connect_bus(Bus* n) { bus = n; }
        void clock();
        void reset();
        void irq();
        void nmi();

        struct DisassembledInstruction {
            uint16_t address;
            std::string hex_bytes;
            std::string instruction;
            std::string name;
            uint8_t length;
        };

        uint16_t get_pc() const { return pc; }
        uint32_t get_total_cycles() const { return total_cycles; }
        uint8_t get_last_instruction_cycles() const { return last_instruction_cycles; }
        uint8_t get_a() const { return a; }
        uint8_t get_x() const { return x; }
        uint8_t get_y() const { return y; }
        uint8_t get_sp() const { return sp; }
        uint8_t get_status() const { return status; }

    private:
        // Registers
        uint8_t a = 0x00;
        uint8_t x = 0x00;
        uint8_t y = 0x00;
        uint8_t sp = 0x00;  
        uint16_t pc = 0x0000;
        uint8_t status = 0x00;

        struct INSTRUCTION {
            std::string name;
            uint8_t (CPU::*operate)(void) = nullptr;
            uint8_t (CPU::*addr_mode)(void) = nullptr;
            uint8_t cycles = 0;
        };

        std::array<INSTRUCTION, 256> lookup;

        // Addressing Modes
        uint8_t IMP(); uint8_t IMM(); uint8_t ZP0(); uint8_t ZPX();
        uint8_t ZPY(); uint8_t REL(); uint8_t ABS(); uint8_t ABX();
        uint8_t ABY(); uint8_t IND(); uint8_t IZX(); uint8_t IZY();

        // Official Instructions
        uint8_t ADC(); uint8_t AND(); uint8_t ASL(); uint8_t BCC();
        uint8_t BCS(); uint8_t BEQ(); uint8_t BIT(); uint8_t BMI();
        uint8_t BNE(); uint8_t BPL(); uint8_t BRK(); uint8_t BVC();
        uint8_t BVS(); uint8_t CLC(); uint8_t CLD(); uint8_t CLI();
        uint8_t CLV(); uint8_t CMP(); uint8_t CPX(); uint8_t CPY();
        uint8_t DEC(); uint8_t DEX(); uint8_t DEY(); uint8_t EOR();
        uint8_t INC(); uint8_t INX(); uint8_t INY(); uint8_t JMP();
        uint8_t JSR(); uint8_t LDA(); uint8_t LDX(); uint8_t LDY();
        uint8_t LSR(); uint8_t NOP(); uint8_t ORA(); uint8_t PHA();
        uint8_t PHP(); uint8_t PLA(); uint8_t PLP(); uint8_t ROL();
        uint8_t ROR(); uint8_t RTI(); uint8_t RTS(); uint8_t SBC();
        uint8_t SEC(); uint8_t SED(); uint8_t SEI(); uint8_t STA();
        uint8_t STX(); uint8_t STY(); uint8_t TAX(); uint8_t TAY();
        uint8_t TSX(); uint8_t TXA(); uint8_t TXS(); uint8_t TYA();

        // Illegal instruction
        uint8_t LAX(); uint8_t SAX(); uint8_t DCP(); uint8_t ISB();
        uint8_t SLO(); uint8_t RLA(); uint8_t SRE(); uint8_t RRA();

        uint8_t XXX();

        // Internal state
        uint8_t fetched = 0x00;
        uint16_t addr_abs = 0x0000;
        uint16_t addr_rel = 0x0000;
        uint8_t opcode = 0x00;
        uint8_t cycles = 0;
        uint32_t total_cycles = 0;
        uint8_t last_instruction_cycles = 0;

        enum StatusFlag {
            C = (1 << 0),   // Carry
            Z = (1 << 1),   // Zero
            I = (1 << 2),   // Interrupt Disable
            D = (1 << 3),   // Decimal Mode
            B = (1 << 4),   // Break Command
            U = (1 << 5),   // Unused
            V = (1 << 6),   // Overflow
            N = (1 << 7),   // Negative
        };

        Bus* bus = nullptr;

        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);

        uint8_t fetch();
        uint8_t get_flag(StatusFlag f);
        void set_flag(StatusFlag f, bool v);

        // Stack operation
        void stack_push(uint8_t data);
        uint8_t stack_pop();

        void branch();
};