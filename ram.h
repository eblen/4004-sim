#pragma once

#include <array>

#include "types.h"
#include "iodevice.h"

class RAM4002
{
    constexpr static int num_ports = 4;

    public:
    Nibble read_mmc  (Crumb reg, Nibble loc)             {return data[reg][loc];}
    Nibble read_stat (Crumb reg, Crumb  loc)             {return data[reg][loc+16];}
    void   write_mmc (Crumb reg, Nibble loc, Nibble val) {data[reg][loc]    = val;}
    void   write_stat(Crumb reg, Crumb  loc, Nibble val) {data[reg][loc+16] = val;}

    // Shortcut to connect ports 0-3 to the same ports on the device
    void connect_all(std::shared_ptr<iodevice> d)
    {
        for (int i=0; i<4; i++) connect(d, i, i);
    }

    void connect(std::shared_ptr<iodevice> d, int ram_port_id, int device_port_id)
    {
        assert((ram_port_id >= 0) && (ram_port_id <= 3));
        io_conn &ioc = conns[ram_port_id];
        if (ioc.is_connected)
            throw std::runtime_error("RAM port already connected");
        ioc.connect(IOTYPE::out, d, device_port_id);
    }

    Nibble port_input()
    {
        throw std::runtime_error("RAM port cannot be used for input");
    }

    // Split nibble output into individual bits
    void port_output(Nibble n)
    {
        for (int i=0; i<4; i++)
        {
            port_output(i, get_bit(n, i));
        }
    }

    private:
    // Just forward output to the connected device
    void port_output(int port_id, Bit val)
    {
        assert((port_id >= 0) && (port_id <= 3));
        io_conn &ioc = conns[port_id];
        if (!ioc.is_connected)
            throw std::runtime_error("RAM port not connected");
        ioc.device->port_output(ioc.port, val);
    }

    // 4 registers, each with 20 4-bit values
    // The 20 values are divided into 16 memory cells and 4 status characters
    std::array<std::array<Nibble,20>, 4> data;
    io_conn conns[4];
};

using ram_rack = std::vector<RAM4002>;

