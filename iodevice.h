#pragma once

#include "types.h"

// Interface for external IO devices, like keyboards, printers, etc.
class iodevice
{
    public:
    virtual Bit  port_input (int port_id)          = 0;
    virtual void port_output(int port_id, Bit val) = 0;
    virtual ~iodevice() {}
};

// Interface for external devices with a test input
class testdevice
{
    public:
    virtual Bit test() = 0;
    virtual ~testdevice() {}
};

// Represents an IO connection between a device and either a RAM or ROM port
enum class IOTYPE {in,out};
struct io_conn
{
    bool is_connected;
    IOTYPE iotype;
    std::shared_ptr<iodevice> device;
    int port;

    io_conn() :is_connected(false) {}
    void connect(IOTYPE t, std::shared_ptr<iodevice> d, int p)
    {
        is_connected = true;
        iotype       = t;
        device       = d;
        port         = p;
    }
};
