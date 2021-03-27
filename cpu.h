#include <vector>
#include "rom.h"
#include "ram.h"
#include "types.h"

class CPU4004
{
    public:
    void run();
    void connect(std::shared_ptr<rom_rack>   r) {roms = r;}
    void connect(std::shared_ptr<ram_rack>   r) {rams = r;}
    void connect(std::shared_ptr<testdevice> t) {tdev = t;}

    private:
    // Basic hardware (flags, pins, and registers)
    Bit carry;
    Nibble acc;
    std::vector<Nibble> reg;
    Addr ip;
    std::shared_ptr<rom_rack> roms;
    std::shared_ptr<ram_rack> rams;
    std::shared_ptr<testdevice> tdev;

    // Selected ROM and RAM values (set with the "SRC" command)
    Nibble sel_rom;
    Crumb  sel_ram;
    Crumb  sel_ram_reg;

    // Functions for processing instructions
    std::tuple<Byte,Byte> read_instr();
    void exec_instr(Byte b1, Byte b2);
};

struct InvalidInstr : public std::exception
{
    const char* what() const throw()
    {
        return "Invalid instruction";
    }
};
