#include "core/Mappers/MMC1.h"

MMC1::MMC1(uint8_t prg_rom_banks, uint8_t chr_rom_banks, std::vector<uint8_t> &prg_rom, std::vector<uint8_t> &chr_rom, std::vector<uint8_t> &prg_ram) 
    : Mapper(prg_rom_banks, chr_rom_banks, prg_rom, chr_rom), prg_ram(prg_ram) {
}

MMC1::~MMC1() {

}

void MMC1::update_offsets() {
    uint8_t prg_mode = (control_register >> 2) & 0x03;
    uint8_t prg_bank = (prg_bank_register & 0x0F) & ((prg_rom_banks) - 1);

    switch (prg_mode) {
        case 0:
        case 1:
            prg_bank_offset[0] = (prg_bank & 0xFE) * 0x4000;
            prg_bank_offset[1] = (prg_bank | 0x01) * 0x4000;
            break;
        case 2:
            prg_bank_offset[0] = 0;
            prg_bank_offset[1] = prg_bank * 0x4000;
            break;
        case 3:
            prg_bank_offset[0] = prg_bank * 0x4000;
            prg_bank_offset[1] = (prg_rom_banks - 1) * 0x4000;
            break;
    }

    uint8_t chr_mode = (control_register >> 4) & 0x01;

    if (chr_rom_banks == 0){
        chr_bank_offset[0] = 0;
        chr_bank_offset[1] = 0x1000;
    }else {
        if (chr_mode == 0){
            uint8_t chr_bank = chr_bank0_register & 0xFE;

            chr_bank_offset[0] = (chr_bank) * 0x1000;
            chr_bank_offset[1] = (chr_bank | 0x01) * 0x1000;
        } else {
            chr_bank_offset[0] = chr_bank0_register * 0x1000;
            chr_bank_offset[1] = chr_bank1_register * 0x1000;
        }
    }
}


void MMC1::cpu_write(uint16_t addr, uint8_t data){
    if (addr >= 0x6000 && addr < 0x8000){
        prg_ram[addr & 0x1FFF] = data;
        return;
    }

    if (data & 0x80){
        shift_register = 0x10;
        control_register |= 0x0C;
        update_offsets();
        return;
    }

    bool is_fifth_write = shift_register & 0x01;

    shift_register >>= 1;
    shift_register |= (data & 0x01) << 4;

    if (is_fifth_write){
        if (addr >= 0x8000 && addr <= 0x9FFF){
            control_register = shift_register;
            switch (control_register & 0x03){
                case 0:
                    mirror_mode = MirrorMode::SINGLE_SCREEN_LOWER;
                    break;
                case 1:
                    mirror_mode = MirrorMode::SINGLE_SCREEN_UPPER;
                    break;
                case 2:
                    mirror_mode = MirrorMode::VERTICAL;
                    break;
                case 3:
                    mirror_mode = MirrorMode::HORIZONTAL;
                    break;
            }
        } else if (addr >= 0xA000 && addr <= 0xBFFF){
            chr_bank0_register = shift_register;
        } else if (addr >= 0xC000 && addr <= 0xDFFF){
            chr_bank1_register = shift_register;
        } else if (addr >= 0xE000 && addr <= 0xFFFF){
            prg_bank_register = shift_register;
        }
        update_offsets();

        shift_register = 0x10;
    }
}

uint8_t MMC1::cpu_read(uint16_t addr){
    if (addr >= 0x6000 && addr < 0x8000){
        return prg_ram[addr & 0x1FFF];
    } else if (addr >= 0x8000 && addr <= 0xBFFF){
        return prg_rom[prg_bank_offset[0] + (addr & 0x3FFF)];
    } else if (addr >= 0xC000 && addr <= 0xFFFF){
        return prg_rom[prg_bank_offset[1] + (addr & 0x3FFF)];
    }
    return 0x00;
}


uint8_t MMC1::ppu_read(uint16_t addr){
    if (addr < 0x1000){
        return chr_rom[chr_bank_offset[0] + addr];
    } else if (addr >= 0x1000 && addr < 0x2000){
        return chr_rom[chr_bank_offset[1] + (addr & 0x0FFF)];
    }
    return 0x00;
}

void MMC1::ppu_write(uint16_t addr, uint8_t data){
    if (chr_rom_banks == 0){
        if (addr < 0x1000){
            chr_rom[chr_bank_offset[0] + addr] = data;
        } else if (addr >= 0x1000 && addr < 0x2000){
            chr_rom[chr_bank_offset[1] + (addr & 0x0FFF)] = data;
        }
    }
}