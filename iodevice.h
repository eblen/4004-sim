#pragma once

#include "types.h"

class iodevice
{
    public:
    virtual void   port_output(Nibble val) = 0;
    virtual Nibble port_input ()           = 0;
    virtual ~iodevice() {}
};

class testdevice
{
    public:
    virtual Bit test() = 0;
    virtual ~testdevice() {}
};

