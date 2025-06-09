#pragma once

#include <array>
#include <memory>
#include <vector>

#include "types.h"
#include "iodevice.h"

class ROM4001
{
    public:
    void load(const std::array<Byte,256>& d) {data = d;}
    void load(const std::vector<Byte>& d);

    Byte read(Byte b) {return data[b];}

    // Shortcut to connect ports 0-3 to the same ports on the device
    void connect_all(IOTYPE t, std::shared_ptr<iodevice> d)
    {
        for (int i=0; i<4; i++) connect(t, d, i, i);
    }

    void connect(IOTYPE t, std::shared_ptr<iodevice> d, int rom_port_id, int device_port_id)
    {
        assert((rom_port_id >= 0) && (rom_port_id <= 3));
        io_conn &ioc = conns[rom_port_id];
        if (ioc.is_connected)
            throw std::runtime_error("ROM port already connected");
        ioc.connect(t, d, device_port_id);
    }

    // Gather bits into a single nibble
    Nibble port_input()
    {
        Nibble n = 0;
        Bit ignore;

        for (int i=0; i<4; i++)
        {
            n = RAR(n, port_input_bit(i), ignore);
        }
        return n;
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
    // Just forward input from the connected device
    // TODO: Currently, non-connected or non-input ports return 0, but the
    // manual says the ROM can be configured to return 1 instead.
    Bit port_input_bit(int port_id)
    {
        assert((port_id >= 0) && (port_id <= 3));
        io_conn &ioc = conns[port_id];

        if (!ioc.is_connected || ioc.iotype != IOTYPE::in)
        {
            return 0;
        }

        else
        {
            return ioc.device->port_input(port_id);
        }
    }

    // Just forwards output to the connected device
    void port_output(int port_id, Bit val)
    {
        assert((port_id >= 0) && (port_id <= 3));
        io_conn &ioc = conns[port_id];
        if (!ioc.is_connected)
            throw std::runtime_error("ROM port not connected");
        if (ioc.iotype != IOTYPE::out)
            throw std::runtime_error("ROM port not configured for output");
        ioc.device->port_output(port_id, val);
    }

    std::array<Byte,256> data;
    io_conn conns[4];
};

using rom_rack = std::vector<ROM4001>;
void load_rom_rack(std::shared_ptr<rom_rack> rr, const std::vector<Byte>& bytes);
