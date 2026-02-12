#pragma once

#include <array>
#include <cstdint>

#include "core/Cartridge.h"

class CPU;
class PPU;
class APU;

class Bus {
    public:
        Bus();
        
        void connect_cpu(CPU* ptr) { cpu = ptr; }
        void connect_cartridge(Cartridge* ptr) { cartridge = ptr; }
        void connect_ppu(PPU* ptr) { ppu = ptr; }
        void connect_apu(class APU* ptr) { apu = ptr; }
        uint8_t read(uint16_t addr);
        void write(uint16_t addr, uint8_t data);

    private:
        CPU* cpu = nullptr;
        PPU* ppu = nullptr;
        APU* apu = nullptr;
        Cartridge* cartridge = nullptr;
        std::array<uint8_t, 2048> cpu_ram;
};
