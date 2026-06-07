#include <bitset>
#include <cassert>
#include <mutex>
#include "ncurses.h"

#include "types.h"
#include "iodevice.h"

// A simple 10-bit shifter (4003) used as a component for the keyboard.

class Shifter4003 : public iodevice
{
    public:
    Shifter4003() :input(0), clock(0), input_was_set(false), clock_was_set(false), bits(0x3FF) {}

    // No input capability
    Bit port_input(int port_id) {return 0;}

    // Shifter input is tricky because of the other input, the clock.
    // From what I can tell:
    // 1) Shifter input is stored but not shifted in until the clock goes from 0 to 1.
    // 2) New bits input will override this stored value (a one-bit buffer)
    // 3) It is not possible to guarantee that the input signal arrives ahead of
    //    the clock signal when both are set simultaneously.
    // 4) The Busicom fixes this by adding a hardware delay so that the input
    //    signal always arrives first. Thus, the Busicom just sends either 01
    //    or 11 to shift in 0 or 1, and then sends 00 to "reset" the bits for
    //    more input (first bit is ignored and could be 1).

    // To handle this in software, we will assume that the two bits are always
    // sent as a pair and simply wait for both before processing. We assert that
    // they alternate and also assert that the clock always changes.
    // We could just ignore the clock and only shift every other input, but the
    // above strategy allows us to check our assumptions.
    // We also assume no race condition where one bit arrives twice before the
    // other even though they are sent as pairs (can't happen in our current
    // implementation).

    // The Busicom shifter has a third input to support cascaded shifters. We
    // will ignore that input for now.
    void port_output(int port_id, Bit val)
    {
        assert((port_id == 0) || (port_id == 1));
        // Clock
        if (port_id == 0)
        {
            // Ensure setting of input and clock alternate
            if (clock_was_set) assert(false);
            // Ensure clock always changes
            if (clock == val) assert(false);
            clock_was_set = true;
            clock = val;
        }
        // Input
        else
        {
            // Ensure setting of input and clock alternate
            if (input_was_set) assert(false);
            input_was_set = true;
            input = val;
        }

        if (input_was_set && clock_was_set)
        {
            if (clock == 1)
            {
                bits <<= 1;
                if (input == 1) bits.set(0);
            }

            input_was_set = false;
            clock_was_set = false;
        }
    }

    bool is_set(int i) const {
        assert(i >= 0 && i <= 9);
        return bits.test(i);
    }

    private:
    Bit input;
    Bit clock;
    Bit input_was_set;
    Bit clock_was_set;
    std::bitset<10> bits;
};
