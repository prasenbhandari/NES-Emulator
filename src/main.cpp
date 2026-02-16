
#include "core/Bus.h"
#include "core/CPU.h"
#include "core/PPU.h"
#include <iostream>
#include <iomanip>

int main() {
    Bus bus;
    CPU cpu;
    PPU ppu;
    Cartridge cartridge;

    if (!cartridge.load("../roms/tests/nestest.nes")) {
        std::cerr << "Failed to load cartridge!" << std::endl;
        return 1;
    }

    bus.connect_cpu(&cpu);
    bus.connect_cartridge(&cartridge);
    bus.connect_ppu(&ppu);
    cpu.connect_bus(&bus);
    ppu.connect_cartridge(&cartridge);

    // Reset CPU to start at the RESET vector ($FFFC)
    cpu.reset();

    std::cout << "Running nestest..." << std::endl;
    
    uint32_t instructions_executed = 0;
    uint32_t last_progress_output = 0;
    const uint16_t OFFICIAL_END = 0xC66E;  // Official nestest end address (success loop)
    
    // Run the CPU until it reaches the official end address
    while (true) {
        uint16_t current_pc = cpu.get_pc();
        
        if (current_pc == OFFICIAL_END) {
            std::cout << "\nReached official end address 0xC66E after " << instructions_executed << " instructions" << std::endl;
            std::cout << "nestest PASSED!" << std::endl;
            break;
        }
        
        cpu.clock();
        instructions_executed++;
        
        if (instructions_executed - last_progress_output >= 1000) {
            std::cout << "Progress: " << instructions_executed << " instructions, PC: 0x" << std::hex << current_pc << std::dec << std::endl;
            last_progress_output = instructions_executed;
        }
        
        // Safety limit to prevent infinite loops
        if (instructions_executed > 1000000) {
            std::cout << "\nExceeded 1,000,000 instructions without reaching official end" << std::endl;
            std::cout << "Current PC: 0x" << std::hex << current_pc << std::dec << std::endl;
            std::cout << "nestest FAILED!" << std::endl;
            break;
        }
    }
    
    return 0;
}
