#include <array>

#include "types.h"
#include "iodevice.h"

class RAM4002 : public iodevice
{
    public:
    // This chip takes ownership of the connected device
    void connect(std::unique_ptr<iodevice> &&d) {ioport = std::move(d);}
    // ROM just forwards output to the connected device
    Nibble port_input()
    {
        throw std::runtime_error("RAM port cannot be used for input");
    }
    void port_output(Nibble out)
    {
        if (!ioport)
            throw std::runtime_error("ROM port not connected");
        ioport->port_output(out);
    }

    private:
    // 4 registers, each with 20 4-bit values
    // The 20 values are divided into 16 memory cells and 4 status characters
    std::array<std::array<Nibble,20>, 4> data;
    std::unique_ptr<iodevice> ioport;
};
