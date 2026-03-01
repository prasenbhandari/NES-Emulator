
#include "core/Bus.h"
#include "core/CPU.h"
#include "core/PPU.h"
#include "platform/Platform.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if(argc < 2){
        std::cerr << "Usage: " << argv[0] << " <path_to_rom>" << std::endl;
        return 1;
    }

    Bus bus;
    CPU cpu;
    PPU ppu;
    Cartridge cartridge;
    Platform platform;

    if (!cartridge.load(argv[1])) {
        std::cerr << "Failed to load: " << argv[1] << std::endl;
        return 1;
    }

    bus.connect_cpu(&cpu);
    bus.connect_cartridge(&cartridge);
    bus.connect_ppu(&ppu);
    cpu.connect_bus(&bus);
    ppu.connect_cartridge(&cartridge);

    // Reset CPU to start at the RESET vector ($FFFC)
    cpu.reset();
    ppu.reset();

    // Initialize platform
    platform.init_window(256, 240);

    std::cout << "Emulator running..." << std::endl;

    // Main emulation loop
    while (!platform.should_close()) {

        // Execute one CPU instruction
        cpu.clock();
        
        uint8_t cpu_cycles = cpu.get_last_instruction_cycles();
        for (uint8_t i = 0; i < cpu_cycles * 3; i++) {
            ppu.clock();
            // Check for NMI from PPU during PPU execution
            if (ppu.nmi_occurred) {
                cpu.nmi();
                ppu.nmi_occurred = false;
            }
        }

        // Render when frame is complete
        if (ppu.frame_complete) {
            platform.handle_input(bus.controller_state);
            platform.render_frame(ppu.get_frame_buffer());
            ppu.frame_complete = false;
        }
    }

    std::cout << "Emulator closed." << std::endl;
    return 0;
}
