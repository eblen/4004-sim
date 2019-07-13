#include "types.h"
#include "iodevice.h"

class SR4003 : public iodevice
{
    Nibble port_input() {return Nibble();}
    void port_output(Nibble i) {}
};
