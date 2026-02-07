#pragma once

#include <array>
#include <cstdint>

class CPU;

class Bus {
    private:
        CPU *cpu = nullptr;
        std::array<uint8_t, 2048> cpu_ram;
  
    public:
        Bus();
    
        void connect_cpu(CPU* ptr) {cpu = ptr;}

        uint8_t read(uint16_t addr, bool b_read_only = false);
        void write(uint16_t addr, uint8_t data);
};
