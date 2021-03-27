#pragma once

#include <array>

#include "types.h"
#include "iodevice.h"

class RAM4002 : public iodevice
{
    constexpr static int num_ports = 4;

    public:
    Nibble read_mem  (Crumb reg, Nibble loc)             {return data[reg][loc];}
    Nibble read_char (Crumb reg, Crumb  loc)             {return data[reg][loc+16];}
    void   write_mem (Crumb reg, Nibble loc, Nibble val) {data[reg][loc]    = val;}
    void   write_char(Crumb reg, Crumb  loc, Nibble val) {data[reg][loc+16] = val;}

    void connect(std::shared_ptr<iodevice> d) {device = d;}

    Nibble port_input()
    {
        throw std::runtime_error("RAM port cannot be used for input");
    }

    // Just forward output to the connected device
    void port_output(Nibble val)
    {
        if (!device)
            throw std::runtime_error("RAM port not connected");
        device->port_output(val);
    }

    private:
    // 4 registers, each with 20 4-bit values
    // The 20 values are divided into 16 memory cells and 4 status characters
    std::array<std::array<Nibble,20>, 4> data;
    std::shared_ptr<iodevice> device;
};

using ram_rack = std::vector<RAM4002>;

