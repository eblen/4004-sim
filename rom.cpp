#include <array>
#include <vector>
#include "rom.h"
#include "types.h"

void ROM4001::load(const std::vector<Byte>& bytes)
{
    if (bytes.size() > 256)
    {
        throw std::runtime_error("Code exceeds ROM capacity of 256 bytes");
    }

    std::array<Byte,256> rom_mirror;
    rom_mirror.fill(0);
    std::copy(bytes.begin(), bytes.end(), rom_mirror.begin());

    load(rom_mirror);
}

void load_rom_rack(std::shared_ptr<rom_rack> rr, const std::vector<Byte>& bytes)
{
    if (bytes.size() > rr->size() * 256)
    {
        throw std::runtime_error("Code exceeds ROM rack capacity");
    }

    auto bb = bytes.begin();
    std::array<Byte,256> code;
    for (size_t rnum = 0; rnum < rr->size(); rnum++)
    {
        code.fill(0);
        size_t first_byte = rnum * 256;
        if (first_byte < bytes.size())
        {
            size_t last_byte = std::min(first_byte + 256, bytes.size());
            std::copy(bb+first_byte, bb+last_byte, code.begin());
            (*rr)[rnum].load(code);
        }
    }
}

