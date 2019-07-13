#include <array>

#include "types.h"
#include "iodevice.h"

enum class IOTYPE {in,out};
class ROM4001 : public iodevice
{
    public:
    void load(std::array<Byte,256> &&d) {data = std::move(d);}
    void set_iotype(IOTYPE t) {iotype = t;}
    // This chip takes ownership of the connected device
    void connect(std::unique_ptr<iodevice> &&d) {ioport = std::move(d);}

    // ROM just forwards IO to/from the connected device
    Nibble port_input()
    {
        if (!ioport)
            throw std::runtime_error("ROM port not connected");
        if (iotype != IOTYPE::in)
            throw std::runtime_error("ROM port not configured for input");
        return ioport->port_input();
    }
    void port_output(Nibble out)
    {
        if (!ioport)
            throw std::runtime_error("ROM port not connected");
        if (iotype != IOTYPE::out)
            throw std::runtime_error("ROM port not configured for output");
        ioport->port_output(out);
    }

    private:
    std::array<Byte,256> data;
    IOTYPE iotype{IOTYPE::in};
    std::unique_ptr<iodevice> ioport;
};
