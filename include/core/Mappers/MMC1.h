#include <array>

#include "core/Mapper.h"

class MMC1 : public Mapper {
    public:
        MMC1(uint8_t prg_rom_banks, uint8_t chr_rom_banks, std::vector<uint8_t> &prg_rom, std::vector<uint8_t> &chr_rom, std::vector<uint8_t> &prg_ram);
        ~MMC1();

        uint8_t cpu_read(uint16_t addr) override;
        void cpu_write(uint16_t addr, uint8_t data) override;

        uint8_t ppu_read(uint16_t addr) override;
        void ppu_write(uint16_t addr, uint8_t data) override;

    private:
        uint8_t shift_register = 0x10;
        uint8_t control_register = 0x0C;
        uint8_t prg_bank_register = 0x00;
        uint8_t chr_bank0_register = 0x00;
        uint8_t chr_bank1_register = 0x00;
        uint32_t prg_bank_offset[2] = {0, 0};
        uint32_t chr_bank_offset[2] = {0, 0};

        std::vector<uint8_t> &prg_ram;

        void update_offsets();
};