#include <vector>

#include "rom.h"
#include "ram.h"
#include "sr.h"
#include "cpu.h"

class Busicom
{
    public:
    Busicom() {
        printf("Busicom initializing...\n");
        rom[0].set_iotype(IOTYPE::out);
        rom[0].connect(std::make_unique<SR4003>());
    }

    private:
    // Last two ROMS do not use IO ports
    std::vector<ROM4001> rom{5};
    std::vector<RAM4002> ram{2};
    CPU4004 cpu;
};
