#include <array>
#include "rom.h"
#include "ram.h"
#include "types.h"

class CPU4004
{
    public:
    CPU4004() :carry(0), acc(0), current_ip(0), sel_rom(0), sel_ram(0), sel_ram_reg(0) {}
    void run();
    void connect(std::shared_ptr<rom_rack>   r) {roms = r;}
    void connect(std::shared_ptr<ram_rack>   r) {rams = r;}
    void connect(std::shared_ptr<testdevice> t) {tdev = t;}

    private:
    // Basic hardware (flags, pins, and registers)
    Bit carry;
    Nibble acc;
    std::array<Nibble,16> reg;
    Crumb current_ip;
    std::array<Addr,4> ip;
    std::shared_ptr<rom_rack> roms;
    std::shared_ptr<ram_rack> rams;
    std::shared_ptr<testdevice> tdev;

    // Selected ROM and RAM values (set with the "SRC" command)
    Nibble sel_rom;
    Crumb  sel_ram;
    Crumb  sel_ram_reg;
    Nibble sel_ram_mmc; // selected main memory character

    // Functions for processing instructions
    std::pair<Byte,Byte> read_instr();
    void exec_instr(Byte b1, Byte b2);
};

struct InvalidInstr : public std::exception
{
    const char* what() const throw()
    {
        return "Invalid instruction";
    }
};
