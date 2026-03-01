#include <iostream>
#include <fstream>

#include "core/Cartridge.h"
#include "core/MMC0.h"

Cartridge::Cartridge(){
    
}

Cartridge::~Cartridge() {
    if (mapper) {
        delete mapper;
        mapper = nullptr;
    }
}

bool Cartridge::load(const std::string &filename) {
    std::ifstream rom(filename, std::ios::binary);
    if (!rom) {
        return false;
    }

    rom.read(reinterpret_cast<char*>(&header), sizeof(INESHeader));

    if (std::string(header.name, 4) != "NES\x1A") {
        return false;
    }

    prg_rom.resize(header.prg_rom_chunks * 16384);
    
    if(header.mapper1 & 0x04) {
        rom.seekg(512, std::ios::cur);
    }

    rom.read(reinterpret_cast<char*>(prg_rom.data()), prg_rom.size());

    if (header.chr_rom_chunks == 0){
        chr_rom.resize(8192);
    } else {
        chr_rom.resize(header.chr_rom_chunks * 8192);
        rom.read(reinterpret_cast<char*>(chr_rom.data()), chr_rom.size());
    }

    mapper_id = ((header.mapper2 & 0xF0) | (header.mapper1 >> 4));
    std::cout << "In cartridge load" << std::endl;
    std::cout << "Mapper ID: " << (int)mapper_id << std::endl;

    prg_ram.resize(header.prg_ram_size * 8192, 0x00);

    if (mapper_id == 0) {
        mapper = new MMC0(header.prg_rom_chunks, header.chr_rom_chunks, prg_rom, chr_rom);
    }
    // TODO: Add other mappers here

    // Set mirroring mode from iNES header
    if (mapper) {
        MirrorMode mirror_mode = (header.mapper1 & 0x01) 
            ? MirrorMode::VERTICAL 
            : MirrorMode::HORIZONTAL;
        mapper->set_mirror_mode(mirror_mode);
    }

    rom.close();
    return true;
}

uint8_t Cartridge::cpu_read(uint16_t addr) {
    return mapper->cpu_read(addr);
}

void Cartridge::cpu_write(uint16_t addr, uint8_t data) {
    mapper->cpu_write(addr, data);
}

uint8_t Cartridge::ppu_read(uint16_t addr) {
    return mapper->ppu_read(addr);
}

void Cartridge::ppu_write(uint16_t addr, uint8_t data) {
    mapper->ppu_write(addr, data);
}

