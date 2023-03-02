#pragma once

#include <array>
#include <memory>
#include <vector>

#include "types.h"
#include "iodevice.h"

enum class IOTYPE {in,out};
class ROM4001 : public iodevice
{
    public:
    void load(const std::array<Byte,256>& d) {data = d;}
    void load(const std::vector<Byte>& d);
    void set_iotype(IOTYPE t) {iotype = t;}

    Byte read(Byte b) {return data[b];}

    void connect(std::shared_ptr<iodevice> d) {device = d;}

    // Just forward input from the connected device
    Nibble port_input()
    {
        if (!device)
            throw std::runtime_error("ROM port not connected");
        if (iotype != IOTYPE::in)
            throw std::runtime_error("ROM port not configured for input");
        return device->port_input();
    }

    // Just forwards output to the connected device
    void port_output(Nibble val)
    {
        if (!device)
            throw std::runtime_error("ROM port not connected");
        if (iotype != IOTYPE::out)
            throw std::runtime_error("ROM port not configured for output");
        device->port_output(val);
    }

    private:
    std::array<Byte,256> data;
    IOTYPE iotype{IOTYPE::in};
    std::shared_ptr<iodevice> device;
};

using rom_rack = std::vector<ROM4001>;
void load_rom_rack(std::shared_ptr<rom_rack> rr, const std::vector<Byte>& bytes);
